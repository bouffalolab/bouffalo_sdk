/**
  ******************************************************************************
  * @file    bflb_emac.c
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

#include "bflb_emac.h"
#include "bflb_clock.h"
#include "hardware/emac_reg.h"

#ifdef EMAC_DEBUG
#define EMAC_DRV_DBG(a, ...) printf("[emac drv]" a, ##__VA_ARGS__)
#else
#define EMAC_DRV_DBG(a, ...)
#endif

/* emac int */
void bflb_emac_isr(int irq, void *arg);

struct bflb_emac_queue_ctrl_s {
    uint32_t emac_tx_bd_head;
    uint32_t emac_tx_bd_tail;

    uint32_t emac_rx_bd_head;
    uint32_t emac_rx_bd_tail;
};

/* isr event callback */
static void *emac_irq_arg = NULL;
static bflb_emac_irq_cb_t emac_irq_event_cb = NULL;

static volatile struct bflb_emac_queue_ctrl_s emac_ctrl = { 0 };

int bflb_emac_init(struct bflb_device_s *dev, const struct bflb_emac_config_s *config)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

#if defined(BL616) || defined(BL808)
#define GLB_EMAC_CLK_OUT_ADDRESS (0x20000390)
#define GLB_UNGATE_CFG2_ADDRESS  (0x20000588)

    /* GLB select inside clock or Not */
    regval = getreg32(GLB_EMAC_CLK_OUT_ADDRESS);
    if (config->clk_internal_mode) {
        regval |= (1 << 5);  /* 1: ref_clk out mode */
        regval |= (1 << 6);  /* 1: ref_clk out invert */
        regval &= ~(1 << 7); /* 1: mac_tx_clk invert */
        regval |= (1 << 10); /* 1: mac_rx_clk invert */
    } else {
        regval &= ~(1 << 5); /* 0: ref_clk in mode */
        regval &= ~(1 << 6);
        regval &= ~(1 << 7);
        regval &= ~(1 << 10);
    }
    putreg32(regval, GLB_EMAC_CLK_OUT_ADDRESS);

    /* ungate emac clock */
    regval = getreg32(GLB_UNGATE_CFG2_ADDRESS);
    regval |= (1 << 23);
    putreg32(regval, GLB_UNGATE_CFG2_ADDRESS);

#elif defined(BL702)
/* enable audio clock and GLB select inside clock or Not */
#define GLB_UNGATE_CFG1_ADDRESS (0x40000024)
#define GLB_CLOCK_CFG3_ADDRESS  (0x4000000C)

    regval = getreg32(GLB_CLOCK_CFG3_ADDRESS);
    if (config->clk_internal_mode) {
        regval |= (1 << 5);
    } else {
        regval &= ~(1 << 5);
    }
    putreg32(regval, GLB_CLOCK_CFG3_ADDRESS);

    /* ungate emac clock */
    regval = getreg32(GLB_UNGATE_CFG1_ADDRESS);
    regval |= (1 << 0x0d);
    putreg32(regval, GLB_UNGATE_CFG1_ADDRESS);
#endif

    /* disbale tx and rx */
    regval = getreg32(reg_base + EMAC_MODE_OFFSET);
    regval &= ~EMAC_TX_EN;
    regval &= ~EMAC_RX_EN;
    putreg32(regval, reg_base + EMAC_MODE_OFFSET);

    /* clean all db */
    for (int i = 0; i < 128 * 2; i++) {
        putreg32(0, reg_base + EMAC_DMA_DESC_OFFSET + i * 4);
    }

    /* defualt config */
    regval = getreg32(reg_base + EMAC_MODE_OFFSET);
    /* enable rmii */
    regval |= EMAC_RMII_EN;
    /* receive small frame */
    regval |= EMAC_RECSMALL;
    /* add pads to small frame */
    regval |= EMAC_PAD;
    /* all additional bytes are dropped */
    regval &= ~EMAC_HUGEN;
    /* enable tx crc */
    regval |= EMAC_CRCEN;
    /* half duplex mode */
    regval &= ~EMAC_FULLD;
    /* disbale frame gap check */
    regval |= EMAC_IFG;
    /* receive all mac_addr frames */
    regval |= EMAC_PRO;
    /* receive all broadcast frames */
    regval |= EMAC_BRO;
    /* enable sent preamble */
    regval &= ~EMAC_NOPRE;
#if (EMAC_SPEED_10M_SUPPORT)
    /* 100M mode */
    regval |= EMAC_100M;
#endif
    putreg32(regval, reg_base + EMAC_MODE_OFFSET);

    /* packet gap, defualt value: 24 clock */
    regval = getreg32(reg_base + EMAC_IPGT_OFFSET);
    regval &= ~(EMAC_IPGT_MASK);
    regval |= (0x18 << EMAC_IPGT_SHIFT) & EMAC_IPGT_MASK;
    putreg32(regval, reg_base + EMAC_IPGT_OFFSET);

    /* packe len limit */
    regval = getreg32(reg_base + EMAC_PACKETLEN_OFFSET);
    regval &= ~(EMAC_MINFL_MASK | EMAC_MAXFL_MASK);
    /* min len */
    if (config->min_frame_len) {
        regval |= (config->min_frame_len << EMAC_MINFL_SHIFT) & EMAC_MINFL_MASK;
    } else {
        regval |= (0x40 << EMAC_MINFL_SHIFT) & EMAC_MINFL_MASK;
    }
    /* max len */
    if (config->max_frame_len) {
        regval |= (config->max_frame_len << EMAC_MAXFL_SHIFT) & EMAC_MAXFL_MASK;
    } else {
        regval |= (0x40 << EMAC_MAXFL_SHIFT) & EMAC_MAXFL_MASK;
    }
    putreg32(regval, reg_base + EMAC_PACKETLEN_OFFSET);

    /* set collision retry and time_window */
    regval = getreg32(reg_base + EMAC_COLLCONFIG_OFFSET);
    regval &= ~(EMAC_MAXRET_MASK | EMAC_COLLVALID_MASK);
    regval |= (0xf << EMAC_MAXRET_SHIFT) & EMAC_MAXRET_MASK;
    regval |= (0x10 << EMAC_COLLVALID_SHIFT) & EMAC_COLLVALID_MASK;
    putreg32(regval, reg_base + EMAC_COLLCONFIG_OFFSET);

    /* set mac address B2~B5 */
    regval = getreg32(reg_base + EMAC_MAC_ADDR0_OFFSET);
    regval &= ~(EMAC_MAC_B2_MASK | EMAC_MAC_B3_MASK | EMAC_MAC_B4_MASK | EMAC_MAC_B5_MASK);
    regval |= config->mac_addr[5] << EMAC_MAC_B5_SHIFT;
    regval |= config->mac_addr[4] << EMAC_MAC_B4_SHIFT;
    regval |= config->mac_addr[3] << EMAC_MAC_B3_SHIFT;
    regval |= config->mac_addr[2] << EMAC_MAC_B2_SHIFT;
    putreg32(regval, reg_base + EMAC_MAC_ADDR0_OFFSET);
    /* set mac address B0~B1 */
    regval = getreg32(reg_base + EMAC_MAC_ADDR1_OFFSET);
    regval &= ~(EMAC_MAC_B0_MASK | EMAC_MAC_B1_MASK);
    regval |= config->mac_addr[1] << EMAC_MAC_B1_SHIFT;
    regval |= config->mac_addr[0] << EMAC_MAC_B0_SHIFT;
    putreg32(regval, reg_base + EMAC_MAC_ADDR1_OFFSET);

    /* Management Data (MD) clk div */
    regval = getreg32(reg_base + EMAC_MIIMODE_OFFSET);
    regval &= ~(EMAC_CLKDIV_MASK);
    regval |= (config->md_clk_div << EMAC_CLKDIV_SHIFT) & EMAC_CLKDIV_MASK;
    /* enable preamble sent */
    regval &= ~EMAC_MIINOPRE;
    putreg32(regval, reg_base + EMAC_MIIMODE_OFFSET);

    /* enable int event */
    regval = getreg32(reg_base + EMAC_INT_MASK_OFFSET);
    regval &= ~(EMAC_RXC_M | EMAC_BUSY_M | EMAC_RXE_M | EMAC_RXB_M); /* rx int */
    regval &= ~(EMAC_TXC_M | EMAC_TXE_M | EMAC_TXB_M);               /* rx int */
    putreg32(regval, reg_base + EMAC_INT_MASK_OFFSET);

#ifndef NOT_USE_BFLB_LHAL_IRQ_ATTACH
    /* enable irq */
    bflb_irq_attach(dev->irq_num, bflb_emac_isr, dev);
    bflb_irq_enable(dev->irq_num);
#endif

    return 0;
}

int bflb_emac_deinit(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* disbale tx and rx */
    regval = getreg32(reg_base + EMAC_MODE_OFFSET);
    regval &= ~EMAC_TX_EN;
    regval &= ~EMAC_RX_EN;
    putreg32(regval, reg_base + EMAC_MODE_OFFSET);

    /* disable irq */
    bflb_irq_disable(dev->irq_num);

    /* TODO: reset emac hw */
    /* clean bd */
    for (int i = 0; i < 128; i++) {
        putreg32(0, reg_base + EMAC_DMA_DESC_OFFSET + i * 4);
    }

    /* clean ctrl */
    arch_memset((void *)&emac_ctrl, 0, sizeof(struct bflb_emac_queue_ctrl_s));

    return 0;
}

int bflb_emac_bd_ctrl_clean(struct bflb_device_s *dev)
{
    uint32_t reg_base = dev->reg_base;

    /* lock */
    uintptr_t flag = bflb_irq_save();

    /* clean bd */
    for (int i = 0; i < 128; i++) {
        putreg32(0, reg_base + EMAC_DMA_DESC_OFFSET + i * 4);
    }

    /* clean ctrl */
    arch_memset((void *)&emac_ctrl, 0, sizeof(struct bflb_emac_queue_ctrl_s));

    /* unlock */
    bflb_irq_restore(flag);

    return 0;
}

int bflb_emac_md_read(struct bflb_device_s *dev, uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* set phy and reg addr */
    regval = getreg32(reg_base + EMAC_MIIADDRESS_OFFSET);
    regval &= ~(EMAC_FIAD_MASK | EMAC_RGAD_MASK);
    regval |= (phy_addr << EMAC_FIAD_SHIFT) & EMAC_FIAD_MASK;
    regval |= (reg_addr << EMAC_RGAD_SHIFT) & EMAC_RGAD_MASK;
    putreg32(regval, reg_base + EMAC_MIIADDRESS_OFFSET);

    /* trigger read */
    regval = getreg32(reg_base + EMAC_MIICOMMAND_OFFSET);
    regval |= (EMAC_RSTAT);
    putreg32(regval, reg_base + EMAC_MIICOMMAND_OFFSET);

    /* wait */
    do {
        arch_delay_us(10);
        regval = getreg32(reg_base + EMAC_MIISTATUS_OFFSET);
    } while (regval & EMAC_MIIM_BUSY);

    regval = getreg32(dev->reg_base + EMAC_MIIRX_DATA_OFFSET);
    *data = (uint16_t)(regval & EMAC_PRSD_MASK);

    return 0;
}

int bflb_emac_md_write(struct bflb_device_s *dev, uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* set phy and reg addr */
    regval = getreg32(reg_base + EMAC_MIIADDRESS_OFFSET);
    regval &= ~(EMAC_FIAD_MASK | EMAC_RGAD_MASK);
    regval |= (phy_addr << EMAC_FIAD_SHIFT) & EMAC_FIAD_MASK;
    regval |= (reg_addr << EMAC_RGAD_SHIFT) & EMAC_RGAD_MASK;
    putreg32(regval, reg_base + EMAC_MIIADDRESS_OFFSET);

    /* set write data */
    regval = getreg32(reg_base + EMAC_MIITX_DATA_OFFSET);
    regval &= ~EMAC_CTRLDATA_MASK;
    regval |= (data << EMAC_CTRLDATA_SHIFT) & EMAC_CTRLDATA_MASK;
    putreg32(regval, dev->reg_base + EMAC_MIITX_DATA_OFFSET);

    /* trigger write */
    regval = getreg32(reg_base + EMAC_MIICOMMAND_OFFSET);
    regval |= (EMAC_WCTRLDATA);
    putreg32(regval, reg_base + EMAC_MIICOMMAND_OFFSET);

    /* wait */
    do {
        arch_delay_us(10);
        regval = getreg32(reg_base + EMAC_MIISTATUS_OFFSET);
    } while (regval & EMAC_MIIM_BUSY);

    return 0;
}

int bflb_emac_queue_tx_push(struct bflb_device_s *dev, struct bflb_emac_trans_desc_s *tx_desc)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval = 0;
    uint32_t bd_index;

    /* lock */
    uintptr_t flag = bflb_irq_save();

    /* check tx_bd_tail */
    if (emac_ctrl.emac_tx_bd_tail - emac_ctrl.emac_tx_bd_head >= EMAC_TX_BD_BUM_MAX) {
        /* the ring queue is full */
        /* unlock */
        bflb_irq_restore(flag);
        return -1;
    }

    bd_index = emac_ctrl.emac_tx_bd_tail & EMAC_TX_BD_BUM_MASK;
    /* update tx_bd_tail */
    emac_ctrl.emac_tx_bd_tail += 1;

    /*  */
    if (!(tx_desc->attr_flag & EMAC_TX_FLAG_FRAGMENT)) {
        regval |= EMAC_BD_TX_EOF_MASK;
    }
    /* enable crc */
    if (!(tx_desc->attr_flag & EMAC_TX_FLAG_NO_CRC)) {
        regval |= EMAC_BD_TX_CRC_MASK;
    }
    /* enable padding attach */
    if (!(tx_desc->attr_flag & EMAC_TX_FLAG_NO_PAD)) {
        regval |= EMAC_BD_TX_PAD_MASK;
    }
    /* enable int */
    if (!(tx_desc->attr_flag & EMAC_TX_FLAG_NO_INT)) {
        regval |= EMAC_BD_TX_IRQ_MASK;
    }
    /* DB end */
    if (bd_index == EMAC_TX_BD_BUM_MASK) {
        regval |= EMAC_BD_TX_WR_MASK;
    }
    /* db ready */
    regval |= EMAC_BD_TX_RD_MASK;
    /* data length */
    regval |= (tx_desc->data_len << EMAC_BD_TX_LEN_SHIFT) & EMAC_BD_TX_LEN_MASK;

    /* hw tx db address */
    uint32_t tx_db_addr = reg_base + EMAC_DMA_DESC_OFFSET + bd_index * sizeof(struct bflb_emac_hw_buff_desc_s);

    /* buff addr */
    putreg32((uintptr_t)tx_desc->buff_addr, tx_db_addr + 4);
    /* len and attr, tx ready */
    putreg32(regval, tx_db_addr);

    /* unlock */
    bflb_irq_restore(flag);

    return 0;
}

int bflb_emac_queue_rx_push(struct bflb_device_s *dev, struct bflb_emac_trans_desc_s *rx_desc)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval = 0;
    uint32_t bd_index;

    /* lock */
    uintptr_t flag = bflb_irq_save();

    /* check rx_bd_tail */
    if (emac_ctrl.emac_rx_bd_tail - emac_ctrl.emac_rx_bd_head >= EMAC_RX_BD_BUM_MAX) {
        /* the ring queue is full */
        /* unlock */
        bflb_irq_restore(flag);
        return -1;
    }

    bd_index = emac_ctrl.emac_rx_bd_tail & EMAC_RX_BD_BUM_MASK;
    /* update rx_bd_tail */
    emac_ctrl.emac_rx_bd_tail += 1;

    /* enable int */
    if (!(rx_desc->attr_flag & EMAC_RX_FLAG_NO_INT)) {
        regval |= EMAC_BD_RX_IRQ_MASK;
    }
    /* DB end */
    if (bd_index == EMAC_RX_BD_BUM_MASK) {
        regval |= EMAC_BD_RX_WR_MASK;
    }
    /* db ready */
    regval |= EMAC_BD_RX_E_MASK;

    /* hw rx db address */
    uint32_t rx_db_addr = reg_base + EMAC_DMA_DESC_OFFSET + (bd_index + 64) * sizeof(struct bflb_emac_hw_buff_desc_s);

    /* buff addr */
    putreg32((uint32_t)(uintptr_t)rx_desc->buff_addr, rx_db_addr + 4);
    /* len and attr, rx ready */
    putreg32(regval, rx_db_addr);

    /* unlock */
    bflb_irq_restore(flag);

    return 0;
}

int bflb_emac_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
    int ret = 0;
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    switch (cmd) {
        case EMAC_CMD_SET_TX_EN:
            /* enable tx, Default:false */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            if (arg) {
                regval |= EMAC_TX_EN;
            } else {
                regval &= ~EMAC_TX_EN;
            }
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

        case EMAC_CMD_SET_TX_AUTO_PADDING:
            /* Add pads to short frames, until the length equals MINFL. Default:true  */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            if (arg) {
                regval |= EMAC_PAD;
            } else {
                regval &= ~EMAC_PAD;
            }
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

        case EMAC_CMD_SET_TX_CRC_FIELD_EN:
            /* TX MAC will append CRC field to every frame. Default:true */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            if (arg) {
                regval |= EMAC_CRCEN;
            } else {
                regval &= ~EMAC_CRCEN;
            }
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

        case EMAC_CMD_SET_TX_PREAMBLE:
            /* 7-byte preamble will be sent. Default:true */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            if (arg) {
                regval &= ~EMAC_NOPRE;
            } else {
                regval |= EMAC_NOPRE;
            }
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

        case EMAC_CMD_SET_TX_GAP_CLK:
            /* Inter packet gap. Default value is 0x18 (24 clock cycles), which equals 9.6 us for 10 Mbps and 0.96 us for 100 Mbps mode*/
            regval = getreg32(reg_base + EMAC_IPGT_OFFSET);
            regval &= ~EMAC_IPGT_MASK;
            regval |= (arg << EMAC_IPGT_SHIFT) & EMAC_IPGT_MASK;
            putreg32(regval, reg_base + EMAC_IPGT_OFFSET);
            break;

        case EMAC_CMD_SET_TX_COLLISION:
            /* Collision valid, This field specifies a collision time window, Default value is 0x3f */
            regval = getreg32(reg_base + EMAC_COLLCONFIG_OFFSET);
            regval &= ~EMAC_COLLVALID_MASK;
            regval |= (arg << EMAC_COLLVALID_SHIFT) & EMAC_COLLVALID_MASK;
            putreg32(regval, reg_base + EMAC_COLLCONFIG_OFFSET);
            break;

        case EMAC_CMD_SET_TX_MAXRET:
            /* maximum number of consequential retransmission attempts after the collision is detected. Default value is 0x0f */
            regval = getreg32(reg_base + EMAC_COLLCONFIG_OFFSET);
            regval &= ~EMAC_MAXRET_MASK;
            regval |= (arg << EMAC_MAXRET_SHIFT) & EMAC_MAXRET_MASK;
            putreg32(regval, reg_base + EMAC_COLLCONFIG_OFFSET);
            break;

        case EMAC_CMD_SET_RX_EN:
            /* enable rx, Default:false */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            if (arg) {
                regval |= EMAC_RX_EN;
            } else {
                regval &= ~EMAC_RX_EN;
            }
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

        case EMAC_CMD_SET_RX_SMALL_FRAME:
            /* Frames smaller than MINFL are accepted, Default:true */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            if (arg) {
                regval |= EMAC_RECSMALL;
            } else {
                regval &= ~EMAC_RECSMALL;
            }
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

        case EMAC_CMD_SET_RX_HUGE_FRAME:
            /* Frame size is not limited by MAXFL and can be up to 64K bytes. Default:false  */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            if (arg) {
                regval |= EMAC_HUGEN;
            } else {
                regval &= ~EMAC_HUGEN;
            }
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

        case EMAC_CMD_SET_RX_GAP_CHECK:
            /* All frames are received regardless to IFG requirement. Default:false  */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            if (arg) {
                regval |= EMAC_IFG;
            } else {
                regval &= ~EMAC_IFG;
            }
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

        case EMAC_CMD_SET_RX_PROMISCUOUS:
            /* All frames received regardless of the address. Default:true  */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            if (arg) {
                regval |= EMAC_PRO;
            } else {
                regval &= ~EMAC_PRO;
            }
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

        case EMAC_CMD_SET_RX_BROADCASE:
            /* Receive all frames containing broadcast address. Default:true  */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            if (arg) {
                regval |= EMAC_BRO;
            } else {
                regval &= ~EMAC_BRO;
            }
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

        case EMAC_CMD_SET_FULL_DUPLEX:
            /* enable full duplex. Default:false */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            if (arg) {
                regval |= EMAC_FULLD;
            } else {
                regval &= ~EMAC_FULLD;
            }
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

#if defined(BL616) || defined(BL808)
        case EMAC_CMD_SET_MAC_RX_CLK_INVERT:
            /* MAC RX clock invert. Default: ref_clk_out mode: true, ref_clk_in mode: false */
            regval = getreg32(GLB_EMAC_CLK_OUT_ADDRESS);
            if (arg) {
                regval |= (1 << 10);
            } else {
                regval &= ~(1 << 10);
            }
            putreg32(regval, GLB_EMAC_CLK_OUT_ADDRESS);
            break;
#endif

#if (EMAC_SPEED_10M_SUPPORT)
        case EMAC_CMD_SET_SPEED_10M:
            /* Switch to 10M mode. Default:100M mode */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            regval &= ~EMAC_100M;
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;

        case EMAC_CMD_SET_SPEED_100M:
            /* Switch to 100M mode. Default:100M mode */
            regval = getreg32(reg_base + EMAC_MODE_OFFSET);
            regval |= EMAC_100M;
            putreg32(regval, reg_base + EMAC_MODE_OFFSET);
            break;
#else
        case EMAC_CMD_SET_SPEED_100M:
            /* Always in 100M mode */
            break;
#endif

        case EMAC_CMD_GET_TX_DB_AVAILABLE:
            ret = EMAC_TX_BD_BUM_MAX - (emac_ctrl.emac_tx_bd_tail - emac_ctrl.emac_tx_bd_head);
            break;

        case EMAC_CMD_GET_RX_DB_AVAILABLE:
            ret = EMAC_RX_BD_BUM_MAX - (emac_ctrl.emac_rx_bd_tail - emac_ctrl.emac_rx_bd_head);
            break;

        case EMAC_CMD_GET_TX_BD_PTR:
            regval = getreg32(reg_base + EMAC_TX_BD_NUM_OFFSET);
            ret = (regval & EMAC_TXBDPTR_MASK) >> EMAC_TXBDPTR_SHIFT;
            break;

        case EMAC_CMD_GET_RX_BD_PTR:
            regval = getreg32(reg_base + EMAC_TX_BD_NUM_OFFSET);
            ret = ((regval & EMAC_RXBDPTR_MASK) >> EMAC_RXBDPTR_SHIFT) - 64;
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

static void bflb_emac_isr_cb_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base = dev->reg_base;

    /* clean int */
    putreg32((EMAC_RXC | EMAC_RXB | EMAC_RXE), reg_base + EMAC_INT_SOURCE_OFFSET);

    uint32_t rx_bd_num = emac_ctrl.emac_rx_bd_tail - emac_ctrl.emac_rx_bd_head;

    for (uint32_t i = 0; i < rx_bd_num; i++) {
        /* get hw bd */
        uint32_t rx_bd_index = emac_ctrl.emac_rx_bd_head & EMAC_RX_BD_BUM_MASK;
        struct bflb_emac_hw_buff_desc_s rx_bd = *(struct bflb_emac_hw_buff_desc_s *)(reg_base + EMAC_DMA_DESC_OFFSET + (rx_bd_index + 64) * sizeof(struct bflb_emac_hw_buff_desc_s));
        /* check bd status */
        if (rx_bd.attribute & EMAC_BD_RX_E_MASK) {
            /* no event */
            break;
        }

        /*  update rx_head */
        emac_ctrl.emac_rx_bd_head += 1;
        /* check cb */
        if (emac_irq_event_cb == NULL) {
            continue;
        }

        struct bflb_emac_trans_desc_s rx_desc = {
            .buff_addr = (void *)(uintptr_t)rx_bd.address,
            .data_len = rx_bd.length,
        };
        /* get err status */
        if (rx_bd.attribute & EMAC_BD_RX_CRC_MASK) {
            rx_desc.err_status |= EMAC_RX_STA_ERR_CRC;
        }
        if (rx_bd.attribute & EMAC_BD_RX_LC_MASK) {
            rx_desc.err_status |= EMAC_RX_STA_ERR_COLLISION;
        }
        if (rx_bd.attribute & EMAC_BD_RX_TL_MASK) {
            rx_desc.err_status |= EMAC_RX_STA_ERR_LONG_FRAME;
        }
        if (rx_bd.attribute & EMAC_BD_RX_OR_MASK) {
            rx_desc.err_status |= EMAC_RX_STA_ERR_FIFO;
        }

        /* callback */
        if (rx_desc.err_status) {
            emac_irq_event_cb(emac_irq_arg, EMAC_IRQ_EVENT_RX_ERR_FRAME, &rx_desc);
        } else if (rx_bd.attribute & EMAC_BD_RX_CF_MASK) {
            emac_irq_event_cb(emac_irq_arg, EMAC_IRQ_EVENT_RX_CTRL_FRAME, &rx_desc);
        } else {
            emac_irq_event_cb(emac_irq_arg, EMAC_IRQ_EVENT_RX_FRAME, &rx_desc);
        }
    }
}

static void bflb_emac_isr_cb_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base = dev->reg_base;

    /* clean int */
    putreg32((EMAC_TXC | EMAC_TXB | EMAC_TXE), reg_base + EMAC_INT_SOURCE_OFFSET);

    uint32_t tx_bd_num = emac_ctrl.emac_tx_bd_tail - emac_ctrl.emac_tx_bd_head;

    for (uint32_t i = 0; i < tx_bd_num; i++) {
        /* get hw bd */
        uint32_t tx_bd_index = emac_ctrl.emac_tx_bd_head & EMAC_TX_BD_BUM_MASK;
        struct bflb_emac_hw_buff_desc_s tx_bd = *(struct bflb_emac_hw_buff_desc_s *)(reg_base + EMAC_DMA_DESC_OFFSET + tx_bd_index * sizeof(struct bflb_emac_hw_buff_desc_s));
        /* check bd status */
        if (tx_bd.attribute & EMAC_BD_TX_RD_MASK) {
            /* no event */
            break;
        }

        /*update tx_head*/
        emac_ctrl.emac_tx_bd_head += 1;
        /* check cb */
        if (emac_irq_event_cb == NULL) {
            continue;
        }

        struct bflb_emac_trans_desc_s tx_desc = {
            .buff_addr = (void *)(uintptr_t)tx_bd.address,
            .data_len = tx_bd.length,
        };
        /* get err status */
        if (tx_bd.attribute & EMAC_BD_TX_LC_MASK) {
            tx_desc.err_status |= EMAC_TX_STA_ERR_COLLISION;
        }
        if (tx_bd.attribute & EMAC_BD_TX_CS_MASK) {
            tx_desc.err_status |= EMAC_TX_STA_ERR_CS;
        }
        if (tx_bd.attribute & EMAC_BD_TX_RL_MASK) {
            tx_desc.err_status |= EMAC_TX_STA_ERR_RETRY_LIMIT;
        }
        if (tx_bd.attribute & EMAC_BD_TX_UR_MASK) {
            tx_desc.err_status |= EMAC_TX_STA_ERR_FIFO;
        }

        /* callback */
        if (tx_desc.err_status) {
            emac_irq_event_cb(emac_irq_arg, EMAC_IRQ_EVENT_TX_ERR_FRAME, &tx_desc);
        } else {
            emac_irq_event_cb(emac_irq_arg, EMAC_IRQ_EVENT_TX_FRAME, &tx_desc);
        }
    }
}

void bflb_emac_isr(int irq, void *arg)
{
    struct bflb_device_s *dev = (struct bflb_device_s *)arg;
    uint32_t reg_base;
    uint32_t intsta, intmask;

    EMAC_DRV_DBG("emac int come\r\n");

    reg_base = dev->reg_base;

    /* get int sta */
    intmask = getreg32(reg_base + EMAC_INT_MASK_OFFSET);
    intsta = getreg32(reg_base + EMAC_INT_SOURCE_OFFSET) & (~intmask);

    /* 1. rx busy, packet is being received and there is no empty buffer descriptor to use */
    if (intsta & EMAC_BUSY) {
        EMAC_DRV_DBG("emac rx_busy\r\n");

        /* clean int */
        putreg32(EMAC_BUSY, reg_base + EMAC_INT_SOURCE_OFFSET);
        /* callback */
        if (emac_irq_event_cb != NULL) {
            emac_irq_event_cb(emac_irq_arg, EMAC_IRQ_EVENT_RX_BUSY, NULL);
        }
    }

    /* 2. rx done */
    if (intsta & (EMAC_RXC | EMAC_RXB | EMAC_RXE)) {
        EMAC_DRV_DBG("emac rx_done\r\n");

        bflb_emac_isr_cb_rx(dev);
    }

    /* 3. tx done */
    if (intsta & (EMAC_TXC | EMAC_TXB | EMAC_TXE)) {
        EMAC_DRV_DBG("emac tx_done\r\n");

        bflb_emac_isr_cb_tx(dev);
    }
}
