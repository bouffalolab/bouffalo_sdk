/*
 * WPA Supplicant / UDP socket -based control interface
 * Copyright (c) 2004-2016, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include <assert.h>
#include "common.h"
#include "eloop.h"
#include "config.h"
#include "wpa_supplicant_i.h"
#include "ctrl_iface.h"
#include "common/wpa_ctrl.h"
#include "ap/hostapd.h"
#include "ap/sta_info.h"
#include "ap/wpa_auth.h"
#include "cfgmacsw.h"
#include "fhost_api.h"
#include "eloop_rtos.h"
#include "drivers/driver_macsw_priv.h"

static int wpa_macsw_forward_status_msg(const char *txt)
{
    return !strncmp(txt, "State###", sizeof("State###") - 1) ||
           !strncmp(txt, "CTRL-EVENT-NETWORK-NOT-FOUND", sizeof("CTRL-EVENT-NETWORK-NOT-FOUND") - 1) ||
           !strncmp(txt, "AP-STA-ENCRYPTION-TYPE-MISMATCH", sizeof("AP-STA-ENCRYPTION-TYPE-MISMATCH") - 1) ||
           !strncmp(txt, "WPS-SUCCESS", sizeof("WPS-SUCCESS") - 1) ||
           !strncmp(txt, "STA-RX-EAPOL1", sizeof("STA-RX-EAPOL1") - 1) ||
           !strncmp(txt, "STA-TX-EAPOL2", sizeof("STA-TX-EAPOL2") - 1) ||
           !strncmp(txt, "STA-RX-EAPOL3", sizeof("STA-RX-EAPOL3") - 1) ||
           !strncmp(txt, "STA-TX-EAPOL4", sizeof("STA-TX-EAPOL4") - 1) ||
           !strncmp(txt, "SAE-COMMITTED", sizeof("SAE-COMMITTED") - 1) ||
	   !strncmp(txt, "SAE-CONFIRMED", sizeof("SAE-CONFIRMED") - 1) ||
	   !strncmp(txt, "SAE-ACCEPTED", sizeof("SAE-ACCEPTED") - 1) ||
	   !strncmp(txt, "CTRL-EVENT-CONNECTED",
		    sizeof("CTRL-EVENT-CONNECTED") - 1);
}

static int wpa_macsw_status_msg_is_sensitive(const char *txt)
{
    return !strncmp(txt, "P2P-GROUP-STARTED",
                    sizeof("P2P-GROUP-STARTED") - 1) ||
           !strncmp(txt, "P2P-PROV-DISC-SHOW-PIN",
                    sizeof("P2P-PROV-DISC-SHOW-PIN") - 1);
}

static void wpa_macsw_msg_hdr_init(struct wpa_macsw_driver_itf_data *drv,
                  struct cfgmacsw_msg_hdr *msg_hdr,
                  uint16_t id, uint16_t len)
{
    msg_hdr->len        = len;
    msg_hdr->id         = id;
    msg_hdr->resp_queue = drv->gdrv->resp_queue;
}


#ifndef CONFIG_NO_WPA_MSG
static int wpa_macsw_ap_sta_auth_fail_send(
	struct wpa_supplicant *wpa_s, struct wpa_macsw_driver_itf_data *drv,
	const char *txt, size_t len)
{
	const char *mac_txt;
	size_t prefix_len;
	u8 sta_mac[ETH_ALEN];
	u8 auth_mode;
	u8 reason;

	if (len >= strlen(AP_STA_POSSIBLE_PSK_MISMATCH) &&
	    !strncmp(txt, AP_STA_POSSIBLE_PSK_MISMATCH,
		     strlen(AP_STA_POSSIBLE_PSK_MISMATCH))) {
		prefix_len = strlen(AP_STA_POSSIBLE_PSK_MISMATCH);
		reason = CFGMACSW_AP_STA_AUTH_FAIL_PSK_MISMATCH;
	} else if (len >= strlen(AP_STA_POSSIBLE_SAE_CREDENTIAL_MISMATCH) &&
		   !strncmp(txt, AP_STA_POSSIBLE_SAE_CREDENTIAL_MISMATCH,
			    strlen(AP_STA_POSSIBLE_SAE_CREDENTIAL_MISMATCH))) {
		prefix_len = strlen(AP_STA_POSSIBLE_SAE_CREDENTIAL_MISMATCH);
		auth_mode = CFGMACSW_AP_STA_AUTH_WPA3_SAE;
		reason = CFGMACSW_AP_STA_AUTH_FAIL_SAE_CONFIRM_MISMATCH;
	} else {
		return 0;
	}

	/* Require exactly one canonical xx:xx:xx:xx:xx:xx address. */
	if (len != prefix_len + 17)
		return -1;
	mac_txt = txt + prefix_len;
	if (hwaddr_aton(mac_txt, sta_mac))
		return -1;

	if (reason == CFGMACSW_AP_STA_AUTH_FAIL_PSK_MISMATCH) {
		struct sta_info *sta;

		if (!wpa_s->ap_iface || !wpa_s->ap_iface->num_bss ||
		    !wpa_s->ap_iface->bss || !wpa_s->ap_iface->bss[0])
			return -1;
		sta = ap_get_sta(wpa_s->ap_iface->bss[0], sta_mac);
		if (!sta)
			return -1;

		/* Use the negotiated version, not the AP's allowed protocols. */
		switch (wpa_auth_sta_wpa_version(sta->wpa_sm)) {
		case 1: /* WPA */
			auth_mode = CFGMACSW_AP_STA_AUTH_WPA_PSK;
			break;
		case 2: /* WPA2/RSN */
			auth_mode = CFGMACSW_AP_STA_AUTH_WPA2_PSK;
			break;
		default:
			return -1;
		}
	}

	if (fhost_cntrl_cfgmacsw_ap_sta_auth_fail_send(drv->fhost_vif_idx,
						    sta_mac, auth_mode,
						    reason))
		return -1;

	return 1;
}

static void wpa_supplicant_ctrl_iface_msg_cb(void *ctx, int level,
					     enum wpa_msg_type type,
					     const char *txt, size_t len)
{
    struct cfgmacsw_status_code_print *cmd;
    size_t copy_len;
    struct wpa_supplicant *wpa_s = ctx;
    struct wpa_macsw_driver_itf_data *drv;

    if (!wpa_s)
        return;
    drv = wpa_s->drv_priv;

    if (level >= wpa_debug_level) {
        if (wpa_macsw_status_msg_is_sensitive(txt))
            dbg(D_CRT "[WPA] sensitive control event redacted\n");
        else
            dbg(D_CRT "[WPA] %s\n", txt);
    }

    if (!drv || drv->fhost_vif_idx < 0 ||
        drv->fhost_vif_idx >= MACSW_VIRT_DEV_MAX) {
        return;
    }

    if (drv->vif_init_type == VIF_AP &&
        wpa_macsw_ap_sta_auth_fail_send(wpa_s, drv, txt, len) != 0)
        return;

    if (wpa_macsw_forward_status_msg(txt)) {

        cmd = rtos_calloc(1, sizeof(struct cfgmacsw_status_code_print));
        if (cmd == NULL) {
          abort();
        }

        wpa_macsw_msg_hdr_init(drv, &cmd->hdr, CFGMACSW_STATUS_CODE_PRINT_CMD, sizeof(*cmd));

        cmd->fhost_vif_idx = drv->fhost_vif_idx;
        copy_len = len;
        if (copy_len >= sizeof(cmd->msg))
            copy_len = sizeof(cmd->msg) - 1;
        memcpy(cmd->msg, txt, copy_len);
        cmd->msg[copy_len] = '\0';

        if (fhost_cntrl_cfgmacsw_cmd_send(&cmd->hdr, NULL)) {
            printf("%s: status event send failed\r\n", __func__);
            rtos_free(cmd);
        } else {
            printf("%s: status event sent\r\n", __func__);
        }
    }
}
#endif


struct ctrl_iface_priv {
};

struct ctrl_iface_global_priv {
};

/* Per interface ctrl interface not used */
struct ctrl_iface_priv *
wpa_supplicant_ctrl_iface_init(struct wpa_supplicant *wpa_s)
{
	// cannot return NULL
	return (struct ctrl_iface_priv *)1;
}

void wpa_supplicant_ctrl_iface_deinit(struct wpa_supplicant *wpa_s,
				      struct ctrl_iface_priv *priv)
{
}

void wpa_supplicant_ctrl_iface_wait(struct ctrl_iface_priv *priv)
{
}

static void wpa_supplicant_global_ctrl_iface_receive(int sock, void *eloop_ctx,
					void *sock_ctx)
{
  struct wpa_global *global = eloop_ctx;
  char *reply = NULL;
  size_t reply_len;

  struct event_sync_msg *req_body;

  eloop_get_request_body(((void **)&req_body));

  reply = wpa_supplicant_global_ctrl_iface_process(global, (char *)(req_body + 1), &reply_len);

  if (reply) {
    *req_body->resp = reply;
    *req_body->resp_len = reply_len;
  } else if (reply_len == 1) {
    *req_body->resp = strdup("FAIL\n");
    *req_body->resp_len = 5;
  } else if (reply_len == 2) {
    *req_body->resp = strdup("OK\n");
    *req_body->resp_len = 3;
  }
  assert(*req_body->resp != NULL);
}

struct ctrl_iface_global_priv *
wpa_supplicant_global_ctrl_iface_init(struct wpa_global *global)
{
  int ret;
  ret = eloop_register_read_sock(ELOOP_EVT_WPA_IFACE_CTRL,
        wpa_supplicant_global_ctrl_iface_receive,
        global, NULL);

  if (ret) {
    return NULL;
  }

  wpa_msg_register_cb(wpa_supplicant_ctrl_iface_msg_cb);
  // cannot return null
  return (struct ctrl_iface_global_priv *)1;
}


void
wpa_supplicant_global_ctrl_iface_deinit(struct ctrl_iface_global_priv *priv)
{
  assert((uintptr_t)priv == 1);
  eloop_unregister_read_sock(ELOOP_EVT_WPA_IFACE_CTRL);
}
