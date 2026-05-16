#include "bl_lp_internal.h"
#include "bl618dg_pm.h"
#include "bflb_clock.h"
#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"
#include "hardware/uart_reg.h"

static ATTR_NOCACHE_RAM_SECTION lp_fw_clock_t pds_resume_clock_cfg = {
    .mcu_clk_sel = GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M,
    .hclk_div = 1,
    .bclk_div = 0,
    .xclk_sel = HBN_MCU_XCLK_XTAL,
};

typedef struct {
    uint32_t cgen_cfg1;
    uint32_t cgen_cfg2;
    uint32_t uart_cfg0;
    uint32_t spi_cfg0;
    uint32_t dbi_cfg0;
    uint32_t i2c_cfg0;
    uint32_t adc_cfg0;
    uint32_t ir_cfg0;
    uint32_t dig_clk_cfg0;
    uint32_t pwm_cfg0;
    uint32_t parm_cfg0;
    uint32_t cam_cfg0;
    uint32_t pka_clk_sel;
} bl_lp_peripheral_clock_snapshot_t;

typedef struct {
    uint32_t baudrate;
    uint8_t tx_pin;
    uint8_t rx_pin;
} bl_lp_uart_snapshot_t;

#if (BL_LP_TIME_DEBUG)
static ATTR_NOCACHE_RAM_SECTION lp_fw_time_debug_t app_pds_time_debug_buff[TIME_DEBUG_NUM_MAX] = { 0 };
#endif

static ATTR_NOCACHE_RAM_SECTION uint32_t pds_resume_gpio_cfg[GPIO_PIN_MAX] = { 0 };
static ATTR_NOCACHE_RAM_SECTION bl_lp_uart_snapshot_t pds_resume_uart_cfg = {
    .baudrate = 2000000,
    // .cfg1 = 0,
    // .cfg2 = 0,
};
static ATTR_NOCACHE_RAM_SECTION bl_lp_peripheral_clock_snapshot_t pds_resume_peripheral_clock_cfg = { 0 };
static ATTR_NOCACHE_RAM_SECTION uint32_t pds_resume_sram_cfg3 = 0;

static void bl_lp_runtime_gpio_snapshot(void);
static void bl_lp_runtime_gpio_restore(void);
static uint32_t get_sf_pin_select(void);
static void bl_lp_system_init(void);
static void bl_lp_system_clock_init(void);
static void bl_lp_peripheral_clock_init(void);
static void bl_lp_console_init(void);
static void pds_cpu_wakeup_cb(void);
static void pds_wakeup_recover(void);
static uint8_t bl_lp_get_current_mcu_clk_sel(void);
static void bl_lp_runtime_clock_snapshot(void);
static void bl_lp_runtime_uart_baudrate_snapshot(void);
static void bl_lp_runtime_peripheral_clock_snapshot(void);
static int is_flash_io(uint8_t pin, uint32_t sf_pin_select);

static void ATTR_TCM_SECTION bl_lp_system_init(void)
{
#if defined(CPU_MODEL_A0)
    uint32_t mxstatus = __get_MXSTATUS();
    mxstatus |= (1 << 22);
    mxstatus |= (1 << 15);
    __set_MXSTATUS(mxstatus);

    CLIC->CLICCFG =
        (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

    for (int i = 0; i < IRQn_LAST; i++) {
        CLIC->CLICINT[i].IE = 0;
        CLIC->CLICINT[i].IP = 0;
#ifdef CONFIG_IRQ_USE_VECTOR
        CLIC->CLICINT[i].ATTR = 1;
#else
        CLIC->CLICINT[i].ATTR = 0;
#endif
    }

#ifdef CONFIG_IRQ_USE_VECTOR
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x3;
#else
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x2;
#endif

    uint32_t mexstatus = __get_MEXSTATUS();
    mexstatus &= ~(0x3 << 16);
    __set_MEXSTATUS(mexstatus);
#else
    /* NMI causes map to 0xFFF, shared with mtvec */
    __RV_CSR_SET(CSR_MMISC_CTL, MMISC_CTL_NMI_CAUSE_FFF);
    /* Enable Branch Prediction Unit */
    __RV_CSR_SET(CSR_MMISC_CTL, MMISC_CTL_BPU);
    /* Enable mcycle and minstret counters */
    __RV_CSR_CLEAR(CSR_MCOUNTINHIBIT, 0x5);

    ECLIC_SetCfgNlbits(__ECLIC_GetInfoCtlbits());

    for (int i = 0; i < IRQn_LAST; i++) {
        ECLIC_DisableIRQ(i);
        ECLIC_ClearPendingIRQ(i);
#ifdef CONFIG_IRQ_USE_VECTOR
        ECLIC_SetShvIRQ(i, 1);
#else
        ECLIC_SetShvIRQ(i, 0);
#endif
    }

#ifdef CONFIG_IRQ_USE_VECTOR
    ECLIC_SetShvIRQ(MSOFT_IRQn, 1);
#else
    ECLIC_SetShvIRQ(MSOFT_IRQn, 0);
#endif
#endif

    /* restore embedded-memory size selection captured before sleep */
    BL_WR_REG(GLB_BASE, GLB_SRAM_CFG3, pds_resume_sram_cfg3);

#if defined(CPU_MODEL_A0)
    bflb_l1c_dcache_enable();
    bflb_l1c_icache_enable();
#else
    /* N900: inline CSR write to avoid calling Flash-resident wrappers */
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_DC_EN);
    __RV_CSR_SET(CSR_MCACHE_CTL, MCACHE_CTL_IC_EN);
#endif
}

static void ATTR_TCM_SECTION bl_lp_system_clock_init(void)
{
    lp_fw_clock_t *clock_cfg = &pds_resume_clock_cfg;

    if ((clock_cfg->xclk_sel == HBN_MCU_XCLK_XTAL) || (clock_cfg->mcu_clk_sel != GLB_MCU_SYS_CLK_RC32M)) {
        GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL | GLB_PLL_CPUPLL);
    }

    HBN_Set_MCU_XCLK_Sel(clock_cfg->xclk_sel);
    GLB_Set_MCU_System_CLK(clock_cfg->mcu_clk_sel);
    GLB_Set_MCU_System_CLK_Div(clock_cfg->hclk_div, clock_cfg->bclk_div);
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_XCLK,
                       Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK) / 1000000 - 1);
}

static void ATTR_TCM_SECTION bl_lp_peripheral_clock_init(void)
{
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, pds_resume_peripheral_clock_cfg.cgen_cfg1);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, pds_resume_peripheral_clock_cfg.cgen_cfg2);
    BL_WR_REG(GLB_BASE, GLB_UART_CFG0, pds_resume_peripheral_clock_cfg.uart_cfg0);
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, pds_resume_peripheral_clock_cfg.spi_cfg0);
    BL_WR_REG(GLB_BASE, GLB_DBI_CFG0, pds_resume_peripheral_clock_cfg.dbi_cfg0);
    BL_WR_REG(GLB_BASE, GLB_I2C_CFG0, pds_resume_peripheral_clock_cfg.i2c_cfg0);
    BL_WR_REG(GLB_BASE, GLB_ADC_CFG0, pds_resume_peripheral_clock_cfg.adc_cfg0);
    BL_WR_REG(GLB_BASE, GLB_IR_CFG0, pds_resume_peripheral_clock_cfg.ir_cfg0);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG0, pds_resume_peripheral_clock_cfg.dig_clk_cfg0);
    BL_WR_REG(GLB_BASE, GLB_PWM_CFG0, pds_resume_peripheral_clock_cfg.pwm_cfg0);
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, pds_resume_peripheral_clock_cfg.parm_cfg0);
    BL_WR_REG(GLB_BASE, GLB_CAM_CFG0, pds_resume_peripheral_clock_cfg.cam_cfg0);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2,
              BL_SET_REG_BITS_VAL(BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2),
                                  GLB_PKA_CLK_SEL,
                                  pds_resume_peripheral_clock_cfg.pka_clk_sel));
}

static void bl_lp_console_init(void)
{
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    struct bflb_uart_config_s cfg;

    bflb_gpio_uart_init(gpio, pds_resume_uart_cfg.tx_pin, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, pds_resume_uart_cfg.rx_pin, GPIO_UART_FUNC_UART0_RX);

    cfg.baudrate = pds_resume_uart_cfg.baudrate;
    cfg.direction = UART_DIRECTION_TXRX;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.bit_order = UART_LSB_FIRST;
    cfg.flow_ctrl = UART_FLOWCTRL_NONE;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;

    bflb_uart_init(uart0, &cfg);

    extern void bflb_uart_set_console(struct bflb_device_s *dev);
    bflb_uart_set_console(uart0);
}

void ATTR_TCM_SECTION pds_cpu_wakeup_cb(void)
{
    lp_fw_restore_cpu_para(GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR);
}

static void ATTR_TCM_SECTION pds_wakeup_recover(void)
{
    __asm__ volatile(
#if defined(CONFIG_FREERTOS)
        "la a0, freertos_risc_v_trap_handler\n\t"
#else
        "la a0, default_trap_handler\n\t"
#endif
        "ori     a0, a0, 3\n\t"
        "csrw    mtvec, a0\n\t"
#ifdef CONFIG_IRQ_USE_VECTOR
        "la      a0, __Vectors\n\t"
        "csrw    mtvt, a0\n\t"
#endif
#if defined(__riscv_flen)
        "csrr    t0, mstatus\n\t"
        "li      t1, ~0x6000\n\t"
        "and     t0, t0, t1\n\t"
        "li      t1, 0x2000\n\t"
        "or      t0, t0, t1\n\t"
        "csrw    mstatus, t0\n\t"
#endif
    );

    AON_Set_Ldo18_AON_Power_Switch_For_FLASH(1);
    AON_Set_Ldo18_AON_Power_Switch_For_PSRAM(1);
    AON_Power_On_MBG();
    AON_Power_On_SFReg();

    bl_lp_system_init();
    bl_lp_system_clock_init();
    bl_lp_xip_recovery();
    bl_lp_peripheral_clock_init();
    bl_lp_runtime_gpio_restore();
    bl_lp_console_init();
}

static uint8_t bl_lp_get_current_mcu_clk_sel(void)
{
    if (HBN_Get_MCU_Root_CLK_Sel() == HBN_MCU_ROOT_CLK_XCLK) {
        return (HBN_Get_MCU_XCLK_Sel() == HBN_MCU_XCLK_XTAL) ? GLB_MCU_SYS_CLK_XTAL : GLB_MCU_SYS_CLK_RC32M;
    }

    switch (BL_GET_REG_BITS_VAL(BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1), PDS_REG_PLL_SEL)) {
        case 0:
#if defined(CPU_MODEL_A0)
            return GLB_MCU_SYS_CLK_CPUPLL_DIV2;
#else
            return GLB_MCU_SYS_CLK_CPUPLL_DIV1;
#endif
        case 1:
            return GLB_MCU_SYS_CLK_CPUPLL_DIV3;
        case 2:
#if defined(CPU_MODEL_A0)
            return GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M;
#else
            return GLB_MCU_SYS_CLK_TOP_WIFIPLL_480M;
#endif
        case 3:
        default:
            return GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M;
    }
}

static void bl_lp_runtime_clock_snapshot(void)
{
    GLB_Get_MCU_System_CLK_Div(&pds_resume_clock_cfg.hclk_div, &pds_resume_clock_cfg.bclk_div);
    pds_resume_clock_cfg.xclk_sel = HBN_Get_MCU_XCLK_Sel();
    pds_resume_clock_cfg.mcu_clk_sel = bl_lp_get_current_mcu_clk_sel();
}

static uint8_t bl_lp_uart_sig_func_get(uint8_t sig)
{
    uint32_t cfg;

    if (sig < 8) {
        cfg = BL_RD_REG(GLB_BASE, GLB_UART_CFG1);
        return (cfg >> (sig * 4)) & 0x0F;
    }

    cfg = BL_RD_REG(GLB_BASE, GLB_UART_CFG2);
    return (cfg >> ((sig - 8) * 4)) & 0x0F;
}

static uint8_t bl_lp_gpio_func_get(uint8_t pin)
{
    uint32_t cfg = BL_RD_WORD(GLB_BASE + GLB_GPIO_CFG0_OFFSET + (pin << 2));

    return BL_GET_REG_BITS_VAL(cfg, GLB_REG_GPIO_0_FUNC_SEL);
}

static void bl_lp_runtime_uart_baudrate_snapshot(void)
{
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    uint32_t reg_base = uart0->reg_base;
    uint32_t bit_prd = getreg32(reg_base + UART_BIT_PRD_OFFSET);
    uint32_t div = (bit_prd & UART_CR_UTX_BIT_PRD_MASK) >> UART_CR_UTX_BIT_PRD_SHIFT;
    uint32_t uart_clk = bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_UART, uart0->idx);
    uint8_t tx_sig = 0xFF;
    uint8_t rx_sig = 0xFF;

    div += 1;
    if (div == 0) {
        div = 1;
    }

    pds_resume_uart_cfg.baudrate = (uart_clk + (div / 2)) / div;

    pds_resume_uart_cfg.tx_pin = 0xFF;
    pds_resume_uart_cfg.rx_pin = 0xFF;

    for (uint8_t sig = 0; sig < 12; sig++) {
        uint8_t func = bl_lp_uart_sig_func_get(sig);

        if (func == GPIO_UART_FUNC_UART0_TX) {
            tx_sig = sig;
        } else if (func == GPIO_UART_FUNC_UART0_RX) {
            rx_sig = sig;
        }
    }

    for (uint8_t pin = 0; pin < GPIO_PIN_MAX; pin++) {
        if (bl_lp_gpio_func_get(pin) != (GPIO_FUNC_UART >> GPIO_FUNC_SHIFT)) {
            continue;
        }

        if ((pin % 12) == tx_sig) {
            pds_resume_uart_cfg.tx_pin = pin;
        } else if ((pin % 12) == rx_sig) {
            pds_resume_uart_cfg.rx_pin = pin;
        }
    }

    // printf("[LP] uart tx_pin:%u rx_pin:%u\r\n", pds_resume_uart_cfg.tx_pin, pds_resume_uart_cfg.rx_pin);
}

static void bl_lp_runtime_peripheral_clock_snapshot(void)
{
    pds_resume_peripheral_clock_cfg.cgen_cfg1 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    pds_resume_peripheral_clock_cfg.cgen_cfg2 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG2);
    pds_resume_peripheral_clock_cfg.uart_cfg0 = BL_RD_REG(GLB_BASE, GLB_UART_CFG0);
    pds_resume_peripheral_clock_cfg.spi_cfg0 = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    pds_resume_peripheral_clock_cfg.dbi_cfg0 = BL_RD_REG(GLB_BASE, GLB_DBI_CFG0);
    pds_resume_peripheral_clock_cfg.i2c_cfg0 = BL_RD_REG(GLB_BASE, GLB_I2C_CFG0);
    pds_resume_peripheral_clock_cfg.adc_cfg0 = BL_RD_REG(GLB_BASE, GLB_ADC_CFG0);
    pds_resume_peripheral_clock_cfg.ir_cfg0 = BL_RD_REG(GLB_BASE, GLB_IR_CFG0);
    pds_resume_peripheral_clock_cfg.dig_clk_cfg0 = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);
    pds_resume_peripheral_clock_cfg.pwm_cfg0 = BL_RD_REG(GLB_BASE, GLB_PWM_CFG0);
    pds_resume_peripheral_clock_cfg.parm_cfg0 = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    pds_resume_peripheral_clock_cfg.cam_cfg0 = BL_RD_REG(GLB_BASE, GLB_CAM_CFG0);
    pds_resume_peripheral_clock_cfg.pka_clk_sel =
        BL_GET_REG_BITS_VAL(BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2), GLB_PKA_CLK_SEL);

    /* embedded-memory size selection — needed by CPU after PDS wakeup */
    pds_resume_sram_cfg3 = BL_RD_REG(GLB_BASE, GLB_SRAM_CFG3);
}

static void ATTR_TCM_SECTION bl_lp_runtime_gpio_snapshot(void)
{
    for (uint8_t i = 0; i < GPIO_PIN_MAX; i++) {
        pds_resume_gpio_cfg[i] = BL_RD_WORD(GLB_BASE + GLB_GPIO_CFG0_OFFSET + (i << 2));
    }
}

static void ATTR_TCM_SECTION bl_lp_runtime_gpio_restore(void)
{
    uint32_t sf_pin_select = get_sf_pin_select();

    for (uint8_t i = 0; i < GPIO_PIN_MAX; i++) {
        if (!is_flash_io(i, sf_pin_select)) {
            BL_WR_WORD(GLB_BASE + GLB_GPIO_CFG0_OFFSET + (i << 2), pds_resume_gpio_cfg[i]);
        }
    }
}

static int ATTR_TCM_SECTION is_flash_io(uint8_t pin, uint32_t sf_pin_select)
{
    if (sf_pin_select & (1 << 2)) {
        return (pin >= GPIO_PIN_6 && pin <= GPIO_PIN_11);
    } else if (sf_pin_select & (1 << 3)) {
        return (pin >= GPIO_PIN_24 && pin <= GPIO_PIN_29);
    }

    return 0;
}

static uint32_t ATTR_TCM_SECTION get_sf_pin_select(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_WORD(0x2000C000 + 0x5C);
    return (tmpVal >> 14) & 0x3f;
}

int ATTR_TCM_SECTION bl_lp_pds_enter_with_restore(uint32_t pds_level, uint32_t sleep_time)
{
    uintptr_t irq_flag;
    volatile bool enter_flag = true;
    int ret = 0;

#if (BL_LP_TIME_DEBUG)
    memset(app_pds_time_debug_buff, 0, sizeof(app_pds_time_debug_buff));
    iot2lp_para->time_debug = app_pds_time_debug_buff;
#endif
    bl_lp_debug_clean_time(iot2lp_para);
    bl_lp_debug_record_time(iot2lp_para, "pds enter start");

    bl_lp_xip_para_save();
    bl_lp_runtime_clock_snapshot();
    if (pds_level > 1) {
        bl_lp_runtime_peripheral_clock_snapshot();
        bl_lp_runtime_uart_baudrate_snapshot();
        bl_lp_runtime_gpio_snapshot();
    }

    irq_flag = bflb_irq_save();

    lp_fw_save_cpu_para(GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR);

    if (enter_flag) {
        enter_flag = false;

        pm_set_wakeup_callback(pds_cpu_wakeup_cb);
        bflb_l1c_dcache_clean_all();

        bl_lp_debug_record_time(iot2lp_para, "pds enter end");
        pm_pds_mode_enter(pds_level, sleep_time);
    }

    bl_lp_debug_record_time(iot2lp_para, "pds exit");
    bl_lp_debug_record_time(iot2lp_para, "restore context start");

    if (pds_level > 1) {
        pds_wakeup_recover();
    } else {
        HBN_Set_MCU_XCLK_Sel(pds_resume_clock_cfg.xclk_sel);
        GLB_Set_MCU_System_CLK(pds_resume_clock_cfg.mcu_clk_sel);
        GLB_Set_MCU_System_CLK_Div(pds_resume_clock_cfg.hclk_div, pds_resume_clock_cfg.bclk_div);
    }

    bl_lp_debug_record_time(iot2lp_para, "restore context end");

    bflb_irq_restore(irq_flag);

#if (BL_LP_TIME_DEBUG)
    bl_lp_debug_dump_time(iot2lp_para);
#endif

    return ret;
}
