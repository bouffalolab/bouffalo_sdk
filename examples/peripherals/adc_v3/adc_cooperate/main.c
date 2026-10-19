#include "bflb_adc_v3.h"
#include "bflb_mtimer.h"
#include "board.h"

#define TEST_COUNT 10

struct bflb_adc_channel_s chan_0[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_1, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO13 -> GND */
};

struct bflb_adc_channel_s chan_1[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_8, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO6 -> GND */
};

struct bflb_device_s *adc[2];
uint32_t raw_data[TEST_COUNT];
struct bflb_adc_result_s result[TEST_COUNT];
volatile int count = 0;

void adc_isr(int irq, void *arg)
{
    if (bflb_adc_get_int_fifo_status(adc[0], ADC_INT_REGULAR_FIFO_RDY)) {
        raw_data[count++] = bflb_adc_read_raw(adc[0]);
        if (count >= TEST_COUNT) {
            bflb_adc_stop_conversion(adc[0]);
            bflb_adc_stop_conversion(adc[1]);
        }
    }
}

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc[0] = bflb_device_get_by_name("adc_v3_0");
    printf("adc_v3_0 = 0x%08lX\r\n", adc[0]);
    adc[1] = bflb_device_get_by_name("adc_v3_1");
    printf("adc_v3_1 = 0x%08lX\r\n", adc[1]);

    struct bflb_adc_common_config_s common_cfg;
    common_cfg.clk_div = ADC_CLK_DIV_20;
    common_cfg.mode = ADC_MODE_CONCURRENT;
    common_cfg.fifo1_enable = false;

    struct bflb_adc_config_s cfg;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = true;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_INTERNAL_1P25;

    bflb_adc_common_init(&common_cfg);
    bflb_adc_init(adc[0], &cfg);
    bflb_adc_init(adc[1], &cfg);
    bflb_adc_channel_config_external(adc[0], chan_0, sizeof(chan_0) / sizeof(chan_0[0]));
    bflb_adc_channel_config_external(adc[1], chan_1, sizeof(chan_1) / sizeof(chan_1[0]));
    bflb_adc_int_fifo_mask(adc[0], ADC_INT_REGULAR_FIFO_RDY, false);
    bflb_irq_attach(adc[0]->irq_num, adc_isr, NULL);
    bflb_irq_enable(adc[0]->irq_num);
    bflb_adc_start_conversion(adc[0]);
    bflb_adc_start_conversion(adc[1]);

    while (count < TEST_COUNT);
    for (int i = 0; i < TEST_COUNT; i++) {
        bflb_adc_parse_result(adc[0], raw_data, result, TEST_COUNT);
        printf("adc%d: raw_data = 0x%08lX, pos=%d, neg=%d, millivolt=%d\r\n", result[i].adc_id, raw_data[i], result[i].pos_chan, result[i].neg_chan, result[i].millivolt);
    }
    printf("ADC cooperate mode test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
