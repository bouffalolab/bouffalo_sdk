#include "bflb_adc_v3.h"
#include "bflb_mtimer.h"
#include "board.h"

#define TEST_COUNT 10

struct bflb_adc_channel_s chan[] = {
    {.pos_chan = ADC0_INTERNAL_CHANNEL_TSEN_P, .neg_chan = ADC0_INTERNAL_CHANNEL_NULL,},
};

struct bflb_device_s *adc;

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc = bflb_device_get_by_name("adc_v3_0");
    printf("adc_v3_0 = 0x%08lX\r\n", adc);

    struct bflb_adc_common_config_s common_cfg;
    common_cfg.clk_div = ADC_CLK_DIV_20;
    common_cfg.mode = ADC_MODE_SEPARATE;
    common_cfg.fifo1_enable = true;

    struct bflb_adc_config_s cfg;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = true;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_INTERNAL_1P25;

    bflb_adc_common_init(&common_cfg);
    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config_internal(adc, chan, sizeof(chan) / sizeof(chan[0]));
    bflb_adc_tsen_init(adc, ADC_TSEN_MOD_INTERNAL_DIODE);

    for (int i = 0; i < TEST_COUNT; i++) {
        bflb_mtimer_delay_ms(100);
        printf("temp = %.2f\r\n", bflb_adc_tsen_get_temp(adc));
    }
    printf("ADC temperature sensor test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
