#include "bflb_adc_v2.h"
#include "bflb_mtimer.h"
#include "board.h"

#define TEST_COUNT 10

struct bflb_adc_channel_s chan[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_2, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO2 -> GND */
};

struct bflb_device_s *adc;
uint32_t raw_data[TEST_COUNT];
struct bflb_adc_result_s result[TEST_COUNT];
int count = 0;

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
    cfg.resolution = ADC_RESOLUTION_12B;
    cfg.vref = ADC_VREF_INTERNAL_1P25;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config_external(adc, chan, sizeof(chan) / sizeof(chan[0]));
    bflb_adc_start_conversion(adc);

    while (count < TEST_COUNT) {
        if (bflb_adc_get_count(adc) > 0) {
            raw_data[count++] = bflb_adc_read_raw(adc);
        }
    }
    bflb_adc_stop_conversion(adc);
    for (int i = 0; i < TEST_COUNT; i++) {
        bflb_adc_parse_result(adc, raw_data, result, TEST_COUNT);
        printf("raw_data[%d] = 0x%08lX, pos=%d, neg=%d, millivolt=%d\r\n", i, raw_data[i], result[i].pos_chan, result[i].neg_chan, result[i].millivolt);
    }
    printf("ADC basic test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
