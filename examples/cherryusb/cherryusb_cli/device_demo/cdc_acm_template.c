/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @file cdc_acm_template.c
 * @brief CherryUSB CDC ACM device with greeting and loopback modes.
 */

#include "usbd_core.h"
#include "usbd_cdc_acm.h"

#include "ring_buffer.h"

/* os */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "portmacro.h"

#define DBG_TAG "ACM"
#include "log.h"

/** @name CDC ACM USB configuration
 * @{ */
#define CDC_IN_EP       0x83
#define CDC_OUT_EP      0x04
#define CDC_INT_EP      0x85

#define USBD_VID        0xFFFF
#define USBD_PID        0xFFFF
#define USBD_MAX_POWER  100

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif
/** @} */

/** @brief USB device descriptor for the CDC ACM device. */
static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01)
};

/** @brief USB configuration descriptor for the CDC ACM device. */
static const uint8_t config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02)
};

/** @brief USB device-qualifier descriptor. */
static const uint8_t device_quality_descriptor[] = {
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x00,
    0x00,
};

/** @brief USB string descriptors. */
static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "CherryUSB",                  /* Manufacturer */
    "CherryUSB_CDC_DEMO",         /* Product */
    "2022123456",                 /* Serial Number */
};

/** @brief Return the device descriptor.
 * @param[in] speed Negotiated USB speed; unused.
 * @return Device descriptor address.
 */
static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    return device_descriptor;
}

/** @brief Return the configuration descriptor.
 * @param[in] speed Negotiated USB speed; unused.
 * @return Configuration descriptor address.
 */
static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    return config_descriptor;
}

/** @brief Return the device-qualifier descriptor.
 * @param[in] speed Negotiated USB speed; unused.
 * @return Device-qualifier descriptor address.
 */
static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    return device_quality_descriptor;
}

/** @brief Return a USB string descriptor.
 * @param[in] speed Negotiated USB speed; unused.
 * @param[in] index String descriptor index.
 * @return Descriptor string, or NULL when unsupported.
 */
static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    if (index > 3) {
        return NULL;
    }
    return string_descriptors[index];
}

/** @brief Descriptor callback table for the CDC ACM device. */
const struct usb_descriptor cdc_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback
};

/** @name CDC ACM data-buffer sizes
 * @{ */
#define USBD_CDC_ACM_RD_BUFF_SIZE (512)
#define USBD_CDC_ACM_WR_BUFF_SIZE (4 * 1024)
/** @} */

/** @brief DMA-accessible endpoint OUT buffer. */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[USBD_CDC_ACM_RD_BUFF_SIZE];
/** @brief DMA-accessible endpoint IN buffer. */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[USBD_CDC_ACM_WR_BUFF_SIZE];

static Ring_Buffer_Type loopback_rb;
static uint8_t loopback_rb_buffer[16 * 1024];

volatile bool usb_acm_ready_flag = false;
static volatile bool acm_tx_busy_flag = false;
static volatile bool acm_rx_busy_flag = false;

/* stress test statistics */
volatile uint32_t acm_stats_rx_bytes = 0;
volatile uint32_t acm_stats_tx_bytes = 0;
volatile uint32_t acm_stats_rx_overflow = 0;
volatile uint32_t acm_stats_rx_cnt = 0;
volatile uint32_t acm_stats_tx_cnt = 0;
static volatile uint32_t acm_stats_start_ms = 0;

TaskHandle_t usbd_cdc_acm_handle = NULL;

/**
 * @brief Notify the CDC ACM send task of a state or transfer event.
 * @note Safe in task and interrupt contexts.
 */
void usbd_cdc_acm_event_trig(void)
{
    if (usbd_cdc_acm_handle) {
        if (xPortIsInsideInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            vTaskNotifyGiveFromISR(usbd_cdc_acm_handle, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            xTaskNotifyGive(usbd_cdc_acm_handle);
        }

    } else {
        LOG_E("ECM usbd_cdc_acm_handle is NULL\r\n");
    }
}

/** @brief Wait for a CDC ACM task notification.
 * @param[in] timeout Maximum wait in FreeRTOS ticks.
 */
void usbd_cdc_acm_event_wait(uint32_t timeout)
{
    ulTaskNotifyTake(pdTRUE, timeout);
}

/** @brief Update CDC ACM readiness for USB device events.
 * @param[in] busid USB device-controller index.
 * @param[in] event CherryUSB device event identifier.
 */
static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            USB_LOG_INFO("CDC_ACM USBD_EVENT_RESET\r\n");
            usb_acm_ready_flag = false;
            usbd_cdc_acm_event_trig();
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            usb_acm_ready_flag = false;
            usbd_cdc_acm_event_trig();
            break;
        case USBD_EVENT_RESUME:
            USB_LOG_INFO("CDC_ACM USBD_EVENT_RESUME\r\n");
            usb_acm_ready_flag = true;
            usbd_cdc_acm_event_trig();
            break;
        case USBD_EVENT_SUSPEND:
            USB_LOG_INFO("CDC_ACM USBD_EVENT_SUSPEND\r\n");
            usb_acm_ready_flag = false;
            usbd_cdc_acm_event_trig();
            break;
        case USBD_EVENT_CONFIGURED:
            USB_LOG_INFO("CDC_ACM configured done\r\n");
            if (usb_acm_ready_flag == true) {
                USB_LOG_ERR("CDC_ACM already configured\r\n");
            }
            usb_acm_ready_flag = true;
            usbd_cdc_acm_event_trig();
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;
        default:
            break;
    }
}

/** @brief Buffer received CDC data and rearm endpoint OUT.
 * @param[in] busid USB device-controller index.
 * @param[in] ep Completed endpoint address; unused.
 * @param[in] nbytes Number of bytes received.
 * @note Runs in USB endpoint-completion context.
 */
void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    // USB_LOG_RAW("dnld done, size: %d\r\n", nbytes);

    if (usb_acm_ready_flag == false) {
        return;
    }

    if (Ring_Buffer_Get_Empty_Length(&loopback_rb) < nbytes) {
        LOG_W("ringbuff FULL\r\n");
        acm_stats_rx_overflow++;
        nbytes = Ring_Buffer_Get_Empty_Length(&loopback_rb);
    }

    /* save to ringbuff */
    if (nbytes) {
        Ring_Buffer_Write(&loopback_rb, (uint8_t *)read_buffer, nbytes);
        acm_stats_rx_bytes += nbytes;
        acm_stats_rx_cnt++;
    }

    /* setup next out ep read transfer */
    usbd_ep_start_read(busid, CDC_OUT_EP, read_buffer, USBD_CDC_ACM_RD_BUFF_SIZE);

    usbd_cdc_acm_event_trig();
}

/** @brief Complete a CDC endpoint-IN transfer and send a ZLP when required.
 * @param[in] busid USB device-controller index.
 * @param[in] ep Completed endpoint address; unused.
 * @param[in] nbytes Number of bytes transmitted.
 * @note Runs in USB endpoint-completion context.
 */
void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    if (usb_acm_ready_flag == false) {
        return;
    }

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(busid, CDC_IN_EP, NULL, 0);
    } else {
        acm_tx_busy_flag = false;
        acm_stats_tx_bytes += nbytes;
        acm_stats_tx_cnt++;
        usbd_cdc_acm_event_trig();
        // USB_LOG_RAW("upld done\r\n");
    }
}

/* ringbuff lock/unlock */
static volatile uintptr_t irq_flag;
/** @brief Enter the ring-buffer critical section by saving IRQ state. */
static void rb_lock_cb(void)
{
    irq_flag = bflb_irq_save();
}
/** @brief Leave the ring-buffer critical section and restore IRQ state. */
static void rb_unlock_cb(void)
{
    bflb_irq_restore(irq_flag);
}

/*!< endpoint call back */
struct usbd_endpoint cdc_acm_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_acm_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

static struct usbd_interface intf0;
static struct usbd_interface intf1;

/** @brief Register and initialize the CDC ACM device.
 * @param[in] busid USB device-controller index.
 * @param[in] reg_base USB controller register base.
 */
void cdc_acm_init(uint8_t busid, uintptr_t reg_base)
{
    usbd_desc_register(busid, &cdc_descriptor);
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(0, &intf0));
    usbd_add_interface(busid, usbd_cdc_acm_init_intf(0, &intf1));
    usbd_add_endpoint(busid, &cdc_acm_out_ep);
    usbd_add_endpoint(busid, &cdc_acm_in_ep);
    usbd_initialize(busid, reg_base, usbd_event_handler);
}

volatile bool dtr_enable = 0;
/* usbd set dtr callback */
/** @brief Handle a host DTR control-line change.
 * @param[in] busid USB device-controller index; unused.
 * @param[in] intf CDC interface number; unused.
 * @param[in] dtr New DTR state; true enables loopback mode.
 */
void usbd_cdc_acm_set_dtr(uint8_t busid, uint8_t intf, bool dtr)
{
    dtr_enable = dtr;
    if (dtr) {
        LOG_I("DTR set, Enter the loopback test mode\r\n");
    } else {
        LOG_I("DTR clear, Exit the loopback test mode\r\n");
    }
    usbd_cdc_acm_event_trig();
}
/* usbd get dtr callback */
/** @brief Handle a host RTS control-line change.
 * @param[in] busid USB device-controller index; unused.
 * @param[in] intf CDC interface number; unused.
 * @param[in] rts New RTS state; currently unused.
 */
void usbd_cdc_acm_set_rts(uint8_t busid, uint8_t intf, bool rts)
{
    // LOG_I("set_rts: %d\r\n", rts);
    usbd_cdc_acm_event_trig();
}

static volatile struct cdc_line_coding acm_line_coding;
/* usbd set line coding callback */
/** @brief Store line-coding parameters supplied by the host.
 * @param[in] busid USB device-controller index; unused.
 * @param[in] intf CDC interface number; unused.
 * @param[in] line_coding New CDC line coding.
 */
void usbd_cdc_acm_set_line_coding(uint8_t busid, uint8_t intf, struct cdc_line_coding *line_coding)
{
    // LOG_I("Set line coding, rate:%d, bits:%d, format:%d, parity:%d.\r\n",
    //       line_coding->dwDTERate, line_coding->bDataBits, line_coding->bCharFormat, line_coding->bParityType);

    acm_line_coding = *line_coding;
}
/* usbd get line coding callback */
/** @brief Return the current line-coding parameters.
 * @param[in] busid USB device-controller index; unused.
 * @param[in] intf CDC interface number; unused.
 * @param[out] line_coding Receives the current CDC line coding.
 */
void usbd_cdc_acm_get_line_coding(uint8_t busid, uint8_t intf, struct cdc_line_coding *line_coding)
{
    (void)busid;
    (void)intf;

    // LOG_I("Get line coding\r\n");
    *line_coding = acm_line_coding;
}

/************************* ACM data send state machine *******************************/
/** @name CDC ACM send-state identifiers
 * @{ */
#define ACM_SEND_STA_STOP               0
#define ACM_SEND_STA_START              1
#define ACM_SEND_STA_WAIT_USBD_CFG      2
#define ACM_SEND_STA_SEND_HELLO         3
#define ACM_SEND_STA_WAIT_LOOPBAKE_DATA 4
#define ACM_SEND_STA_WAIT_SEND_DONE     5
/** @} */

/**
 * @brief Run the CDC ACM greeting and DTR-controlled loopback state machine.
 * @param[in] param Unused FreeRTOS task parameter.
 * @note This task runs indefinitely and waits on task notifications when idle.
 */
void cdc_acm_data_send_task(void *param)
{
    uint32_t time_node_ms = 0;
    uint32_t size;

    uint32_t usbd_acm_send_sta = ACM_SEND_STA_START;

    while (1) {
        switch (usbd_acm_send_sta) {
            case ACM_SEND_STA_STOP:
                // usbd_acm_send_sta = ACM_SEND_STA_START;
                break;

            case ACM_SEND_STA_START:
                LOG_I("usbd cdc_acm machine start/restart \r\n");
                /* Reset state machine */
                dtr_enable = false;
                acm_tx_busy_flag = false;
                acm_rx_busy_flag = false;
                acm_stats_rx_bytes = 0;
                acm_stats_tx_bytes = 0;
                acm_stats_rx_overflow = 0;
                acm_stats_rx_cnt = 0;
                acm_stats_tx_cnt = 0;
                acm_stats_start_ms = bflb_mtimer_get_time_ms();
                Ring_Buffer_Init(&loopback_rb, (uint8_t *)loopback_rb_buffer, sizeof(loopback_rb_buffer), rb_lock_cb, rb_unlock_cb);

                usbd_acm_send_sta = ACM_SEND_STA_WAIT_USBD_CFG;
                break;

            case ACM_SEND_STA_WAIT_USBD_CFG:
                /* check usb status */
                if (usb_acm_ready_flag == false) {
                    usbd_cdc_acm_event_wait(pdMS_TO_TICKS(100));
                    break;
                }
                LOG_I("USBD ACM Ready!\r\n");
                vTaskDelay(5);

                /* setup first out ep read transfer */
                usbd_ep_start_read(0, CDC_OUT_EP, read_buffer, USBD_CDC_ACM_RD_BUFF_SIZE);
                usbd_acm_send_sta = ACM_SEND_STA_SEND_HELLO;
                break;

            case ACM_SEND_STA_SEND_HELLO:
                /* usb status check */
                if (usb_acm_ready_flag == false) {
                    usbd_acm_send_sta = ACM_SEND_STA_START;
                    break;
                }

                if (dtr_enable == true) {
                    usbd_acm_send_sta = ACM_SEND_STA_WAIT_LOOPBAKE_DATA;
                    break;
                } else if (bflb_mtimer_get_time_ms() - time_node_ms > 2000) {
                    time_node_ms = bflb_mtimer_get_time_ms();
                    sprintf((char *)write_buffer, "Hello, this is the ACM test, enabling DTR to enter the loopback test mode\r\n");
                } else {
                    usbd_cdc_acm_event_wait(pdMS_TO_TICKS(100));
                    break;
                }

                acm_tx_busy_flag = true;
                usbd_ep_start_write(0, CDC_IN_EP, write_buffer, strlen((char *)write_buffer));
                usbd_acm_send_sta = ACM_SEND_STA_WAIT_SEND_DONE;
                break;

            case ACM_SEND_STA_WAIT_LOOPBAKE_DATA:
                /* usb status check */
                if (usb_acm_ready_flag == false) {
                    usbd_acm_send_sta = ACM_SEND_STA_START;
                    break;
                }

                if (dtr_enable == false) {
                    usbd_acm_send_sta = ACM_SEND_STA_SEND_HELLO;
                    break;
                }

                size = Ring_Buffer_Get_Length(&loopback_rb);
                if (size == 0) {
                    usbd_cdc_acm_event_wait(pdMS_TO_TICKS(100));
                    break;
                } else if (size > USBD_CDC_ACM_WR_BUFF_SIZE) {
                    size = USBD_CDC_ACM_WR_BUFF_SIZE;
                }
                Ring_Buffer_Read(&loopback_rb, (uint8_t *)&write_buffer, size);

                acm_tx_busy_flag = true;
                usbd_ep_start_write(0, CDC_IN_EP, write_buffer, size);
                usbd_acm_send_sta = ACM_SEND_STA_WAIT_SEND_DONE;
                break;

            case ACM_SEND_STA_WAIT_SEND_DONE:
                /* usb status check */
                if (usb_acm_ready_flag == false) {
                    usbd_acm_send_sta = ACM_SEND_STA_START;
                    break;
                }

                if (acm_tx_busy_flag == true) {
                    usbd_cdc_acm_event_wait(pdMS_TO_TICKS(100));
                    break;
                }

                if (dtr_enable) {
                    /* loopback mode */
                    usbd_acm_send_sta = ACM_SEND_STA_WAIT_LOOPBAKE_DATA;
                } else {
                    /* normal mode */
                    usbd_acm_send_sta = ACM_SEND_STA_SEND_HELLO;
                }
                break;

            default:
                LOG_E("usbd_acm_send_sta error: %d\r\n", usbd_acm_send_sta);
                usbd_acm_send_sta = ACM_SEND_STA_STOP;
                break;
        }
    }
}

/** @brief Start the CDC ACM device and send-state task. */
void usbd_cdc_acm_init(void)
{
    /* usb init */
    usb_acm_ready_flag = false;
    cdc_acm_init(0, 0);

    LOG_I("Create USB device cdc_acm data send task...\r\n");
    xTaskCreate(cdc_acm_data_send_task, (char *)"cdc_acm_send", 1024, NULL, 15, &usbd_cdc_acm_handle);
}

/** @brief Delete the send-state task and deinitialize the CDC ACM device. */
void usbd_cdc_acm_deinit(void)
{
    if (usbd_cdc_acm_handle) {
        vTaskDelete(usbd_cdc_acm_handle);
        usbd_cdc_acm_handle = NULL;
    }

    usbd_deinitialize(0);
}
