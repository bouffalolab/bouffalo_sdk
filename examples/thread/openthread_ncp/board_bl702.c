
#include <bl702_glb.h>
#include <bflb_clock.h>
#include <bflb_gpio.h>
#include <bflb_uart.h>
#include <bflb_flash.h>

#include <FreeRTOS.h>
#include "mem.h"

extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;

extern void log_start(void);

static void system_clock_ncp_init(void)
{
    GLB_Set_System_CLK(GLB_DLL_XTAL_32M, GLB_SYS_CLK_DLL144M);
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, 71);
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);
}

static void peripheral_clock_ncp_init(void)
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
    PERIPHERAL_CLOCK_IR_ENABLE();
    PERIPHERAL_CLOCK_I2S_ENABLE();
    PERIPHERAL_CLOCK_USB_ENABLE();
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_CAM);

    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_96M, 0);
    GLB_Set_SPI_CLK(ENABLE, 0);
    GLB_Set_I2C_CLK(ENABLE, 0);
    GLB_Set_IR_CLK(ENABLE, GLB_IR_CLK_SRC_XCLK, 15);

    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, 1);
    GLB_Set_DAC_CLK(ENABLE, GLB_DAC_CLK_XCLK, 0x3E);

    GLB_Set_USB_CLK(ENABLE);
}

static void console_ncp_init()
{
    struct bflb_device_s *gpio;
    struct bflb_device_s *uart0;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, GPIO_PIN_14, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_15, GPIO_UART_FUNC_UART0_RX);

    struct bflb_uart_config_s cfg;
    cfg.baudrate = 460800;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 15;
    cfg.rx_fifo_threshold = 15;

    uart0 = bflb_device_get_by_name("uart0");

    bflb_uart_init(uart0, &cfg);
}

void board_ncp_init(void)
{
    int ret = -1;
    uintptr_t flag;
    size_t heap_len;

    flag = bflb_irq_save();
    ret = bflb_flash_init();
    configASSERT(ret == 0);

    system_clock_ncp_init();
    peripheral_clock_ncp_init();
    bflb_irq_initialize();

    console_ncp_init();

    heap_len = ((size_t)&__HeapLimit - (size_t)&__HeapBase);
    kmem_init((void *)&__HeapBase, heap_len);

    log_start();

#if (defined(CONFIG_LUA) || defined(CONFIG_BFLB_LOG) || defined(CONFIG_FATFS))
    rtc = bflb_device_get_by_name("rtc");
#endif

#ifdef CONFIG_MBEDTLS
    extern void bflb_sec_mutex_init(void);
    bflb_sec_mutex_init();
#endif
    bflb_irq_restore(flag);
}

