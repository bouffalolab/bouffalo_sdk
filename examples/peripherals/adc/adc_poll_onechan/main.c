#include "bflb_adc.h"
#include "bflb_mtimer.h"
#include "board.h"

#define TEST_ADC_CHANNEL_x ADC_CHANNEL_0

static struct bflb_adc_channel_s chan[1] = {
    { .pos_chan = TEST_ADC_CHANNEL_x,
      .neg_chan = ADC_CHANNEL_GND }
};

#define TEST_COUNT 10

static struct bflb_device_s *adc;

static uint32_t raw_data;

void test_adc_poll(void)
{
    struct bflb_adc_result_s result;

    board_adc_gpio_init();

    adc = bflb_device_get_by_name("adc");

    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = true; /* do not support single mode */
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, chan, 1);

    bflb_adc_start_conversion(adc);

    raw_data = 0;
    for (uint16_t i = 0; i < TEST_COUNT; i++) {
        while (bflb_adc_get_count(adc) == 0) {
            bflb_mtimer_delay_ms(1);
        }

        if (i > 4) {
            raw_data += bflb_adc_read_raw(adc);
        } else {
            /* drop incorrect data */
            bflb_adc_read_raw(adc);
        }
    }

    raw_data = raw_data / 5;

    bflb_adc_stop_conversion(adc);

    bflb_adc_parse_result(adc, &raw_data, &result, 1);

    printf("raw data:%08x\r\n", raw_data);
    printf("pos chan %d,%d mv \r\n", result.pos_chan, result.millivolt);

    bflb_adc_deinit(adc);
}

int main(void)
{
    board_init();

    while (1) {
        test_adc_poll();
        bflb_mtimer_delay_ms(1000);
    }
}
