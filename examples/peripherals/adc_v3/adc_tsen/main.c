#include "bflb_adc_v3.h"
#include "bflb_mtimer.h"
#include "board.h"

#define TEST_COUNT 10

static struct bflb_adc_channel_s chan_vbat[] = {
    {
        .pos_chan = ADC0_INTERNAL_CHANNEL_VBAT_HALF,
        .neg_chan = ADC0_INTERNAL_CHANNEL_NULL,
    },
};

static struct bflb_adc_channel_s chan_tsen_inject[] = {
    {
        .pos_chan = ADC0_INTERNAL_CHANNEL_TSEN_P,
        .neg_chan = ADC0_INTERNAL_CHANNEL_NULL,
    },
    {
        .pos_chan = ADC0_INTERNAL_CHANNEL_TSEN_N,
        .neg_chan = ADC0_INTERNAL_CHANNEL_NULL,
    },
};

static struct bflb_device_s *adc;

static float adc_read_tsen_inject(void)
{
    uint32_t raw_tsen_p;
    uint32_t raw_tsen_n;

    bflb_adc_clear_fifo_inject(adc, 0);
    bflb_adc_clear_fifo_inject(adc, 1);
    bflb_adc_start_conversion_inject(adc);
    while ((bflb_adc_get_count_inject(adc, 0) == 0) || (bflb_adc_get_count_inject(adc, 1) == 0)) {}

    raw_tsen_p = bflb_adc_read_raw_inject(adc, 0);
    raw_tsen_n = bflb_adc_read_raw_inject(adc, 1);

    return bflb_adc_tsen_raw_to_temperature(adc, raw_tsen_p, raw_tsen_n);
}

int main(void)
{
    uint32_t raw_vbat;
    struct bflb_adc_result_s vbat_result;

    board_init();

    adc = bflb_device_get_by_name(BFLB_NAME_ADC_V3_0);
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
    bflb_adc_channel_config_internal(adc, chan_vbat, sizeof(chan_vbat) / sizeof(chan_vbat[0]));
    bflb_adc_channel_config_internal_inject(adc, chan_tsen_inject,
                                            sizeof(chan_tsen_inject) / sizeof(chan_tsen_inject[0]));
    bflb_adc_feature_control(adc, ADC_CMD_VBAT_EN, true);
    bflb_adc_start_conversion(adc);

    for (int i = 0; i < TEST_COUNT; i++) {
        while (bflb_adc_get_count(adc) == 0) {}
        raw_vbat = bflb_adc_read_raw(adc);
        bflb_adc_parse_result(adc, &raw_vbat, &vbat_result, 1);
        printf("vbat= %d mV\r\n", vbat_result.millivolt * 2);

        printf("temperature = %.2f\r\n", adc_read_tsen_inject());
        bflb_mtimer_delay_ms(100);
    }

    bflb_adc_stop_conversion(adc);
    printf("ADC VBAT and temperature sensor inject test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
