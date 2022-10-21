#include "bflb_adc.h"
#include "bflb_mtimer.h"
#include "board.h"

struct bflb_device_s *adc;
static uint8_t int_flag = 0;

void adc_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_adc_get_intstatus(adc);
    if (intstatus&ADC_INTSTS_ADC_READY) {
        struct bflb_adc_result_s result;

        printf("INT fifo count = %d \n", bflb_adc_get_count(adc));

        do {
            uint32_t raw_data = bflb_adc_read_raw(adc);
            if (raw_data) {
                bflb_adc_parse_result(adc, &raw_data, &result, 1);
                printf("PosId = %d NegId = %d V= %d mV \n", result.pos_chan, result.neg_chan, result.millivolt);
                bflb_adc_int_clear(adc, ADC_INTCLR_ADC_READY);
                int_flag++;
            }
        } while (bflb_adc_get_count(adc) != 0);

        if (int_flag > 16) {
            bflb_adc_rxint_mask(adc, true);
        }
    }
}

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc = bflb_device_get_by_name("adc");

    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    struct bflb_adc_channel_s chan;

    chan.pos_chan = ADC_CHANNEL_8;
    chan.neg_chan = ADC_CHANNEL_GND;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, &chan, 1);

    bflb_adc_int_clear(adc, ADC_INTCLR_ADC_READY);
    bflb_adc_rxint_mask(adc, false);
    bflb_irq_attach(adc->irq_num, adc_isr, adc);
    bflb_irq_enable(adc->irq_num);

    while (int_flag < 16) {
        printf("%d\r\n", int_flag);
        bflb_adc_start_conversion(adc);
        bflb_mtimer_delay_ms(500);
        bflb_adc_stop_conversion(adc);
    }
}
