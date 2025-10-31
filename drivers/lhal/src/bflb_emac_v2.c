/**
  ******************************************************************************
  * @file    bflb_emac_v2.c
  * @version V1.0
  * @date    2024-05-01
  * @brief   This file is the low hardware abstraction layer file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2024 Bouffalo Lab</center></h2>
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
#include "bflb_emac_v2.h"
#include "bflb_clock.h"
#include "bflb_l1c.h"

#define DEFAULT_LOOP_VARIABLE  1000
#define DEFAULT_DELAY_VARIABLE 10
#define emac_v2_delay_us       bflb_mtimer_delay_us

#if 0
#define EMAC_V2_DBG_ENABLE       1
#define EMAC_V2_DRV_DBG(a, ...)  printf(a, ##__VA_ARGS__)
#define EMAC_V2_DRV_ERR(a, ...)  printf(a, ##__VA_ARGS__)
#define EMAC_V2_DRV_WARN(a, ...) printf(a, ##__VA_ARGS__)
#else
//#define EMAC_V2_DBG_ENABLE 0
#define EMAC_V2_DRV_DBG(a, ...)
#define EMAC_V2_DRV_ERR(a, ...)  printf(a, ##__VA_ARGS__)
#define EMAC_V2_DRV_WARN(a, ...) printf(a, ##__VA_ARGS__)
#endif

/**
 * @brief emac_v2 handle type definition
 * @param tx_index_emac_v2  TX index: EMAC_V2
 * @param tx_index_cpu   TX index: CPU/SW
 * @param tx_index_cnt TX index max
 * @param rsv0           rsv0
 * @param tx_dma_desc    tx first dma descriptor
 * @param rx_index_emac_v2  RX index: EMAC_V2
 * @param rx_index_cpu   RX index: CPU/SW
 * @param rx_index_cnt RX index max
 * @param rsv1           rsv1
 * @param rx_dma_desc    rx first dma descriptor
 *
 */
struct bflb_emac_v2_handle_s {
    uint16_t tx_index_emac_v2;
    uint16_t tx_index_cpu;
    uint16_t tx_index_cnt;
    uint16_t tx_busy_cnt;
    struct bflb_emac_v2_dma_desc_s *tx_dma_desc;
    uint32_t tx_bytes;
    uint32_t tx_packets;
    uint32_t tx_errors;
    uint32_t tx_aborted_errors;
    uint32_t tx_carrier_errors;
    uint32_t collisions;

    uint16_t rx_index_emac_v2;
    uint16_t rx_index_cpu;
    uint16_t rx_index_cnt;
    uint16_t rx_idle_cnt;
    struct bflb_emac_v2_dma_desc_s *rx_dma_desc;
    uint32_t rx_bytes;
    uint32_t rx_packets;
    uint32_t rx_errors;
    uint32_t rx_crc_errors;
    uint32_t rx_frame_errors;
    uint32_t rx_length_errors;
};

static struct bflb_emac_v2_handle_s eth_handle;
static struct bflb_emac_v2_handle_s *thiz = NULL;

static ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(4))) struct bflb_emac_v2_dma_desc_s tx_dma_desc[EMAC_TX_BD_NUM_MAX];
static ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(4))) struct bflb_emac_v2_dma_desc_s rx_dma_desc[EMAC_RX_BD_NUM_MAX];

static void bflb_emac_v2_isr(int irq, void *arg);

/* isr event callback */
static void *emac_irq_arg = NULL;
static bflb_emac_irq_cb_t emac_irq_event_cb = NULL;

/**
 * @brief bflb emac_v2 enable the watchdog timer on the receiver.
 *
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_wd_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_WATCHDOG_DISABLE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable the watchdog timer on the receiver.
 *
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_wd_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_WATCHDOG_DISABLE_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enables the Jabber frame support. 
 *
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_jabber_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    /*when enabled, disable the jabber timer and can transfer 16,384 byte frames */
    reg_val |= EMAC_V2_JABBER_TIMER_DISABLE_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disables the Jabber frame support. 
 *
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_jabber_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    /* When disabled, EMAC_V2 enables jabber timer. It cuts of transmitter if application */
    /* sends more than 2048 bytes of data (10240 if Jumbo frame enabled). */
    reg_val &= ~(EMAC_V2_JABBER_TIMER_DISABLE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable frame burst. 
 * Enables Frame bursting (Only in Half Duplex Mode). 
 * When enabled, EMAC_V2 allows frame bursting in GMII Half Duplex mode.
 * Reserved in 10/100 and Full-Duplex configurations.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_frame_burst_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_FRAME_BURST_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable frame burst. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_frame_burst_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_FRAME_BURST_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable jumbo frame. 
 * Enable Jumbo frame support. 
 * When Enabled EMAC_V2 supports jumbo frames of 9018/9022(VLAN tagged).
 * Giant frame error is not reported in receive frame status.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_jumbo_frame_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_JUMBO_FRAME_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable frame burst. 
 * When Disabled EMAC_V2 does not supports jumbo frames.
 * Giant frame error is reported in receive frame status.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_jumbo_frame_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_JUMBO_FRAME_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable carrier sense. 
 * When Disabled EMAC_V2 ignores CRS signal during frame transmission
 * in half duplex mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_crs_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_DISABLE_CRS_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 select gmii interface. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_gmii_select(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_MII_GMII_SELECT_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 select mii interface. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_mii_select(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_MII_GMII_SELECT_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 select mii interface. 
 * 
 * @param dev
 *
 * @param speed
 * 
 * @return 0 for success
 *
 */
int bflb_emac_v2_mii_speed_set(struct bflb_device_s *dev, uint32_t speed)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    if (speed == 100) {
        reg_val |= EMAC_V2_FE_100M_MASK;
    } else if (speed == 10) {
        reg_val &= (~EMAC_V2_FE_100M_MASK);
    }
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable rx own frame. 
 * Enables Receive Own bit (Only in Half Duplex Mode). 
 * When enaled EMAC_V2 receives all the packets given by phy while transmitting.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_rx_own_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_RX_OWN_DISABLE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable rx own frame. 
 * Disables Receive Own bit (Only in Half Duplex Mode). 
 * When enaled EMAC_V2 disables the reception of frames when gmii_txen_o is asserted.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_rx_own_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_RX_OWN_DISABLE_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable loopback mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_loopback_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_LOOPBACK_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable loopback mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_loopback_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_LOOPBACK_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable full duplex mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_full_duplex_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_DUPLEX_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable half duplex mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_half_duplex_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_DUPLEX_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable retry.
 * EMAC_V2 tries retransmission (Only in Half Duplex mode).
 * If collision occurs on the GMII/MII, EMAC_V2 attempt retries based on the 
 * back off limit configured. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_retry_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_RETRY_DISABLE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable retry.
 * EMAC_V2 tries only one transmission (Only in Half Duplex mode).
 * If collision occurs on the GMII/MII, EMAC_V2 will ignore the current frami
 * transmission and report a frame abort with excessive collision in tranmit frame status. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_retry_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_RETRY_DISABLE_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 strips the Pad/FCS field of incoming frames.
 * This is true only if the length field value is less than or equal to
 * 1500 bytes. All received frames with length field greater than or equal to
 * 1501 bytes are passed to the application without stripping the Pad/FCS field. 
 *  
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_pad_crc_strip_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_PAD_CRC_STRIP_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable strips the Pad/FCS field of incoming frames.
 * EMAC_V2 doesnot strips the Pad/FCS field of incoming frames.
 * EMAC_V2 will pass all the incoming frames to Host unmodified. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_pad_crc_strip_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_PAD_CRC_STRIP_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 set back limits.
 * 
 * @param dev
 * 
 * @param value
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_back_off_limit_set(struct bflb_device_s *dev, uint32_t value)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_BACKOFF_LIMIT_MASK);
    reg_val |= ((value << EMAC_V2_BACKOFF_LIMIT_SHIFT) & EMAC_V2_BACKOFF_LIMIT_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable deferral check.
 * Enables the Deferral check in EMAC_V2 (Only in Half Duplex mode)
 * EMAC_V2 issues a Frame Abort Status, along with the excessive deferral error bit set in the 
 * transmit frame status when transmit state machine is deferred for more than
 *  - 24,288 bit times in 10/100Mbps mode
 *  - 155,680 bit times in 1000Mbps mode or Jumbo frame mode in 10/100Mbps operation.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_deferral_check_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_DEFERRAL_CHECK_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable deferral check.
 * Disables the Deferral check in EMAC_V2 (Only in Half Duplex mode).
 * EMAC_V2 defers until the CRS signal goes inactive.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_deferral_check_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_DEFERRAL_CHECK_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable the reception of frames on GMII/MII.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_rx_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_RX_ENABLE_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable the reception of frames on GMII/MII.
 * EMAC_V2 receive state machine is disabled after completion of reception of current frame.
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_rx_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_RX_ENABLE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable the transmission of frames on GMII/MII.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_tx_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_TX_ENABLE_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable the transmission of frames on GMII/MII.
 * EMAC_V2 transmit state machine is disabled after completion of transmission of current frame.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_tx_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= ~(EMAC_V2_TX_ENABLE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable reception of all the frames to application.
 * EMAC_V2 passes all the frames received to application irrespective of whether they
 * pass SA/DA address filtering or not.
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_frame_filter_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val &= ~(EMAC_V2_FILTER_OFF_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable reception of all the frames to application.
 * EMAC_V2 passes only those received frames to application which 
 * pass SA/DA address filtering.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_frame_filter_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val |= EMAC_V2_FILTER_OFF_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 set hash table high.
 * Populates the Hash High register with the data supplied.
 * This function is called when the Hash filtering is to be enabled.
 * 
 * @param dev
 *
 * @param data
 * 
 * @return 0 for success
 *
 */
int bflb_emac_v2_hash_table_high_set(struct bflb_device_s *dev, uint32_t data)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(data, reg_base + EMAC_V2_HASH_HIGH_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 set hash table low.
 * Populates the Hash Low register with the data supplied.
 * This function is called when the Hash filtering is to be enabled.
 * 
 * @param dev
 *
 * @param data
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_hash_table_low_set(struct bflb_device_s *dev, uint32_t data)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(data, reg_base + EMAC_V2_HASH_LOW_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable hash or perfect filter
 * Enables Hash or Perfect filter (only if Hash filter is enabled in H/W).
 * Only frames matching either perfect filtering or Hash Filtering as per HMC and HUC 
 * configuration are sent to application.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_hash_perfect_filter_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val |= EMAC_V2_HASH_PERFECT_FILTER_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable hash filter only
 * Enables only Hash(only if Hash filter is enabled in H/W).
 * Only frames matching Hash Filtering as per HMC and HUC 
 * configuration are sent to application.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_hash_filter_only_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val |= EMAC_V2_HASH_PERFECT_FILTER_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable src address filter
 * Enables Source address filtering.
 * When enabled source address filtering is performed. Only frames matching SA filtering are passed  to application with 
 * SAMatch bit of RxStatus is set. EMAC_V2 drops failed frames. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_src_addr_filter_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val |= EMAC_V2_SRC_ARRD_FILTER_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable src address filter
 * When disabled EMAC_V2 forwards the received frames with updated SAMatch bit in RxStatus. 
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_src_addr_filter_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val &= ~(EMAC_V2_SRC_ARRD_FILTER_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 inverse dst address filter
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_dst_addr_filter_inverse(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val |= EMAC_V2_SRC_INV_ARRD_FILTER_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 normal dst address filter
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_dst_addr_filter_normal(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val &= ~(EMAC_V2_SRC_INV_ARRD_FILTER_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 set forwarding of control frames.
 * When set forwards all the control frames (incl. unicast and multicast PAUSE frames).
 *
 * @param dev
 * 
 * @param val
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_pass_control_set(struct bflb_device_s *dev, uint32_t val)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val &= ~(EMAC_V2_PASS_CTRL_MASK);
    reg_val |= ((val << EMAC_V2_PASS_CTRL_SHIFT) & EMAC_V2_PASS_CTRL_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable Broadcast frames.
 * When enabled Address filtering module passes all incoming broadcast frames.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_broadcast_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val &= ~(EMAC_V2_BCAST_DISABLE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable Broadcast frames.
 * When disabled Address filtering module filters all incoming broadcast frames.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_broadcast_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val |= EMAC_V2_BCAST_DISABLE_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable Multicast frames.
  * When enabled all multicast frames are passed.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_multicast_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val |= EMAC_V2_MCAST_FILTER_OFF_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable Multicast frames.
 * When disabled multicast frame filtering depends on HMC bit.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_multicast_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val &= ~(EMAC_V2_MCAST_FILTER_OFF_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable multicast hash filtering.
 * When enabled EMAC_V2 performs teh destination address filtering according to the hash table.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_multicast_hash_filter_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val |= EMAC_V2_MCAST_HASH_FILTER_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable multicast hash filtering.
 * When disabled EMAC_V2 performs perfect destination address filtering for multicast frames, it compares 
 * DA field with the value programmed in DA register.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_multicast_hash_filter_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val &= ~(EMAC_V2_MCAST_HASH_FILTER_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable promiscous mode.
 * When enabled Address filter modules pass all incoming frames regardless of their Destination
 * and source addresses.
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_promisc_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val |= EMAC_V2_PROMISCUOUS_MODE_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable promiscous mode.
 * When called the EMAC_V2 falls back to normal operation from promiscous mode.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_promisc_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val &= ~(EMAC_V2_PROMISCUOUS_MODE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable unicast hash filtering.
 * When disabled EMAC_V2 performs perfect destination address filtering for unicast frames, it compares 
 * DA field with the value programmed in DA register.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_unicast_hash_filter_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val |= EMAC_V2_UCAST_HASH_FILTER_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable unicast hash filtering.
 * When disabled EMAC_V2 performs perfect destination address filtering for multicast frames, it compares 
 * DA field with the value programmed in DA register.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_unicast_hash_filter_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FRAME_FILTER_OFFSET);
    reg_val &= ~(EMAC_V2_UCAST_HASH_FILTER_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FRAME_FILTER_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable detection of pause frames with stations unicast address.
 * When enabled EMAC_V2 detects the pause frames with stations unicast address in addition to the
 * detection of pause frames with unique multicast address.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_unicast_pause_frame_detect_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);
    reg_val |= EMAC_V2_UCAST_PAUSE_FRAME_ON_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable detection of pause frames with stations unicast address.
 * When disabled EMAC_V2 only detects with the unique multicast address (802.3x).
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_unicast_pause_frame_detect_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);
    reg_val &= ~(EMAC_V2_UCAST_PAUSE_FRAME_ON_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable Rx flow control enable.
 * When Enabled EMAC_V2 will decode the rx pause frame and disable the tx for a specified time.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_rx_flow_control_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);
    reg_val |= EMAC_V2_RX_FLOW_CONTROL_ENABLE_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable Rx flow control disable.
 * When disabled EMAC_V2 will not decode pause frame.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_rx_flow_control_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);
    reg_val &= ~(EMAC_V2_RX_FLOW_CONTROL_ENABLE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 enable Tx flow control enable.
 * When Enabled 
 *  - In full duplex EMAC_V2 enables flow control operation to transmit pause frames.
 *  - In Half duplex EMAC_V2 enables the back pressure operation
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_tx_flow_control_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);
    reg_val |= EMAC_V2_TX_FLOW_CONTROL_ENABLE_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 disable Tx flow control disable.
 * When Disabled 
 *  - In full duplex EMAC_V2 will not transmit any pause frames.
 *  - In Half duplex EMAC_V2 disables the back pressure feature.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_tx_flow_control_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);
    reg_val &= ~(EMAC_V2_TX_FLOW_CONTROL_ENABLE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 activate Flowcontrol operation.
 * When Set
 *  - In full duplex EMAC_V2 initiates pause control frame.
 *  - In Half duplex EMAC_V2 initiates back pressure function.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_tx_flow_control_activate(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);
    reg_val |= EMAC_V2_APPLY_BACK_PRESSURE_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 stops Flowcontrol operation.called only in HALFDUPLEX
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_tx_flow_control_deactivate(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);
    reg_val &= ~(EMAC_V2_APPLY_BACK_PRESSURE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_FLOW_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 set rx dma description
 *
 * @param dev
 * @param last_ring_desc
 * 
 * @return 0 for success
 *
 */
int bflb_emac_v2_rx_dma_desc_init(struct bflb_emac_v2_dma_desc_s *desc, uint8_t last_ring_desc)
{
    desc->status = EMAC_V2_DESC_RX_EXT_STS;
    desc->length = last_ring_desc ? EMAC_V2_DESC_RX_END_OF_RING : 0;
    //desc->buffer1 = 0;
    //desc->buffer2 = 0;
    //desc->data1 = 0;
    //desc->data2 = 0;
    desc->extstatus = 0;
    desc->timestamplow = 0;
    desc->timestamphigh = 0;

    return 0;
}

/**
 * @brief bflb emac_v2 set tx dma description
 *
 * @param dev
 * @param last_ring_desc
 * 
 * @return 0 for success
 *
 */
int bflb_emac_v2_tx_dma_desc_init(struct bflb_emac_v2_dma_desc_s *desc, uint8_t last_ring_desc)
{
    desc->status = last_ring_desc ? EMAC_V2_DESC_TX_END_OF_RING : 0;
    desc->length = 0;
    //desc->buffer1 = 0;
    //desc->buffer2 = 0;
    //desc->data1 = 0;
    //desc->data2 = 0;

    return 0;
}

/**
 * @brief bflb emac_v2 dma description list init
 *
 * @param dev
 * @param handle
 * @param tx_dma_desc
 * @param tx_buff
 * @param tx_dma_desc_cnt
 * @param rx_dma_desc
 * @param rx_buff
 * @param rx_dma_desc_cnt
 * 
 * @return 0 for success
 * 
 */
static int bflb_emac_v2_dma_desc_list_init(struct bflb_device_s *dev, struct bflb_emac_v2_handle_s *handle,
                                           struct bflb_emac_v2_dma_desc_s *tx_dma_desc,
                                           uint8_t *tx_buff,
                                           uint32_t tx_dma_desc_cnt,
                                           struct bflb_emac_v2_dma_desc_s *rx_dma_desc,
                                           uint8_t *rx_buff,
                                           uint32_t rx_dma_desc_cnt)
{
    uint32_t i = 0;
    uint32_t reg_base;
    struct bflb_emac_v2_dma_desc_s *dma_desc;

    reg_base = dev->reg_base;

    /* Set the Ethernet handler env */
    handle->tx_index_emac_v2 = 0;
    handle->tx_index_cpu = 0;
    handle->tx_index_cnt = tx_dma_desc_cnt - 1;
    handle->tx_dma_desc = tx_dma_desc;
    handle->rx_index_emac_v2 = 0;
    handle->rx_index_cpu = 0;
    handle->rx_index_cnt = rx_dma_desc_cnt - 1;
    handle->rx_dma_desc = rx_dma_desc;

    /* Fill each DMARxDesc descriptor with the right values */
    for (i = 0; i < tx_dma_desc_cnt; i++) {
        dma_desc = handle->tx_dma_desc + i;
        bflb_emac_v2_tx_dma_desc_init(dma_desc, i == tx_dma_desc_cnt - 1);
        dma_desc->buffer1 = (NULL == tx_buff) ? 0 : (uint32_t)(uintptr_t)(tx_buff + (ETH_TX_BUFFER_SIZE * i));
        dma_desc->buffer2 = 0;
        dma_desc->data1 = 0;
        dma_desc->data2 = 0;
    }

    for (i = 0; i < rx_dma_desc_cnt; i++) {
        dma_desc = handle->rx_dma_desc + i;
        bflb_emac_v2_rx_dma_desc_init(dma_desc, i == rx_dma_desc_cnt - 1);
        if (NULL != rx_buff) {
            dma_desc->length |= ((ETH_RX_BUFFER_SIZE << EMAC_V2_DESC_SIZE1_SHIFT) & EMAC_V2_DESC_SIZE1_MASK);
        }
        dma_desc->buffer1 = (NULL == rx_buff) ? 0 : (uint32_t)(uintptr_t)(rx_buff + (ETH_RX_BUFFER_SIZE * i));
        dma_desc->buffer2 = 0;
        dma_desc->data1 = 0;
        dma_desc->data2 = 0;
        if (NULL != rx_buff) {
            dma_desc->status |= EMAC_V2_DESC_OWN_BY_DMA;
        }
    }

    putreg32((uint32_t)handle->tx_dma_desc, reg_base + EMAC_V2_DMA_TX_BASE_ADDR_OFFSET);
    putreg32((uint32_t)handle->rx_dma_desc, reg_base + EMAC_V2_DMA_RX_BASE_ADDR_OFFSET);

    return 0;
}

/**
 * @brief emac_v2 buffer description init
 *
 * @param dev
 * @param tx_dma_desc
 * @param tx_buff
 * @param tx_dma_desc_cnt
 * @param rx_dma_desc
 * @param rx_buff
 * @param rx_dma_desc_cnt
 * 
 * @return 0 for success
 * 
 */
int bflb_emac_v2_dma_desc_init(struct bflb_device_s *dev,
                               struct bflb_emac_v2_dma_desc_s *tx_dma_desc, uint8_t *tx_buff, uint32_t tx_dma_desc_cnt,
                               struct bflb_emac_v2_dma_desc_s *rx_dma_desc, uint8_t *rx_buff, uint32_t rx_dma_desc_cnt)
{
    thiz = &eth_handle;

    return bflb_emac_v2_dma_desc_list_init(dev, thiz, tx_dma_desc, tx_buff, tx_dma_desc_cnt,
                                           rx_dma_desc, rx_buff, rx_dma_desc_cnt);
}

/**
 * @brief This function tracks the tx descriptor the DMA just closed after the transmission of data from this descriptor is 
 * over. This returns the descriptor fields to the caller.
 * 
 * @param[in] dev pointer to emac_v2.
 * @param[out] status status field of the descriptor.
 * @param[out] buffer1 Dma-able buffer1 pointer.
 * @param[out] length1 length of buffer1 (Max is 2048).
 * @param[out] data1 virtual pointer for buffer1.
 * @param[out] buffer2 Dma-able buffer2 pointer.
 * @param[out] length2 length of buffer2 (Max is 2048).
 * @param[out] data2 virtual pointer for buffer2.
 * @param[out] ext_status extend status field of the descriptor.
 * @param[out] time_stamp_high time stamp high.
 * @param[out] time_stamp_low time stamp low.
 * 
 * @return returns present tx descriptor index on success. Negative value if error.
 */

int bflb_emac_v2_get_tx_qptr(struct bflb_device_s *dev, uint32_t *status, uint32_t *buffer1, uint32_t *length1,
                             uint32_t *data1, uint32_t *buffer2, uint32_t *length2, uint32_t *data2,
                             uint32_t *ext_status, uint32_t *time_stamp_high, uint32_t *time_stamp_low)
{
    int txover = thiz->tx_index_emac_v2;
    struct bflb_emac_v2_dma_desc_s *txdesc = thiz->tx_dma_desc + thiz->tx_index_emac_v2;

    EMAC_V2_DRV_DBG("Check txdesc sts=%08x,len=%08x\r\n", txdesc->status, txdesc->length);

    if (bflb_emac_v2_is_desc_owned_by_dma(txdesc)) {
        return -1;
    }

    if (bflb_emac_v2_is_desc_empty(txdesc)) {
        return -1;
    }

    /* busy tx descriptor is reduced by one as it will be handed over to Processor now */
    (thiz->tx_busy_cnt)--;

    if (status != 0) {
        *status = txdesc->status;
    }
    if (ext_status != 0) {
        *ext_status = txdesc->extstatus;
    }

    if (time_stamp_high != 0) {
        *time_stamp_high = txdesc->timestamphigh;
    }
    if (time_stamp_low != 0) {
        *time_stamp_low = txdesc->timestamplow;
    }

    if (buffer1 != 0) {
        *buffer1 = txdesc->buffer1;
    }
    if (length1 != 0) {
        *length1 = (txdesc->length & EMAC_V2_DESC_SIZE1_MASK) >> EMAC_V2_DESC_SIZE1_SHIFT;
    }
    if (data1 != 0) {
        *data1 = txdesc->data1;
    }

    if (buffer2 != 0) {
        *buffer2 = txdesc->buffer2;
    }
    if (length2 != 0) {
        *length2 = (txdesc->length & EMAC_V2_DESC_SIZE2_MASK) >> EMAC_V2_DESC_SIZE2_SHIFT;
    }
    if (data2 != 0) {
        *data2 = txdesc->data2;
    }

    bflb_emac_v2_tx_dma_desc_init(txdesc, bflb_emac_v2_is_last_tx_desc(txdesc));
    thiz->tx_index_emac_v2 = bflb_emac_v2_is_last_tx_desc(txdesc) ? 0 : txover + 1;
    //txdesc = thiz->tx_dma_desc + thiz->tx_index_emac_v2;

    return txover;
}

/**
 * @brief Once the driver has a packet ready to be transmitted, this function is called with the 
 * valid dma-able buffer addresses and their lengths. This function populates the descriptor
 * and make the DMA the owner for the descriptor. This function also controls whetther Checksum
 * offloading to be done in hardware or not.
 * 
 * @param[in] dev pointer to emac_v2.
 * @param[in] buffer1 Dma-able buffer1 pointer.
 * @param[in] length1 length of buffer1 (Max is 2048).
 * @param[in] data1 virtual pointer for buffer1.
 * @param[in] buffer2 Dma-able buffer2 pointer.
 * @param[in] length2 length of buffer2 (Max is 2048).
 * @param[in] data2 virtual pointer for buffer2.
 * @param[in] offload_needed indicating whether the checksum offloading in HW/SW.
 * 
 * @return returns present tx descriptor index on success. Negative value if error.
 */
int bflb_emac_v2_set_tx_qptr(struct bflb_device_s *dev, uint32_t buffer1, uint32_t length1, uint32_t data1,
                             uint32_t buffer2, uint32_t length2, uint32_t data2, uint32_t offload_needed)
{
    int txnext = thiz->tx_index_cpu;
    struct bflb_emac_v2_dma_desc_s *txdesc = thiz->tx_dma_desc + thiz->tx_index_cpu;

    if (!bflb_emac_v2_is_desc_empty(txdesc)) {
        return -1;
    }

    /* busy tx descriptor is incremented by one as it will be handed over to DMA */
    (thiz->tx_busy_cnt)++;

    txdesc->length |= (((length1 << EMAC_V2_DESC_SIZE1_SHIFT) & EMAC_V2_DESC_SIZE1_MASK) | ((length2 << EMAC_V2_DESC_SIZE2_SHIFT) & EMAC_V2_DESC_SIZE2_MASK));
    txdesc->status |= (EMAC_V2_DESC_TX_FIRST | EMAC_V2_DESC_TX_LAST | EMAC_V2_DESC_TX_INT_ENABLE);

    txdesc->buffer1 = buffer1;
    txdesc->data1 = data1;

    txdesc->buffer2 = buffer2;
    txdesc->data2 = data2;

#ifdef EMAC_V2_IPC_OFFLOAD
    if (offload_needed) {
        /*
        Make sure that the OS you are running supports the IP and TCP checkusm offloaidng,
        before calling any of the functions given below.
        */
        bflb_emac_v2_tx_checksum_offload_ipv4hdr(txdesc);
        bflb_emac_v2_tx_checksum_offload_tcponly(txdesc);
    }
#endif

    txdesc->status |= EMAC_V2_DESC_OWN_BY_DMA;

    thiz->tx_index_cpu = bflb_emac_v2_is_last_tx_desc(txdesc) ? 0 : txnext + 1;

    return txnext;
}

/**
 * @brief Prepares the descriptor to receive packets.After successful return from this function the
 * descriptor is added to the receive descriptor pool/queue.
 * 
 * @param[in] dev pointer to emac_v2.
 * @param[in] buffer1 Dma-able buffer1 pointer.
 * @param[in] length1 length of buffer1 (Max is 2048).
 * @param[in] data1 virtual pointer for buffer1.
 * @param[in] buffer2 Dma-able buffer2 pointer.
 * @param[in] length2 length of buffer2 (Max is 2048).
 * @param[in] data2 virtual pointer for buffer2.
 * 
 * @return returns returns present rx descriptor index on success. Negative value if error.
 */
int bflb_emac_v2_set_rx_qptr(struct bflb_device_s *dev, uint32_t buffer1, uint32_t length1, uint32_t data1,
                             uint32_t buffer2, uint32_t length2, uint32_t data2)
{
    int rxnext = thiz->rx_index_cpu;
    struct bflb_emac_v2_dma_desc_s *rxdesc = thiz->rx_dma_desc + thiz->rx_index_cpu;

#if 0
    if (!bflb_emac_v2_is_desc_empty(rxdesc)) {
        EMAC_V2_DRV_DBG("rxdesc is not empty, rxnext=%d\r\n", rxnext);
        return -1;
    }
#endif

    bflb_emac_v2_rx_dma_desc_init(rxdesc, bflb_emac_v2_is_last_rx_desc(rxdesc));

    rxdesc->length |= (((length1 << EMAC_V2_DESC_SIZE1_SHIFT) & EMAC_V2_DESC_SIZE1_MASK) | ((length2 << EMAC_V2_DESC_SIZE2_SHIFT) & EMAC_V2_DESC_SIZE2_MASK));
    rxdesc->buffer1 = buffer1;
    rxdesc->data1 = data1;

    rxdesc->buffer2 = buffer2;
    rxdesc->data2 = data2;
    rxdesc->status |= EMAC_V2_DESC_OWN_BY_DMA;

    thiz->rx_index_cpu = bflb_emac_v2_is_last_rx_desc(rxdesc) ? 0 : rxnext + 1;

    /* idle rx descriptor is incremented by one as it will be handed over to DMA */
    (thiz->rx_idle_cnt)++;

    return rxnext;
}

/**
 * @brief When the DMA indicates that the data is received (interrupt is generated), this function should be
 * called to get the descriptor and hence the data buffers received. With successful return from this
 * function caller gets the descriptor fields for processing.
 * 
 * @param[in] dev pointer to emac_v2.
 * @param[out] buffer1 Dma-able buffer1 pointer.
 * @param[out] length1 length of buffer1 (Max is 2048).
 * @param[out] data1 virtual pointer for buffer1.
 * @param[out] buffer2 Dma-able buffer2 pointer.
 * @param[out] length2 length of buffer2 (Max is 2048).
 * @param[out] data2 virtual pointer for buffer2.
 * @param[out] ext_status extend status field of the descriptor.
 * @param[out] time_stamp_high time stamp high.
 * @param[out] time_stamp_low time stamp low.
 * 
 * @return returns returns present rx descriptor index on success. Negative value if error.
 */
int bflb_emac_v2_get_rx_qptr(struct bflb_device_s *dev, uint32_t *status, uint32_t *buffer1, uint32_t *length1,
                             uint32_t *data1, uint32_t *buffer2, uint32_t *length2, uint32_t *data2,
                             uint32_t *ext_status, uint32_t *time_stamp_high, uint32_t *time_stamp_low)
{
    int rxnext = thiz->rx_index_emac_v2;
    struct bflb_emac_v2_dma_desc_s *rxdesc = thiz->rx_dma_desc + thiz->rx_index_emac_v2;

    if (bflb_emac_v2_is_desc_owned_by_dma(rxdesc)) {
        return -1;
    }

    if (bflb_emac_v2_is_desc_empty(rxdesc)) {
        return -1;
    }

    if (status != 0) {
        *status = rxdesc->status;
    }
    if (ext_status != 0) {
        *ext_status = rxdesc->extstatus;
    }

    if (time_stamp_high != 0) {
        *time_stamp_high = rxdesc->timestamphigh;
    }
    if (time_stamp_low != 0) {
        *time_stamp_low = rxdesc->timestamplow;
    }

    EMAC_V2_DRV_DBG("RX Timestamp:high=%08x,low=%08x\r\n", rxdesc->timestamphigh, rxdesc->timestamplow);

    if (buffer1 != 0) {
        *buffer1 = rxdesc->buffer1;
    }
    if (length1 != 0) {
        *length1 = (rxdesc->length & EMAC_V2_DESC_SIZE1_MASK) >> EMAC_V2_DESC_SIZE1_SHIFT;
    }
    if (data1 != 0) {
        *data1 = rxdesc->data1;
    }

    if (buffer2 != 0) {
        *buffer2 = rxdesc->buffer2;
    }
    if (length2 != 0) {
        *length2 = (rxdesc->length & EMAC_V2_DESC_SIZE2_MASK) >> EMAC_V2_DESC_SIZE2_SHIFT;
    }
    if (data2 != 0) {
        *data2 = rxdesc->data2;
    }

    thiz->rx_index_emac_v2 = bflb_emac_v2_is_last_rx_desc(rxdesc) ? 0 : rxnext + 1;
#if 1
    /* rxdesc should be recoveried by CPU RX thread when data is processed */
    bflb_emac_v2_rx_dma_desc_init(rxdesc, bflb_emac_v2_is_last_rx_desc(rxdesc));
    //rxdesc->length |= ((ETH_RX_BUFFER_SIZE << EMAC_V2_DESC_SIZE1_SHIFT) & EMAC_V2_DESC_SIZE1_MASK);
    //rxdesc->status |= EMAC_V2_DESC_OWN_BY_DMA;
#endif
    /* idle rx descriptor is reduced by one as it will be handed over to CPU */
    (thiz->rx_idle_cnt)--;

    return rxnext;
}

/**
 * @brief Get all the interrupts.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return interrupt status
 */
uint32_t bflb_emac_v2_get_interrupt_status(struct bflb_device_s *dev)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;

    return getreg32(reg_base + EMAC_V2_DMA_STATUS_OFFSET);
}

/**
 * @brief Clears all the pending interrupts.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] interrupts interrupts to clear.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_clear_interrupt(struct bflb_device_s *dev, uint32_t interrupts)
{
    uint32_t reg_val = 0;

    reg_val = getreg32(dev->reg_base + EMAC_V2_DMA_STATUS_OFFSET);
    reg_val |= interrupts;
    putreg32(reg_val, dev->reg_base + EMAC_V2_DMA_STATUS_OFFSET);

    return 0;
}

/**
 * @brief Returns the all unmasked interrupt status after reading the DmaStatus register.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return the all unmasked interrupt status
 */
uint32_t bflb_emac_v2_get_interrupt_type(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;
    uint32_t interrupts = 0;

    reg_base = dev->reg_base;
    reg_val = getreg32(dev->reg_base + EMAC_V2_DMA_STATUS_OFFSET);

    putreg32(reg_val, reg_base + EMAC_V2_DMA_STATUS_OFFSET);

    if (reg_val & EMAC_V2_DMA_INT_BUS_ERROR)
        interrupts |= EMAC_V2_DMA_INT_API_ERROR;
    if (reg_val & EMAC_V2_DMA_INT_RX_COMPLETED)
        interrupts |= EMAC_V2_DMA_INT_API_RX_NORMAL;
    if (reg_val & EMAC_V2_DMA_INT_RX_NOBUFFER)
        interrupts |= EMAC_V2_DMA_INT_API_RX_ABNORMAL;
    if (reg_val & EMAC_V2_DMA_INT_RX_STOPPED)
        interrupts |= EMAC_V2_DMA_INT_API_RX_STOPPED;
    if (reg_val & EMAC_V2_DMA_INT_TX_COMPLETED)
        interrupts |= EMAC_V2_DMA_INT_API_TX_NORMAL;
    if (reg_val & EMAC_V2_DMA_INT_TX_UNDERFLOW)
        interrupts |= EMAC_V2_DMA_INT_API_TX_ABNORMAL;
    if (reg_val & EMAC_V2_DMA_INT_TX_STOPPED)
        interrupts |= EMAC_V2_DMA_INT_API_TX_STOPPED;
#ifdef EMAC_V2_AVB_SUPPORT
    if (reg_val & EMAC_V2_DMA_SLOT_COUNTER_INTR)
        interrupts |= EMAC_V2_DMA_INT_API_SLOT_COUNTER;
#endif

    return interrupts;
}

/**
 * @brief Get the interrupt mask.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return the interrupt mask.
 */

uint32_t bflb_emac_v2_get_interrupt_mask(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;
    reg_val = getreg32(reg_base + EMAC_V2_DMA_INTERRUPT_ENABLE_OFFSET);

    return reg_val;
}

/**
 * @brief Enables the DMA interrupt as specified by the bit mask.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] interrupts bit mask for interrupts to be enabled.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_enable_interrupt(struct bflb_device_s *dev, uint32_t interrupts)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(interrupts, reg_base + EMAC_V2_DMA_INTERRUPT_ENABLE_OFFSET);

    return 0;
}

/**
 * @brief Disable all the interrupts.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_disable_interrupt_all(struct bflb_device_s *dev)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(0, reg_base + EMAC_V2_DMA_INTERRUPT_ENABLE_OFFSET);

    return 0;
}

/**
 * @brief Disable interrupt according to the bitfield supplied.
 * Disables only those interrupts specified in the bit mask in second argument.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] interrupts bit mask for interrupts to be disabled.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_disable_interrupt(struct bflb_device_s *dev, uint32_t interrupts)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_DMA_INTERRUPT_ENABLE_OFFSET);
    reg_val &= (~interrupts);
    putreg32(reg_val, reg_base + EMAC_V2_DMA_INTERRUPT_ENABLE_OFFSET);

    return 0;
}

/**
 * @brief Enable the DMA Reception.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_enable_dma_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_DMA_CONTROL_OFFSET);
    reg_val |= EMAC_V2_DMA_RX_START;
    putreg32(reg_val, reg_base + EMAC_V2_DMA_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief Enable the DMA Transmission.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_enable_dma_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_DMA_CONTROL_OFFSET);
    reg_val |= EMAC_V2_DMA_TX_START;
    putreg32(reg_val, reg_base + EMAC_V2_DMA_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief Resumes the DMA Transmission.
 * the DmaTxPollDemand is written. (the data writeen could be anything).
 * This forces the DMA to resume transmission.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_resume_dma_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(0, reg_base + EMAC_V2_DMA_TX_POLL_DEMAND_OFFSET);

    return 0;
}

/**
 * @brief Resumes the DMA Reception.
 * the DmaRxPollDemand is written. (the data writeen could be anything).
 * This forces the DMA to resume reception.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_resume_dma_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(0, reg_base + EMAC_V2_DMA_RX_POLL_DEMAND_OFFSET);

    return 0;
}

/**
 * @brief Take ownership of all the rx Descriptors.
 * This function is called when there is fatal error in DMA transmission.
 * When called it takes the ownership of all the tx descriptor in tx descriptor pool/queue from DMA.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_take_desc_ownership_rx(struct bflb_device_s *dev)
{
    int i;

    for (i = 0; i < thiz->rx_index_cnt; i++) {
        bflb_emac_v2_take_desc_ownership((thiz->rx_dma_desc + i));
    }

    return 0;
}

/**
 * @brief Take ownership of all the tx Descriptors.
 * This function is called when there is fatal error in DMA transmission.
 * When called it takes the ownership of all the tx descriptor in tx descriptor pool/queue from DMA.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_take_desc_ownership_tx(struct bflb_device_s *dev)
{
    int i;

    for (i = 0; i < thiz->tx_index_cnt; i++) {
        bflb_emac_v2_take_desc_ownership((thiz->tx_dma_desc + i));
    }

    return 0;
}

/**
 * @brief Disable the DMA for Transmission.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_disable_dma_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_DMA_CONTROL_OFFSET);
    reg_val &= (~EMAC_V2_DMA_TX_START);
    putreg32(reg_val, reg_base + EMAC_V2_DMA_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief Disable the DMA for Reception.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_disable_dma_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_DMA_CONTROL_OFFSET);
    reg_val &= (~EMAC_V2_DMA_RX_START);
    putreg32(reg_val, reg_base + EMAC_V2_DMA_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief Decodes the Rx Descriptor status to various checksum error conditions.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] u32 status field of the corresponding descriptor.
 * 
 * @return returns decoded enum (u32) indicating the status.
 */
uint32_t bflb_emac_v2_is_rx_checksum_error(struct bflb_device_s *dev, uint32_t status)
{
    if (((status & EMAC_V2_DESC_RX_CHK_Bit5) == 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit7) == 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit0) == 0))
        return EMAC_V2_RX_LEN_LT600;
    else if (((status & EMAC_V2_DESC_RX_CHK_Bit5) == 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit7) == 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit0) != 0))
        return EMAC_V2_RX_IPHDR_PAYLOAD_CHK_BYPASS;
    else if (((status & EMAC_V2_DESC_RX_CHK_Bit5) == 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit7) != 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit0) != 0))
        return EMAC_V2_RX_CHK_BYPASS;
    else if (((status & EMAC_V2_DESC_RX_CHK_Bit5) != 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit7) == 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit0) == 0))
        return EMAC_V2_RX_NO_CHK_ERROR;
    else if (((status & EMAC_V2_DESC_RX_CHK_Bit5) != 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit7) == 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit0) != 0))
        return EMAC_V2_RX_PAYLOAD_CHK_ERROR;
    else if (((status & EMAC_V2_DESC_RX_CHK_Bit5) != 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit7) != 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit0) == 0))
        return EMAC_V2_RX_IPHDR_CHK_ERROR;
    else if (((status & EMAC_V2_DESC_RX_CHK_Bit5) != 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit7) != 0) && ((status & EMAC_V2_DESC_RX_CHK_Bit0) != 0))
        return EMAC_V2_RX_IPHDR_PAYLOAD_CHK_ERROR;
    else
        return EMAC_V2_RX_IPHDR_PAYLOAD_RES;
}

/**
 * @brief Enables the assertion of PMT interrupt.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_pmt_int_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_DMA_CONTROL_OFFSET);
    reg_val &= (~EMAC_V2_DMA_RX_START);
    putreg32(reg_val, reg_base + EMAC_V2_DMA_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief This function enables the timestamping. This enables the timestamping for transmit and receive frames.
 * When disabled timestamp is not added to tx and receive frames and timestamp generator is suspended.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val |= EMAC_V2_TS_ENA_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief This function disables the timestamping. 
 * When disabled timestamp is not added to tx and receive frames and timestamp generator is suspended.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_ENA_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Enable the interrupt to get timestamping interrupt. 
  * This enables the host to get the interrupt when (1) system time is greater or equal to the 
  * target time high and low register or (2) there is a overflow in th esecond register.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_int_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_INTERRUPT_MASK_OFFSET);
    reg_val &= (~EMAC_V2_TS_INT_MASK_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_INTERRUPT_MASK_OFFSET);

    return 0;
}

/**
 * @brief Disable the interrupt to get timestamping interrupt. 
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_int_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_INTERRUPT_MASK_OFFSET);
    reg_val |= EMAC_V2_TS_INT_MASK_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_INTERRUPT_MASK_OFFSET);

    return 0;
}

/**
 * @brief Enable MAC address for PTP frame filtering. 
 * When enabled, uses MAC address (apart from MAC address 0) to filter the PTP frames when
 * PTP is sent directly over Ethernet.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_mac_addr_filt_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val |= EMAC_V2_TS_EN_MAC_ADDR_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Disables MAC address for PTP frame filtering. 
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_mac_addr_filt_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_EN_MAC_ADDR_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Selet the type of clock mode for PTP. 
 * Please note to use one of the follwoing as the clk_type argument.
 *     00=> Ordinary clock
 *     01=> Boundary clock
 *     10=> End-to-End transparent clock
 *     11=> P-to-P transparent clock
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] clk_type value representing one of the above clk value
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_set_clk_type(struct bflb_device_s *dev, uint32_t clk_type)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_CLK_TYPE_MASK);
    reg_val |= ((clk_type << EMAC_V2_TS_CLK_TYPE_SHIFT) & EMAC_V2_TS_CLK_TYPE_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Enable Snapshot for messages relevant to Master. 
 * When enabled, snapshot is taken for messages relevant to master mode only, else snapshot is taken for messages relevant 
 * to slave node. 
 * Valid only for Ordinary clock and Boundary clock
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_master_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val |= EMAC_V2_TS_MSTR_ENA_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Disable Snapshot for messages relevant to Master. 
 * When disabled, snapshot is taken for messages relevant 
 * to slave node. 
 * Valid only for Ordinary clock and Boundary clock
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_master_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_MSTR_ENA_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Enable Snapshot for Event messages. 
 * When enabled, snapshot is taken for event messages only (SYNC, Delay_Req, Pdelay_Req or Pdelay_Resp)
 * When disabled, snapshot is taken for all messages except Announce, Management and Signaling.
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_event_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val |= EMAC_V2_TS_EVNT_ENA_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Disable Snapshot for Event messages. 
 * When disabled, snapshot is taken for all messages except Announce, Management and Signaling.
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_event_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_EVNT_ENA_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Enable time stamp snapshot for IPV4 frames. 
 * When enabled, time stamp snapshot is taken for IPV4 frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_ipv4_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val |= EMAC_V2_TS_IPV4_ENA_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Disable time stamp snapshot for IPV4 frames. 
 * When disabled, time stamp snapshot is not taken for IPV4 frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_ipv4_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_IPV4_ENA_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Enable time stamp snapshot for IPV6 frames. 
 * When enabled, time stamp snapshot is taken for IPV6 frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_ipv6_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val |= EMAC_V2_TS_IPV6_ENA_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Disable time stamp snapshot for IPV6 frames. 
 * When disabled, time stamp snapshot is not taken for IPV6 frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_ipv6_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_IPV6_ENA_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Enable time stamp snapshot for PTP over Ethernet frames. 
 * When enabled, time stamp snapshot is taken for PTP over Ethernet frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_ptp_over_ethernet_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val |= EMAC_V2_TS_IP_ENA_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Disable time stamp snapshot for PTP over Ethernet frames. 
 * When disabled, time stamp snapshot is not taken for PTP over Ethernet frames
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_ptp_over_ethernet_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_IP_ENA_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Snoop PTP packet for version 2 format 
 * When set the PTP packets are snooped using the version 2 format.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_pkt_snoop_ver2(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val |= EMAC_V2_TS_V2_ENA_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Snoop PTP packet for version 1 format 
 * When set the PTP packets are snooped using the version 1 format.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_pkt_snoop_ver1(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_V2_ENA_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Timestamp digital rollover 
 * When set the timestamp low register rolls over after 0x3B9A_C9FF value.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_digital_rollover_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val |= EMAC_V2_TS_CTRL_SSR_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Timestamp binary rollover 
 * When set the timestamp low register rolls over after 0x7FFF_FFFF value.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_binary_rollover_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_CTRL_SSR_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Enable Time Stamp for All frames 
 * When set the timestamp snap shot is enabled for all frames received by the core.
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_all_frames_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val |= EMAC_V2_TS_EN_ALL_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Disable Time Stamp for All frames 
 * When reset the timestamp snap shot is not enabled for all frames received by the core.
 * Reserved when "Advanced Time Stamp" is not selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_all_frames_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_EN_ALL_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Addend Register Update 
 * This function loads the contents of Time stamp addend register with the supplied 32 value.
 * This is reserved function when only coarse correction option is selected
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] addend_value 32 bit addend value.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_addend_update(struct bflb_device_s *dev, uint32_t addend_value)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;
    uint32_t loop_variable;

    reg_base = dev->reg_base;

    putreg32(addend_value, reg_base + EMAC_V2_TS_ADDEND_OFFSET);

    for (loop_variable = 0; loop_variable < DEFAULT_LOOP_VARIABLE; loop_variable++) {
        reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
        if (!(reg_val & EMAC_V2_TS_ADD_REG_MASK)) {
            /* if it is cleared ,success, set it then return */
            reg_val |= EMAC_V2_TS_ADD_REG_MASK;
            putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);
            return 0;
        }
        emac_v2_delay_us(DEFAULT_DELAY_VARIABLE);
    }
    return -1;
}

/**
 * @brief time stamp Update 
 * This function updates (adds/subtracts) with the value specified in the Timestamp High Update and
 * Timestamp Low Update register.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] high_value Timestamp High Update value
 * @param[in] low_value Timestamp Low Update value
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_timestamp_update(struct bflb_device_s *dev, uint32_t high_value, uint32_t low_value)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;
    uint32_t loop_variable;

    reg_base = dev->reg_base;

    putreg32(high_value, reg_base + EMAC_V2_TS_HIGH_UPDATE_OFFSET);
    putreg32(low_value, reg_base + EMAC_V2_TS_LOW_UPDATE_OFFSET);

    for (loop_variable = 0; loop_variable < DEFAULT_LOOP_VARIABLE; loop_variable++) {
        reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
        if (!(reg_val & EMAC_V2_TS_UPDT_MASK)) {
            /* if it is cleared ,success, set it then return */
            reg_val |= EMAC_V2_TS_UPDT_MASK;
            putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);
            return 0;
        }
        emac_v2_delay_us(DEFAULT_DELAY_VARIABLE);
    }
    return -1;
}

/**
 * @brief time stamp Initialize 
 * This function Loads/Initializes h the value specified in the Timestamp High Update and
 * Timestamp Low Update register.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] high_value Timestamp High Update value
 * @param[in] low_value Timestamp Low Update value
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_timestamp_init(struct bflb_device_s *dev, uint32_t high_value, uint32_t low_value)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;
    uint32_t loop_variable;

    reg_base = dev->reg_base;

    putreg32(high_value, reg_base + EMAC_V2_TS_HIGH_UPDATE_OFFSET);
    putreg32(low_value, reg_base + EMAC_V2_TS_LOW_UPDATE_OFFSET);

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    /* if it is cleared ,success, set it then return */
    reg_val |= EMAC_V2_TS_INIT_MASK;
    reg_val |= EMAC_V2_TS_UPDT_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);
    return 0;
        
    for (loop_variable = 0; loop_variable < DEFAULT_LOOP_VARIABLE; loop_variable++) {
        reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
        if (!(reg_val & EMAC_V2_TS_INIT_MASK)) {
            /* if it is cleared ,success, set it then return */
            reg_val |= EMAC_V2_TS_INIT_MASK;
            putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);
            return 0;
        }
        emac_v2_delay_us(DEFAULT_DELAY_VARIABLE);
    }
    return -1;
}

/**
 * @brief Time Stamp Update Coarse 
  * When reset the timestamp update is done using coarse method.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_coarse_update(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val &= (~EMAC_V2_TS_CF_UPDT_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Time Stamp Update Fine 
  * When reset the timestamp update is done using Fine method.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_fine_update(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    reg_val = getreg32(dev->reg_base + EMAC_V2_TS_CTRL_OFFSET);
    reg_val |= EMAC_V2_TS_CF_UPDT_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_TS_CTRL_OFFSET);

    return 0;
}

/**
 * @brief Load the Sub Second Increment value in to Sub Second increment register 
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] sub_sec_inc_value 32 bit addend value.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_subsecond_init(struct bflb_device_s *dev, uint32_t sub_sec_inc_value)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(sub_sec_inc_value & EMAC_V2_SS_INC_MASK, reg_base + EMAC_V2_TS_SUB_SEC_INCR_OFFSET);

    return 0;
}

/**
 * @brief Reads the time stamp contents in to the respective pointers 
 * These registers are readonly.
 * This function returns the 48 bit time stamp assuming Version 2 timestamp with higher word is selected.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] higher_sec_val pointer to hold 16 higher bit second register contents
 * 
 * @param[in] sec_val pointer to hold 32 bit second register contents
 * 
 * @param[in] sub_sec_val pointer to hold 32 bit subnanosecond register contents
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_read_timestamp(struct bflb_device_s *dev,
                                   uint16_t *higher_sec_val, uint32_t *sec_val, uint32_t *sub_sec_val)
{
    *higher_sec_val = (uint16_t)(getreg32(dev->reg_base + EMAC_V2_TS_HIGH_WORD_OFFSET) & EMAC_V2_TS_HIGH_WORD_MASK);
    *sec_val = getreg32(dev->reg_base + EMAC_V2_TS_HIGH_OFFSET);
    *sub_sec_val = getreg32(dev->reg_base + EMAC_V2_TS_LOW_OFFSET);

    return 0;
}

/**
 * @brief Loads the time stamp higher sec value from the value supplied 
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] higher_sec_val 16 higher bit second register contents
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_load_timestamp_higher_val(struct bflb_device_s *dev, uint16_t higher_sec_val)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(higher_sec_val & EMAC_V2_TS_HIGH_WORD_MASK, reg_base + EMAC_V2_TS_HIGH_WORD_OFFSET);

    return 0;
}

/**
 * @brief Loads the time stamp higher sec value from the value supplied 
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] higher_sec_val pointer to hold 16 higher bit second register contents
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_read_timestamp_higher_val(struct bflb_device_s *dev, uint16_t *higher_sec_val)
{
    *higher_sec_val = (uint16_t)(getreg32(dev->reg_base + EMAC_V2_TS_HIGH_WORD_OFFSET) & EMAC_V2_TS_HIGH_WORD_MASK);

    return 0;
}

/**
 * @brief Loads the time stamp higher sec value from the value supplied 
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] sec_val target Timestamp High value
 * 
 * @param[in] sub_sec_val target Timestamp Low value
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_load_target_timestamp(struct bflb_device_s *dev, uint32_t sec_val, uint32_t sub_sec_val)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(sec_val, reg_base + EMAC_V2_TS_TARGET_TIME_HIGH_OFFSET);
    putreg32(sub_sec_val, reg_base + EMAC_V2_TS_TARGET_TIME_LOW_OFFSET);

    return 0;
}

/**
 * @brief Reads the Target time stamp registers 
 * This function Loads the target time stamp registers with the values proviced
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @param[in] sec_val pointer of target Timestamp High value
 * 
 * @param[in] sub_sec_val pointer of target Timestamp Low value
 * 
 * @return 0 for success
 */
int bflb_emac_v2_ts_read_target_timestamp(struct bflb_device_s *dev, uint32_t *sec_val, uint32_t *sub_sec_val)
{
    *sec_val = getreg32(dev->reg_base + EMAC_V2_TS_TARGET_TIME_HIGH_OFFSET);
    *sub_sec_val = getreg32(dev->reg_base + EMAC_V2_TS_TARGET_TIME_LOW_OFFSET);

    return 0;
}

/**
 * @brief Enables the ip checksum offloading in receive path.
 * When set EMAC_V2 calculates 16 bit 1's complement of all received ethernet frame payload.
 * It also checks IPv4 Header checksum is correct. EMAC_V2 core appends the 16 bit checksum calculated
 * for payload of IP datagram and appends it to Ethernet frame transferred to the application.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_rx_chksum_offload_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val |= EMAC_V2_RX_IPC_OFFLOAD_MASK;
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief Disable the ip checksum offloading in receive path.
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_rx_chksum_offload_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_CONFIG_OFFSET);
    reg_val &= (~EMAC_V2_RX_IPC_OFFLOAD_MASK);
    putreg32(reg_val, reg_base + EMAC_V2_CONFIG_OFFSET);

    return 0;
}

/**
 * @brief Instruct the DMA to drop the packets fails tcp ip checksum.
 * This is to instruct the receive DMA engine to drop the recevied packet if they 
 * fails the tcp/ip checksum in hardware. Valid only when full checksum offloading is enabled(type-2).
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_rx_tcpip_chksum_drop_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_DMA_CONTROL_OFFSET);
    reg_val &= (~EMAC_V2_DMA_DISABLE_DROP_TCP_CS);
    putreg32(reg_val, reg_base + EMAC_V2_DMA_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief Instruct the DMA not to drop the packets even if it fails tcp ip checksum.
 * This is to instruct the receive DMA engine to allow the packets even if recevied packet
 * fails the tcp/ip checksum in hardware. Valid only when full checksum offloading is enabled(type-2).
 * 
 * @param[in] dev pointer to emac_v2.
 * 
 * @return 0 for success
 */
int bflb_emac_v2_rx_tcpip_chksum_drop_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + EMAC_V2_DMA_CONTROL_OFFSET);
    reg_val |= EMAC_V2_DMA_DISABLE_DROP_TCP_CS;
    putreg32(reg_val, reg_base + EMAC_V2_DMA_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 set mdc clock div
 *
 * @param dev
 * @param phy_reg
 * @param phy_reg_val
 * 
 * @return int
 *
 */
int bflb_emac_v2_mdc_clk_div_set(struct bflb_device_s *dev, uint16_t div)
{
    uint32_t reg_val;

    /* Set Register Address */
    reg_val = getreg32(dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);
    reg_val &= ~(EMAC_V2_GMII_CSR_CLK_MASK);
    reg_val |= ((div << EMAC_V2_GMII_CSR_CLK_SHIFT) & EMAC_V2_GMII_CSR_CLK_MASK);
    putreg32(reg_val, dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 get verison
 *
 * @param dev pointer to emac_v2.
 * 
 * @param version pointer to version varial
 * 
 * @return int
 *
 */
int bflb_emac_v2_version_get(struct bflb_device_s *dev, uint32_t *version)
{
    *version = getreg32(dev->reg_base + EMAC_V2_VERSION_OFFSET);
    return 0;
}

/**
 * @brief bflb emac_v2 set mac address
 *
 * @param dev
 * @param mac_addr
 * 
 * @return 0 for success
 *
 */
int bflb_emac_v2_mac_addr_set(struct bflb_device_s *dev, const uint8_t mac_addr[6])
{
    uint32_t reg_base;
    uint32_t reg_val = 0;

    reg_base = dev->reg_base;

    /* set mac address */
    reg_val |= (mac_addr[5]) << 0;
    reg_val |= (mac_addr[4]) << 8;
    reg_val |= (mac_addr[3]) << 16;
    reg_val |= (mac_addr[2]) << 24;
    putreg32(reg_val, reg_base + EMAC_V2_MAC_ADDR0_LOW_OFFSET);

    reg_val = getreg32(reg_base + EMAC_V2_MAC_ADDR0_HIGH_OFFSET);
    reg_val &= ~(0xffff);
    reg_val |= (mac_addr[1]) << 0;
    reg_val |= (mac_addr[0]) << 8;
    putreg32(reg_val, reg_base + EMAC_V2_MAC_ADDR0_HIGH_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 reset.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_reset(struct bflb_device_s *dev)
{
    putreg32(EMAC_V2_DMA_RESET_ON, dev->reg_base + EMAC_V2_DMA_BUS_MODE_OFFSET);
    arch_delay_us(DEFAULT_DELAY_VARIABLE);
    return 0;
}

/**
 * @brief bflb emac_v2 DMA BUS Init.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_dma_bus_init(struct bflb_device_s *dev)
{
    putreg32(EMAC_V2_DMA_BURST_LEN_32 | EMAC_V2_DMA_DESCRIPTOR_8_WORDS | EMAC_V2_DMA_DESCRIPTOR_SKIP_2,
             dev->reg_base + EMAC_V2_DMA_BUS_MODE_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 DMA control Init.
 * 
 * @param dev
 *
 * @return 0 for success
 *
 */
int bflb_emac_v2_dma_ctrl_init(struct bflb_device_s *dev)
{
    putreg32(EMAC_V2_DMA_STORE_AND_FORWARD | EMAC_V2_DMA_TX_SECOND_FRAME | EMAC_V2_DMA_RX_THRESH_CTRL_128,
             dev->reg_base + EMAC_V2_DMA_CONTROL_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 init
 *
 * @param dev
 * 
 * @param config
 *
 * @return 0 for success
 * 
 */
int bflb_emac_init(struct bflb_device_s *dev, const struct bflb_emac_config_s *config)
{
    bflb_emac_v2_reset(dev);

    /* set mac address */
    bflb_emac_v2_mac_addr_set(dev, config->mac_addr);

    /* set mdio clock */
    bflb_emac_v2_mdc_clk_div_set(dev, config->md_clk_div);

    /* set emac_v2 core */
    bflb_emac_v2_wd_enable(dev);
    bflb_emac_v2_jabber_enable(dev);
    bflb_emac_v2_frame_burst_enable(dev);
    bflb_emac_v2_jumbo_frame_disable(dev);
    bflb_emac_v2_rx_own_enable(dev);
    bflb_emac_v2_loopback_disable(dev);

    bflb_emac_v2_retry_enable(dev);
    bflb_emac_v2_pad_crc_strip_disable(dev);
    bflb_emac_v2_back_off_limit_set(dev, EMAC_V2_PASS_CONTROL0);
    bflb_emac_v2_deferral_check_disable(dev);
    bflb_emac_v2_ts_ptp_over_ethernet_enable(dev);
    bflb_emac_v2_ts_pkt_snoop_ver2(dev);

    bflb_emac_v2_ts_subsecond_init(dev, 0x1);
    bflb_emac_v2_ts_digital_rollover_enable(dev);
    bflb_emac_v2_ts_enable(dev);
    bflb_emac_v2_ts_timestamp_init(dev, 0x00, 0x00);
    //bflb_emac_v2_ts_timestamp_update(dev, 0x00, 0x02);


    if (config->speed == 1000) {
        bflb_emac_v2_gmii_select(dev);
    } else {
        bflb_emac_v2_mii_select(dev);
        if (config->speed == 100) {
            bflb_emac_v2_mii_speed_set(dev, 100);
        } else if (config->speed == 10) {
            bflb_emac_v2_mii_speed_set(dev, 10);
        }
    }

    /* Frame Filter Configuration */
    bflb_emac_v2_frame_filter_enable(dev);
    bflb_emac_v2_pass_control_set(dev, EMAC_V2_PASS_CONTROL0);
    bflb_emac_v2_broadcast_enable(dev);
    bflb_emac_v2_src_addr_filter_disable(dev);
    bflb_emac_v2_multicast_disable(dev);
    bflb_emac_v2_dst_addr_filter_normal(dev);
    bflb_emac_v2_multicast_hash_filter_disable(dev);
    bflb_emac_v2_promisc_disable(dev);
    bflb_emac_v2_unicast_hash_filter_disable(dev);
    /* Flow Control Configuration */
    bflb_emac_v2_unicast_pause_frame_detect_disable(dev);

    if (config->duplex_mode == EAMC_FULLDUPLEX) {
        bflb_emac_v2_full_duplex_enable(dev);
        bflb_emac_v2_rx_flow_control_enable(dev);
        bflb_emac_v2_tx_flow_control_enable(dev);
    } else {
        /* for Half Duplex configuration */
        bflb_emac_v2_half_duplex_enable(dev);
        bflb_emac_v2_rx_flow_control_disable(dev);
        bflb_emac_v2_tx_flow_control_disable(dev);
        /*TODO: set PHY register to enable CRS on Transmit*/
    }

#define GLB_EMAC_CLK_OUT_ADDRESS (0x20000390)
    uint32_t regval = 0;
    /* GLB select inside clock or Not */
    regval = getreg32(GLB_EMAC_CLK_OUT_ADDRESS);
    if (config->clk_internal_mode) {
        if (dev->idx == 0) {
            regval &= ~(1 << 4); /* 0:not invert refclk,1:invert refclk */
            regval |= (1 << 5);  /* 0: ref_clk in mode */
            regval |= (1 << 6);  /* if [5] = 1, set this bit to 1 will invert the clock to RMII PHY1 */
            regval &= ~(1 << 7); /* Invert clock to our RMII MAC1 TX */
            regval |= (1 << 8);  /* Invert clock to our RMII MAC1 RX */
        } else {
            regval &= ~(1 << 16); /* 0:not invert refclk,1:invert refclk */
            regval |= (1 << 17);  /* 0: ref_clk in mode */
            regval &= ~(1 << 18); /* if [5] = 1, set this bit to 1 will invert the clock to RMII PHY1 */
            regval |= (1 << 19);  /* Invert clock to our RMII MAC1 TX */
            regval |= (1 << 20);  /* Invert clock to our RMII MAC1 RX */
        }
    } else {
        if (dev->idx == 0) {
            regval |= (1 << 4);  /* 0:not invert refclk,1:invert refclk */
            regval &= ~(1 << 5); /* 0: ref_clk in mode */
            regval &= ~(1 << 6); /* if [5] = 1, set this bit to 1 will invert the clock to RMII PHY1 */
            regval &= ~(1 << 7); /* Invert clock to our RMII MAC1 TX */
            regval &= ~(1 << 8); /* Invert clock to our RMII MAC1 RX */
        } else {
            regval |= (1 << 16);  /* 0:not invert refclk,1:invert refclk */
            regval &= ~(1 << 17); /* 0: ref_clk in mode */
            regval &= ~(1 << 18); /* if [5] = 1, set this bit to 1 will invert the clock to RMII PHY1 */
            regval &= ~(1 << 19); /* Invert clock to our RMII MAC1 TX */
            regval &= ~(1 << 20); /* Invert clock to our RMII MAC1 RX */
        }
    }
    putreg32(regval, GLB_EMAC_CLK_OUT_ADDRESS);

#if 1
    bflb_emac_v2_dma_desc_init(dev, tx_dma_desc, NULL, EMAC_TX_BD_NUM_MAX,
                               rx_dma_desc, NULL, EMAC_RX_BD_NUM_MAX);
#else
    bflb_emac_v2_dma_desc_init(dev, tx_dma_desc, (uint8_t *)ethTxBuff, EMAC_TX_BD_NUM_MAX,
                               rx_dma_desc, NULL, EMAC_RX_BD_NUM_MAX);
#endif
    bflb_emac_v2_start_pre(dev);
#ifndef NOT_USE_BFLB_LHAL_IRQ_ATTACH
    bflb_irq_attach(dev->irq_num, bflb_emac_v2_isr, dev);
    bflb_irq_enable(dev->irq_num);
#endif
    return 0;
}

/**
 * @brief bflb emac_v2 deinit
 *
 * @param dev
 *
 */
void bflb_emac_v2_deinit(struct bflb_device_s *dev)
{
}

/**
 * @brief bflb emac_v2 start pre
 *
 * @param dev
 *
 */
void bflb_emac_v2_start_pre(struct bflb_device_s *dev)
{
    bflb_emac_v2_dma_bus_init(dev);
    bflb_emac_v2_dma_ctrl_init(dev);

    /* enable rx ipc offload */
    //bflb_emac_v2_rx_chksum_offload_enable(dev);
    //bflb_emac_v2_rx_tcpip_chksum_drop_disable(dev);

    bflb_emac_v2_promisc_enable(dev);
    //bflb_emac_v2_tx_enable(dev);
    //bflb_emac_v2_rx_enable(dev);
    bflb_emac_v2_enable_dma_rx(dev);
    bflb_emac_v2_enable_dma_tx(dev);
    bflb_emac_v2_enable_interrupt(dev, EMAC_V2_INT_ENABLE_CFG);
}

/**
 * @brief bflb emac_v2 handle tx done
 *
 * @param dev
 *
 * @return 0 for success
 * 
 */
int bflb_emac_v2_handle_transmit_over(struct bflb_device_s *dev)
{
    int32_t desc_index;
    uint32_t data1, data2;
    uint32_t status = 0;
    uint32_t length1, length2;
    uint32_t dma_addr1, dma_addr2;
    uint32_t ext_status;
#ifdef EMAC_V2_DBG_ENABLE
    uint16_t time_stamp_higher;
#endif
    uint32_t time_stamp_high;
    uint32_t time_stamp_low;

    /*Handle the transmit Descriptors*/
    do {
        desc_index = bflb_emac_v2_get_tx_qptr(dev, &status, &dma_addr1, &length1, &data1, &dma_addr2, &length2, &data2, &ext_status, &time_stamp_high, &time_stamp_low);
        if (desc_index < 0) {
            break;
        }
#ifdef EMAC_V2_DBG_ENABLE
        bflb_emac_v2_ts_read_timestamp_higher_val(dev, &time_stamp_higher);
#endif
        struct bflb_emac_trans_desc_s tx_desc = {
            .buff_addr = (uint8_t *)dma_addr1,
            .data_len = length1,
            .err_status = 0,
        };
        if (desc_index >= 0 && dma_addr1 != 0) {
            EMAC_V2_DRV_DBG("Finished Transmit at Tx Descriptor %d with status %08x\r\n", desc_index, status);
            EMAC_V2_DRV_DBG("timestamp_low =%08x,timestamp_high =%08x\r\n", time_stamp_low, time_stamp_high);
            EMAC_V2_DRV_DBG("timestamp_higher =%08x\r\n", time_stamp_higher);

#ifdef EMAC_V2_IPC_OFFLOAD
            if (bflb_emac_v2_is_tx_ipv4header_checksum_error(status)) {
                EMAC_V2_DRV_DBG("Harware Failed to Insert IPV4 Header Checksum\r\n");
            }
            if (bflb_emac_v2_is_tx_payload_checksum_error(status)) {
                EMAC_V2_DRV_DBG("Harware Failed to Insert Payload Checksum\r\n");
            }
#endif
            if (bflb_emac_v2_is_desc_valid(status)) {
                thiz->tx_bytes += length1;
                thiz->tx_packets++;
            } else {
                EMAC_V2_DRV_ERR("Error in Status %08x\r\n", status);
                thiz->tx_errors++;
                thiz->tx_aborted_errors += bflb_emac_v2_is_tx_aborted(status);
                thiz->tx_carrier_errors += bflb_emac_v2_is_tx_carrier_error(status);
                tx_desc.err_status |= EMAC_TX_STA_ERR_UNKNOWN;
            }
            /* callback */
            if (tx_desc.err_status) {
                emac_irq_event_cb(emac_irq_arg, EMAC_IRQ_EVENT_TX_ERR_FRAME, &tx_desc);
            } else {
                emac_irq_event_cb(emac_irq_arg, EMAC_IRQ_EVENT_TX_FRAME, &tx_desc);
            }
        }
        thiz->collisions += bflb_emac_v2_get_tx_collision_count(status);
    } while (1);

    return 0;
}

/**
 * @brief
 *
 * @param flags
 * @param len
 * @param data_in
 * @return int
 */
int bflb_emac_v2_bd_tx_enqueue(struct bflb_device_s *dev, uint32_t flags, uint32_t len, const uint8_t *data_in)
{
    int txnext = thiz->tx_index_cpu;
    struct bflb_emac_v2_dma_desc_s *txdesc = thiz->tx_dma_desc + txnext;
    uint32_t length1 = len;
    uint32_t length2 = 0;
    int offload_needed = 0;

    if (bflb_emac_v2_is_desc_owned_by_dma(txdesc)) {
        EMAC_V2_DRV_ERR("Txdesc busy:%08x\r\n", txdesc->status);
        return -1;
    }

    if (!bflb_emac_v2_is_desc_empty(txdesc)) {
        EMAC_V2_DRV_ERR("Txdesc len not zero:%08x\r\n", txdesc->length);
        return -1;
    }

    /* busy tx descriptor is incremented by one as it will be handed over to DMA */
    (thiz->tx_busy_cnt)++;

    txdesc->length |= (((length1 << EMAC_V2_DESC_SIZE1_SHIFT) & EMAC_V2_DESC_SIZE1_MASK) | ((length2 << EMAC_V2_DESC_SIZE2_SHIFT) & EMAC_V2_DESC_SIZE2_MASK));
    txdesc->status |= (EMAC_V2_DESC_TX_FIRST | EMAC_V2_DESC_TX_LAST | EMAC_V2_DESC_TX_INT_ENABLE | EMAC_V2_DESC_TX_TS_ENABLE);

    if (flags & EMAC_V2_NOCOPY_PACKET) {
        txdesc->buffer1 = (uint32_t)(uintptr_t)data_in;
    } else {
        arch_memcpy_fast((void *)(uintptr_t)(txdesc->buffer1), data_in, length1);
    }
#ifdef EMAC_V2_DO_FLUSH_DATA
#if defined(BL628) || defined(BL616D)
    bflb_l1c_dcache_clean_range((void *)txdesc->buffer1, length1);
#endif
#endif

#ifdef EMAC_V2_IPC_OFFLOAD
    if (offload_needed) {
        /*
        Make sure that the OS you are running supports the IP and TCP checkusm offloaidng,
        before calling any of the functions given below.
        */
        bflb_emac_v2_tx_checksum_offload_ipv4hdr(txdesc);
        bflb_emac_v2_tx_checksum_offload_tcponly(txdesc);
    }
#endif

    txdesc->status |= EMAC_V2_DESC_OWN_BY_DMA;

    if ((bflb_emac_v2_get_interrupt_status(dev) & EMAC_V2_DMA_TX_STATE) == EMAC_V2_DMA_TX_SUSPENDED) {
        bflb_emac_v2_resume_dma_tx(dev);
    }

    thiz->tx_index_cpu = bflb_emac_v2_is_last_tx_desc(txdesc) ? 0 : txnext + 1;

    //return txnext;
    return 0;
}

/**
 * @brief
 * @param dev
 * @param flags
 * @param len
 * @param data_out
 * @return int
 */
#ifndef EMAC_V2_RX_DATA_ZERO_COPY
int bflb_emac_v2_bd_rx_dequeue(struct bflb_device_s *dev, uint32_t flags, uint32_t *len, uint8_t *data_out)
#else
int bflb_emac_v2_bd_rx_dequeue(struct bflb_device_s *dev, uint32_t flags, uint32_t *len, uint8_t **data_out)
#endif
{
    //uint32_t data1;
    //uint32_t data2;
    uint32_t status;
    //uint32_t dma_addr1;
    //uint32_t dma_addr2;
    uint32_t ext_status;
#ifdef EMAC_V2_DBG_ENABLE
    uint16_t time_stamp_higher;
    uint32_t time_stamp_high;
    uint32_t time_stamp_low;
    uint32_t msg_type = 0;
    uint32_t *p_timestamp = NULL;
#endif

    int rxnext = thiz->rx_index_emac_v2;
    struct bflb_emac_v2_dma_desc_s *rxdesc = thiz->rx_dma_desc + thiz->rx_index_emac_v2;

    *len = 0;
    if (bflb_emac_v2_is_desc_owned_by_dma(rxdesc)) {
        return -1;
    }

    if (bflb_emac_v2_is_desc_empty(rxdesc)) {
        return -1;
    }

    /* get status */
    status = rxdesc->status;
    ext_status = rxdesc->extstatus;
#ifdef EMAC_V2_DBG_ENABLE
    time_stamp_high = rxdesc->timestamphigh;
    time_stamp_low = rxdesc->timestamplow;
    //buffer1 = rxdesc->buffer1;
    //length1 = (rxdesc->length & EMAC_V2_DESC_SIZE1_MASK) >> EMAC_V2_DESC_SIZE1_SHIFT;
    //data1 = rxdesc->data1;
    //buffer2 = rxdesc->buffer2;
    //length2 = (rxdesc->length & EMAC_V2_DESC_SIZE2_MASK) >> EMAC_V2_DESC_SIZE2_SHIFT;
    //data2 = rxdesc->data2;
    bflb_emac_v2_ts_read_timestamp_higher_val(dev, &time_stamp_higher);
    EMAC_V2_DRV_DBG("high=%08x,low=%08x\r\n", time_stamp_high, time_stamp_low);
#endif
    // extended status present indicates that the RDES4 need to be probed
    if (bflb_emac_v2_is_ext_status(status)) {
        EMAC_V2_DRV_DBG("Extended Status present:%08x\r\n", ext_status);
        if (ext_status & EMAC_V2_EDESC_RX_PTP_AVAIL) {
            EMAC_V2_DRV_DBG("PTP available\r\n");
        }
        if (ext_status & EMAC_V2_EDESC_RX_PTP_VER) {
            EMAC_V2_DRV_DBG("PTP V2\r\n");
        }
        if (ext_status & EMAC_V2_EDESC_RX_PTP_FRAME_TYPE) {
            EMAC_V2_DRV_DBG("PTP FT\r\n");
        }
#ifdef EMAC_V2_DBG_ENABLE
        msg_type = (ext_status & EMAC_V2_EDESC_RX_PTP_MESSAGE_TYPE) >> 16;
        EMAC_V2_DRV_DBG("Msg type=%d\r\n", msg_type);
        if ((ext_status & EMAC_V2_EDESC_RX_PTP_MESSAGE_TYPE) != 0) {
            p_timestamp = (uint32_t *)(rxdesc->buffer1 + 48);
            EMAC_V2_DRV_DBG("stamp=%08x,%08x\r\n", *p_timestamp, *(p_timestamp + 1));
        }
#endif
        if (ext_status & EMAC_V2_EDESC_RX_PTP_IPV6) {
            EMAC_V2_DRV_DBG("IPv6\r\n");
        }
        if (ext_status & EMAC_V2_EDESC_RX_PTP_IPV4) {
            EMAC_V2_DRV_DBG("IPv4\r\n");
        }
        if (ext_status & EMAC_V2_EDESC_RX_CHKSUM_BYPASS) {
            EMAC_V2_DRV_DBG("CKS bypass\r\n");
        }
        if (ext_status & EMAC_V2_EDESC_RX_IP_PAYLOAD_ERROR) {
            EMAC_V2_DRV_ERR("Payload err\r\n");
        }
        if (ext_status & EMAC_V2_EDESC_RX_IP_HEADER_ERROR) {
            EMAC_V2_DRV_ERR("Header err\r\n");
        }
        EMAC_V2_DRV_DBG("Payload type=%d\r\n", (ext_status & EMAC_V2_EDESC_RX_IP_PAYLOAD_TYPE));
    }

    /* deal data according to status */
    if (bflb_emac_v2_is_rx_desc_valid(status)) {
        *len = bflb_emac_v2_get_rx_desc_frame_length(status) - 4; //Not interested in Ethernet CRC bytes
#ifdef EMAC_V2_IPC_OFFLOAD
        // Now lets check for the IPC offloading
        /*  Since we have enabled the checksum offloading in hardware, lets inform the kernel
            not to perform the checksum computation on the incoming packet. Note that ip header 
            checksum will be computed by the kernel immaterial of what we inform. Similary TCP/UDP/ICMP
            pseudo header checksum will be computed by the stack. What we can inform is not to perform
            payload checksum. 
            When CHECKSUM_UNNECESSARY is set kernel bypasses the checksum computation.
        */
        if (bflb_emac_v2_is_ext_status(status)) { // extended status present indicates that the RDES4 need to be probed
            EMAC_V2_DRV_ERR("Extended Status present\r\n");
            if (bflb_emac_v2_es_is_rx_checksum_bypassed(ext_status)) { // Hardware engine bypassed the checksum computation/checking
                EMAC_V2_DRV_ERR("(EXTSTS)Hardware bypassed checksum computation\r\n");
            }
            if ((status & EMAC_V2_DESC_RX_FRAME_ETHER) && bflb_emac_v2_es_is_ip_header_error(ext_status)) { // IP header (IPV4) checksum error
                EMAC_V2_DRV_ERR("(EXTSTS)Error in IP header error\r\n");
            }
            if ((status & EMAC_V2_DESC_RX_FRAME_ETHER) && bflb_emac_v2_es_is_ip_payload_error(ext_status)) { // IP payload checksum is in error (UDP/TCP/ICMP checksum error)
                EMAC_V2_DRV_ERR("(EXTSTS) Error in EP payload\r\n");
            }
        } else { // No extended status. So relevant information is available in the status itself

            if (status & EMAC_V2_DESC_RX_LEN_ERROR) {
                EMAC_V2_DRV_ERR("Err:EMAC_V2_RX_LEN_ERR\r\n");
            }
            if (status & EMAC_V2_DESC_RX_CRC) {
                EMAC_V2_DRV_ERR("Err:EMAC_V2_RX_CRC_ERR\r\n");
            }
#if 0
            /* bflb_emac_v2_is_rx_checksum_error is not correct */
            if ((bflb_emac_v2_is_rx_checksum_error(dev, status) == EMAC_V2_RX_LEN_LT600)) {
                EMAC_V2_DRV_DBG("Err:EMAC_V2_RX_LEN_LT600\r\n");
            }
            if ((bflb_emac_v2_is_rx_checksum_error(dev, status) == EMAC_V2_RX_PAYLOAD_CHK_ERROR)) {
                EMAC_V2_DRV_DBG("Err:EMAC_V2_RX_PAYLOAD_CHK_ERROR\r\n");
            }
            if ((bflb_emac_v2_is_rx_checksum_error(dev, status) == EMAC_V2_RX_IPHDR_CHK_ERROR)) {
                EMAC_V2_DRV_DBG("Err:EMAC_V2_RX_IPHDR_CHK_ERROR\r\n");
            }
            if ((bflb_emac_v2_is_rx_checksum_error(dev, status) == EMAC_V2_RX_IPHDR_PAYLOAD_CHK_ERROR)) {
                EMAC_V2_DRV_DBG("Err:EMAC_V2_RX_IPHDR_PAYLOAD_CHK_ERROR\r\n");
            }
#endif
        }
#endif
        thiz->rx_packets++;
        thiz->rx_bytes += *len;

        /* copy data or get buffer pointer */
        if (data_out) {
#ifndef EMAC_V2_RX_DATA_ZERO_COPY
            arch_memcpy_fast(data_out, (const void *)(uintptr_t)rxdesc->buffer1, *len);
#else
            *data_out = (uint8_t *)(uintptr_t)rxdesc->buffer1;
#endif
        }
    } else {
        /* descriptor is invalid*/
        EMAC_V2_DRV_ERR("RX descriptor is invalid\r\n");
        thiz->rx_errors++;
        thiz->collisions += bflb_emac_v2_is_rx_frame_collision(status);
        thiz->rx_crc_errors += bflb_emac_v2_is_rx_crc(status);
        thiz->rx_frame_errors += bflb_emac_v2_is_frame_dribbling_errors(status);
        thiz->rx_length_errors += bflb_emac_v2_is_rx_frame_length_errors(status);
    }

    /* move to next one*/
    thiz->rx_index_emac_v2 = bflb_emac_v2_is_last_rx_desc(rxdesc) ? 0 : rxnext + 1;
    bflb_emac_v2_rx_dma_desc_init(rxdesc, bflb_emac_v2_is_last_rx_desc(rxdesc));
    rxdesc->length |= ((ETH_RX_BUFFER_SIZE << EMAC_V2_DESC_SIZE1_SHIFT) & EMAC_V2_DESC_SIZE1_MASK);
    rxdesc->status |= EMAC_V2_DESC_OWN_BY_DMA;

    /* idle rx descriptor is reduced by one as it will be handed over to CPU */
    (thiz->rx_idle_cnt)--;

    //return rxnext;
    return 0;
}

/**
 * @brief bflb emac_v2 phy register read
 *
 * @param dev
 * @param phy_reg
 * @param phy_reg_val
 * @return int
 *
 */
int bflb_emac_md_read(struct bflb_device_s *dev, uint8_t phy_addr, uint16_t phy_reg, uint16_t *data)
{
    uint32_t reg_val;
    uint32_t timeout = DEFAULT_LOOP_VARIABLE;

    /* Set phy address */
    reg_val = getreg32(dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);
    reg_val &= ~(EMAC_V2_GMII_ADDR_MASK);
    reg_val |= (uint32_t)(phy_addr << EMAC_V2_GMII_ADDR_SHIFT);
    putreg32(reg_val, dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);

    /* Set Register Address */
    reg_val = getreg32(dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);
    reg_val &= ~(EMAC_V2_GMII_REG_MASK);
    reg_val |= ((phy_reg << EMAC_V2_GMII_REG_SHIFT) & EMAC_V2_GMII_REG_MASK);
    reg_val &= ~((uint32_t)1 << EMAC_V2_GMII_WRITE_SHIFT);
    reg_val |= EMAC_V2_GMII_BUSY_MASK;
    putreg32(reg_val, dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);

    __ASM volatile("nop");
    __ASM volatile("nop");
    __ASM volatile("nop");
    __ASM volatile("nop");

    do {
        if (timeout == 0) {
            return -1;
        }
        timeout--;
        reg_val = getreg32(dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);
        emac_v2_delay_us(DEFAULT_DELAY_VARIABLE);
    } while ((reg_val & (EMAC_V2_GMII_BUSY_MASK)) != 0);

    *data = getreg32(dev->reg_base + EMAC_V2_GMII_DATA_OFFSET);

    return 0;
}

/**
 * @brief bflb emac_v2 phy register write
 *
 * @param dev
 * @param phy_reg
 * @param phy_reg_val
 * @return int
 *
 */
int bflb_emac_md_write(struct bflb_device_s *dev, uint8_t phy_addr, uint16_t phy_reg, uint16_t phy_reg_val)
{
    uint32_t reg_val;
    uint32_t timeout = DEFAULT_LOOP_VARIABLE;

    /* Set Write data */
    putreg32(phy_reg_val, dev->reg_base + EMAC_V2_GMII_DATA_OFFSET);

    /* Set phy address */
    reg_val = getreg32(dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);
    reg_val &= ~(EMAC_V2_GMII_ADDR_MASK);
    reg_val |= (uint32_t)(phy_addr << EMAC_V2_GMII_ADDR_SHIFT);
    putreg32(reg_val, dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);

    /* Set Register Address */
    reg_val = getreg32(dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);
    reg_val &= ~(EMAC_V2_GMII_REG_MASK);
    reg_val |= ((phy_reg << EMAC_V2_GMII_REG_SHIFT) & EMAC_V2_GMII_REG_MASK);
    reg_val |= ((uint32_t)1 << EMAC_V2_GMII_WRITE_SHIFT);
    reg_val |= EMAC_V2_GMII_BUSY_MASK;
    putreg32(reg_val, dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);

    __ASM volatile("nop");
    __ASM volatile("nop");
    __ASM volatile("nop");
    __ASM volatile("nop");

    do {
        if (timeout == 0) {
            return -1;
        }
        timeout--;
        reg_val = getreg32(dev->reg_base + EMAC_V2_GMII_ADDR_OFFSET);
        emac_v2_delay_us(DEFAULT_DELAY_VARIABLE);
    } while ((reg_val & (EMAC_V2_GMII_BUSY_MASK)) != 0);

    return 0;
}

/**
 * @brief bflb emac_v2 feature control
 *
 * @param dev
 * @param cmd
 * @param arg
 * @return int
 *
 */
int bflb_emac_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
    int ret = 0;
    uint32_t reg_val;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    switch (cmd) {
        case EMAC_CMD_SET_TX_EN:
            bflb_emac_v2_tx_enable(dev);
            break;

        case EMAC_CMD_SET_RX_EN:
            bflb_emac_v2_rx_enable(dev);
            break;
        case EMAC_CMD_SET_FULL_DUPLEX:
            if (arg) {
                bflb_emac_v2_full_duplex_enable(dev);
            } else {
                bflb_emac_v2_half_duplex_enable(dev);
            }
            break;
            
        case EMAC_CMD_SET_PHY_ADDRESS:
            reg_val = getreg32(reg_base + EMAC_V2_GMII_ADDR_OFFSET);
            reg_val &= ~(EMAC_V2_GMII_ADDR_MASK);
            reg_val |= (uint32_t)(arg << EMAC_V2_GMII_ADDR_SHIFT);
            putreg32(reg_val, reg_base + EMAC_V2_GMII_ADDR_OFFSET);
            break;
        case EMAC_CMD_SET_SPEED_1000M:
            bflb_emac_v2_gmii_select(dev);
            break;
        case EMAC_CMD_SET_SPEED_100M:
            bflb_emac_v2_mii_select(dev);
            bflb_emac_v2_mii_speed_set(dev, 100);
            break;
        case EMAC_CMD_SET_SPEED_10M:
            bflb_emac_v2_mii_select(dev);
            bflb_emac_v2_mii_speed_set(dev, 10);
            break;
        default:
            ret = -EPERM;
            break;
    }
    return ret;
}

/* isr callback attach */
int bflb_emac_irq_attach(struct bflb_device_s *dev, bflb_emac_irq_cb_t irq_event_cb, void *arg)
{
    emac_irq_event_cb = irq_event_cb;
    emac_irq_arg = arg;

    return 0;
}

void bflb_emac_v2_isr(int irq, void *arg)
{
    uint32_t dma_status_reg;
    uint32_t interrupt;
    struct bflb_device_s *emac_v2_dev = (struct bflb_device_s *)arg;
    uint32_t status;
    uint32_t ext_status;

    EMAC_V2_DRV_DBG("emac int come\r\n");

    dma_status_reg = bflb_emac_v2_get_interrupt_status(emac_v2_dev);
    if (dma_status_reg == 0) {
        return;
    }

    bflb_emac_v2_disable_interrupt_all(emac_v2_dev);

    /*Now lets handle the DMA interrupts*/
    interrupt = bflb_emac_v2_get_interrupt_type(emac_v2_dev);

    if (interrupt & EMAC_V2_DMA_INT_API_ERROR) {
        bflb_emac_v2_disable_dma_tx(emac_v2_dev);
        bflb_emac_v2_disable_dma_rx(emac_v2_dev);

        bflb_emac_v2_take_desc_ownership_tx(emac_v2_dev);
        bflb_emac_v2_take_desc_ownership_rx(emac_v2_dev);
        //bflb_emac_v2_init_tx_rx_desc_queue(emac_v2x);
        bflb_emac_v2_reset(emac_v2_dev);
        EMAC_V2_DRV_ERR("EMAC_V2 DMA BUS Error\r\n");
    }

    if (interrupt & EMAC_V2_DMA_INT_API_RX_NORMAL) {
        EMAC_V2_DRV_DBG("RX done interrupts\r\n");

        struct bflb_emac_trans_desc_s rx_desc;
        while (bflb_emac_v2_get_rx_qptr(emac_v2_dev, &status, (uint32_t *)&rx_desc.buff_addr, NULL, NULL,
                                        NULL, NULL, NULL, &ext_status, NULL, NULL) >= 0) {
            if (bflb_emac_v2_is_rx_desc_valid(status)) {
                if (bflb_emac_v2_is_ext_status(status)) {
                    EMAC_V2_DRV_DBG("Extended Status present:%08x\r\n", ext_status);
                    if (ext_status & EMAC_V2_EDESC_RX_PTP_AVAIL) {
                        EMAC_V2_DRV_DBG("PTP available\r\n");
                    }
                    if (ext_status & EMAC_V2_EDESC_RX_PTP_VER) {
                        EMAC_V2_DRV_DBG("PTP V2\r\n");
                    }
                    if (ext_status & EMAC_V2_EDESC_RX_PTP_FRAME_TYPE) {
                        EMAC_V2_DRV_DBG("PTP FT\r\n");
                    }
            #ifdef EMAC_V2_DBG_ENABLE
                    int msg_type = (ext_status & EMAC_V2_EDESC_RX_PTP_MESSAGE_TYPE) >> 16;
                    EMAC_V2_DRV_DBG("Msg type=%d\r\n", msg_type);
                    if ((ext_status & EMAC_V2_EDESC_RX_PTP_MESSAGE_TYPE) != 0) {
                        uint32_t * p_timestamp = (uint32_t *)((uint8_t*)rx_desc.buff_addr + 48);
                        EMAC_V2_DRV_DBG("stamp=%08x,%08x\r\n", *p_timestamp, *(p_timestamp + 1));
                    }
            #endif
                    if (ext_status & EMAC_V2_EDESC_RX_PTP_IPV6) {
                        EMAC_V2_DRV_DBG("IPv6\r\n");
                    }
                    if (ext_status & EMAC_V2_EDESC_RX_PTP_IPV4) {
                        EMAC_V2_DRV_DBG("IPv4\r\n");
                    }
                    if (ext_status & EMAC_V2_EDESC_RX_CHKSUM_BYPASS) {
                        EMAC_V2_DRV_DBG("CKS bypass\r\n");
                    }
                    if (ext_status & EMAC_V2_EDESC_RX_IP_PAYLOAD_ERROR) {
                        EMAC_V2_DRV_ERR("Payload err\r\n");
                    }
                    if (ext_status & EMAC_V2_EDESC_RX_IP_HEADER_ERROR) {
                        EMAC_V2_DRV_ERR("Header err\r\n");
                    }
                    EMAC_V2_DRV_DBG("Payload type=%d\r\n", (ext_status & EMAC_V2_EDESC_RX_IP_PAYLOAD_TYPE));
                }
                rx_desc.data_len = bflb_emac_v2_get_rx_desc_frame_length(status) - 4;
                if (emac_irq_event_cb != NULL) {
                    emac_irq_event_cb(emac_irq_arg, EMAC_IRQ_EVENT_RX_FRAME, &rx_desc);
                } else {
                    EMAC_V2_DRV_ERR("IRQ callback not register!!!!!\r\n");
                }
            } else {
                EMAC_V2_DRV_WARN("!!!!!!!!!!!!!!!!RX descriptor is invalid, status=%08x\r\n", status);
            }
            //bflb_emac_v2_set_rx_qptr(emac_v2_dev, (uint32_t)rx_desc.buff_addr, ETH_RX_BUFFER_SIZE, 0,0, 0, 0);
        }
    }

    if (interrupt & EMAC_V2_DMA_INT_API_RX_ABNORMAL) {
        EMAC_V2_DRV_WARN("Abnormal Rx Interrupt Seen\r\n");
        //bflb_emac_v2_resume_dma_rx(emac_v2_dev);
        //rx_err_cnt++;
    }

    if (interrupt & EMAC_V2_DMA_INT_API_RX_STOPPED) {
        EMAC_V2_DRV_WARN("Receiver stopped seeing Rx interrupts\r\n");
        bflb_emac_v2_enable_dma_rx(emac_v2_dev);
    }

    if (interrupt & EMAC_V2_DMA_INT_API_TX_NORMAL) {
        bflb_emac_v2_handle_transmit_over(emac_v2_dev);
        //tx_pkg_cnt++;
    }

    if (interrupt & EMAC_V2_DMA_INT_API_TX_ABNORMAL) {
        bflb_emac_v2_handle_transmit_over(emac_v2_dev);
        EMAC_V2_DRV_WARN("EMAC_V2 DMA TX Abnormal Error\r\n");
        //tx_err_cnt++;
    }

    if (interrupt & EMAC_V2_DMA_INT_API_TX_STOPPED) {
        bflb_emac_v2_disable_dma_tx(emac_v2_dev);
        bflb_emac_v2_take_desc_ownership_tx(emac_v2_dev);
        bflb_emac_v2_enable_dma_tx(emac_v2_dev);
        EMAC_V2_DRV_WARN("EMAC_V2 DMA TX Stopped Error\r\n");
    }
    bflb_emac_v2_enable_interrupt(emac_v2_dev, EMAC_V2_INT_ENABLE_CFG);
}

int bflb_emac_queue_rx_push(struct bflb_device_s *dev, struct bflb_emac_trans_desc_s *rx_desc)
{
    /* lock */
    uintptr_t flag = bflb_irq_save();

    bflb_emac_v2_set_rx_qptr(dev, (uint32_t)rx_desc->buff_addr, ETH_RX_BUFFER_SIZE, 0, 0, 0, 0);
    /* unlock */
    bflb_irq_restore(flag);

    return 0;
}

int bflb_emac_queue_tx_push(struct bflb_device_s *dev, struct bflb_emac_trans_desc_s *tx_desc)
{
    /* lock */
    uintptr_t flag = bflb_irq_save();

    int ret = bflb_emac_v2_bd_tx_enqueue(dev, EMAC_V2_NOCOPY_PACKET, tx_desc->data_len, tx_desc->buff_addr);

    /* unlock */
    bflb_irq_restore(flag);
    return ret;
}

int bflb_emac_bd_ctrl_clean(struct bflb_device_s *dev)
{
    return 0;
}

#if 0
#if defined(BL628) || defined(BL616D)
/**
 * @brief bflb emac phy register read
 *
 * @param dev
 * @param phy_reg
 * @param phy_reg_val
 * @return int
 *
 */
int bflb_emac_phy_reg_read(struct bflb_device_s *dev, uint16_t phy_reg, uint16_t *phy_reg_val)
{
    return bflb_emac_v2_phy_reg_read(dev, phy_reg, phy_reg_val);
}
/**
 * @brief bflb emac phy register write
 *
 * @param dev
 * @param phy_reg
 * @param phy_reg_val
 * @return int
 *
 */
int bflb_emac_phy_reg_write(struct bflb_device_s *dev, uint16_t phy_reg, uint16_t phy_reg_val)
{
    return bflb_emac_v2_phy_reg_write(dev, phy_reg, phy_reg_val);
}

/**
 * @brief bflb emac feature control
 *
 * @param dev
 * @param cmd
 * @param arg
 * @return int
 *
 */
int bflb_emac_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
    return bflb_emac_v2_feature_control(dev, cmd, arg);
}
#endif
#endif