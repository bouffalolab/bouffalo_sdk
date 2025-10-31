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

/* MAC ADDRESS NUM */
#define MAC_ADDR_NUM_0     (0x18)
#define MAC_ADDR_NUM_1     (0xB9)
#define MAC_ADDR_NUM_2     (0x05)
#define MAC_ADDR_NUM_3     (0x12)
#define MAC_ADDR_NUM_4     (0x34)
#define MAC_ADDR_NUM_5     (0x56)
/* MAC ADDRESS ASCII */
#define MAC_ADDR_ASCII_00  ('1')
#define MAC_ADDR_ASCII_01  ('8')
#define MAC_ADDR_ASCII_10  ('B')
#define MAC_ADDR_ASCII_11  ('9')
#define MAC_ADDR_ASCII_20  ('0')
#define MAC_ADDR_ASCII_21  ('5')
#define MAC_ADDR_ASCII_30  ('1')
#define MAC_ADDR_ASCII_31  ('2')
#define MAC_ADDR_ASCII_40  ('3')
#define MAC_ADDR_ASCII_41  ('4')
#define MAC_ADDR_ASCII_50  ('5')
#define MAC_ADDR_ASCII_51  ('6')

/*!< endpoint address */
#define CDC_RNDIS_IN_EP    0x81
#define CDC_RNDIS_OUT_EP   0x02
#define CDC_RNDIS_INT_EP   0x83

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE    (9 + CDC_RNDIS_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

#ifdef CONFIG_USBDEV_ADVANCE_DESC
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
#else
/*!< global descriptor */
static const uint8_t cdc_rndis_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_RNDIS_DESCRIPTOR_INIT(0x00, CDC_RNDIS_INT_EP, CDC_RNDIS_OUT_EP, CDC_RNDIS_IN_EP, CDC_MAX_MPS, 0x02),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x2A,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    '_', 0x00,                  /* wcChar9 */
    'R', 0x00,                  /* wcChar10 */
    'N', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    'I', 0x00,                  /* wcChar13 */
    'S', 0x00,                  /* wcChar14 */
    '_', 0x00,                  /* wcChar15 */
    'D', 0x00,                  /* wcChar16 */
    'E', 0x00,                  /* wcChar17 */
    'M', 0x00,                  /* wcChar18 */
    'O', 0x00,                  /* wcChar19 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
    ///////////////////////////////////////
    /// string4 descriptor
    ///////////////////////////////////////
    0x1A,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    MAC_ADDR_ASCII_00, 0x00,    /* wcChar0 */
    MAC_ADDR_ASCII_01, 0x00,    /* wcChar1 */
    MAC_ADDR_ASCII_10, 0x00,    /* wcChar2 */
    MAC_ADDR_ASCII_11, 0x00,    /* wcChar3 */
    MAC_ADDR_ASCII_20, 0x00,    /* wcChar4 */
    MAC_ADDR_ASCII_21, 0x00,    /* wcChar5 */
    MAC_ADDR_ASCII_30, 0x00,    /* wcChar6 */
    MAC_ADDR_ASCII_31, 0x00,    /* wcChar7 */
    MAC_ADDR_ASCII_40, 0x00,    /* wcChar8 */
    MAC_ADDR_ASCII_41, 0x00,    /* wcChar9 */
    MAC_ADDR_ASCII_50, 0x00,    /* wcChar10 */
    MAC_ADDR_ASCII_51, 0x00,    /* wcChar11 */
#ifdef CONFIG_USB_HS
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
#endif
    0x00
};
#endif

static TaskHandle_t usbd_rndis_emac_handle = NULL;
static volatile bool usb_rndis_ready_flag = false;

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

static void usbd_rndis_emac_event_wait(uint32_t timeout)
{
    ulTaskNotifyTake(pdTRUE, timeout);
}

static void usbd_emac_event_cb(uint32_t event)
{
    usbd_rndis_emac_event_trig();
}

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

#ifdef CONFIG_USBDEV_ADVANCE_DESC
    usbd_desc_register(busid, &cdc_rndis_descriptor);
#else
    usbd_desc_register(busid, cdc_rndis_descriptor);
#endif
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

static int usbd_rndis_in_emac_rx_polling(uint8_t ctrl)
{
    int ret = 0;
    static struct bflb_emac_trans_desc_s trans_desc;

polling_continue:

    switch (usbd_rndis_in_status) {
        case RNDIS_IN_STA_STOP:
            if (ctrl == RNDIS_IN_STA_START) {
                usbd_rndis_in_status = RNDIS_IN_STA_START;
                goto polling_continue;
            }
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
            if (usbd_rndis_in_busy_flag == true) {
                LOG_E("rndis in is busy\r\n");
                usbd_rndis_in_status = RNDIS_IN_STA_STOP;
                break;
            }
            // LOG_I("emac RX data: 0x%X, len:%d\r\n", trans_desc.buff_addr, trans_desc.data_len);
            usbd_rndis_in_busy_flag = true;
            usbd_rndis_data_send_start(&trans_desc);
            usbd_rndis_in_status = RNDIS_IN_STA_WAIT_USBD_IN;
            // goto polling_continue;
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

static int usbd_rndis_out_emac_tx_polling(uint8_t ctrl)
{
    int ret = 0;
    static struct bflb_emac_trans_desc_s trans_desc;

polling_continue:

    switch (usbd_rndis_out_status) {
        case RNDIS_OUT_STA_STOP:
            if (ctrl == RNDIS_OUT_STA_START) {
                usbd_rndis_out_status = RNDIS_OUT_STA_START;
                goto polling_continue;
            }
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
            if (usbd_rndis_out_busy_flag == true) {
                LOG_E("rndis out is busy\r\n");
                usbd_rndis_out_status = RNDIS_OUT_STA_STOP;
                break;
            }
            // LOG_I("rndis TX buff:0x%X\r\n", trans_desc.buff_addr);
            usbd_rndis_out_busy_flag = true;
            usbd_rndis_data_recv_start(&trans_desc);
            usbd_rndis_out_status = RNDIS_OUT_STA_WAIT_USBD_OUT;
            // goto polling_continue;
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

static void usbd_rndis_emac_task(void *param)
{
    uint32_t status_time_ms = 0;
    uint32_t info_dump_time_ms = 0;
    uint32_t time_ms = 0;
    bool eth_link_sta = false;

    uint32_t usbd_rndis_emac_sta = RNDIS_EMAC_STA_START;

    LOG_I("USB device cdc_rndis <-> emac task start...\r\n");

    while (1) {
        switch (usbd_rndis_emac_sta) {
            case RNDIS_EMAC_STA_STOP:
                // usbd_rndis_emac_sta = RNDIS_EMAC_STA_START;
                break;

            case RNDIS_EMAC_STA_START:
                LOG_I("usbd rndis machine start/restart \r\n");
                /* emac start/restart */
                eth_emac_restart();
                /* Reset: RNDIS_IN <-> EMAC_RX Sub-state machine */
                usbd_rndis_in_status = RNDIS_IN_STA_STOP;
                usbd_rndis_in_busy_flag = false;
                /* Reset: RNDIS_OUT <-> EMAC_TX Sub-state machine */
                usbd_rndis_out_status = RNDIS_OUT_STA_STOP;
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

                vTaskDelay(20);
                eth_link_sta = false;
                usbd_rndis_set_connect(false);
                usbd_rndis_emac_sta = RNDIS_EMAC_STA_WAIT_EPHY_LINKUP;
                LOG_I("USBD RNDIS EMAC Ready!\r\n");
                break;

            case RNDIS_EMAC_STA_WAIT_EPHY_LINKUP:
                /* check usb status */
                if (usb_rndis_ready_flag == false) {
                    usbd_rndis_emac_sta = RNDIS_EMAC_STA_START;
                    break;
                }

                if (eth_link_state_update() == true) {
                    /* linkup */
                    if (eth_link_sta == false) {
                        eth_link_sta = true;
                        usbd_rndis_set_connect(true);
                        LOG_I("RNDIS EMAC connect\r\n");
                    }
                    usbd_rndis_emac_sta = RNDIS_EMAC_STA_DATA_POLLING;
                } else {
                    /* linkdown */
                    if (eth_link_sta == true) {
                        eth_link_sta = false;
                        usbd_rndis_set_connect(false);
                        LOG_I("RNDIS EMAC disconnect\r\n");
                    }
                    usbd_rndis_emac_event_wait(pdMS_TO_TICKS(100));
                }
                break;

            case RNDIS_EMAC_STA_DATA_POLLING:
                /* wait event trig */
                usbd_rndis_emac_event_wait(pdMS_TO_TICKS(100));

                /* usb status check */
                if (usb_rndis_ready_flag == false) {
                    usbd_rndis_emac_sta = RNDIS_EMAC_STA_START;
                    break;
                }

                usbd_rndis_in_emac_rx_polling(RNDIS_IN_STA_START);
                usbd_rndis_out_emac_tx_polling(RNDIS_OUT_STA_START);

                time_ms = bflb_mtimer_get_time_ms();

                /* emac link check */
                if (time_ms - status_time_ms > 100) {
                    /* wait usb in/out busy */
                    status_time_ms = time_ms;
                    usbd_rndis_emac_sta = RNDIS_EMAC_STA_WAIT_EPHY_LINKUP;
                }

                /* info dump */
                if (time_ms - info_dump_time_ms > 5000) {
                    info_dump_time_ms = time_ms;
                    eth_eamc_info_dump();
                }
                break;

            default:
                LOG_E("usbd_rndis_emac_sta error: %d\r\n", usbd_rndis_emac_sta);
                usbd_rndis_emac_sta = RNDIS_EMAC_STA_STOP;
                break;
        }
    }
}

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
