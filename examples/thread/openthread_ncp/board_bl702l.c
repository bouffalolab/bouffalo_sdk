
#include <bl702l_glb.h>
#include <bl702l_clock.h>
#include <bflb_clock.h>
#include <bflb_gpio.h>
#include <bflb_uart.h>
#include <bflb_flash.h>

#include <FreeRTOS.h>
#include "mm.h"

extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;

extern void log_start(void);

static void system_clock_ncp_init(void)
{
    GLB_Set_System_CLK(GLB_DLL_XTAL_32M, GLB_SYS_CLK_DLL128M);
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_XCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK) / 1000000 - 1);
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);
    // HBN_Power_On_Xtal_32K();
    // HBN_32K_Sel(HBN_32K_XTAL);
}

static void peripheral_clock_ncp_init(void)
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
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;

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

    /* ram heap init */
    mem_manager_init();
    /* ocram heap init */
    heap_len = ((size_t)&__HeapLimit - (size_t)&__HeapBase);
    mm_register_heap(MM_HEAP_OCRAM_0, "OCRAM", MM_ALLOCATOR_TLSF, &__HeapBase, heap_len);

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

