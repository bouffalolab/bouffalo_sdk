#include "bflb_mtimer.h"
#include "bflb_emac.h"

#include "usbd_core.h"
#include "rndis_protocol.h"
#include "usbd_rndis.h"

#include "eth_phy.h"
#include "ephy_general.h"
#include "ephy_lan8720.h"

#include "board.h"

/* os */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "portmacro.h"

#include "eth_emac.h"

#define DBG_TAG "RNDIS"
#include "log.h"

/*!< endpoint address */
#define CDC_RNDIS_IN_EP  0x81
#define CDC_RNDIS_OUT_EP 0x02
#define CDC_RNDIS_INT_EP 0x83

#define USBD_VID         0xFFFF
#define USBD_PID         0xFFFF
#define USBD_MAX_POWER   100

/*!< config descriptor size */
#define USB_CONFIG_SIZE  (9 + CDC_RNDIS_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01)
};

static const uint8_t config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_RNDIS_DESCRIPTOR_INIT(0x00, CDC_RNDIS_INT_EP, CDC_RNDIS_OUT_EP, CDC_RNDIS_IN_EP, CDC_MAX_MPS, 0x02)
};

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

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "CherryUSB",                  /* Manufacturer */
    "CherryUSB_RNDIS_DEMO",       /* Product */
    "2022123456",                 /* Serial Number */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    return config_descriptor;
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    return device_quality_descriptor;
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    if (index > 3) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor cdc_rndis_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback
};

/*  */
static void usbd_rndis_emac_event_trig(void);
static void usbd_rndis_emac_event_wait(uint32_t timeout);
static volatile bool usb_rndis_ready_flag = false;

/* usb event handler */
static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            usb_rndis_ready_flag = false;
            usbd_rndis_emac_event_trig();
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            usb_rndis_ready_flag = false;
            usbd_rndis_emac_event_trig();
            break;
        case USBD_EVENT_RESUME:
            USB_LOG_INFO("CDC_RNDIS USBD_EVENT_RESUME\r\n");
            usb_rndis_ready_flag = true;
            usbd_rndis_emac_event_trig();
            break;
        case USBD_EVENT_SUSPEND:
            USB_LOG_INFO("CDC_RNDIS USBD_EVENT_SUSPEND\r\n");
            usb_rndis_ready_flag = false;
            usbd_rndis_emac_event_trig();
            break;
        case USBD_EVENT_CONFIGURED:
            USB_LOG_INFO("CDC_RNDIS USBD_EVENT_CONFIGURED\r\n");
            if (usb_rndis_ready_flag == true) {
                USB_LOG_ERR("CDC_RNDIS already configured\r\n");
            }
            usb_rndis_ready_flag = true;
            usbd_rndis_emac_event_trig();
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;
        default:
            break;
    }
}

static struct usbd_interface intf0;
static struct usbd_interface intf1;

void cdc_rndis_init(uint8_t busid, uintptr_t reg_base)
{
    uint8_t mac[6] = { MAC_ADDR_NUM_0, MAC_ADDR_NUM_1, MAC_ADDR_NUM_2,
                       MAC_ADDR_NUM_3, MAC_ADDR_NUM_4, MAC_ADDR_NUM_5 };

    usbd_desc_register(busid, &cdc_rndis_descriptor);
    usbd_add_interface(busid, usbd_rndis_init_intf(&intf0, CDC_RNDIS_OUT_EP, CDC_RNDIS_IN_EP, CDC_RNDIS_INT_EP, mac));
    usbd_add_interface(busid, usbd_rndis_init_intf(&intf1, CDC_RNDIS_OUT_EP, CDC_RNDIS_IN_EP, CDC_RNDIS_INT_EP, mac));
    usbd_initialize(busid, reg_base, usbd_event_handler);
}

/************************* RNDIS_IN <-> EMAC_RX Sub-state machine *******************************/
#define RNDIS_IN_STA_STOP         0
#define RNDIS_IN_STA_START        1
#define RNDIS_IN_STA_WAIT_EMAC_RX 2
#define RNDIS_IN_STA_WAIT_USBD_IN 3
static volatile uint8_t usbd_rndis_in_status = RNDIS_IN_STA_STOP;

static volatile bool usbd_rndis_in_busy_flag = false;

void usbd_rndis_data_send_done(uint32_t len)
{
    // LOG_I("rndis IN send done, len:%d\r\n", len);

    usbd_rndis_in_busy_flag = false;
    usbd_rndis_emac_event_trig();
}

void usbd_rndis_data_send_start(struct bflb_emac_trans_desc_s *trans_desc)
{
    rndis_data_packet_t *hdr;

    hdr = (rndis_data_packet_t *)(trans_desc->buff_addr) - 1;

    memset(hdr, 0, sizeof(rndis_data_packet_t));
    hdr->MessageType = REMOTE_NDIS_PACKET_MSG;
    hdr->MessageLength = sizeof(rndis_data_packet_t) + trans_desc->data_len;
    hdr->DataOffset = sizeof(rndis_data_packet_t) - sizeof(rndis_generic_msg_t);
    hdr->DataLength = trans_desc->data_len;

    usbd_rndis_start_write((uint8_t *)hdr, hdr->MessageLength);
}

static int usbd_rndis_in_emac_rx_polling(void)
{
    int ret = 0;
    static struct bflb_emac_trans_desc_s trans_desc;

polling_continue:

    switch (usbd_rndis_in_status) {
        case RNDIS_IN_STA_STOP:
            break;

        case RNDIS_IN_STA_START:
            LOG_I("USBD RNDIS IN/RX start\r\n");
            usbd_rndis_in_status = RNDIS_IN_STA_WAIT_EMAC_RX;
            goto polling_continue;
            break;

        case RNDIS_IN_STA_WAIT_EMAC_RX:
            if (eth_emac_rx_data_get(&trans_desc, 0) < 0) {
                break;
            }

            // LOG_I("emac RX data: 0x%X, len:%d\r\n", trans_desc.buff_addr, trans_desc.data_len);
            usbd_rndis_in_busy_flag = true;
            usbd_rndis_data_send_start(&trans_desc);
            usbd_rndis_in_status = RNDIS_IN_STA_WAIT_USBD_IN;
            break;

        case RNDIS_IN_STA_WAIT_USBD_IN:
            if (usbd_rndis_in_busy_flag == true) {
                break;
            }
            eth_emac_rx_data_free(&trans_desc);
            usbd_rndis_in_status = RNDIS_IN_STA_WAIT_EMAC_RX;
            goto polling_continue;

        default:
            LOG_E("usbd_rndis_in_status error: %d\r\n", usbd_rndis_in_status);
            usbd_rndis_in_status = RNDIS_IN_STA_STOP;
            break;
    }

    return ret;
}

/************************* RNDIS_OUT <-> EMAC_TX Sub-state machine *******************************/
#define RNDIS_OUT_STA_STOP          0
#define RNDIS_OUT_STA_START         1
#define RNDIS_OUT_STA_WAIT_EMAC_TX  2
#define RNDIS_OUT_STA_WAIT_USBD_OUT 3
static volatile uint8_t usbd_rndis_out_status = RNDIS_OUT_STA_STOP;

static volatile bool usbd_rndis_out_busy_flag = false;
static volatile uint16_t usbd_rndis_out_done_len = 0;

void usbd_rndis_data_recv_done(uint32_t len)
{
    // LOG_I("rndis OUT recv done, len:%d\r\n", len);

    usbd_rndis_out_busy_flag = false;
    usbd_rndis_out_done_len = len;
    usbd_rndis_emac_event_trig();
}

void usbd_rndis_data_recv_start(struct bflb_emac_trans_desc_s *trans_desc)
{
    rndis_data_packet_t *hdr;

    hdr = (rndis_data_packet_t *)(trans_desc->buff_addr) - 1;

    usbd_rndis_start_read((uint8_t *)hdr, sizeof(rndis_data_packet_t) + EMAC_TX_BUFF_SIZE);
}

static int usbd_rndis_out_emac_tx_polling(void)
{
    int ret = 0;
    static struct bflb_emac_trans_desc_s trans_desc;

polling_continue:

    switch (usbd_rndis_out_status) {
        case RNDIS_OUT_STA_STOP:
            break;

        case RNDIS_OUT_STA_START:
            LOG_I("USBD RNDIS OUT/TX start\r\n");
            usbd_rndis_out_status = RNDIS_OUT_STA_WAIT_EMAC_TX;
            goto polling_continue;
            break;

        case RNDIS_OUT_STA_WAIT_EMAC_TX:
            if (eth_emac_tx_buff_get(&trans_desc, 0) < 0) {
                break;
            }

            // LOG_I("rndis TX buff:0x%X\r\n", trans_desc.buff_addr);
            usbd_rndis_out_busy_flag = true;
            usbd_rndis_data_recv_start(&trans_desc);
            usbd_rndis_out_status = RNDIS_OUT_STA_WAIT_USBD_OUT;
            break;

        case RNDIS_OUT_STA_WAIT_USBD_OUT:
            if (usbd_rndis_out_busy_flag == true) {
                break;
            }
            trans_desc.data_len = usbd_rndis_out_done_len;
            eth_emac_tx_buff_push(&trans_desc);
            usbd_rndis_out_status = RNDIS_OUT_STA_WAIT_EMAC_TX;
            goto polling_continue;

        default:
            LOG_E("usbd_rndis_out_status error: %d\r\n", usbd_rndis_out_status);
            usbd_rndis_out_status = RNDIS_OUT_STA_STOP;
            break;
    }

    return ret;
}

/************************* RNDIS-EMAC state machine *******************************/

#define RNDIS_EMAC_STA_STOP             0
#define RNDIS_EMAC_STA_START            1
#define RNDIS_EMAC_STA_WAIT_USBD_CFG    2
#define RNDIS_EMAC_STA_WAIT_EPHY_LINKUP 3
#define RNDIS_EMAC_STA_DATA_POLLING     4
static volatile uint32_t usbd_rndis_emac_sta = RNDIS_EMAC_STA_STOP;

static int usbd_rndis_emac_polling(void)
{
    static uint32_t status_time_ms = 0;
    static uint32_t info_dump_time_ms = 0;
    uint32_t time_ms = 0;

    switch (usbd_rndis_emac_sta) {
        case RNDIS_EMAC_STA_STOP:
            break;

        case RNDIS_EMAC_STA_START:
            LOG_I("usbd rndis machine start/restart \r\n");
            /* emac start/restart */
            eth_emac_restart();
            /* Reset: RNDIS_IN <-> EMAC_RX Sub-state machine */
            usbd_rndis_in_status = RNDIS_IN_STA_START;
            usbd_rndis_in_busy_flag = false;
            /* Reset: RNDIS_OUT <-> EMAC_TX Sub-state machine */
            usbd_rndis_out_status = RNDIS_OUT_STA_START;
            usbd_rndis_out_busy_flag = false;
            /* */
            usbd_rndis_emac_sta = RNDIS_EMAC_STA_WAIT_USBD_CFG;
            break;

        case RNDIS_EMAC_STA_WAIT_USBD_CFG:
            /* check usb status */
            if (usb_rndis_ready_flag == false) {
                usbd_rndis_emac_event_wait(pdMS_TO_TICKS(100));
                break;
            }

            LOG_I("USBD RNDIS EMAC Ready!\r\n");
            vTaskDelay(20);

            usbd_rndis_set_connect(false);
            usbd_rndis_emac_sta = RNDIS_EMAC_STA_WAIT_EPHY_LINKUP;

            break;

        case RNDIS_EMAC_STA_WAIT_EPHY_LINKUP:
            /* check usb status */
            if (usb_rndis_ready_flag == false) {
                usbd_rndis_emac_sta = RNDIS_EMAC_STA_START;
                break;
            }

            if (eth_link_state_update() == true) {
                /* linkup */
                usbd_rndis_set_connect(true);
                usbd_rndis_emac_sta = RNDIS_EMAC_STA_DATA_POLLING;
            } else {
                /* linkdown */
                usbd_rndis_set_connect(false);
                usbd_rndis_emac_event_wait(pdMS_TO_TICKS(100));
            }
            break;

        case RNDIS_EMAC_STA_DATA_POLLING:
            /* usb status check */
            if (usb_rndis_ready_flag == false) {
                usbd_rndis_emac_sta = RNDIS_EMAC_STA_START;
                break;
            }

            /* rx polling */
            usbd_rndis_in_emac_rx_polling();
            /* tx polling */
            usbd_rndis_out_emac_tx_polling();

            time_ms = bflb_mtimer_get_time_ms();
            /* info dump */
            if (time_ms - info_dump_time_ms > 5000) {
                info_dump_time_ms = time_ms;
                eth_eamc_info_dump();
            }
            /* emac link check */
            if (time_ms - status_time_ms > 100) {
                /* wait usb in/out busy */
                status_time_ms = time_ms;
                usbd_rndis_emac_sta = RNDIS_EMAC_STA_WAIT_EPHY_LINKUP;
            }

            /* wait event trig */
            usbd_rndis_emac_event_wait(pdMS_TO_TICKS(100));
            break;

        default:
            LOG_E("usbd_rndis_emac_sta error: %d\r\n", usbd_rndis_emac_sta);
            usbd_rndis_emac_sta = RNDIS_EMAC_STA_STOP;
            break;
    }

    return 0;
}

/************************* USBD-RNDIS-EMAC task *******************************/
static TaskHandle_t usbd_rndis_emac_handle = NULL;

/* usbd_rndis_emac event trigger */
static void usbd_rndis_emac_event_trig(void)
{
    if (usbd_rndis_emac_handle) {
        if (xPortIsInsideInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            vTaskNotifyGiveFromISR(usbd_rndis_emac_handle, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            xTaskNotifyGive(usbd_rndis_emac_handle);
        }

    } else {
        LOG_E("RNDIS usbd_rndis_emac_handle is NULL\r\n");
    }
}

/* usbd_rndis_emac event wait */
static void usbd_rndis_emac_event_wait(uint32_t timeout)
{
    ulTaskNotifyTake(pdTRUE, timeout);
}

/* usbd_emac_event_cb */
static void usbd_emac_event_cb(uint32_t event)
{
    usbd_rndis_emac_event_trig();
}

/* usbd_rndis_emac_task */
static void usbd_rndis_emac_task(void *param)
{
    usbd_rndis_emac_sta = RNDIS_EMAC_STA_START;
    LOG_I("USB device cdc_rndis <-> emac task start...\r\n");

    while (1) {
        usbd_rndis_emac_polling();
    }

    vTaskDelete(NULL);
}

/* usbd_rndis_emac init */
void usbd_rndis_emac_init(void)
{
    /* usb init */
    usb_rndis_ready_flag = false;
    cdc_rndis_init(0, 0);

    /* eth emac init */
    board_emac_gpio_init();
    eth_emac_init();
    eth_emac_event_cb_register(usbd_emac_event_cb);

    LOG_I("Create USB device cdc_rndis <-> emac task...\r\n");
    xTaskCreate(usbd_rndis_emac_task, (char *)"usbd_rndis_emac", 1024, NULL, 15, &usbd_rndis_emac_handle);
}

void usb_rndis_emac_deinit(void)
{
    if (usbd_rndis_emac_handle) {
        vTaskDelete(usbd_rndis_emac_handle);
        usbd_rndis_emac_handle = NULL;
    }

    eth_emac_deinit();

    usbd_deinitialize(0);
}
