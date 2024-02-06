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

#if 0
#define SDIO2_DRV_DBG(a, ...) printf(a, ##__VA_ARGS__)
#else
#define SDIO2_DRV_DBG(a, ...)
#endif

#ifndef BOOTROM
#define SDIO2_DRV_ERR(a, ...) printf("[Error]:" a, ##__VA_ARGS__)
#else
extern void bflb_bootrom_printf(char *fmt, ...);
#define SDIO2_DRV_ERR(a, ...) bflb_bootrom_printf("[Error]:" a, ##__VA_ARGS__)
#endif

#if defined(BL602)
uint32_t sdio2_base[] = { 0x4000d000 };
#elif defined(BL616)
uint32_t sdio2_base[] = { 0x2000d000 };
#endif

/* isr */
static void bflb_sdio2_isr(int irq, void *arg);
#if defined(BL616)
static void bflb_sdio2_reset_irq(int irq, void *arg);
#endif

/* dnld irq callback */
static void *sdio2_dnld_irq_callback_arg;
static bflb_sdio2_trans_irq_callback_t sdio2_dnld_irq_callback;
/* upld irq callback */
static void *sdio2_upld_irq_callback_arg;
static bflb_sdio2_trans_irq_callback_t sdio2_upld_irq_callback;
/* error irq callback */
static void *sdio2_error_irq_callback_arg;
static bflb_sdio2_error_irq_callback_t sdio2_error_irq_callback;

#if defined(BL616)
/* reset irq callback */
static void *sdio2_reset_irq_callback_arg;
static bflb_sdio2_reset_irq_callback_t sdio2_reset_irq_callback;
#endif

/* upld/upld max size */
static volatile uint32_t sdio2_upld_size_max, sdio2_dnld_size_max;

/* dnld ring queue */
static volatile bflb_sdio2_trans_desc_t sdio_dnld_port_table[SDIO2_MAX_PORT_NUM];
static volatile uintptr_t sdio_dnld_port_head, sdio_dnld_port_tail;

/* upld ring queue */
static volatile bflb_sdio2_trans_desc_t sdio_upld_port_table[SDIO2_MAX_PORT_NUM];
static volatile uintptr_t sdio_upld_port_head, sdio_upld_port_tail;

int bflb_sdio2_dnld_port_push(struct bflb_device_s *dev, bflb_sdio2_trans_desc_t *trans_desc)
{
    uint32_t reg_base = dev->reg_base;

    uint8_t port_tail = sdio_dnld_port_tail & SDIO2_MAX_PORT_MASK;

    if (sdio_dnld_port_tail - sdio_dnld_port_head >= SDIO2_MAX_PORT_NUM) {
        /* the ring queue is full */
        return -1;
    }

    if (trans_desc->buff_len < bflb_sdio2_get_dnld_max_size(dev)) {
        /* there is a risk of receiving overflow */
        return -2;
    }

    /* attach new buffer */
    putreg8(port_tail, reg_base + SDIO2_WRITE_INDEX_OFFSET);
    putreg32((uint32_t)(uintptr_t)(trans_desc->buff), reg_base + SDIO2_SQ_WRITE_BASE_OFFSET);
    putreg16(1 << port_tail, reg_base + SDIO2_WR_BIT_MAP_OFFSET);

    sdio_dnld_port_table[port_tail] = *trans_desc;
    sdio_upld_port_table[port_tail].data_len = 0;

    /* update dnld_port_tail */
    sdio_dnld_port_tail += 1;

    SDIO2_DRV_DBG("dnld_port_tail %d, bitmap 0x%04X\r\n", sdio_dnld_port_tail, getreg16(reg_base + SDIO2_WR_BIT_MAP_OFFSET));

    return 0;
}

int bflb_sdio2_dnld_get_waiting(struct bflb_device_s *dev)
{
    return sdio_dnld_port_tail - sdio_dnld_port_head;
}

int bflb_sdio2_dnld_get_available(struct bflb_device_s *dev)
{
    return SDIO2_MAX_PORT_NUM - (sdio_dnld_port_tail - sdio_dnld_port_head);
}

int bflb_sdio2_upld_port_push(struct bflb_device_s *dev, bflb_sdio2_trans_desc_t *trans_desc)
{
    uint32_t reg_base = dev->reg_base;

    uint16_t trans_len;
    uint8_t port_tail = sdio_upld_port_tail & SDIO2_MAX_PORT_MASK;

    if (sdio_upld_port_tail - sdio_upld_port_head >= SDIO2_MAX_PORT_NUM) {
        /* the ring queue is full */
        return -1;
    }
    if (trans_desc->data_len == 0) {
        return -2;
    }

    /* buff length check */
    if (trans_desc->data_len > SDIO2_BYTE_MOD_SIZE_MAX) {
        /* block mode, align to block size */
        trans_len = (trans_desc->data_len + bflb_sdio2_get_block_size(dev) - 1);
        trans_len -= trans_len % (bflb_sdio2_get_block_size(dev));
    } else {
        /* byte mode, max size 512 */
        trans_len = trans_desc->data_len;
    }
    if (trans_len > trans_desc->buff_len) {
        return -2;
    }

    /* attach new buffer */
    putreg8(port_tail, reg_base + SDIO2_READ_INDEX_OFFSET);
    putreg32((uint32_t)(uintptr_t)(trans_desc->buff), reg_base + SDIO2_SQ_READ_BASE_OFFSET);
    putreg16(trans_desc->data_len, reg_base + (SDIO2_RD_LEN_OFFSET + port_tail * 2));
    putreg16(1 << port_tail, reg_base + SDIO2_RD_BIT_MAP_OFFSET);

    sdio_upld_port_table[port_tail] = *trans_desc;

    /* update upld_port_tail */
    sdio_upld_port_tail += 1;

    SDIO2_DRV_DBG("upld_port_tail %d, bitmap 0x%04X\r\n\r\n", sdio_upld_port_tail, getreg16(reg_base + SDIO2_RD_BIT_MAP_OFFSET));

    return 0;
}

int bflb_sdio2_upld_get_waiting(struct bflb_device_s *dev)
{
    return sdio_upld_port_tail - sdio_upld_port_head;
}

int bflb_sdio2_upld_get_available(struct bflb_device_s *dev)
{
    return SDIO2_MAX_PORT_NUM - (sdio_upld_port_tail - sdio_upld_port_head);
}

int bflb_sdio2_init(struct bflb_device_s *dev, uint32_t dnld_size_max)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval = 0;

    sdio_dnld_port_head = 0;
    sdio_dnld_port_tail = 0;
    sdio_upld_port_head = 0;
    sdio_upld_port_tail = 0;

    sdio2_dnld_size_max = 0;
    sdio2_upld_size_max = 0;

    if (dnld_size_max) {
        if (dnld_size_max > 0x3f * SDIO2_SIZE_CONSULT_MULTIPLE) {
            /* oversize */
            return -1;
        } else {
            sdio2_dnld_size_max = dnld_size_max - (dnld_size_max % SDIO2_SIZE_CONSULT_MULTIPLE);
        }
    } else {
        sdio2_dnld_size_max = SDIO2_DEFAULT_SIZE_MAX;
    }

    bflb_sdio2_trans_desc_t trans_desc_null = { 0 };
    for (uint32_t i = 0; i < SDIO2_MAX_PORT_NUM; i++) {
        sdio_dnld_port_table[i] = trans_desc_null;
        sdio_upld_port_table[i] = trans_desc_null;
    }

    /* automatic negotiation max len */
    uint8_t scratch_reg = sdio2_dnld_size_max / SDIO2_SIZE_CONSULT_MULTIPLE;
    scratch_reg = (scratch_reg << SDIO2_SCRATCH_MAX_SIZE_SHIFT) | SDIO2_SCRATCH_SLAVE_READY_MASK;
    putreg8(scratch_reg, reg_base + SDIO2_SCRATCH_OFFSET);

    /* clean bitmap (write invalid) */
    putreg16(0, reg_base + SDIO2_RD_BIT_MAP_OFFSET);
    putreg16(0, reg_base + SDIO2_WR_BIT_MAP_OFFSET);

    /* toggle SDIO_CCR_CIC_DnLdOvr on WL_SDIO_CCR_CARD_INT_CAUSE */
    putreg8(SDIO2_CCR_CIC_DnLdOvr, reg_base + SDIO2_CARD_INT_STATUS_OFFSET);
    putreg8(0, reg_base + SDIO2_CARD_INT_STATUS_OFFSET);

    if (1) {
        /* multiport */
        regval = getreg32(reg_base + SDIO2_CONFIG2_OFFSET);
        putreg32(regval | SDIO2_CONFIG2_MSK, reg_base + SDIO2_CONFIG2_OFFSET);
        regval = getreg8(reg_base + SDIO2_CONFIG_OFFSET);
        putreg8(regval | 0x00000010, reg_base + SDIO2_CONFIG_OFFSET);
    }

    /* unmask the interrupts */
    putreg8(SDIO2_CCR_CIM_MASK, reg_base + SDIO2_CARD_INT_MASK_OFFSET);
    /* select interrupt reset mode */
    putreg8(0, reg_base + SDIO2_CARD_INT_MODE_OFFSET);

    bflb_irq_attach(dev->irq_num, bflb_sdio2_isr, dev);
    bflb_irq_enable(dev->irq_num);

#if defined(BL616)
#define BL616_IRQ_SDU_SOFT_RST 19
    bflb_irq_attach(BL616_IRQ_SDU_SOFT_RST, bflb_sdio2_reset_irq, dev);
    bflb_irq_enable(BL616_IRQ_SDU_SOFT_RST);
#endif

    return 0;
}

int bflb_sdio2_check_host_ready(struct bflb_device_s *dev)
{
    uint32_t reg_base = dev->reg_base;
    uint32_t regval = 0;

    regval = getreg8(reg_base + SDIO2_SCRATCH_OFFSET);

    if ((regval & SDIO2_SCRATCH_HOST_READY_MASK) == 0) {
        return 0;
    }

    /* get host max size */
    regval = (regval & SDIO2_SCRATCH_MAX_SIZE_MASK) >> SDIO2_SCRATCH_MAX_SIZE_SHIFT;

    if (regval == 0) {
        sdio2_upld_size_max = SDIO2_DEFAULT_SIZE_MAX;
    } else {
        sdio2_upld_size_max = regval * SDIO2_SIZE_CONSULT_MULTIPLE;
    }

    return 1;
}

int bflb_sdio2_get_upld_max_size(struct bflb_device_s *dev)
{
    if (sdio2_upld_size_max) {
        return sdio2_upld_size_max;
    } else {
        return -1;
    }
}

int bflb_sdio2_get_dnld_max_size(struct bflb_device_s *dev)
{
    if (sdio2_dnld_size_max) {
        return sdio2_dnld_size_max;
    } else {
        return -1;
    }
}

int bflb_sdio2_get_block_size(struct bflb_device_s *dev)
{
    uint16_t blk_size = 0;
    uint32_t reg_base = dev->reg_base;

    blk_size = getreg8(reg_base + SDIO2_FN1_BLK_SIZE_0_OFFSET);
    blk_size |= ((getreg8(reg_base + SDIO2_FN1_BLK_SIZE_1_OFFSET) & SDIO2_FN1_BLK_SIZE_1_MASK) << 8);

    if (blk_size == 0) {
        blk_size = 512;
    }

    return blk_size;
}

int bflb_sdio2_get_dnld_size_max(struct bflb_device_s *dev)
{
    return sdio2_dnld_size_max;
}

int bflb_sdio2_get_upld_size_max(struct bflb_device_s *dev)
{
    return sdio2_upld_size_max;
}

int bflb_sdio2_dnld_irq_attach(struct bflb_device_s *dev, bflb_sdio2_trans_irq_callback_t dnld_irq_callback, void *arg)
{
    sdio2_dnld_irq_callback_arg = arg;
    sdio2_dnld_irq_callback = dnld_irq_callback;

    return 0;
}

int bflb_sdio2_upld_irq_attach(struct bflb_device_s *dev, bflb_sdio2_trans_irq_callback_t upld_irq_callback, void *arg)
{
    sdio2_upld_irq_callback_arg = arg;
    sdio2_upld_irq_callback = upld_irq_callback;

    return 0;
}

int bflb_sdio2_error_irq_attach(struct bflb_device_s *dev, bflb_sdio2_error_irq_callback_t error_irq_callback, void *arg)
{
    sdio2_error_irq_callback_arg = arg;
    sdio2_error_irq_callback = error_irq_callback;

    return 0;
}

#if defined(BL616)
int bflb_sdio2_reset_irq_attach(struct bflb_device_s *dev, bflb_sdio2_reset_irq_callback_t reset_irq_callback, void *arg)
{
    sdio2_reset_irq_callback_arg = arg;
    sdio2_reset_irq_callback = reset_irq_callback;

    return 0;
}
#endif

int bflb_sdio2_trig_host_int(struct bflb_device_s *dev, uint32_t event)
{
    uint32_t reg_base = dev->reg_base;

    /* set event */
    putreg8(event, reg_base + SDIO2_CARD_TO_HOST_EVENT_OFFSET);

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
            if (sdio_upld_port_head == sdio_upld_port_tail) {
                /* the ring queue is empty */
                break;
            } else if (upld_bitmap & (1 << ((uint32_t)sdio_upld_port_head & SDIO2_MAX_PORT_MASK))) {
                /* no upld event */
                break;
            } else {
                /*  */
                bflb_sdio2_trans_desc_t upld_trans_desc = sdio_upld_port_table[sdio_upld_port_head & SDIO2_MAX_PORT_MASK];
                /*  update upld_port_head */
                sdio_upld_port_head += 1;

                /* upld done, isr callback */
                if (sdio2_upld_irq_callback != NULL) {
                    sdio2_upld_irq_callback(sdio2_upld_irq_callback_arg, &upld_trans_desc);
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
            if (sdio_dnld_port_head == sdio_dnld_port_tail) {
                /* the ring queue is empty */
                break;
            } else if (dnld_bitmap & (1 << ((uint32_t)sdio_dnld_port_head & SDIO2_MAX_PORT_MASK))) {
                /* no dnld event */
                break;
            } else {
                /*  */
                bflb_sdio2_trans_desc_t trans_desc = sdio_dnld_port_table[sdio_dnld_port_head & SDIO2_MAX_PORT_MASK];
                trans_desc.data_len = sdio2_dnld_size_max;
                /*  update dnld_port_head */
                sdio_dnld_port_head += 1;

                /* dnld done, isr callback */
                if (sdio2_dnld_irq_callback != NULL) {
                    sdio2_dnld_irq_callback(sdio2_dnld_irq_callback_arg, &trans_desc);
                }
            }
        }
    }

    if (crcerror & SDIO2_CCR_HOST_INT_DnLdCRC_err) {
        SDIO2_DRV_ERR("sdio dnld crc error int\r\n");

        if (sdio2_error_irq_callback != NULL) {
            sdio2_error_irq_callback(sdio2_error_irq_callback_arg, SDIO2_ERROR_CRC);
        }
    }

    if ((regval & SDIO2_CCR_CIC_Abort)) {
        SDIO2_DRV_ERR("sdio abort int\r\n");

        putreg8(~SDIO2_CCR_CIC_Abort, reg_base + SDIO2_CARD_INT_STATUS_OFFSET);

        if (sdio2_error_irq_callback != NULL) {
            sdio2_error_irq_callback(sdio2_error_irq_callback_arg, SDIO2_ERROR_ABORT);
        }
    }
}

#if defined(BL616)

/* sdio2 reset irq (CMD52 write 1<<3 to 0x06) */
static void bflb_sdio2_reset_irq(int irq, void *arg)
{
    struct bflb_device_s *dev = (struct bflb_device_s *)arg;
    uint32_t reg_base = dev->reg_base;
    uint32_t regval = 0;

    bflb_irq_clear_pending(BL616_IRQ_SDU_SOFT_RST);

    uint32_t dnld_desc_num = 0;
    bflb_sdio2_trans_desc_t dnld_desc_s[SDIO2_MAX_PORT_NUM] = { 0 };

    uint32_t upld_desc_num = 0;
    bflb_sdio2_trans_desc_t upld_desc_s[SDIO2_MAX_PORT_NUM] = { 0 };

    /* save dnld desc */
    for (; sdio_dnld_port_tail > sdio_dnld_port_head; sdio_dnld_port_head++) {
        dnld_desc_s[dnld_desc_num] = sdio_dnld_port_table[sdio_dnld_port_head & SDIO2_MAX_PORT_MASK];
        dnld_desc_num += 1;
    }

    /* save upld desc */
    for (; sdio_upld_port_tail > sdio_upld_port_head; sdio_upld_port_head++) {
        upld_desc_s[upld_desc_num] = sdio_upld_port_table[sdio_upld_port_head & SDIO2_MAX_PORT_MASK];
        upld_desc_num += 1;
    }

    /* sdio2 reinit */
    do {
        /* reset port point */
        sdio_dnld_port_head = 0;
        sdio_dnld_port_tail = 0;
        sdio_upld_port_head = 0;
        sdio_upld_port_tail = 0;

        /* automatic negotiation max len */
        uint8_t scratch_reg = sdio2_dnld_size_max / SDIO2_SIZE_CONSULT_MULTIPLE;
        scratch_reg = (scratch_reg << SDIO2_SCRATCH_MAX_SIZE_SHIFT) | SDIO2_SCRATCH_SLAVE_READY_MASK;
        putreg8(scratch_reg, reg_base + SDIO2_SCRATCH_OFFSET);

        /* clean bitmap (write invalid) */
        putreg16(0, reg_base + SDIO2_RD_BIT_MAP_OFFSET);
        putreg16(0, reg_base + SDIO2_WR_BIT_MAP_OFFSET);

        /* toggle SDIO_CCR_CIC_DnLdOvr on WL_SDIO_CCR_CARD_INT_CAUSE */
        putreg8(SDIO2_CCR_CIC_DnLdOvr, reg_base + SDIO2_CARD_INT_STATUS_OFFSET);
        putreg8(0, reg_base + SDIO2_CARD_INT_STATUS_OFFSET);

        if (1) {
            /* multiport */
            regval = getreg32(reg_base + SDIO2_CONFIG2_OFFSET);
            putreg32(regval | SDIO2_CONFIG2_MSK, reg_base + SDIO2_CONFIG2_OFFSET);
            regval = getreg8(reg_base + SDIO2_CONFIG_OFFSET);
            putreg8(regval | 0x00000010, reg_base + SDIO2_CONFIG_OFFSET);
        }

        /* unmask the interrupts */
        putreg8(SDIO2_CCR_CIM_MASK, reg_base + SDIO2_CARD_INT_MASK_OFFSET);
        /* select interrupt reset mode */
        putreg8(0, reg_base + SDIO2_CARD_INT_MODE_OFFSET);

    } while (0);

    /* reset irq callback */
    if (sdio2_reset_irq_callback != NULL) {
        sdio2_reset_irq_callback(sdio2_reset_irq_callback_arg,
                                 upld_desc_s, upld_desc_num,
                                 dnld_desc_s, dnld_desc_num);
    }

    /* automatic negotiation max len (slave ready) */
    uint8_t scratch_reg = sdio2_dnld_size_max / SDIO2_SIZE_CONSULT_MULTIPLE;
    scratch_reg = (scratch_reg << SDIO2_SCRATCH_MAX_SIZE_SHIFT) | SDIO2_SCRATCH_SLAVE_READY_MASK;
    putreg8(scratch_reg, reg_base + SDIO2_SCRATCH_OFFSET);
}

#endif
