
#include <bl616_glb.h>
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
    /* wifipll/audiopll */
    GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL | GLB_PLL_AUPLL);
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M);

    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_XCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK) / 1000000 - 1);
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
    PERIPHERAL_CLOCK_CAN_ENABLE();
    PERIPHERAL_CLOCK_AUDIO_ENABLE();
    PERIPHERAL_CLOCK_CKS_ENABLE();

    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_XCLK, 0);
    GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_MCU_MUXPLL_160M, 0);
    GLB_Set_DBI_CLK(ENABLE, GLB_SPI_CLK_MCU_MUXPLL_160M, 0);
    GLB_Set_I2C_CLK(ENABLE, GLB_I2C_CLK_XCLK, 0);
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, 1);
    GLB_Set_DIG_CLK_Sel(GLB_DIG_CLK_XCLK);
    GLB_Set_DIG_512K_CLK(ENABLE, ENABLE, 0x4E);
    GLB_Set_PWM1_IO_Sel(GLB_PWM1_IO_SINGLE_END);
    GLB_Set_IR_CLK(ENABLE, GLB_IR_CLK_SRC_XCLK, 19);
    GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_WIFIPLL_96M, 3);

    GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_MCU_MUXPLL_160M);
#ifdef CONFIG_BSP_SDH_SDCARD
    PERIPHERAL_CLOCK_SDH_ENABLE();
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_EXT_SDH);
#endif

    GLB_Set_USB_CLK_From_WIFIPLL(1);
    GLB_Swap_MCU_SPI_0_MOSI_With_MISO(0);
}

static void console_ncp_init()
{
    struct bflb_device_s *gpio;
    struct bflb_device_s *uart0;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, GPIO_PIN_21, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_22, GPIO_UART_FUNC_UART0_RX);

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

