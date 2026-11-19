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
#include "wifi6_chip.h"

#ifdef WIFI6_MFG_MEDIA_HEADER
#include WIFI6_MFG_MEDIA_HEADER
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "lwip/tcpip.h"
#include "rfparam_adapter.h"
#include "rfparam_adapter_internal.h"
#include "sysinit.h"

/* Real startup entry points provided by the firmware and FHOST components. */
void _wifi_task_create(void);
int _fhost_init(void);
int _wifi_mgmr_task_start(void);

/* Chip-specific RF bring-up, provided by the linked platform_chip_xxxx.c. */
extern int platform_chip_rf_init(void);

#define DBG_TAG "WIFI_INIT"
#include "log.h"

#include "macsw.h"

#include <bflb_efuse.h>
#include <bflb_sec_trng.h>
#include "mm.h"

#ifdef CONFIG_LPAPP
#include "bl_lp.h"
#endif

#ifdef CONFIG_WIFI_P2P
typedef char
    wifi_mgmr_p2p_event_header_size_check[(sizeof(wifi_mgmr_async_event_t) == sizeof(struct async_input_event)) ? 1 :
                                                                                                                  -1];
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

static bool wifi_firmware_ready;
static bool wifi_manager_ready;

static void wifi_startup_event_handler(async_input_event_t event, void *priv)
{
    (void)priv;

    if (event->code == CODE_WIFI_ON_INIT_DONE_SYS) {
        __atomic_store_n(&wifi_firmware_ready, true, __ATOMIC_RELEASE);
    }
    if (event->code == CODE_WIFI_ON_MGMR_DONE_SYS) {
        __atomic_store_n(&wifi_manager_ready, true, __ATOMIC_RELEASE);
    }
}

static void wifi_startup_events(const void *arg)
{
    (void)arg;
    /* The async event loop must be ready before the synchronous Wi-Fi
     * bring-up and the application main(). */
    async_event_init(async_event_loop_wake);
    if (async_register_event_filter(EV_WIFI, wifi_startup_event_handler, NULL) != 0) {
        abort();
    }
}
SYSINIT(wifi_events, SI_SUB_EVENTS, SI_ORDER_FIRST, wifi_startup_events, NULL);

/**
 ****************************************************************************************
 * @brief Program the PHY through the SDK RF parameter driver.
 *
 * Default RF bring-up shared by the chip adapters. An RF configuration that is
 * already published is reused instead of initializing the PHY twice.
 *
 * @return 0 on success and a negative value if initialization fails.
 ****************************************************************************************
 */
int platform_rf_param_init(void)
{
    if (rfparam_cfg_get() != NULL) {
        return 0;
    }
    if (_rfparam_init(0, NULL, 0) != 0) {
        LOG_E("PHY RF init failed!\r\n");
        return -1;
    }
    return 0;
}

static void wifi_startup_rf_init(void)
{
    /* Chip-specific RF bring-up lives in platform_chip_xxxx.c. */
    if (platform_chip_rf_init() != 0) {
        abort();
    }
}

static void wifi_startup_before_firmware_config(void)
{
    /* Firmware-before HT40 policy for the WFA certification cases: the WFA
     * project's Kconfig selects CONFIG_WIFI6_HT40_ENABLE and the policy is
     * applied here after RF bring-up, before the firmware/manager task exists.
     * It used to be applied by the application before it created the Wi-Fi
     * task; SYSINIT owns that startup now, so the project option drives it.
     * BL618DG keeps its built-in 40 MHz default when the option is off. */
#if IS_ENABLED(CONFIG_WIFI6_HT40_ENABLE)
    if (wifi_mgmr_set_ht40_enable(1) != 0) {
        LOG_D("HT40 enable request not applied by the PHY\r\n");
    } else {
        LOG_I("HT40 default: enabled\r\n");
    }
#endif
}

#ifndef WIFI6_WIFI_STARTUP_PRIORITY
#define WIFI6_WIFI_STARTUP_PRIORITY 10
#endif
#ifndef WIFI6_FHOST_INIT_DELAY_MS
#define WIFI6_FHOST_INIT_DELAY_MS 300
#endif
#ifndef WIFI6_APP_EVENTS_PRIORITY
#define WIFI6_APP_EVENTS_PRIORITY 1
#endif

/* FHOST uses the same startup scheme as WL80211: the SYSINIT continuation task
 * runs at configMAX_PRIORITIES - 1, above the Wi-Fi firmware task (27), so it
 * is temporarily lowered before _wifi_task_create() to let the firmware finish
 * macswl_init()/ke_init() before the host sends the first KE message. Unlike
 * WL80211, the FHOST control task and the manager task are asynchronous, so the
 * internal _SYS readiness events are still used to wait for those two stages. */
#if !defined(INCLUDE_vTaskPrioritySet) || (INCLUDE_vTaskPrioritySet != 1)
#error "FHOST SYSINIT bring-up requires INCLUDE_vTaskPrioritySet = 1 in FreeRTOSConfig.h"
#endif

static void wifi_startup_sysinit(const void *arg)
{
    (void)arg;

    /* Runs synchronously before the application main(); the async event loop is
     * already initialized (SI_SUB_EVENTS). */
    wifi_startup_rf_init();
    wifi_startup_before_firmware_config();

    /* The continuation task runs at configMAX_PRIORITIES - 1, above the Wi-Fi
     * firmware task (27). Lower it so the firmware preempts the SYSINIT task
     * right after _wifi_task_create() and finishes macswl_init()/ke_init()
     * before the host starts fhost/manager communication. */
    vTaskPrioritySet(NULL, WIFI6_WIFI_STARTUP_PRIORITY);

    LOG_I("Starting wifi ...\r\n");
    _wifi_task_create();
    /* The SDK owns TCP/IP initialization; WIRELESS runs before SI_SUB_APP. */
    _tcpip_init(NULL, NULL);

    /* Give the firmware task time to finish its early MACSW init before fhost
     * opens the control channel. */
    vTaskDelay(pdMS_TO_TICKS(WIFI6_FHOST_INIT_DELAY_MS));

    if (_fhost_init() != 0) {
        abort();
    }

    while (!__atomic_load_n(&wifi_firmware_ready, __ATOMIC_ACQUIRE)) {
        vTaskDelay(1);
    }
    /* Manager initialization must stay outside the async event dispatcher. */
    if (_wifi_mgmr_task_start() != 0) {
        abort();
    }
    /* Wait for the manager so main() sees a fully initialized Wi-Fi stack. */
    while (!__atomic_load_n(&wifi_manager_ready, __ATOMIC_ACQUIRE)) {
        vTaskDelay(1);
    }

    /* Back to the priority used by sysinit_freertos.c for the continuation
     * task; the remaining WIRELESS/NET/APP records must not be preempted by
     * application tasks. */
    vTaskPrioritySet(NULL, configMAX_PRIORITIES - 1);
}
SYSINIT(wifi_start, SI_SUB_WIRELESS, SI_ORDER_FIRST, wifi_startup_sysinit, NULL);

/* Application-facing compatibility events. This record runs after the APP
 * main() record (SI_SUB_APP/MIDDLE), so application filters and tasks created
 * by main() exist. The SYSINIT task is temporarily lowered to let application
 * tasks run while the events are queued, then restored for the remaining
 * records. The events stay isolated from the internal CODE_WIFI_ON_*_SYS
 * events. */
static void wifi_app_events(const void *arg)
{
    (void)arg;

    vTaskPrioritySet(NULL, WIFI6_APP_EVENTS_PRIORITY);
    if (async_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0) != 0 ||
        async_post_event(EV_WIFI, CODE_WIFI_ON_MGMR_DONE, 0) != 0) {
        abort();
    }
    vTaskPrioritySet(NULL, configMAX_PRIORITIES - 1);
}
SYSINIT(wifi_app_events, SI_SUB_APP, SI_ORDER_LAST, wifi_app_events, NULL);

#ifdef CONFIG_WIFI_P2P
static void p2p_event_copy_field(char *dst, size_t dst_len, const char *src, size_t src_len)
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

static void p2p_event_format_mac(char *dst, size_t dst_len, const uint8_t *addr)
{
    if (!dst || dst_len < WIFI_MGMR_P2P_ADDR_STR_LEN || !addr)
        return;

    snprintf(dst, dst_len, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

static void p2p_event_encode_ssid(char *dst, size_t dst_len, const uint8_t *ssid, size_t ssid_len)
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

static void platform_p2p_event_fill(wifi_mgmr_p2p_event_t *dst, int code, const struct cfgmacsw_p2p_event *src)
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
        p2p_event_format_mac(dst->peer_addr, sizeof(dst->peer_addr), src->peer_addr);
    if (valid & CFGMACSW_P2P_VALID_PEER_IFACE_ADDR)
        p2p_event_format_mac(dst->peer_iface_addr, sizeof(dst->peer_iface_addr), src->peer_iface_addr);
    if (valid & CFGMACSW_P2P_VALID_GO_DEV_ADDR)
        p2p_event_format_mac(dst->go_dev_addr, sizeof(dst->go_dev_addr), src->go_dev_addr);
    if (valid & CFGMACSW_P2P_VALID_BSSID)
        p2p_event_format_mac(dst->bssid, sizeof(dst->bssid), src->bssid);
    if (valid & CFGMACSW_P2P_VALID_ROLE) {
        dst->role = src->role == CFGMACSW_P2P_ROLE_GO     ? WIFI_MGMR_P2P_ROLE_GO :
                    src->role == CFGMACSW_P2P_ROLE_CLIENT ? WIFI_MGMR_P2P_ROLE_CLIENT :
                                                            WIFI_MGMR_P2P_ROLE_UNKNOWN;
        p2p_event_copy_field(dst->role_name, sizeof(dst->role_name), p2p_event_role_name(src->role),
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
        p2p_event_copy_field(dst->wps_method, sizeof(dst->wps_method), p2p_event_wps_method_name(src->wps_method),
                             strlen(p2p_event_wps_method_name(src->wps_method)) + 1);

    p2p_event_copy_field(dst->ifname, sizeof(dst->ifname), src->ifname, sizeof(src->ifname));
    p2p_event_copy_field(dst->device_name, sizeof(dst->device_name), src->device_name, sizeof(src->device_name));
    p2p_event_copy_field(dst->pin, sizeof(dst->pin), src->pin, sizeof(src->pin));
    p2p_event_copy_field(dst->reason, sizeof(dst->reason), src->reason, sizeof(src->reason));
    p2p_event_encode_ssid(dst->ssid, sizeof(dst->ssid), src->ssid,
                          src->ssid_len <= sizeof(src->ssid) ? src->ssid_len : sizeof(src->ssid));
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
void platform_post_p2p_event(int code, const struct cfgmacsw_p2p_event *event)
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
