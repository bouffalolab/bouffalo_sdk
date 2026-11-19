#include "board.h"
#include "bflb_gpio.h"
#include "bflb_i2c.h"
#include "board_gpio.h"
#include "hardware/i2c_reg.h"
#include "bl618dg_glb.h"

#ifdef CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"

static StaticSemaphore_t shared_i2c_storage;
static SemaphoreHandle_t shared_i2c_mutex;

int board_shared_i2c_init(void)
{
    if (shared_i2c_mutex == NULL) {
        shared_i2c_mutex = xSemaphoreCreateMutexStatic(&shared_i2c_storage);
    }
    return shared_i2c_mutex == NULL ? -1 : 0;
}

int board_shared_i2c_acquire(bool hardware)
{
    if (shared_i2c_mutex == NULL || xSemaphoreTake(shared_i2c_mutex, portMAX_DELAY) != pdTRUE) {
        return -1;
    }
    if (hardware) {
        /* Restore hardware I2C after another user changes the shared pin mode. */
        struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
        struct bflb_device_s *i2c = bflb_device_get_by_name("i2c0");
        bflb_gpio_init(gpio, BOARD_I2C_SCL_PIN, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
        bflb_gpio_init(gpio, BOARD_I2C_SDA_PIN, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
        bflb_i2c_init(i2c, BOARD_AUDIO_I2C_FREQUENCY);
        /* GPIO-to-I2C handoff can leave the hardware bus monitor busy. */
        putreg32(I2C_CR_I2C_BUS_BUSY_CLR, i2c->reg_base + I2C_BUS_BUSY_OFFSET);
        putreg32(0, i2c->reg_base + I2C_BUS_BUSY_OFFSET);
    }
    return 0;
}

void board_shared_i2c_release(void)
{
    xSemaphoreGive(shared_i2c_mutex);
}

#endif /* CONFIG_FREERTOS */

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

void board_i2s_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* I2S_FS */
    bflb_gpio_init(gpio, BOARD_I2S_FS_PIN, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_DI */
    bflb_gpio_init(gpio, BOARD_I2S_DI_PIN, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_DO */
    bflb_gpio_init(gpio, BOARD_I2S_DO_PIN, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_BCLK */
    bflb_gpio_init(gpio, BOARD_I2S_BCLK_PIN, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_i2s_codec_gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    board_i2s_gpio_init();

    /* I2S output MCLK,
    Will change the clock source of i2s,
    It needs to be called before i2s is initialized
    clock source 960/156M
    */
    GLB_Set_I2S_CLK(ENABLE, 0, GLB_I2S_DI_SEL_I2S_DI_INPUT, GLB_I2S_DO_SEL_I2S_DO_OUTPT);
    GLB_Set_Chip_Clock_Out1_Sel(GLB_CHIP_CLK_OUT_1_I2S_REF_CLK);

    /* MCLK CLKOUT */
    bflb_gpio_init(gpio, BOARD_I2S_MCLK_PIN, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* I2C0_SCL */
    bflb_gpio_init(gpio, BOARD_I2C_SCL_PIN, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    /* I2C0_SDA */
    bflb_gpio_init(gpio, BOARD_I2C_SDA_PIN, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    /* PA enable (active high) */
    bflb_gpio_init(gpio, BOARD_AUDIO_PA_PIN, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_set(gpio, BOARD_AUDIO_PA_PIN);
}
