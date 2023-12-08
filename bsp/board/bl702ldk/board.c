#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bflb_rtc.h"
#include "bflb_flash.h"
#include "bl702l_glb.h"
#include "bl702l_clock.h"
#include "board.h"

#include "mem.h"

extern void log_start(void);

extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;

static struct bflb_device_s *uart0;

#if defined(CONFIG_BFLOG)
static struct bflb_device_s *rtc;
#endif

static void system_clock_init(void)
{
    GLB_Set_System_CLK(GLB_DLL_XTAL_32M, GLB_SYS_CLK_DLL128M);
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_XCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK) / 1000000 - 1);
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);
    // HBN_Power_On_Xtal_32K();
    // HBN_32K_Sel(HBN_32K_XTAL);
}

static void peripheral_clock_init(void)
{
    PERIPHERAL_CLOCK_ADC_DAC_ENABLE();
    PERIPHERAL_CLOCK_SEC_ENABLE();
    PERIPHERAL_CLOCK_DMA0_ENABLE();
    PERIPHERAL_CLOCK_UART0_ENABLE();
    PERIPHERAL_CLOCK_SPI0_ENABLE();
    PERIPHERAL_CLOCK_I2C0_ENABLE();
    PERIPHERAL_CLOCK_PWM0_ENABLE();
    PERIPHERAL_CLOCK_TIMER0_1_WDG_ENABLE();
    PERIPHERAL_CLOCK_IR_ENABLE();

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_CHECKSUM);

    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_XCLK, 0);
    GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_SRC_XCLK, 0);
    GLB_Set_I2C_CLK(ENABLE, GLB_I2C_CLK_SRC_XCLK, 0);
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_SRC_XCLK, 1);
    GLB_Set_IR_CLK(ENABLE, GLB_IR_CLK_SRC_XCLK, 15);
    GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_SRC_DLL128M);
}

void bl_show_log(void)
{
    printf("\r\n");
    printf("  ____               __  __      _       _       _     \r\n");
    printf(" |  _ \\             / _|/ _|    | |     | |     | |    \r\n");
    printf(" | |_) | ___  _   _| |_| |_ __ _| | ___ | | __ _| |__  \r\n");
    printf(" |  _ < / _ \\| | | |  _|  _/ _` | |/ _ \\| |/ _` | '_ \\ \r\n");
    printf(" | |_) | (_) | |_| | | | || (_| | | (_) | | (_| | |_) |\r\n");
    printf(" |____/ \\___/ \\__,_|_| |_| \\__,_|_|\\___/|_|\\__,_|_.__/ \r\n");
    printf("\r\n");
    printf("Build:%s,%s\r\n", __TIME__, __DATE__);
    printf("Copyright (c) 2022 Bouffalolab team\r\n");
}

void bl_show_flashinfo(void)
{
    spi_flash_cfg_type flashCfg;
    uint8_t *pFlashCfg = NULL;
    uint32_t flashSize = 0;
    uint32_t flashCfgLen = 0;
    uint32_t flashJedecId = 0;

    flashJedecId = bflb_flash_get_jedec_id();
    flashSize = bflb_flash_get_size();
    bflb_flash_get_cfg(&pFlashCfg, &flashCfgLen);
    arch_memcpy((void *)&flashCfg, pFlashCfg, flashCfgLen);
    printf("======== flash cfg ========\r\n");
    printf("flash size 0x%08X\r\n", flashSize);
    printf("jedec id     0x%06X\r\n", flashJedecId);
    printf("mid              0x%02X\r\n", flashCfg.mid);
    printf("iomode           0x%02X\r\n", flashCfg.io_mode);
    printf("clk delay        0x%02X\r\n", flashCfg.clk_delay);
    printf("clk invert       0x%02X\r\n", flashCfg.clk_invert);
    printf("read reg cmd0    0x%02X\r\n", flashCfg.read_reg_cmd[0]);
    printf("read reg cmd1    0x%02X\r\n", flashCfg.read_reg_cmd[1]);
    printf("write reg cmd0   0x%02X\r\n", flashCfg.write_reg_cmd[0]);
    printf("write reg cmd1   0x%02X\r\n", flashCfg.write_reg_cmd[1]);
    printf("qe write len     0x%02X\r\n", flashCfg.qe_write_reg_len);
    printf("cread support    0x%02X\r\n", flashCfg.c_read_support);
    printf("cread code       0x%02X\r\n", flashCfg.c_read_mode);
    printf("burst wrap cmd   0x%02X\r\n", flashCfg.burst_wrap_cmd);
    printf("===========================\r\n");
}

extern void bflb_uart_set_console(struct bflb_device_s *dev);

static void console_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, GPIO_PIN_14, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_15, GPIO_UART_FUNC_UART0_RX);

    struct bflb_uart_config_s cfg;
    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 0;

    uart0 = bflb_device_get_by_name("uart0");

    bflb_uart_init(uart0, &cfg);
    bflb_uart_set_console(uart0);
}

void board_init(void)
{
    int ret = -1;
    uintptr_t flag;

    flag = bflb_irq_save();

    ret = bflb_flash_init();

    system_clock_init();
    peripheral_clock_init();
    bflb_irq_initialize();

    size_t heap_len = ((size_t)&__HeapLimit - (size_t)&__HeapBase);
    kmem_init((void *)&__HeapBase, heap_len);

    console_init();

    bl_show_log();
    if (ret != 0) {
        printf("flash init fail!!!\r\n");
    }
    bl_show_flashinfo();

    printf("dynamic memory init success,heap size = %d Kbyte \r\n", ((size_t)&__HeapLimit - (size_t)&__HeapBase) / 1024);

    printf("cgen1:%08x\r\n", getreg32(BFLB_GLB_CGEN1_BASE));
#if defined(CONFIG_BFLOG)
    rtc = bflb_device_get_by_name("rtc");
#endif
    bflb_irq_restore(flag);
}

void board_uartx_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* just use console uart pin */
    bflb_gpio_uart_init(gpio, GPIO_PIN_0, GPIO_UART_FUNC_UART0_CTS);
    bflb_gpio_uart_init(gpio, GPIO_PIN_1, GPIO_UART_FUNC_UART0_RTS);
}

void board_i2c0_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* I2C0_SDA */
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2C0_SCL */
    bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_spi0_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* spi mosi */
    bflb_gpio_init(gpio, GPIO_PIN_8, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* spi miso */
    bflb_gpio_init(gpio, GPIO_PIN_9, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* spi cs */
    bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* spi clk */
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_pwm_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* pwm0 ch 0 */
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* pwm1 ch 0 */
    /* pwm1 ch 1 */
    /* pwm1 ch 2 */
    /* pwm1 ch 3 */
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_PWM1 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUNC_PWM1 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_8, GPIO_FUNC_PWM1 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_9, GPIO_FUNC_PWM1 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_adc_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* ch0 and ch1 is for uart log */
    /* adc ch 0 */
    // bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    // /* adc ch 1 */
    // bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* adc ch 2 */
    bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* adc ch 3 */
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);

    /* ch4~ch6 are only for BL704L */
    // /* adc ch 4 */
    // bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    // /* adc ch 5 */
    // bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    // /* adc ch 6 */
    // bflb_gpio_init(gpio, GPIO_PIN_21, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* adc ch 7 */
    bflb_gpio_init(gpio, GPIO_PIN_7, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* adc ch 8 */
    bflb_gpio_init(gpio, GPIO_PIN_8, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* adc ch 9 */
    bflb_gpio_init(gpio, GPIO_PIN_9, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* adc ch 10 */
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);

    /* ch11 is only for BL704L */
    // /* adc ch 11 */
    // bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
}

void board_ir_gpio_init(void)
{
    struct bflb_device_s *gpio;

    /* Can only use GPIO18 or GPIO22 */
    uint32_t pin = GPIO_PIN_22;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, pin, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);

    GLB_IR_LED_Driver_Enable();

    if (pin == GPIO_PIN_22) {
        GLB_IR_LED_Driver_Output_Enable(GLB_IR_LED0);
    } else if (pin == GPIO_PIN_18) {
        GLB_IR_LED_Driver_Output_Enable(GLB_IR_LED1);
    }
}

void board_keyscan_gpio_init(void)
{
    struct bflb_device_s *gpio;

    HBN_AON_PAD_CFG_Type aonPadCfg = {
        .ctrlEn = 0,
        .ie = 0,
        .pullUp = 0,
        .pullDown = 0,
        .oe = 0,
    };
    HBN_Aon_Pad_Cfg(1, HBN_AON_PAD_GPIO30, &aonPadCfg);
    HBN_Aon_Pad_Cfg(1, HBN_AON_PAD_GPIO31, &aonPadCfg);
    HBN_Hw_Pu_Pd_Cfg(0);

    gpio = bflb_device_get_by_name("gpio");
    /* keyscan driver io init // col */
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_FUN_KEY_SCAN_DRIVE | GPIO_ALTERNATE | GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_8, GPIO_FUN_KEY_SCAN_DRIVE | GPIO_ALTERNATE | GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_9, GPIO_FUN_KEY_SCAN_DRIVE | GPIO_ALTERNATE | GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_FUN_KEY_SCAN_DRIVE | GPIO_ALTERNATE | GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_FUN_KEY_SCAN_DRIVE | GPIO_ALTERNATE | GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_21, GPIO_FUN_KEY_SCAN_DRIVE | GPIO_ALTERNATE | GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_22, GPIO_FUN_KEY_SCAN_DRIVE | GPIO_ALTERNATE | GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_FUN_KEY_SCAN_DRIVE | GPIO_ALTERNATE | GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    GLB_Set_Kys_Drv_Col(0); // GPIO output value
    /* keyscan input io init // row */
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_FUN_KEY_SCAN_IN | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1); //
    bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_FUN_KEY_SCAN_IN | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1); //
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUN_KEY_SCAN_IN | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1); //
    bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_FUN_KEY_SCAN_IN | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1); //
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUN_KEY_SCAN_IN | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1); //
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUN_KEY_SCAN_IN | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1); //
    bflb_gpio_init(gpio, GPIO_PIN_30, GPIO_FUN_KEY_SCAN_IN | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1); //
    bflb_gpio_init(gpio, GPIO_PIN_31, GPIO_FUN_KEY_SCAN_IN | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1); //
}

#ifdef CONFIG_BFLOG
__attribute__((weak)) uint64_t bflog_clock(void)
{
    return bflb_mtimer_get_time_us();
}

__attribute__((weak)) uint32_t bflog_time(void)
{
    return BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc));
}

__attribute__((weak)) char *bflog_thread(void)
{
    return "";
}
#endif