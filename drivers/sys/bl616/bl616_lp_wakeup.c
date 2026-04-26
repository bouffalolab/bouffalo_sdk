#include "bl_lp_internal.h"

static bl_lp_io_cfg_t *gp_lp_io_cfg = NULL;
static bl_lp_io_cfg_t g_lp_io_cfg_bak;
static bl_lp_acomp_cfg_t *gp_lp_acomp_cfg = NULL;
static bl_lp_acomp_cfg_t g_lp_acomp_cfg_bak;

static void bl_lp_set_aon_io(bl_lp_aon_io_cfg_t cfg)
{
    uint8_t pu, pd, ie;

    ie = cfg.ie;
    if (cfg.res_mode == BL_LP_IO_RES_PULL_UP) {
        pu = 1;
        pd = 0;
    } else if (cfg.res_mode == BL_LP_IO_RES_PULL_DOWN) {
        pu = 0;
        pd = 1;
    } else {
        pu = 0;
        pd = 0;
    }

    if (pu | pd | ie) {
        HBN_AON_PAD_CFG_Type aonPadCfg;

        aonPadCfg.ctrlEn = 1;
        aonPadCfg.ie = ie;
        aonPadCfg.oe = 0;
        aonPadCfg.pullUp = pu;
        aonPadCfg.pullDown = pd;
        HBN_Aon_Pad_Cfg(ENABLE, (cfg.pin - 16), &aonPadCfg);
    }

    if (ie & cfg.unmask) {
        uint32_t mask = 0;

        mask = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        mask = BL_GET_REG_BITS_VAL(mask, HBN_PIN_WAKEUP_MASK);
        mask = mask & ~(1 << (cfg.pin - 16));
        HBN_Aon_Pad_WakeUpCfg(DISABLE, cfg.trigMode, mask, 0, 7);
        pm_pds_wakeup_src_en(PDS_WAKEUP_BY_HBN_IRQ_OUT_EN_POS);
    }
}

static void bl_lp_set_gpio_as_analog(uint8_t pin)
{
    GLB_GPIO_Cfg_Type cfg;

    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive = 0;
    cfg.smtCtrl = 0;
    cfg.gpioPin = pin;
    cfg.gpioFun = GPIO_FUN_ANALOG;
    GLB_GPIO_Init(&cfg);
}

uint32_t bl_lp_set_acomp(uint8_t chan, uint8_t pin, uint8_t pos_edge_en, uint8_t neg_edge_en)
{
    struct bflb_acomp_config_s acompCfg = {
        .mux_en = ENABLE,
        .pos_chan_sel = AON_ACOMP_CHAN_ADC0,
        .neg_chan_sel = AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_1,
        .vio_sel = BL616_ACOMP_VREF_1V65,
        .scaling_factor = AON_ACOMP_SCALING_FACTOR_1,
        .bias_prog = AON_ACOMP_BIAS_POWER_MODE1,
        .hysteresis_pos_volt = AON_ACOMP_HYSTERESIS_VOLT_NONE,
        .hysteresis_neg_volt = AON_ACOMP_HYSTERESIS_VOLT_NONE,
    };

    if (pin == GLB_GPIO_PIN_20) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC0;
    } else if (pin == GLB_GPIO_PIN_19) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC1;
    } else if (pin == GLB_GPIO_PIN_2) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC2;
    } else if (pin == GLB_GPIO_PIN_3) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC3;
    } else if (pin == GLB_GPIO_PIN_14) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC4;
    } else if (pin == GLB_GPIO_PIN_13) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC5;
    } else if (pin == GLB_GPIO_PIN_12) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC6;
    } else if (pin == GLB_GPIO_PIN_10) {
        acompCfg.pos_chan_sel = AON_ACOMP_CHAN_ADC7;
    } else {
        return -1;
    }

    bl_lp_set_gpio_as_analog(pin);
    bflb_acomp_init(chan, &acompCfg);
    bflb_acomp_enable(chan);
    HBN_Clear_IRQ(HBN_INT_ACOMP0 + chan * 2);

    if (pos_edge_en) {
        HBN_Enable_AComp_IRQ(chan, HBN_ACOMP_INT_EDGE_POSEDGE);
    } else {
        HBN_Disable_AComp_IRQ(chan, HBN_ACOMP_INT_EDGE_POSEDGE);
    }

    if (neg_edge_en) {
        HBN_Enable_AComp_IRQ(chan, HBN_ACOMP_INT_EDGE_NEGEDGE);
    } else {
        HBN_Disable_AComp_IRQ(chan, HBN_ACOMP_INT_EDGE_NEGEDGE);
    }

    pm_pds_wakeup_src_en(PDS_WAKEUP_BY_HBN_IRQ_OUT_EN_POS);

    return 0;
}

uint32_t bl_lp_set_acomp_pin(uint32_t acomp_chan)
{
    uint32_t tmpPin = 0;

    if (acomp_chan == AON_ACOMP_CHAN_ADC0) {
        tmpPin = GLB_GPIO_PIN_20;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC1) {
        tmpPin = GLB_GPIO_PIN_19;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC2) {
        tmpPin = GLB_GPIO_PIN_2;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC3) {
        tmpPin = GLB_GPIO_PIN_3;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC4) {
        tmpPin = GLB_GPIO_PIN_14;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC5) {
        tmpPin = GLB_GPIO_PIN_13;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC6) {
        tmpPin = GLB_GPIO_PIN_12;
    } else if (acomp_chan == AON_ACOMP_CHAN_ADC7) {
        tmpPin = GLB_GPIO_PIN_10;
    } else {
        return -1;
    }

    bl_lp_set_gpio_as_analog(tmpPin);
    return 0;
}

void bl_lp_io_wakeup_init(bl_lp_io_cfg_t *io_wakeup_cfg)
{
    uint32_t tmpVal;
    uint64_t io_unmask = io_wakeup_cfg->io_wakeup_unmask;
    uint8_t res_mode, pu, pd, ie;
    uint32_t sf_pin_select = 0;
    uint8_t sf0_pin = 0xFF;
    uint8_t sf1_pin = 0xFF;
    uint8_t xtal32k_in_pin = 0xFF;
    uint8_t xtal32k_out_pin = 0xFF;
    GLB_GPIO_Cfg_Type gpio_cfg;

    if (io_wakeup_cfg == NULL) {
        return;
    }

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_SW_USAGE_0);
    sf_pin_select = (tmpVal >> 14) & 0x3f;

    if (sf_pin_select & (1 << 2)) {
        sf0_pin = 4;
    }
    if (sf_pin_select & (1 << 3)) {
        sf1_pin = 10;
    }
    if (BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_GLB), HBN_F32K_SEL)) {
        xtal32k_in_pin = 16;
        xtal32k_out_pin = 17;
    }

    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 0;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioMode = GPIO_MODE_INPUT;
    gpio_cfg.pullType = GPIO_PULL_NONE;
    gpio_cfg.gpioPin = (uint8_t)0;
    gpio_cfg.gpioFun = GPIO_FUN_GPIO;

    for (uint8_t i = 0; i < 35; i++) {
        if ((sf0_pin != 0xFF) && (i >= sf0_pin) && (i < (sf0_pin + 6))) {
            continue;
        }
        if ((sf1_pin != 0xFF) && (i >= sf1_pin) && (i < (sf1_pin + 6))) {
            continue;
        }
        if ((i == xtal32k_in_pin) || (i == xtal32k_out_pin)) {
            continue;
        }

        if ((io_unmask >> i) & 0x1) {
            gpio_cfg.gpioPin = (uint8_t)i;
            GLB_GPIO_Init(&gpio_cfg);
        } else {
            *((volatile uint32_t *)(0x200008C4 + i * 4)) = 0x00400B00;
        }
    }

    ie = io_wakeup_cfg->io_0_15_ie;
    res_mode = io_wakeup_cfg->io_0_15_res;
    if (res_mode == BL_LP_IO_RES_PULL_UP) {
        pu = 1;
        pd = 0;
    } else if (res_mode == BL_LP_IO_RES_PULL_DOWN) {
        pu = 0;
        pd = 1;
    } else {
        pu = 0;
        pd = 0;
    }
    PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(PDS_GPIO_GROUP_SET_GPIO0_GPIO15, pu, pd, ie);

    if ((io_wakeup_cfg->io_0_15_ie)) {
        if (io_unmask & 0xFFFF) {
            if (io_unmask & 0xFF) {
                PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_1_GPIO0_GPIO7);
                PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_INT_SET_1_GPIO0_GPIO7, io_wakeup_cfg->io_0_7_pds_trig_mode);
            }

            if (io_unmask & 0xFF00) {
                PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_2_GPIO8_GPIO15);
                PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_INT_SET_2_GPIO8_GPIO15, io_wakeup_cfg->io_8_15_pds_trig_mode);
            }

            tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_SET);
            tmpVal &= ~0xFFFF;
            tmpVal |= ((~io_unmask) & 0xFFFF);
            BL_WR_REG(PDS_BASE, PDS_GPIO_PD_SET, tmpVal);
            pm_pds_wakeup_src_en(PDS_WAKEUP_BY_PDS_GPIO_IRQ_EN_POS);
        }
    }

    for (uint8_t aon_pin_id = 16; aon_pin_id < 20; aon_pin_id++) {
        bl_lp_aon_io_cfg_t aon_io_cfg;

        aon_io_cfg.pin = aon_pin_id;
        aon_io_cfg.ie = *(((volatile uint8_t *)&io_wakeup_cfg->io_16_ie) + (aon_pin_id - 16));
        aon_io_cfg.res_mode = *(((volatile uint8_t *)&io_wakeup_cfg->io_16_res) + (aon_pin_id - 16));
        aon_io_cfg.unmask = (io_unmask >> aon_pin_id) & 0x01;
        aon_io_cfg.trigMode = io_wakeup_cfg->io_16_19_aon_trig_mode;
        bl_lp_set_aon_io(aon_io_cfg);
    }

    ie = io_wakeup_cfg->io_20_34_ie;
    res_mode = io_wakeup_cfg->io_20_34_res;
    if (res_mode == BL_LP_IO_RES_PULL_UP) {
        pu = 1;
        pd = 0;
    } else if (res_mode == BL_LP_IO_RES_PULL_DOWN) {
        pu = 0;
        pd = 1;
    } else {
        pu = 0;
        pd = 0;
    }
    PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(PDS_GPIO_GROUP_SET_GPIO20_GPIO36, pu, pd, ie);

    if ((io_wakeup_cfg->io_20_34_ie)) {
        if ((io_unmask >> 20) & 0x7FFF) {
            if ((io_unmask >> 20) & 0xFF) {
                PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_3_GPIO20_GPIO27);
                PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_INT_SET_3_GPIO20_GPIO27, io_wakeup_cfg->io_20_27_pds_trig_mode);
            }

            if ((io_unmask >> 20) & 0x7F00) {
                PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_4_GPIO28_GPIO34);
                PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_INT_SET_4_GPIO28_GPIO34, io_wakeup_cfg->io_28_34_pds_trig_mode);
            }

            tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_SET);
            tmpVal &= ~0xFFFF0000;
            tmpVal |= ((((~io_unmask) >> 20) & 0x7FFF) << 16);
            BL_WR_REG(PDS_BASE, PDS_GPIO_PD_SET, tmpVal);
            pm_pds_wakeup_src_en(PDS_WAKEUP_BY_PDS_GPIO_IRQ_EN_POS);
        }
    }
}

void bl_lp_acomp_wakeup_init(bl_lp_acomp_cfg_t *acomp_wakeup_cfg)
{
    if (acomp_wakeup_cfg->acomp0_en && acomp_wakeup_cfg->acomp0_trig_mode) {
        bl_lp_set_acomp(0, acomp_wakeup_cfg->acomp0_io_num, (acomp_wakeup_cfg->acomp0_trig_mode & 0x02),
                        (acomp_wakeup_cfg->acomp0_trig_mode & 0x01));
    } else {
        bflb_acomp_disable(0);
    }

    if (acomp_wakeup_cfg->acomp1_en && acomp_wakeup_cfg->acomp1_trig_mode) {
        bl_lp_set_acomp(1, acomp_wakeup_cfg->acomp1_io_num, (acomp_wakeup_cfg->acomp1_trig_mode & 0x02),
                        (acomp_wakeup_cfg->acomp1_trig_mode & 0x01));
    } else {
        bflb_acomp_disable(1);
    }

    arch_delay_us(15);
    HBN_Clear_IRQ(HBN_INT_ACOMP0);
    HBN_Clear_IRQ(HBN_INT_ACOMP1);
}

void bl616_lp_wakeup_prepare(void)
{
    if (gp_lp_io_cfg) {
        g_lp_io_cfg_bak = *gp_lp_io_cfg;
        BL_LP_LOG("io_wakeup_unmask: 0x%llX\r\n", (unsigned long long)g_lp_io_cfg_bak.io_wakeup_unmask);
        bl_lp_io_wakeup_init(&g_lp_io_cfg_bak);
    }

    if (gp_lp_acomp_cfg) {
        g_lp_acomp_cfg_bak = *gp_lp_acomp_cfg;
        BL_LP_LOG("acomp0: %d, acomp1: %d\r\n", g_lp_acomp_cfg_bak.acomp0_trig_mode,
                  g_lp_acomp_cfg_bak.acomp1_trig_mode);
        bl_lp_acomp_wakeup_init(&g_lp_acomp_cfg_bak);
    }
}

static uint8_t bl_lp_wakeup_check(void)
{
    if ((iot2lp_para->wake_io_bits) || (iot2lp_para->wake_acomp_bits)) {
        return 1;
    } else {
        return 0;
    }
}

__WEAK uint64_t bl_lp_check_gpio_int(void)
{
    uint64_t result = 0;
    return result;
}

__WEAK uint8_t bl_lp_check_acomp_int(void)
{
    uint8_t result = 0;
    return result;
}

uint32_t bl_lp_get_hbn_wakeup_reason(void)
{
    return iot2boot2_para->wakeup_reason;
}

void bl_lp_get_hbn_wakeup_io_bits(uint8_t *io_bits, uint8_t *io_edge_bits)
{
    *io_bits = iot2boot2_para->wkup_io_bits;
    *io_edge_bits = iot2boot2_para->wkup_io_edge_bits;
}

void bl_lp_get_hbn_wakeup_acomp_bits(uint8_t *acomp_bits, uint8_t *acomp_edge_bits)
{
    *acomp_bits = iot2boot2_para->wkup_acomp_bits;
    *acomp_edge_bits = iot2boot2_para->wkup_acomp_edge_bits;
}

int bl_lp_wakeup_io_get_mode(uint8_t io_num)
{
    uint64_t wakeup_io_bits = iot2lp_para->wake_io_bits;
    uint64_t wakeup_io_edge_bits = iot2lp_para->wake_io_edge_bits;
    uint8_t trig_mode;

    if (io_num >= BL_LP_WAKEUP_IO_MAX_NUM) {
        return -1;
    }

    if ((wakeup_io_bits & ((uint64_t)0x0001 << io_num)) == 0) {
        return 0;
    }

    if (io_num >= 16 && io_num <= 19) {
        trig_mode = g_lp_io_cfg_bak.io_16_19_aon_trig_mode;

        if (trig_mode == BL_LP_AON_IO_TRIG_SYNC_RISING_FALLING_EDGE) {
            if ((wakeup_io_edge_bits & ((uint64_t)0x0001 << io_num)) == 0) {
                return BL_LP_IO_WAKEUP_MODE_FALLING;
            } else {
                return BL_LP_IO_WAKEUP_MODE_RISING;
            }
        } else if (trig_mode == BL_LP_AON_IO_TRIG_SYNC_FALLING_EDGE || trig_mode == BL_LP_AON_IO_TRIG_ASYNC_FALLING_EDGE) {
            return BL_LP_IO_WAKEUP_MODE_FALLING;
        } else if (trig_mode == BL_LP_AON_IO_TRIG_SYNC_RISING_EDGE || trig_mode == BL_LP_AON_IO_TRIG_ASYNC_RISING_EDGE) {
            return BL_LP_IO_WAKEUP_MODE_RISING;
        } else if (trig_mode == BL_LP_AON_IO_TRIG_SYNC_LOW_LEVEL || trig_mode == BL_LP_AON_IO_TRIG_ASYNC_LOW_LEVEL) {
            return BL_LP_IO_WAKEUP_MODE_LOW;
        } else if (trig_mode == BL_LP_AON_IO_TRIG_SYNC_HIGH_LEVEL || trig_mode == BL_LP_AON_IO_TRIG_ASYNC_HIGH_LEVEL) {
            return BL_LP_IO_WAKEUP_MODE_HIGH;
        } else {
            return -1;
        }
    }

    if (io_num <= 7) {
        trig_mode = g_lp_io_cfg_bak.io_0_7_pds_trig_mode;
    } else if (io_num >= 8 && io_num <= 15) {
        trig_mode = g_lp_io_cfg_bak.io_8_15_pds_trig_mode;
    } else if (io_num >= 20 && io_num <= 27) {
        trig_mode = g_lp_io_cfg_bak.io_20_27_pds_trig_mode;
    } else if (io_num >= 28 && io_num <= 34) {
        trig_mode = g_lp_io_cfg_bak.io_28_34_pds_trig_mode;
    } else {
        return -1;
    }

    if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_FALLING_EDGE) {
        return BL_LP_IO_WAKEUP_MODE_FALLING;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_HIGH_LEVEL) {
        return BL_LP_IO_WAKEUP_MODE_HIGH;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_RISING_EDGE || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_RISING_EDGE) {
        return BL_LP_IO_WAKEUP_MODE_RISING;
    } else {
        return -1;
    }
}

int bl_lp_io_wakeup_cfg(void *io_wakeup_cfg)
{
    gp_lp_io_cfg = (bl_lp_io_cfg_t *)io_wakeup_cfg;
    return 0;
}

int bl_lp_acomp_wakeup_cfg(bl_lp_acomp_cfg_t *acomp_wakeup_cfg)
{
    gp_lp_acomp_cfg = acomp_wakeup_cfg;
    return 0;
}

int bl_lp_wakeup_acomp_get_mode(uint8_t acomp_num)
{
    uint8_t wakeup_acomp_bits = iot2lp_para->wake_acomp_bits;
    uint8_t wakeup_acomp_edge_bits = iot2lp_para->wake_acomp_edge_bits;

    if (acomp_num == 0) {
        if (g_lp_acomp_cfg_bak.acomp0_en == BL_LP_ACOMP_DISABLE || (wakeup_acomp_bits & 0x01) == 0) {
            return 0;
        }
        if (g_lp_acomp_cfg_bak.acomp0_trig_mode == BL_LP_ACOMP_TRIG_EDGE_FALLING) {
            return BL_LP_ACOMP_WAKEUP_MODE_FALLING;
        } else if (g_lp_acomp_cfg_bak.acomp0_trig_mode == BL_LP_ACOMP_TRIG_EDGE_RISING) {
            return BL_LP_ACOMP_WAKEUP_MODE_RISING;
        } else if (g_lp_acomp_cfg_bak.acomp0_trig_mode == BL_LP_ACOMP_TRIG_EDGE_FALLING_RISING) {
            if (wakeup_acomp_edge_bits & 0x01) {
                return BL_LP_ACOMP_WAKEUP_MODE_RISING;
            } else {
                return BL_LP_ACOMP_WAKEUP_MODE_FALLING;
            }
        } else {
            return -1;
        }
    }

    if (acomp_num == 1) {
        if (g_lp_acomp_cfg_bak.acomp1_en == BL_LP_ACOMP_DISABLE || (wakeup_acomp_bits & 0x02) == 0) {
            return 0;
        }
        if (g_lp_acomp_cfg_bak.acomp1_trig_mode == BL_LP_ACOMP_TRIG_EDGE_FALLING) {
            return BL_LP_ACOMP_WAKEUP_MODE_FALLING;
        } else if (g_lp_acomp_cfg_bak.acomp1_trig_mode == BL_LP_ACOMP_TRIG_EDGE_RISING) {
            return BL_LP_ACOMP_WAKEUP_MODE_RISING;
        } else if (g_lp_acomp_cfg_bak.acomp1_trig_mode == BL_LP_ACOMP_TRIG_EDGE_FALLING_RISING) {
            if (wakeup_acomp_edge_bits & 0x02) {
                return BL_LP_ACOMP_WAKEUP_MODE_RISING;
            } else {
                return BL_LP_ACOMP_WAKEUP_MODE_FALLING;
            }
        } else {
            return -1;
        }
    }

    return -2;
}

int bl_lp_wakeup_check_allow(void)
{
    return bl_lp_wakeup_check();
}
