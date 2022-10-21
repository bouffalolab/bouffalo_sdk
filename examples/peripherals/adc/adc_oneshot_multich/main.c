#include "bflb_adc.h"
#include "bflb_mtimer.h"
#include "board.h"

struct bflb_device_s *adc;

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc = bflb_device_get_by_name("adc");

    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = true;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    struct bflb_adc_channel_s chan[3];

    chan[0].pos_chan = ADC_CHANNEL_8;
    chan[0].neg_chan = ADC_CHANNEL_GND;
    chan[1].pos_chan = ADC_CHANNEL_9;
    chan[1].neg_chan = ADC_CHANNEL_GND;
    chan[2].pos_chan = ADC_CHANNEL_10;
    chan[2].neg_chan = ADC_CHANNEL_GND;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, chan, 3);

    for (uint32_t i = 0; i < 10; i++) {
        bflb_adc_start_conversion(adc);
        while (bflb_adc_get_count(adc) < 3) {
        }
        struct bflb_adc_result_s result;

        for (uint8_t i = 0; i < 3; i++) {
            uint32_t raw_data = bflb_adc_read_raw(adc);
            printf("raw data:%08x\r\n", raw_data);
            bflb_adc_parse_result(adc, &raw_data, &result, 1);
            printf("pos chan %d,%d mv \r\n", result.pos_chan, result.millivolt);
        }

        bflb_adc_stop_conversion(adc);
        bflb_mtimer_delay_ms(100);
    }

    while (1) {
    }
}
