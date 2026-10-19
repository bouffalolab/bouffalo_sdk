#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include <stdint.h>
#include <lwip/tcpip.h>
#include <lwip/pbuf.h>
#include <lwip/netif.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include <string.h>

#include "pm_manager.h"

#include "bl_lp.h"
#include "bflb_common.h"
#include "lpfw_bcmc_mailbox.h"
#include <wifi_mgmr_ext.h>

#if defined(CONFIG_DUAL_STACK) || (defined(CONFIG_LWIP_ONHOST_ENABLE) && CONFIG_LWIP_ONHOST_ENABLE)
#include <dual_stack_input.h>
#define PM_BCMC_USE_DUAL_STACK_INPUT 1
#else
#define PM_BCMC_USE_DUAL_STACK_INPUT 0
#endif

#if defined(CONFIG_DUAL_STACK)
#include <wifi_pkt_hooks.h>
#define PM_BCMC_USE_WIFI_INPUT_HOOK 1
#else
#define PM_BCMC_USE_WIFI_INPUT_HOOK 0
#endif

#define PM_BCMC_PAYLOAD_SIZE LPFW_BCMC_PAYLOAD_SIZE
#define PM_BCMC_POOL_SIZE    LPFW_BCMC_MAILBOX_POOL_SIZE
#define PM_BCMC_ETH_ADDR_LEN 6
#define PM_BCMC_ETH_HDR_LEN  14

static struct lpfw_bcmc_mailbox *pm_mailbox;
static uint8_t pm_free_after_bcmc_process;
void *pm_pbuf = NULL;
int enable_multicast_broadcast = 0;
TaskHandle_t rxl_process_task_hd = NULL;
extern bl_lp_fw_cfg_t lpfw_cfg;
extern int tickless_enter(void);
extern int tickless_exit(void);

static pm_sleep_check_entry_t pm_sleep_check_registry[PM_SLEEP_CHECK_MAX_CALLBACKS];
static uint8_t pm_sleep_check_count = 0;

static int pm_bcmc_mailbox_is_valid(const struct lpfw_bcmc_mailbox *mailbox)
{
    if (!mailbox) {
        return 0;
    }

    if (mailbox->magic != LPFW_BCMC_MAILBOX_MAGIC ||
        mailbox->version != LPFW_BCMC_MAILBOX_VERSION ||
        mailbox->header_size != LPFW_BCMC_MAILBOX_HEADER_SIZE(LPFW_BCMC_DESC_CAPACITY) ||
        mailbox->desc_capacity != LPFW_BCMC_DESC_CAPACITY ||
        mailbox->payload_size != PM_BCMC_PAYLOAD_SIZE ||
        mailbox->packet_count > mailbox->desc_capacity ||
        mailbox->payload_used > mailbox->payload_size) {
        return 0;
    }

    return 1;
}

static void pm_bcmc_mailbox_init(struct lpfw_bcmc_mailbox *mailbox)
{
    if (!mailbox) {
        return;
    }

    memset(mailbox, 0, PM_BCMC_POOL_SIZE);
    mailbox->magic = LPFW_BCMC_MAILBOX_MAGIC;
    mailbox->version = LPFW_BCMC_MAILBOX_VERSION;
    mailbox->header_size = LPFW_BCMC_MAILBOX_HEADER_SIZE(LPFW_BCMC_DESC_CAPACITY);
    mailbox->desc_capacity = LPFW_BCMC_DESC_CAPACITY;
    mailbox->payload_size = PM_BCMC_PAYLOAD_SIZE;
}

static void pm_bcmc_mailbox_reset(void)
{
    if (!pm_bcmc_mailbox_is_valid(pm_mailbox)) {
        pm_bcmc_mailbox_init(pm_mailbox);
        return;
    }

    pm_mailbox->packet_count = 0;
    pm_mailbox->payload_used = 0;
    pm_mailbox->last_error = LPFW_BCMC_ERR_NONE;
    pm_mailbox->drop_count = 0;
}

static int pm_bcmc_mailbox_has_pending(void)
{
    return pm_bcmc_mailbox_is_valid(pm_mailbox) &&
           (pm_mailbox->packet_count > 0 || pm_mailbox->drop_count > 0);
}

static int pm_bcmc_eth_frame_sanity_check(const uint8_t *frame, uint16_t len)
{
    if (!frame) {
        return 0;
    }

    if (len < PM_BCMC_ETH_HDR_LEN || len > LPFW_BCMC_FRAME_SIZE) {
        return 0;
    }

    if (!(frame[0] & 0x01)) {
        return 0;
    }

    if (frame[PM_BCMC_ETH_ADDR_LEN] & 0x01) {
        return 0;
    }

    return 1;
}

static int pm_bcmc_netif_ready(const struct netif *nif)
{
    return nif &&
           nif->input &&
           (nif->flags & NETIF_FLAG_LINK_UP);
}

#if !PM_BCMC_USE_DUAL_STACK_INPUT
static int pm_bcmc_frame_from_self(const uint8_t *frame, const struct netif *nif)
{
    if (!frame || !nif) {
        return 0;
    }

    if (nif->hwaddr_len >= PM_BCMC_ETH_ADDR_LEN &&
        memcmp(frame + PM_BCMC_ETH_ADDR_LEN, nif->hwaddr, PM_BCMC_ETH_ADDR_LEN) == 0) {
        return 1;
    }

    return 0;
}
#endif

void set_dtim_config(int dtim)
{
    int listen_interval = 1;

    if (dtim == 0) {
        listen_interval = 1;
    } else {
        listen_interval = dtim;
    }
    lpfw_cfg.dtim_origin = listen_interval;
}

uint8_t lp_interval_get(void)
{
    return lpfw_cfg.dtim_num;
}

int pm_mem_pool_alloc(void)
{
    void *addr;

    if (pm_pbuf) {
        if (pm_free_after_bcmc_process) {
            printf("[PM] BCMC pool free pending, disable BCMC capture for this sleep\r\n");
            return -1;
        }
        if (pm_bcmc_mailbox_has_pending()) {
            printf("[PM] BCMC mailbox pending packets=%u drops=%u, disable BCMC capture for this sleep\r\n",
                   (unsigned int)pm_mailbox->packet_count,
                   (unsigned int)pm_mailbox->drop_count);
            return -1;
        }
        pm_bcmc_mailbox_init(pm_mailbox);
        return 0;
    }

    addr = pvPortMalloc(PM_BCMC_POOL_SIZE);
    if (!addr) {
        pm_mailbox = NULL;
        printf("[PM] Failed to alloc BCMC pool size=%u\r\n", (unsigned int)PM_BCMC_POOL_SIZE);
        return -1;
    }

    pm_pbuf = addr;
    pm_mailbox = (struct lpfw_bcmc_mailbox *)bflb_get_no_cache_addr(addr);
    pm_bcmc_mailbox_init(pm_mailbox);
    pm_free_after_bcmc_process = 0;

    return 0;
}

static void pm_mem_pool_free_now(void)
{
    if (pm_pbuf) {
        vPortFree(pm_pbuf);
        pm_pbuf = NULL;
    }

    pm_mailbox = NULL;
    pm_free_after_bcmc_process = 0;
}

int pm_mem_pool_free(void)
{
    if (pm_bcmc_mailbox_has_pending()) {
        pm_free_after_bcmc_process = 1;
        printf("[PM] BCMC mailbox pending packets=%u drops=%u, delay pool free\r\n",
               (unsigned int)pm_mailbox->packet_count,
               (unsigned int)pm_mailbox->drop_count);
        if (rxl_process_task_hd) {
            xTaskNotifyGive(rxl_process_task_hd);
        }
        return 0;
    }

    pm_mem_pool_free_now();

    return 0;
}

uint32_t pm_manager_handle_get(void)
{
    uint32_t addr = (uint32_t)(uintptr_t)pm_mailbox;

    if ((addr & 0xF0000000) == 0x60000000) {
        addr = (addr & 0x0FFFFFFF) | 0x20000000;
    }

    return addr;
}

int pm_pbufc_check(void)
{
    return pm_bcmc_mailbox_has_pending();
}

int pm_enter_lp_perparation(void)
{
    int ret = 0;
    if (enable_multicast_broadcast) {
        ret = pm_mem_pool_alloc();

        if (!ret) {
            lpfw_cfg.buf_addr = pm_manager_handle_get();
            lpfw_cfg.bcmc_dtim_mode = 1;
        } else {
            lpfw_cfg.buf_addr = 0;
            lpfw_cfg.bcmc_dtim_mode = 0;
        }
    } else {
        lpfw_cfg.buf_addr = 0;
        lpfw_cfg.bcmc_dtim_mode = 0;
    }

    bl_lp_fw_bcn_loss_cfg_dtim_default(lpfw_cfg.dtim_origin);

    if (wifi_mgmr_sta_state_get()) {
        wifi_mgmr_sta_ps_enter();
    }

    return ret;
}

int pm_exit_lp_perparation(void)
{
    pm_mem_pool_free();
    lpfw_cfg.buf_addr = 0;

    if (wifi_mgmr_sta_state_get()) {
        wifi_mgmr_sta_ps_exit();
    }

    return 0;
}

int pm_enable_tickless(void)
{
    pm_enter_lp_perparation();

    tickless_enter();

    return 0;
}

int pm_disable_tickless(void)
{
    pm_exit_lp_perparation();

    tickless_exit();

    return 0;
}

int pm_status_update(int status)
{
    //update status
    if (status) {
        pm_disable_tickless();
    } else {
        pm_enable_tickless();
    }

    return 0;
}

static void process_multicast_broadcast(void *pvParameters)
{
    (void)pvParameters;

    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (!pm_bcmc_mailbox_is_valid(pm_mailbox)) {
            if (pm_free_after_bcmc_process) {
                pm_mem_pool_free_now();
            } else {
                pm_bcmc_mailbox_reset();
            }
            continue;
        }

        uint8_t *payload_base = (uint8_t *)pm_mailbox + pm_mailbox->header_size;
        uint16_t packet_count = pm_mailbox->packet_count;
        uint16_t last_error = pm_mailbox->last_error;
        uint16_t drop_count = pm_mailbox->drop_count;
        uint16_t invalid_count = 0;

        for (uint16_t i = 0; i < packet_count; i++) {
            const struct lpfw_bcmc_packet_desc *desc = &pm_mailbox->desc[i];
            uint32_t offset = desc->offset;
            uint16_t len = desc->len;
            uint8_t *frame;

            if (len == 0 ||
                offset > pm_mailbox->payload_used ||
                len > (pm_mailbox->payload_used - offset)) {
                printf("[RECV_TASK] Invalid BCMC packet desc idx=%u offset=%lu len=%u\r\n",
                       i, (unsigned long)offset, len);
                invalid_count++;
                continue;
            }

            frame = payload_base + offset;
            if (!pm_bcmc_eth_frame_sanity_check(frame, len)) {
                invalid_count++;
                continue;
            }

#if !PM_BCMC_USE_DUAL_STACK_INPUT
            struct netif *nif = netif_default;
            if (!pm_bcmc_netif_ready(nif) || pm_bcmc_frame_from_self(frame, nif)) {
                invalid_count++;
                continue;
            }
#endif

            struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
            if (!p) {
                printf("[RECV_TASK] Failed to alloc pbuf len=%u\r\n", len);
                continue;
            }

            if (pbuf_take(p, frame, len) != ERR_OK) {
                printf("[RECV_TASK] Failed to copy BCMC packet len=%u\r\n", len);
                pbuf_free(p);
                continue;
            }

#if PM_BCMC_USE_DUAL_STACK_INPUT
#if PM_BCMC_USE_WIFI_INPUT_HOOK
            if (bl_wifi_pkt_eth_input_hook) {
                struct pbuf *hook_p;
                struct netif *nif;
                err_t ret;

                /*
                 * Host-router may consume the pbuf for host delivery and return NULL,
                 * or return a pbuf that still needs to enter the embedded stack.
                 */
                hook_p = bl_wifi_pkt_eth_input_hook(true, p, bl_wifi_pkt_eth_input_hook_arg);
                if (!hook_p) {
                    continue;
                }

                nif = netif_default;
                if (!pm_bcmc_netif_ready(nif)) {
                    pbuf_free(hook_p);
                    invalid_count++;
                    continue;
                }

                ret = nif->input(hook_p, nif);
                if (ret != ERR_OK) {
                    printf("[RECV_TASK] Failed to input pbuf to netif\r\n");
                    pbuf_free(hook_p);
                }
                continue;
            }
#endif
            if (dual_stack_input(p, NULL)) {
                pbuf_free(p);
            }
#else
            err_t ret = nif->input(p, nif);
            if (ret != ERR_OK) {
                printf("[RECV_TASK] Failed to input pbuf to netif\r\n");
                pbuf_free(p);
            }
#endif
        }

        if (drop_count > 0) {
            printf("[RECV_TASK] BCMC truncated drop_count=%u last_error=%u packet_count=%u payload_used=%lu payload_size=%lu\r\n",
                   (unsigned int)drop_count, (unsigned int)last_error, (unsigned int)packet_count,
                   (unsigned long)pm_mailbox->payload_used,
                   (unsigned long)pm_mailbox->payload_size);
        }

        if (invalid_count > 0) {
            printf("[RECV_TASK] BCMC invalid frame count=%u\r\n", (unsigned int)invalid_count);
        }

        pm_bcmc_mailbox_reset();
        if (pm_free_after_bcmc_process) {
            pm_mem_pool_free_now();
        }
    }

    vTaskDelete(NULL);
}

int pm_alloc_mem_reset(void)
{
    pm_bcmc_mailbox_reset();

    return 0;
}

int pm_sys_init(void)
{
    pm_mailbox = NULL;

    //wifi_mgmr_sta_ps_status_register(tickless_handke_get());

    lpfw_cfg.bcmc_dtim_mode = 0;
    lpfw_cfg.dtim_origin = 1;

    printf("[OS] Starting process_multicase_broadcast task...\r\n");
    xTaskCreate(process_multicast_broadcast, (char *)"bcmc_rx", 384, NULL, 10, &rxl_process_task_hd);

    return 0;
}

int pm_sleep_check_register(const char *name, pm_sleep_check_cb_t cb, uint8_t priority)
{
    if (name == NULL || cb == NULL) {
        return -1;
    }

    for (uint8_t i = 0; i < pm_sleep_check_count; i++) {
        if (pm_sleep_check_registry[i].enabled &&
            pm_sleep_check_registry[i].name != NULL &&
            strcmp(pm_sleep_check_registry[i].name, name) == 0) {
            return -2;
        }
    }

    if (pm_sleep_check_count >= PM_SLEEP_CHECK_MAX_CALLBACKS) {
        return -1;
    }

    uint8_t insert_idx = pm_sleep_check_count;
    for (uint8_t i = 0; i < pm_sleep_check_count; i++) {
        if (priority < pm_sleep_check_registry[i].priority) {
            insert_idx = i;
            break;
        }
    }

    for (uint8_t i = pm_sleep_check_count; i > insert_idx; i--) {
        pm_sleep_check_registry[i] = pm_sleep_check_registry[i - 1];
    }

    pm_sleep_check_registry[insert_idx].name = name;
    pm_sleep_check_registry[insert_idx].cb = cb;
    pm_sleep_check_registry[insert_idx].priority = priority;
    pm_sleep_check_registry[insert_idx].enabled = 1;
    pm_sleep_check_count++;

    return 0;
}

int pm_sleep_check_unregister(const char *name)
{
    if (name == NULL) {
        return -1;
    }

    for (uint8_t i = 0; i < pm_sleep_check_count; i++) {
        if (pm_sleep_check_registry[i].enabled &&
            pm_sleep_check_registry[i].name != NULL &&
            strcmp(pm_sleep_check_registry[i].name, name) == 0) {
            for (uint8_t j = i; j < pm_sleep_check_count - 1; j++) {
                pm_sleep_check_registry[j] = pm_sleep_check_registry[j + 1];
            }

            pm_sleep_check_registry[pm_sleep_check_count - 1].name = NULL;
            pm_sleep_check_registry[pm_sleep_check_count - 1].cb = NULL;
            pm_sleep_check_registry[pm_sleep_check_count - 1].priority = 0;
            pm_sleep_check_registry[pm_sleep_check_count - 1].enabled = 0;
            pm_sleep_check_count--;
            return 0;
        }
    }

    return -1;
}

int pm_sleep_check_dispatch(void)
{
    int ret;

    if (pm_sleep_check_count == 0) {
        return 0;
    }

    for (uint8_t i = 0; i < pm_sleep_check_count; i++) {
        if (pm_sleep_check_registry[i].enabled && pm_sleep_check_registry[i].cb != NULL) {
            ret = pm_sleep_check_registry[i].cb();
            if (ret != 0) {
#if 0
                printf("[PM] Sleep prevented by '%s' (ret=%d)\r\n",
                       pm_sleep_check_registry[i].name ? pm_sleep_check_registry[i].name : "unknown",
                       ret);
#endif
                return ret;
            }
        }
    }

    return 0;
}
