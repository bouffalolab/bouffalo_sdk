#ifndef _BFLB_EMAC_COMMON_H
#define _BFLB_EMAC_COMMON_H

#include "bflb_core.h"

#if defined(BL616L) || defined(BL616D)
#define EMAC_SPEED_10M_SUPPORT (1)
#else
#define EMAC_SPEED_10M_SUPPORT (0)
#endif

#ifndef EMAC_TX_BD_NUM_MAX
#define EMAC_TX_BD_NUM_MAX (64)
#endif

#ifndef EMAC_RX_BD_NUM_MAX
#define EMAC_RX_BD_NUM_MAX (64)
#endif

#if (EMAC_TX_BD_NUM_MAX & (EMAC_TX_BD_NUM_MAX - 1) != 0) || EMAC_TX_BD_NUM_MAX > 64
#error "emac tx bd num error, must be 2^n and <= 64"
#else
#define EMAC_TX_BD_NUM_MASK (EMAC_TX_BD_NUM_MAX - 1)
#endif

#if (EMAC_RX_BD_NUM_MAX & (EMAC_RX_BD_NUM_MAX - 1) != 0) || EMAC_RX_BD_NUM_MAX > 64
#error "emac tx bd num error, must be 2^n and <= 64"
#else
#define EMAC_RX_BD_NUM_MASK (EMAC_RX_BD_NUM_MAX - 1)
#endif

/* ETH packet size */
/* ETH     | Header | Extra | VLAN tag | Payload   | CRC | */
/* Size    | 14     | 2     | 4        | 46 ~ 1500 | 4   | */
#define ETH_MAX_PACKET_SIZE          ((uint32_t)1524U) /*!< ETH_HEADER + ETH_EXTRA + ETH_VLAN_TAG + ETH_MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_HEADER_SZIE              ((uint32_t)14U)   /*!< 6 byte Dest addr, 6 byte Src addr, 2 byte length/type */
#define ETH_CRC_SIZE                 ((uint32_t)4U)    /*!< Ethernet CRC */
#define ETH_EXTRA_SIZE               ((uint32_t)2U)    /*!< Extra bytes in some cases */
#define ETH_VLAN_TAG_SIZE            ((uint32_t)4U)    /*!< optional 802.1q VLAN Tag */
#define ETH_MIN_ETH_PAYLOAD_SIZE     ((uint32_t)46U)   /*!< Minimum Ethernet payload size */
#define ETH_MAX_ETH_PAYLOAD_SIZE     ((uint32_t)1500U) /*!< Maximum Ethernet payload size */
#define ETH_JUMBO_FRAME_PAYLOAD_SIZE ((uint32_t)9000U) /*!< Jumbo frame payload size */

/* ETH tx & rx buffer size */
#ifndef ETH_TX_BUFFER_SIZE
#define ETH_TX_BUFFER_SIZE (ETH_MAX_PACKET_SIZE)
#endif
#ifndef ETH_RX_BUFFER_SIZE
#define ETH_RX_BUFFER_SIZE (ETH_MAX_PACKET_SIZE)
#endif

#define EAMC_HALFDUPLEX              0
#define EAMC_FULLDUPLEX              1

/* feature CMD */
#define EMAC_CMD_SET_TX_EN           (0)
#define EMAC_CMD_SET_TX_AUTO_PADDING (1)
#define EMAC_CMD_SET_TX_CRC_FIELD_EN (2)
#define EMAC_CMD_SET_TX_PREAMBLE     (3)
#define EMAC_CMD_SET_TX_GAP_CLK      (4)
#define EMAC_CMD_SET_TX_COLLISION    (5)
#define EMAC_CMD_SET_TX_MAXRET       (6)
#define EMAC_CMD_SET_RX_EN           (7)
#define EMAC_CMD_SET_RX_SMALL_FRAME  (8)
#define EMAC_CMD_SET_RX_HUGE_FRAME   (9)
#define EMAC_CMD_SET_RX_GAP_CHECK    (10)
#define EMAC_CMD_SET_RX_PROMISCUOUS  (11)
#define EMAC_CMD_SET_RX_BROADCASE    (12)
#define EMAC_CMD_SET_FULL_DUPLEX     (13)
#define EMAC_CMD_SET_SPEED_100M      (14)
#if (EMAC_SPEED_10M_SUPPORT)
#define EMAC_CMD_SET_SPEED_10M (15)
#endif
#define EMAC_CMD_SET_MAC_RX_CLK_INVERT (16)
#define EMAC_CMD_GET_TX_DB_AVAILABLE   (20)
#define EMAC_CMD_GET_RX_DB_AVAILABLE   (21)
#define EMAC_CMD_GET_TX_BD_PTR         (22)
#define EMAC_CMD_GET_RX_BD_PTR         (23)
#define EMAC_CMD_SET_PHY_ADDRESS       (24)
#define EMAC_CMD_SET_SPEED_1000M       (25)

/* irq callback event */
#define EMAC_IRQ_EVENT_RX_BUSY         (1)
#define EMAC_IRQ_EVENT_RX_FRAME        (2)
#define EMAC_IRQ_EVENT_RX_CTRL_FRAME   (3)
#define EMAC_IRQ_EVENT_RX_ERR_FRAME    (4)
#define EMAC_IRQ_EVENT_TX_FRAME        (5)
#define EMAC_IRQ_EVENT_TX_ERR_FRAME    (6)

/* tx attribute flag */
#define EMAC_TX_FLAG_FRAGMENT          (1 << 0) /* This BD does not contain EoF and controller will proceed to read out the next BD */
#define EMAC_TX_FLAG_NO_INT            (1 << 1) /* No interrupt is generated after the transmission */
#define EMAC_TX_FLAG_NO_CRC            (1 << 2) /* CRC would not be attached to the end of the packet */
#define EMAC_TX_FLAG_NO_PAD            (1 << 3) /* No padding is appended to the end of the short packet */
/* tx done err status */
#define EMAC_TX_STA_ERR_COLLISION      (1 << 0) /* Late Collision */
#define EMAC_TX_STA_ERR_CS             (1 << 1) /* Carrier Sense Lost */
#define EMAC_TX_STA_ERR_RETRY_LIMIT    (1 << 2) /* Retransmission Limit*/
#define EMAC_TX_STA_ERR_FIFO           (1 << 3)
#define EMAC_TX_STA_ERR_UNKNOWN        (1 << 4) /* Error with unknown */

/* rx attribute flag */
#define EMAC_RX_FLAG_NO_INT            (1 << 1)
/* rx done err status */
#define EMAC_RX_STA_ERR_CRC            (1 << 0)
#define EMAC_RX_STA_ERR_COLLISION      (1 << 1)
#define EMAC_RX_STA_ERR_LONG_FRAME     (1 << 2)
#define EMAC_RX_STA_ERR_FIFO           (1 << 3)

struct bflb_emac_config_s {
    uint8_t mac_addr[6];
    bool clk_internal_mode;
    uint8_t md_clk_div;
    uint8_t duplex_mode;
    uint16_t speed;
    uint16_t min_frame_len;
    uint16_t max_frame_len;
};

struct bflb_emac_trans_desc_s {
    void *buff_addr;    /* buff address */
    uint16_t data_len;  /* data len */
    uint8_t attr_flag;  /* attribute flag */
    uint8_t err_status; /* err_status */
};

typedef void (*bflb_emac_irq_cb_t)(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc);

#ifdef __cplusplus
extern "C" {
#endif

int bflb_emac_init(struct bflb_device_s *dev, const struct bflb_emac_config_s *config);
int bflb_emac_deinit(struct bflb_device_s *dev);
int bflb_emac_queue_tx_push(struct bflb_device_s *dev, struct bflb_emac_trans_desc_s *tx_desc);
int bflb_emac_queue_rx_push(struct bflb_device_s *dev, struct bflb_emac_trans_desc_s *rx_desc);
int bflb_emac_md_read(struct bflb_device_s *dev, uint8_t phy_addr, uint16_t reg_addr, uint16_t *data);
int bflb_emac_md_write(struct bflb_device_s *dev, uint8_t phy_addr, uint16_t reg_addr, uint16_t data);
int bflb_emac_bd_ctrl_clean(struct bflb_device_s *dev);
int bflb_emac_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);
int bflb_emac_irq_attach(struct bflb_device_s *dev, bflb_emac_irq_cb_t irq_event_cb, void *arg);

#ifdef __cplusplus
}
#endif

#endif