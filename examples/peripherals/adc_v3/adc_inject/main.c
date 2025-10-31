#include "bflb_adc_v3.h"
#include "bflb_mtimer.h"
#include "board.h"

struct bflb_adc_channel_s chan_regular[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_1, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO13 -> GND */
};

struct bflb_adc_channel_s chan_inject[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_4, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO28 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_5, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO31 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_4, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO28 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_5, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO31 -> GND */
};

struct bflb_device_s *adc;
uint32_t raw_data;
struct bflb_adc_result_s result;
int count = 0;
volatile int flag_inject_done = 0;

void adc_isr(int irq, void *arg)
{
    if (bflb_adc_get_int_done_status(adc, ADC_INT_DONE_INJECT)) {
        flag_inject_done = 1;
        bflb_adc_int_done_clear(adc, ADC_INT_DONE_INJECT);
    }
}

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
    bflb_adc_channel_config_external(adc, chan_regular, sizeof(chan_regular) / sizeof(chan_regular[0]));
    bflb_adc_channel_config_external_inject(adc, chan_inject, sizeof(chan_inject) / sizeof(chan_inject[0]));
    bflb_adc_int_done_mask(adc, ADC_INT_DONE_INJECT, false);
    bflb_irq_attach(adc->irq_num, adc_isr, NULL);
    bflb_irq_enable(adc->irq_num);
    bflb_adc_start_conversion(adc);
    bflb_mtimer_delay_ms(1);
    bflb_adc_start_conversion_inject(adc);

    while (1) {
        if (bflb_adc_get_count(adc) > 0) {
            raw_data = bflb_adc_read_raw(adc);
            bflb_adc_parse_result(adc, &raw_data, &result, 1);
            printf("regular: raw_data = 0x%08lX, pos=%d, neg=%d, millivolt=%d\r\n", raw_data, result.pos_chan, result.neg_chan, result.millivolt);
            count++;
            if (count >= 10) {
                break;
            }
        }
        if (flag_inject_done) {
            flag_inject_done = 0;
            for (int i = 0; i < sizeof(chan_inject) / sizeof(chan_inject[0]); i++) {
                raw_data = bflb_adc_read_raw_inject(adc, i);
                bflb_adc_parse_result(adc, &raw_data, &result, 1);
                printf("inject_%d: raw_data = 0x%08lX, pos=%d, neg=%d, millivolt=%d\r\n", i, raw_data, result.pos_chan, result.neg_chan, result.millivolt);
            }
        }
    }
    bflb_adc_stop_conversion(adc);
    printf("ADC continuous regular and accidental inject mode test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
