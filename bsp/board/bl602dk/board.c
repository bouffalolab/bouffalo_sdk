#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bflb_rtc.h"
#include "bflb_flash.h"
#include "mmheap.h"
#include "board.h"
#include "bl602_glb.h"
#include "bl602_sflash.h"

extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;

struct heap_info mmheap_root;

static struct bflb_device_s *uart0;

static struct heap_region system_mmheap[] = {
    { NULL, 0 },
    { NULL, 0 }, /* Terminates the array. */
};

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
    SPI_Flash_Cfg_Type flashCfg;
    uint8_t *pFlashCfg = NULL;
    uint32_t flashCfgLen = 0;
    uint32_t flashJedecId = 0;

    flashJedecId = bflb_flash_get_jedec_id();
    bflb_flash_get_cfg(&pFlashCfg, &flashCfgLen);
    arch_memcpy((void *)&flashCfg, pFlashCfg, flashCfgLen);
    printf("=========== flash cfg ==============\r\n");
    printf("jedec id   0x%06X\r\n", flashJedecId);
    printf("mid            0x%02X\r\n", flashCfg.mid);
    printf("iomode         0x%02X\r\n", flashCfg.ioMode);
    printf("clk delay      0x%02X\r\n", flashCfg.clkDelay);
    printf("clk invert     0x%02X\r\n", flashCfg.clkInvert);
    printf("read reg cmd0  0x%02X\r\n", flashCfg.readRegCmd[0]);
    printf("read reg cmd1  0x%02X\r\n", flashCfg.readRegCmd[1]);
    printf("write reg cmd0 0x%02X\r\n", flashCfg.writeRegCmd[0]);
    printf("write reg cmd1 0x%02X\r\n", flashCfg.writeRegCmd[1]);
    printf("qe write len   0x%02X\r\n", flashCfg.qeWriteRegLen);
    printf("cread support  0x%02X\r\n", flashCfg.cReadSupport);
    printf("cread code     0x%02X\r\n", flashCfg.cReadMode);
    printf("burst wrap cmd 0x%02X\r\n", flashCfg.burstWrapCmd);
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
    uintptr_t flag;

    flag = bflb_irq_save();

    bflb_flash_init();

    system_clock_init();
    peripheral_clock_init();
    bflb_irq_initialize();

    bflb_irq_restore(flag);

    system_mmheap[0].addr = (uint8_t *)&__HeapBase;
    system_mmheap[0].mem_size = ((size_t)&__HeapLimit - (size_t)&__HeapBase);

    if (system_mmheap[0].mem_size > 0) {
        mmheap_init(&mmheap_root, system_mmheap);
    }

    console_init();

    bl_show_log();

    printf("dynamic memory init success,heap size = %d Kbyte \r\n", system_mmheap[0].mem_size / 1024);
}

void board_uartx_gpio_init()
{
    // struct bflb_device_s *gpio;

    // gpio = bflb_device_get_by_name("gpio");
    // bflb_gpio_uart_init(gpio, GPIO_PIN_18, GPIO_UART_FUNC_UART1_TX);
    // bflb_gpio_uart_init(gpio, GPIO_PIN_19, GPIO_UART_FUNC_UART1_RX);
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