/* MACSW structured P2P notifications. */

#include "utils/includes.h"

#include <limits.h>
#include <stdint.h>

#include "utils/common.h"
#include "config.h"
#include "wpa_supplicant_i.h"
#include "notify.h"
#include "p2p_notify_macsw.h"
#include "wps/wps_defs.h"
#include "cfgmacsw.h"
#include "drivers/driver_macsw_priv.h"

#define CFGMACSW_P2P_GO_INTENT_MAX 15U
#define CFGMACSW_P2P_PIN_MAX       99999999U

void mm_sec_keydump(void);

static void wpas_macsw_p2p_keyram_dump(struct wpa_supplicant *wpa_s,
				       const char *role,
				       const char *trigger,
				       const u8 *peer_addr)
{
	if (peer_addr) {
		wpa_msg(wpa_s, MSG_INFO,
			"P2P: Keyram dump role=%s trigger=%s peer=" MACSTR,
			role, trigger, MAC2STR(peer_addr));
	} else {
		wpa_msg(wpa_s, MSG_INFO,
			"P2P: Keyram dump role=%s trigger=%s", role, trigger);
	}
	mm_sec_keydump();
}

static int wpas_macsw_p2p_freq_valid(int freq)
{
	return freq > 0 && (unsigned int)freq <= UINT16_MAX;
}

static int wpas_macsw_p2p_persistent_id_valid(int id, int id_valid)
{
	return id_valid && id >= 0 && id <= INT32_MAX;
}

static struct cfgmacsw_p2p_event *
wpas_macsw_p2p_event_alloc(struct wpa_supplicant *wpa_s,
				   enum cfgmacsw_p2p_event_type type)
{
	struct wpa_macsw_driver_itf_data *drv;
	struct cfgmacsw_p2p_event *event;

	if (!wpa_s)
		return NULL;

	drv = wpa_s->drv_priv;
	if (!drv || !drv->gdrv || drv->fhost_vif_idx < 0 ||
	    drv->fhost_vif_idx >= MACSW_VIRT_DEV_MAX)
		return NULL;

	event = rtos_calloc(1, sizeof(*event));
	if (!event)
		return NULL;

	event->hdr.resp_queue = drv->gdrv->resp_queue;
	event->hdr.len = sizeof(*event);
	event->hdr.id = CFGMACSW_P2P_EVENT;
	event->fhost_vif_idx = drv->fhost_vif_idx;
	event->type = type;
	return event;
}

static void wpas_macsw_p2p_event_send(struct cfgmacsw_p2p_event *event)
{
	if (!event)
		return;

	if (fhost_cntrl_cfgmacsw_cmd_send(&event->hdr, NULL))
		rtos_free(event);
}

static void wpas_macsw_p2p_set_addr(struct cfgmacsw_p2p_event *event,
				    uint32_t valid, u8 *dst,
				    const u8 *addr)
{
	if (!addr)
		return;

	os_memcpy(dst, addr, CFGMACSW_P2P_ADDR_LEN);
	event->valid_fields |= valid;
}

static void wpas_macsw_p2p_set_string(char *dst, size_t dst_len,
				      const char *src)
{
	if (src)
		os_strlcpy(dst, src, dst_len);
}

static void wpas_macsw_p2p_set_role(struct cfgmacsw_p2p_event *event,
				    int client)
{
	event->role = client ? CFGMACSW_P2P_ROLE_CLIENT : CFGMACSW_P2P_ROLE_GO;
	event->valid_fields |= CFGMACSW_P2P_VALID_ROLE;
}

static uint8_t wpas_macsw_p2p_wps_method(enum p2p_wps_method method)
{
	switch (method) {
	case WPS_PIN_DISPLAY:
		return CFGMACSW_P2P_WPS_METHOD_PIN_DISPLAY;
	case WPS_PIN_KEYPAD:
		return CFGMACSW_P2P_WPS_METHOD_PIN_KEYPAD;
	case WPS_PBC:
		return CFGMACSW_P2P_WPS_METHOD_PBC;
	case WPS_NFC:
		return CFGMACSW_P2P_WPS_METHOD_NFC;
	case WPS_P2PS:
		return CFGMACSW_P2P_WPS_METHOD_P2PS;
	default:
		return CFGMACSW_P2P_WPS_METHOD_UNKNOWN;
	}
}

void wpas_macsw_p2p_find_stopped(struct wpa_supplicant *wpa_s)
{
	wpas_macsw_p2p_event_send(wpas_macsw_p2p_event_alloc(
		wpa_s, CFGMACSW_P2P_FIND_STOPPED));
}

void wpas_macsw_p2p_listen_stopped(struct wpa_supplicant *wpa_s)
{
	wpas_macsw_p2p_event_send(wpas_macsw_p2p_event_alloc(
		wpa_s, CFGMACSW_P2P_LISTEN_STOPPED));
}

static void wpas_macsw_p2p_device_found_fill(
	struct cfgmacsw_p2p_event *event, const u8 *dev_addr,
	const u8 *iface_addr, const struct p2p_peer_info *info)
{
	wpas_macsw_p2p_set_addr(event, CFGMACSW_P2P_VALID_PEER_ADDR,
				  event->peer_addr, dev_addr);
	wpas_macsw_p2p_set_addr(event,
				CFGMACSW_P2P_VALID_PEER_IFACE_ADDR,
				event->peer_iface_addr, iface_addr);
	if (info) {
		wpas_macsw_p2p_set_string(event->device_name,
					  sizeof(event->device_name),
					  info->device_name);
		event->config_methods = info->config_methods;
		event->dev_capab = info->dev_capab;
		event->group_capab = info->group_capab;
		event->valid_fields |= CFGMACSW_P2P_VALID_CONFIG_METHODS |
			CFGMACSW_P2P_VALID_DEV_CAPAB |
			CFGMACSW_P2P_VALID_GROUP_CAPAB;
		if (info->level >= INT8_MIN && info->level <= INT8_MAX) {
			event->level = info->level;
			event->valid_fields |= CFGMACSW_P2P_VALID_LEVEL;
		}
	}
}

void wpas_macsw_p2p_device_found(struct wpa_supplicant *wpa_s,
				 const u8 *dev_addr,
				 const u8 *iface_addr,
				 const struct p2p_peer_info *info)
{
	struct cfgmacsw_p2p_event *event = wpas_macsw_p2p_event_alloc(
		wpa_s, CFGMACSW_P2P_DEVICE_FOUND);

	if (!event)
		return;
	wpas_macsw_p2p_device_found_fill(event, dev_addr, iface_addr, info);
	wpas_macsw_p2p_event_send(event);
}

void wpas_macsw_p2p_device_lost(struct wpa_supplicant *wpa_s,
				const u8 *dev_addr)
{
	struct cfgmacsw_p2p_event *event = wpas_macsw_p2p_event_alloc(
		wpa_s, CFGMACSW_P2P_DEVICE_LOST);

	if (!event)
		return;
	wpas_macsw_p2p_set_addr(event, CFGMACSW_P2P_VALID_PEER_ADDR,
				  event->peer_addr, dev_addr);
	wpas_macsw_p2p_event_send(event);
}

void wpas_macsw_p2p_group_removed(struct wpa_supplicant *wpa_s,
				  const char *role, const char *reason)
{
	struct cfgmacsw_p2p_event *event = wpas_macsw_p2p_event_alloc(
		wpa_s, CFGMACSW_P2P_GROUP_REMOVED);

	if (!event)
		return;
	wpas_macsw_p2p_set_string(event->ifname, sizeof(event->ifname),
				  wpa_s->ifname);
	if (reason && os_strncmp(reason, " reason=", 8) == 0)
		reason += 8;
	wpas_macsw_p2p_set_string(event->reason, sizeof(event->reason), reason);
	if (role && os_strcmp(role, "GO") == 0)
		wpas_macsw_p2p_set_role(event, 0);
	else if (role && os_strcmp(role, "client") == 0)
		wpas_macsw_p2p_set_role(event, 1);
	wpas_macsw_p2p_event_send(event);
}

void wpas_macsw_p2p_go_neg_req(struct wpa_supplicant *wpa_s,
			       const u8 *src, u16 dev_passwd_id,
			       u8 go_intent)
{
	struct cfgmacsw_p2p_event *event = wpas_macsw_p2p_event_alloc(
		wpa_s, CFGMACSW_P2P_GO_NEG_REQUEST);

	if (!event)
		return;
	wpas_macsw_p2p_set_addr(event, CFGMACSW_P2P_VALID_PEER_ADDR,
				  event->peer_addr, src);
	event->dev_passwd_id = dev_passwd_id;
	event->valid_fields |= CFGMACSW_P2P_VALID_DEV_PASSWD_ID;
	if (go_intent <= CFGMACSW_P2P_GO_INTENT_MAX) {
		event->go_intent = go_intent;
		event->valid_fields |= CFGMACSW_P2P_VALID_GO_INTENT;
	}
	wpas_macsw_p2p_event_send(event);
}

void wpas_macsw_p2p_go_neg_completed(struct wpa_supplicant *wpa_s,
				     const struct p2p_go_neg_results *res)
{
	struct cfgmacsw_p2p_event *event;

	if (!res)
		return;
	event = wpas_macsw_p2p_event_alloc(
		wpa_s, res->status ? CFGMACSW_P2P_GO_NEG_FAILURE :
		CFGMACSW_P2P_GO_NEG_SUCCESS);
	if (!event)
		return;

	if (res->status) {
		event->status = res->status;
		event->valid_fields |= CFGMACSW_P2P_VALID_STATUS;
	} else {
		wpas_macsw_p2p_set_role(event, !res->role_go);
		if (wpas_macsw_p2p_freq_valid(res->freq)) {
			event->freq = res->freq;
			event->valid_fields |= CFGMACSW_P2P_VALID_FREQ;
		}
		wpas_macsw_p2p_set_addr(event, CFGMACSW_P2P_VALID_PEER_ADDR,
					  event->peer_addr,
					  res->peer_device_addr);
		wpas_macsw_p2p_set_addr(event,
					  CFGMACSW_P2P_VALID_PEER_IFACE_ADDR,
					  event->peer_iface_addr,
					  res->peer_interface_addr);
		event->wps_method = wpas_macsw_p2p_wps_method(res->wps_method);
		if (event->wps_method != CFGMACSW_P2P_WPS_METHOD_UNKNOWN)
			event->valid_fields |= CFGMACSW_P2P_VALID_WPS_METHOD;
	}
	wpas_macsw_p2p_event_send(event);
}

static int wpas_macsw_p2p_provision_discovery_fill(
	struct cfgmacsw_p2p_event *event, const u8 *dev_addr, int request,
	enum p2p_prov_disc_status status, int status_valid, u16 config_methods,
	unsigned int generated_pin,
	const struct wpas_p2p_prov_disc_info *info)
{
	enum cfgmacsw_p2p_event_type type;

	if (!status_valid || status != P2P_PROV_DISC_SUCCESS)
		type = CFGMACSW_P2P_PROV_DISC_FAILURE;
	else if (request && (config_methods & WPS_CONFIG_DISPLAY))
		type = CFGMACSW_P2P_PROV_DISC_SHOW_PIN;
	else if (request && (config_methods & WPS_CONFIG_KEYPAD))
		type = CFGMACSW_P2P_PROV_DISC_ENTER_PIN;
	else if (request && (config_methods & WPS_CONFIG_PUSHBUTTON))
		type = CFGMACSW_P2P_PROV_DISC_PBC_REQ;
	else if (!request && (config_methods & WPS_CONFIG_DISPLAY))
		type = CFGMACSW_P2P_PROV_DISC_ENTER_PIN;
	else if (!request && (config_methods & WPS_CONFIG_KEYPAD))
		type = CFGMACSW_P2P_PROV_DISC_SHOW_PIN;
	else if (!request && (config_methods & WPS_CONFIG_PUSHBUTTON))
		type = CFGMACSW_P2P_PROV_DISC_PBC_RESP;
	else
		return -1;

	event->type = type;
	wpas_macsw_p2p_set_addr(event, CFGMACSW_P2P_VALID_PEER_ADDR,
				  event->peer_addr,
				  info && info->p2p_device_addr ?
				  info->p2p_device_addr : dev_addr);
	if (status_valid && status != P2P_PROV_DISC_SUCCESS) {
		event->status = status;
		event->valid_fields |= CFGMACSW_P2P_VALID_STATUS;
	} else if (status_valid) {
		event->config_methods = info ? info->config_methods : config_methods;
		event->valid_fields |= CFGMACSW_P2P_VALID_CONFIG_METHODS;
	}
	if (info) {
		wpas_macsw_p2p_set_string(event->device_name,
					  sizeof(event->device_name),
					  info->device_name);
		wpas_macsw_p2p_set_string(event->ifname, sizeof(event->ifname),
					  info->group_ifname);
		event->dev_capab = info->dev_capab;
		event->group_capab = info->group_capab;
		event->valid_fields |= CFGMACSW_P2P_VALID_DEV_CAPAB |
			CFGMACSW_P2P_VALID_GROUP_CAPAB;
	}
	if (generated_pin <= CFGMACSW_P2P_PIN_MAX &&
	    type == CFGMACSW_P2P_PROV_DISC_SHOW_PIN)
		os_snprintf(event->pin, sizeof(event->pin), "%08u", generated_pin);
	return 0;
}

static void wpas_macsw_p2p_provision_discovery_send(
	struct wpa_supplicant *wpa_s, const u8 *dev_addr, int request,
	enum p2p_prov_disc_status status, int status_valid, u16 config_methods,
	unsigned int generated_pin,
	const struct wpas_p2p_prov_disc_info *info)
{
	struct cfgmacsw_p2p_event *event = wpas_macsw_p2p_event_alloc(
		wpa_s, CFGMACSW_P2P_PROV_DISC_FAILURE);

	if (!event)
		return;
	if (wpas_macsw_p2p_provision_discovery_fill(
		    event, dev_addr, request, status, status_valid,
		    config_methods, generated_pin, info)) {
		rtos_free(event);
		return;
	}
	wpas_macsw_p2p_event_send(event);
}

void wpas_macsw_p2p_provision_discovery(
	struct wpa_supplicant *wpa_s, const u8 *dev_addr, int request,
	enum p2p_prov_disc_status status, u16 config_methods,
	unsigned int generated_pin,
	const struct wpas_p2p_prov_disc_info *info)
{
	wpas_macsw_p2p_provision_discovery_send(
		wpa_s, dev_addr, request, status, 1, config_methods,
		generated_pin, info);
}

void wpas_macsw_p2p_provision_discovery_failure(
	struct wpa_supplicant *wpa_s, const u8 *dev_addr)
{
	wpas_macsw_p2p_provision_discovery_send(
		wpa_s, dev_addr, 0, P2P_PROV_DISC_SUCCESS, 0, 0, 0, NULL);
}

void wpas_macsw_p2p_group_started(struct wpa_supplicant *wpa_s,
				  const struct wpa_ssid *ssid, int persistent,
				  int client, int freq, const u8 *go_dev_addr)
{
	struct cfgmacsw_p2p_event *event = wpas_macsw_p2p_event_alloc(
		wpa_s, CFGMACSW_P2P_GROUP_STARTED);

	if (client)
		wpas_macsw_p2p_keyram_dump(wpa_s, "GC", "group-started",
					      go_dev_addr);

	if (!event)
		return;
	wpas_macsw_p2p_set_string(event->ifname, sizeof(event->ifname),
				  wpa_s->ifname);
	wpas_macsw_p2p_set_role(event, client);
	event->persistent = !!persistent;
	event->valid_fields |= CFGMACSW_P2P_VALID_PERSISTENT;
	if (wpas_macsw_p2p_freq_valid(freq)) {
		event->freq = freq;
		event->valid_fields |= CFGMACSW_P2P_VALID_FREQ;
	}
	wpas_macsw_p2p_set_addr(event, CFGMACSW_P2P_VALID_GO_DEV_ADDR,
				  event->go_dev_addr, go_dev_addr);
	if (ssid) {
		event->ssid_len = ssid->ssid_len > sizeof(event->ssid) ?
			sizeof(event->ssid) : ssid->ssid_len;
		os_memcpy(event->ssid, ssid->ssid, event->ssid_len);
	}
	wpas_macsw_p2p_event_send(event);
}

void wpas_macsw_p2p_go_sta_authorized(struct wpa_supplicant *wpa_s,
				      const u8 *sta_addr)
{
	if (!wpa_s || !wpa_s->current_ssid)
		return;

	if (wpa_s->current_ssid->mode != WPAS_MODE_P2P_GO &&
	    wpa_s->current_ssid->mode != WPAS_MODE_P2P_GROUP_FORMATION)
		return;

	wpas_macsw_p2p_keyram_dump(wpa_s, "GO", "sta-authorized", sta_addr);
}

void wpas_macsw_p2p_group_formation_success(struct wpa_supplicant *wpa_s)
{
	wpas_macsw_p2p_event_send(wpas_macsw_p2p_event_alloc(
		wpa_s, CFGMACSW_P2P_GROUP_FORMATION_SUCCESS));
}

void wpas_macsw_p2p_group_formation_failure(struct wpa_supplicant *wpa_s,
					    const char *reason)
{
	struct cfgmacsw_p2p_event *event = wpas_macsw_p2p_event_alloc(
		wpa_s, CFGMACSW_P2P_GROUP_FORMATION_FAILURE);

	if (!event)
		return;
	wpas_macsw_p2p_set_string(event->reason, sizeof(event->reason), reason);
	wpas_macsw_p2p_event_send(event);
}

static void wpas_macsw_p2p_invitation(
	struct wpa_supplicant *wpa_s, enum cfgmacsw_p2p_event_type type,
	const u8 *sa, const u8 *go_dev_addr, const u8 *bssid, int id,
	int id_valid, int op_freq)
{
	struct cfgmacsw_p2p_event *event = wpas_macsw_p2p_event_alloc(wpa_s,
								      type);

	if (!event)
		return;
	wpas_macsw_p2p_set_addr(event, CFGMACSW_P2P_VALID_PEER_ADDR,
				  event->peer_addr, sa);
	wpas_macsw_p2p_set_addr(event, CFGMACSW_P2P_VALID_GO_DEV_ADDR,
				  event->go_dev_addr, go_dev_addr);
	wpas_macsw_p2p_set_addr(event, CFGMACSW_P2P_VALID_BSSID,
				  event->bssid, bssid);
	if (wpas_macsw_p2p_persistent_id_valid(id, id_valid)) {
		event->persistent_id = id;
		event->valid_fields |= CFGMACSW_P2P_VALID_PERSISTENT_ID;
	}
	if (wpas_macsw_p2p_freq_valid(op_freq)) {
		event->freq = op_freq;
		event->valid_fields |= CFGMACSW_P2P_VALID_FREQ;
	}
	wpas_macsw_p2p_event_send(event);
}

void wpas_macsw_p2p_invitation_received(struct wpa_supplicant *wpa_s,
					const u8 *sa,
					const u8 *go_dev_addr,
					const u8 *bssid, int id,
					int id_valid, int op_freq)
{
	wpas_macsw_p2p_invitation(wpa_s, CFGMACSW_P2P_INVITATION_RECEIVED,
				   sa, go_dev_addr, bssid, id, id_valid, op_freq);
}

void wpas_macsw_p2p_invitation_accepted(struct wpa_supplicant *wpa_s,
					const u8 *sa,
					const u8 *go_dev_addr,
					const u8 *bssid, int id,
					int id_valid, int op_freq)
{
	wpas_macsw_p2p_invitation(wpa_s, CFGMACSW_P2P_INVITATION_ACCEPTED,
				   sa, go_dev_addr, bssid, id, id_valid, op_freq);
}

void wpas_macsw_p2p_invitation_result(struct wpa_supplicant *wpa_s,
				      int status, const u8 *bssid)
{
	struct cfgmacsw_p2p_event *event = wpas_macsw_p2p_event_alloc(
		wpa_s, CFGMACSW_P2P_INVITATION_RESULT);

	if (!event)
		return;
	event->status = status;
	event->valid_fields |= CFGMACSW_P2P_VALID_STATUS;
	wpas_macsw_p2p_set_addr(event, CFGMACSW_P2P_VALID_BSSID,
				  event->bssid, bssid);
	wpas_macsw_p2p_event_send(event);
}
