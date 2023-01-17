#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bflb_rtc.h"
#include "bflb_flash.h"
#include "board.h"
#include "bl602_glb.h"

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
    GLB_Set_System_CLK(GLB_PLL_XTAL_40M, GLB_SYS_CLK_PLL192M);
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, 95);
}

static void peripheral_clock_init(void)
{
    PERIPHERAL_CLOCK_ADC_DAC_ENABLE();
    PERIPHERAL_CLOCK_SEC_ENABLE();
    PERIPHERAL_CLOCK_DMA0_ENABLE();
    PERIPHERAL_CLOCK_UART0_ENABLE();
    PERIPHERAL_CLOCK_UART1_ENABLE();
    PERIPHERAL_CLOCK_SPI0_ENABLE();
    PERIPHERAL_CLOCK_I2C0_ENABLE();
    PERIPHERAL_CLOCK_PWM0_ENABLE();
    PERIPHERAL_CLOCK_TIMER0_1_WDG_ENABLE();

    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_160M, 0);
    GLB_Set_SPI_CLK(ENABLE, 0);
    GLB_Set_I2C_CLK(ENABLE, 0);

    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, 1);
    GLB_Set_DAC_CLK(ENABLE, GLB_DAC_CLK_XCLK, 0x3E);
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
    uint32_t flashCfgLen = 0;
    uint32_t flashJedecId = 0;

    flashJedecId = bflb_flash_get_jedec_id();
    bflb_flash_get_cfg(&pFlashCfg, &flashCfgLen);
    arch_memcpy((void *)&flashCfg, pFlashCfg, flashCfgLen);
    printf("=========== flash cfg ==============\r\n");
    printf("jedec id   0x%06X\r\n", flashJedecId);
    printf("mid            0x%02X\r\n", flashCfg.mid);
    printf("iomode         0x%02X\r\n", flashCfg.io_mode);
    printf("clk delay      0x%02X\r\n", flashCfg.clk_delay);
    printf("clk invert     0x%02X\r\n", flashCfg.clk_invert);
    printf("read reg cmd0  0x%02X\r\n", flashCfg.read_reg_cmd[0]);
    printf("read reg cmd1  0x%02X\r\n", flashCfg.read_reg_cmd[1]);
    printf("write reg cmd0 0x%02X\r\n", flashCfg.write_reg_cmd[0]);
    printf("write reg cmd1 0x%02X\r\n", flashCfg.write_reg_cmd[1]);
    printf("qe write len   0x%02X\r\n", flashCfg.qe_write_reg_len);
    printf("cread support  0x%02X\r\n", flashCfg.c_read_support);
    printf("cread code     0x%02X\r\n", flashCfg.c_read_mode);
    printf("burst wrap cmd 0x%02X\r\n", flashCfg.burst_wrap_cmd);
    printf("=====================================\r\n");
}

extern void bflb_uart_set_console(struct bflb_device_s *dev);

static void console_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, GPIO_PIN_16, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_7, GPIO_UART_FUNC_UART0_RX);

    struct bflb_uart_config_s cfg;
    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 15;
    cfg.rx_fifo_threshold = 15;

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

    console_init();

    size_t heap_len = ((size_t)&__HeapLimit - (size_t)&__HeapBase);
    kmem_init((void *)&__HeapBase, heap_len);

    bl_show_log();
    if (ret != 0) {
        printf("flash init fail!!!\r\n");
    }
    bl_show_flashinfo();

    printf("dynamic memory init success,heap size = %d Kbyte \r\n", ((size_t)&__HeapLimit - (size_t)&__HeapBase) / 1024);

    printf("cgen1:%08x\r\n", getreg32(BFLB_GLB_CGEN1_BASE));

    log_start();
#if defined(CONFIG_BFLOG)
    rtc = bflb_device_get_by_name("rtc");
#endif

    bflb_irq_restore(flag);
}

void board_uartx_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, GPIO_PIN_1, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_12, GPIO_UART_FUNC_UART1_RX);
}

void board_i2c0_gpio_init()
{
    // struct bflb_device_s *gpio;

    // gpio = bflb_device_get_by_name("gpio");
    // /* I2C0_SDA */
    // bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    // /* I2C0_SCL */
    // bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_spi0_gpio_init()
{
    // struct bflb_device_s *gpio;

    // gpio = bflb_device_get_by_name("gpio");
    // bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    // bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    // bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_pwm_gpio_init()
{
    // struct bflb_device_s *gpio;

    // gpio = bflb_device_get_by_name("gpio");
    // bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    // bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    // bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    // bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    // bflb_gpio_init(gpio, GPIO_PIN_4, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_ir_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* IR TX only support GPIO 11 */
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);

    /* IR RX support GPIO 11 ~ GPIO 13 */
    bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_0);
    GLB_IR_RX_GPIO_Sel(GLB_GPIO_PIN_12);
}

void board_adc_gpio_init()
{
    // struct bflb_device_s *gpio;

    // gpio = bflb_device_get_by_name("gpio");
    // bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    // bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    // bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
}

void board_dac_gpio_init()
{
    // struct bflb_device_s *gpio;

    // gpio = bflb_device_get_by_name("gpio");
    /* DAC_CHA */
    // bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    // /* DAC_CHB */
    // bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
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
