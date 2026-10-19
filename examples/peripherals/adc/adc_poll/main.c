#include "bflb_adc.h"
#include "bflb_mtimer.h"
#include "board.h"

struct bflb_device_s *adc;

#define TEST_ADC_CHANNEL_0  1
#define TEST_ADC_CHANNEL_1  1
#define TEST_ADC_CHANNEL_2  1
#define TEST_ADC_CHANNEL_3  1
#define TEST_ADC_CHANNEL_4  1
#define TEST_ADC_CHANNEL_5  1
#define TEST_ADC_CHANNEL_6  1
#define TEST_ADC_CHANNEL_7  1
#define TEST_ADC_CHANNEL_8  1
#define TEST_ADC_CHANNEL_9  1
#define TEST_ADC_CHANNEL_10 1

#define TEST_ADC_CHANNELS   (TEST_ADC_CHANNEL_0 + \
                           TEST_ADC_CHANNEL_1 +   \
                           TEST_ADC_CHANNEL_2 +   \
                           TEST_ADC_CHANNEL_3 +   \
                           TEST_ADC_CHANNEL_4 +   \
                           TEST_ADC_CHANNEL_5 +   \
                           TEST_ADC_CHANNEL_6 +   \
                           TEST_ADC_CHANNEL_7 +   \
                           TEST_ADC_CHANNEL_8 +   \
                           TEST_ADC_CHANNEL_9 +   \
                           TEST_ADC_CHANNEL_10)

struct bflb_adc_channel_s chan[] = {
#if TEST_ADC_CHANNEL_0
    { .pos_chan = ADC_CHANNEL_0,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_1
    { .pos_chan = ADC_CHANNEL_1,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_2
    { .pos_chan = ADC_CHANNEL_2,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_3
    { .pos_chan = ADC_CHANNEL_3,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_4
    { .pos_chan = ADC_CHANNEL_4,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_5
    { .pos_chan = ADC_CHANNEL_5,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_6
    { .pos_chan = ADC_CHANNEL_6,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_7
    { .pos_chan = ADC_CHANNEL_7,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_8
    { .pos_chan = ADC_CHANNEL_8,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_9
    { .pos_chan = ADC_CHANNEL_9,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_10
    { .pos_chan = ADC_CHANNEL_10,
      .neg_chan = ADC_CHANNEL_GND },
#endif
};

#define TEST_COUNT (16 + 10) /* must drop 10 counts */

uint32_t raw_data[TEST_COUNT];
struct bflb_adc_result_s result[TEST_COUNT];

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc = bflb_device_get_by_name("adc");

    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false; /* do not support scan mode */
    cfg.continuous_conv_mode = true; /* do not support single mode */
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    bflb_adc_init(adc, &cfg);

    for (size_t i = 0; i < TEST_ADC_CHANNELS; i++) {
        printf("ch :%d\r\n", i);

        memset(raw_data, 0, sizeof(raw_data));

        bflb_adc_feature_control(adc, ADC_CMD_CLR_FIFO, 0);
        bflb_adc_channel_config(adc, &chan[i], 1);
        bflb_adc_start_conversion(adc);

        for (uint16_t j = 0; j < TEST_COUNT; j++) {
            while (bflb_adc_get_count(adc) == 0) {
                bflb_mtimer_delay_ms(1);
            }

            raw_data[j] = bflb_adc_read_raw(adc);
        }

        bflb_adc_stop_conversion(adc);
        bflb_adc_parse_result(adc, raw_data, result, TEST_COUNT);
        for (size_t k = 10; k < TEST_COUNT; k++) {
            printf("raw data:%08x\r\n", raw_data[k]);
            printf("pos chan %d,%d mv \r\n", result[k].pos_chan, result[k].millivolt);
        }
    }
  
    bflb_adc_deinit(adc);

    while (1) {
    }
}
