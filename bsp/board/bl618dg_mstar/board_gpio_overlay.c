#include "board_gpio_overlay.h"

#include "bl618dg_glb.h"
#include "bflb_mtimer.h"

void board_ap_console_gpio_init(void)
{
    volatile uint32_t *package_info = (uint32_t *)0x2000C060;
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, GPIO_PIN_11, GPIO_UART_FUNC_UART0_TX);
    if ((*package_info) & (1U << 22)) {
        bflb_gpio_uart_init(gpio, GPIO_PIN_42, GPIO_UART_FUNC_UART0_RX);
    } else {
        bflb_gpio_uart_init(gpio, GPIO_PIN_12, GPIO_UART_FUNC_UART0_RX);
    }
}

void board_np_console_gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, GPIO_PIN_20, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_21, GPIO_UART_FUNC_UART1_RX);
}

void board_lp_console_gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_22,
                   GPIO_FUNC_UART3 | GPIO_ALTERNATE | GPIO_PULLUP |
                       GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_19,
                   GPIO_FUNC_UART3 | GPIO_ALTERNATE | GPIO_PULLUP |
                       GPIO_SMT_EN | GPIO_DRV_1);
}

void board_i2c0_gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    /* Current audio-board route: SCL GPIO5, SDA GPIO4. */
    bflb_gpio_init(gpio, GPIO_PIN_5,
                   GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP |
                       GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(gpio, GPIO_PIN_4,
                   GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP |
                       GPIO_SMT_EN | GPIO_DRV_2);
}

void board_audio_pll_config_for_rate(unsigned int samplerate_hz)
{
    switch (samplerate_hz) {
        case 8000:
        case 16000:
        case 32000:
        case 48000:
        case 96000:
            GLB_Config_CPUPLL_To_638P976M_By_Sdmin();
            GLB_Set_CPUPLL_PostOut(ENABLE, 26);
            break;
        case 11025:
        case 22050:
        case 44100:
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
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    /* J7 I2S: MCLK/DO/BCLK/FS/DI = GPIO6/7/8/9/10. */
    bflb_gpio_init(gpio, GPIO_PIN_9,
                   GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP |
                       GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_10,
                   GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP |
                       GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_7,
                   GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP |
                       GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_8,
                   GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP |
                       GPIO_SMT_EN | GPIO_DRV_1);

    GLB_Set_I2S_CLK(ENABLE, 0, GLB_I2S_DI_SEL_I2S_DI_INPUT,
                    GLB_I2S_DO_SEL_I2S_DO_OUTPT);
    GLB_Set_Chip_Clock_Out3_Sel(GLB_CHIP_CLK_OUT_3_I2S_REF_CLK);
    bflb_gpio_init(gpio, GPIO_PIN_6,
                   GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP |
                       GPIO_SMT_EN | GPIO_DRV_1);

    board_i2c0_gpio_init();

    /* Enable the attached audio board before wave_player probes its codec. */
    bflb_gpio_set(gpio, GPIO_PIN_48);
    bflb_gpio_init(gpio, GPIO_PIN_48,
                   GPIO_OUTPUT | GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN |
                       GPIO_DRV_0);
    bflb_mtimer_delay_ms(50);
}

void board_audac_gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    board_audio_pll_config_for_rate(48000);
    GLB_Set_Audio_SOLO_CLK(ENABLE, 3);
    bflb_gpio_init(gpio, GPIO_PIN_0,
                   GPIO_FUNC_AUDAC | GPIO_ALTERNATE | GPIO_PULLUP |
                       GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(gpio, GPIO_PIN_1,
                   GPIO_FUNC_AUDAC | GPIO_ALTERNATE | GPIO_PULLUP |
                       GPIO_SMT_EN | GPIO_DRV_2);
}

void board_auadc_gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    board_audio_pll_config_for_rate(48000);
    GLB_Set_Audio_SOLO_CLK(ENABLE, 3);
#ifdef CONFIG_AUDIO_MIC_PDM_MODE
    bflb_gpio_init(gpio, GPIO_PIN_25,
                   GPIO_FUNC_PDM | GPIO_ALTERNATE | GPIO_FLOAT |
                       GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(gpio, GPIO_PIN_26,
                   GPIO_FUNC_PDM | GPIO_ALTERNATE | GPIO_FLOAT |
                       GPIO_SMT_EN | GPIO_DRV_2);
#else
    bflb_gpio_init(gpio, GPIO_PIN_30,
                   GPIO_ANALOG | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(gpio, GPIO_PIN_31,
                   GPIO_ANALOG | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_2);
#endif
}

void board_usb_gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_40,
                   GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_41,
                   GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
}
