#include "bflb_kys_v2.h"
#include "bflb_gpio.h"
#include "bl618dg_glb.h"
#include "bl618dg_hbn.h"
#include "bl618dg_pds.h"
#include "bl618dg_pm.h"
#include "hardware/bl618dg.h"
#include "hardware/pds_reg.h"
#include "hardware/glb_reg.h"
#include "hardware/kys_reg.h"

#if !defined(BL618DG)
#error "bflb_kys_v2 is only supported on BL618DG"
#endif

void bflb_kys_v2_init(struct bflb_device_s *dev, const struct bflb_kys_v2_config_s *config)
{
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(config);
    LHAL_PARAM_ASSERT(config->row >= 1 && config->row <= 8);
    LHAL_PARAM_ASSERT(config->col >= 1 && config->col <= 8);
    LHAL_PARAM_ASSERT(config->idle_duration <= 3);
    LHAL_PARAM_ASSERT(config->deglitch_cnt <= 15);

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_KYS);

    regval = getreg32(GLB_BASE + GLB_SOC_INFO0_OFFSET);
    regval &= ~(GLB_KYS_1M_CLK_DIV_MSK | GLB_KYS_1M_CLK_SEL_MSK);
    regval |= GLB_KYS_1M_CLK_SEL_MSK | (3U << GLB_KYS_1M_CLK_DIV_POS);
    putreg32(regval, GLB_BASE + GLB_SOC_INFO0_OFFSET);

    regval = getreg32(dev->reg_base + KYS_KS_CTRL_OFFSET);
    regval &= ~(KYS_KS_EN_MASK | KYS_FIFO_MODE_MASK | KYS_COL_NUM_MASK | KYS_ROW_NUM_MASK |
                KYS_RC_EXT_MASK | KYS_GHOST_EN_MASK | KYS_DEG_EN_MASK | KYS_DEG_CNT_MASK);
    /* Hardware COL_O drives logical rows; hardware ROW_I samples logical columns. */
    regval |= KYS_FIFO_MODE_MASK |
              (((uint32_t)(config->row - 1) << KYS_COL_NUM_SHIFT) & KYS_COL_NUM_MASK) |
              (((uint32_t)(config->col - 1) << KYS_ROW_NUM_SHIFT) & KYS_ROW_NUM_MASK) |
              (((uint32_t)config->idle_duration << KYS_RC_EXT_SHIFT) & KYS_RC_EXT_MASK) |
              (((uint32_t)config->ghost_en << KYS_GHOST_EN_SHIFT) & KYS_GHOST_EN_MASK) |
              (((uint32_t)config->deglitch_en << KYS_DEG_EN_SHIFT) & KYS_DEG_EN_MASK) |
              (((uint32_t)config->deglitch_cnt << KYS_DEG_CNT_SHIFT) & KYS_DEG_CNT_MASK);
    putreg32(regval, dev->reg_base + KYS_KS_CTRL_OFFSET);

    putreg32(0, dev->reg_base + KYS_KS_INT_EN_OFFSET);
    bflb_kys_v2_int_clear(dev, BFLB_KYS_V2_INT_CLEAR_ALL);
}

void bflb_kys_v2_enable(struct bflb_device_s *dev)
{
    uint32_t regval;
    LHAL_PARAM_ASSERT(dev);
    regval = getreg32(dev->reg_base + KYS_KS_CTRL_OFFSET);
    putreg32(regval | KYS_KS_EN_MASK, dev->reg_base + KYS_KS_CTRL_OFFSET);
}

void bflb_kys_v2_disable(struct bflb_device_s *dev)
{
    uint32_t regval;
    LHAL_PARAM_ASSERT(dev);
    regval = getreg32(dev->reg_base + KYS_KS_CTRL_OFFSET);
    putreg32(regval & ~KYS_KS_EN_MASK, dev->reg_base + KYS_KS_CTRL_OFFSET);
}

void bflb_kys_v2_int_enable(struct bflb_device_s *dev, uint32_t flag, bool enable)
{
    uint32_t regval;
    LHAL_PARAM_ASSERT(dev);
    flag &= BFLB_KYS_V2_INT_ENABLE_ALL;
    regval = getreg32(dev->reg_base + KYS_KS_INT_EN_OFFSET);
    regval = enable ? (regval | flag) : (regval & ~flag);
    putreg32(regval, dev->reg_base + KYS_KS_INT_EN_OFFSET);
}

void bflb_kys_v2_int_clear(struct bflb_device_s *dev, uint32_t flag)
{
    LHAL_PARAM_ASSERT(dev);
    putreg32(flag & BFLB_KYS_V2_INT_CLEAR_ALL, dev->reg_base + KYS_KEYCODE_CLR_OFFSET);
}

uint32_t bflb_kys_v2_get_int_status(struct bflb_device_s *dev)
{
    uint32_t int_enable;

    LHAL_PARAM_ASSERT(dev);
    int_enable = getreg32(dev->reg_base + KYS_KS_INT_EN_OFFSET);
    return bflb_kys_v2_get_raw_int_status(dev) & int_enable;
}

uint32_t bflb_kys_v2_get_raw_int_status(struct bflb_device_s *dev)
{
    LHAL_PARAM_ASSERT(dev);
    return getreg32(dev->reg_base + KYS_KS_INT_STS_OFFSET) & BFLB_KYS_V2_INT_ENABLE_ALL;
}

uint32_t bflb_kys_v2_get_fifo_count(struct bflb_device_s *dev)
{
    LHAL_PARAM_ASSERT(dev);
    return (getreg32(dev->reg_base + KYS_KEYFIFO_IDX_OFFSET) >> 16) & 0xfU;
}

uint8_t bflb_kys_v2_read_keyvalue(struct bflb_device_s *dev)
{
    LHAL_PARAM_ASSERT(dev);
    return (uint8_t)(getreg32(dev->reg_base + KYS_KEYFIFO_VALUE_OFFSET) & 0x7fU);
}

int bflb_kys_v2_get_result(struct bflb_device_s *dev, struct bflb_kys_v2_result_s *result)
{
    uint32_t count;

    if ((dev == NULL) || (result == NULL)) {
        return -EINVAL;
    }

    memset(result, 0, sizeof(*result));
    result->status = bflb_kys_v2_get_raw_int_status(dev);
    count = bflb_kys_v2_get_fifo_count(dev);
    result->count = (count > BFLB_KYS_V2_MAX_KEYS) ? BFLB_KYS_V2_MAX_KEYS : (uint8_t)count;
    for (uint32_t i = 0; i < count; i++) {
        uint8_t keycode = bflb_kys_v2_read_keyvalue(dev);

        if (i < BFLB_KYS_V2_MAX_KEYS) {
            result->keycode[i] = keycode;
        }
    }

    return 0;
}

int bflb_kys_v2_trigger_poll(struct bflb_device_s *dev,
                             struct bflb_kys_v2_result_s *result,
                             uint32_t timeout_ms)
{
    uint32_t count;
    uint64_t start_ms;

    if ((dev == NULL) || (result == NULL)) {
        return -EINVAL;
    }
    if ((getreg32(dev->reg_base + KYS_KS_INT_EN_OFFSET) & BFLB_KYS_V2_INT_ENABLE_ALL) != 0U) {
        return -EBUSY;
    }

    memset(result, 0, sizeof(*result));
    bflb_kys_v2_disable(dev);
    bflb_kys_v2_int_clear(dev, BFLB_KYS_V2_INT_CLEAR_ALL);

    /*
     * The KYS block only asserts the terminal status when the
     * corresponding source is enabled.  Polling does not enable the CPU IRQ,
     * but it still needs the KYS source mask, as in the vendor reference
     * implementation.
     */
    bflb_kys_v2_int_enable(dev,
                           BFLB_KYS_V2_INT_DONE | BFLB_KYS_V2_INT_FIFO_FULL |
                               BFLB_KYS_V2_INT_GHOST,
                           true);

    start_ms = bflb_mtimer_get_time_ms();
    bflb_kys_v2_enable(dev);
    while ((bflb_kys_v2_get_raw_int_status(dev) & BFLB_KYS_V2_INT_DONE) == 0) {
        if ((timeout_ms != 0U) && ((bflb_mtimer_get_time_ms() - start_ms) >= timeout_ms)) {
            bflb_kys_v2_disable(dev);
            result->status = bflb_kys_v2_get_raw_int_status(dev);
            result->count = (bflb_kys_v2_get_fifo_count(dev) > BFLB_KYS_V2_MAX_KEYS) ?
                                BFLB_KYS_V2_MAX_KEYS :
                                (uint8_t)bflb_kys_v2_get_fifo_count(dev);
            count = bflb_kys_v2_get_fifo_count(dev);
            for (uint32_t i = 0; i < count; i++) {
                (void)bflb_kys_v2_read_keyvalue(dev);
            }
            bflb_kys_v2_int_clear(dev, BFLB_KYS_V2_INT_CLEAR_ALL);
            bflb_kys_v2_int_enable(dev,
                                   BFLB_KYS_V2_INT_DONE | BFLB_KYS_V2_INT_FIFO_FULL |
                                       BFLB_KYS_V2_INT_GHOST,
                                   false);
            return -ETIMEDOUT;
        }
    }

    bflb_kys_v2_get_result(dev, result);
    bflb_kys_v2_disable(dev);
    bflb_kys_v2_int_clear(dev, BFLB_KYS_V2_INT_CLEAR_ALL);
    bflb_kys_v2_int_enable(dev,
                           BFLB_KYS_V2_INT_DONE | BFLB_KYS_V2_INT_FIFO_FULL |
                               BFLB_KYS_V2_INT_GHOST,
                           false);

    return 0;
}

static uint32_t bflb_kys_v2_pack_pins(uint8_t count, const uint8_t pins[], uint8_t start)
{
    uint32_t value = 0;
    uint8_t end = start + 4;

    for (uint8_t i = start; i < count && i < end; i++) {
        value |= ((uint32_t)pins[i] & 0x3fU) << ((i - start) * 8U);
    }
    return value;
}

void bflb_kys_v2_matrix_init(uint8_t row_count, uint8_t col_count,
                             const uint8_t row_pins[], const uint8_t col_pins[])
{
    struct bflb_device_s *gpio;

    LHAL_PARAM_ASSERT(row_count >= 1 && row_count <= BFLB_KYS_V2_MAX_KEYS);
    LHAL_PARAM_ASSERT(col_count >= 1 && col_count <= BFLB_KYS_V2_MAX_KEYS);
    LHAL_PARAM_ASSERT(row_pins);
    LHAL_PARAM_ASSERT(col_pins);

    gpio = bflb_device_get_by_name(BFLB_NAME_GPIO);
    for (uint8_t i = 0; i < row_count; i++) {
        bflb_gpio_init(gpio, row_pins[i],
                       GPIO_FUNC_KEY_SCAN_DRIVE | GPIO_ALTERNATE | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_1);
    }
    for (uint8_t i = 0; i < col_count; i++) {
        bflb_gpio_init(gpio, col_pins[i],
                       GPIO_FUNC_KEY_SCAN_IN | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    }

    putreg32(bflb_kys_v2_pack_pins(col_count, col_pins, 0),
             PDS_BASE + PDS_KYS_ROW_I_GPIO_SEL0_OFFSET);
    putreg32(bflb_kys_v2_pack_pins(col_count, col_pins, 4),
             PDS_BASE + PDS_KYS_ROW_I_GPIO_SEL1_OFFSET);
    putreg32(bflb_kys_v2_pack_pins(row_count, row_pins, 0),
             PDS_BASE + PDS_KYS_COL_O_GPIO_SEL0_OFFSET);
    putreg32(bflb_kys_v2_pack_pins(row_count, row_pins, 4),
             PDS_BASE + PDS_KYS_COL_O_GPIO_SEL1_OFFSET);
}

void bflb_kys_v2_pds_gpio_configure(uint8_t col_count, const uint8_t col_pins[])
{
    LHAL_PARAM_ASSERT(col_count >= 1 && col_count <= BFLB_KYS_V2_MAX_KEYS);
    LHAL_PARAM_ASSERT(col_pins);

    /* KEY_SCAN_IN columns need a defined idle level in PDS15. */
    for (uint8_t i = 0; i < col_count; i++) {
        pm_set_gpio_pu_pd_ie(col_pins[i], 1, 0, 1);
    }
}

void bflb_kys_v2_pds_gpio_deconfigure(uint8_t col_count, const uint8_t col_pins[])
{
    LHAL_PARAM_ASSERT(col_count >= 1 && col_count <= BFLB_KYS_V2_MAX_KEYS);
    LHAL_PARAM_ASSERT(col_pins);

    for (uint8_t i = 0; i < col_count; i++) {
        if (col_pins[i] <= 7U) {
            HBN_Aon_Pad_Cfg_Set(0, col_pins[i]);
        } else {
            PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(col_pins[i], 0, 0, 0);
        }
    }
}

void bflb_kys_v2_pds_configure(struct bflb_device_s *dev,
                               const struct bflb_kys_v2_config_s *config)
{
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(config);
    LHAL_PARAM_ASSERT(config->row >= 1 && config->row <= BFLB_KYS_V2_MAX_KEYS);
    LHAL_PARAM_ASSERT(config->col >= 1 && config->col <= BFLB_KYS_V2_MAX_KEYS);
    LHAL_PARAM_ASSERT(config->pds_drive_value <= 1);
    LHAL_PARAM_ASSERT(config->pds_input_pull_en <= 1);

    regval = getreg32(PDS_BASE + PDS_KYD_CTL_OFFSET);
    regval &= ~(PDS_CR_PDS_KYD_EN_MSK |
                PDS_CR_PDS_KYD_COL_O_VAL_MSK |
                PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_MSK |
                PDS_CR_PDS_KYD_COL_MATRIX_SIZE_MSK |
                PDS_CR_PDS_KYD_ROW_I_PULL_EN_MSK);
    regval |= ((uint32_t)config->pds_drive_value << PDS_CR_PDS_KYD_COL_O_VAL_POS) &
              PDS_CR_PDS_KYD_COL_O_VAL_MSK;
    regval |= ((uint32_t)(config->col - 1) << PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_POS) &
              PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_MSK;
    regval |= ((uint32_t)(config->row - 1) << PDS_CR_PDS_KYD_COL_MATRIX_SIZE_POS) &
              PDS_CR_PDS_KYD_COL_MATRIX_SIZE_MSK;
    regval |= ((uint32_t)config->pds_input_pull_en << PDS_CR_PDS_KYD_ROW_I_PULL_EN_POS) &
              PDS_CR_PDS_KYD_ROW_I_PULL_EN_MSK;
    putreg32(regval, PDS_BASE + PDS_KYD_CTL_OFFSET);

    /* Disable all white-key filters so any configured matrix key can wake. */
    putreg32(0, PDS_BASE + PDS_KYD_WHITE_SET_OFFSET);
}

void bflb_kys_v2_pds_enable(struct bflb_device_s *dev, bool enable)
{
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    regval = getreg32(PDS_BASE + PDS_KYD_CTL_OFFSET);
    if (enable) {
        regval |= PDS_CR_PDS_KYD_EN_MSK;
    } else {
        regval &= ~PDS_CR_PDS_KYD_EN_MSK;
    }
    putreg32(regval, PDS_BASE + PDS_KYD_CTL_OFFSET);
}

void bflb_kys_v2_pds_clear_wakeup(struct bflb_device_s *dev)
{
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    regval = getreg32(PDS_BASE + PDS_KYD_CTL_OFFSET);
    putreg32(regval | PDS_CLR_PDS_KYD_WAKEUP_MSK, PDS_BASE + PDS_KYD_CTL_OFFSET);
}

int bflb_kys_v2_pds_get_wakeup_result(struct bflb_device_s *dev,
                                      struct bflb_kys_v2_pds_wakeup_s *result)
{
    uint32_t regval;

    if ((dev == NULL) || (result == NULL)) {
        return -EINVAL;
    }

    regval = getreg32(PDS_BASE + PDS_KYD_CTL_OFFSET);
    result->wakeup = PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_KEYSCAN) == SET;
    /* Translate hardware COL_O/ROW_I coordinates to logical row/column. */
    result->row = (uint8_t)((regval & PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_MSK) >>
                            PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_POS);
    result->col = (uint8_t)((regval & PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_MSK) >>
                            PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_POS);
    result->row_gpio = (uint8_t)((regval & PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_MSK) >>
                                 PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_POS);
    result->col_gpio = (uint8_t)((regval & PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_MSK) >>
                                 PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_POS);

    return 0;
}
