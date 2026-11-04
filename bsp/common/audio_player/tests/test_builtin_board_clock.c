/* SPDX-License-Identifier: Apache-2.0 */
/* Compile the real builtin GPIO hooks so clock policy is checked as executed. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ENABLE          1
#define GPIO_PIN_0      0
#define GPIO_PIN_1      1
#define GPIO_PIN_25     25
#define GPIO_PIN_26     26
#define GPIO_PIN_30     30
#define GPIO_PIN_31     31
#define GPIO_FUNC_AUDAC 1U
#define GPIO_FUNC_PDM   2U
#define GPIO_ALTERNATE  4U
#define GPIO_PULLUP     8U
#define GPIO_FLOAT      16U
#define GPIO_SMT_EN     32U
#define GPIO_DRV_2      64U
#define GPIO_ANALOG     128U

struct bflb_device_s {
    int unused;
};
static struct bflb_device_s gpio;
static unsigned phase;
static unsigned pins;

static struct bflb_device_s *bflb_device_get_by_name(const char *name)
{
    assert(strcmp(name, "gpio") == 0);
    return &gpio;
}

static void board_audio_pll_config_for_rate(unsigned rate)
{
    assert(rate == 48000 && phase == 0);
    phase = 1;
}

static int GLB_Set_Audio_SOLO_CLK(uint8_t enable, uint8_t divider)
{
    assert(enable == ENABLE && divider == 3 && phase == 1);
    phase = 2;
    return 0;
}

static int GLB_Set_Audio_AUTO_CLK(uint8_t enable)
{
    assert(enable == ENABLE && phase == 2);
    phase = 3;
    return 0;
}

static void bflb_gpio_init(struct bflb_device_s *device, uint32_t pin, uint32_t flags)
{
    assert(device == &gpio && phase == 3 && flags != 0);
    assert(pin == GPIO_PIN_0 || pin == GPIO_PIN_1 || pin == GPIO_PIN_25 || pin == GPIO_PIN_26 || pin == GPIO_PIN_30 ||
           pin == GPIO_PIN_31);
    ++pins;
}

#include "board_builtin_clock_under_test.h"

int main(void)
{
    board_audac_gpio_init();
    assert(phase == 3 && pins == 2);
    phase = pins = 0;
    board_auadc_gpio_init();
    assert(phase == 3 && pins == 2);
    puts("Builtin board hooks: PLL, SOLO clock and automatic rate divider precede ADC/DAC pin setup");
    return 0;
}
