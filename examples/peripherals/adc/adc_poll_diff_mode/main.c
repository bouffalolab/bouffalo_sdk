#include "bflb_adc.h"
#include "bflb_mtimer.h"
#include "board.h"

struct bflb_device_s *adc;

#define TEST_ADC_CHANNELS 2

#define TEST_COUNT 10

struct bflb_adc_channel_s chan[] = {
    { .pos_chan = ADC_CHANNEL_2,
      .neg_chan = ADC_CHANNEL_GND },
    { .pos_chan = ADC_CHANNEL_GND,
      .neg_chan = ADC_CHANNEL_3 },
};

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc = bflb_device_get_by_name("adc");

    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = true;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = true;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, chan, TEST_ADC_CHANNELS);

    for (uint32_t i = 0; i < TEST_COUNT; i++) {
        bflb_adc_start_conversion(adc);

        while (bflb_adc_get_count(adc) < TEST_ADC_CHANNELS) {
            bflb_mtimer_delay_ms(1);
        }

        for (size_t j = 0; j < TEST_ADC_CHANNELS; j++) {
            struct bflb_adc_result_s result;
            uint32_t raw_data = bflb_adc_read_raw(adc);
            printf("raw data:%08x\r\n", raw_data);
            bflb_adc_parse_result(adc, &raw_data, &result, 1);
            printf("pos chan %d,neg chan %d,%d mv \r\n", result.pos_chan, result.neg_chan, result.millivolt);
        }

        bflb_adc_stop_conversion(adc);
        bflb_mtimer_delay_ms(100);
    }

    bflb_adc_deinit(adc);
    
    while (1) {
    }
}
