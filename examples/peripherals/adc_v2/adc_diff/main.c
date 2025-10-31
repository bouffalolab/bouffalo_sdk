#include "bflb_adc_v2.h"
#include "bflb_mtimer.h"
#include "board.h"

#define TEST_COUNT 10

struct bflb_adc_channel_s chan[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_0, .neg_chan = ADC_EXTERNAL_CHANNEL_1,}, /* GPIO0 -> GPIO1 */
};

struct bflb_device_s *adc;
uint32_t raw_data;
struct bflb_adc_result_s result;
volatile int flag = 0;

void adc_isr(int irq, void *arg)
{
    if (bflb_adc_get_int_status(adc, ADC_INT_REGULAR_FIFO_RDY)) {
        flag = 1;
        bflb_adc_int_mask(adc, ADC_INT_REGULAR_FIFO_RDY, true);
    }
}

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc = bflb_device_get_by_name(BFLB_NAME_ADC_V2_0);
    printf("adc = 0x%08lX\r\n", adc);

    struct bflb_adc_config_s cfg;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = true;
    cfg.differential_mode = true;
    cfg.resolution = ADC_RESOLUTION_12B;
    cfg.vref = ADC_VREF_INTERNAL_1P25;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config_external(adc, chan, sizeof(chan) / sizeof(chan[0]));
    bflb_adc_feature_control(adc, ADC_CMD_SET_THRE_RDY_REGULAR, TEST_COUNT);
    bflb_adc_int_mask(adc, ADC_INT_REGULAR_FIFO_RDY, false);
    bflb_irq_attach(adc->irq_num, adc_isr, NULL);
    bflb_irq_enable(adc->irq_num);
    bflb_adc_start_conversion(adc);

    while (flag == 0) {
        bflb_mtimer_delay_ms(100);
    }
    bflb_adc_stop_conversion(adc);
    for (int i = 0; i < TEST_COUNT; i++) {
        raw_data = bflb_adc_read_raw(adc);
        bflb_adc_parse_result(adc, &raw_data, &result, 1);
        printf("raw_data = 0x%08lX, pos=%d, neg=%d, millivolt=%d\r\n", raw_data, result.pos_chan, result.neg_chan, result.millivolt);
    }
    printf("ADC differential mode test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
