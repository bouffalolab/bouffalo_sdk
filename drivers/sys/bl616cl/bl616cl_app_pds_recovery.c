#include "bl_lp_internal.h"
#include "bl616cl_ext_dcdc.h"
#include "bl616cl_pm.h"
#include "bflb_clock.h"
#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_wo.h"

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
#ifdef CONFIG_BSP_SDH_SDCARD
    uint32_t sdh_cfg0;
#endif
} bl_lp_peripheral_clock_snapshot_t;

typedef struct {
    uint32_t baudrate;
    uint32_t cfg1;
    uint32_t cfg2;
} bl_lp_uart_snapshot_t;

static ATTR_NOCACHE_RAM_SECTION uint32_t pds_resume_gpio_cfg[GPIO_PIN_MAX] = { 0 };
#ifndef CONFIG_CONSOLE_WO
static ATTR_NOCACHE_RAM_SECTION bl_lp_uart_snapshot_t pds_resume_uart_cfg = {
    .baudrate = 2000000,
    .cfg1 = 0,
    .cfg2 = 0,
};
#endif
static ATTR_NOCACHE_RAM_SECTION bl_lp_peripheral_clock_snapshot_t pds_resume_peripheral_clock_cfg = { 0 };

static struct bflb_device_s *gpio_lp = NULL;

static void ATTR_TCM_SECTION pds_gpio_keep_enable(uint32_t sf_pin_select);
static void ATTR_TCM_SECTION bl_lp_runtime_gpio_snapshot(void);
static void ATTR_TCM_SECTION bl_lp_runtime_gpio_restore(void);
static uint32_t ATTR_TCM_SECTION get_sf_pin_select(void);
static void ATTR_TCM_SECTION bl_lp_system_init(void);
static void ATTR_TCM_SECTION bl_lp_system_clock_init(void);
static void ATTR_TCM_SECTION bl_lp_peripheral_clock_init(void);
static void bl_lp_console_init(void);
static void ATTR_TCM_SECTION pds_cpu_wakeup_cb(void);
static void ATTR_TCM_SECTION pds_wakeup_recover(void);
static uint8_t bl_lp_get_current_mcu_clk_sel(void);
static void bl_lp_runtime_clock_snapshot(void);
static void bl_lp_runtime_uart_baudrate_snapshot(void);
static void bl_lp_runtime_peripheral_clock_snapshot(void);
static int ATTR_TCM_SECTION is_flash_io(uint8_t pin, uint32_t sf_pin_select);
static int ATTR_TCM_SECTION is_psram_io(uint8_t pin);
static int ATTR_TCM_SECTION is_uart_io(uint8_t pin);

static void ATTR_TCM_SECTION bl_lp_system_init(void)
{
    uint32_t mstatus = __get_MSTATUS();
    mstatus |= (1 << 13);
    __set_MSTATUS(mstatus);

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
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x3;
#else
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x2;
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x2;
#endif

    csi_dcache_enable();
    csi_icache_enable();

    uint32_t mexstatus = __get_MEXSTATUS();
    mexstatus &= ~(0x3 << 16);
    __set_MEXSTATUS(mexstatus);

    extern uint8_t __LD_CONFIG_EM_SEL;
    volatile uint32_t em_size = (uint32_t)&__LD_CONFIG_EM_SEL;
    uint32_t tmpVal = BL_RD_REG(GLB_BASE, GLB_SRAM_CFG3);

    if (em_size == 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x00);
    } else if (em_size == 16 * 1024) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x01);
    } else if (em_size == 32 * 1024) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x03);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x00);
    }
    BL_WR_REG(GLB_BASE, GLB_SRAM_CFG3, tmpVal);
}

static void ATTR_TCM_SECTION bl_lp_system_clock_init(void)
{
    lp_fw_clock_t *clock_cfg = &pds_resume_clock_cfg;

    if ((clock_cfg->xclk_sel == HBN_MCU_XCLK_XTAL) || (clock_cfg->mcu_clk_sel != GLB_MCU_SYS_CLK_RC32M)) {
        GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL);
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
#ifdef CONFIG_BSP_SDH_SDCARD
    BL_WR_REG(GLB_BASE, GLB_SDH_CFG0, pds_resume_peripheral_clock_cfg.sdh_cfg0);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDH);
#endif
}

static void bl_lp_console_init(void)
{
#ifdef CONFIG_CONSOLE_WO
    extern void bflb_wo_set_console(struct bflb_device_s *dev);
    struct bflb_device_s *wo = bflb_device_get_by_name("wo");
    bflb_wo_uart_init(wo, 2000000, GPIO_PIN_34);
    bflb_wo_set_console(wo);
#else
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    struct bflb_uart_config_s cfg;

    BL_WR_REG(GLB_BASE, GLB_UART_CFG1, pds_resume_uart_cfg.cfg1);
    BL_WR_REG(GLB_BASE, GLB_UART_CFG2, pds_resume_uart_cfg.cfg2);

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
#endif
}

static void ATTR_TCM_SECTION pds_cpu_wakeup_cb(void)
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
        "ori    a0, a0, 3\n\t"
        "csrw   mtvec, a0\n\t"
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

#if PM_PDS_LDO18IO_POWER_DOWN
    GLB_Power_Up_Ldo18ioVout();
#endif

    AON_LDO18_IO_Switch_Flash(1);

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
            return GLB_MCU_SYS_CLK_WIFIPLL_96M;
        case 1:
            return GLB_MCU_SYS_CLK_WIFIPLL_192M;
        case 2:
            return GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M;
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

static void bl_lp_runtime_uart_baudrate_snapshot(void)
{
#ifndef CONFIG_CONSOLE_WO
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    uint32_t reg_base = uart0->reg_base;
    uint32_t bit_prd = getreg32(reg_base + UART_BIT_PRD_OFFSET);
    uint32_t div = (bit_prd & UART_CR_UTX_BIT_PRD_MASK) >> UART_CR_UTX_BIT_PRD_SHIFT;
    uint32_t uart_clk = bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_UART, uart0->idx);

    div += 1;
    if (div == 0) {
        div = 1;
    }

    pds_resume_uart_cfg.baudrate = (uart_clk + (div / 2)) / div;
    pds_resume_uart_cfg.cfg1 = BL_RD_REG(GLB_BASE, GLB_UART_CFG1);
    pds_resume_uart_cfg.cfg2 = BL_RD_REG(GLB_BASE, GLB_UART_CFG2);
#endif
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
#ifdef CONFIG_BSP_SDH_SDCARD
    pds_resume_peripheral_clock_cfg.sdh_cfg0 = BL_RD_REG(GLB_BASE, GLB_SDH_CFG0);
#endif
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
        if (!is_flash_io(i, sf_pin_select) && !is_psram_io(i)) {
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

static int ATTR_TCM_SECTION is_psram_io(uint8_t pin)
{
    return (pin >= 46 && pin <= 57);
}

static int ATTR_TCM_SECTION is_uart_io(uint8_t pin)
{
    return (pin >= 34 && pin <= 35);
}

static void ATTR_TCM_SECTION pds_gpio_keep_enable(uint32_t sf_pin_select)
{
    if (gpio_lp == NULL) {
        gpio_lp = bflb_device_get_by_name("gpio");
    }

    for (uint8_t i = GPIO_PIN_6; i < GPIO_PIN_MAX; i++) {
        if (!is_flash_io(i, sf_pin_select) && !is_psram_io(i) && !is_uart_io(i)) {
            if (bl_ext_dcdc_pds_is_keep_pin(i)) {
                PDS_Enable_GPIO_Keep(i);
                continue;
            }
            bflb_gpio_init(gpio_lp, i, GPIO_ANALOG | GPIO_FLOAT | GPIO_DRV_0);
            PDS_Enable_GPIO_Keep(i);
        }
    }
}

static uint32_t ATTR_TCM_SECTION get_sf_pin_select(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_WORD(0x20056000 + 0x74);
    return (tmpVal >> 5) & 0x3f;
}

int ATTR_TCM_SECTION bl_lp_pds_enter_with_restore(uint32_t pds_level, uint32_t sleep_time)
{
    uintptr_t irq_flag;
    volatile bool enter_flag = true;
    uint32_t sf_pin_select;
    int ret = 0;

#if (BL_LP_TIME_DEBUG)
    memset(time_debug_buff, 0, sizeof(time_debug_buff));
    iot2lp_para->time_debug = time_debug_buff;
#endif
    bl_lp_debug_clean_time(iot2lp_para);
    bl_lp_debug_record_time(iot2lp_para, "pds enter start");

#ifdef CONFIG_PSRAM
    bl_lp_psram_para_save();
#endif
    bl_lp_xip_para_save();
    bl_lp_runtime_clock_snapshot();
    if (pds_level > 1) {
        bl_lp_runtime_peripheral_clock_snapshot();
        bl_lp_runtime_uart_baudrate_snapshot();
        bl_lp_runtime_gpio_snapshot();
    }

    if (bl_ext_dcdc_pds_prepare_enter() != 0) {
        return -1;
    }

    irq_flag = bflb_irq_save();

    lp_fw_save_cpu_para(GET_OFFSET(iot2lp_para_t, cpu_regs) + IOT2LP_PARA_ADDR);

    if (enter_flag) {
        enter_flag = false;

        pm_set_wakeup_callback(pds_cpu_wakeup_cb);
        bflb_l1c_dcache_clean_all();

        sf_pin_select = get_sf_pin_select();
        pds_gpio_keep_enable(sf_pin_select);
        arch_delay_us(200);

#ifdef CONFIG_PSRAM
        if (BL616CL_PSRAM_INIT_DONE) {
            PSram_Ctrl_Winbond_Enter_Hybrid_Sleep(PSRAM0_ID);
        }
#endif

        bl_lp_debug_record_time(iot2lp_para, "pds enter end");
        pm_pds_mode_enter(pds_level, sleep_time);
    }

    bl_lp_debug_record_time(iot2lp_para, "pds exit");

    PDS_Disable_ALL_GPIO_Keep();
    bl_lp_debug_record_time(iot2lp_para, "restore context start");

    if (pds_level > 1) {
        pds_wakeup_recover();
    } else {
        HBN_Set_MCU_XCLK_Sel(pds_resume_clock_cfg.xclk_sel);
        GLB_Set_MCU_System_CLK(pds_resume_clock_cfg.mcu_clk_sel);
        GLB_Set_MCU_System_CLK_Div(pds_resume_clock_cfg.hclk_div, pds_resume_clock_cfg.bclk_div);
    }

#ifdef CONFIG_PSRAM
    bl_lp_debug_record_time(iot2lp_para, "psram resume start");
    ret = bl_lp_psram_resume(pds_level);
    bl_lp_debug_record_time(iot2lp_para, "psram resume end");
#endif

    if (bl_ext_dcdc_pds_restore_exit() != 0 && ret == 0) {
        ret = -1;
    }

    bl_lp_debug_record_time(iot2lp_para, "restore context end");

    bflb_irq_restore(irq_flag);

#if (BL_LP_TIME_DEBUG)
    bl_lp_debug_dump_time(iot2lp_para);
#endif

    return ret;
}
