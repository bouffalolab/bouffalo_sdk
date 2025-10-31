#include "bflb_adc_v2.h"
#include "bflb_mtimer.h"
#include "board.h"

#define TEST_COUNT 10

struct bflb_adc_channel_s chan[] = {
    {.pos_chan = ADC_INTERNAL_CHANNEL_TSEN_P, .neg_chan = ADC_INTERNAL_CHANNEL_NULL,},
};

struct bflb_device_s *adc;

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc = bflb_device_get_by_name(BFLB_NAME_ADC_V2_0);
    printf("adc = 0x%08lX\r\n", adc);

    struct bflb_adc_config_s cfg;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = true;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_INTERNAL_1P25;

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
