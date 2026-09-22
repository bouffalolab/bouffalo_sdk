#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform_al.h"
#include "async_event.h"
#include "cfgmacsw.h"
#include "wifi_mgmr_ext.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "macsw.h"

#include <bflb_efuse.h>
#include <bflb_sec_trng.h>
#include "mm.h"
#include "wifi_mgmr_coex_internal.h"

#if defined(BL618DG) && defined(CONFIG_RF) && defined(CONFIG_FHOST)
#include <errno.h>
#include "board_rf.h"
#endif

#if defined(BL618DG) && defined(CONFIG_RF) && defined(CONFIG_FHOST) && \
    defined(BOARD_RF_COEX_OPS_SUPPORTED)
static int platform_coex_rf_result(int ret)
{
    switch (ret) {
    case 0: return WIFI_MGMR_COEX_OK;
    case -EINVAL: return WIFI_MGMR_COEX_ERR_INVALID_ARGUMENT;
    case -ENOTSUP: return WIFI_MGMR_COEX_ERR_NOT_SUPPORTED;
    case -EAGAIN:
    case -ENODEV: return WIFI_MGMR_COEX_ERR_NOT_READY;
    case -EBUSY: return WIFI_MGMR_COEX_ERR_BUSY;
    default: return WIFI_MGMR_COEX_ERR_APPLY_FAILED;
    }
}

static int platform_coex_board_modes_get(
    const struct wifi_mgmr_coex_board_config *board, uint32_t *modes)
{
    uint32_t board_modes;
    int ret;

    if (board == NULL || modes == NULL ||
        board->topology < WIFI_MGMR_COEX_BOARD_COMBO_SHARED_PATH ||
        board->topology > WIFI_MGMR_COEX_BOARD_STANDALONE_SINGLE_ANT_SPDT ||
        (board->topology == WIFI_MGMR_COEX_BOARD_STANDALONE_SINGLE_ANT_SPDT ?
         board->spdt_gpio < 0 : board->spdt_gpio != -1)) {
        return WIFI_MGMR_COEX_ERR_INVALID_ARGUMENT;
    }
    ret = board_rf_modes_get(board->topology != WIFI_MGMR_COEX_BOARD_COMBO_SHARED_PATH,
                            board->spdt_gpio, &board_modes);
    if (ret != 0) {
        return platform_coex_rf_result(ret);
    }
    *modes = 0;
    if (board_modes & BOARD_RF_MODE_COMBO_BIT) {
        *modes |= 1u << WIFI_MGMR_COEX_RF_COMBO;
    }
    if (board_modes & BOARD_RF_MODE_STANDALONE_BIT) {
        *modes |= 1u << WIFI_MGMR_COEX_RF_STANDALONE;
    }
    return WIFI_MGMR_COEX_OK;
}

static int platform_coex_rf_prepare(
    const struct wifi_mgmr_coex_board_config *board,
    enum wifi_mgmr_coex_rf_mode mode)
{
    uint32_t modes;
    int ret;

    if (mode != WIFI_MGMR_COEX_RF_COMBO && mode != WIFI_MGMR_COEX_RF_STANDALONE) {
        return WIFI_MGMR_COEX_ERR_INVALID_ARGUMENT;
    }
    ret = platform_coex_board_modes_get(board, &modes);
    if (ret != WIFI_MGMR_COEX_OK) {
        return ret;
    }
    if (!(modes & (1u << mode))) {
        return WIFI_MGMR_COEX_ERR_NOT_SUPPORTED;
    }
    switch (board->topology) {
    case WIFI_MGMR_COEX_BOARD_COMBO_SHARED_PATH:
        ret = board_rf_combo_init();
        break;
    case WIFI_MGMR_COEX_BOARD_STANDALONE_DUAL_ANT:
        ret = board_rf_dual_ant_init();
        break;
    case WIFI_MGMR_COEX_BOARD_STANDALONE_SINGLE_ANT_SPDT:
        ret = board_rf_single_ant_spdt_init(board->spdt_gpio);
        break;
    default:
        return WIFI_MGMR_COEX_ERR_INVALID_ARGUMENT;
    }
    return platform_coex_rf_result(ret);
}

static int platform_coex_rf_status_get(void)
{
    return platform_coex_rf_result(board_rf_init_status_get());
}

static int platform_coex_spdt_apply(uint8_t mode)
{
    switch (mode) {
    case COEXM_SPDT_FIXED_BT:
        return board_rf_spdt_mode_apply(BOARD_RF_SPDT_FIXED_BT);
    case COEXM_SPDT_DYNAMIC_PTA:
        return board_rf_spdt_mode_apply(BOARD_RF_SPDT_DYNAMIC_PTA);
    default:
        return -EINVAL;
    }
}

static int platform_coex_spdt_verify(uint8_t mode)
{
    switch (mode) {
    case COEXM_SPDT_FIXED_BT:
        return board_rf_spdt_mode_verify(BOARD_RF_SPDT_FIXED_BT);
    case COEXM_SPDT_DYNAMIC_PTA:
        return board_rf_spdt_mode_verify(BOARD_RF_SPDT_DYNAMIC_PTA);
    default:
        return -EINVAL;
    }
}

static const struct wifi_mgmr_coex_rf_ops platform_coex_rf_ops = {
    .board_modes_get = platform_coex_board_modes_get,
    .prepare = platform_coex_rf_prepare,
    .status_get = platform_coex_rf_status_get,
    .spdt = {
        .spdt_apply = platform_coex_spdt_apply,
        .spdt_verify = platform_coex_spdt_verify,
    },
};
#endif

const struct wifi_mgmr_coex_rf_ops *platform_coex_rf_ops_get(void)
{
#if defined(BL618DG) && defined(CONFIG_RF) && defined(CONFIG_FHOST) && \
    defined(BOARD_RF_COEX_OPS_SUPPORTED)
    return &platform_coex_rf_ops;
#else
    return NULL;
#endif
}

int platform_coex_spdt_debug_apply(enum wifi_mgmr_coex_spdt_debug_mode mode)
{
#if defined(BL618DG) && defined(CONFIG_RF) && defined(CONFIG_FHOST) && \
    defined(BOARD_RF_COEX_OPS_SUPPORTED) && defined(CONFIG_WIFI_COEX_DEBUG_CLI)
    switch (mode) {
    case WIFI_MGMR_COEX_SPDT_DEBUG_2G:
        return platform_coex_rf_result(board_rf_spdt_mode_apply(BOARD_RF_SPDT_FIXED_2G));
    case WIFI_MGMR_COEX_SPDT_DEBUG_BT:
        return platform_coex_rf_result(board_rf_spdt_mode_apply(BOARD_RF_SPDT_FIXED_BT));
#ifdef CONFIG_COEX_SPDT_EXPERIMENT
    case WIFI_MGMR_COEX_SPDT_DEBUG_PTA:
        /* Lab-only handover; caller already checked MACSW readiness/ownership. */
        if ((*(volatile uint32_t *)0x24920028u & (1u << 31)) == 0) {
            return WIFI_MGMR_COEX_ERR_NOT_READY;
        }
        return platform_coex_rf_result(board_rf_spdt_mode_apply(BOARD_RF_SPDT_DYNAMIC_PTA));
#endif
    default:
        break;
    }
#else
    (void)mode;
#endif
    return WIFI_MGMR_COEX_ERR_NOT_SUPPORTED;
}

#if defined(BL616)
#include <bl616_mfg_media.h>
#elif defined(BL616CL)
#include <bl616cl_mfg_media.h>
#elif defined(BL618DG)
#include <bl618dg_mfg_media.h>
#endif

#ifdef CONFIG_LPAPP
#include "bl_lp.h"
#include "macsw.h"
#endif

#ifdef CONFIG_WIFI_P2P
typedef char wifi_mgmr_p2p_event_header_size_check[
    (sizeof(wifi_mgmr_async_event_t) == sizeof(struct async_input_event)) ? 1 : -1];
#endif

/**
 ****************************************************************************************
 * @brief Get MAC Address
 *
 * @param[out] mac Buffer of hold mac address.
 * @return 0 on success and OTHERS if error occurred.
 ****************************************************************************************
 */

int platform_get_mac(uint8_t *mac)
{
    if (0 == mfg_media_read_macaddr_with_lock(mac, 1)) {
        return 0;
    }
    return -1;
}

int platform_get_random(unsigned char *buf, size_t len)
{
    int ret;

    taskENTER_CRITICAL();
    ret = bflb_trng_readlen(buf, len);
    taskEXIT_CRITICAL();
    return ret;
}

/**
****************************************************************************************
* @brief Wrapper of platform malloc/free
****************************************************************************************
*/
void *rtos_malloc(uint32_t size)
{
    void *ptr;

    ptr = kmalloc(size, 0);
    if (NULL == ptr) {
        abort();
    }
    return ptr;
}

void *rtos_calloc(uint32_t nb_elt, uint32_t size)
{
    void *ptr;

    ptr = kmalloc(nb_elt * size, MM_FLAG_PROP_ZERO);
    if (NULL == ptr) {
        abort();
    }
    return ptr;
}

void rtos_free(void *ptr)
{
    kfree(ptr);
}

void *platform_malloc(uint32_t size)
{
    void *ret = kmalloc(size, 0);
    // TODO sleep and retry able
    if (ret == NULL) {
        abort();
    }
    return ret;
}

void platform_free(void *mem_ptr)
{
    kfree(mem_ptr);
}

/* async event handler */
static void async_event_handler(void *arg1, uint32_t arg2)
{
    /* XXX: Prevents blocking operations in the Timer context. */
    vTaskSuspendAll();
    async_event_loop();
    xTaskResumeAll();
}

static void async_event_loop_wake(void)
{
    BaseType_t xReturn;
    TickType_t wait = portMAX_DELAY;

    if (xTimerGetTimerDaemonTaskHandle() == xTaskGetCurrentTaskHandle()) {
        wait = 0;
    }

    xReturn = xTimerPendFunctionCall(async_event_handler, (void *)NULL, 0, wait);
    configASSERT(xReturn == pdPASS);
}

#ifdef CONFIG_WIFI_P2P
static void p2p_event_copy_field(char *dst, size_t dst_len,
				 const char *src, size_t src_len)
{
	size_t len;

	if (!dst || dst_len == 0)
		return;
	dst[0] = '\0';
	if (!src)
		return;

	for (len = 0; len < src_len && src[len] != '\0'; len++)
		;
	if (len >= dst_len)
		len = dst_len - 1;
	memcpy(dst, src, len);
	dst[len] = '\0';
}

static void p2p_event_format_mac(char *dst, size_t dst_len,
				 const uint8_t *addr)
{
	if (!dst || dst_len < WIFI_MGMR_P2P_ADDR_STR_LEN || !addr)
		return;

	snprintf(dst, dst_len, "%02x:%02x:%02x:%02x:%02x:%02x",
		 addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

static void p2p_event_encode_ssid(char *dst, size_t dst_len,
				  const uint8_t *ssid, size_t ssid_len)
{
	static const char hex[] = "0123456789abcdef";
	size_t i;
	size_t pos = 0;

	if (!dst || dst_len == 0)
		return;

	for (i = 0; i < ssid_len && pos + 4 < dst_len; i++) {
		uint8_t c = ssid[i];

		switch (c) {
		case '"':
		case '\\':
			dst[pos++] = '\\';
			dst[pos++] = c;
			break;
		case 0x1b:
			dst[pos++] = '\\';
			dst[pos++] = 'e';
			break;
		case '\n':
			dst[pos++] = '\\';
			dst[pos++] = 'n';
			break;
		case '\r':
			dst[pos++] = '\\';
			dst[pos++] = 'r';
			break;
		case '\t':
			dst[pos++] = '\\';
			dst[pos++] = 't';
			break;
		default:
			if (c >= 32 && c <= 126) {
				dst[pos++] = c;
			} else {
				dst[pos++] = '\\';
				dst[pos++] = 'x';
				dst[pos++] = hex[c >> 4];
				dst[pos++] = hex[c & 0x0f];
			}
			break;
		}
	}
	dst[pos] = '\0';
}

static const char *p2p_event_role_name(uint8_t role)
{
	switch (role) {
	case CFGMACSW_P2P_ROLE_GO:
		return "GO";
	case CFGMACSW_P2P_ROLE_CLIENT:
		return "client";
	default:
		return "";
	}
}

static const char *p2p_event_wps_method_name(uint8_t method)
{
	switch (method) {
	case CFGMACSW_P2P_WPS_METHOD_PIN_DISPLAY:
		return "Display";
	case CFGMACSW_P2P_WPS_METHOD_PIN_KEYPAD:
		return "Keypad";
	case CFGMACSW_P2P_WPS_METHOD_PBC:
		return "PBC";
	case CFGMACSW_P2P_WPS_METHOD_NFC:
		return "NFC";
	case CFGMACSW_P2P_WPS_METHOD_P2PS:
		return "P2PS";
	default:
		return "";
	}
}

static void platform_p2p_event_fill(wifi_mgmr_p2p_event_t *dst, int code,
				    const struct cfgmacsw_p2p_event *src)
{
	uint32_t valid;

	if (!dst || !src)
		return;

	memset(dst, 0, sizeof(*dst));
	dst->event.size = sizeof(*dst);
	dst->event.type = EV_WIFI;
	dst->event.code = (uint16_t)code;
	dst->event.value = (unsigned long)src->fhost_vif_idx;
	dst->vif_idx = (uint8_t)src->fhost_vif_idx;
	valid = src->valid_fields;

	if (valid & CFGMACSW_P2P_VALID_PEER_ADDR)
		p2p_event_format_mac(dst->peer_addr, sizeof(dst->peer_addr),
				     src->peer_addr);
	if (valid & CFGMACSW_P2P_VALID_PEER_IFACE_ADDR)
		p2p_event_format_mac(dst->peer_iface_addr,
				     sizeof(dst->peer_iface_addr),
				     src->peer_iface_addr);
	if (valid & CFGMACSW_P2P_VALID_GO_DEV_ADDR)
		p2p_event_format_mac(dst->go_dev_addr, sizeof(dst->go_dev_addr),
				     src->go_dev_addr);
	if (valid & CFGMACSW_P2P_VALID_BSSID)
		p2p_event_format_mac(dst->bssid, sizeof(dst->bssid), src->bssid);
	if (valid & CFGMACSW_P2P_VALID_ROLE) {
		dst->role = src->role == CFGMACSW_P2P_ROLE_GO ?
			WIFI_MGMR_P2P_ROLE_GO :
			src->role == CFGMACSW_P2P_ROLE_CLIENT ?
			WIFI_MGMR_P2P_ROLE_CLIENT : WIFI_MGMR_P2P_ROLE_UNKNOWN;
		p2p_event_copy_field(dst->role_name, sizeof(dst->role_name),
				     p2p_event_role_name(src->role),
				     strlen(p2p_event_role_name(src->role)) + 1);
	}
	if (valid & CFGMACSW_P2P_VALID_FREQ) {
		dst->freq = src->freq;
		dst->has_freq = 1;
	}
	if (valid & CFGMACSW_P2P_VALID_STATUS) {
		dst->status = src->status;
		dst->has_status = 1;
	}
	if (valid & CFGMACSW_P2P_VALID_PERSISTENT_ID) {
		dst->persistent_id = src->persistent_id;
		dst->has_persistent_id = 1;
	}
	if (valid & CFGMACSW_P2P_VALID_PERSISTENT) {
		dst->persistent = src->persistent;
		dst->has_persistent = 1;
	}
	if (valid & CFGMACSW_P2P_VALID_CONFIG_METHODS) {
		dst->config_methods = src->config_methods;
		dst->has_config_methods = 1;
	}
	if (valid & CFGMACSW_P2P_VALID_DEV_CAPAB) {
		dst->dev_capab = src->dev_capab;
		dst->has_dev_capab = 1;
	}
	if (valid & CFGMACSW_P2P_VALID_GROUP_CAPAB) {
		dst->group_capab = src->group_capab;
		dst->has_group_capab = 1;
	}
	if (valid & CFGMACSW_P2P_VALID_LEVEL) {
		dst->level = src->level;
		dst->has_level = 1;
	}
	if (valid & CFGMACSW_P2P_VALID_GO_INTENT) {
		dst->go_intent = src->go_intent;
		dst->has_go_intent = 1;
	}
	if (valid & CFGMACSW_P2P_VALID_DEV_PASSWD_ID) {
		dst->dev_passwd_id = src->dev_passwd_id;
		dst->has_dev_passwd_id = 1;
	}
	if (valid & CFGMACSW_P2P_VALID_WPS_METHOD)
		p2p_event_copy_field(dst->wps_method, sizeof(dst->wps_method),
				     p2p_event_wps_method_name(src->wps_method),
				     strlen(p2p_event_wps_method_name(src->wps_method)) + 1);

	p2p_event_copy_field(dst->ifname, sizeof(dst->ifname), src->ifname,
			     sizeof(src->ifname));
	p2p_event_copy_field(dst->device_name, sizeof(dst->device_name),
			     src->device_name, sizeof(src->device_name));
	p2p_event_copy_field(dst->pin, sizeof(dst->pin), src->pin,
			     sizeof(src->pin));
	p2p_event_copy_field(dst->reason, sizeof(dst->reason), src->reason,
			     sizeof(src->reason));
	p2p_event_encode_ssid(dst->ssid, sizeof(dst->ssid), src->ssid,
			      src->ssid_len <= sizeof(src->ssid) ?
			      src->ssid_len : sizeof(src->ssid));
}

#endif /* CONFIG_WIFI_P2P */

/**
****************************************************************************************
* @brief Post Event to upper layer
*
* @param[in] catalogue Type of event.
* @param[in] code Code of event.
****************************************************************************************
*/

void platform_post_event(int catalogue, int code1, int code2)
{
    async_event_init(async_event_loop_wake);
    async_post_event(EV_WIFI, code1, code2);
}

#ifdef CONFIG_WIFI_P2P
void platform_post_p2p_event(int code,
			     const struct cfgmacsw_p2p_event *event)
{
	wifi_mgmr_p2p_event_t evt;

	if (!event)
		return;
	platform_p2p_event_fill(&evt, code, event);

	async_event_init(async_event_loop_wake);
	if (async_post_general_event((async_input_event_t)&evt)) {
		async_post_event(EV_WIFI, code, event->fhost_vif_idx);
	}
}
#endif /* CONFIG_WIFI_P2P */

#ifdef CFG_LPM
uint8_t platform_get_chip_version(void)
{
    bflb_efuse_device_info_type device_info;

    bflb_efuse_get_device_info(&device_info);
    return device_info.version;
}
#endif
