/**
  ******************************************************************************
  * @file    bflb_emac.h
  * @version V1.0
  * @date    2022-09-27
  * @brief   This file is the low hardware abstraction layer file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2022 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#ifndef _BFLB_EMAC_H
#define _BFLB_EMAC_H

#include "bflb_core.h"

#define EMAC_DO_FLUSH_DATA (1)

/* EMAC clock use external or use internal; 0: used external 1: used internal */
#define EMAC_CLK_USE_EXTERNAL (0)
#define EMAC_CLK_USE_INTERNAL (1)

/** @defgroup EMAC_CMD emac feature control cmd definition
  * @{
  */
#define EMAC_CMD_NO_PREAMBLE_MODE (0x01)
#define EMAC_CMD_EN_PROMISCUOUS   (0x02)
#define EMAC_CMD_FRAME_GAP_CHECK  (0x03)
#define EMAC_CMD_FULL_DUPLEX      (0x04)
#define EMAC_CMD_EN_TX_CRC_FIELD  (0x05)
#define EMAC_CMD_RECV_HUGE_FRAMES (0x06)
#define EMAC_CMD_EN_AUTO_PADDING  (0x07)
#define EMAC_CMD_RECV_SMALL_FRAME (0x08)
#define EMAC_CMD_SET_PHY_ADDRESS  (0x09)
#define EMAC_CMD_SET_MAC_ADDRESS  (0x0A)
#define EMAC_CMD_SET_PACKET_GAP   (0x0B)
#define EMAC_CMD_SET_MIN_FRAME    (0x0C)
#define EMAC_CMD_SET_MAX_FRAME    (0x0D)
#define EMAC_CMD_SET_MAXRET       (0x0E)
#define EMAC_CMD_SET_COLLVALID    (0x0F)

/** @defgroup PHY state definition
  * @{
  */
#define PHY_STATE_DOWN    (0) /* PHY is not usable */
#define PHY_STATE_READY   (1) /* PHY is OK, wait for controller */
#define PHY_STATE_UP      (2) /* Network is ready for TX/RX */
#define PHY_STATE_RUNNING (3) /* working */
#define PHY_STATE_NOLINK  (4) /* no cable connected */
#define PHY_STATE_STOPPED (5) /* PHY has been stopped */
#define PHY_STATE_TESTING (6) /* in test mode */

/* EMAC PACKET */
#define EMAC_NORMAL_PACKET   (uint32_t)(0)
#define EMAC_FRAGMENT_PACKET (uint32_t)(0x01)
#define EMAC_NOCOPY_PACKET   (uint32_t)(0x02)

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

/* emac interrupt UNMASK/MASK define */
#define EMAC_INT_EN_TX_DONE  (1 << 0)
#define EMAC_INT_EN_TX_ERROR (1 << 1)
#define EMAC_INT_EN_RX_DONE  (1 << 2)
#define EMAC_INT_EN_RX_ERROR (1 << 3)
#define EMAC_INT_EN_RX_BUSY  (1 << 4)
#define EMAC_INT_EN_TX_CTRL  (1 << 5)
#define EMAC_INT_EN_RX_CTRL  (1 << 6)
#define EMAC_INT_EN_ALL      (0x7f << 0)

/* emac interrupt status define */
#define EMAC_INT_STS_TX_DONE  (1 << 0)
#define EMAC_INT_STS_TX_ERROR (1 << 1)
#define EMAC_INT_STS_RX_DONE  (1 << 2)
#define EMAC_INT_STS_RX_ERROR (1 << 3)
#define EMAC_INT_STS_RX_BUSY  (1 << 4)
#define EMAC_INT_STS_TX_CTRL  (1 << 5)
#define EMAC_INT_STS_RX_CTRL  (1 << 6)
#define EMAC_INT_STS_ALL      (0x7f << 0)

/* emac buffer descriptors type define */
#define EMAC_BD_TYPE_INVLAID (0)
#define EMAC_BD_TYPE_TX      (1)
#define EMAC_BD_TYPE_RX      (2)
#define EMAC_BD_TYPE_NONE    (3)
#define EMAC_BD_TYPE_MAX     (0x7FFFFFFF)

/**
 * @brief EMAC configuration structure
 *
 * @param mac_addr  EMAC mac addr
 * @param inside_clk EMAC select inside or external @ref EMAC_CLK_USE_EXTERNAL or EMAC_CLK_USE_INTERNAL
 * @param mii_clk_div mii clock div
 * @param min_frame_len min frame len
 * @param max_frame_len max frame len
 *
 */
struct bflb_emac_config_s {
    uint8_t mac_addr[6];
    uint8_t inside_clk;
    uint8_t mii_clk_div;
    uint16_t min_frame_len;
    uint16_t max_frame_len;
};

/**
 * @brief EMAC phy configuration structure
 *
 * @param auto_negotiation EMAC phy speed and mode auto negotiation
 * @param full_duplex      EMAC phy duplex mode
 * @param phy_state        EMAC phy down,ready,up,running,nolink,halted, @ref PHY_STATE
 * @param use_irq          EMAC phy interrupt enable 0: no IRQ used
 * @param speed            EMAC phy speed mode
 * @param phy_address      EMAC phy address
 * @param phy_id           EMAC phy read phy id
 */
struct emac_phy_cfg_s {
    uint8_t auto_negotiation;
    uint8_t full_duplex;
    uint8_t phy_state;
    uint8_t use_irq;
    uint16_t speed;
    uint16_t phy_address;
    uint32_t phy_id;
};

/**
 *  @brief Note: Always write DWORD1 (buffer addr) first then DWORD0 for racing concern.
 */
struct emac_bd_desc_s {
    uint32_t C_S_L;  /*!< Buffer Descriptors(BD) control,status,length */
    uint32_t Buffer; /*!< BD buffer address */
};

/**
 * @brief emac handle type definition
 * @param bd             Tx descriptor header pointer
 * @param tx_index_emac  TX index: EMAC
 * @param tx_index_cpu   TX index: CPU/SW
 * @param tx_buff_limit  TX index max
 * @param rsv0           rsv0
 * @param rx_index_emac  RX index: EMAC
 * @param rx_index_cpu   RX index: CPU/SW
 * @param rx_buff_limit  RX index max
 * @param rsv1           rsv1
 *
 */
struct emac_handle_s {
    struct emac_bd_desc_s *bd;
    uint8_t tx_index_emac;
    uint8_t tx_index_cpu;
    uint8_t tx_buff_limit;
    uint8_t rsv0;
    uint8_t rx_index_emac;
    uint8_t rx_index_cpu;
    uint8_t rx_buff_limit;
    uint8_t rsv1;
};

#ifdef __cplusplus
extern "C" {
#endif

void bflb_emac_init(struct bflb_device_s *dev, const struct bflb_emac_config_s *config);
void bflb_emac_bd_init(struct bflb_device_s *dev, uint8_t *eth_tx_buff, uint8_t tx_buf_count, uint8_t *eth_rx_buff, uint8_t rx_buf_count);
int emac_bd_fragment_support(void);
int bflb_emac_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);
void bflb_emac_int_enable(struct bflb_device_s *dev, uint32_t flag, bool enable);
void bflb_emac_int_clear(struct bflb_device_s *dev, uint32_t flag);
uint32_t bflb_emac_get_int_status(struct bflb_device_s *dev);
uint32_t bflb_emac_bd_get_cur_active(struct bflb_device_s *dev, uint8_t bdt);
void bflb_emac_bd_rx_enqueue(uint32_t index);
void bflb_emac_bd_rx_on_err(uint32_t index);
void bflb_emac_bd_tx_dequeue(uint32_t index);
void bflb_emac_bd_tx_on_err(uint32_t index);
int bflb_emac_bd_tx_enqueue(uint32_t flags, uint32_t len, const uint8_t *data_in);
int bflb_emac_bd_rx_dequeue(uint32_t flags, uint32_t *len, uint8_t *data_out);
int bflb_emac_phy_reg_read(struct bflb_device_s *dev, uint16_t phy_reg, uint16_t *phy_reg_val);
int bflb_emac_phy_reg_write(struct bflb_device_s *dev, uint16_t phy_reg, uint16_t phy_reg_val);
void bflb_emac_stop(struct bflb_device_s *dev);
void bflb_emac_start(struct bflb_device_s *dev);
void bflb_emac_start_tx(struct bflb_device_s *dev);
void bflb_emac_stop_tx(struct bflb_device_s *dev);
void bflb_emac_start_rx(struct bflb_device_s *dev);
void bflb_emac_stop_rx(struct bflb_device_s *dev);

#ifdef __cplusplus
}
#endif

#endif /* _BFLB_EMAC_H */