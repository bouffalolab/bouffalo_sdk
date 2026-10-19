/**
  ******************************************************************************
  * @file    bflb_kys.c
  * @version V1.0
  * @date    2022-11-17
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
#include "bflb_core.h"
#include "bflb_kys.h"
#include "bflb_clock.h"
#include "bflb_l1c.h"
#include "bflb_gpio.h"
#include "bflb_irq.h"
#include "bflb_mtimer.h"
#include "hardware/kys_reg.h"
#include "hardware/pds_reg.h"
#if defined(BL618DG)
#include "bl618dg_pm.h"
#endif

#define KYS_WHITE_KEY_KYD_MODE  1

static void bflb_kys_gpio_init(const struct bflb_kys_config_s *config)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    int i;

    for (i = 0; i < config->row_num; i++) {
        bflb_gpio_init(gpio, config->row_pins[i], GPIO_FUNC_KEY_SCAN_IN | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    }

    for (i = 0; i < config->col_num; i++) {
        bflb_gpio_init(gpio, config->col_pins[i], GPIO_FUNC_KEY_SCAN_DRIVE | GPIO_ALTERNATE | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_1);
    }
}

static void bflb_kys_matrix_init(const struct bflb_kys_config_s *config)
{
    uint32_t row_sel[2] = {0, 0};
    uint32_t col_sel[2] = {0, 0};
    int i;

    for (i = 0; i < config->row_num; i++) {
        row_sel[i/4] |= config->row_pins[i] << (8 * (i%4));
    }

    for (i = config->row_num; i < 8; i++) {
        row_sel[i/4] |= config->dummy_pin << (8 * (i%4));
    }

    for (i = 0; i < config->col_num; i++) {
        col_sel[i/4] |= config->col_pins[i] << (8 * (i%4));
    }

    putreg32(row_sel[0], PDS_BASE + PDS_KYS_ROW_I_GPIO_SEL0_OFFSET);
    putreg32(row_sel[1], PDS_BASE + PDS_KYS_ROW_I_GPIO_SEL1_OFFSET);
    putreg32(col_sel[0], PDS_BASE + PDS_KYS_COL_O_GPIO_SEL0_OFFSET);
    putreg32(col_sel[1], PDS_BASE + PDS_KYS_COL_O_GPIO_SEL1_OFFSET);
}

static void bflb_kys_kyd_init(const struct bflb_kys_config_s *config)
{
    uint32_t reg_val;

#if defined(BL618DG)
    int pu, pd, ie;
    int i;

    pu = config->kyd_en ? 1 : 0;
    pd = 0;
    ie = config->kyd_en ? 1 : 0;

    for (i = 0; i < config->row_num; i++) {
        pm_set_gpio_pu_pd_ie(config->row_pins[i], pu, pd, ie);
    }

    if (config->row_num < 8) {
        pm_set_gpio_pu_pd_ie(config->dummy_pin, pu, pd, ie);
    }
#endif

    if (config->kyd_en) {
        reg_val = getreg32(PDS_BASE + PDS_KYD_CTL_OFFSET);
        reg_val &= ~PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_MSK;
        reg_val |= ((config->row_num - 1) << PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_POS);
        reg_val &= ~PDS_CR_PDS_KYD_COL_MATRIX_SIZE_MSK;
        reg_val |= ((config->col_num - 1) << PDS_CR_PDS_KYD_COL_MATRIX_SIZE_POS);
        reg_val &= ~PDS_CR_PDS_KYD_COL_O_VAL_MSK;
        reg_val |= (0 << PDS_CR_PDS_KYD_COL_O_VAL_POS);
        reg_val &= ~PDS_CR_PDS_KYD_ROW_I_PULL_EN_MSK;
        reg_val |= (1 << PDS_CR_PDS_KYD_ROW_I_PULL_EN_POS);
        reg_val &= ~PDS_CR_PDS_KYD_EN_MSK;
        reg_val |= (1 << PDS_CR_PDS_KYD_EN_POS);
        putreg32(reg_val, PDS_BASE + PDS_KYD_CTL_OFFSET);
    } else {
        reg_val = getreg32(PDS_BASE + PDS_KYD_CTL_OFFSET);
        reg_val &= ~PDS_CR_PDS_KYD_EN_MSK;
        putreg32(reg_val, PDS_BASE + PDS_KYD_CTL_OFFSET);
    }
}

static void bflb_kys_white_set(uint8_t key_id, uint8_t row_index, uint8_t col_index, uint8_t mode)
{
    uint32_t reg_val;

    switch (key_id) {
        case 0:
            reg_val = getreg32(PDS_BASE + PDS_KYD_WHITE_SET_OFFSET);
            reg_val &= ~PDS_CFG_WHITE_KEY0_ROW_INDEX_MSK;
            reg_val |= (row_index << PDS_CFG_WHITE_KEY0_ROW_INDEX_POS);
            reg_val &= ~PDS_CFG_WHITE_KEY0_COL_INDEX_MSK;
            reg_val |= (col_index << PDS_CFG_WHITE_KEY0_COL_INDEX_POS);
            reg_val &= ~PDS_CFG_WHITE_KEY0_KYD_MODE_MSK;
            reg_val |= (mode << PDS_CFG_WHITE_KEY0_KYD_MODE_POS);
            putreg32(reg_val, PDS_BASE + PDS_KYD_WHITE_SET_OFFSET);
            break;

        case 1:
            reg_val = getreg32(PDS_BASE + PDS_KYD_WHITE_SET_OFFSET);
            reg_val &= ~PDS_CFG_WHITE_KEY1_ROW_INDEX_MSK;
            reg_val |= (row_index << PDS_CFG_WHITE_KEY1_ROW_INDEX_POS);
            reg_val &= ~PDS_CFG_WHITE_KEY1_COL_INDEX_MSK;
            reg_val |= (col_index << PDS_CFG_WHITE_KEY1_COL_INDEX_POS);
            reg_val &= ~PDS_CFG_WHITE_KEY1_KYD_MODE_MSK;
            reg_val |= (mode << PDS_CFG_WHITE_KEY1_KYD_MODE_POS);
            putreg32(reg_val, PDS_BASE + PDS_KYD_WHITE_SET_OFFSET);
            break;

        case 2:
            reg_val = getreg32(PDS_BASE + PDS_KYD_WHITE_SET_OFFSET);
            reg_val &= ~PDS_CFG_WHITE_KEY2_ROW_INDEX_MSK;
            reg_val |= (row_index << PDS_CFG_WHITE_KEY2_ROW_INDEX_POS);
            reg_val &= ~PDS_CFG_WHITE_KEY2_COL_INDEX_MSK;
            reg_val |= (col_index << PDS_CFG_WHITE_KEY2_COL_INDEX_POS);
            reg_val &= ~PDS_CFG_WHITE_KEY2_KYD_MODE_MSK;
            reg_val |= (mode << PDS_CFG_WHITE_KEY2_KYD_MODE_POS);
            putreg32(reg_val, PDS_BASE + PDS_KYD_WHITE_SET_OFFSET);
            break;

        case 3:
            reg_val = getreg32(PDS_BASE + PDS_KYD_WHITE_SET_OFFSET);
            reg_val &= ~PDS_CFG_WHITE_KEY3_ROW_INDEX_MSK;
            reg_val |= (row_index << PDS_CFG_WHITE_KEY3_ROW_INDEX_POS);
            reg_val &= ~PDS_CFG_WHITE_KEY3_COL_INDEX_MSK;
            reg_val |= (col_index << PDS_CFG_WHITE_KEY3_COL_INDEX_POS);
            reg_val &= ~PDS_CFG_WHITE_KEY3_KYD_MODE_MSK;
            reg_val |= (mode << PDS_CFG_WHITE_KEY3_KYD_MODE_POS);
            putreg32(reg_val, PDS_BASE + PDS_KYD_WHITE_SET_OFFSET);
            break;

        default:
            break;
    }
}

static uint8_t bflb_kys_is_white_key(uint8_t row_index, uint8_t col_index, uint8_t mode)
{
    uint32_t white_set;
    uint8_t pattern;

    white_set = getreg32(PDS_BASE + PDS_KYD_WHITE_SET_OFFSET);
    pattern = (row_index << 0) + (col_index << 3) + (mode << 6);

    while (white_set != 0) {
        if ((white_set & 0xFF) == pattern) {
            return 1;
        }
        white_set >>= 8;
    }

    return 0;
}

/**
 * @brief keyscan init
 *
 * @param dev
 * @param config
 */
void bflb_kys_init(struct bflb_device_s *dev, const struct bflb_kys_config_s *config)
{
#ifdef romapi_bflb_kys_init
    romapi_bflb_kys_init(dev, config);
#else
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;
    /* disable kys */
    reg_val = getreg32(reg_base + KYS_KS_CTRL_OFFSET);
    reg_val &= ~(KYS_KS_EN_MASK);
    putreg32(reg_val, reg_base + KYS_KS_CTRL_OFFSET);

    /* mask all interrupt before init */
    reg_val = 0;
    putreg32(reg_val, reg_base + KYS_KS_INT_EN_OFFSET);

    /* kys init */
    reg_val = 0;
    reg_val |= ((config->col_num - 1) << KYS_COL_NUM_SHIFT);
    reg_val |= ((config->row_num - 1) << KYS_ROW_NUM_SHIFT);
    reg_val |= (config->idle_duration << KYS_RC_EXT_SHIFT);
    reg_val |= (config->deglitch_cnt << KYS_DEG_CNT_SHIFT);
    reg_val |= (config->deglitch_en << KYS_DEG_EN_SHIFT);
    reg_val |= (config->ghost_en << KYS_GHOST_EN_SHIFT);
    reg_val |= (0x1 << KYS_FIFO_MODE_SHIFT);
    putreg32(reg_val, reg_base + KYS_KS_CTRL_OFFSET);

    bflb_kys_gpio_init(config);
    bflb_kys_matrix_init(config);
    bflb_kys_kyd_init(config);
#endif
}

/**
 * @brief keyscan enable
 *
 */
void bflb_kys_enable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_kys_enable
    romapi_bflb_kys_enable(dev);
#else
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;
    reg_val = getreg32(reg_base + KYS_KS_CTRL_OFFSET);
    reg_val |= (0x1 << KYS_KS_EN_SHIFT);
    putreg32(reg_val, reg_base + KYS_KS_CTRL_OFFSET);
#endif
}

/**
 * @brief keyscan disable
 *
 */
void bflb_kys_disable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_kys_disable
    romapi_bflb_kys_disable(dev);
#else
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + KYS_KS_CTRL_OFFSET);
    reg_val &= ~(KYS_KS_EN_MASK);
    putreg32(reg_val, reg_base + KYS_KS_CTRL_OFFSET);
#endif
}

/**
 * @brief keyscan interrupt enable
 *
 * @param dev
 * @param flag interrupt flag @KEYSCAN_INT_EN define
 * @param enable enable or disable interrupt
 */
void bflb_kys_int_enable(struct bflb_device_s *dev, uint32_t flag, bool enable)
{
#ifdef romapi_bflb_kys_int_enable
    romapi_bflb_kys_int_enable(dev, flag, enable);
#else
    uint32_t reg_base;
    uint32_t reg_val_en;

    reg_base = dev->reg_base;
    reg_val_en = getreg32(reg_base + KYS_KS_INT_EN_OFFSET);

    if (enable) {
        reg_val_en |= (flag);
    } else {
        reg_val_en &= ~(flag);
    }
    putreg32(reg_val_en, reg_base + KYS_KS_INT_EN_OFFSET);
#endif
}

/**
 * @brief keyscan interrupt clear flag
 *
 * @param dev
 * @param flag
 */
void bflb_kys_int_clear(struct bflb_device_s *dev, uint32_t flag)
{
#ifdef romapi_bflb_kys_int_clear
    romapi_bflb_kys_int_clear(dev, flag);
#else
    putreg32(flag, dev->reg_base + KYS_KEYCODE_CLR_OFFSET);
#endif
}

/**
 * @brief keyscan get interrupt status
 *
 * @param dev
 * @return uint32_t
 */
uint32_t bflb_kys_get_int_status(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_kys_get_int_status
    return romapi_bflb_kys_get_int_status(dev);
#else
    uint32_t reg_base;
    uint32_t reg_sts_val, reg_mask_val;

    reg_base = dev->reg_base;
    reg_sts_val = getreg32(reg_base + KYS_KS_INT_STS_OFFSET);
    reg_mask_val = getreg32(reg_base + KYS_KS_INT_EN_OFFSET);

    return (reg_sts_val & (reg_mask_val));
#endif
}

/**
 * @brief get keyscan keycode index in fifo
 *
 * @param dev
 * @param fifo_head
 * @param fifo_tail
 * @param fifo_valid_cnt
 *
 */
void bflb_kys_get_fifo_info(struct bflb_device_s *dev, uint8_t *fifo_head, uint8_t *fifo_tail, uint8_t *fifo_valid_cnt)
{
#ifdef romapi_bflb_kys_get_fifo_info
    romapi_bflb_kys_get_fifo_info(dev, fifo_head, fifo_tail, fifo_valid_cnt);
#else
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;
    reg_val = getreg32(reg_base + KYS_KEYFIFO_IDX_OFFSET);

    *fifo_head = (uint8_t)(reg_val & 0x7);
    *fifo_tail = (uint8_t)((reg_val & 0x700) >> 8);
    *fifo_valid_cnt = (uint8_t)((reg_val & 0xf0000) >> 16);
#endif
}

/**
 * @brief get keyscan keycode value
 *
 * @param dev
 * @param index
 * @return uint8_t
 */
uint8_t bflb_kys_read_keyvalue(struct bflb_device_s *dev, uint8_t index)
{
#ifdef romapi_bflb_kys_read_keyvalue
    return romapi_bflb_kys_read_keyvalue(dev, index);
#else
    return (uint8_t)(getreg32(dev->reg_base + KYS_KEYFIFO_VALUE_OFFSET) & 0xff);
#endif
}


/**
 * @brief keyscan trigger in poll mode
 *
 * @param dev
 * @param result
 * @param timeout_ms
 * @return int
 */
int bflb_kys_trigger_poll(struct bflb_device_s *dev, struct bflb_kys_result_s *result, uint32_t timeout_ms)
{
#ifdef romapi_bflb_kys_trigger_poll
    return romapi_bflb_kys_trigger_poll(dev, result, timeout_ms);
#else
    uint32_t reg_base;
    uint32_t reg_val;
    uintptr_t mstatus;
    bool ks_done;
    uint64_t time_start;

    reg_base = dev->reg_base;

    /* keyscan is already running */
    reg_val = getreg32(reg_base + KYS_KS_CTRL_OFFSET);
    if (reg_val & KYS_KS_EN_MASK) {
        return -EBUSY;
    }

    /* ks_done interrupt is not enabled */
    reg_val = getreg32(reg_base + KYS_KS_INT_EN_OFFSET);
    if (!(reg_val & KYS_KS_DONE_INT_EN_MASK)) {
        return -EPERM;
    }

    mstatus = bflb_irq_save();

    ks_done = false;
    bflb_kys_enable(dev);

    time_start = bflb_mtimer_get_time_us();
    while (bflb_mtimer_get_time_us() - time_start < (uint64_t)timeout_ms * 1000) {
        if (bflb_kys_get_int_status(dev) & KEYSCAN_INT_STS_DONE) {
            bflb_kys_get_result(dev, result);
            ks_done = true;
            break;
        }
    }

    bflb_kys_abort(dev);

    bflb_irq_restore(mstatus);

    return ks_done ? 0 : -ETIMEDOUT;
#endif
}

/**
 * @brief keyscan trigger in interrupt mode
 *
 * @param dev
 * @return int
 */
int bflb_kys_trigger_interrupt(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_kys_trigger_interrupt
    return romapi_bflb_kys_trigger_interrupt(dev);
#else
    uint32_t reg_base;
    uint32_t reg_val;

    reg_base = dev->reg_base;

    /* keyscan is already running */
    reg_val = getreg32(reg_base + KYS_KS_CTRL_OFFSET);
    if (reg_val & KYS_KS_EN_MASK) {
        return -EBUSY;
    }

    bflb_kys_enable(dev);

    return 0;
#endif
}

/**
 * @brief keyscan get result
 *
 * @param dev
 * @param result
 */
void bflb_kys_get_result(struct bflb_device_s *dev, struct bflb_kys_result_s *result)
{
#ifdef romapi_bflb_kys_get_result
    romapi_bflb_kys_get_result(dev, result);
#else
    uint32_t reg_base;
    uint32_t reg_val;
    uint8_t row_num;
    uint8_t col_num;
    uint8_t key_num;
    uint8_t key_code;
    int i;

    reg_base = dev->reg_base;

    reg_val = getreg32(reg_base + KYS_KS_CTRL_OFFSET);
    row_num = ((reg_val & KYS_ROW_NUM_MASK) >> KYS_ROW_NUM_SHIFT) + 1;
    col_num = ((reg_val & KYS_COL_NUM_MASK) >> KYS_COL_NUM_SHIFT) + 1;
    (void)col_num;

    reg_val = getreg32(reg_base + KYS_KEYFIFO_IDX_OFFSET);
    key_num = ((reg_val & KYS_KEYFIFO_CNT_MASK) >> KYS_KEYFIFO_CNT_SHIFT);

    if (result) {
        result->key_num = key_num;
        for (i = 0; i < result->key_num; i++) {
            reg_val = getreg32(reg_base + KYS_KEYFIFO_VALUE_OFFSET);
            key_code = reg_val & 0x7F;
            result->key_code[i] = key_code;
            result->row_idx[i] = key_code % row_num;
            result->col_idx[i] = key_code / row_num;
        }
    }
#endif
}

/**
 * @brief keyscan abort
 *
 * @param dev
 */
void bflb_kys_abort(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_kys_abort
    romapi_bflb_kys_abort(dev);
#else
    /* disable keyscan, will also clear interrupt status and key fifo */
    bflb_kys_disable(dev);

    /* clear pending irq */
    bflb_irq_clear_pending(dev->irq_num);
#endif
}


/**
 * @brief keyscan set white keys
 *
 * @param white_key_num
 * @param row_idx
 * @param col_idx
 */
void bflb_kys_set_white_keys(uint8_t white_key_num, uint8_t row_idx[], uint8_t col_idx[])
{
#ifdef romapi_bflb_kys_set_white_keys
    romapi_bflb_kys_set_white_keys(white_key_num, row_idx, col_idx);
#else
    int i;

    for (i = 0; i < 4; i++) {
        if (i < white_key_num) {
            bflb_kys_white_set(i, row_idx[i], col_idx[i], KYS_WHITE_KEY_KYD_MODE);
        } else {
            bflb_kys_white_set(i, 0, 0, 0);
        }
    }
#endif
}

/**
 * @brief keyscan get wakeup info
 *
 * @param wakeup_info
 */
void bflb_kys_get_wakeup_info(struct bflb_kys_wakeup_info_s *wakeup_info)
{
#ifdef romapi_bflb_kys_get_wakeup_info
    romapi_bflb_kys_get_wakeup_info(wakeup_info);
#else
    uint32_t reg_val;
    uint8_t row_num;
    uint8_t row_idx;
    uint8_t col_idx;

    reg_val = getreg32(PDS_BASE + PDS_KYD_CTL_OFFSET);
    row_num = ((reg_val & PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_MSK) >> PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_POS) + 1;
    row_idx = ((reg_val & PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_MSK) >> PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_POS);
    col_idx = ((reg_val & PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_MSK) >> PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_POS);

    if (wakeup_info) {
        reg_val = getreg32(PDS_BASE + PDS_KYD_CTL_OFFSET);
        if (reg_val & PDS_RO_PDS_KYD_WAKEUP_MSK) {
            if (bflb_kys_is_white_key(row_idx, col_idx, KYS_WHITE_KEY_KYD_MODE)) {
                wakeup_info->key_event = KEYSCAN_WAKEUP_KEY_EVENT_RELEASE;
            } else {
                wakeup_info->key_event = KEYSCAN_WAKEUP_KEY_EVENT_PRESS;
            }

            wakeup_info->key_code = row_idx + col_idx * row_num;
            wakeup_info->row_idx = row_idx;
            wakeup_info->col_idx = col_idx;
        } else {
            wakeup_info->key_event = KEYSCAN_WAKEUP_KEY_EVENT_NONE;
        }
    }
#endif
}

/**
 * @brief keyscan clear wakeup
 *
 */
void bflb_kys_clear_wakeup(void)
{
#ifdef romapi_bflb_kys_clear_wakeup
    romapi_bflb_kys_clear_wakeup();
#else
    uint32_t reg_val;

    reg_val = getreg32(PDS_BASE + PDS_KYD_CTL_OFFSET);
    reg_val |= PDS_CLR_PDS_KYD_WAKEUP_MSK;
    putreg32(reg_val, PDS_BASE + PDS_KYD_CTL_OFFSET);
#endif
}
