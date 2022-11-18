#include "bflb_adc.h"
#include "bflb_efuse.h"
#include "bflb_mtimer.h"
#include "board.h"

struct bflb_device_s *adc;

int main(void)
{
    board_init();
    board_adc_gpio_init();
    uint16_t i = 0;
    float average_filter = 0.0;

    adc = bflb_device_get_by_name("adc");

    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_2P0V;

    struct bflb_adc_channel_s chan;

    chan.pos_chan = ADC_CHANNEL_TSEN_P;
    chan.neg_chan = ADC_CHANNEL_GND;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, &chan, 1);
    bflb_adc_tsen_init(adc, ADC_TSEN_MOD_INTERNAL_DIODE);

    while (1) {
        for (i = 0; i < 50; i++) {
            average_filter += bflb_adc_tsen_get_temp(adc);
            bflb_mtimer_delay_ms(10);
        }

        printf("temp = %d\r\n", (uint32_t)(average_filter / 50.0));
        average_filter = 0;
    }
}
