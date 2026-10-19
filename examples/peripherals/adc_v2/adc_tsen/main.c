#include "bflb_adc_v2.h"
#include "bflb_mtimer.h"
#include "board.h"

#define TEST_COUNT 10

static struct bflb_adc_channel_s chan_vbat[] = {
    {
        .pos_chan = ADC_INTERNAL_CHANNEL_VBAT_HALF,
        .neg_chan = ADC_INTERNAL_CHANNEL_NULL,
    },
};

static struct bflb_adc_channel_s chan_tsen_inject[] = {
    {
        .pos_chan = ADC_INTERNAL_CHANNEL_TSEN_P,
        .neg_chan = ADC_INTERNAL_CHANNEL_NULL,
    },
    {
        .pos_chan = ADC_INTERNAL_CHANNEL_TSEN_N,
        .neg_chan = ADC_INTERNAL_CHANNEL_NULL,
    },
};

static struct bflb_device_s *adc;

static float adc_read_tsen_inject(void)
{
    struct bflb_adc_inject_context_s context;
    uint32_t raw_tsen_p;
    uint32_t raw_tsen_n;
    float temperature = 0.0f;
    int ret;

    ret = bflb_adc_inject_context_save(adc, &context);
    if (ret < 0) {
        printf("Failed to acquire ADC inject context: %d\r\n", ret);
        return temperature;
    }

    ret = bflb_adc_channel_config_internal_inject(adc, chan_tsen_inject,
                                                   sizeof(chan_tsen_inject) / sizeof(chan_tsen_inject[0]));
    if (ret < 0) {
        goto restore_context;
    }
    ret = bflb_adc_feature_control(adc, ADC_CMD_TSEN_EN, true);
    if (ret < 0) {
        goto restore_context;
    }

    bflb_adc_clear_fifo_inject(adc);
    bflb_adc_start_conversion_inject(adc);
    while (bflb_adc_get_count_inject(adc) < 2) {}

    raw_tsen_p = bflb_adc_read_raw_inject(adc);
    raw_tsen_n = bflb_adc_read_raw_inject(adc);
    temperature = bflb_adc_tsen_raw_to_temperature(adc, raw_tsen_p, raw_tsen_n);

restore_context:
    ret = bflb_adc_inject_context_restore(adc, &context);
    if (ret < 0) {
        printf("Failed to restore ADC inject context: %d\r\n", ret);
    }
    return temperature;
}

int main(void)
{
    uint32_t raw_vbat;
    struct bflb_adc_result_s vbat_result;

    board_init();

    adc = bflb_device_get_by_name(BFLB_NAME_ADC_V2_0);
    printf("adc = 0x%08lX\r\n", adc);

    struct bflb_adc_config_s cfg;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = true;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_INTERNAL_1P25;

    if (bflb_adc_init(adc, &cfg) < 0) {
        printf("Failed to initialize ADC\r\n");
        return -1;
    }
    bflb_adc_channel_config_internal(adc, chan_vbat, sizeof(chan_vbat) / sizeof(chan_vbat[0]));
    bflb_adc_feature_control(adc, ADC_CMD_VBAT_EN, true);
    bflb_adc_start_conversion(adc);

    for (int i = 0; i < TEST_COUNT; i++) {
        while (bflb_adc_get_count(adc) == 0) {}
        raw_vbat = bflb_adc_read_raw(adc);
        bflb_adc_parse_result(adc, &raw_vbat, &vbat_result, 1);
        printf("vbat = %d mV\r\n", vbat_result.millivolt * 2);

        printf("temperature = %.2f\r\n", adc_read_tsen_inject());
        bflb_mtimer_delay_ms(100);
    }

    bflb_adc_stop_conversion(adc);
    printf("ADC VBAT and temperature sensor inject test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
