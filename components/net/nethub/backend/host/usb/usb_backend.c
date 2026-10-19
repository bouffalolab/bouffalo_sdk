#include "usb_backend.h"

#include "nethub_usb_device.h"

#define DBG_TAG "NETHUB_USB"
#include "log.h"

#include "nh_hub.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "compiler/compiler_ld.h"
#include "FreeRTOS.h"
#if defined(CONFIG_NETHUB_PROFILE_DUAL)
#include "nh_host_select.h"
#endif
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "task.h"

#include "lwip/pbuf.h"
#include "usbd_core.h"

extern int wifi_mgmr_sta_mac_get(uint8_t mac[6]);

#if defined(CONFIG_NETHUB_PROFILE_SDIO) && defined(CONFIG_NETHUB_PROFILE_USB)
#define NH_USB_RX_SLOT_CNT 2U
#else
#define NH_USB_RX_SLOT_CNT 8U
#endif
#define NH_USB_TX_QUEUE_DEPTH            8U
#define NH_USB_FRAME_MAX_LEN             (1536U)
#define NH_USB_ACM_STREAM_SIZE           2048U
#define NH_USB_ACM_IO_BUFFER_SIZE        512U
#define NH_USB_ACM_DISPATCH_SIZE         256U
#define NH_USB_LINK_SPEED_BPS            (100U * 1000U * 1000U)
#define NH_USB_WAIT_TIMEOUT_MS           1000U
#define NH_USB_IDLE_POLL_MS              20U
#define NH_USB_TASK_STACK_SIZE           1024U
#define NH_USB_TASK_PRIORITY             25U
#define NH_USB_NOTIFY_ALL_BITS           0xFFFFFFFFUL
#define NH_USB_EVENT_RX_DONE             (1UL << 0)
#define NH_USB_EVENT_CONFIG_CHANGE       (1UL << 1)
#define NH_USB_EVENT_TX_DONE             (1UL << 0)
#define NH_USB_EVENT_ACTIVE              (1UL << 1)

typedef struct {
    /*
     * The Wi-Fi bridge reuses frame->cb_arg as an in-place custom pbuf
     * descriptor for host->air forwarding, so keep this header layout
     * compatible with the descriptor it builds in nh_wifi_bridge.c.
     */
    struct pbuf_custom pbuf;
    void (*free_cb)(void *arg);
    void *cb_arg;
    uint8_t *buffer;
} nh_usb_rx_slot_t;

typedef struct {
    uint8_t *data;
    uint32_t len;
    void (*free_cb)(void *arg);
    void *cb_arg;
} nh_usb_tx_msg_t;

typedef struct {
    bool initialized;
    volatile bool configured;
    volatile bool active;
    QueueHandle_t dnld_free_queue;
    QueueHandle_t upld_queue;
    TaskHandle_t rx_task;
    TaskHandle_t tx_task;
    nh_usb_rx_slot_t *rx_pending_slot;
    volatile uint32_t rx_pending_len;
    volatile bool tx_pending;
    StreamBufferHandle_t acm_rx_stream;
    SemaphoreHandle_t acm_tx_done_sem;
    SemaphoreHandle_t acm_tx_mutex;
    TaskHandle_t acm_rx_task;
    nh_usb_backend_acm_rx_cb_t acm_rx_cb;
    void *acm_rx_arg;
} nh_usb_ctx_t;

#define FRAME_BUFFER_ATTR  __ALIGNED(64) ATTR_WIFI_RAM_SECTION
#define NH_USB_RX_HEADROOM USB_ALIGN_UP(PBUF_LINK_ENCAPSULATION_HLEN, CONFIG_USB_ALIGN_SIZE)

static nh_usb_ctx_t g_transport_usb_ctx;
static nethub_usb_device_ops_t g_transport_usb_device;
static nethub_usb_device_cdc_ops_t g_transport_usb_ecm;
static nethub_usb_device_cdc_ops_t g_transport_usb_cmd_acm;
static nh_usb_rx_slot_t g_transport_usb_rx_slots[NH_USB_RX_SLOT_CNT];
static FRAME_BUFFER_ATTR uint8_t
    g_transport_usb_rx_buffers[NH_USB_RX_SLOT_CNT][NH_USB_FRAME_MAX_LEN + NH_USB_RX_HEADROOM];
static FRAME_BUFFER_ATTR uint8_t g_transport_usb_tx_buffer[USB_ALIGN_UP(NH_USB_FRAME_MAX_LEN, CONFIG_USB_ALIGN_SIZE)];
static FRAME_BUFFER_ATTR uint8_t
    g_transport_usb_acm_read_buffer[USB_ALIGN_UP(NH_USB_ACM_IO_BUFFER_SIZE, CONFIG_USB_ALIGN_SIZE)];
static FRAME_BUFFER_ATTR uint8_t
    g_transport_usb_acm_write_buffer[USB_ALIGN_UP(NH_USB_ACM_IO_BUFFER_SIZE, CONFIG_USB_ALIGN_SIZE)];
static char g_transport_usb_mac_string[13] = "000000000000";
static uint32_t g_transport_usb_link_speed[2] = {
    NH_USB_LINK_SPEED_BPS,
    NH_USB_LINK_SPEED_BPS,
};

static int transport_usb_cmd_acm_start_out_read(void);

static void transport_usb_fill_mac_string(void)
{
    static const char hex[] = "0123456789abcdef";
    uint8_t mac[6] = { 0 };
    size_t i;

    if (wifi_mgmr_sta_mac_get(mac) != 0) {
        LOG_W("wifi_mgmr_sta_mac_get failed, using default usb mac string\r\n");
    }

    for (i = 0; i < ARRAY_SIZE(mac); i++) {
        g_transport_usb_mac_string[i * 2U] = hex[(mac[i] >> 4) & 0x0F];
        g_transport_usb_mac_string[(i * 2U) + 1U] = hex[mac[i] & 0x0F];
    }
}

static void transport_usb_rx_slot_release_cb(void *arg)
{
    nh_usb_rx_slot_t *slot = (nh_usb_rx_slot_t *)arg;

    if (slot == NULL || g_transport_usb_ctx.dnld_free_queue == NULL) {
        return;
    }

    if (xQueueSend(g_transport_usb_ctx.dnld_free_queue, &slot, 0) != pdPASS) {
        LOG_W("usb rx free queue full, drop slot %p\r\n", (void *)slot);
    }
}

static void transport_usb_notify_task(TaskHandle_t task, uint32_t events, BaseType_t *woken)
{
    if (task != NULL) {
        if (xPortIsInsideInterrupt()) {
            xTaskNotifyFromISR(task, events, eSetBits, woken);
        } else {
            (void)xTaskNotify(task, events, eSetBits);
        }
    }
}

static void transport_usb_yield_from_isr(BaseType_t woken)
{
    if (woken != pdFALSE && xPortIsInsideInterrupt()) {
        portYIELD_FROM_ISR(woken);
    }
}

static void transport_usb_give_acm_tx_done(BaseType_t *woken)
{
    if (g_transport_usb_ctx.acm_tx_done_sem == NULL) {
        return;
    }

    if (xPortIsInsideInterrupt()) {
        (void)xSemaphoreGiveFromISR(g_transport_usb_ctx.acm_tx_done_sem, woken);
    } else {
        (void)xSemaphoreGive(g_transport_usb_ctx.acm_tx_done_sem);
    }
}

static void transport_usb_wake_io_tasks(BaseType_t *woken)
{
    transport_usb_notify_task(g_transport_usb_ctx.rx_task, NH_USB_EVENT_CONFIG_CHANGE, woken);
    transport_usb_notify_task(g_transport_usb_ctx.tx_task, NH_USB_EVENT_ACTIVE, woken);
}

static void transport_usb_drain_task_events(void)
{
    uint32_t events;

    while (xTaskNotifyWait(0, NH_USB_NOTIFY_ALL_BITS, &events, 0) == pdTRUE) {}
}

/* Internal helpers: callers are all static and pass &g_transport_usb_* whose
 * start_out_read/start_in_write are set by init, with static buffers and non-zero lengths.
 * Parameter validation is done at the public entry points, not here. */
static int transport_usb_cdc_start_out_read(const nethub_usb_device_cdc_ops_t *ops, uint8_t *data, uint32_t len)
{
    return ops->start_out_read(data, len);
}

static int transport_usb_cdc_start_in_write(const nethub_usb_device_cdc_ops_t *ops, uint8_t *data, uint32_t len)
{
    return ops->start_in_write(data, len);
}

static int transport_usb_ecm_set_connect(bool connect)
{
    uint8_t *data = connect ? (uint8_t *)g_transport_usb_link_speed : NULL;
    uint32_t len = connect ? sizeof(g_transport_usb_link_speed) : 0U;

    if (g_transport_usb_ecm.write_interrupt_in == NULL) {
        return NETHUB_OK;
    }

    return g_transport_usb_ecm.write_interrupt_in(data, len);
}

static int transport_usb_cmd_acm_start_out_read(void)
{
    if (!g_transport_usb_ctx.configured) {
        return NETHUB_ERR_INVALID_STATE;
    }

    return transport_usb_cdc_start_out_read(&g_transport_usb_cmd_acm, g_transport_usb_acm_read_buffer,
                                            sizeof(g_transport_usb_acm_read_buffer));
}

static void transport_usb_cmd_acm_out_done_cb(uint32_t len)
{
    BaseType_t higher_priority_task_woken = pdFALSE;
    size_t sent_len = 0;

    if (len > sizeof(g_transport_usb_acm_read_buffer)) {
        LOG_W("usb acm rx length too large: %u > %u\r\n", (unsigned int)len,
              (unsigned int)sizeof(g_transport_usb_acm_read_buffer));
        (void)transport_usb_cmd_acm_start_out_read();
        return;
    }

    if (g_transport_usb_ctx.acm_rx_stream != NULL && len > 0U) {
        sent_len = xStreamBufferSendFromISR(g_transport_usb_ctx.acm_rx_stream, g_transport_usb_acm_read_buffer, len,
                                            &higher_priority_task_woken);
        if (sent_len != len) {
            LOG_W("usb acm rx overflow: %u -> %u\r\n", (unsigned int)len, (unsigned int)sent_len);
        }
    }

    (void)transport_usb_cmd_acm_start_out_read();

    transport_usb_yield_from_isr(higher_priority_task_woken);
}

static void transport_usb_cmd_acm_in_done_cb(uint32_t len)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    NETHUB_UNUSED(len);

    transport_usb_give_acm_tx_done(&higher_priority_task_woken);
    transport_usb_yield_from_isr(higher_priority_task_woken);
}

static int transport_usb_acm_send(const uint8_t *data, uint32_t len, TickType_t timeout)
{
    uint32_t sent_len = 0;
    uint32_t remaining_len = len;

    if (data == NULL || len == 0U) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    if (!g_transport_usb_ctx.configured ||
        g_transport_usb_ctx.acm_tx_mutex == NULL ||
        g_transport_usb_ctx.acm_tx_done_sem == NULL) {
        return NETHUB_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(g_transport_usb_ctx.acm_tx_mutex, timeout) != pdTRUE) {
        return NETHUB_ERR_INVALID_STATE;
    }

    while (remaining_len > 0U) {
        uint32_t chunk_len = remaining_len;

        if (chunk_len > sizeof(g_transport_usb_acm_write_buffer)) {
            chunk_len = sizeof(g_transport_usb_acm_write_buffer);
        }

        if (xSemaphoreTake(g_transport_usb_ctx.acm_tx_done_sem, timeout) != pdTRUE) {
            xSemaphoreGive(g_transport_usb_ctx.acm_tx_mutex);
            return NETHUB_ERR_INVALID_STATE;
        }

        if (!g_transport_usb_ctx.active || !g_transport_usb_ctx.configured) {
            xSemaphoreGive(g_transport_usb_ctx.acm_tx_done_sem);
            xSemaphoreGive(g_transport_usb_ctx.acm_tx_mutex);
            return NETHUB_ERR_INVALID_STATE;
        }

        memcpy(g_transport_usb_acm_write_buffer, data + sent_len, chunk_len);

        if (transport_usb_cdc_start_in_write(&g_transport_usb_cmd_acm, g_transport_usb_acm_write_buffer, chunk_len) !=
            0) {
            xSemaphoreGive(g_transport_usb_ctx.acm_tx_done_sem);
            xSemaphoreGive(g_transport_usb_ctx.acm_tx_mutex);
            return NETHUB_ERR_INTERNAL;
        }

        sent_len += chunk_len;
        remaining_len -= chunk_len;
    }

    if (xSemaphoreTake(g_transport_usb_ctx.acm_tx_done_sem, timeout) != pdTRUE) {
        xSemaphoreGive(g_transport_usb_ctx.acm_tx_mutex);
        return NETHUB_ERR_INVALID_STATE;
    }
    if (!g_transport_usb_ctx.active || !g_transport_usb_ctx.configured) {
        xSemaphoreGive(g_transport_usb_ctx.acm_tx_done_sem);
        xSemaphoreGive(g_transport_usb_ctx.acm_tx_mutex);
        return NETHUB_ERR_INVALID_STATE;
    }

    xSemaphoreGive(g_transport_usb_ctx.acm_tx_done_sem);
    xSemaphoreGive(g_transport_usb_ctx.acm_tx_mutex);
    return (int)sent_len;
}

static void transport_usb_ecm_in_done_cb(uint32_t len)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    NETHUB_UNUSED(len);

    g_transport_usb_ctx.tx_pending = false;
    transport_usb_notify_task(g_transport_usb_ctx.tx_task, NH_USB_EVENT_TX_DONE, &higher_priority_task_woken);

    transport_usb_yield_from_isr(higher_priority_task_woken);
}

static void transport_usb_ecm_out_done_cb(uint32_t len)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    if (len > NH_USB_FRAME_MAX_LEN) {
        LOG_W("usb ecm rx length too large: %u > %u\r\n", (unsigned int)len, (unsigned int)NH_USB_FRAME_MAX_LEN);
        len = 0U;
    }

    if (g_transport_usb_ctx.rx_pending_slot != NULL) {
        g_transport_usb_ctx.rx_pending_len = len;
    }

    transport_usb_notify_task(g_transport_usb_ctx.rx_task, NH_USB_EVENT_RX_DONE, &higher_priority_task_woken);

    transport_usb_yield_from_isr(higher_priority_task_woken);
}

static void transport_usb_event_cb(uint8_t event)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    switch (event) {
        case USBD_EVENT_RESET:
        case USBD_EVENT_DISCONNECTED:
        case USBD_EVENT_SUSPEND:
            LOG_W("USBD_EVENT_UNCONFIGURED: %u\r\n", (unsigned int)event);
            g_transport_usb_ctx.configured = false;
            (void)transport_usb_ecm_set_connect(false);
            transport_usb_give_acm_tx_done(&higher_priority_task_woken);
            transport_usb_wake_io_tasks(&higher_priority_task_woken);
            break;
        case USBD_EVENT_RESUME:
            LOG_W("USBD_EVENT_RESUME\r\n");
            break;
        case USBD_EVENT_CONFIGURED:
            LOG_W("USBD_EVENT_CONFIGURED\r\n");
            g_transport_usb_ctx.configured = true;
#if defined(CONFIG_NETHUB_PROFILE_DUAL)
            if (nethub_host_report_candidate(NETHUB_CHANNEL_USB)) {
                (void)transport_usb_cmd_acm_start_out_read();
                (void)transport_usb_ecm_set_connect(true);
            } else {
                g_transport_usb_ctx.configured = false;
                (void)transport_usb_ecm_set_connect(false);
            }
#else
            (void)transport_usb_cmd_acm_start_out_read();
            (void)transport_usb_ecm_set_connect(true);
#endif
            transport_usb_wake_io_tasks(&higher_priority_task_woken);
            break;
        default:
            break;
    }

    transport_usb_yield_from_isr(higher_priority_task_woken);
}

static bool transport_usb_ops_ready(void)
{
    return g_transport_usb_ecm.start_out_read != NULL && g_transport_usb_ecm.start_in_write != NULL &&
           g_transport_usb_cmd_acm.start_out_read != NULL && g_transport_usb_cmd_acm.start_in_write != NULL;
}

int nethub_usb_device_init(const nethub_usb_device_ops_t *ops)
{
    if (ops == NULL || ops->init == NULL || ops->deinit == NULL) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    if (g_transport_usb_ctx.initialized) {
        return NETHUB_ERR_INVALID_STATE;
    }

    g_transport_usb_device = *ops;
    return NETHUB_OK;
}

int nethub_usb_device_cdc_ecm_init(const nethub_usb_device_cdc_ops_t *ops, nethub_usb_device_cdc_cbs_t *cbs,
                                   uint32_t link_speed_bps)
{
    uint32_t speed;

    if (ops == NULL || cbs == NULL || ops->start_out_read == NULL || ops->start_in_write == NULL) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    if (g_transport_usb_ctx.initialized) {
        return NETHUB_ERR_INVALID_STATE;
    }

    g_transport_usb_ecm = *ops;

    speed = (link_speed_bps == 0U) ? NH_USB_LINK_SPEED_BPS : link_speed_bps;
    g_transport_usb_link_speed[0] = speed;
    g_transport_usb_link_speed[1] = speed;

    cbs->in_done_cb = transport_usb_ecm_in_done_cb;
    cbs->out_done_cb = transport_usb_ecm_out_done_cb;
    cbs->event_cb = transport_usb_event_cb;
    return NETHUB_OK;
}

int nethub_usb_device_cdc_acm_cmd_init(const nethub_usb_device_cdc_ops_t *ops, nethub_usb_device_cdc_cbs_t *cbs)
{
    if (ops == NULL || cbs == NULL || ops->start_out_read == NULL || ops->start_in_write == NULL) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    if (g_transport_usb_ctx.initialized) {
        return NETHUB_ERR_INVALID_STATE;
    }

    g_transport_usb_cmd_acm = *ops;

    cbs->in_done_cb = transport_usb_cmd_acm_in_done_cb;
    cbs->out_done_cb = transport_usb_cmd_acm_out_done_cb;
    cbs->event_cb = NULL;
    return NETHUB_OK;
}

const char *nethub_usb_device_ecm_mac_string(void)
{
    return g_transport_usb_mac_string;
}

static void transport_usb_rx_task(void *arg)
{
    nh_usb_ctx_t *ctx = (nh_usb_ctx_t *)arg;

    for (;;) {
        nh_usb_rx_slot_t *slot = NULL;
        nethub_frame_t frame = { 0 };
        nethub_route_result_t route_result;
        uint32_t events = 0;

        if (!ctx->configured) {
            xTaskNotifyWait(0, NH_USB_NOTIFY_ALL_BITS, &events, portMAX_DELAY);
            continue;
        }

        if (xQueueReceive(ctx->dnld_free_queue, &slot, portMAX_DELAY) != pdPASS) {
            continue;
        }

        transport_usb_drain_task_events();

        ctx->rx_pending_slot = slot;
        ctx->rx_pending_len = 0;

        if (transport_usb_cdc_start_out_read(&g_transport_usb_ecm, slot->buffer, NH_USB_FRAME_MAX_LEN) != 0) {
            ctx->rx_pending_slot = NULL;
            transport_usb_rx_slot_release_cb(slot);
            if (ctx->configured) {
                taskYIELD();
            }
            continue;
        }

        events = 0;
        xTaskNotifyWait(0, NH_USB_NOTIFY_ALL_BITS, &events, portMAX_DELAY);
        if ((events & NH_USB_EVENT_RX_DONE) == 0U) {
            ctx->rx_pending_slot = NULL;
            transport_usb_rx_slot_release_cb(slot);
            slot = NULL;
        }

        if (slot == NULL) {
            continue;
        }

        ctx->rx_pending_slot = NULL;

        if (!ctx->configured || ctx->rx_pending_len == 0U) {
            transport_usb_rx_slot_release_cb(slot);
            continue;
        }

        frame.data = slot->buffer;
        frame.len = ctx->rx_pending_len;
        frame.free_cb = transport_usb_rx_slot_release_cb;
        frame.cb_arg = slot;
        frame.next = NULL;

        route_result = nethub_process_input(&frame, NETHUB_CHANNEL_USB);
        if (route_result == NETHUB_ROUTE_ERROR) {
            transport_usb_rx_slot_release_cb(slot);
        }
    }
}

static void transport_usb_tx_task(void *arg)
{
    nh_usb_ctx_t *ctx = (nh_usb_ctx_t *)arg;

    for (;;) {
        nh_usb_tx_msg_t msg;
        uint32_t events = 0;

        if (xQueueReceive(ctx->upld_queue, &msg, portMAX_DELAY) != pdPASS) {
            continue;
        }

        while (ctx->active && !ctx->configured) {
            xTaskNotifyWait(0, NH_USB_NOTIFY_ALL_BITS, &events, portMAX_DELAY);
        }

        if (!ctx->active || msg.len == 0U || msg.len > sizeof(g_transport_usb_tx_buffer)) {
            if (msg.free_cb != NULL) {
                msg.free_cb(msg.cb_arg);
            }
            continue;
        }

        memcpy(g_transport_usb_tx_buffer, msg.data, msg.len);

        transport_usb_drain_task_events();

        ctx->tx_pending = true;
        if (transport_usb_cdc_start_in_write(&g_transport_usb_ecm, g_transport_usb_tx_buffer, msg.len) != 0) {
            ctx->tx_pending = false;
            if (msg.free_cb != NULL) {
                msg.free_cb(msg.cb_arg);
            }
            continue;
        }

        events = 0;
        if (xTaskNotifyWait(0, NH_USB_NOTIFY_ALL_BITS, &events, pdMS_TO_TICKS(NH_USB_WAIT_TIMEOUT_MS)) != pdTRUE ||
            (events & NH_USB_EVENT_TX_DONE) == 0U) {
            LOG_W("usb tx wait timeout\r\n");
            ctx->tx_pending = false;
        }

        if (msg.free_cb != NULL) {
            msg.free_cb(msg.cb_arg);
        }
    }
}

static void transport_usb_acm_rx_task(void *arg)
{
    nh_usb_ctx_t *ctx = (nh_usb_ctx_t *)arg;
    uint8_t rx_buf[NH_USB_ACM_DISPATCH_SIZE];

    for (;;) {
        size_t recv_len;

        recv_len = xStreamBufferReceive(ctx->acm_rx_stream, rx_buf, sizeof(rx_buf), portMAX_DELAY);
        if (recv_len == 0U) {
            continue;
        }

        if (ctx->acm_rx_cb != NULL) {
            ctx->acm_rx_cb(ctx->acm_rx_arg, rx_buf, (uint32_t)recv_len);
        }
    }
}

static int transport_usb_prepare_runtime(void)
{
    nh_usb_ctx_t *ctx = &g_transport_usb_ctx;
    uint32_t i;

    if (ctx->dnld_free_queue == NULL) {
        ctx->dnld_free_queue = xQueueCreate(NH_USB_RX_SLOT_CNT, sizeof(nh_usb_rx_slot_t *));
    }
    if (ctx->upld_queue == NULL) {
        ctx->upld_queue = xQueueCreate(NH_USB_TX_QUEUE_DEPTH, sizeof(nh_usb_tx_msg_t));
    }
    if (ctx->acm_rx_stream == NULL) {
        ctx->acm_rx_stream = xStreamBufferCreate(NH_USB_ACM_STREAM_SIZE, 1);
    }
    if (ctx->acm_tx_done_sem == NULL) {
        ctx->acm_tx_done_sem = xSemaphoreCreateBinary();
    }
    if (ctx->acm_tx_mutex == NULL) {
        ctx->acm_tx_mutex = xSemaphoreCreateMutex();
    }
    if (ctx->dnld_free_queue == NULL || ctx->upld_queue == NULL) {
        return NETHUB_ERR_NO_MEMORY;
    }
    if (ctx->acm_rx_stream == NULL || ctx->acm_tx_done_sem == NULL || ctx->acm_tx_mutex == NULL) {
        return NETHUB_ERR_NO_MEMORY;
    }

    if (uxQueueMessagesWaiting(ctx->dnld_free_queue) == 0U) {
        for (i = 0; i < NH_USB_RX_SLOT_CNT; i++) {
            nh_usb_rx_slot_t *slot = &g_transport_usb_rx_slots[i];

            memset(slot, 0, sizeof(*slot));
            slot->buffer = &g_transport_usb_rx_buffers[i][NH_USB_RX_HEADROOM];
            if (xQueueSend(ctx->dnld_free_queue, &slot, 0) != pdPASS) {
                return NETHUB_ERR_INTERNAL;
            }
        }
    }

    if (ctx->rx_task == NULL) {
        if (xTaskCreate(transport_usb_rx_task, "nhusb_rx", NH_USB_TASK_STACK_SIZE, ctx, NH_USB_TASK_PRIORITY,
                        &ctx->rx_task) != pdPASS) {
            return NETHUB_ERR_NO_MEMORY;
        }
    }

    if (ctx->tx_task == NULL) {
        if (xTaskCreate(transport_usb_tx_task, "nhusb_tx", NH_USB_TASK_STACK_SIZE, ctx, NH_USB_TASK_PRIORITY,
                        &ctx->tx_task) != pdPASS) {
            return NETHUB_ERR_NO_MEMORY;
        }
    }

    if (ctx->acm_rx_task == NULL) {
        if (xTaskCreate(transport_usb_acm_rx_task, "nhusb_acm", NH_USB_TASK_STACK_SIZE, ctx, NH_USB_TASK_PRIORITY,
                        &ctx->acm_rx_task) != pdPASS) {
            return NETHUB_ERR_NO_MEMORY;
        }
    }

    (void)xSemaphoreGive(ctx->acm_tx_done_sem);
    return NETHUB_OK;
}

int nh_usb_backend_init(void)
{
    int ret;

    if (g_transport_usb_ctx.initialized) {
        g_transport_usb_ctx.active = true;
        return NETHUB_OK;
    }

    if (!transport_usb_ops_ready()) {
        LOG_E("NetHub USB device is not configured\r\n");
        return NETHUB_ERR_INVALID_STATE;
    }

    memset(&g_transport_usb_ctx, 0, sizeof(g_transport_usb_ctx));

    ret = transport_usb_prepare_runtime();
    if (ret != NETHUB_OK) {
        LOG_E("transport_usb_prepare_runtime failed: %d\r\n", ret);
        return ret;
    }

    transport_usb_fill_mac_string();
    g_transport_usb_ctx.active = true;
    g_transport_usb_ctx.initialized = true;
    LOG_I("usb data path initialized\r\n");
    return NETHUB_OK;
}

nethub_route_result_t nh_usb_backend_output(nethub_frame_t *frame)
{
    nh_usb_tx_msg_t msg;

    if (frame == NULL || frame->data == NULL || frame->len == 0U) {
        return NETHUB_ROUTE_ERROR;
    }

    if (!g_transport_usb_ctx.initialized || g_transport_usb_ctx.upld_queue == NULL) {
        if (frame->free_cb != NULL) {
            frame->free_cb(frame->cb_arg);
        }
        return NETHUB_ROUTE_ERROR;
    }

    msg.data = frame->data;
    msg.len = frame->len;
    msg.free_cb = frame->free_cb;
    msg.cb_arg = frame->cb_arg;

    if (xQueueSend(g_transport_usb_ctx.upld_queue, &msg, pdMS_TO_TICKS(NH_USB_IDLE_POLL_MS)) != pdPASS) {
        if (frame->free_cb != NULL) {
            frame->free_cb(frame->cb_arg);
        }
        return NETHUB_ROUTE_ERROR;
    }

    return NETHUB_ROUTE_CONTINUE;
}

int nh_usb_backend_acm_send(const uint8_t *data_buff, uint32_t data_size)
{
    return transport_usb_acm_send(data_buff, data_size, pdMS_TO_TICKS(NH_USB_WAIT_TIMEOUT_MS));
}

int nh_usb_backend_acm_recv_register(nh_usb_backend_acm_rx_cb_t recv_cb, void *cb_arg)
{
    g_transport_usb_ctx.acm_rx_cb = recv_cb;
    g_transport_usb_ctx.acm_rx_arg = cb_arg;
    return NETHUB_OK;
}

bool nh_usb_backend_is_idle(void)
{
    nh_usb_ctx_t *ctx = &g_transport_usb_ctx;
    UBaseType_t free_slots;

    if (!ctx->initialized || !ctx->active || !ctx->configured) {
        return true;
    }

    if ((ctx->acm_tx_mutex != NULL && uxSemaphoreGetCount(ctx->acm_tx_mutex) == 0U) ||
        (ctx->acm_tx_done_sem != NULL && uxSemaphoreGetCount(ctx->acm_tx_done_sem) == 0U)) {
        return false;
    }

    if (ctx->tx_pending || (ctx->upld_queue != NULL && uxQueueMessagesWaiting(ctx->upld_queue) != 0U)) {
        return false;
    }

    if (ctx->dnld_free_queue != NULL) {
        free_slots = uxQueueMessagesWaiting(ctx->dnld_free_queue);
        if (ctx->rx_pending_slot != NULL) {
            return (ctx->rx_pending_len == 0U) && (free_slots == (NH_USB_RX_SLOT_CNT - 1U));
        }
        return free_slots == NH_USB_RX_SLOT_CNT;
    }

    return true;
}

int nh_usb_backend_lowpower_prepare(void)
{
    nh_usb_ctx_t *ctx = &g_transport_usb_ctx;

    if (!ctx->initialized || !ctx->active) {
        return NETHUB_OK;
    }

    if (!nh_usb_backend_is_idle()) {
        return NETHUB_ERR_INVALID_STATE;
    }

    ctx->active = false;
    ctx->configured = false;
    ctx->rx_pending_len = 0U;
    ctx->tx_pending = false;

    (void)transport_usb_ecm_set_connect(false);
    g_transport_usb_device.deinit();
    if (ctx->acm_tx_done_sem != NULL) {
        (void)xSemaphoreGive(ctx->acm_tx_done_sem);
    }

    if (ctx->rx_task != NULL) {
        xTaskNotify(ctx->rx_task, NH_USB_EVENT_CONFIG_CHANGE, eSetBits);
    }
    if (ctx->tx_task != NULL) {
        xTaskNotify(ctx->tx_task, NH_USB_EVENT_ACTIVE, eSetBits);
    }

    LOG_I("usb data path suspended\r\n");
    return NETHUB_OK;
}

int nh_usb_backend_lowpower_resume(void)
{
    nh_usb_ctx_t *ctx = &g_transport_usb_ctx;

    if (!ctx->initialized) {
        return NETHUB_OK;
    }

    g_transport_usb_device.init();

    ctx->active = true;
    if (ctx->rx_task != NULL) {
        xTaskNotify(ctx->rx_task, NH_USB_EVENT_CONFIG_CHANGE, eSetBits);
    }
    if (ctx->tx_task != NULL) {
        xTaskNotify(ctx->tx_task, NH_USB_EVENT_ACTIVE, eSetBits);
    }

    LOG_I("usb data path recovered\r\n");
    return NETHUB_OK;
}
