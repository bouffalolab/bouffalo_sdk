#include "bflb_gpio.h"
#include "hardware/gpio_reg.h"

struct bflb_gpio_irq_callback {
    void (*handler)(uint8_t pin);
};

#if defined(BL702) || defined(BL602) || defined(BL702L)
struct bflb_gpio_irq_callback g_gpio_irq_callback[32] = { 0 };
#elif defined(BL616)
struct bflb_gpio_irq_callback g_gpio_irq_callback[35] = { 0 };
#elif defined(BL616L)
struct bflb_gpio_irq_callback g_gpio_irq_callback[37] = { 0 };
#elif defined(BL606P) || defined(BL808)
struct bflb_gpio_irq_callback g_gpio_irq_callback[46] = { 0 };
#elif defined(BL628)
struct bflb_gpio_irq_callback g_gpio_irq_callback[35] = { 0 };
#elif defined(BL616D)
struct bflb_gpio_irq_callback g_gpio_irq_callback[53] = { 0 };
#endif

#if defined(BL616)
extern void bflb_gpio_pad_check(uint8_t pin);
#endif

void bflb_gpio_init(struct bflb_device_s *dev, uint8_t pin, uint32_t cfgset)
{
#ifdef romapi_bflb_gpio_init
    romapi_bflb_gpio_init(dev, pin, cfgset);
#else
    uint32_t cfg = 0;
    uint32_t reg_base;
    uint32_t cfg_address;
    uint8_t drive;
    uint8_t function;
    uint16_t mode;

    reg_base = dev->reg_base;

    function = (cfgset & GPIO_FUNC_MASK) >> GPIO_FUNC_SHIFT;
    mode = (cfgset & GPIO_MODE_MASK);
    drive = (cfgset & GPIO_DRV_MASK) >> GPIO_DRV_SHIFT;

#if defined(BL616)
    bflb_gpio_pad_check(pin);

    /* disable muxed to be xtal32k */
    if (pin == GPIO_PIN_16) {
        *(volatile uint32_t *)(0x2000f000 + 0x38) &= ~(1 << 20);
    } else if (pin == GPIO_PIN_17) {
        *(volatile uint32_t *)(0x2000f000 + 0x38) &= ~(1 << 21);
    }
#endif

#if defined(BL702L)
    /* disable muxed to be xtal32k */
    if (pin == GPIO_PIN_30) {
        *(volatile uint32_t *)(0x4000F000 + 0x38) &= ~(1 << 25);
    } else if (pin == GPIO_PIN_31) {
        *(volatile uint32_t *)(0x4000F000 + 0x38) &= ~(1 << 26);
    }
#endif

#if defined(BL702) || defined(BL602) || defined(BL702L)
    uint32_t regval;
    uint8_t real_pin;
    uint8_t is_odd = 0;

    /* Disable output anyway */
    regval = getreg32(reg_base + GLB_GPIO_CFGCTL34_OFFSET + ((pin >> 5) << 2));
    regval &= ~(1 << (pin & 0x1f));
    putreg32(regval, reg_base + GLB_GPIO_CFGCTL34_OFFSET + ((pin >> 5) << 2));

    real_pin = pin;

#if defined(BL702)
#define GLB_BASE 0x40000000
#define GLB_GPIO_USE_PSRAM__IO_OFFSET 0x88
    /* SF pad use exclusive IE/PD/PU/DRIVE/SMTCTRL */
    if (pin >= 23 && pin <= 28) {
        if (getreg32(GLB_BASE + GLB_GPIO_USE_PSRAM__IO_OFFSET) & (1 << (pin - 23))) {
            real_pin += 9;
        }
    }
#endif

    is_odd = real_pin & 1;

    cfg_address = reg_base + GLB_GPIO_CFGCTL0_OFFSET + (real_pin / 2 * 4);
    cfg = getreg32(cfg_address);
    cfg &= ~(0xffff << (16 * is_odd));

    regval = getreg32(reg_base + GLB_GPIO_CFGCTL34_OFFSET + ((pin >> 5) << 2));

    if (mode & GPIO_ANALOG) {
        regval &= ~(1 << (pin & 0x1f));
        function = 10;
    } else if (mode & GPIO_ALTERNATE) {
        cfg |= (1 << (is_odd * 16 + 0));
        regval &= ~(1 << (pin & 0x1f));
#if defined(BL702L)
        if (function == 22) {
            cfg &= ~(1 << (is_odd * 16 + 0));
        }
#endif
    } else {
        function = 11;

        if (mode & GPIO_INPUT) {
            cfg |= (1 << (is_odd * 16 + 0));
        }

        if (mode & GPIO_OUTPUT) {
            regval |= (1 << (pin & 0x1f));
        }
    }

    putreg32(regval, reg_base + GLB_GPIO_CFGCTL34_OFFSET + ((pin >> 5) << 2));

    if (cfgset & GPIO_PULLUP) {
        cfg |= (1 << (is_odd * 16 + 4));
    } else if (cfgset & GPIO_PULLDOWN) {
        cfg |= (1 << (is_odd * 16 + 5));
    } else {
    }

    if (cfgset & GPIO_SMT_EN) {
        cfg |= (1 << (is_odd * 16 + 1));
    }

    cfg |= (drive << (is_odd * 16 + 2));
    cfg |= (function << (is_odd * 16 + 8));
#if defined(BL702L)
    /* configure output mode:set and clr mode */
    if ((function != 22) && (function != 21)) {
        cfg |= (1 << (is_odd * 16 + 15));
    }
#endif

#if defined(BL702)
#define HBN_BASE ((uint32_t)0x4000F000)
#define HBN_IRQ_MODE_OFFSET (0x14)
#define HBN_REG_AON_PAD_IE_SMT_SHIFT (8U)
#define HBN_REG_AON_PAD_IE_SMT_MASK (0x1f << HBN_REG_AON_PAD_IE_SMT_SHIFT)
    /* always on pads IE control (in HBN) */
    if (pin >= 9 && pin <= 13) {
        regval = getreg32(HBN_BASE + HBN_IRQ_MODE_OFFSET);
        uint32_t aon_pad_ie_smt = (regval & HBN_REG_AON_PAD_IE_SMT_MASK) >> HBN_REG_AON_PAD_IE_SMT_SHIFT;

        if (mode & GPIO_ANALOG) {
            /* clear AON pad IE */
            aon_pad_ie_smt &= ~(1 << (pin - 9));
        } else {
            /* not analog mode */
            if (mode & GPIO_INPUT) {
                aon_pad_ie_smt |= (1 << (pin - 9));
            }
        }

        regval &= ~HBN_REG_AON_PAD_IE_SMT_MASK;
        regval |= (aon_pad_ie_smt << HBN_REG_AON_PAD_IE_SMT_SHIFT);
        putreg32(regval, HBN_BASE + HBN_IRQ_MODE_OFFSET);
    }

    /* Pins 23...28: SF pad use exclusive IE/PD/PU/DRIVE/SMTCTRL */
    if (pin != real_pin) {
        is_odd = pin & 1;

        regval = getreg32(reg_base + GLB_GPIO_CFGCTL0_OFFSET + (pin / 2 * 4));
        regval &= ~(0x1f << (is_odd * 16 + 8));
        regval |= (function << (is_odd * 16 + 8));
        putreg32(regval, reg_base + GLB_GPIO_CFGCTL0_OFFSET + (pin / 2 * 4));
    }
#endif
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
    cfg_address = reg_base + GLB_GPIO_CFG0_OFFSET + (pin << 2);
    cfg = 0;
    cfg |= GLB_REG_GPIO_0_INT_MASK;

    if (mode & GPIO_ANALOG) {
        function = 10;
    } else if (mode & GPIO_ALTERNATE) {
        cfg |= GLB_REG_GPIO_0_IE;
    } else {
        function = 11;

        if (mode & GPIO_INPUT) {
            cfg |= GLB_REG_GPIO_0_IE;
        }

        if (mode & GPIO_OUTPUT) {
            cfg |= GLB_REG_GPIO_0_OE;
        }
    }

    if (cfgset & GPIO_PULLUP) {
        cfg |= GLB_REG_GPIO_0_PU;
    } else if (cfgset & GPIO_PULLDOWN) {
        cfg |= GLB_REG_GPIO_0_PD;
    } else {
    }

    if (cfgset & GPIO_SMT_EN) {
        cfg |= GLB_REG_GPIO_0_SMT;
    }

    cfg |= (drive << GLB_REG_GPIO_0_DRV_SHIFT);
    cfg |= (function << GLB_REG_GPIO_0_FUNC_SEL_SHIFT);

    /* configure output mode:set and clr mode */
    cfg |= 0x1 << GLB_REG_GPIO_0_MODE_SHIFT;
#endif
    putreg32(cfg, cfg_address);
#endif
}

void bflb_gpio_deinit(struct bflb_device_s *dev, uint8_t pin)
{
#ifdef romapi_bflb_gpio_deinit
    romapi_bflb_gpio_deinit(dev, pin);
#else
#if defined(BL616)
    bflb_gpio_pad_check(pin);
#endif
    bflb_gpio_init(dev, pin, GPIO_INPUT | GPIO_FLOAT);
#endif
}

#if defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
void bflb_gpio_output_value_mode_enable(struct bflb_device_s *dev, uint8_t pin)
{
#ifdef romapi_bflb_gpio_output_value_mode_enable
    romapi_bflb_gpio_output_value_mode_enable(dev, pin);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + GLB_GPIO_CFG0_OFFSET + (pin << 2));
    /* configure output mode: output value mode */
    regval &= ~GLB_REG_GPIO_0_MODE_MASK;
    regval &= ~GLB_REG_GPIO_0_O;
    putreg32(regval, reg_base + GLB_GPIO_CFG0_OFFSET + (pin << 2));
#endif
}
#endif

void bflb_gpio_pin0_31_output(struct bflb_device_s *dev, uint32_t value)
{
#ifdef romapi_bflb_gpio_pin0_31_output
    romapi_bflb_gpio_pin0_31_output(dev, value);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    putreg32(value, dev->reg_base + GLB_GPIO_CFGCTL32_OFFSET);
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
#if defined(BL616)
    uint32_t pin;
    for (pin = 0; pin < 32; pin++) {
        if (value & (1 << pin)) {
            bflb_gpio_pad_check(pin);
        }
    }
#endif
    putreg32(value, dev->reg_base + GLB_GPIO_CFG136_OFFSET);
#endif
#endif
}

#if defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
void bflb_gpio_pin32_63_output(struct bflb_device_s *dev, uint32_t value)
{
#ifdef romapi_bflb_gpio_pin32_63_output
    romapi_bflb_gpio_pin32_63_output(dev, value);
#else
#if defined(BL616)
    uint32_t pin;
    for (pin = 0; pin < 32; pin++) {
        if (value & (1 << pin)) {
            bflb_gpio_pad_check(pin);
        }
    }
#endif
    putreg32(value, dev->reg_base + GLB_GPIO_CFG137_OFFSET);
#endif
}
#endif

void bflb_gpio_set(struct bflb_device_s *dev, uint8_t pin)
{
#ifdef romapi_bflb_gpio_set
    romapi_bflb_gpio_set(dev, pin);
#else
#if defined(BL702) || defined(BL602)
    uint32_t regval = getreg32(dev->reg_base + GLB_GPIO_CFGCTL32_OFFSET);
    putreg32(regval | 1 << (pin & 0x1f), dev->reg_base + GLB_GPIO_CFGCTL32_OFFSET);
#elif defined(BL702L)
    putreg32(1 << (pin & 0x1f), dev->reg_base + GLB_GPIO_CFGCTL35_OFFSET);
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
#if defined(BL616)
    bflb_gpio_pad_check(pin);
#endif
    putreg32(1 << (pin & 0x1f), dev->reg_base + GLB_GPIO_CFG138_OFFSET + ((pin >> 5) << 2));
#endif
#endif
}

void bflb_gpio_pin0_31_set(struct bflb_device_s *dev, uint32_t value)
{
#ifdef romapi_bflb_gpio_pin0_31_set
    romapi_bflb_gpio_pin0_31_set(dev, value);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    uint32_t regval = getreg32(dev->reg_base + GLB_GPIO_CFGCTL32_OFFSET);
    putreg32(regval |= (value), dev->reg_base + GLB_GPIO_CFGCTL32_OFFSET);
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
#if defined(BL616)
    uint32_t pin;
    for (pin = 0; pin < 32; pin++) {
        if (value & (1 << pin)) {
            bflb_gpio_pad_check(pin);
        }
    }
#endif
    putreg32(value, dev->reg_base + GLB_GPIO_CFG138_OFFSET);
#endif
#endif
}

#if defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
void bflb_gpio_pin32_63_set(struct bflb_device_s *dev, uint32_t value)
{
#ifdef romapi_bflb_gpio_pin32_63_set
    romapi_bflb_gpio_pin32_63_set(dev, value);
#else
#if defined(BL616)
    uint32_t pin;
    for (pin = 0; pin < 32; pin++) {
        if (value & (1 << pin)) {
            bflb_gpio_pad_check(pin + 32);
        }
    }
#endif
    putreg32(value, dev->reg_base + GLB_GPIO_CFG139_OFFSET);
#endif
}
#endif

void bflb_gpio_reset(struct bflb_device_s *dev, uint8_t pin)
{
#ifdef romapi_bflb_gpio_reset
    romapi_bflb_gpio_reset(dev, pin);
#else
#if defined(BL702) || defined(BL602)
    uint32_t regval = getreg32(dev->reg_base + GLB_GPIO_CFGCTL32_OFFSET);
    putreg32(regval & ~(1 << (pin & 0x1f)), dev->reg_base + GLB_GPIO_CFGCTL32_OFFSET);
#elif defined(BL702L)
    putreg32(1 << (pin & 0x1f), dev->reg_base + GLB_GPIO_CFGCTL36_OFFSET);
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
#if defined(BL616)
    bflb_gpio_pad_check(pin);
#endif
    putreg32(1 << (pin & 0x1f), dev->reg_base + GLB_GPIO_CFG140_OFFSET + ((pin >> 5) << 2));
#endif
#endif
}

void bflb_gpio_pin0_31_reset(struct bflb_device_s *dev, uint32_t value)
{
#ifdef romapi_bflb_gpio_pin0_31_reset
    romapi_bflb_gpio_pin0_31_reset(dev, value);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    uint32_t regval = getreg32(dev->reg_base + GLB_GPIO_CFGCTL32_OFFSET);
    putreg32(regval & ~(value), dev->reg_base + GLB_GPIO_CFGCTL32_OFFSET);
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
#if defined(BL616)
    uint32_t pin;
    for (pin = 0; pin < 32; pin++) {
        if (value & (1 << pin)) {
            bflb_gpio_pad_check(pin);
        }
    }
#endif
    putreg32(value, dev->reg_base + GLB_GPIO_CFG140_OFFSET);
#endif
#endif
}

#if defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
void bflb_gpio_pin32_63_reset(struct bflb_device_s *dev, uint32_t value)
{
#ifdef romapi_bflb_gpio_pin32_63_reset
    romapi_bflb_gpio_pin32_63_reset(dev, value);
#else
#if defined(BL616)
    uint32_t pin;
    for (pin = 0; pin < 32; pin++) {
        if (value & (1 << pin)) {
            bflb_gpio_pad_check(pin + 32);
        }
    }
#endif
    putreg32(value, dev->reg_base + GLB_GPIO_CFG141_OFFSET);
#endif
}
#endif

bool bflb_gpio_read(struct bflb_device_s *dev, uint8_t pin)
{
#ifdef romapi_bflb_gpio_read
    return romapi_bflb_gpio_read(dev, pin);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    return (getreg32(dev->reg_base + GLB_GPIO_CFGCTL30_OFFSET) & (1 << pin));
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
#if defined(BL616)
    bflb_gpio_pad_check(pin);
#endif
    return (getreg32(dev->reg_base + GLB_GPIO_CFG0_OFFSET + (pin << 2)) & GLB_REG_GPIO_0_I);
#endif
#endif
}

uint32_t bflb_gpio_pin0_31_read(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_gpio_pin0_31_read
    return romapi_bflb_gpio_pin0_31_read(dev);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    return (getreg32(dev->reg_base + GLB_GPIO_CFGCTL30_OFFSET));
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
    return (getreg32(dev->reg_base + GLB_GPIO_CFG128_OFFSET));
#endif
#endif
}

#if defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
uint32_t bflb_gpio_pin32_63_read(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_gpio_pin32_63_read
    return romapi_bflb_gpio_pin32_63_read(dev);
#else
    return (getreg32(dev->reg_base + GLB_GPIO_CFG129_OFFSET));
#endif
}
#endif

void bflb_gpio_int_init(struct bflb_device_s *dev, uint8_t pin, uint8_t trig_mode)
{
#ifdef romapi_bflb_gpio_int_init
    romapi_bflb_gpio_int_init(dev, pin, trig_mode);
#else
    uint32_t reg_base;
    uint32_t cfg_address;
    uint32_t regval;

    reg_base = dev->reg_base;

    bflb_gpio_int_mask(dev, pin, true);
    bflb_gpio_int_clear(dev, pin);

#if defined(BL702) || defined(BL602)
    cfg_address = reg_base + GLB_GPIO_INT_MODE_SET1_OFFSET + ((pin / 10) << 2);
    regval = getreg32(cfg_address);
    regval &= ~(0x07 << ((pin % 10) * 3));
    regval |= (trig_mode << ((pin % 10) * 3));
#elif defined(BL702L)
    cfg_address = reg_base + GLB_GPIO_INT_MODE_SET1_OFFSET + ((pin / 8) << 2);
    regval = getreg32(cfg_address);
    regval &= ~(0x0f << ((pin % 8) * 4));
    regval |= (trig_mode << ((pin % 8) * 4));
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
    cfg_address = reg_base + GLB_GPIO_CFG0_OFFSET + (pin << 2);
    regval = getreg32(cfg_address);
    regval &= ~GLB_REG_GPIO_0_INT_MODE_SET_MASK;
    regval |= (trig_mode << GLB_REG_GPIO_0_INT_MODE_SET_SHIFT);
#endif
    putreg32(regval, cfg_address);
#endif
}

void bflb_gpio_int_mask(struct bflb_device_s *dev, uint8_t pin, bool mask)
{
#ifdef romapi_bflb_gpio_int_mask
    romapi_bflb_gpio_int_mask(dev, pin, mask);
#else
    uint32_t reg_base;
    uint32_t cfg_address;
    uint32_t regval;

    reg_base = dev->reg_base;
#if defined(BL702) || defined(BL602) || defined(BL702L)
    cfg_address = reg_base + GLB_GPIO_INT_MASK1_OFFSET;

    regval = getreg32(cfg_address);
    if (mask) {
        regval |= (1 << pin);
    } else {
        regval &= ~(1 << pin);
    }
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
    cfg_address = reg_base + GLB_GPIO_CFG0_OFFSET + (pin << 2);

    regval = getreg32(cfg_address);
    if (mask) {
        regval |= GLB_REG_GPIO_0_INT_MASK;
    } else {
        regval &= ~GLB_REG_GPIO_0_INT_MASK;
    }
#endif
    putreg32(regval, cfg_address);
#endif
}

bool bflb_gpio_get_intstatus(struct bflb_device_s *dev, uint8_t pin)
{
#ifdef romapi_bflb_gpio_get_intstatus
    return romapi_bflb_gpio_get_intstatus(dev, pin);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    return (getreg32(dev->reg_base + GLB_GPIO_INT_STAT1_OFFSET) & (1 << pin));
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
    return (getreg32(dev->reg_base + GLB_GPIO_CFG0_OFFSET + (pin << 2)) & GLB_GPIO_0_INT_STAT);
#endif
#endif
}

uint32_t bflb_gpio_get_intstatus_pin0_31(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_gpio_get_intstatus_pin0_31
    return romapi_bflb_gpio_get_intstatus_pin0_31(dev);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    return (getreg32(dev->reg_base + GLB_GPIO_INT_STAT1_OFFSET));
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
    uint32_t status = 0;
    uint32_t index = 0;
    int pin_count = 0;
    if (GPIO_PIN_MAX > 32) {
        pin_count = 32;
    } else {
        pin_count = GPIO_PIN_MAX;
    }
    for (index = 0; index < pin_count; index++) {
        if ((getreg32(dev->reg_base + GLB_GPIO_CFG0_OFFSET + (index << 2)) & GLB_GPIO_0_INT_STAT)) {
            status |= (1 << index);
        }
    }
    return status;
#endif
#endif
}

#if defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
uint32_t bflb_gpio_get_intstatus_pin32_63(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_gpio_get_intstatus_pin32_63
    return romapi_bflb_gpio_get_intstatus_pin32_63(dev);
#else
    uint32_t status = 0;
    uint32_t index = 0;
    int pin_count = 0;
    if (GPIO_PIN_MAX > 64) {
        pin_count = 32;
    } else {
        pin_count = GPIO_PIN_MAX - 32;
    }
    for (index = 0; index < pin_count; index++) {
        if ((getreg32(dev->reg_base + GLB_GPIO_CFG0_OFFSET + ((index + 32) << 2)) & GLB_GPIO_0_INT_STAT)) {
            status |= (1 << index);
        }
    }
    return status;
#endif
}
#endif

void bflb_gpio_int_clear(struct bflb_device_s *dev, uint8_t pin)
{
#ifdef romapi_bflb_gpio_int_clear
    romapi_bflb_gpio_int_clear(dev, pin);
#else
    uint32_t reg_base;
    uint32_t cfg_address;
    uint32_t regval;

    reg_base = dev->reg_base;
#if defined(BL702) || defined(BL602) || defined(BL702L)
    cfg_address = reg_base + GLB_GPIO_INT_CLR1_OFFSET;

    regval = getreg32(cfg_address);
    regval |= (1 << pin);
    putreg32(regval, cfg_address);
    regval &= ~(1 << pin);
    putreg32(regval, cfg_address);
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
    cfg_address = reg_base + GLB_GPIO_CFG0_OFFSET + (pin << 2);

    regval = getreg32(cfg_address);
    regval |= GLB_REG_GPIO_0_INT_CLR;
    putreg32(regval, cfg_address);
    regval &= ~GLB_REG_GPIO_0_INT_CLR;
    putreg32(regval, cfg_address);
#endif
#endif
}

void bflb_gpio_int_clear_pin0_31(struct bflb_device_s *dev, uint32_t pins)
{
#ifdef romapi_bflb_gpio_int_clear_pin0_31
    romapi_bflb_gpio_int_clear_pin0_31(dev, pins);
#else
    uint32_t reg_base;
    uint32_t cfg_address;
    uint32_t regval;

    reg_base = dev->reg_base;
#if defined(BL702) || defined(BL602) || defined(BL702L)
    cfg_address = reg_base + GLB_GPIO_INT_CLR1_OFFSET;

    regval = getreg32(cfg_address);
    regval |= pins;
    putreg32(regval, cfg_address);
    regval &= ~(pins);
    putreg32(regval, cfg_address);
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
    for (int pin = 0; pin < 32; pin++) {
        if (pins & (1 << pin)) {
            cfg_address = reg_base + GLB_GPIO_CFG0_OFFSET + (pin << 2);
            regval = getreg32(cfg_address);
            regval |= GLB_REG_GPIO_0_INT_CLR;
            putreg32(regval, cfg_address);
            regval &= ~GLB_REG_GPIO_0_INT_CLR;
            putreg32(regval, cfg_address);
        }
    }
#endif
#endif
}

#if defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
void bflb_gpio_int_clear_pin32_63(struct bflb_device_s *dev, uint32_t pins)
{
#ifdef romapi_bflb_gpio_int_clear_pin32_63
    romapi_bflb_gpio_int_clear_pin32_63(dev, pins);
#else

    uint32_t reg_base;
    uint32_t cfg_address;
    uint32_t regval;

    reg_base = dev->reg_base;

    for (int pin = 0; pin < 32; pin++) {
        if (pins & (1 << pin)) {
            cfg_address = reg_base + GLB_GPIO_CFG0_OFFSET + ((pin + 32) << 2);
            regval = getreg32(cfg_address);
            regval |= GLB_REG_GPIO_0_INT_CLR;
            putreg32(regval, cfg_address);
            regval &= ~GLB_REG_GPIO_0_INT_CLR;
            putreg32(regval, cfg_address);
        }
    }
#endif
}
#endif

void bflb_gpio_uart_init(struct bflb_device_s *dev, uint8_t pin, uint8_t uart_func)
{
#ifdef romapi_bflb_gpio_uart_init
    romapi_bflb_gpio_uart_init(dev, pin, uart_func);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint8_t sig;
    uint8_t sig_pos;

    reg_base = dev->reg_base;
#if defined(BL616)
    bflb_gpio_pad_check(pin);
#endif
#if defined(BL702) || defined(BL602)
#define GLB_UART_SIG_SEL_0_OFFSET (0xC0)
    regval = getreg32(reg_base + GLB_UART_SIG_SEL_0_OFFSET);

    sig = pin % 8;
    sig_pos = sig << 2;

    regval &= (~(0x0f << sig_pos));
    regval |= (uart_func << sig_pos);

    for (uint8_t i = 0; i < 8; i++) {
        /* reset other sigs which are the same with uart_func */
        sig_pos = i << 2;
        if (((regval & (0x0f << sig_pos)) == (uart_func << sig_pos)) && (i != sig) && (uart_func != 0x0f)) {
            regval &= (~(0x0f << sig_pos));
            regval |= (0x0f << sig_pos);
        }
    }

    putreg32(regval, reg_base + GLB_UART_SIG_SEL_0_OFFSET);
#elif defined(BL702L)
#define GLB_UART_SIG_SEL_0_OFFSET (0xC0)
    regval = getreg32(reg_base + GLB_UART_SIG_SEL_0_OFFSET);

    sig = pin % 4;
    sig_pos = sig << 2;

    regval &= (~(0x0f << sig_pos));
    regval |= (uart_func << sig_pos);

    for (uint8_t i = 0; i < 4; i++) {
        /* reset other sigs which are the same with uart_func */
        sig_pos = i << 2;
        if (((regval & (0x0f << sig_pos)) == (uart_func << sig_pos)) && (i != sig) && (uart_func != 0x0f)) {
            regval &= (~(0x0f << sig_pos));
            regval |= (0x0f << sig_pos);
        }
    }

    putreg32(regval, reg_base + GLB_UART_SIG_SEL_0_OFFSET);
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
#define GLB_UART_CFG1_OFFSET (0x154)
#define GLB_UART_CFG2_OFFSET (0x158)
    uint32_t regval2;
    sig = pin % 12;

    if (sig < 8) {
        sig_pos = sig << 2;

        regval = getreg32(reg_base + GLB_UART_CFG1_OFFSET);
        regval &= (~(0x0f << sig_pos));
        regval |= (uart_func << sig_pos);

        for (uint8_t i = 0; i < 8; i++) {
            /* reset other sigs which are the same with uart_func */
            sig_pos = i << 2;
            if (((regval & (0x0f << sig_pos)) == (uart_func << sig_pos)) && (i != sig) && (uart_func != 0x0f)) {
                regval &= (~(0x0f << sig_pos));
                regval |= (0x0f << sig_pos);
            }
        }
        regval2 = getreg32(reg_base + GLB_UART_CFG2_OFFSET);

        for (uint8_t i = 8; i < 12; i++) {
            /* reset other sigs which are the same with uart_func */
            sig_pos = (i - 8) << 2;
            if (((regval2 & (0x0f << sig_pos)) == (uart_func << sig_pos)) && (i != sig) && (uart_func != 0x0f)) {
                regval2 &= (~(0x0f << sig_pos));
                regval2 |= (0x0f << sig_pos);
            }
        }
        putreg32(regval, reg_base + GLB_UART_CFG1_OFFSET);
        putreg32(regval2, reg_base + GLB_UART_CFG2_OFFSET);
    } else {
        sig_pos = (sig - 8) << 2;

        regval = getreg32(reg_base + GLB_UART_CFG2_OFFSET);
        regval &= (~(0x0f << sig_pos));
        regval |= (uart_func << sig_pos);

        for (uint8_t i = 8; i < 12; i++) {
            /* reset other sigs which are the same with uart_func */
            sig_pos = (i - 8) << 2;
            if (((regval & (0x0f << sig_pos)) == (uart_func << sig_pos)) && (i != sig) && (uart_func != 0x0f)) {
                regval &= (~(0x0f << sig_pos));
                regval |= (0x0f << sig_pos);
            }
        }
        regval2 = getreg32(reg_base + GLB_UART_CFG1_OFFSET);

        for (uint8_t i = 0; i < 8; i++) {
            /* reset other sigs which are the same with uart_func */
            sig_pos = i << 2;
            if (((regval2 & (0x0f << sig_pos)) == (uart_func << sig_pos)) && (i != sig) && (uart_func != 0x0f)) {
                regval2 &= (~(0x0f << sig_pos));
                regval2 |= (0x0f << sig_pos);
            }
        }
        putreg32(regval, reg_base + GLB_UART_CFG2_OFFSET);
        putreg32(regval2, reg_base + GLB_UART_CFG1_OFFSET);
    }
#endif
    bflb_gpio_init(dev, pin, (7 << GPIO_FUNC_SHIFT) | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
#endif
}

int bflb_gpio_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_gpio_feature_control
    return romapi_bflb_gpio_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    uint32_t reg_base;
    uint32_t regval;
    uint8_t pin = arg;

    reg_base = dev->reg_base;
    switch (cmd) {
        case GPIO_CMD_GET_GPIO_FUN:
#if defined(BL702) || defined(BL602) || defined(BL702L)
            if ((pin % 2)) {
                regval = getreg32(reg_base + GLB_GPIO_CFGCTL0_OFFSET + (pin / 2 * 4)) & GLB_REG_GPIO_0_FUNC_SEL_MASK;
                regval >>= GLB_REG_GPIO_0_FUNC_SEL_SHIFT;
            } else {
                regval = getreg32(reg_base + GLB_GPIO_CFGCTL0_OFFSET + (pin / 2 * 4)) & GLB_REG_GPIO_1_FUNC_SEL_MASK;
                regval >>= GLB_REG_GPIO_1_FUNC_SEL_SHIFT;
            }
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL808) || defined(BL606P) || defined(BL628)
            regval = getreg32(reg_base + GLB_GPIO_CFG0_OFFSET + (pin << 2)) & GLB_REG_GPIO_0_FUNC_SEL_MASK;
            regval >>= GLB_REG_GPIO_0_FUNC_SEL_SHIFT;
#endif
            return regval;
        default:
            ret = -EPERM;
            break;
    }
    return ret;
#endif
}

struct bflb_device_s *g_gpio_int = NULL;

void gpio_all_isr(int irq, void *arg)
{
    char log_buf[64];

    if (NULL == g_gpio_int) {
        snprintf(log_buf, sizeof(log_buf), "GPIO interrupt not init\r\n");
        bflb_lhal_assert_func(__FILE__, __LINE__, __func__, log_buf);
    }
    for (uint8_t i = 0; i < sizeof(g_gpio_irq_callback) / sizeof(struct bflb_gpio_irq_callback); i++) {
        if (bflb_gpio_get_intstatus(g_gpio_int, i)) {
            if (g_gpio_irq_callback[i].handler) {
                bflb_gpio_int_clear(g_gpio_int, i);
                g_gpio_irq_callback[i].handler(i);
            } else {
                snprintf(log_buf, sizeof(log_buf), "GPIO %d interrupt not register\r\n", i);
                bflb_lhal_assert_func(__FILE__, __LINE__, __func__, log_buf);
            }
        }
    }
}

void bflb_gpio_irq_attach(uint8_t pin, void (*callback)(uint8_t pin))
{
    g_gpio_int = bflb_device_get_by_name("gpio");
    bflb_gpio_int_mask(g_gpio_int, pin, true);
    g_gpio_irq_callback[pin].handler = callback;
    bflb_gpio_int_mask(g_gpio_int, pin, false);
    bflb_irq_attach(g_gpio_int->irq_num, gpio_all_isr, NULL);
}

void bflb_gpio_irq_detach(uint8_t pin)
{
    g_gpio_int = bflb_device_get_by_name("gpio");
    bflb_gpio_int_mask(g_gpio_int, pin, true);
    g_gpio_irq_callback[pin].handler = NULL;
}