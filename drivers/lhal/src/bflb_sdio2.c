/**
  ******************************************************************************
  * @file    bflb_sdio2.c
  * @version V1.0
  * @date    2022-10-12
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

#include "bflb_sdio2.h"
#include "hardware/sdio2_reg.h"

#ifdef SDIO2_DEBUG
#define SDIO2_DRV_DBG(a, ...) printf("[sdio2 drv]" a, ##__VA_ARGS__)
#else
#define SDIO2_DRV_DBG(a, ...)
#endif

/* sdio reset isr */
#if defined(SDIO2_SOFT_RST_INT_SUP)
static void bflb_sdio2_reset_isr(int irq, void *arg);
#endif
/* sdio int */
static void bflb_sdio2_isr(int irq, void *arg);

struct bflb_sdio2_queue_ctrl_s {
    uint32_t sdio_dnld_port_head;
    uint32_t sdio_dnld_port_tail;
    bflb_sdio2_trans_desc_t sdio_dnld_port_table[SDIO2_MAX_PORT_NUM];

    uint32_t sdio_upld_port_head;
    uint32_t sdio_upld_port_tail;
    bflb_sdio2_trans_desc_t sdio_upld_port_table[SDIO2_MAX_PORT_NUM];
};

/* isr event callback */
static void *sdio2_irq_arg = NULL;
static bflb_sdio2_irq_cb_t sdio2_irq_event_cb = NULL;

/* sdio2 ctrl */
static volatile struct bflb_sdio2_queue_ctrl_s sdio2_ctrl = { 0 };

int bflb_sdio2_init(struct bflb_device_s *dev, uint32_t dnld_size_max)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;
    uint32_t dnld_max;

    if (dnld_size_max) {
        if (dnld_size_max > 0xff * SDIO2_SIZE_CONSULT_MULTIPLE) {
            /* oversize */
            return -1;
        } else {
            dnld_max = dnld_size_max - (dnld_size_max % SDIO2_SIZE_CONSULT_MULTIPLE);
        }
    } else {
        dnld_max = SDIO2_DEFAULT_SIZE_MAX;
    }

    /* clean bitmap (write invalid) */
    putreg16(0, reg_base + SDIO2_RD_BIT_MAP_OFFSET);
    putreg16(0, reg_base + SDIO2_WR_BIT_MAP_OFFSET);

    /* auto clean card2host after cmd53 is completed */
    regval = getreg8(reg_base + SDIO2_CONFIG2_OFFSET);
    regval |= (0x01 << 4);
    putreg8(regval, reg_base + SDIO2_CONFIG2_OFFSET);

    /* unmask the interrupts */
    putreg8(SDIO2_CCR_CIM_DnLdOvr | SDIO2_CCR_CIM_UpLdOvr | SDIO2_CCR_CIM_Abort, reg_base + SDIO2_CARD_INT_MASK_OFFSET);

    /* sdio bootrom/app flag */
    regval = getreg8(reg_base + SDIO2_SCRATCH0_2_OFFSET);
    regval |= SDIO2_STA_FLAG_RD_LEN_COMPRESS_SUP;
#ifdef BOOTROM
    regval &= ~SDIO2_STA_FLAG_NOT_SDIO_BOOT;
#else
    regval |= SDIO2_STA_FLAG_APP_RUN;
#endif
    putreg8(regval, reg_base + SDIO2_SCRATCH0_2_OFFSET);

    /* slave ready / dnld max len */
    putreg8((dnld_max / SDIO2_SIZE_CONSULT_MULTIPLE), reg_base + SDIO2_SCRATCH0_1_OFFSET);

    bflb_irq_attach(dev->irq_num, bflb_sdio2_isr, dev);
    bflb_irq_enable(dev->irq_num);

#if defined(SDIO2_SOFT_RST_INT_SUP)
    bflb_irq_attach(SDIO2_SOFT_RST_IRQ_NUM, bflb_sdio2_reset_isr, dev);
    bflb_irq_enable(SDIO2_SOFT_RST_IRQ_NUM);
#endif

    return 0;
}

int bflb_sdio2_deinit(struct bflb_device_s *dev)
{
    bflb_irq_disable(dev->irq_num);

#if defined(SDIO2_SOFT_RST_INT_SUP)
    bflb_irq_disable(SDIO2_SOFT_RST_IRQ_NUM);
#endif

    /* reset 2_ctrl */
    arch_memset((void *)&sdio2_ctrl, 0, sizeof(sdio2_ctrl));
    return 0;
}

int bflb_sdio2_trig_host_int(struct bflb_device_s *dev, uint32_t event)
{
    uint32_t reg_base = dev->reg_base;

    /* set event */
    putreg8(event, reg_base + SDIO2_CARD_TO_HOST_EVENT_OFFSET);

    return 0;
}

int bflb_sdio2_dnld_port_push(struct bflb_device_s *dev, bflb_sdio2_trans_desc_t *trans_desc)
{
    uint32_t reg_base = dev->reg_base;
    uint8_t port_tail;
    uintptr_t flag;

    if (trans_desc->buff_len < bflb_sdio2_feature_control(dev, SDIO2_CMD_GET_DNLD_MAX_SIZE, 0)) {
        /* there is a risk of receiving overflow */
        return -2;
    }

    /* get dnld_port_tail */
    if (sdio2_ctrl.sdio_dnld_port_tail - sdio2_ctrl.sdio_dnld_port_head >= SDIO2_MAX_PORT_NUM) {
        /* the ring queue is full */
        return -1;
    }
    port_tail = sdio2_ctrl.sdio_dnld_port_tail & SDIO2_MAX_PORT_MASK;

    sdio2_ctrl.sdio_dnld_port_table[port_tail] = *trans_desc;
    sdio2_ctrl.sdio_upld_port_table[port_tail].data_len = 0;

    /* lock */
    flag = bflb_irq_save();

    /* attach buffer address */
    putreg8(port_tail, reg_base + SDIO2_WRITE_INDEX_OFFSET);
    putreg32((uint32_t)(uintptr_t)(trans_desc->buff), reg_base + SDIO2_SQ_WRITE_BASE_OFFSET);

    /* set dnld bitmap  */
    putreg16(1 << port_tail, reg_base + SDIO2_WR_BIT_MAP_OFFSET);

    /* update dnld_port_tail */
    sdio2_ctrl.sdio_dnld_port_tail += 1;

    /* trig dnld card2host int */
    bflb_sdio2_trig_host_int(dev, SDIO2_HOST_INT_EVENT_DNLD_RDY);

    /* unlock */
    bflb_irq_restore(flag);

    SDIO2_DRV_DBG("dnld_port_tail %d, bitmap 0x%04X\r\n", sdio2_ctrl.sdio_dnld_port_tail, getreg16(reg_base + SDIO2_WR_BIT_MAP_OFFSET));

    return 0;
}

int bflb_sdio2_upld_port_push(struct bflb_device_s *dev, bflb_sdio2_trans_desc_t *trans_desc)
{
    uint32_t reg_base = dev->reg_base;
    uint16_t trans_len;
    uint8_t port_tail;
    uintptr_t flag;

    /* check buff length */
    if (trans_desc->data_len > SDIO2_BYTE_MOD_SIZE_MAX) {
        /* block mode, align to block size */
        uint16_t block_size = (uint16_t)bflb_sdio2_feature_control(dev, SDIO2_CMD_GET_FUNC_BLK_SIZE, 0);
        trans_len = (trans_desc->data_len + block_size - 1);
        trans_len -= trans_len % block_size;
    } else {
        /* byte mode, max size 512 */
        trans_len = trans_desc->data_len;
    }
    if (trans_len > trans_desc->buff_len || trans_desc->data_len == 0) {
        return -1;
    }

    /* get upld_port_tail */
    if (sdio2_ctrl.sdio_upld_port_tail - sdio2_ctrl.sdio_upld_port_head >= SDIO2_MAX_PORT_NUM) {
        /* the ring queue is full */
        return -2;
    }
    port_tail = sdio2_ctrl.sdio_upld_port_tail & SDIO2_MAX_PORT_MASK;

    sdio2_ctrl.sdio_upld_port_table[port_tail] = *trans_desc;

    /* set upld len */
    uint16_t blk_size, upld_elem;
    blk_size = (uint16_t)bflb_sdio2_feature_control(dev, SDIO2_CMD_GET_FUNC_BLK_SIZE, 0);
    if (!blk_size || trans_desc->data_len % blk_size) {
        /* byte mode */
        if (trans_desc->data_len < 64) {
            upld_elem = (trans_desc->data_len << 2) | 0x00; /* 1-byte, byte mode */
        } else {
            upld_elem = (trans_desc->data_len << 2) | 0x01; /* 2-byte, byte mode */
        }
    } else {
        /* block mode */
        if (trans_desc->data_len / blk_size < 64) {
            upld_elem = ((trans_desc->data_len / blk_size) << 2) | 0x02; /* 1-byte, block mode */
        } else {
            upld_elem = ((trans_desc->data_len / blk_size) << 2) | 0x03; /* 2-byte, block mode */
        }
    }

    /* lock */
    flag = bflb_irq_save();

    /* set read_len */
    putreg16(upld_elem, reg_base + (SDIO2_RD_LEN_OFFSET + port_tail * 2));

    /* attach buffer address */
    putreg8(port_tail, reg_base + SDIO2_READ_INDEX_OFFSET);
    putreg32((uint32_t)(uintptr_t)(trans_desc->buff), reg_base + SDIO2_SQ_READ_BASE_OFFSET);

    /* set upld bitmap  */
    putreg16(1 << port_tail, reg_base + SDIO2_RD_BIT_MAP_OFFSET);

    /* update upld_port_tail */
    sdio2_ctrl.sdio_upld_port_tail += 1;

    /* trig dnld card2host int */
    bflb_sdio2_trig_host_int(dev, SDIO2_HOST_INT_EVENT_UPLD_RDY);

    /* unlock */
    bflb_irq_restore(flag);

    SDIO2_DRV_DBG("upld_port_tail %d, bitmap 0x%04X\r\n\r\n", sdio2_ctrl.sdio_upld_port_tail, getreg16(reg_base + SDIO2_RD_BIT_MAP_OFFSET));

    return 0;
}

/* pop dnld queue, can only be used after reset. */
int bflb_sdio2_dnld_port_pop(struct bflb_device_s *dev, bflb_sdio2_trans_desc_t *trans_desc)
{
    int ret = 0;
    arch_memset(trans_desc, 0, sizeof(bflb_sdio2_trans_desc_t));

    /* lock */
    uintptr_t flag = bflb_irq_save();
    if (sdio2_ctrl.sdio_dnld_port_tail - sdio2_ctrl.sdio_dnld_port_head > 0) {
        *trans_desc = sdio2_ctrl.sdio_dnld_port_table[sdio2_ctrl.sdio_dnld_port_head & SDIO2_MAX_PORT_MASK];
        sdio2_ctrl.sdio_dnld_port_head++;
    } else {
        ret = 1;
    }
    /* unlock */
    bflb_irq_restore(flag);

    return ret;
}

/* pop upld queue, can only be used after reset. */
int bflb_sdio2_upld_port_pop(struct bflb_device_s *dev, bflb_sdio2_trans_desc_t *trans_desc)
{
    int ret = 0;
    arch_memset(trans_desc, 0, sizeof(bflb_sdio2_trans_desc_t));

    /* lock */
    uintptr_t flag = bflb_irq_save();
    if (sdio2_ctrl.sdio_upld_port_tail - sdio2_ctrl.sdio_upld_port_head > 0) {
        *trans_desc = sdio2_ctrl.sdio_upld_port_table[sdio2_ctrl.sdio_upld_port_head & SDIO2_MAX_PORT_MASK];
        sdio2_ctrl.sdio_upld_port_head++;
    } else {
        ret = 1;
    }
    /* unlock */
    bflb_irq_restore(flag);

    return ret;
}

/* feature control */
int bflb_sdio2_feature_control(struct bflb_device_s *dev, int cmd, uintptr_t arg)
{
    int ret = 0;
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    switch (cmd) {
        case SDIO2_CMD_GET_BUS_WIDTH:
            /* get sdio bus width */
            regval = getreg8(dev->reg_base + SDIO2_BUS_WIDTH_OFFSET);
            if (regval & 0x02) {
                ret = 4;
            } else {
                ret = 1;
            }
            break;

        case SDIO2_CMD_GET_FUNC_HOST_READY:
            /* get host ready status */
            regval = getreg8(dev->reg_base + SDIO2_SCRATCH0_0_OFFSET);
            if (regval) {
                ret = 1;
            } else {
                ret = 0;
            }
            break;

        case SDIO2_CMD_GET_FUNC_BLK_SIZE:
            /* get block size */
            regval = getreg8(reg_base + SDIO2_FN1_BLK_SIZE_0_OFFSET);
            regval |= ((getreg8(reg_base + SDIO2_FN1_BLK_SIZE_1_OFFSET) & SDIO2_FN1_BLK_SIZE_1_MASK) << 8);
            if (regval) {
                ret = regval;
            } else {
                ret = 512;
            }
            break;

        case SDIO2_CMD_GET_DNLD_MAX_SIZE:
            /* get dnld max size (slave buff size) */
            regval = getreg8(dev->reg_base + SDIO2_SCRATCH0_1_OFFSET);
            if (regval) {
                ret = regval * SDIO2_SIZE_CONSULT_MULTIPLE;
            } else {
                ret = -1;
            }
            break;

        case SDIO2_CMD_GET_UPLD_MAX_SIZE:
            /* get upld max size (host buff size) */
            regval = getreg8(dev->reg_base + SDIO2_SCRATCH0_0_OFFSET);
            if (regval) {
                ret = regval * SDIO2_SIZE_CONSULT_MULTIPLE;
            } else {
                ret = -1;
            }
            break;

        case SDIO2_CMD_GET_DNLD_QUEUE_WAIT_NUM:
            ret = sdio2_ctrl.sdio_dnld_port_tail - sdio2_ctrl.sdio_dnld_port_head;
            break;

        case SDIO2_CMD_GET_DNLD_QUEUE_AVAILABLE:
            ret = SDIO2_MAX_PORT_NUM - (sdio2_ctrl.sdio_dnld_port_tail - sdio2_ctrl.sdio_dnld_port_head);
            break;

        case SDIO2_CMD_GET_UPLD_QUEUE_WAIT_NUM:
            ret = sdio2_ctrl.sdio_upld_port_tail - sdio2_ctrl.sdio_upld_port_head;
            break;

        case SDIO2_CMD_GET_UPLD_QUEUE_AVAILABLE:
            ret = SDIO2_MAX_PORT_NUM - (sdio2_ctrl.sdio_upld_port_tail - sdio2_ctrl.sdio_upld_port_head);
            break;

        default:
            ret = -1;
            break;
    }

    return ret;
}

/* isr callback attach */
int bflb_sdio2_irq_attach(struct bflb_device_s *dev, bflb_sdio2_irq_cb_t irq_event_cb, void *arg)
{
    sdio2_irq_event_cb = irq_event_cb;
    sdio2_irq_arg = arg;

    return 0;
}

static void bflb_sdio2_isr(int irq, void *arg)
{
    struct bflb_device_s *dev = (struct bflb_device_s *)arg;
    uint32_t reg_base;
    uint32_t regval;
    uint8_t crcerror;

    SDIO2_DRV_DBG("sdio2 isr\r\n");

    reg_base = dev->reg_base;
    regval = getreg8(reg_base + SDIO2_CARD_INT_STATUS_OFFSET);
    crcerror = getreg8(reg_base + SDIO2_HOST_TRANS_STATUS_OFFSET);

    if ((regval & SDIO2_CCR_CIC_UpLdOvr)) {
        SDIO2_DRV_DBG("sdio upld int\r\n");

        /* clean upld done interrupt */
        putreg8(~SDIO2_CCR_CIC_UpLdOvr, reg_base + SDIO2_CARD_INT_STATUS_OFFSET);

        /* get upld bitmap */
        uint16_t upld_bitmap = getreg16(reg_base + SDIO2_RD_BIT_MAP_OFFSET);

        SDIO2_DRV_DBG("sdio upld bitmap 0x%04X\r\n", upld_bitmap);

        for (uint8_t i = 0; i < SDIO2_MAX_PORT_NUM; i++) {
            if (sdio2_ctrl.sdio_upld_port_head == sdio2_ctrl.sdio_upld_port_tail) {
                /* the ring queue is empty */
                break;
            } else if (upld_bitmap & (1 << ((uint32_t)sdio2_ctrl.sdio_upld_port_head & SDIO2_MAX_PORT_MASK))) {
                /* no upld event */
                break;
            } else {
                /*  */
                bflb_sdio2_trans_desc_t upld_trans_desc = sdio2_ctrl.sdio_upld_port_table[sdio2_ctrl.sdio_upld_port_head & SDIO2_MAX_PORT_MASK];
                /*  update upld_port_head */
                sdio2_ctrl.sdio_upld_port_head += 1;

                /* upld done, isr callback */
                if (sdio2_irq_event_cb != NULL) {
                    sdio2_irq_event_cb(sdio2_irq_arg, SDIO2_IRQ_EVENT_UPLD_CPL, &upld_trans_desc);
                    // sdio2_upld_irq_callback(sdio2_ctrl.sdio2_upld_irq_callback_arg, &upld_trans_desc);
                }
            }
        }
    }

    if ((regval & SDIO2_CCR_CIC_DnLdOvr) &&
        !(crcerror & SDIO2_CCR_HOST_INT_DnLdCRC_err)) {
        SDIO2_DRV_DBG("sdio dnld int\r\n");

        /* clean dnld done interrupt */
        putreg8(~SDIO2_CCR_CIC_DnLdOvr, reg_base + SDIO2_CARD_INT_STATUS_OFFSET);

        /* get dnld bitmap */
        uint16_t dnld_bitmap = getreg16(reg_base + SDIO2_WR_BIT_MAP_OFFSET);

        SDIO2_DRV_DBG("sdio dnld bitmap 0x%04X\r\n", dnld_bitmap);

        for (uint8_t i = 0; i < SDIO2_MAX_PORT_NUM; i++) {
            if (sdio2_ctrl.sdio_dnld_port_head == sdio2_ctrl.sdio_dnld_port_tail) {
                /* the ring queue is empty */
                break;
            } else if (dnld_bitmap & (1 << ((uint32_t)sdio2_ctrl.sdio_dnld_port_head & SDIO2_MAX_PORT_MASK))) {
                /* no dnld event */
                break;
            } else {
                /*  */
                bflb_sdio2_trans_desc_t trans_desc = sdio2_ctrl.sdio_dnld_port_table[sdio2_ctrl.sdio_dnld_port_head & SDIO2_MAX_PORT_MASK];
                trans_desc.data_len = getreg8(dev->reg_base + SDIO2_SCRATCH0_1_OFFSET) * SDIO2_SIZE_CONSULT_MULTIPLE;
                /*  update dnld_port_head */
                sdio2_ctrl.sdio_dnld_port_head += 1;

                /* dnld done, isr callback */
                if (sdio2_irq_event_cb != NULL) {
                    sdio2_irq_event_cb(sdio2_irq_arg, SDIO2_IRQ_EVENT_DNLD_CPL, &trans_desc);
                }
            }
        }
    }

    if (crcerror & SDIO2_CCR_HOST_INT_DnLdCRC_err) {
        SDIO2_DRV_DBG("sdio dnld crc error int\r\n");

        if (sdio2_irq_event_cb != NULL) {
            sdio2_irq_event_cb(sdio2_irq_arg, SDIO2_IRQ_EVENT_ERR_CRC, NULL);
        }
    }

    if ((regval & SDIO2_CCR_CIC_Abort)) {
        SDIO2_DRV_DBG("sdio abort int\r\n");

        putreg8(~SDIO2_CCR_CIC_Abort, reg_base + SDIO2_CARD_INT_STATUS_OFFSET);

        if (sdio2_irq_event_cb != NULL) {
            sdio2_irq_event_cb(sdio2_irq_arg, SDIO2_IRQ_EVENT_ERR_ABORT, NULL);
        }
    }
}

#if defined(SDIO2_SOFT_RST_INT_SUP)
/* sdio2 reset irq (CMD52 write 1<<3 to 0x06) */
static void bflb_sdio2_reset_isr(int irq, void *arg)
{
    bflb_irq_clear_pending(SDIO2_SOFT_RST_IRQ_NUM);

    if (sdio2_irq_event_cb != NULL) {
        sdio2_irq_event_cb(sdio2_irq_arg, SDIO2_IRQ_EVENT_SOFT_RESET, NULL);
    }
}
#endif
