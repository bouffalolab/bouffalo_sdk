#include "usb_backend.h"

#define DBG_TAG "NETHUB_USB"
#include "log.h"

#include "nh_hub.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "compiler/compiler_ld.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "task.h"

#include "lwip/pbuf.h"
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_ecm.h"

extern int wifi_mgmr_sta_mac_get(uint8_t mac[6]);

#define NH_USB_BUSID                0U
#define NH_USB_ECM_IN_EP            0x81U
#define NH_USB_ECM_OUT_EP           0x02U
#define NH_USB_ECM_INT_EP           0x85U
#define NH_USB_ACM_IN_EP            0x83U
#define NH_USB_ACM_OUT_EP           0x04U
#define NH_USB_ACM_INT_EP           0x86U
#define NH_USB_VID                  0xFFFFU
#define NH_USB_PID                  0xFFFFU
#define NH_USB_MAX_POWER            100U
#define NH_USB_LANGID_STRING        1033U
#define NH_USB_MAC_STRING_INDEX     4U
#if defined(CONFIG_NETHUB_PROFILE_SDIO) && defined(CONFIG_NETHUB_PROFILE_USB)
#define NH_USB_RX_SLOT_CNT          2U
#else
#define NH_USB_RX_SLOT_CNT          8U
#endif
#define NH_USB_TX_QUEUE_DEPTH       8U
#define NH_USB_FRAME_MAX_LEN        (1536U)
#define NH_USB_ACM_STREAM_SIZE      2048U
#define NH_USB_ACM_IO_BUFFER_SIZE   512U
#define NH_USB_ACM_DISPATCH_SIZE    256U
#define NH_USB_LINK_SPEED_BPS       (100U * 1000U * 1000U)
#define NH_USB_WAIT_TIMEOUT_MS      1000U
#define NH_USB_IDLE_POLL_MS         20U
#define NH_USB_TASK_STACK_SIZE      1024U
#define NH_USB_TASK_PRIORITY        25U
#define NH_USB_NOTIFY_ALL_BITS      0xFFFFFFFFUL
#define NH_USB_EVENT_RX_DONE        (1UL << 0)
#define NH_USB_EVENT_CONFIG_CHANGE  (1UL << 1)
#define NH_USB_EVENT_TX_DONE        (1UL << 0)
#define NH_USB_EVENT_ACTIVE         (1UL << 1)

#define NH_USB_ACM_DESCRIPTOR_SZ CDC_ACM_DESCRIPTOR_LEN
#define NH_USB_INTERFACE_COUNT   0x04U

#define NH_USB_CONFIG_DESCRIPTOR_SZ (9U + CDC_ECM_DESCRIPTOR_LEN + NH_USB_ACM_DESCRIPTOR_SZ)

#ifdef CONFIG_USB_HS
#define NH_USB_CDC_MAX_MPS 512U
#else
#define NH_USB_CDC_MAX_MPS 64U
#endif

#ifndef USB_ALIGN_UP
#define USB_ALIGN_UP(size, align) (((size) + ((align) - 1U)) & ~((align) - 1U))
#endif

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
    struct usbd_interface ecm_intf0;
    struct usbd_interface ecm_intf1;
    StreamBufferHandle_t acm_rx_stream;
    SemaphoreHandle_t acm_tx_done_sem;
    SemaphoreHandle_t acm_tx_mutex;
    TaskHandle_t acm_rx_task;
    volatile bool acm_read_started;
    nh_usb_backend_acm_rx_cb_t acm_rx_cb;
    void *acm_rx_arg;
    struct usbd_interface acm_intf0;
    struct usbd_interface acm_intf1;
} nh_usb_ctx_t;

#define FRAME_BUFFER_ATTR __ALIGNED(64) ATTR_WIFI_RAM_SECTION
#define NH_USB_RX_HEADROOM USB_ALIGN_UP(PBUF_LINK_ENCAPSULATION_HLEN, CONFIG_USB_ALIGN_SIZE)

static nh_usb_ctx_t g_transport_usb_ctx;
static nh_usb_rx_slot_t g_transport_usb_rx_slots[NH_USB_RX_SLOT_CNT];
static FRAME_BUFFER_ATTR uint8_t g_transport_usb_rx_buffers[NH_USB_RX_SLOT_CNT][NH_USB_FRAME_MAX_LEN + NH_USB_RX_HEADROOM];
static FRAME_BUFFER_ATTR uint8_t g_transport_usb_tx_buffer[USB_ALIGN_UP(NH_USB_FRAME_MAX_LEN, CONFIG_USB_ALIGN_SIZE)];
static FRAME_BUFFER_ATTR uint8_t g_transport_usb_acm_read_buffer[USB_ALIGN_UP(NH_USB_ACM_IO_BUFFER_SIZE, CONFIG_USB_ALIGN_SIZE)];
static FRAME_BUFFER_ATTR uint8_t g_transport_usb_acm_write_buffer[USB_ALIGN_UP(NH_USB_ACM_IO_BUFFER_SIZE, CONFIG_USB_ALIGN_SIZE)];
static char g_transport_usb_mac_string[13] = "000000000000";
static uint32_t g_transport_usb_link_speed[2] = {
    NH_USB_LINK_SPEED_BPS,
    NH_USB_LINK_SPEED_BPS,
};

#define NH_USB_MANUFACTURER_STR_LEN 24U
#define NH_USB_PRODUCT_STR_LEN      40U
#define NH_USB_SERIAL_STR_LEN       22U
#define NH_USB_RAW_MAC_OFFSET       (18U + 9U + CDC_ECM_DESCRIPTOR_LEN + NH_USB_ACM_DESCRIPTOR_SZ + 4U + \
                                     NH_USB_MANUFACTURER_STR_LEN + NH_USB_PRODUCT_STR_LEN + \
                                     NH_USB_SERIAL_STR_LEN + 2U)

static uint8_t g_transport_usb_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, NH_USB_VID, NH_USB_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(NH_USB_CONFIG_DESCRIPTOR_SZ, NH_USB_INTERFACE_COUNT, 0x01,
                               USB_CONFIG_BUS_POWERED, NH_USB_MAX_POWER),
    CDC_ECM_DESCRIPTOR_INIT(0x00, NH_USB_ECM_INT_EP, NH_USB_ECM_OUT_EP, NH_USB_ECM_IN_EP,
                            NH_USB_CDC_MAX_MPS, NH_USB_MAC_STRING_INDEX),
    CDC_ACM_DESCRIPTOR_INIT(0x02, NH_USB_ACM_INT_EP, NH_USB_ACM_OUT_EP, NH_USB_ACM_IN_EP,
                            NH_USB_CDC_MAX_MPS, 0x02),
    USB_LANGID_INIT(NH_USB_LANGID_STRING),
    0x18, USB_DESCRIPTOR_TYPE_STRING,
    'B', 0x00, 'o', 0x00, 'u', 0x00, 'f', 0x00, 'f', 0x00, 'a', 0x00,
    'l', 0x00, 'o', 0x00, 'L', 0x00, 'a', 0x00, 'b', 0x00,
    0x28, USB_DESCRIPTOR_TYPE_STRING,
    'B', 0x00, 'o', 0x00, 'u', 0x00, 'f', 0x00, 'f', 0x00, 'a', 0x00,
    'l', 0x00, 'o', 0x00, ' ', 0x00, 'N', 0x00, 'e', 0x00, 't', 0x00,
    'h', 0x00, 'u', 0x00, 'b', 0x00, ' ', 0x00, 'E', 0x00, 'C', 0x00,
    'M', 0x00,
    0x16, USB_DESCRIPTOR_TYPE_STRING,
    '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00,
    '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00, '1', 0x00,
    0x1A, USB_DESCRIPTOR_TYPE_STRING,
    '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00,
    '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00,
#ifdef CONFIG_USB_HS
    0x0a, USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
#endif
    0x00,
};

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

    for (i = 0; i < 12U; i++) {
        g_transport_usb_descriptor[NH_USB_RAW_MAC_OFFSET + (i * 2U)] = (uint8_t)g_transport_usb_mac_string[i];
    }
}

static void transport_usb_rx_slot_release(void *arg)
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

static void transport_usb_wake_io_tasks(BaseType_t *woken)
{
    transport_usb_notify_task(g_transport_usb_ctx.rx_task,
                              NH_USB_EVENT_CONFIG_CHANGE,
                              woken);
    transport_usb_notify_task(g_transport_usb_ctx.tx_task,
                              NH_USB_EVENT_ACTIVE,
                              woken);
}

static void transport_usb_drain_task_events(void)
{
    uint32_t events;

    while (xTaskNotifyWait(0, NH_USB_NOTIFY_ALL_BITS, &events, 0) == pdTRUE) {
    }
}

static void transport_usb_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    BaseType_t higher_priority_task_woken = pdFALSE;
    size_t sent_len = 0;

    NETHUB_UNUSED(ep);

    if (g_transport_usb_ctx.acm_rx_stream != NULL && nbytes > 0U) {
        sent_len = xStreamBufferSendFromISR(g_transport_usb_ctx.acm_rx_stream,
                                            g_transport_usb_acm_read_buffer,
                                            nbytes,
                                            &higher_priority_task_woken);
        if (sent_len != nbytes) {
            LOG_W("usb acm rx overflow: %u -> %u\r\n", nbytes, (unsigned int)sent_len);
        }
    }

    usbd_ep_start_read(busid, NH_USB_ACM_OUT_EP,
                       g_transport_usb_acm_read_buffer,
                       sizeof(g_transport_usb_acm_read_buffer));

    portYIELD_FROM_ISR(higher_priority_task_woken);
}

static void transport_usb_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    if ((nbytes % usbd_get_ep_mps(busid, ep)) == 0U && nbytes != 0U) {
        usbd_ep_start_write(busid, ep, NULL, 0);
        return;
    }

    if (g_transport_usb_ctx.acm_tx_done_sem != NULL) {
        xSemaphoreGiveFromISR(g_transport_usb_ctx.acm_tx_done_sem, &higher_priority_task_woken);
    }

    portYIELD_FROM_ISR(higher_priority_task_woken);
}

static struct usbd_endpoint g_transport_usb_acm_out_ep = {
    .ep_addr = NH_USB_ACM_OUT_EP,
    .ep_cb = transport_usb_acm_bulk_out,
};

static struct usbd_endpoint g_transport_usb_acm_in_ep = {
    .ep_addr = NH_USB_ACM_IN_EP,
    .ep_cb = transport_usb_acm_bulk_in,
};

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
        if (usbd_ep_start_write(NH_USB_BUSID, NH_USB_ACM_IN_EP,
                                g_transport_usb_acm_write_buffer, chunk_len) != 0) {
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

static void transport_usb_acm_init(void)
{
    usbd_add_interface(NH_USB_BUSID, usbd_cdc_acm_init_intf(NH_USB_BUSID, &g_transport_usb_ctx.acm_intf0));
    usbd_add_interface(NH_USB_BUSID, usbd_cdc_acm_init_intf(NH_USB_BUSID, &g_transport_usb_ctx.acm_intf1));
    usbd_add_endpoint(NH_USB_BUSID, &g_transport_usb_acm_out_ep);
    usbd_add_endpoint(NH_USB_BUSID, &g_transport_usb_acm_in_ep);
}

void usbd_cdc_ecm_data_send_done(uint32_t len)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    NETHUB_UNUSED(len);

    g_transport_usb_ctx.tx_pending = false;
    transport_usb_notify_task(g_transport_usb_ctx.tx_task,
                              NH_USB_EVENT_TX_DONE,
                              &higher_priority_task_woken);

    transport_usb_yield_from_isr(higher_priority_task_woken);
}

void usbd_cdc_ecm_data_recv_done(uint32_t len)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    if (g_transport_usb_ctx.rx_pending_slot != NULL) {
        g_transport_usb_ctx.rx_pending_len = len;
    }

    transport_usb_notify_task(g_transport_usb_ctx.rx_task,
                              NH_USB_EVENT_RX_DONE,
                              &higher_priority_task_woken);

    transport_usb_yield_from_isr(higher_priority_task_woken);
}

static void transport_usb_event_handler(uint8_t busid, uint8_t event)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    NETHUB_UNUSED(busid);

    switch (event) {
        case USBD_EVENT_RESET:
            LOG_W("USBD_EVENT_RESET\r\n");
            break;
        case USBD_EVENT_DISCONNECTED:
            LOG_W("USBD_EVENT_DISCONNECTED\r\n");
            g_transport_usb_ctx.configured = false;
            g_transport_usb_ctx.acm_read_started = false;
            usbd_cdc_ecm_set_connect(false, NULL);
            transport_usb_wake_io_tasks(&higher_priority_task_woken);
            break;
        case USBD_EVENT_SUSPEND:
            LOG_W("USBD_EVENT_SUSPEND\r\n");
            g_transport_usb_ctx.configured = false;
            g_transport_usb_ctx.acm_read_started = false;
            usbd_cdc_ecm_set_connect(false, NULL);
            transport_usb_wake_io_tasks(&higher_priority_task_woken);
            break;
        case USBD_EVENT_RESUME:
            LOG_W("USBD_EVENT_RESUME\r\n");
            break;
        case USBD_EVENT_CONFIGURED:
            LOG_W("USBD_EVENT_CONFIGURED\r\n");
            g_transport_usb_ctx.configured = true;
            if (!g_transport_usb_ctx.acm_read_started) {
                if (usbd_ep_start_read(NH_USB_BUSID, NH_USB_ACM_OUT_EP,
                                       g_transport_usb_acm_read_buffer,
                                       sizeof(g_transport_usb_acm_read_buffer)) == 0) {
                    g_transport_usb_ctx.acm_read_started = true;
                }
            }
            usbd_cdc_ecm_set_connect(true, g_transport_usb_link_speed);
            transport_usb_wake_io_tasks(&higher_priority_task_woken);
            break;
        default:
            break;
    }

    transport_usb_yield_from_isr(higher_priority_task_woken);
}

static void transport_usb_desc_register(void)
{
    usbd_desc_register(NH_USB_BUSID, g_transport_usb_descriptor);
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

        if (usbd_cdc_ecm_start_read(slot->buffer, NH_USB_FRAME_MAX_LEN) != 0) {
            ctx->rx_pending_slot = NULL;
            transport_usb_rx_slot_release(slot);
            if (ctx->configured) {
                taskYIELD();
            }
            continue;
        }

        events = 0;
        xTaskNotifyWait(0, NH_USB_NOTIFY_ALL_BITS, &events, portMAX_DELAY);
        if ((events & NH_USB_EVENT_RX_DONE) == 0U) {
            ctx->rx_pending_slot = NULL;
            transport_usb_rx_slot_release(slot);
            slot = NULL;
        }

        if (slot == NULL) {
            continue;
        }

        ctx->rx_pending_slot = NULL;

        if (!ctx->configured || ctx->rx_pending_len == 0U) {
            transport_usb_rx_slot_release(slot);
            continue;
        }

        frame.data = slot->buffer;
        frame.len = ctx->rx_pending_len;
        frame.free_cb = transport_usb_rx_slot_release;
        frame.cb_arg = slot;
        frame.next = NULL;

        route_result = nethub_process_input(&frame, NETHUB_CHANNEL_USB);
        if (route_result == NETHUB_ROUTE_ERROR) {
            transport_usb_rx_slot_release(slot);
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
        if (usbd_cdc_ecm_start_write(g_transport_usb_tx_buffer, msg.len) != 0) {
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
        if (xTaskCreate(transport_usb_rx_task, "nhusb_rx", NH_USB_TASK_STACK_SIZE, ctx,
                        NH_USB_TASK_PRIORITY, &ctx->rx_task) != pdPASS) {
            return NETHUB_ERR_NO_MEMORY;
        }
    }

    if (ctx->tx_task == NULL) {
        if (xTaskCreate(transport_usb_tx_task, "nhusb_tx", NH_USB_TASK_STACK_SIZE, ctx,
                        NH_USB_TASK_PRIORITY, &ctx->tx_task) != pdPASS) {
            return NETHUB_ERR_NO_MEMORY;
        }
    }

    if (ctx->acm_rx_task == NULL) {
        if (xTaskCreate(transport_usb_acm_rx_task, "nhusb_acm", NH_USB_TASK_STACK_SIZE, ctx,
                        NH_USB_TASK_PRIORITY, &ctx->acm_rx_task) != pdPASS) {
            return NETHUB_ERR_NO_MEMORY;
        }
    }

    xSemaphoreGive(ctx->acm_tx_done_sem);

    return NETHUB_OK;
}

int nh_usb_backend_init(void)
{
    int ret;

    if (g_transport_usb_ctx.initialized) {
        g_transport_usb_ctx.active = true;
        return NETHUB_OK;
    }

    memset(&g_transport_usb_ctx, 0, sizeof(g_transport_usb_ctx));

    ret = transport_usb_prepare_runtime();
    if (ret != NETHUB_OK) {
        LOG_E("transport_usb_prepare_runtime failed: %d\r\n", ret);
        return ret;
    }

    transport_usb_fill_mac_string();
    transport_usb_desc_register();
    usbd_add_interface(NH_USB_BUSID, usbd_cdc_ecm_init_intf(&g_transport_usb_ctx.ecm_intf0,
                                                            NH_USB_ECM_INT_EP,
                                                            NH_USB_ECM_OUT_EP,
                                                            NH_USB_ECM_IN_EP));
    usbd_add_interface(NH_USB_BUSID, usbd_cdc_ecm_init_intf(&g_transport_usb_ctx.ecm_intf1,
                                                            NH_USB_ECM_INT_EP,
                                                            NH_USB_ECM_OUT_EP,
                                                            NH_USB_ECM_IN_EP));
    transport_usb_acm_init();
    ret = usbd_initialize(NH_USB_BUSID, 0, transport_usb_event_handler);
    if (ret != 0) {
        LOG_E("usbd_initialize failed: %d\r\n", ret);
        return NETHUB_ERR_INTERNAL;
    }

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

    if (ctx->tx_pending ||
        (ctx->upld_queue != NULL && uxQueueMessagesWaiting(ctx->upld_queue) != 0U)) {
        return false;
    }

    if (ctx->dnld_free_queue != NULL) {
        free_slots = uxQueueMessagesWaiting(ctx->dnld_free_queue);
        if (ctx->rx_pending_slot != NULL) {
            return (ctx->rx_pending_len == 0U) &&
                   (free_slots == (NH_USB_RX_SLOT_CNT - 1U));
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
    ctx->acm_read_started = false;
    ctx->rx_pending_len = 0U;
    ctx->tx_pending = false;

    usbd_cdc_ecm_set_connect(false, NULL);
    (void)usbd_deinitialize(NH_USB_BUSID);
    if (ctx->acm_tx_done_sem != NULL) {
        xSemaphoreGive(ctx->acm_tx_done_sem);
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
    int ret;

    if (!ctx->initialized) {
        return NETHUB_OK;
    }

    transport_usb_fill_mac_string();
    transport_usb_desc_register();
    usbd_add_interface(NH_USB_BUSID, usbd_cdc_ecm_init_intf(&ctx->ecm_intf0,
                                                            NH_USB_ECM_INT_EP,
                                                            NH_USB_ECM_OUT_EP,
                                                            NH_USB_ECM_IN_EP));
    usbd_add_interface(NH_USB_BUSID, usbd_cdc_ecm_init_intf(&ctx->ecm_intf1,
                                                            NH_USB_ECM_INT_EP,
                                                            NH_USB_ECM_OUT_EP,
                                                            NH_USB_ECM_IN_EP));
    transport_usb_acm_init();

    ret = usbd_initialize(NH_USB_BUSID, 0, transport_usb_event_handler);
    if (ret != 0) {
        LOG_E("usb lowpower resume failed: %d\r\n", ret);
        return NETHUB_ERR_INTERNAL;
    }

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
