#include "bflb_wo.h"
#include "bflb_clock.h"
#include "hardware/wo_reg.h"

static uint8_t wo_uart_pin = 0;
static uint16_t wo_uart_pin_high = 0;
static uint16_t wo_uart_pin_low = 0;
static uint16_t wo_uart_buff16[10];

void bflb_wo_pin_init(struct bflb_device_s *dev, uint8_t pin, uint8_t mode)
{
#ifdef romapi_bflb_wo_pin_init
    romapi_bflb_wo_pin_init(dev, pin, mode);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base + GLB_GPIO_CFG0_OFFSET;
    reg_base += pin * 4;
    regval = GLB_REG_GPIO_0_INT_MASK | GLB_REG_GPIO_0_OE | GLB_REG_GPIO_0_DRV_MASK | GLB_REG_GPIO_0_SMT;
    regval |= (1 << GLB_REG_GPIO_0_DRV_SHIFT);
    regval |= (0xB << GLB_REG_GPIO_0_FUNC_SEL_SHIFT);
    if (mode == WO_MODE_WRITE) {
        regval |= (2 << GLB_REG_GPIO_0_MODE_SHIFT);
    } else {
        regval |= (3 << GLB_REG_GPIO_0_MODE_SHIFT);
    }
    putreg32(regval, reg_base);
#endif
}

void bflb_wo_init(struct bflb_device_s *dev, struct bflb_wo_cfg_s *cfg)
{
#ifdef romapi_bflb_wo_init
    romapi_bflb_wo_init(dev, cfg);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    /* disable wo first */
    regval = getreg32(reg_base + GLB_GPIO_CFG142_OFFSET);
    regval &= ~GLB_CR_GPIO_TX_EN;
    putreg32(regval, reg_base + GLB_GPIO_CFG142_OFFSET);

    /* config wo wave count and polarity */
    regval = getreg32(reg_base + GLB_GPIO_CFG142_OFFSET);
    regval &= ~GLB_CR_CODE_TOTAL_TIME_MASK;
    regval &= ~GLB_CR_CODE0_HIGH_TIME_MASK;
    regval &= ~GLB_CR_CODE1_HIGH_TIME_MASK;
    regval |= (cfg->code_total_cnt << GLB_CR_CODE_TOTAL_TIME_SHIFT);
    regval |= (cfg->code0_first_cnt << GLB_CR_CODE0_HIGH_TIME_SHIFT);
    regval |= (cfg->code1_first_cnt << GLB_CR_CODE1_HIGH_TIME_SHIFT);
    if (cfg->code0_first_level) {
        regval &= ~GLB_CR_INVERT_CODE0_HIGH;
    } else {
        regval |= GLB_CR_INVERT_CODE0_HIGH;
    }
    if (cfg->code1_first_level) {
        regval &= ~GLB_CR_INVERT_CODE1_HIGH;
    } else {
        regval |= GLB_CR_INVERT_CODE1_HIGH;
    }
    putreg32(regval, reg_base + GLB_GPIO_CFG142_OFFSET);

    /* config idle level, fifo_threshold, mode  */
    regval = getreg32(reg_base + GLB_GPIO_CFG143_OFFSET);
    regval &= ~GLB_CR_GPIO_TX_FIFO_TH_MASK;
    regval |= (cfg->fifo_threshold << GLB_CR_GPIO_TX_FIFO_TH_SHIFT);
    if (cfg->idle_level) {
        regval |= GLB_CR_GPIO_DMA_PARK_VALUE;
    } else {
        regval &= ~GLB_CR_GPIO_DMA_PARK_VALUE;
    }
    if (cfg->mode == WO_MODE_WRITE) {
        regval &= ~GLB_CR_GPIO_DMA_OUT_SEL_LATCH;
    } else {
        regval |= GLB_CR_GPIO_DMA_OUT_SEL_LATCH;
    }
    /* clear end interrupt */
    regval |= GLB_GPIO_TX_END_CLR;
    regval |= GLB_GPIO_TX_FIFO_CLR;
    /* clear fifo */
    putreg32(regval, reg_base + GLB_GPIO_CFG143_OFFSET);

    /* enable wo */
    regval = getreg32(reg_base + GLB_GPIO_CFG142_OFFSET);
    regval |= GLB_CR_GPIO_TX_EN;
    putreg32(regval, reg_base + GLB_GPIO_CFG142_OFFSET);
#endif
}

void bflb_wo_enable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wo_enable
    romapi_bflb_wo_enable(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + GLB_GPIO_CFG142_OFFSET);
    regval |= GLB_CR_GPIO_TX_EN;
    putreg32(regval, reg_base + GLB_GPIO_CFG142_OFFSET);
#endif
}

void bflb_wo_disable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wo_disable
    romapi_bflb_wo_disable(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + GLB_GPIO_CFG142_OFFSET);
    regval &= ~GLB_CR_GPIO_TX_EN;
    putreg32(regval, reg_base + GLB_GPIO_CFG142_OFFSET);
#endif
}

uint32_t bflb_wo_get_fifo_available_cnt(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wo_get_fifo_available_cnt
    return romapi_bflb_wo_get_fifo_available_cnt(dev);
#else
    uint32_t regval;

    regval = getreg32(dev->reg_base + GLB_GPIO_CFG143_OFFSET);
    regval &= GLB_GPIO_TX_FIFO_CNT_MASK;
    regval >>= GLB_GPIO_TX_FIFO_CNT_SHIFT;
    return regval;
#endif
}

uint32_t bflb_wo_push_fifo(struct bflb_device_s *dev, uint16_t *data, uint32_t len)
{
#ifdef romapi_bflb_wo_push_fifo
    return romapi_bflb_wo_push_fifo(dev, data, len);
#else
    uint32_t idx = 0;
    uint32_t fclk = bflb_clk_get_system_clock(BFLB_SYSTEM_CPU_CLK);
    uint32_t xclk = bflb_clk_get_system_clock(BFLB_SYSTEM_XCLK);
    uint32_t to_max = fclk / xclk * 512;
    uint32_t to = to_max;

    while (idx < len) {
        if (bflb_wo_get_fifo_available_cnt(dev) > 0) {
            putreg32((uint32_t)data[idx++], dev->reg_base + GLB_GPIO_CFG144_OFFSET);
            to = to_max;
        } else {
            to--;
            if (to == 0) {
                return idx;
            }
        }
    }
    return idx;
#endif
}

void bflb_wo_clear_fifo(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wo_clear_fifo
    romapi_bflb_wo_clear_fifo(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + GLB_GPIO_CFG143_OFFSET);
    regval |= GLB_GPIO_TX_FIFO_CLR;
    putreg32(regval, reg_base + GLB_GPIO_CFG143_OFFSET);
#endif
}

void bflb_wo_enable_dma(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wo_enable_dma
    romapi_bflb_wo_enable_dma(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + GLB_GPIO_CFG143_OFFSET);
    regval |= GLB_CR_GPIO_DMA_TX_EN;
    putreg32(regval, reg_base + GLB_GPIO_CFG143_OFFSET);
#endif
}

void bflb_wo_disable_dma(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wo_disable_dma
    romapi_bflb_wo_disable_dma(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + GLB_GPIO_CFG143_OFFSET);
    regval &= ~GLB_CR_GPIO_DMA_TX_EN;
    putreg32(regval, reg_base + GLB_GPIO_CFG143_OFFSET);
#endif
}

uint32_t bflb_wo_get_int_status(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wo_get_int_status
    return romapi_bflb_wo_get_int_status(dev);
#else
    uint32_t regval;
    uint32_t sts = 0;

    regval = getreg32(dev->reg_base + GLB_GPIO_CFG143_OFFSET);
    if (regval & GLB_R_GPIO_TX_END_INT) {
        sts |= WO_INT_END;
    }
    if (regval & GLB_R_GPIO_TX_FIFO_INT) {
        sts |= WO_INT_FIFO;
    }
    if (regval & GLB_R_GPIO_TX_FER_INT) {
        sts |= WO_INT_FER;
    }
    return sts;
#endif
}

void bflb_wo_int_mask(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_wo_int_mask
    romapi_bflb_wo_int_mask(dev, int_type);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + GLB_GPIO_CFG143_OFFSET);
    if (int_type & WO_INT_END) {
        regval |= GLB_CR_GPIO_TX_END_MASK;
    }
    if (int_type & WO_INT_FIFO) {
        regval |= GLB_CR_GPIO_TX_FIFO_MASK;
    }
    if (int_type & WO_INT_FER) {
        regval |= GLB_CR_GPIO_TX_FER_MASK;
    }
    putreg32(regval, reg_base + GLB_GPIO_CFG143_OFFSET);
#endif
}

void bflb_wo_int_unmask(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_wo_int_unmask
    romapi_bflb_wo_int_unmask(dev, int_type);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + GLB_GPIO_CFG143_OFFSET);
    if (int_type & WO_INT_END) {
        regval &= ~GLB_CR_GPIO_TX_END_MASK;
    }
    if (int_type & WO_INT_FIFO) {
        regval &= ~GLB_CR_GPIO_TX_FIFO_MASK;
    }
    if (int_type & WO_INT_FER) {
        regval &= ~GLB_CR_GPIO_TX_FER_MASK;
    }
    putreg32(regval, reg_base + GLB_GPIO_CFG143_OFFSET);
#endif
}

void bflb_wo_int_clear(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_wo_int_clear
    romapi_bflb_wo_int_clear(dev, int_type);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + GLB_GPIO_CFG143_OFFSET);
    if (int_type & WO_INT_END) {
        regval |= GLB_GPIO_TX_END_CLR;
    }
    if (int_type & WO_INT_FIFO) {
    }
    if (int_type & WO_INT_FER) {
        regval |= GLB_GPIO_TX_FIFO_CLR;
    }
    putreg32(regval, reg_base + GLB_GPIO_CFG143_OFFSET);
#endif
}

void bflb_wo_uart_init(struct bflb_device_s *dev, uint32_t baudrate, uint8_t pin)
{
    struct bflb_wo_cfg_s cfg;

    if (baudrate == 0) {
        baudrate = 2 * 1000 * 1000;
    }
    baudrate = bflb_clk_get_system_clock(BFLB_SYSTEM_XCLK) / baudrate;
    cfg.code_total_cnt = baudrate,
    cfg.code0_first_cnt = 0,
    cfg.code1_first_cnt = baudrate;
    cfg.code0_first_level = 1;
    cfg.code1_first_level = 1;
    cfg.idle_level = 1;
    cfg.fifo_threshold = 64;
    cfg.mode = WO_MODE_SET_CLR;
    bflb_wo_pin_init(dev, pin, WO_MODE_SET_CLR);
    bflb_wo_init(dev, &cfg);
    wo_uart_pin = pin;
    wo_uart_pin_high = (1 << (wo_uart_pin & 7));
    wo_uart_pin_low = (wo_uart_pin_high << 8);
    wo_uart_buff16[0] = wo_uart_pin_low;
    wo_uart_buff16[9] = wo_uart_pin_high;
}

void bflb_wo_uart_putchar(struct bflb_device_s *dev, uint8_t ch)
{
    for (uint8_t i = 0; i < 8; i++) {
        if ((ch >> i) & 1) {
            wo_uart_buff16[i + 1] = wo_uart_pin_high;
        } else {
            wo_uart_buff16[i + 1] = wo_uart_pin_low;
        }
    }
    bflb_wo_push_fifo(dev, wo_uart_buff16, 10);
}

void bflb_wo_uart_put(struct bflb_device_s *dev, uint8_t *data, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        bflb_wo_uart_putchar(dev, data[i]);
    }
}
