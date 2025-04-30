#include "bflb_mtimer.h"
#include "bflb_emac.h"

#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_ecm.h"

#include "eth_phy.h"
#include "ephy_general.h"
#include "ephy_lan8720.h"

#include "board.h"

#include "ring_buffer.h"

#define DBG_TAG "ECM"
#include "log.h"

/* MTU */
#define ECM_EMAC_NETIF_MTU (1514)

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
#define CDC_ECM_IN_EP      0x81
#define CDC_ECM_OUT_EP     0x02
#define CDC_ECM_INT_EP     0x83

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE    (9 + CDC_ECM_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

#define CDC_ECM_ETH_STATISTICS_BITMAP 0x00000000

/* str idx = 4 is for mac address: aa:bb:cc:dd:ee:ff*/
#define CDC_ECM_MAC_STRING_INDEX      4

/*!< global descriptor */
static const uint8_t cdc_ecm_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x04, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ECM_DESCRIPTOR_INIT(0x00, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP, CDC_MAX_MPS, CDC_ECM_ETH_STATISTICS_BITMAP, ECM_EMAC_NETIF_MTU, 0, 0, CDC_ECM_MAC_STRING_INDEX),
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
    0x2E,                       /* bLength */
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
    ' ', 0x00,                  /* wcChar9 */
    'C', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'E', 0x00,                  /* wcChar14 */
    'C', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    ' ', 0x00,                  /* wcChar17 */
    'D', 0x00,                  /* wcChar18 */
    'E', 0x00,                  /* wcChar19 */
    'M', 0x00,                  /* wcChar20 */
    'O', 0x00,                  /* wcChar21 */
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
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

/* emac buff cfg */
/* rx buff cnt */
#define EMAC_RX_BUFF_CNT    (32) /* < EMAC_RX_BD_BUM_MAX */
/* rx buff size */
#define EMAC_RX_BUFF_SIZE   (14 + 4 + 1500 + 4)
/* tx buff size */
#define EMAC_TX_BUFF_CNT    (32) /* < EMAC_TX_BD_BUM_MAX */
/* tx buff cnt */
#define EMAC_TX_BUFF_SIZE   (14 + 4 + 1500 + 4)
/* emac min frame size  */
#define EMAC_FRAME_SIZE_MIN (14 + 46 + 4)
/* emac max frame size  */
#define EMAC_FRAME_SIZE_MAX (14 + 4 + 1500 + 4)

#define EMAC_DESC_SIZE      (sizeof(struct bflb_emac_trans_desc_s))

/* emac and ephy */
struct bflb_device_s *emac0;
eth_phy_ctrl_t phy_ctrl;

/* emac rx/tx desc ring buffer */
Ring_Buffer_Type rb_emac_desc_dnld;
Ring_Buffer_Type rb_emac_desc_upld;
ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) uint8_t rb_emac_desc_dnld_buff[EMAC_DESC_SIZE * (EMAC_RX_BUFF_CNT + 1)];
ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) uint8_t rb_emac_desc_upld_buff[EMAC_DESC_SIZE * (EMAC_TX_BUFF_CNT + 1)];

/* emac data buff */
ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) uint8_t emac_rx_buff[EMAC_RX_BUFF_CNT][EMAC_RX_BUFF_SIZE];
ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) uint8_t emac_tx_buff[EMAC_TX_BUFF_CNT][EMAC_TX_BUFF_SIZE];

/* emac tx debug info */
static volatile uint32_t tx_push_cnt = 0;
static volatile uint32_t tx_success_cnt = 0;
static volatile uint32_t tx_error_cnt = 0;
static volatile uint64_t tx_total_size = 0;
/* emac rx debug info */
static volatile uint32_t rx_push_cnt = 0;
static volatile uint32_t rx_success_cnt = 0;
static volatile uint32_t rx_error_cnt = 0;
static volatile uint32_t rx_busy_cnt = 0;
static volatile uint64_t rx_total_size = 0;

/* ecm debug info */
static volatile uint32_t ecm_upld_cnt = 0;
static volatile uint64_t ecm_upld_size = 0;
static volatile uint32_t ecm_dnld_cnt = 0;
static volatile uint64_t ecm_dnld_size = 0;

/* usb ecm status flag */
volatile bool ecm_tx_busy_flag = false;
volatile bool ecm_rx_busy_flag = false;
struct bflb_emac_trans_desc_s ecm_upld_desc_using;
struct bflb_emac_trans_desc_s ecm_dnld_desc_using;

/* emac link_up status flag */
volatile bool emac_link_up_flag = false;
volatile bool usb_ecm_ready_flag = false;

/* phy cfg */
static eth_phy_init_cfg_t phy_cfg = {
    .speed_mode = EPHY_SPEED_MODE_AUTO_NEGOTIATION,
    .local_auto_negotiation_ability = EPHY_ABILITY_100M_TX | EPHY_ABILITY_100M_FULL_DUPLEX,
};

/* emac cfg */
static struct bflb_emac_config_s emac_cfg = {
    .mac_addr = { MAC_ADDR_NUM_0, MAC_ADDR_NUM_1, MAC_ADDR_NUM_2, MAC_ADDR_NUM_3, MAC_ADDR_NUM_4, MAC_ADDR_NUM_5 },
    .md_clk_div = 39,
    .min_frame_len = EMAC_FRAME_SIZE_MIN,
    .max_frame_len = EMAC_FRAME_SIZE_MAX,
};

/* ringbuff lock/unlock */
static volatile uintptr_t irq_flag;
static void rb_lock_cb(void)
{
    irq_flag = bflb_irq_save();
}
static void rb_unlock_cb(void)
{
    bflb_irq_restore(irq_flag);
}

/* ecm tx/rx done callback  */
static void emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc)
{
    switch (irq_event) {
        case EMAC_IRQ_EVENT_RX_BUSY:
            LOG_W("rx busy\r\n");
            /* debug */
            rx_busy_cnt++;
            break;

        case EMAC_IRQ_EVENT_RX_CTRL_FRAME:
            LOG_W("rx ctrl frame, drop.\r\n");
            trans_desc->attr_flag = 0;
            trans_desc->err_status = 0;
            bflb_emac_queue_rx_push(emac0, trans_desc);
            /* debug */
            rx_push_cnt++;
            rx_success_cnt++;
            rx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_RX_ERR_FRAME:
            LOG_W("rx err frame sta %d, drop.\r\n", trans_desc->err_status);
            trans_desc->attr_flag = 0;
            trans_desc->err_status = 0;
            bflb_emac_queue_rx_push(emac0, trans_desc);
            /* debug */
            rx_push_cnt++;
            rx_error_cnt++;
            break;

        case EMAC_IRQ_EVENT_RX_FRAME:
            Ring_Buffer_Write(&rb_emac_desc_upld, (uint8_t *)trans_desc, EMAC_DESC_SIZE);
            /* debug */
            rx_success_cnt++;
            rx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_TX_FRAME:
            Ring_Buffer_Write(&rb_emac_desc_dnld, (uint8_t *)trans_desc, EMAC_DESC_SIZE);
            /* debug */
            tx_success_cnt++;
            tx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_TX_ERR_FRAME:
            Ring_Buffer_Write(&rb_emac_desc_dnld, (uint8_t *)trans_desc, EMAC_DESC_SIZE);
            /* debug */
            if (trans_desc->err_status & (~EMAC_TX_STA_ERR_CS)) {
                LOG_W("tx err sta:%d\r\n", trans_desc->err_status);
                tx_error_cnt++;
            } else {
                tx_success_cnt++;
                tx_total_size += trans_desc->data_len;
            }
            break;

        default:
            break;
    }
}

/* emac/ephy init */
int emac_init()
{
    int ret;

    /* emac init */
    emac0 = bflb_device_get_by_name("emac0");
    if (emac0 == NULL) {
        LOG_E("device_get error\r\n");
        return -1;
    }
    bflb_emac_init(emac0, &emac_cfg);
    bflb_emac_irq_attach(emac0, emac_irq_cb, NULL);
    bflb_emac_feature_control(emac0, EMAC_CMD_SET_RX_PROMISCUOUS, false);

    /* scan eth_phy */
    ret = eth_phy_scan(&phy_ctrl, EPHY_ADDR_MIN, EPHY_ADDR_MAX);
    if (ret < 0) {
        return -2;
    }

    /* eth_phy init */
    ret = eth_phy_init(&phy_ctrl, &phy_cfg);
    if (ret < 0) {
        return -3;
    }

    /* LAN8720 Timing Adjustment: When in ref_clk input mode, invert the rx_clk. */
    if( (emac_cfg.clk_internal_mode == false) &&
        (phy_ctrl.phy_drv->phy_id == EPHY_LAN8720_ID)) {
        LOG_W("Invert rx_clk for LAN8720 Timing Adjustment.\r\n");
        bflb_emac_feature_control(emac0, EMAC_CMD_SET_MAC_RX_CLK_INVERT, true);
    }

    /* tx ringbuff init */
    Ring_Buffer_Init(&rb_emac_desc_upld, (uint8_t *)rb_emac_desc_upld_buff, sizeof(rb_emac_desc_upld_buff), rb_lock_cb, rb_unlock_cb);

    /* rx queue init */
    Ring_Buffer_Init(&rb_emac_desc_dnld, (uint8_t *)rb_emac_desc_dnld_buff, sizeof(rb_emac_desc_dnld_buff), rb_lock_cb, rb_unlock_cb);

    return 0;
}

/* usb event handler */
void usbd_event_handler(uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            LOG_W("USBD_EVENT_RESET\r\n");
            ecm_rx_busy_flag = false;
            ecm_tx_busy_flag = false;
            break;
        case USBD_EVENT_CONNECTED:
            LOG_W("USBD_EVENT_CONNECTED\r\n");
            break;
        case USBD_EVENT_DISCONNECTED:
            usb_ecm_ready_flag = false;
            emac_link_up_flag = false;
            LOG_W("USBD_EVENT_DISCONNECTED\r\n");
            break;
        case USBD_EVENT_RESUME:
            LOG_W("USBD_EVENT_RESUME\r\n");
            usb_ecm_ready_flag = true;
            break;
        case USBD_EVENT_SUSPEND:
            LOG_W("USBD_EVENT_SUSPEND\r\n");
            usb_ecm_ready_flag = false;
            emac_link_up_flag = false;
            break;
        case USBD_EVENT_CONFIGURED:
            LOG_W("USBD_EVENT_CONFIGURED\r\n");
            /* EMAC transmit start */
            usbd_cdc_ecm_send_connect_status(false);
            usb_ecm_ready_flag = true;
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            LOG_W("USBD_EVENT_SET_REMOTE_WAKEUP\r\n");
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            LOG_W("USBD_EVENT_CLR_REMOTE_WAKEUP\r\n");
            break;

        default:
            break;
    }
}

struct usbd_interface intf0;
struct usbd_interface intf1;

/* ecm only supports in linux, and you should input the following command
 * 
 * sudo ifconfig enxaabbccddeeff up
 * sudo dhcpclient enxaabbccddeeff
*/
void cdc_ecm_init(void)
{
    usbd_desc_register(cdc_ecm_descriptor);
    usbd_add_interface(usbd_cdc_ecm_init_intf(&intf0, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP));
    usbd_add_interface(usbd_cdc_ecm_init_intf(&intf1, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP));
    usbd_initialize();
}

/* ephy status update */
void ecm_emac_status_poll()
{
    static int speed_mode;
    static int link_sta;
    static bool ecm_ready = false;

    /* reset emac */
    if (ecm_ready == true && usb_ecm_ready_flag == false) {
        ecm_ready = false;

        LOG_W("EMAC STOP!\r\n");

        emac_link_up_flag = false;
        if (emac0) {
            /* disable tx and rx, and clean tx/rx bd */
            bflb_emac_deinit(emac0);
        }
    }

    /* init emac */
    if (ecm_ready == false && usb_ecm_ready_flag == true) {
        ecm_ready = true;

        LOG_W("EMAC START!\r\n");

        emac_link_up_flag = false;
        link_sta = 0;
        speed_mode = 0;

        do {
            if (emac_init() < 0) {
                LOG_E("emac_init failed, retry...\r\n");
                bflb_mtimer_delay_ms(1000);
            } else {
                LOG_I("emac_init OK\r\n");
                break;
            }
        } while (1);
    }

    if (usb_ecm_ready_flag == false) {
        return;
    }

    int sta = eth_phy_ctrl(&phy_ctrl, EPHY_CMD_GET_LINK_STA, 0);
    int speed = eth_phy_ctrl(&phy_ctrl, EPHY_CMD_GET_SPEED_MODE, 0);

    if (sta == EPHY_LINK_STA_UP && link_sta != EPHY_LINK_STA_UP) {
        LOG_W("ECM Eth Emac LinkUp !!!\r\n");

        /* ecm_dnld/emac_tx queue reinit */
        Ring_Buffer_Reset(&rb_emac_desc_dnld);
        for (int i = 0; i < EMAC_TX_BUFF_CNT; i++) {
            struct bflb_emac_trans_desc_s tx_desc = {
                .buff_addr = emac_tx_buff[i],
            };
            Ring_Buffer_Write(&rb_emac_desc_dnld, (uint8_t *)&tx_desc, EMAC_DESC_SIZE);
        }
        /* ecm_upld/emac_rx queue reinit */
        Ring_Buffer_Reset(&rb_emac_desc_upld);
        for (int i = 0; i < EMAC_RX_BUFF_CNT; i++) {
            struct bflb_emac_trans_desc_s rx_desc = {
                .buff_addr = emac_rx_buff[i],
            };
            bflb_emac_queue_rx_push(emac0, &rx_desc);
            rx_push_cnt += 1;
        }

        /* enable full duplex mode */
        if (speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX || speed == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
            bflb_emac_feature_control(emac0, EMAC_CMD_SET_FULL_DUPLEX, true);
        } else {
            bflb_emac_feature_control(emac0, EMAC_CMD_SET_FULL_DUPLEX, false);
        }
        /* enable tx and rx */
        bflb_emac_feature_control(emac0, EMAC_CMD_SET_TX_EN, true);
        bflb_emac_feature_control(emac0, EMAC_CMD_SET_RX_EN, true);

        emac_link_up_flag = true;
        usbd_cdc_ecm_send_connect_status(true);

    } else if (sta != EPHY_LINK_STA_UP && link_sta == EPHY_LINK_STA_UP) {
        LOG_W("ECM Eth Emac LinkDown !!!\r\n");
        usbd_cdc_ecm_send_connect_status(false);
        emac_link_up_flag = false;

        /* disable tx and rx, and clean tx/rx bd */
        bflb_emac_feature_control(emac0, EMAC_CMD_SET_TX_EN, false);
        bflb_emac_feature_control(emac0, EMAC_CMD_SET_RX_EN, false);
        bflb_emac_bd_ctrl_clean(emac0);
        bflb_mtimer_delay_us(200);

        speed_mode = 0;
    }
    link_sta = sta;

    if (link_sta != EPHY_LINK_STA_UP) {
        return;
    }

    if (speed != speed_mode) {
        speed_mode = speed;
        if (speed_mode == EPHY_SPEED_MODE_10M_HALF_DUPLEX) {
            LOG_I("eth_phy speed: 10M_HALF_DUPLEX\r\n");
        } else if (speed_mode == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
            LOG_I("eth_phy speed: 10M_FULL_DUPLEX\r\n");
        } else if (speed_mode == EPHY_SPEED_MODE_100M_HALF_DUPLEX) {
            LOG_I("eth_phy speed: 100M_HALF_DUPLEX\r\n");
        } else if (speed_mode == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
            LOG_I("eth_phy speed: 100M_FULL_DUPLEX\r\n");
        }

        uint32_t peed_table[2] = { (100 * 1000 * 1000), (100 * 1000 * 1000) }; /* upstrem, downstrem */
        usbd_cdc_ecm_set_connect_speed(peed_table);
    }
}

/* device has received the packet from the host. */
void usbd_cdc_ecm_packet_recv_done(uint32_t nbytes)
{
    ecm_dnld_cnt++;
    ecm_dnld_size += nbytes;

    ecm_rx_busy_flag = false;

    if (emac_link_up_flag == false) {
        return;
    }

    // LOG_E("ecm_recv_done size: %d\r\n", nbytes);

    // uint8_t *buff = (uint8_t *)ecm_dnld_desc_using.buff_addr;
    // LOG_E("ECM-DNLD: dest: %02X:%02X:%02X:%02X:%02X:%02X, src:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
    //       buff[0], buff[1], buff[2], buff[3], buff[4], buff[5], buff[6], buff[7], buff[8], buff[9], buff[10], buff[11]);

    /* usb receive done, push the desc to emac tx buff */
    ecm_dnld_desc_using.data_len = nbytes;
    bflb_emac_queue_tx_push(emac0, &ecm_dnld_desc_using);
    tx_push_cnt += 1;
}

/* device has sent the package to the host */
void usbd_cdc_ecm_packet_send_done(uint32_t nbytes)
{
    ecm_upld_cnt++;
    ecm_upld_size += nbytes;

    ecm_tx_busy_flag = false;

    if (emac_link_up_flag == false) {
        return;
    }

    // LOG_W("ecm_send_done size: %d\r\n", nbytes);

    /* usb send done, push the desc to emac rx buff */
    bflb_emac_queue_rx_push(emac0, &ecm_upld_desc_using);
    rx_push_cnt += 1;
}

/* ecm upld */
void ecm_send_poll()
{
    if (emac_link_up_flag == false) {
        return;
    }

    if ((ecm_tx_busy_flag == false) && (Ring_Buffer_Get_Length(&rb_emac_desc_upld) > EMAC_DESC_SIZE)) {
        /* get emac_rx/ecm_upld buff desc */
        Ring_Buffer_Read(&rb_emac_desc_upld, (uint8_t *)&ecm_upld_desc_using, EMAC_DESC_SIZE);

        // uint8_t *buff = (uint8_t *)ecm_upld_desc_using.buff_addr;
        // LOG_W("ECM-UPLD: dest: %02X:%02X:%02X:%02X:%02X:%02X, src:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
        //       buff[0], buff[1], buff[2], buff[3], buff[4], buff[5], buff[6], buff[7], buff[8], buff[9], buff[10], buff[11]);

        /* Starts sending a packet to host */
        ecm_tx_busy_flag = true;
        usbd_cdc_ecm_send_packet((uint8_t *)(uintptr_t)ecm_upld_desc_using.buff_addr, ecm_upld_desc_using.data_len);
    }
}
/* ecm dnld */
void ecm_receive_poll()
{
    if (emac_link_up_flag == false) {
        return;
    }

    if (ecm_rx_busy_flag == false && (Ring_Buffer_Get_Length(&rb_emac_desc_dnld) > EMAC_DESC_SIZE)) {
        /* get emac_tx/ecm_dnld buff desc */
        Ring_Buffer_Read(&rb_emac_desc_dnld, (uint8_t *)&ecm_dnld_desc_using, EMAC_DESC_SIZE);

        /* starts receiving a packet from host  */
        ecm_rx_busy_flag = true;
        usbd_cdc_ecm_receive_packet((uint8_t *)(uintptr_t)ecm_dnld_desc_using.buff_addr, EMAC_TX_BUFF_SIZE);
    }
}

void ecm_eamc_info_dump()
{
    uint32_t tx_db_avail = bflb_emac_feature_control(emac0, EMAC_CMD_GET_TX_DB_AVAILABLE, 0);
    uint32_t rx_db_avail = bflb_emac_feature_control(emac0, EMAC_CMD_GET_RX_DB_AVAILABLE, 0);

    LOG_RI("\r\n");
    LOG_I("USB-ECM DNLD: success cnt:%d, total size:%lldByte\r\n", ecm_dnld_cnt, ecm_dnld_size);
    LOG_I("        dnld queue_cnt:%d\r\n", Ring_Buffer_Get_Length(&rb_emac_desc_dnld) / EMAC_DESC_SIZE);
    LOG_I("USB-ECM UPLD: success cnt:%d, total size:%lldByte\r\n", ecm_upld_cnt, ecm_upld_size);
    LOG_I("        upld queue_cnt:%d\r\n", Ring_Buffer_Get_Length(&rb_emac_desc_upld) / EMAC_DESC_SIZE);

    if (emac0 == NULL) {
        return;
    }

    LOG_I("TX: success cnt:%d, error cnt:%d, total size:%lldByte\r\n", tx_success_cnt, tx_error_cnt, tx_total_size);
    LOG_I("    push_cnt:%d, tx_db waiting:%d, tx_bd_ptr:%d\r\n", tx_push_cnt, (EMAC_TX_BD_BUM_MAX - tx_db_avail), bflb_emac_feature_control(emac0, EMAC_CMD_GET_TX_BD_PTR, 0));

    LOG_I("RX: success cnt:%d, error cnt:%d, total size:%lldByte\r\n", rx_success_cnt, rx_error_cnt, rx_total_size);
    LOG_I("    push_cnt:%d, rx_db waiting:%d, rx_bd_ptr:%d, busy cnt:%d\r\n", rx_push_cnt, (EMAC_RX_BD_BUM_MAX - rx_db_avail), bflb_emac_feature_control(emac0, EMAC_CMD_GET_RX_BD_PTR, 0), rx_busy_cnt);
    LOG_RI("\r\n");

}

void ecm_emac_poll()
{
    static uint64_t status_time_ms = 0;
    static uint64_t info_dump_time_ms = 0;

    if (bflb_mtimer_get_time_ms() - status_time_ms > 100) {
        status_time_ms = bflb_mtimer_get_time_ms();
        ecm_emac_status_poll();
    }

    if (bflb_mtimer_get_time_ms() - info_dump_time_ms > 5000) {
        info_dump_time_ms = bflb_mtimer_get_time_ms();
        ecm_eamc_info_dump();
    }

    ecm_send_poll();

    ecm_receive_poll();
}
