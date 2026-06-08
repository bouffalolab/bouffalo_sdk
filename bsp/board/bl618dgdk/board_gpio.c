
#if __has_include("board_gpio_overlay.h")
/* Use board_gpio_overlay.c instead of this file */
#else

#include "board_gpio.h"
#include "bflb_gpio.h"
#include "bl618dg_glb.h"

void board_uartx_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, GPIO_PIN_19, GPIO_UART_FUNC_UART1_CTS);
    bflb_gpio_uart_init(gpio, GPIO_PIN_20, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_21, GPIO_UART_FUNC_UART1_RX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_22, GPIO_UART_FUNC_UART1_RTS);
}

void board_ap_console_gpio_init(void)
{
    volatile uint32_t *p = (uint32_t *)0x2000C060;
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, GPIO_PIN_11, GPIO_UART_FUNC_UART0_TX);
    if ((*p) & (0x1 << 22)) {
        /* QFN68 */
        bflb_gpio_uart_init(gpio, GPIO_PIN_42, GPIO_UART_FUNC_UART0_RX);
    } else {
        bflb_gpio_uart_init(gpio, GPIO_PIN_12, GPIO_UART_FUNC_UART0_RX);
    }
}

void board_np_console_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, GPIO_PIN_20, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_21, GPIO_UART_FUNC_UART1_RX);
}

void board_lp_console_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* map GPIO_PIN_22 and GPIO_PIN_19 as UART for LP core */
    bflb_gpio_init(gpio, GPIO_PIN_22, GPIO_FUNC_UART3 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUNC_UART3 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_i2c0_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* I2C0_SDA */
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2C0_SCL */
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_spi0_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* spi cs */
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* spi clk */
    bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* spi miso */
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* spi mosi */
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_pwm_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_21, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_22, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_23, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_adc_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* ADC0_CH0 */
    // bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0); /* occupied by UART_TX of CPU_AP */
    /* ADC0_CH1 */
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC0_CH2 */
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC0_CH3 */
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC0_CH4 */
#if defined(CPU_MODEL_A0)
    bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#else
    bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#endif
    /* ADC0_CH5 */
    bflb_gpio_init(gpio, GPIO_PIN_31, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC0_CH6 */
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC0_CH7 */
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC0_CH8 */
    bflb_gpio_init(gpio, GPIO_PIN_5, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC0_CH9 */
    bflb_gpio_init(gpio, GPIO_PIN_7, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC0_CH10 */
    bflb_gpio_init(gpio, GPIO_PIN_9, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC0_CH11 */
#if defined(CPU_MODEL_A0)
    bflb_gpio_init(gpio, GPIO_PIN_25, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#else
    bflb_gpio_init(gpio, GPIO_PIN_26, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#endif
    /* ADC1_CH0 */
    // bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0); /* occupied by UART_RX of CPU_AP */
    /* ADC1_CH1 */
#if defined(CPU_MODEL_A0)
    bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#else
    bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#endif
    /* ADC1_CH2 */
    bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#if defined(CPU_MODEL_A0)
    /* ADC1_CH3 */
    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC1_CH4 */
    bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC1_CH5 */
    bflb_gpio_init(gpio, GPIO_PIN_32, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#else
    /* ADC1_CH3 */
    bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC1_CH4 */
    bflb_gpio_init(gpio, GPIO_PIN_30, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC1_CH5 */
    bflb_gpio_init(gpio, GPIO_PIN_25, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#endif
    /* ADC1_CH6 */
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC1_CH7 */
#if defined(CPU_MODEL_A0)
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#else
    bflb_gpio_init(gpio, GPIO_PIN_4, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#endif
    /* ADC1_CH8 */
    bflb_gpio_init(gpio, GPIO_PIN_6, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC1_CH9 */
    bflb_gpio_init(gpio, GPIO_PIN_8, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC1_CH10 */
    bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC1_CH11 */
#if defined(CPU_MODEL_A0)
    bflb_gpio_init(gpio, GPIO_PIN_26, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#else
    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
#endif
}

void board_dac_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* DAC_CHA */
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* DAC_CHB */
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
}

void board_audac_gpio_init(void)
{
    struct bflb_device_s *gpio;

    /* clock cfg */
    GLB_Set_Audio_AUTO_CLK(false);

    /* 638.976MHz / 26 = 24.576MHz */
    GLB_Set_CPUPLL_PostOut(true, 26);

    /* 24.576MHz / 32 = 768KHz */
    GLB_Set_Audio_SOLO_CLK(true, 31);

    gpio = bflb_device_get_by_name("gpio");

    /* audac pwm output mode */
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_AUDAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_AUDAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    /* PA enable */
    // bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    // bflb_gpio_set(gpio, GPIO_PIN_11);
}

static void board_emac_select_gmac_ip(uint8_t port)
{
    volatile uint32_t *p = (uint32_t *)0x20000390;

    if (port == 0) {
        *p |= (1 << 30);
    } else if (port == 1) {
        *p |= (1 << 31);
    }
}

void board_emac_rmii_gpio_init(uint8_t port)
{
    struct bflb_device_s *gpio;

    board_emac_select_gmac_ip(port);

    gpio = bflb_device_get_by_name("gpio");

    if (port == 0) {
        bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_30, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_31, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_32, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    } else if (port == 1) {
        bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_4, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_6, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_7, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    }
}

void board_emac_mdio_gpio_init(uint8_t port)
{
    struct bflb_device_s *gpio;

    board_emac_select_gmac_ip(port);
    gpio = bflb_device_get_by_name("gpio");

    if (port == 0) {
        bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    } else if (port == 1) {
        bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    }
}

void board_sdh_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_43, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_44, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_45, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_46, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_47, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_48, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_sdio_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_43, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_44, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_45, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_46, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_47, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_48, GPIO_FUNC_SDU | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_sdio_gpio_deinit()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_43, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_44, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_45, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_46, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_47, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_48, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
}

void board_ir_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* IR RX */
    bflb_gpio_init(gpio, GPIO_PIN_7, GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_0);
    GLB_IR_RX_GPIO_Sel(GPIO_PIN_7);

    /* IR TX */
    bflb_gpio_init(gpio, GPIO_PIN_6, GPIO_FUNC_IRTX | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_dvp_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* I2C GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* Power down GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_reset(gpio, GPIO_PIN_29);

    /* Reset GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_reset(gpio, GPIO_PIN_19);
    bflb_mtimer_delay_ms(10);
    bflb_gpio_set(gpio, GPIO_PIN_19);
    bflb_mtimer_delay_ms(10);

    /* MCLK GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* DVP0 GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_4, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_5, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_6, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_7, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_8, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_9, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_dpi_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_21, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_22, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_23, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_24, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_25, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_26, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_30, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_31, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_32, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_33, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_43, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_44, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_45, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_46, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_47, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_48, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_i2s_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_timer_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    GLB_Sel_MCU_TMR_GPIO_Clock(GPIO_PIN_0);
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_pec_i2c_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, PEC_I2C_SCL_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PEC_I2C_SDA_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_pec_ir_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, PEC_IR_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_pec_pwm_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, PEC_PWM_CH0_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PEC_PWM_CH1_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PEC_PWM_CH2_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PEC_PWM_CH3_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_pec_spi_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, PEC_SPI_SCK_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PEC_SPI_MOSI_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PEC_SPI_MISO_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_pec_uart_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, PEC_UART_TX_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PEC_UART_RX_PIN, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_canfd_gpio_init(int idx)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    if (idx == 0) {
        bflb_gpio_init(gpio, GPIO_PIN_6, GPIO_FUNC_CANFD | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_7, GPIO_FUNC_CANFD | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    } else if (idx == 1) {
        bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_FUNC_CANFD | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_CANFD | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    } else if (idx == 2) {
        bflb_gpio_init(gpio, GPIO_PIN_4, GPIO_FUNC_CANFD | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_5, GPIO_FUNC_CANFD | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    } else {
        return;
    }
}

void board_pec_dpi_gpio_init(void)
{
    struct bflb_device_s *gpio;
    const uint8_t pins[] = { PEC_DPI_PCLK_PIN, PEC_DPI_VSYNC_PIN, PEC_DPI_HSYNC_PIN, PEC_DPI_DE_PIN,         \
                             PEC_DPI_DATA0_PIN, PEC_DPI_DATA1_PIN, PEC_DPI_DATA2_PIN, PEC_DPI_DATA3_PIN,     \
                             PEC_DPI_DATA4_PIN, PEC_DPI_DATA5_PIN, PEC_DPI_DATA6_PIN, PEC_DPI_DATA7_PIN,     \
                             PEC_DPI_DATA8_PIN, PEC_DPI_DATA9_PIN, PEC_DPI_DATA10_PIN, PEC_DPI_DATA11_PIN,   \
                             PEC_DPI_DATA12_PIN, PEC_DPI_DATA13_PIN, PEC_DPI_DATA14_PIN, PEC_DPI_DATA15_PIN, \
                             PEC_DPI_DATA16_PIN, PEC_DPI_DATA17_PIN, PEC_DPI_DATA18_PIN, PEC_DPI_DATA19_PIN, \
                             PEC_DPI_DATA20_PIN, PEC_DPI_DATA21_PIN, PEC_DPI_DATA22_PIN, PEC_DPI_DATA23_PIN };

    gpio = bflb_device_get_by_name("gpio");
    for (int i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        bflb_gpio_init(gpio, pins[i], GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    }
}

void board_usb_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_40, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_41, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
}

void board_audio_pll_config_for_rate(unsigned int samplerate_hz)
{
    switch (samplerate_hz) {
        case 8000:
        case 16000:
        case 32000:
        case 48000:
        case 96000: //638.976MHz/26/256=96KHz
            GLB_Config_CPUPLL_To_638P976M_By_Sdmin();
            GLB_Set_CPUPLL_PostOut(ENABLE, 26);
            break;
        case 11025:
        case 22050:
        case 44100: //632.2176MHz/28/512=44.1KHz
            GLB_Config_CPUPLL_To_632P2176M_By_Sdmin();
            GLB_Set_CPUPLL_PostOut(ENABLE, 28);
            break;
        default:
            GLB_Config_CPUPLL_To_638P976M_By_Sdmin();
            GLB_Set_CPUPLL_PostOut(ENABLE, 26);
            break;
    }
}

void board_i2s_codec_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* I2S_FS */
    bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_DI */
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_DO */
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_BCLK */
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* I2S output MCLK,
    Will change the clock source of i2s,
    It needs to be called before i2s is initialized
    clock source 960/156M
    */
    GLB_Set_I2S_CLK(ENABLE, 0, GLB_I2S_DI_SEL_I2S_DI_INPUT, GLB_I2S_DO_SEL_I2S_DO_OUTPT);
    GLB_Set_Chip_Clock_Out3_Sel(GLB_CHIP_CLK_OUT_3_I2S_REF_CLK);

    /* MCLK CLKOUT */
    bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* I2C0_SCL */
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    /* I2C0_SDA */
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
}

#if defined(CONFIG_BSP_LCD)

void board_lcd_dbi_type_c_4_wire_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_FUNC_DBI_C | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_FUNC_DBI_C | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_DBI_C | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_FUNC_DBI_C | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_lcd_dbi_type_c_3_wire_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_FUNC_DBI_C | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_FUNC_DBI_C | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_DBI_C | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_lcd_dbi_type_b_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_21, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_22, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_23, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_24, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_25, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_26, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_FUNC_DBI_B | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_lcd_dbi_ex_qspi_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_DBI_QSPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_DBI_QSPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUNC_DBI_QSPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_FUNC_DBI_QSPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_4, GPIO_FUNC_DBI_QSPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_5, GPIO_FUNC_DBI_QSPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_lcd_spi_0_hard_4_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* LCD_SPI_DAT */
    bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    /* LCD_SPI_CLK */
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}
#if defined(GPIO_FUNC_SPI1)
void board_lcd_spi_1_hard_4_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* LCD_SPI_DAT */
    bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_SPI1 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    /* LCD_SPI_CLK */
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_SPI1 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}
#endif
#endif
#endif
