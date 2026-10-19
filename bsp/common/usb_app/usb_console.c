#include "usb_console.h"

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "bflb_irq.h"
#include "board.h"
#include "ring_buffer.h"
#include "shell.h"
#include "usbd_cdc_acm.h"
#include "usbd_core.h"

#define USB_CONSOLE_BUS_ID         0U
#define USB_CONSOLE_IN_EP          0x83U
#define USB_CONSOLE_OUT_EP         0x04U
#define USB_CONSOLE_INT_EP         0x85U
#define USB_CONSOLE_FS_MPS         64U
#define USB_CONSOLE_HS_MPS         512U
#define USB_CONSOLE_CONFIG_SIZE    (9U + CDC_ACM_DESCRIPTOR_LEN)
#define USB_CONSOLE_EVENT_IDLE     USBD_EVENT_UNKNOWN

#ifndef CONFIG_CONSOLE_USB_CDC_VID
#define CONFIG_CONSOLE_USB_CDC_VID 0xFFFFU
#endif

#ifndef CONFIG_CONSOLE_USB_CDC_PID
#define CONFIG_CONSOLE_USB_CDC_PID 0xFFFFU
#endif

#ifndef CONFIG_CONSOLE_USB_CDC_TX_RING_SIZE
#define CONFIG_CONSOLE_USB_CDC_TX_RING_SIZE 4096U
#endif

#ifndef CONFIG_CONSOLE_USB_CDC_TASK_STACK_SIZE
#define CONFIG_CONSOLE_USB_CDC_TASK_STACK_SIZE 1024U
#endif

#ifndef CONFIG_CONSOLE_USB_CDC_TX_FLUSH_TIMEOUT_MS
#define CONFIG_CONSOLE_USB_CDC_TX_FLUSH_TIMEOUT_MS 5U
#endif

#define USB_CONSOLE_RETRY_DELAY_MS 10U

static const uint8_t usb_console_device_descriptor[] = { USB_DEVICE_DESCRIPTOR_INIT(
    USB_2_0, 0xEF, 0x02, 0x01, CONFIG_CONSOLE_USB_CDC_VID, CONFIG_CONSOLE_USB_CDC_PID, 0x0100, 0x01) };

#ifdef CONFIG_USB_HS
static const uint8_t usb_console_hs_config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONSOLE_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, 100),
    CDC_ACM_DESCRIPTOR_INIT(0x00, USB_CONSOLE_INT_EP, USB_CONSOLE_OUT_EP, USB_CONSOLE_IN_EP, USB_CONSOLE_HS_MPS, 0x02)
};

static const uint8_t usb_console_other_hs_config_descriptor[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONSOLE_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, 100),
    CDC_ACM_DESCRIPTOR_INIT(0x00, USB_CONSOLE_INT_EP, USB_CONSOLE_OUT_EP, USB_CONSOLE_IN_EP, USB_CONSOLE_HS_MPS, 0x02)
};
#endif

static const uint8_t usb_console_fs_config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONSOLE_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, 100),
    CDC_ACM_DESCRIPTOR_INIT(0x00, USB_CONSOLE_INT_EP, USB_CONSOLE_OUT_EP, USB_CONSOLE_IN_EP, USB_CONSOLE_FS_MPS, 0x02)
};

#ifdef CONFIG_USB_HS
static const uint8_t usb_console_other_fs_config_descriptor[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONSOLE_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, 100),
    CDC_ACM_DESCRIPTOR_INIT(0x00, USB_CONSOLE_INT_EP, USB_CONSOLE_OUT_EP, USB_CONSOLE_IN_EP, USB_CONSOLE_FS_MPS, 0x02)
};

static const uint8_t usb_console_device_qualifier_descriptor[] = { USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(
    USB_2_0, 0xEF, 0x02, 0x01, 0x01) };
#endif

static const char *usb_console_string_descriptors[] = {
    (const char[]){ 0x09, 0x04 },
    "Bouffalo Lab",
    "Bouffalo Lab USB Console",
    "00000001",
};

static const uint8_t *usb_console_device_descriptor_cb(uint8_t speed)
{
    (void)speed;
    return usb_console_device_descriptor;
}

static const uint8_t *usb_console_config_descriptor_cb(uint8_t speed)
{
#ifdef CONFIG_USB_HS
    if (speed == USB_SPEED_HIGH) {
        return usb_console_hs_config_descriptor;
    }
#else
    (void)speed;
#endif
    return usb_console_fs_config_descriptor;
}

#ifdef CONFIG_USB_HS
static const uint8_t *usb_console_device_qualifier_descriptor_cb(uint8_t speed)
{
    (void)speed;
    return usb_console_device_qualifier_descriptor;
}

static const uint8_t *usb_console_other_speed_descriptor_cb(uint8_t speed)
{
    return speed == USB_SPEED_HIGH ? usb_console_other_fs_config_descriptor : usb_console_other_hs_config_descriptor;
}
#endif

static const char *usb_console_string_descriptor_cb(uint8_t speed, uint8_t index)
{
    (void)speed;

    if (index >= (sizeof(usb_console_string_descriptors) / sizeof(usb_console_string_descriptors[0]))) {
        return NULL;
    }
    return usb_console_string_descriptors[index];
}

static const struct usb_descriptor usb_console_descriptor = {
    .device_descriptor_callback = usb_console_device_descriptor_cb,
    .config_descriptor_callback = usb_console_config_descriptor_cb,
#ifdef CONFIG_USB_HS
    .device_quality_descriptor_callback = usb_console_device_qualifier_descriptor_cb,
    .other_speed_descriptor_callback = usb_console_other_speed_descriptor_cb,
#endif
    .string_descriptor_callback = usb_console_string_descriptor_cb,
};

enum usb_console_state {
    USB_CONSOLE_STA_WAIT_USBD_CFG = 0,
    USB_CONSOLE_STA_DATA_POLLING,
    USB_CONSOLE_STA_SUSPENDED,
};

enum usb_console_tx_state {
    USB_CONSOLE_TX_STA_IDLE = 0,
    USB_CONSOLE_TX_STA_WAIT_USBD_IN,
};

enum usb_console_rx_state {
    USB_CONSOLE_RX_STA_START_USBD_OUT = 0,
    USB_CONSOLE_RX_STA_WAIT_USBD_OUT,
};

struct usb_console_context {
    Ring_Buffer_Type tx_ring;
    SemaphoreHandle_t event_sem;
    volatile enum usb_console_state state;
    enum usb_console_tx_state tx_state;
    enum usb_console_rx_state rx_state;
    uint32_t tx_len;
    uint32_t rx_done_len;
    uint8_t usb_event;
    bool tx_done_pending;
    bool rx_done_pending;
};

static struct usb_console_context usb_console = {
    .usb_event = USB_CONSOLE_EVENT_IDLE,
};
static uint8_t usb_console_tx_ring_pool[CONFIG_CONSOLE_USB_CDC_TX_RING_SIZE];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t usb_console_tx_buffer[512];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t usb_console_rx_buffer[512];
static struct usbd_interface usb_console_intf0;
static struct usbd_interface usb_console_intf1;

static void usb_console_bulk_in_cb(uint8_t busid, uint8_t ep, uint32_t nbytes);
static void usb_console_bulk_out_cb(uint8_t busid, uint8_t ep, uint32_t nbytes);
static void usb_console_event_cb(uint8_t busid, uint8_t event);

static struct usbd_endpoint usb_console_in_endpoint = {
    .ep_addr = USB_CONSOLE_IN_EP,
    .ep_cb = usb_console_bulk_in_cb,
};

static struct usbd_endpoint usb_console_out_endpoint = {
    .ep_addr = USB_CONSOLE_OUT_EP,
    .ep_cb = usb_console_bulk_out_cb,
};

static void usb_console_notify_task(void)
{
    if (usb_console.event_sem == NULL) {
        return;
    }

    if (xPortIsInsideInterrupt()) {
        BaseType_t higher_priority_task_woken = pdFALSE;

        (void)xSemaphoreGiveFromISR(usb_console.event_sem, &higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    } else {
        (void)xSemaphoreGive(usb_console.event_sem);
    }
}

static void usb_console_data_reset(enum usb_console_state next_state)
{
    uintptr_t flags = bflb_irq_save();

    usb_console.state = next_state;
    Ring_Buffer_Reset(&usb_console.tx_ring);
    usb_console.tx_done_pending = false;
    usb_console.rx_done_pending = false;
    usb_console.tx_len = 0U;
    usb_console.rx_done_len = 0U;
    usb_console.tx_state = USB_CONSOLE_TX_STA_IDLE;
    usb_console.rx_state = USB_CONSOLE_RX_STA_START_USBD_OUT;
    bflb_irq_restore(flags);
}

ssize_t usb_console_write(const void *data, size_t size)
{
    bool should_notify;
    uint32_t length_before;
    uint32_t written;
    uintptr_t flags;

    if (size == 0U) {
        return 0;
    }
    if (data == NULL) {
        return -1;
    }

    flags = bflb_irq_save();
    if ((usb_console.state != USB_CONSOLE_STA_DATA_POLLING) ||
        (size > Ring_Buffer_Get_Empty_Length(&usb_console.tx_ring))) {
        bflb_irq_restore(flags);
        return (ssize_t)size;
    }

    length_before = Ring_Buffer_Get_Length(&usb_console.tx_ring);
    written = Ring_Buffer_Write(&usb_console.tx_ring, data, (uint32_t)size);
    should_notify = (length_before == 0U) ||
                    ((length_before < sizeof(usb_console_tx_buffer)) &&
                     (written >= (sizeof(usb_console_tx_buffer) - length_before)));
    bflb_irq_restore(flags);

    if (should_notify) {
        usb_console_notify_task();
    }
    return (ssize_t)size;
}

static void usb_console_bulk_in_cb(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    uintptr_t flags;

    (void)ep;
    (void)nbytes;
    if (busid != USB_CONSOLE_BUS_ID) {
        return;
    }

    flags = bflb_irq_save();
    usb_console.tx_done_pending = true;
    bflb_irq_restore(flags);
    usb_console_notify_task();
}

static void usb_console_bulk_out_cb(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    uintptr_t flags;

    (void)ep;
    if (busid != USB_CONSOLE_BUS_ID) {
        return;
    }

    flags = bflb_irq_save();
    usb_console.rx_done_len = nbytes;
    usb_console.rx_done_pending = true;
    bflb_irq_restore(flags);
    usb_console_notify_task();
}

static void usb_console_event_cb(uint8_t busid, uint8_t event)
{
    uintptr_t flags;

    if (busid != USB_CONSOLE_BUS_ID) {
        return;
    }

    flags = bflb_irq_save();
    usb_console.usb_event = event;
    bflb_irq_restore(flags);
    usb_console_notify_task();
}

static bool usb_console_tx_polling(bool flush_timeout, TickType_t *wait_ticks, bool *flush_waiting)
{
    bool done;
    uint32_t pending_len;
    TickType_t flush_ticks;
    uintptr_t flags;

polling_continue:
    switch (usb_console.tx_state) {
        case USB_CONSOLE_TX_STA_IDLE:
            if (usb_console.tx_len == 0U) {
                flush_ticks = pdMS_TO_TICKS(CONFIG_CONSOLE_USB_CDC_TX_FLUSH_TIMEOUT_MS);
                if ((CONFIG_CONSOLE_USB_CDC_TX_FLUSH_TIMEOUT_MS != 0U) && (flush_ticks == 0U)) {
                    flush_ticks = 1U;
                }

                flags = bflb_irq_save();
                pending_len = Ring_Buffer_Get_Length(&usb_console.tx_ring);
                if ((pending_len != 0U) && (pending_len < sizeof(usb_console_tx_buffer)) &&
                    (flush_ticks != 0U) && !flush_timeout) {
                    bflb_irq_restore(flags);
                    *flush_waiting = true;
                    *wait_ticks = flush_ticks;
                    break;
                }

                usb_console.tx_len = pending_len;
                if (usb_console.tx_len > sizeof(usb_console_tx_buffer)) {
                    usb_console.tx_len = sizeof(usb_console_tx_buffer);
                }
                if (usb_console.tx_len != 0U) {
                    usb_console.tx_len =
                        Ring_Buffer_Read(&usb_console.tx_ring, usb_console_tx_buffer, usb_console.tx_len);
                }
                bflb_irq_restore(flags);
            }

            if (usb_console.tx_len == 0U) {
                break;
            }

            flags = bflb_irq_save();
            usb_console.tx_done_pending = false;
            bflb_irq_restore(flags);

            if (usbd_ep_start_write(USB_CONSOLE_BUS_ID, USB_CONSOLE_IN_EP, usb_console_tx_buffer, usb_console.tx_len) !=
                0) {
                return true;
            }
            usb_console.tx_state = USB_CONSOLE_TX_STA_WAIT_USBD_IN;
            break;

        case USB_CONSOLE_TX_STA_WAIT_USBD_IN:
            flags = bflb_irq_save();
            done = usb_console.tx_done_pending;
            if (done) {
                usb_console.tx_done_pending = false;
            }
            bflb_irq_restore(flags);

            if (!done) {
                break;
            }
            usb_console.tx_len = 0U;
            usb_console.tx_state = USB_CONSOLE_TX_STA_IDLE;
            goto polling_continue;

        default:
            usb_console.tx_state = USB_CONSOLE_TX_STA_IDLE;
            break;
    }

    return false;
}

static bool usb_console_rx_polling(void)
{
    bool done;
    uint32_t rx_len;
    uintptr_t flags;

polling_continue:
    switch (usb_console.rx_state) {
        case USB_CONSOLE_RX_STA_START_USBD_OUT:
            flags = bflb_irq_save();
            usb_console.rx_done_pending = false;
            bflb_irq_restore(flags);

            if (usbd_ep_start_read(USB_CONSOLE_BUS_ID, USB_CONSOLE_OUT_EP, usb_console_rx_buffer,
                                   sizeof(usb_console_rx_buffer)) != 0) {
                return true;
            }
            usb_console.rx_state = USB_CONSOLE_RX_STA_WAIT_USBD_OUT;
            break;

        case USB_CONSOLE_RX_STA_WAIT_USBD_OUT:
            flags = bflb_irq_save();
            done = usb_console.rx_done_pending;
            rx_len = usb_console.rx_done_len;
            if (done) {
                usb_console.rx_done_pending = false;
            }
            bflb_irq_restore(flags);

            if (!done) {
                break;
            }
            if ((rx_len != 0U) && (rx_len <= sizeof(usb_console_rx_buffer))) {
                shell_exe_cmd(usb_console_rx_buffer, (uint16_t)rx_len);
            }
            usb_console.rx_state = USB_CONSOLE_RX_STA_START_USBD_OUT;
            goto polling_continue;

        default:
            usb_console.rx_state = USB_CONSOLE_RX_STA_START_USBD_OUT;
            break;
    }

    return false;
}

static void usb_console_task(void *arg)
{
    BaseType_t wait_result;
    bool retry;
    bool flush_timeout;
    bool flush_waiting;
    TickType_t retry_ticks;
    TickType_t wait_ticks;
    uint8_t usb_event;
    uintptr_t flags;

    (void)arg;
    usb_console_data_reset(USB_CONSOLE_STA_WAIT_USBD_CFG);
    flush_timeout = false;

    for (;;) {
        flags = bflb_irq_save();
        usb_event = usb_console.usb_event;
        usb_console.usb_event = USB_CONSOLE_EVENT_IDLE;
        bflb_irq_restore(flags);

        retry = false;
        flush_waiting = false;
        wait_ticks = portMAX_DELAY;

state_continue:
        switch (usb_console.state) {
            case USB_CONSOLE_STA_WAIT_USBD_CFG:
                if (usb_event == USBD_EVENT_CONFIGURED) {
                    usb_console_data_reset(USB_CONSOLE_STA_DATA_POLLING);
                    usb_event = USB_CONSOLE_EVENT_IDLE;
                    goto state_continue;
                }
                break;

            case USB_CONSOLE_STA_DATA_POLLING:
                switch (usb_event) {
                    case USBD_EVENT_RESET:
                    case USBD_EVENT_ERROR:
                    case USBD_EVENT_DISCONNECTED:
                    case USBD_EVENT_DEINIT:
                        usb_console_data_reset(USB_CONSOLE_STA_WAIT_USBD_CFG);
                        break;

                    case USBD_EVENT_CONFIGURED:
                        usb_console_data_reset(USB_CONSOLE_STA_DATA_POLLING);
                        usb_event = USB_CONSOLE_EVENT_IDLE;
                        goto state_continue;

                    case USBD_EVENT_SUSPEND:
                        usb_console_data_reset(USB_CONSOLE_STA_SUSPENDED);
                        break;

                    case USB_CONSOLE_EVENT_IDLE:
                        retry = usb_console_rx_polling();
                        if (usb_console_tx_polling(flush_timeout, &wait_ticks, &flush_waiting)) {
                            retry = true;
                        }
                        flush_timeout = false;
                        break;

                    default:
                        break;
                }
                break;

            case USB_CONSOLE_STA_SUSPENDED:
                switch (usb_event) {
                    case USBD_EVENT_RESET:
                    case USBD_EVENT_ERROR:
                    case USBD_EVENT_DISCONNECTED:
                    case USBD_EVENT_DEINIT:
                        usb_console_data_reset(USB_CONSOLE_STA_WAIT_USBD_CFG);
                        break;

                    case USBD_EVENT_CONFIGURED:
                        usb_console_data_reset(USB_CONSOLE_STA_DATA_POLLING);
                        usb_event = USB_CONSOLE_EVENT_IDLE;
                        goto state_continue;

                    case USBD_EVENT_RESUME:
                        usb_console.state = USB_CONSOLE_STA_DATA_POLLING;
                        usb_event = USB_CONSOLE_EVENT_IDLE;
                        goto state_continue;

                    default:
                        break;
                }
                break;

            default:
                usb_console_data_reset(USB_CONSOLE_STA_WAIT_USBD_CFG);
                break;
        }

        if (retry) {
            retry_ticks = pdMS_TO_TICKS(USB_CONSOLE_RETRY_DELAY_MS);
            if ((wait_ticks == portMAX_DELAY) || (retry_ticks < wait_ticks)) {
                wait_ticks = retry_ticks;
                flush_waiting = false;
            }
        }
        wait_result = xSemaphoreTake(usb_console.event_sem, wait_ticks);
        flush_timeout = flush_waiting && (wait_result == pdFALSE);
    }
}

void usb_console_init(void)
{
    int ret;

    Ring_Buffer_Init(&usb_console.tx_ring, usb_console_tx_ring_pool, sizeof(usb_console_tx_ring_pool), NULL, NULL);

    usb_console.event_sem = xSemaphoreCreateBinary();
    if (usb_console.event_sem == NULL) {
        return;
    }

    shell_init_with_task(NULL);

    usbd_desc_register(USB_CONSOLE_BUS_ID, &usb_console_descriptor);
    usbd_add_interface(USB_CONSOLE_BUS_ID, usbd_cdc_acm_init_intf(USB_CONSOLE_BUS_ID, &usb_console_intf0));
    usbd_add_interface(USB_CONSOLE_BUS_ID, usbd_cdc_acm_init_intf(USB_CONSOLE_BUS_ID, &usb_console_intf1));
    usbd_add_endpoint(USB_CONSOLE_BUS_ID, &usb_console_in_endpoint);
    usbd_add_endpoint(USB_CONSOLE_BUS_ID, &usb_console_out_endpoint);

#ifdef BOARD_USB_VIA_GPIO
    board_usb_gpio_init();
#endif

    ret = usbd_initialize(USB_CONSOLE_BUS_ID, 0, usb_console_event_cb);

    if (ret != 0) {
        vSemaphoreDelete(usb_console.event_sem);
        usb_console.event_sem = NULL;
        return;
    }

    if (xTaskCreate(usb_console_task, "usb_console", CONFIG_CONSOLE_USB_CDC_TASK_STACK_SIZE, NULL,
                    configMAX_PRIORITIES - 2U, NULL) != pdPASS) {
        usbd_deinitialize(USB_CONSOLE_BUS_ID);
        vSemaphoreDelete(usb_console.event_sem);
        usb_console.event_sem = NULL;
        return;
    }
}
