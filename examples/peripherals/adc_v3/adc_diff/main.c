#include "bflb_adc_v3.h"
#include "bflb_mtimer.h"
#include "board.h"

#define TEST_COUNT 10

struct bflb_adc_channel_s chan[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_8, .neg_chan = ADC_EXTERNAL_CHANNEL_10,}, /* GPIO6 -> GPIO10 */
};

struct bflb_device_s *adc;
uint32_t raw_data[TEST_COUNT];
struct bflb_adc_result_s result[TEST_COUNT];
volatile int count = 0;

void adc_isr(int irq, void *arg)
{
    if (bflb_adc_get_int_fifo_status(adc, ADC_INT_REGULAR_FIFO_RDY)) {
        raw_data[count++] = bflb_adc_read_raw(adc);
        if (count >= TEST_COUNT) {
            bflb_adc_stop_conversion(adc);
        }
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
    cfg.differential_mode = true;
    cfg.resolution = ADC_RESOLUTION_12B;
    cfg.vref = ADC_VREF_INTERNAL_1P25;

    bflb_adc_common_init(&common_cfg);
    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config_external(adc, chan, sizeof(chan) / sizeof(chan[0]));
    bflb_adc_int_fifo_mask(adc, ADC_INT_REGULAR_FIFO_RDY, false);
    bflb_irq_attach(adc->irq_num, adc_isr, NULL);
    bflb_irq_enable(adc->irq_num);
    bflb_adc_start_conversion(adc);

    while (count < TEST_COUNT);
    for (int i = 0; i < TEST_COUNT; i++) {
        bflb_adc_parse_result(adc, raw_data, result, TEST_COUNT);
        printf("raw_data[%d] = 0x%08lX, pos=%d, neg=%d, millivolt=%d\r\n", i, raw_data[i], result[i].pos_chan, result[i].neg_chan, result[i].millivolt);
    }
    printf("ADC differential mode test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
