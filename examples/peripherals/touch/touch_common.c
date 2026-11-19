#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "bflb_gpio.h"
#include "bflb_irq.h"
#include "bflb_mtimer.h"
#include "touch_common.h"

int touch_gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    const uint8_t pins[] = { GPIO_PIN_6,  GPIO_PIN_7,  GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
                             GPIO_PIN_16, GPIO_PIN_17, GPIO_PIN_18, GPIO_PIN_19 };

    if (gpio == NULL) {
        return -ENODEV;
    }

    /* BL618DG: GPIO13 is C_EXT; the other pins are TOUCH0~TOUCH7. */
    for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        bflb_gpio_init(gpio, pins[i], GPIO_ANALOG | GPIO_FLOAT | GPIO_DRV_0);
    }

    return 0;
}

void touch_print_gpio_map(bool mutual_mode)
{
    printf("BL618DG touch GPIO map:\r\n");
    if (mutual_mode) {
        printf("  TX: CH0/GPIO6, CH1/GPIO7, CH2/GPIO14, CH3/GPIO15\r\n");
        printf("  RX: CH4/GPIO16, CH5/GPIO17, CH6/GPIO18, CH7/GPIO19\r\n");
    } else {
        printf("  keys: CH0/GPIO6, CH1/GPIO7, CH2/GPIO14, CH3/GPIO15\r\n");
        printf("        CH5/GPIO17, CH6/GPIO18, CH7/GPIO19\r\n");
        printf("  active shield: CH4/GPIO16\r\n");
    }
    printf("  C_EXT: GPIO13\r\n");
}

void touch_config_init(struct bflb_touch_v2_config_s *config, bool mutual_mode, bool calibration)
{
    uint16_t channel_mask = mutual_mode ? TOUCH_MUTUAL_CHANNEL_MASK : TOUCH_SELF_CAP_CHANNEL_MASK;

    memset(config, 0, sizeof(*config));

    /* Calibrate with one scan, then disable channel calibration for continuous operation. */
    config->cont_mode_en = !calibration;
    config->ldo_settle_cyc = TOUCH_V2_LDO_SETTLE_400;
    config->vref_sel = TOUCH_V2_VREF_0P6V;
    config->vldo_sel = TOUCH_V2_VLDO_1P8V;
    config->hys_sel = TOUCH_V2_HYS_22MV;
    config->clk_sel = TOUCH_V2_CLK_SEL_32M;
    config->touch_32m_clk_sel = true;
    config->clk_div = TOUCH_V2_CLK_DIV_16;

    config->filter_sel = TOUCH_V2_FILTER_FIR;
    config->filter_en = false;
    config->filter_order = TOUCH_V2_FILTER_ORDER_1;
    config->lta_sel = TOUCH_V2_FILTER_FIR;
    config->lta_en = true;
    config->lta_order = TOUCH_V2_LTA_ORDER_4;
    config->channel_cal_en = calibration;

    config->rx_channel_en = TOUCH_SELF_CAP_CHANNEL_MASK;
    config->as_channel_sel = (1U << 4);
    config->as_chl_sel = true;
    config->pcharge_low = TOUCH_V2_PCHARGE_LOW_255;
    config->pcharge_high = TOUCH_V2_PCHARGE_HIGH_8191;

    /* A set bit masks the corresponding interrupt. Poll scan completion during calibration. */
    config->det_int_mask = (uint16_t)(~channel_mask);
    config->scan_int_mask = calibration ? 0 : 1;
    config->det_on = true;
    config->hop_freq_th = 0x7F;
    config->hop_freq_div0 = 13;
    config->hop_freq_div1 = 10;
    config->hop_freq_div2 = 8;

    if (mutual_mode) {
        config->tx_channel_en = TOUCH_MUTUAL_TX_CHANNEL_MASK;
        config->rx_channel_en = TOUCH_MUTUAL_RX_CHANNEL_MASK;
        config->as_channel_sel = 0;
        config->as_chl_sel = false;
        config->vref_sel = TOUCH_V2_VREF_1P1V;
        config->vldo_sel = TOUCH_V2_VLDO_1P3V;
        config->pcharge_high = TOUCH_V2_PCHARGE_HIGH_4095;
    }
}

void touch_set_thresholds(struct bflb_device_s *touch, uint16_t channel_mask)
{
    for (uint8_t channel = 0; channel < TOUCH_V2_CHANNEL_MAX; channel++) {
        if (channel_mask & (1U << channel)) {
            bflb_touch_v2_set_channel_threshold(touch, channel, TOUCH_DETECT_THRESHOLD);
            bflb_touch_v2_set_channel_hysteresis(touch, channel, TOUCH_DETECT_HYSTERESIS);
        }
    }
}

static void touch_print_channel_data(uint8_t channel, const struct bflb_touch_v2_channel_data_s *data, const char *tag)
{
    /* Filtering is disabled in these examples, so the detector uses raw data. */
    int32_t diff = (int32_t)data->lta_data - (int32_t)data->raw_data;

    printf("[%s] CH%u raw=%5u flt=%5u lta=%5u vth=%3u hys=%3u diff=%5ld sw=%s\r\n", tag, channel, data->raw_data,
           data->flt_data, data->lta_data, data->vth_data, data->hys_data, (long)diff,
           (diff > 0 && diff >= (int32_t)data->vth_data) ? "PRESSED" : "idle");
}

void touch_dump_channels(struct bflb_device_s *touch, uint16_t channel_mask, const char *tag)
{
    struct bflb_touch_v2_channel_data_s data;

    for (uint8_t channel = 0; channel < TOUCH_V2_CHANNEL_MAX; channel++) {
        if ((channel_mask & (1U << channel)) == 0) {
            continue;
        }

        bflb_touch_v2_get_channel_data(touch, channel, &data);
        touch_print_channel_data(channel, &data, tag);
    }
}

void touch_irq_handler(int irq, void *arg)
{
    struct touch_irq_context_s *context = (struct touch_irq_context_s *)arg;
    uint32_t int_status = bflb_touch_v2_get_int_status(context->dev);
    uint16_t det_status = int_status & context->channel_mask;

    (void)irq;

    /* Capture data before acknowledging the interrupt; defer UART output to main. */
    for (uint8_t channel = 0; channel < TOUCH_V2_CHANNEL_MAX; channel++) {
        if (det_status & (1U << channel)) {
            bflb_touch_v2_get_channel_data(context->dev, channel, &context->channel_data[channel]);
        }
    }
    bflb_touch_v2_clear_int(context->dev, int_status);
    context->pending_status |= int_status;
}

void touch_process_events(struct touch_irq_context_s *context)
{
    struct bflb_touch_v2_channel_data_s channel_data[TOUCH_V2_CHANNEL_MAX];
    uint32_t int_status;
    uint16_t det_status;
    uintptr_t flags;

    /* Copy and consume the pending snapshot atomically with respect to the ISR. */
    flags = bflb_irq_save();
    int_status = context->pending_status;
    det_status = int_status & context->channel_mask;
    for (uint8_t channel = 0; channel < TOUCH_V2_CHANNEL_MAX; channel++) {
        if (det_status & (1U << channel)) {
            channel_data[channel] = context->channel_data[channel];
        }
    }
    context->pending_status = 0;
    bflb_irq_restore(flags);

    for (uint8_t channel = 0; channel < TOUCH_V2_CHANNEL_MAX; channel++) {
        if (det_status & (1U << channel)) {
            touch_print_channel_data(channel, &channel_data[channel], "DET");
        }
    }
    if (int_status & (TOUCH_V2_INT_HOP_FREQ_0_MASK | TOUCH_V2_INT_HOP_FREQ_1_MASK)) {
        printf("Touch frequency hopping threshold interrupt\r\n");
    }
}

static int touch_wait_scan_done(struct bflb_device_s *touch, uint32_t *status)
{
    uint64_t start_time = bflb_mtimer_get_time_ms();

    do {
        *status = bflb_touch_v2_get_int_status(touch);
        if (*status & TOUCH_V2_INT_SCAN_LAST_MASK) {
            return 0;
        }
    } while ((bflb_mtimer_get_time_ms() - start_time) < TOUCH_SCAN_TIMEOUT_MS);

    return -ETIMEDOUT;
}

int touch_calibrate(struct bflb_device_s *touch, bool mutual_mode)
{
    struct bflb_touch_v2_config_s config;
    uint16_t channel_mask = mutual_mode ? TOUCH_MUTUAL_CHANNEL_MASK : TOUCH_SELF_CAP_CHANNEL_MASK;
    uint32_t int_status = 0;
    int ret;

    printf("Release all touch keys for %u ms before calibration\r\n", TOUCH_IDLE_CAL_DELAY_MS);
    bflb_mtimer_delay_ms(TOUCH_IDLE_CAL_DELAY_MS);

    touch_config_init(&config, mutual_mode, true);
    ret = bflb_touch_v2_init(touch, &config);
    if (ret != 0) {
        return ret;
    }

    touch_set_thresholds(touch, channel_mask);
    bflb_touch_v2_clear_int(touch, TOUCH_INT_CLEAR_ALL);
    bflb_touch_v2_scan_start(touch);
    ret = touch_wait_scan_done(touch, &int_status);
    bflb_touch_v2_scan_stop(touch);
    bflb_touch_v2_clear_int(touch, TOUCH_INT_CLEAR_ALL);
    if (ret != 0) {
        printf("Touch calibration timeout, status=0x%08lx\r\n", (unsigned long)int_status);
        return ret;
    }

    printf("Touch calibration complete, baseline LTA:\r\n");
    touch_dump_channels(touch, channel_mask, "CAL");
    return 0;
}
