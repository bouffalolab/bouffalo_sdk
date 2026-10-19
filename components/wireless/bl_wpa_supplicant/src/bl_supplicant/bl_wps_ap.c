#include "bl_wps.h"

#include <string.h>

#include "utils/includes.h"
#include "utils/common.h"
#include "utils/uuid.h"
#include "common/eapol_common.h"
#include "common/ieee802_11_defs.h"
#include "crypto/dh_group5.h"
#include "eap_peer/eap_defs.h"
#include "eap_peer/eap_common.h"
#include "rsn_supp/wpa.h"
#include "utils/wpa_debug.h"
#include "wps/wps.h"
#include "wps/wps_defs.h"
#include "wps/wps_dev_attr.h"
#include "wps/wps_i.h"

#include "bl_supplicant/bl_wifi_driver.h"
#include "bl_supplicant/bl_wpas_glue.h"
#include "wifi_mgmr.h"

#define WPS_AP_EAPOL_VERSION 1
#define WSC_FLAGS_MF         0x01
#define WSC_FLAGS_LF         0x02
#define WSC_FRAGMENT_SIZE    1400
#define EAP_VENDOR_TYPE_WSC  1

#define WSC_ID_ENROLLEE      "WFA-SimpleConfig-Enrollee-1-0"
#define WSC_ID_ENROLLEE_LEN  29

enum bl_wps_ap_eap_state {
    BL_WPS_AP_WAIT_EAPOL_START = 0,
    BL_WPS_AP_WAIT_IDENTITY,
    BL_WPS_AP_WAIT_MESG,
    BL_WPS_AP_WAIT_FRAG_ACK,
    BL_WPS_AP_SEND_FRAG_ACK,
    BL_WPS_AP_DONE,
    BL_WPS_AP_FAIL,
};

struct bl_wps_ap_ctx {
    bl_wps_config_t cfg;
    struct wps_context *wps_ctx;
    struct wps_data *wps;
    struct wps_device_data *dev;
    wps_factory_information_t factory_info;
    char serial_number[17];
    u8 ownaddr[ETH_ALEN];
    u8 uuid[WPS_UUID_LEN];
    u8 ssid[MAX_SSID_LEN];
    size_t ssid_len;
    char passphrase[MAX_PASSPHRASE_LEN + 1];
    u8 pin[9];
    u8 *beacon_ie;
    uint16_t beacon_ie_len;
    u8 *probe_resp_ie;
    uint16_t probe_resp_ie_len;
    u8 peer_addr[ETH_ALEN];
    bool peer_active;
    bool eap_fail_pending;
    bool wpa_start_pending;
    u8 vif_idx;
    u8 sta_idx;
    u8 current_identifier;
    enum bl_wps_ap_eap_state state;
    struct wpabuf *in_buf;
    struct wpabuf *out_buf;
    enum wsc_op_code in_op_code;
    enum wsc_op_code out_op_code;
    size_t out_used;
    size_t fragment_size;
};

static struct bl_wps_ap_ctx *g_wps_ap;
extern const struct wpa_funcs *wpa_cbs;
extern void *_ap_get_wpa_sm(uint8_t *mac);

bool bl_wifi_wps_ap_active_internal(void);
bool bl_wifi_wps_ap_assoc_req_internal(uint8_t vif_idx, uint8_t sta_idx,
                                       const uint8_t *peer_addr,
                                       const uint8_t *wps_ie, size_t wps_ie_len);
bool bl_wifi_wps_ap_assoc_done_internal(uint8_t vif_idx, uint8_t sta_idx,
                                        const uint8_t *peer_addr);
bool bl_wifi_wps_ap_rx_eapol_internal(const uint8_t *src_addr, uint8_t *buf,
                                      size_t len);
void bl_wifi_wps_ap_probe_req_rx_internal(const uint8_t *addr,
                                          const uint8_t *wps_ie, size_t wps_ie_len);
void bl_wifi_wps_ap_sta_removed_internal(const uint8_t *peer_addr);

static const struct wps_ap_funcs bl_wps_ap_cb = {
    .active = bl_wifi_wps_ap_active_internal,
    .assoc_req = bl_wifi_wps_ap_assoc_req_internal,
    .assoc_done = bl_wifi_wps_ap_assoc_done_internal,
    .rx_eapol = bl_wifi_wps_ap_rx_eapol_internal,
    .probe_req_rx = bl_wifi_wps_ap_probe_req_rx_internal,
    .sta_removed = bl_wifi_wps_ap_sta_removed_internal,
};

static void bl_wps_ap_log_rx_drop(struct bl_wps_ap_ctx *ctx, const u8 *src_addr,
                                  const char *reason,
                                  const struct ieee802_1x_hdr *xhdr,
                                  size_t frame_len,
                                  const struct eap_hdr *ehdr,
                                  size_t plen, size_t eap_len, int eap_type)
{
    wpa_printf(MSG_INFO,
               "WPS AP: drop %s peer=" MACSTR
               " state=%d xhdr_type=%u frame_len=%u plen=%u"
               " eap_code=%u eap_type=%d eap_len=%u",
               reason, MAC2STR(src_addr), ctx ? ctx->state : -1,
               xhdr ? xhdr->type : 0xff, (unsigned int) frame_len,
               (unsigned int) plen, ehdr ? ehdr->code : 0xff, eap_type,
               (unsigned int) eap_len);
}

static void bl_wps_ap_notify_user(struct bl_wps_ap_ctx *ctx, bl_wps_event_t event,
                                  void *payload)
{
    if (ctx->cfg.event_cb) {
        ctx->cfg.event_cb(event, payload, ctx->cfg.event_cb_arg);
    } else if (payload != NULL) {
        vPortFree(payload);
    }
}

static void bl_wps_ap_start_pending_wpa(struct bl_wps_ap_ctx *ctx,
                                        const char *reason)
{
    void *wpa_sm;

    if (ctx == NULL || !ctx->wpa_start_pending || !ctx->peer_active) {
        return;
    }

    ctx->wpa_start_pending = false;
    wpa_sm = _ap_get_wpa_sm(ctx->peer_addr);
    if (wpa_sm == NULL || wpa_cbs == NULL ||
        wpa_cbs->wpa_ap_sta_associated == NULL) {
        wpa_printf(MSG_INFO,
                   "WPS AP: delayed WPA start skipped after %s for " MACSTR
                   " sta=%u sm=%p",
                   reason ? reason : "unknown", MAC2STR(ctx->peer_addr),
                   ctx->sta_idx, wpa_sm);
        return;
    }

    wpa_cbs->wpa_ap_sta_associated(wpa_sm, ctx->sta_idx);
}

static void bl_wps_ap_set_default_factory(struct bl_wps_ap_ctx *ctx)
{
    snprintf(ctx->factory_info.manufacturer, sizeof(ctx->factory_info.manufacturer),
             "Bouffalo Lab");
    snprintf(ctx->factory_info.model_name, sizeof(ctx->factory_info.model_name),
             "BL60X");
    snprintf(ctx->factory_info.model_number, sizeof(ctx->factory_info.model_number),
             "BL60X");
    snprintf(ctx->factory_info.device_name, sizeof(ctx->factory_info.device_name),
             "BL60X AP");
}

static void bl_wps_ap_apply_factory(const struct bl_wps_config *config,
                                    struct bl_wps_ap_ctx *ctx)
{
    bl_wps_ap_set_default_factory(ctx);

    if (config->factory_info.manufacturer[0] != '\0') {
        memcpy(ctx->factory_info.manufacturer, config->factory_info.manufacturer,
               WPS_MAX_MANUFACTURER_LEN - 1);
    }
    if (config->factory_info.model_number[0] != '\0') {
        memcpy(ctx->factory_info.model_number, config->factory_info.model_number,
               WPS_MAX_MODEL_NUMBER_LEN - 1);
    }
    if (config->factory_info.model_name[0] != '\0') {
        memcpy(ctx->factory_info.model_name, config->factory_info.model_name,
               WPS_MAX_MODEL_NAME_LEN - 1);
    }
    if (config->factory_info.device_name[0] != '\0') {
        memcpy(ctx->factory_info.device_name, config->factory_info.device_name,
               WPS_MAX_DEVICE_NAME_LEN - 1);
    }
}

static void bl_wps_ap_free_ies(struct bl_wps_ap_ctx *ctx)
{
    if (ctx->beacon_ie) {
        os_free(ctx->beacon_ie);
        ctx->beacon_ie = NULL;
        ctx->beacon_ie_len = 0;
    }
    if (ctx->probe_resp_ie) {
        os_free(ctx->probe_resp_ie);
        ctx->probe_resp_ie = NULL;
        ctx->probe_resp_ie_len = 0;
    }
}

static int bl_wps_ap_copy_ie(u8 **dst, uint16_t *dst_len, const struct wpabuf *src)
{
    size_t len;

    if (*dst) {
        os_free(*dst);
        *dst = NULL;
        *dst_len = 0;
    }
    if (src == NULL) {
        return 0;
    }

    len = wpabuf_len(src);
    if (len == 0) {
        return 0;
    }

    *dst = os_malloc(len);
    if (*dst == NULL) {
        return -1;
    }

    os_memcpy(*dst, wpabuf_head_u8(src), len);
    *dst_len = len;
    return 0;
}

static int bl_wps_ap_set_ie_cb(void *cb_ctx, struct wpabuf *beacon_ie,
                               struct wpabuf *probe_resp_ie)
{
    struct bl_wps_ap_ctx *ctx = cb_ctx;
    int ret;

    ret = bl_wps_ap_copy_ie(&ctx->beacon_ie, &ctx->beacon_ie_len, beacon_ie);
    if (ret == 0) {
        ret = bl_wps_ap_copy_ie(&ctx->probe_resp_ie, &ctx->probe_resp_ie_len,
                                probe_resp_ie);
    }

    wpabuf_free(beacon_ie);
    wpabuf_free(probe_resp_ie);

    return ret;
}

static void bl_wps_ap_event_cb(void *cb_ctx, enum wps_event event,
                               union wps_event_data *data)
{
    struct bl_wps_ap_ctx *ctx = cb_ctx;

    (void)data;

    switch (event) {
        case WPS_EV_FAIL:
            bl_wps_ap_notify_user(ctx, BL_WPS_EVENT_FAILURE, NULL);
            break;
        case WPS_EV_PBC_OVERLAP:
            bl_wps_ap_notify_user(ctx, BL_WPS_EVENT_SESSION_OVERLAP, NULL);
            break;
        default:
            break;
    }
}

static void bl_wps_ap_reg_success_cb(void *cb_ctx, const u8 *mac_addr,
                                     const u8 *uuid_e, const u8 *dev_pw,
                                     size_t dev_pw_len)
{
    struct bl_wps_ap_ctx *ctx = cb_ctx;
    void *wpa_sm;

    (void)uuid_e;
    (void)dev_pw;
    (void)dev_pw_len;

    if (ctx != NULL && mac_addr != NULL &&
        os_memcmp(ctx->peer_addr, mac_addr, ETH_ALEN) == 0) {
        wpa_sm = _ap_get_wpa_sm((uint8_t *) mac_addr);
        if (wpa_sm != NULL) {
            ctx->wpa_start_pending = true;
        } else {
            wpa_printf(MSG_INFO, "WPS AP: registrar completed without WPA SM for "
                       MACSTR " sta=%u", MAC2STR(mac_addr),
                       ctx ? ctx->sta_idx : 0);
        }
    }

    bl_wps_ap_notify_user(ctx, BL_WPS_EVENT_REG_SUCCESS, NULL);
}

static void bl_wps_ap_enrollee_seen_cb(void *cb_ctx, const u8 *addr,
                                       const u8 *uuid_e,
                                       const u8 *pri_dev_type,
                                       u16 config_methods,
                                       u16 dev_password_id, u8 request_type,
                                       const char *dev_name)
{
    struct bl_wps_ap_ctx *ctx = cb_ctx;
    char uuid[40];
    char devtype[WPS_DEV_TYPE_BUFSIZE];

    (void)ctx;

    if (uuid_bin2str(uuid_e, uuid, sizeof(uuid))) {
        uuid[0] = '\0';
    }
    if (dev_name == NULL) {
        dev_name = "";
    }

    wpa_printf(MSG_INFO,
               "WPS-ENROLLEE-SEEN " MACSTR " %s %s 0x%x %u %u [%s]",
               MAC2STR(addr), uuid,
               wps_dev_type_bin2str(pri_dev_type, devtype, sizeof(devtype)),
               config_methods, dev_password_id, request_type, dev_name);
}

static int bl_wps_ap_init_device(struct bl_wps_ap_ctx *ctx)
{
    struct wps_device_data *dev;

    dev = &ctx->wps_ctx->dev;
    ctx->dev = dev;

    dev->config_methods = WPS_CONFIG_PUSHBUTTON | WPS_CONFIG_DISPLAY;
#ifdef CONFIG_WPS2
    dev->config_methods |= WPS_CONFIG_VIRT_PUSHBUTTON |
                           WPS_CONFIG_PHY_PUSHBUTTON |
                           WPS_CONFIG_PHY_DISPLAY;
#endif
    dev->rf_bands = WPS_RF_24GHZ;

    WPA_PUT_BE16(dev->pri_dev_type, WPS_DEV_COMPUTER);
    WPA_PUT_BE32(dev->pri_dev_type + 2, WPS_DEV_OUI_WFA);
    WPA_PUT_BE16(dev->pri_dev_type + 6, WPS_DEV_COMPUTER_PC);

    dev->manufacturer = ctx->factory_info.manufacturer;
    dev->model_name = ctx->factory_info.model_name;
    dev->model_number = ctx->factory_info.model_number;
    dev->device_name = ctx->factory_info.device_name;
    snprintf(ctx->serial_number, sizeof(ctx->serial_number),
             "%02x%02x%02x%02x%02x%02x",
             ctx->ownaddr[0], ctx->ownaddr[1], ctx->ownaddr[2], ctx->ownaddr[3],
             ctx->ownaddr[4], ctx->ownaddr[5]);
    dev->serial_number = ctx->serial_number;

    uuid_gen_mac_addr(ctx->ownaddr, ctx->uuid);
    memcpy(ctx->wps_ctx->uuid, ctx->uuid, WPS_UUID_LEN);
    memcpy(dev->mac_addr, ctx->ownaddr, ETH_ALEN);

    return 0;
}

static void bl_wps_ap_deinit_device(struct bl_wps_ap_ctx *ctx)
{
    if (ctx->dev == NULL) {
        return;
    }

    ctx->dev->manufacturer = NULL;
    ctx->dev->model_name = NULL;
    ctx->dev->model_number = NULL;
    ctx->dev->device_name = NULL;
    ctx->dev->serial_number = NULL;
    ctx->dev = NULL;
}

static void bl_wps_ap_free_session(struct bl_wps_ap_ctx *ctx)
{
    if (ctx == NULL) {
        return;
    }

    if (ctx->in_buf) {
        wpabuf_free(ctx->in_buf);
        ctx->in_buf = NULL;
    }
    if (ctx->out_buf) {
        wpabuf_free(ctx->out_buf);
        ctx->out_buf = NULL;
    }
    if (ctx->wps) {
        wpabuf_free(ctx->wps->dh_privkey);
        wpabuf_free(ctx->wps->dh_pubkey_e);
        wpabuf_free(ctx->wps->dh_pubkey_r);
        wpabuf_free(ctx->wps->last_msg);
        bin_clear_free(ctx->wps->dev_password, ctx->wps->dev_password_len);
        wps_device_data_free(&ctx->wps->peer_dev);
        dh5_free(ctx->wps->dh_ctx);
        os_free(ctx->wps);
        ctx->wps = NULL;
    }
    ctx->peer_active = false;
    ctx->eap_fail_pending = false;
    ctx->wpa_start_pending = false;
    os_memset(ctx->peer_addr, 0, sizeof(ctx->peer_addr));
    ctx->current_identifier = 0;
    ctx->out_used = 0;
    ctx->state = BL_WPS_AP_WAIT_EAPOL_START;
}

static void bl_wps_ap_free_ctx(struct bl_wps_ap_ctx *ctx)
{
    if (ctx == NULL) {
        return;
    }

    bl_wps_ap_free_session(ctx);
    bl_wps_ap_free_ies(ctx);

    if (ctx->wps_ctx) {
        if (ctx->wps_ctx->registrar) {
            wps_registrar_deinit(ctx->wps_ctx->registrar);
            ctx->wps_ctx->registrar = NULL;
        }
        ctx->wps_ctx->network_key = NULL;
        bl_wps_ap_deinit_device(ctx);
        os_free(ctx->wps_ctx);
    }

    os_free(ctx);
}

static int bl_wps_ap_send_frame(struct bl_wps_ap_ctx *ctx, const u8 *payload,
                                size_t payload_len)
{
    u8 *frame;
    size_t frame_len;
    struct l2_ethhdr *eth;

    frame = wpa_sm_alloc_eapol(WPS_AP_EAPOL_VERSION, IEEE802_1X_TYPE_EAP_PACKET,
                               payload, payload_len, &frame_len, NULL);
    if (frame == NULL) {
        return -1;
    }

    eth = (struct l2_ethhdr *)(frame - sizeof(struct l2_ethhdr));
    os_memcpy(eth->h_dest, ctx->peer_addr, ETH_ALEN);
    os_memcpy(eth->h_source, ctx->ownaddr, ETH_ALEN);
    eth->h_proto = host_to_be16(ETH_P_EAPOL);

    wpa_sendto_wrapper(false, eth, sizeof(*eth) + frame_len, NULL);
    wpa_sm_free_eapol(frame);
    return 0;
}

static int bl_wps_ap_send_identity_request(struct bl_wps_ap_ctx *ctx)
{
    struct wpabuf *msg;
    int ret = -1;

    ctx->current_identifier++;
    msg = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_IDENTITY, 0,
                        EAP_CODE_REQUEST, ctx->current_identifier);
    if (msg == NULL) {
        return -1;
    }

    ret = bl_wps_ap_send_frame(ctx, wpabuf_head_u8(msg), wpabuf_len(msg));
    wpabuf_free(msg);
    if (ret == 0) {
        ctx->state = BL_WPS_AP_WAIT_IDENTITY;
    }
    return ret;
}

static int bl_wps_ap_send_failure(struct bl_wps_ap_ctx *ctx)
{
    struct eap_hdr *hdr;
    u8 *msg;
    size_t msg_len = sizeof(*hdr);
    int ret = -1;

    ctx->current_identifier++;
    msg = os_zalloc(msg_len);
    if (msg == NULL) {
        return -1;
    }

    hdr = (struct eap_hdr *)msg;
    hdr->code = EAP_CODE_FAILURE;
    hdr->identifier = ctx->current_identifier;
    hdr->length = host_to_be16(msg_len);

    ret = bl_wps_ap_send_frame(ctx, msg, msg_len);
    os_free(msg);
    return ret;
}

static int bl_wps_ap_send_wsc_start(struct bl_wps_ap_ctx *ctx)
{
    struct wpabuf *msg;
    int ret = -1;

    ctx->current_identifier++;
    msg = eap_msg_alloc(EAP_VENDOR_WFA, EAP_VENDOR_TYPE_WSC, 2,
                        EAP_CODE_REQUEST, ctx->current_identifier);
    if (msg == NULL) {
        return -1;
    }

    wpabuf_put_u8(msg, WSC_Start);
    wpabuf_put_u8(msg, 0);

    ret = bl_wps_ap_send_frame(ctx, wpabuf_head_u8(msg), wpabuf_len(msg));
    wpabuf_free(msg);
    if (ret == 0) {
        ctx->state = BL_WPS_AP_WAIT_MESG;
    }
    return ret;
}

static int bl_wps_ap_send_frag_ack(struct bl_wps_ap_ctx *ctx)
{
    struct wpabuf *msg;
    int ret = -1;

    msg = eap_msg_alloc(EAP_VENDOR_WFA, EAP_VENDOR_TYPE_WSC, 2,
                        EAP_CODE_REQUEST, ctx->current_identifier);
    if (msg == NULL) {
        return -1;
    }

    wpabuf_put_u8(msg, WSC_FRAG_ACK);
    wpabuf_put_u8(msg, 0);

    ret = bl_wps_ap_send_frame(ctx, wpabuf_head_u8(msg), wpabuf_len(msg));
    wpabuf_free(msg);
    if (ret == 0) {
        ctx->state = BL_WPS_AP_WAIT_MESG;
    }
    return ret;
}

static int bl_wps_ap_send_next_msg(struct bl_wps_ap_ctx *ctx)
{
    struct wpabuf *msg;
    u8 flags;
    size_t send_len;
    size_t plen;
    int ret = -1;

    if (ctx->out_buf == NULL) {
        ctx->out_buf = wps_get_msg(ctx->wps, &ctx->out_op_code);
        ctx->out_used = 0;
        if (ctx->out_buf == NULL) {
            return -1;
        }
    }

    flags = 0;
    send_len = wpabuf_len(ctx->out_buf) - ctx->out_used;
    if (send_len + 2 > ctx->fragment_size) {
        send_len = ctx->fragment_size - 2;
        flags |= WSC_FLAGS_MF;
        if (ctx->out_used == 0) {
            flags |= WSC_FLAGS_LF;
            send_len -= 2;
        }
    }

    plen = 2 + send_len;
    if (flags & WSC_FLAGS_LF) {
        plen += 2;
    }

    ctx->current_identifier++;
    msg = eap_msg_alloc(EAP_VENDOR_WFA, EAP_VENDOR_TYPE_WSC, plen,
                        EAP_CODE_REQUEST, ctx->current_identifier);
    if (msg == NULL) {
        return -1;
    }

    wpabuf_put_u8(msg, ctx->out_op_code);
    wpabuf_put_u8(msg, flags);
    if (flags & WSC_FLAGS_LF) {
        wpabuf_put_be16(msg, wpabuf_len(ctx->out_buf));
    }
    wpabuf_put_data(msg, wpabuf_head_u8(ctx->out_buf) + ctx->out_used, send_len);

    ret = bl_wps_ap_send_frame(ctx, wpabuf_head_u8(msg), wpabuf_len(msg));
    wpabuf_free(msg);
    if (ret != 0) {
        return ret;
    }

    ctx->out_used += send_len;
    if (ctx->out_used == wpabuf_len(ctx->out_buf)) {
        wpabuf_free(ctx->out_buf);
        ctx->out_buf = NULL;
        ctx->out_used = 0;
        if (ctx->eap_fail_pending) {
            ctx->eap_fail_pending = false;
            ctx->state = BL_WPS_AP_DONE;
            ret = bl_wps_ap_send_failure(ctx);
            if (ret == 0) {
                bl_wps_ap_start_pending_wpa(ctx, "WPS final EAP-Failure");
            }
            return ret;
        }
        ctx->state = BL_WPS_AP_WAIT_MESG;
    } else {
        ctx->state = BL_WPS_AP_WAIT_FRAG_ACK;
    }

    return 0;
}

static int bl_wps_ap_process_fragment(struct bl_wps_ap_ctx *ctx, u8 op_code,
                                      u8 flags, const u8 *payload, size_t len)
{
    u16 total_len = 0;

    if (flags & WSC_FLAGS_LF) {
        if (len < 2) {
            return -1;
        }

        total_len = WPA_GET_BE16(payload);
        payload += 2;
        len -= 2;

        if (total_len < len || total_len > 50000) {
            return -1;
        }

        wpabuf_free(ctx->in_buf);
        ctx->in_buf = wpabuf_alloc(total_len);
        if (ctx->in_buf == NULL) {
            return -1;
        }
        ctx->in_op_code = op_code;
    } else if (ctx->in_buf == NULL) {
        /*
         * Unfragmented WSC payloads commonly omit the Length Field and arrive
         * as a single WSC_MSG frame. Those should flow through directly
         * without forcing temporary reassembly state.
         */
        if (flags & WSC_FLAGS_MF) {
            return -1;
        }
        return 0;
    } else if (ctx->in_op_code != op_code) {
        return -1;
    }

    if (len > wpabuf_tailroom(ctx->in_buf)) {
        return -1;
    }

    wpabuf_put_data(ctx->in_buf, payload, len);
    if (flags & WSC_FLAGS_MF) {
        ctx->state = BL_WPS_AP_SEND_FRAG_ACK;
        return bl_wps_ap_send_frag_ack(ctx);
    }

    return 0;
}

static int bl_wps_ap_complete_input(struct bl_wps_ap_ctx *ctx,
                                    const struct wpabuf *msg,
                                    enum wsc_op_code op_code)
{
    enum wps_process_res res;

    res = wps_process_msg(ctx->wps, op_code, msg);
    switch (res) {
        case WPS_CONTINUE:
            return bl_wps_ap_send_next_msg(ctx);
        case WPS_DONE:
            ctx->eap_fail_pending = true;
            return bl_wps_ap_send_next_msg(ctx);
        case WPS_PENDING:
            ctx->state = BL_WPS_AP_WAIT_MESG;
            return 0;
        case WPS_IGNORE:
            return 0;
        default:
            ctx->state = BL_WPS_AP_FAIL;
            bl_wps_ap_notify_user(ctx, BL_WPS_EVENT_FAILURE, NULL);
            return -1;
    }
}

static int bl_wps_ap_process_wsc_message(struct bl_wps_ap_ctx *ctx, u8 op_code,
                                         u8 flags, const u8 *payload, size_t len)
{
    struct wpabuf tmp;
    int ret;

    if (ctx->state == BL_WPS_AP_WAIT_FRAG_ACK) {
        if (op_code != WSC_FRAG_ACK) {
            return -1;
        }
        return bl_wps_ap_send_next_msg(ctx);
    }

    if (op_code != WSC_ACK && op_code != WSC_NACK && op_code != WSC_MSG &&
        op_code != WSC_Done) {
        return -1;
    }

    ret = bl_wps_ap_process_fragment(ctx, op_code, flags, payload, len);
    if (ret != 0) {
        return ret;
    }
    if (flags & WSC_FLAGS_MF) {
        return 0;
    }

    if (ctx->in_buf != NULL) {
        ret = bl_wps_ap_complete_input(ctx, ctx->in_buf, op_code);
        wpabuf_free(ctx->in_buf);
        ctx->in_buf = NULL;
        return ret;
    }

    wpabuf_set(&tmp, payload + ((flags & WSC_FLAGS_LF) ? 2 : 0),
               len - ((flags & WSC_FLAGS_LF) ? 2 : 0));
    return bl_wps_ap_complete_input(ctx, &tmp, op_code);
}

static int bl_wps_ap_start_registrar(struct bl_wps_ap_ctx *ctx)
{
    struct wps_registrar_config cfg;
    int ret;

    memset(&cfg, 0, sizeof(cfg));
    cfg.set_ie_cb = bl_wps_ap_set_ie_cb;
    cfg.reg_success_cb = bl_wps_ap_reg_success_cb;
    cfg.enrollee_seen_cb = bl_wps_ap_enrollee_seen_cb;
    cfg.cb_ctx = ctx;

    ctx->wps_ctx->registrar = wps_registrar_init(ctx->wps_ctx, &cfg);
    if (ctx->wps_ctx->registrar == NULL) {
        return -1;
    }

    if (ctx->cfg.type == WPS_TYPE_PBC) {
        ctx->wps_ctx->ap_pin_len = 0;
        ret = wps_registrar_button_pushed(ctx->wps_ctx->registrar, NULL);
    } else {
        if (ctx->cfg.pin && wps_pin_str_valid(ctx->cfg.pin)) {
            memcpy(ctx->pin, ctx->cfg.pin, 8);
            ctx->pin[8] = '\0';
        } else {
            snprintf((char *)ctx->pin, sizeof(ctx->pin), "%08u",
                     wps_generate_pin());
        }
        os_memset(ctx->wps_ctx->ap_pin, 0, sizeof(ctx->wps_ctx->ap_pin));
        os_memcpy(ctx->wps_ctx->ap_pin, ctx->pin, 8);
        ctx->wps_ctx->ap_pin_len = 8;
        ret = wps_registrar_add_pin(ctx->wps_ctx->registrar, NULL, NULL,
                                    ctx->pin, 8, WPS_TIMEOUT_MS / 1000);
        if (ret == 0) {
            bl_wps_pin_t *pin = pvPortMalloc(sizeof(*pin));

            if (pin != NULL) {
                memset(pin, 0, sizeof(*pin));
                memcpy(pin->pin, ctx->pin, 8);
                bl_wps_ap_notify_user(ctx, BL_WPS_EVENT_PIN, pin);
            }
        }
    }

    if (ret != 0) {
        return -1;
    }

    return wps_registrar_update_ie(ctx->wps_ctx->registrar);
}

static int bl_wps_ap_init_ctx(struct bl_wps_ap_ctx *ctx, const char *ssid,
                              const char *passphrase)
{
    wifi_mgmr_ap_mac_get(ctx->ownaddr);
    if (is_zero_ether_addr(ctx->ownaddr)) {
        wifi_mgmr_sta_mac_get(ctx->ownaddr);
    }

    ctx->ssid_len = os_strlen(ssid);
    if (ctx->ssid_len > sizeof(ctx->ssid)) {
        ctx->ssid_len = sizeof(ctx->ssid);
    }
    os_memcpy(ctx->ssid, ssid, ctx->ssid_len);
    os_strlcpy(ctx->passphrase, passphrase, sizeof(ctx->passphrase));
    ctx->fragment_size = WSC_FRAGMENT_SIZE;
    ctx->state = BL_WPS_AP_WAIT_EAPOL_START;

    ctx->wps_ctx = os_zalloc(sizeof(*ctx->wps_ctx));
    if (ctx->wps_ctx == NULL) {
        return -1;
    }

    bl_wps_ap_apply_factory(&ctx->cfg, ctx);
    if (bl_wps_ap_init_device(ctx) != 0) {
        return -1;
    }

    ctx->wps_ctx->ap = 1;
    ctx->wps_ctx->wps_state = WPS_STATE_CONFIGURED;
    ctx->wps_ctx->event_cb = bl_wps_ap_event_cb;
    ctx->wps_ctx->cb_ctx = ctx;
    ctx->wps_ctx->ssid_len = ctx->ssid_len;
    os_memcpy(ctx->wps_ctx->ssid, ctx->ssid, ctx->ssid_len);
    ctx->wps_ctx->config_methods = ctx->dev->config_methods;
    ctx->wps_ctx->auth_types = WPS_AUTH_WPA2PSK;
    ctx->wps_ctx->encr_types = WPS_ENCR_AES;
    ctx->wps_ctx->network_key_len = os_strlen(ctx->passphrase);
    ctx->wps_ctx->network_key = (u8 *) ctx->passphrase;

    return bl_wps_ap_start_registrar(ctx);
}

static int bl_wps_ap_open_session(struct bl_wps_ap_ctx *ctx, uint8_t vif_idx,
                                  uint8_t sta_idx, const uint8_t *peer_addr)
{
    struct wps_data *wps;

    if (ctx == NULL || peer_addr == NULL) {
        return -1;
    }

    if (ctx->peer_active) {
        if (os_memcmp(ctx->peer_addr, peer_addr, ETH_ALEN) != 0) {
            return -1;
        }

        ctx->vif_idx = vif_idx;
        ctx->sta_idx = sta_idx;
        return 0;
    }

    if (ctx->wps != NULL || ctx->in_buf != NULL || ctx->out_buf != NULL) {
        bl_wps_ap_free_session(ctx);
    }

    wps = os_zalloc(sizeof(*wps));
    if (wps == NULL) {
        wpa_printf(MSG_ERROR, "WPS AP: failed to allocate session for "
                   MACSTR, MAC2STR(peer_addr));
        return -1;
    }

    wps->wps = ctx->wps_ctx;
    wps->registrar = 1;
    wps->pbc = ctx->cfg.type == WPS_TYPE_PBC;
    wps->state = RECV_M1;
    os_memcpy(wps->uuid_r, ctx->uuid, WPS_UUID_LEN);
    os_memcpy(ctx->peer_addr, peer_addr, ETH_ALEN);

    ctx->wps = wps;
    ctx->peer_active = true;
    ctx->vif_idx = vif_idx;
    ctx->sta_idx = sta_idx;
    ctx->state = BL_WPS_AP_WAIT_EAPOL_START;
    return 0;
}

bl_wps_err_t bl_wifi_wps_ap_start(const struct bl_wps_config *config,
                                  const char *ssid, const char *passphrase)
{
    struct bl_wps_ap_ctx *ctx;
    int wifi_state = 0;

    if (g_wps_ap != NULL || wps_sm_get() != NULL) {
        return BL_WPS_ERR_DUPLICATE_INSTANCE;
    }
    if (config == NULL || ssid == NULL || passphrase == NULL || passphrase[0] == '\0') {
        return BL_WPS_ERR_WIFI_STATE;
    }

    wifi_mgmr_state_get(&wifi_state);
    if (!(wifi_state == WIFI_STATE_IDLE || wifi_state == WIFI_STATE_WITH_AP_IDLE)) {
        return BL_WPS_ERR_WIFI_STATE;
    }

    ctx = os_zalloc(sizeof(*ctx));
    if (ctx == NULL) {
        return BL_WPS_ERR_MEMORY;
    }

    ctx->cfg = *config;
    if (bl_wps_ap_init_ctx(ctx, ssid, passphrase) != 0) {
        bl_wps_ap_free_ctx(ctx);
        return BL_WPS_ERR_MEMORY;
    }

    g_wps_ap = ctx;
    bl_wifi_register_wps_ap_cb_internal(&bl_wps_ap_cb);
    return BL_WPS_ERR_OK;
}

void bl_wifi_wps_stop(void)
{
    if (g_wps_ap == NULL) {
        return;
    }

    bl_wifi_unregister_wps_ap_cb_internal();
    bl_wps_ap_free_ctx(g_wps_ap);
    g_wps_ap = NULL;
}

int bl_wifi_wps_ap_get_ies(const uint8_t **beacon_ie, uint16_t *beacon_ie_len,
                           const uint8_t **probe_resp_ie, uint16_t *probe_resp_ie_len)
{
    if (g_wps_ap == NULL) {
        return -1;
    }

    if (beacon_ie) {
        *beacon_ie = g_wps_ap->beacon_ie;
    }
    if (beacon_ie_len) {
        *beacon_ie_len = g_wps_ap->beacon_ie_len;
    }
    if (probe_resp_ie) {
        *probe_resp_ie = g_wps_ap->probe_resp_ie;
    }
    if (probe_resp_ie_len) {
        *probe_resp_ie_len = g_wps_ap->probe_resp_ie_len;
    }

    return 0;
}

bool bl_wifi_wps_ap_active_internal(void)
{
    return g_wps_ap != NULL;
}

bool bl_wifi_wps_ap_assoc_req_internal(uint8_t vif_idx, uint8_t sta_idx,
                                       const uint8_t *peer_addr,
                                       const uint8_t *wps_ie, size_t wps_ie_len)
{
    int ret;

    if (g_wps_ap == NULL || peer_addr == NULL) {
        return false;
    }

    ret = bl_wps_ap_open_session(g_wps_ap, vif_idx, sta_idx, peer_addr);
    return ret == 0;
}

bool bl_wifi_wps_ap_assoc_done_internal(uint8_t vif_idx, uint8_t sta_idx,
                                        const uint8_t *peer_addr)
{
    if (g_wps_ap == NULL || peer_addr == NULL || !g_wps_ap->peer_active) {
        return false;
    }

    if (os_memcmp(g_wps_ap->peer_addr, peer_addr, ETH_ALEN) != 0) {
        return false;
    }

    g_wps_ap->vif_idx = vif_idx;
    g_wps_ap->sta_idx = sta_idx;
    return true;
}

bool bl_wifi_wps_ap_rx_eapol_internal(const uint8_t *src_addr, uint8_t *buf,
                                      size_t len)
{
    struct bl_wps_ap_ctx *ctx = g_wps_ap;
    struct ieee802_1x_hdr *xhdr;

    struct eap_hdr *ehdr;
    const u8 *payload;
    size_t plen;
    size_t eap_len;
    u8 op_code;
    u8 flags;
    int ret;
    static const u8 wfa_vendor_id[3] = { 0x00, 0x37, 0x2A };

    if (ctx == NULL || src_addr == NULL) {
        return false;
    }
    if (len < sizeof(*xhdr)) {
        bl_wps_ap_log_rx_drop(ctx, src_addr, "short 802.1X header", NULL, len,
                              NULL, 0, 0, -1);
        return false;
    }

    xhdr = (struct ieee802_1x_hdr *)buf;

    if (ctx->peer_active == false) {
        if (xhdr->type != IEEE802_1X_TYPE_EAPOL_START &&
            xhdr->type != IEEE802_1X_TYPE_EAP_PACKET) {
            bl_wps_ap_log_rx_drop(ctx, src_addr,
                                  "first frame is neither EAPOL-Start nor EAP",
                                  xhdr, len, NULL, 0, 0, -1);
            return false;
        }

        if (bl_wps_ap_open_session(ctx, 0, 0, src_addr) != 0) {
            bl_wps_ap_log_rx_drop(ctx, src_addr,
                                  "failed to open lazy WPS AP session", xhdr,
                                  len, NULL, 0, 0, -1);
            return false;
        }

    }

    if (os_memcmp(ctx->peer_addr, src_addr, ETH_ALEN) != 0) {
        return false;
    }

    plen = be_to_host16(xhdr->length);

    if (xhdr->type == IEEE802_1X_TYPE_EAPOL_START) {
        if (ctx->state == BL_WPS_AP_WAIT_EAPOL_START) {
            ret = bl_wps_ap_send_identity_request(ctx);
            if (ret != 0) {
                bl_wps_ap_log_rx_drop(ctx, src_addr,
                                      "failed to send Identity request", xhdr,
                                      len, NULL, plen, 0, -1);
            }
            return ret == 0;
        }
        return true;
    }

    if (xhdr->type != IEEE802_1X_TYPE_EAP_PACKET ||
        plen < sizeof(*ehdr) ||
        len < sizeof(*xhdr) + plen) {
        bl_wps_ap_log_rx_drop(ctx, src_addr, "invalid EAP packet envelope",
                              xhdr, len, NULL, plen, 0, -1);
        return false;
    }

    ehdr = (struct eap_hdr *)(xhdr + 1);
    eap_len = be_to_host16(ehdr->length);
    if (eap_len < sizeof(*ehdr) || eap_len > plen) {
        bl_wps_ap_log_rx_drop(ctx, src_addr, "invalid EAP length", xhdr, len,
                              ehdr, plen, eap_len, -1);
        return false;
    }

    payload = (const u8 *)(ehdr + 1);
    if (ehdr->code != EAP_CODE_RESPONSE) {
        bl_wps_ap_log_rx_drop(ctx, src_addr, "unexpected EAP code", xhdr, len,
                              ehdr, plen, eap_len,
                              eap_len >= sizeof(*ehdr) + 1 ? payload[0] : -1);
        return false;
    }
    if (eap_len < sizeof(*ehdr) + 1) {
        bl_wps_ap_log_rx_drop(ctx, src_addr, "missing EAP type byte", xhdr,
                              len, ehdr, plen, eap_len, -1);
        return false;
    }

    switch (payload[0]) {
        case EAP_TYPE_IDENTITY:
            if (ctx->state == BL_WPS_AP_WAIT_EAPOL_START) {
                ctx->state = BL_WPS_AP_WAIT_IDENTITY;
            }
            if (ctx->state != BL_WPS_AP_WAIT_IDENTITY) {
                bl_wps_ap_log_rx_drop(ctx, src_addr,
                                      "Identity response arrived in wrong state",
                                      xhdr, len, ehdr, plen, eap_len,
                                      payload[0]);
                return false;
            }
            if (eap_len < sizeof(*ehdr) + 1 + WSC_ID_ENROLLEE_LEN ||
                os_memcmp(payload + 1, WSC_ID_ENROLLEE, WSC_ID_ENROLLEE_LEN) != 0) {
                wpa_printf(MSG_INFO, "WPS AP: identity mismatch from " MACSTR
                           " payload_len=%u expected_len=%u",
                           MAC2STR(src_addr),
                           (unsigned int)(eap_len - sizeof(*ehdr) - 1),
                           (unsigned int)WSC_ID_ENROLLEE_LEN);
                return false;
            }
            ret = bl_wps_ap_send_wsc_start(ctx);
            if (ret != 0) {
                bl_wps_ap_log_rx_drop(ctx, src_addr,
                                      "failed to send WSC_Start", xhdr, len,
                                      ehdr, plen, eap_len, payload[0]);
            }
            return ret == 0;
        case EAP_TYPE_EXPANDED: {
            const u8 *ubuf;
            const struct eap_expand *expd;

            if (eap_len < sizeof(*ehdr) + 1 + sizeof(struct eap_expand) + 1) {
                bl_wps_ap_log_rx_drop(ctx, src_addr,
                                      "short expanded EAP payload", xhdr, len,
                                      ehdr, plen, eap_len, payload[0]);
                return false;
            }
            ubuf = payload + 1;
            expd = (const struct eap_expand *)ubuf;
            if (os_memcmp(expd->vendor_id, wfa_vendor_id,
                          sizeof(wfa_vendor_id)) != 0 ||
                be_to_host32(expd->vendor_type) != EAP_VENDOR_TYPE_WSC) {
                bl_wps_ap_log_rx_drop(ctx, src_addr,
                                      "expanded EAP payload is not WSC", xhdr,
                                      len, ehdr, plen, eap_len, payload[0]);
                return false;
            }
            op_code = expd->opcode;
            payload = ubuf + sizeof(struct eap_expand);
            flags = *payload++;
            ret = bl_wps_ap_process_wsc_message(ctx, op_code, flags, payload,
                                                eap_len - sizeof(*ehdr) - 1 -
                                                sizeof(struct eap_expand) - 1);
            if (ret != 0) {
                wpa_printf(MSG_INFO, "WPS AP: WSC processing failed for "
                           MACSTR " state=%d opcode=%u flags=0x%02x",
                           MAC2STR(src_addr), ctx->state, op_code, flags);
            }
            return ret == 0;
        }
        default:
            bl_wps_ap_log_rx_drop(ctx, src_addr, "unexpected EAP type", xhdr,
                                  len, ehdr, plen, eap_len, payload[0]);
            return false;
    }
}

void bl_wifi_wps_ap_probe_req_rx_internal(const uint8_t *addr,
                                          const uint8_t *wps_ie,
                                          size_t wps_ie_len)
{
    struct wpabuf *wps_buf;

    if (g_wps_ap == NULL || g_wps_ap->wps_ctx == NULL ||
        g_wps_ap->wps_ctx->registrar == NULL || addr == NULL ||
        wps_ie == NULL || wps_ie_len < 6) {
        return;
    }

    wps_buf = wpabuf_alloc_copy(wps_ie + 6, wps_ie_len - 6);
    if (wps_buf == NULL) {
        wpa_printf(MSG_ERROR, "WPS AP: probe request buffer allocation failed for "
                   MACSTR " ie_len=%u", MAC2STR(addr),
                   (unsigned int) wps_ie_len);
        return;
    }
    wps_registrar_probe_req_rx(g_wps_ap->wps_ctx->registrar, addr, wps_buf, 0);
    wpabuf_free(wps_buf);
}

void bl_wifi_wps_ap_sta_removed_internal(const uint8_t *peer_addr)
{
    if (g_wps_ap == NULL || g_wps_ap->peer_active == false || peer_addr == NULL) {
        return;
    }
    if (os_memcmp(g_wps_ap->peer_addr, peer_addr, ETH_ALEN) == 0) {
        wpa_printf(MSG_INFO, "WPS AP session remove for " MACSTR,
                   MAC2STR(peer_addr));
        bl_wps_ap_free_session(g_wps_ap);
    }
}
