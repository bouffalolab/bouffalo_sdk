#include "bflb_adc_v3.h"
#include "bflb_mtimer.h"
#include "board.h"

#define TEST_COUNT 10

#define VOLTAGE_MEAN  (1200 * 65536 / 1800)
#define VOLTAGE_NOISE (100 * 65536 / 1800)

struct bflb_adc_channel_s chan[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_8, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO6 -> GND */
};

struct bflb_device_s *adc;
uint32_t raw_data;
struct bflb_adc_result_s result;
volatile int count = 0;

void adc_isr(int irq, void *arg)
{
    if (bflb_adc_get_int_fifo_status(adc, ADC_INT_REGULAR_FIFO_RDY)) {
        raw_data = bflb_adc_read_raw(adc);
        bflb_adc_parse_result(adc, &raw_data, &result, 1);
        printf("raw_data[%d] = 0x%08lX, pos=%d, neg=%d, millivolt=%d\r\n", count, raw_data, result.pos_chan, result.neg_chan, result.millivolt);
        if (++count >= TEST_COUNT) {
            bflb_adc_stop_conversion(adc);
            bflb_adc_clear_fifo(adc);
        }
    }
    if (bflb_adc_get_int_watchdog_status(adc, ADC_INT_WATCHDOG)) {
        printf("ADC watchdog triggered\r\n");
        bflb_adc_int_watchdog_clear(adc, ADC_INT_WATCHDOG);
    }
}

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc = bflb_device_get_by_name("adc_v3_1");
    printf("adc_v3_1 = 0x%08lX\r\n", adc);

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

    struct bflb_adc_watchdog_config_s wdg;
    wdg.enable = true;
    wdg.adc_id = adc->idx;
    wdg.channel_type = 0;
    wdg.channel_id = 0;
    wdg.upper_threshold = VOLTAGE_MEAN + VOLTAGE_NOISE;
    wdg.lower_threshold = VOLTAGE_MEAN - VOLTAGE_NOISE;
    wdg.counter_threshold = 3;

    bflb_adc_common_init(&common_cfg);
    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config_external(adc, chan, sizeof(chan) / sizeof(chan[0]));
    bflb_adc_watchdog_init(adc, &wdg);
    bflb_adc_int_watchdog_mask(adc, ADC_INT_WATCHDOG, false);
    bflb_adc_int_fifo_mask(adc, ADC_INT_REGULAR_FIFO_RDY, false);
    bflb_irq_attach(adc->irq_num, adc_isr, NULL);
    bflb_irq_enable(adc->irq_num);
    bflb_adc_start_conversion(adc);

    while (count < TEST_COUNT);
    bflb_mtimer_delay_ms(100);
    printf("ADC watchdog test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
