#include "bflb_adc.h"
#include "bflb_mtimer.h"
#include "board.h"

struct bflb_device_s *adc;

int main(void)
{
    board_init();
    uint16_t i = 0;

    adc = bflb_device_get_by_name("adc");

    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_4;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_2P0V;

    struct bflb_adc_channel_s chan;

    chan.pos_chan = ADC_CHAN_VABT_HALF;
    chan.neg_chan = ADC_CHANNEL_GND;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, &chan, 1);
    bflb_adc_vbat_enable(adc);

    struct bflb_adc_result_s result;
    for (i = 0; i < 10; i++) {
        bflb_adc_start_conversion(adc);

        while (bflb_adc_get_count(adc) == 0)
            ;
        do {
            uint32_t raw_data = bflb_adc_read_raw(adc);
            if (raw_data) {
                bflb_adc_parse_result(adc, &raw_data, &result, 1);
                printf("vBat = %d mV\n", (uint32_t)(result.millivolt * 2));
                bflb_adc_stop_conversion(adc);
            }
        } while (bflb_adc_get_count(adc) != 0);

        bflb_mtimer_delay_ms(500);
    }
    while (1) {
    }
}
