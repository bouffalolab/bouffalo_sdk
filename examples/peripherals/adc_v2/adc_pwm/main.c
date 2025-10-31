#include "bflb_adc_v2.h"
#include "bflb_pwm_v2.h"
#include "bflb_mtimer.h"
#include "bflb_clock.h"
#include "board.h"

struct bflb_adc_channel_s chan[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_1, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO1 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_2, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO2 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_3, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO3 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_0, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO0 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_4, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO4 -> GND */
};

struct bflb_device_s *pwm;
struct bflb_device_s *adc;
uint32_t raw_data;
struct bflb_adc_result_s result;

void adc_isr(int irq, void *arg)
{
    if (bflb_adc_get_int_status(adc, ADC_INT_REGULAR_FIFO_RDY)) {
        bflb_adc_int_clear(adc, ADC_INT_REGULAR_FIFO_RDY);
        printf("\r\nscan done:\r\n");
        while (bflb_adc_get_count(adc) > 0) {
            raw_data = bflb_adc_read_raw(adc);
            bflb_adc_parse_result(adc, &raw_data, &result, 1);
            printf("raw_data = 0x%08lX, pos=%d, neg=%d, millivolt=%d\r\n", raw_data, result.pos_chan, result.neg_chan, result.millivolt);
        }
    }
}

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc = bflb_device_get_by_name(BFLB_NAME_ADC_V2_0);
    printf("adc = 0x%08lX\r\n", adc);

    struct bflb_adc_config_s cfg;
    cfg.scan_conv_mode = true;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_12B;
    cfg.vref = ADC_VREF_INTERNAL_1P25;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config_external(adc, chan, sizeof(chan) / sizeof(chan[0]));
    bflb_adc_feature_control(adc, ADC_CMD_SET_THRE_RDY_REGULAR, sizeof(chan) / sizeof(chan[0]));
    bflb_adc_feature_control(adc, ADC_CMD_PWM_TRIG_EN, true);
    bflb_adc_int_mask(adc, ADC_INT_REGULAR_FIFO_RDY, false);
    bflb_irq_attach(adc->irq_num, adc_isr, NULL);
    bflb_irq_enable(adc->irq_num);

    pwm = bflb_device_get_by_name(BFLB_NAME_PWM_V2_0);
    printf("pwm = 0x%08lX\r\n", pwm);
    struct bflb_pwm_v2_config_s pwm_cfg;
    pwm_cfg.clk_source = BFLB_SYSTEM_PBCLK;
    pwm_cfg.clk_div = 160;
    pwm_cfg.period = 1000;
    bflb_pwm_v2_init(pwm, &pwm_cfg);
    bflb_pwm_v2_feature_control(pwm, PWM_CMD_SET_REPT_COUNT, 5);
    bflb_pwm_v2_feature_control(pwm, PWM_CMD_SET_STOP_ON_REPT, true);
    bflb_pwm_v2_feature_control(pwm, PWM_CMD_SET_TRIG_ADC_SRC, PWM_TRIG_ADC_SRC_PERIOD);
    bflb_pwm_v2_start(pwm);

    bflb_mtimer_delay_ms(1000);
    printf("\r\nADC scan mode trigged by pwm test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
