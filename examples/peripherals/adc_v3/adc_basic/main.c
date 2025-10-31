#include "bflb_adc_v3.h"
#include "bflb_mtimer.h"
#include "board.h"

#define TEST_COUNT 10

struct bflb_adc_channel_s chan[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_1, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO13 -> GND */
};

struct bflb_device_s *adc;
uint32_t raw_data[TEST_COUNT];
struct bflb_adc_result_s result[TEST_COUNT];
int count = 0;

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
    cfg.resolution = ADC_RESOLUTION_12B;
    cfg.vref = ADC_VREF_INTERNAL_1P25;

    bflb_adc_common_init(&common_cfg);
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
