/**
 * @file adckey.c
 * @brief ADC key component
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "adckey.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bflb_core.h"
#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_name.h"

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include <task.h>
#endif

#if defined(BL616CL)
#include "bflb_adc_v2.h"
#elif defined(BL618DG)
#include "bflb_adc_v3.h"
#else
#include "bflb_adc.h"
#endif

#define ADCKEY_INVALID_KEY_INDEX       (0xFFu)
#define ADCKEY_SAMPLE_COUNT_DEFAULT    (4u)
#define ADCKEY_DEBOUNCE_COUNT_DEFAULT  (2u)
#define ADCKEY_WARMUP_SAMPLES          (2u)
#define ADCKEY_ADC_WAIT_TIMEOUT_MS     (50u)
#define ADCKEY_TASK_STACK_SIZE_DEFAULT (1024u)
#define ADCKEY_TASK_PRIORITY_DEFAULT   (5u)
#define ADCKEY_SCAN_CHANNELS_MAX       (16u)

#if defined(CONFIG_ADCKEY_DEFAULT_LOW)
#define ADCKEY_IDLE_MV (0u)
#else
#define ADCKEY_IDLE_MV (3200u)
#endif

struct adckey_key_state_s {
    uint8_t stable_key_index;
    uint8_t pending_key_index;
    uint8_t pending_count;
    bool long_press_reported;
    uint16_t active_sampled_mv;
    uint64_t press_start_ms;
};

struct adckey_item_state_s {
    uint64_t next_sample_ms;
    bool due_this_round;
    struct adckey_key_state_s key_state;
};

struct adckey_handle_s {
    adckey_config_t config;
    struct bflb_device_s *adc_dev;
    struct bflb_device_s *gpio_dev;
    struct adckey_item_state_s *item_states;
    struct bflb_adc_channel_s *scan_channels;
    uint16_t *sample_cache_mv;
    uint8_t current_channel;
    uint8_t scan_channel_num;
    bool current_internal;
    bool channel_valid;
    bool initialized;
    bool running;
    bool use_scan;
#if defined(CONFIG_FREERTOS)
    TaskHandle_t task_handle;
#endif
};

#if defined(BL618DG)
static bool g_adckey_adc_common_inited;
#endif

static const char *adckey_get_default_adc_name(void)
{
#if defined(BL616CL)
    return BFLB_NAME_ADC_V2_0;
#elif defined(BL618DG)
    return BFLB_NAME_ADC_V3_0;
#else
    return BFLB_NAME_ADC0;
#endif
}

static void adckey_key_state_reset(struct adckey_key_state_s *state)
{
    if (state == NULL) {
        return;
    }

    state->stable_key_index = ADCKEY_INVALID_KEY_INDEX;
    state->pending_key_index = ADCKEY_INVALID_KEY_INDEX;
    state->pending_count = 0u;
    state->long_press_reported = false;
    state->active_sampled_mv = 0u;
    state->press_start_ms = 0u;
}

static bool adckey_key_target_order_valid(const adckey_key_item_cfg_t *key_cfg)
{
    uint8_t index;

    if ((key_cfg == NULL) || (key_cfg->target_mv == NULL) || (key_cfg->key_num == 0u)) {
        return false;
    }

#if defined(CONFIG_ADCKEY_DEFAULT_LOW)
    if (key_cfg->target_mv[0] <= ADCKEY_IDLE_MV) {
        return false;
    }

    for (index = 1u; index < key_cfg->key_num; index++) {
        if (key_cfg->target_mv[index] <= key_cfg->target_mv[index - 1u]) {
            return false;
        }
    }
#else
    if (key_cfg->target_mv[0] >= ADCKEY_IDLE_MV) {
        return false;
    }

    for (index = 1u; index < key_cfg->key_num; index++) {
        if (key_cfg->target_mv[index] >= key_cfg->target_mv[index - 1u]) {
            return false;
        }
    }
#endif

    return true;
}

static int adckey_validate_item_config(const adckey_item_config_t *item)
{
    if (item == NULL) {
        return -1;
    }
    if (item->sample_period_ms == 0u) {
        return -1;
    }

    switch (item->type) {
        case ADCKEY_ITEM_TYPE_KEY:
            if (item->gpio_pin == ADCKEY_GPIO_UNUSED) {
                return -1;
            }
            if ((item->config.key.callback == NULL) || (!adckey_key_target_order_valid(&item->config.key))) {
                return -1;
            }
            return 0;

        case ADCKEY_ITEM_TYPE_ADC:
            if (item->config.adc.callback == NULL) {
                return -1;
            }
            return 0;

        default:
            return -1;
    }
}

static int adckey_validate_config(const adckey_config_t *config)
{
    uint8_t index;

    if ((config == NULL) || (config->items == NULL) || (config->item_num == 0u)) {
        return -1;
    }

    for (index = 0u; index < config->item_num; index++) {
        if (adckey_validate_item_config(&config->items[index]) != 0) {
            return -1;
        }
    }

    return 0;
}

static uint16_t adckey_clamp_mv(int32_t mv)
{
    if (mv <= 0) {
        return 0u;
    }
    if (mv >= 0xFFFF) {
        return 0xFFFFu;
    }

    return (uint16_t)mv;
}

static void adckey_adc_fill_default_config(struct bflb_adc_config_s *adc_cfg, bool use_scan)
{
    if (adc_cfg == NULL) {
        return;
    }

#if defined(BL616CL)
    adc_cfg->scan_conv_mode = use_scan;
    adc_cfg->continuous_conv_mode = use_scan ? false : true;
    adc_cfg->differential_mode = false;
    adc_cfg->resolution = ADC_RESOLUTION_12B;
    adc_cfg->vref = ADC_VREF_INTERNAL_1P25;
#elif defined(BL618DG)
    adc_cfg->scan_conv_mode = use_scan;
    adc_cfg->continuous_conv_mode = use_scan ? false : true;
    adc_cfg->differential_mode = false;
    adc_cfg->resolution = ADC_RESOLUTION_12B;
    adc_cfg->vref = ADC_VREF_INTERNAL_1P25;
#else
    adc_cfg->clk_div = ADC_CLK_DIV_32;
    adc_cfg->scan_conv_mode = use_scan;
    adc_cfg->continuous_conv_mode = use_scan ? false : true;
    adc_cfg->differential_mode = false;
    adc_cfg->resolution = ADC_RESOLUTION_16B;
    adc_cfg->vref = ADC_VREF_3P2V;
#endif
}

static void adckey_adc_clear_fifo(struct bflb_device_s *adc_dev)
{
#if defined(BL616CL) || defined(BL618DG)
    bflb_adc_clear_fifo(adc_dev);
#else
    (void)bflb_adc_feature_control(adc_dev, ADC_CMD_CLR_FIFO, 0);
#endif
}

#if defined(BL618DG)
static void adckey_adc_common_init(void)
{
    struct bflb_adc_common_config_s common_cfg;

    if (g_adckey_adc_common_inited) {
        return;
    }

    common_cfg.clk_div = ADC_CLK_DIV_20;
    common_cfg.mode = ADC_MODE_SEPARATE;
    common_cfg.fifo1_enable = true;
    bflb_adc_common_init(&common_cfg);
    g_adckey_adc_common_inited = true;
}
#endif

static bool adckey_item_uses_internal_channel(const adckey_item_config_t *item)
{
#if defined(BL616CL) || defined(BL618DG)
    if ((item != NULL) && (item->gpio_pin == ADCKEY_GPIO_UNUSED)) {
        return true;
    }
#else
    (void)item;
#endif

    return false;
}

static uint8_t adckey_get_scan_channel_limit(void)
{
#if defined(BL618DG)
    return 16u;
#else
    return 12u;
#endif
}

static bool adckey_can_use_scan(const adckey_config_t *config)
{
    uint8_t index;

    if ((config == NULL) || (config->item_num <= 1u) || (config->item_num > adckey_get_scan_channel_limit())) {
        return false;
    }

#if defined(BL616CL) || defined(BL618DG)
    for (index = 0u; index < config->item_num; index++) {
        if (adckey_item_uses_internal_channel(&config->items[index])) {
            return false;
        }
    }
#else
    (void)index;
#endif

    return true;
}

static bool adckey_item_requires_vbat_enable(const adckey_item_config_t *item)
{
    if (item == NULL) {
        return false;
    }

#if defined(BL616CL)
    return item->adc_channel == ADC_INTERNAL_CHANNEL_VBAT_HALF;
#elif defined(BL618DG)
    return item->adc_channel == ADC0_INTERNAL_CHANNEL_VBAT_HALF;
#else
    return item->adc_channel == ADC_CHANNEL_VABT_HALF;
#endif
}

static void adckey_adc_update_vbat_feature(adckey_handle_t handle, bool enable)
{
    uint8_t index;

    if ((handle == NULL) || (handle->adc_dev == NULL)) {
        return;
    }

    for (index = 0u; index < handle->config.item_num; index++) {
        if (adckey_item_requires_vbat_enable(&handle->config.items[index])) {
            (void)bflb_adc_feature_control(handle->adc_dev, ADC_CMD_VBAT_EN, enable);
            break;
        }
    }
}

static int adckey_adc_configure_scan_channels(adckey_handle_t handle)
{
    uint8_t index;

    if ((handle == NULL) || (!handle->use_scan) || (handle->scan_channels == NULL)) {
        return -1;
    }

    for (index = 0u; index < handle->config.item_num; index++) {
        handle->scan_channels[index].pos_chan = handle->config.items[index].adc_channel;
#if defined(BL616CL) || defined(BL618DG)
        handle->scan_channels[index].neg_chan = ADC_EXTERNAL_CHANNEL_NULL;
#else
        handle->scan_channels[index].neg_chan = ADC_CHANNEL_GND;
#endif
    }

#if defined(BL616CL)
    return bflb_adc_channel_config_external(handle->adc_dev, handle->scan_channels, handle->scan_channel_num);
#elif defined(BL618DG)
    return bflb_adc_channel_config_external(handle->adc_dev, handle->scan_channels, handle->scan_channel_num);
#else
    return bflb_adc_channel_config(handle->adc_dev, handle->scan_channels, handle->scan_channel_num);
#endif
}

static int adckey_adc_select_channel(adckey_handle_t handle, const adckey_item_config_t *item)
{
    struct bflb_adc_channel_s channel_cfg;
    bool use_internal;

    if ((handle == NULL) || (handle->adc_dev == NULL) || (item == NULL)) {
        return -1;
    }

    use_internal = adckey_item_uses_internal_channel(item);
    if (handle->channel_valid && (handle->current_channel == item->adc_channel) && (handle->current_internal == use_internal)) {
        return 0;
    }

    bflb_adc_stop_conversion(handle->adc_dev);
    adckey_adc_clear_fifo(handle->adc_dev);

    memset(&channel_cfg, 0, sizeof(channel_cfg));
    channel_cfg.pos_chan = item->adc_channel;
#if defined(BL616CL)
    channel_cfg.neg_chan = use_internal ? ADC_INTERNAL_CHANNEL_NULL : ADC_EXTERNAL_CHANNEL_NULL;
    if (use_internal) {
        if (bflb_adc_channel_config_internal(handle->adc_dev, &channel_cfg, 1u) != 0) {
            return -1;
        }
    } else if (bflb_adc_channel_config_external(handle->adc_dev, &channel_cfg, 1u) != 0) {
        return -1;
    }
#elif defined(BL618DG)
    channel_cfg.neg_chan = use_internal ? ADC0_INTERNAL_CHANNEL_NULL : ADC_EXTERNAL_CHANNEL_NULL;
    if (use_internal) {
        if (bflb_adc_channel_config_internal(handle->adc_dev, &channel_cfg, 1u) != 0) {
            return -1;
        }
    } else if (bflb_adc_channel_config_external(handle->adc_dev, &channel_cfg, 1u) != 0) {
        return -1;
    }
#else
    channel_cfg.neg_chan = ADC_CHANNEL_GND;
    if (bflb_adc_channel_config(handle->adc_dev, &channel_cfg, 1u) != 0) {
        return -1;
    }
#endif

    handle->current_channel = item->adc_channel;
    handle->current_internal = use_internal;
    handle->channel_valid = true;
    return 0;
}

static int adckey_adc_wait_count(adckey_handle_t handle, uint8_t target_count)
{
    uint64_t start_ms;

    if ((handle == NULL) || (handle->adc_dev == NULL)) {
        return -1;
    }

    start_ms = bflb_mtimer_get_time_ms();
    while (bflb_adc_get_count(handle->adc_dev) < target_count) {
        if ((bflb_mtimer_get_time_ms() - start_ms) >= ADCKEY_ADC_WAIT_TIMEOUT_MS) {
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return 0;
}

static int adckey_adc_wait_ready(adckey_handle_t handle)
{
    uint64_t start_ms;

    if ((handle == NULL) || (handle->adc_dev == NULL)) {
        return -1;
    }

    start_ms = bflb_mtimer_get_time_ms();
    while (bflb_adc_get_count(handle->adc_dev) == 0u) {
        if ((bflb_mtimer_get_time_ms() - start_ms) >= ADCKEY_ADC_WAIT_TIMEOUT_MS) {
            return -1;
        }
        bflb_mtimer_delay_us(2);
    }

    return 0;
}

static int adckey_adc_scan_read_once(adckey_handle_t handle, uint16_t *sampled_mv)
{
    uint8_t index;
    uint32_t raw_data[ADCKEY_SCAN_CHANNELS_MAX];
    struct bflb_adc_result_s result[ADCKEY_SCAN_CHANNELS_MAX];

    if ((handle == NULL) || (sampled_mv == NULL) || (!handle->use_scan) || (handle->scan_channel_num == 0u) ||
        (handle->scan_channel_num > ADCKEY_SCAN_CHANNELS_MAX)) {
        return -1;
    }

    adckey_adc_clear_fifo(handle->adc_dev);
    bflb_adc_start_conversion(handle->adc_dev);

    if (adckey_adc_wait_count(handle, handle->scan_channel_num) != 0) {
        bflb_adc_stop_conversion(handle->adc_dev);
        adckey_adc_clear_fifo(handle->adc_dev);
        return -1;
    }

    bflb_adc_stop_conversion(handle->adc_dev);

    for (index = 0u; index < handle->scan_channel_num; index++) {
        if (bflb_adc_get_count(handle->adc_dev) == 0u) {
            adckey_adc_clear_fifo(handle->adc_dev);
            return -1;
        }
        raw_data[index] = bflb_adc_read_raw(handle->adc_dev);
    }

    bflb_adc_parse_result(handle->adc_dev, raw_data, result, handle->scan_channel_num);
    for (index = 0u; index < handle->scan_channel_num; index++) {
        sampled_mv[index] = adckey_clamp_mv(result[index].millivolt);
    }

    adckey_adc_clear_fifo(handle->adc_dev);
    return 0;
}

static int adckey_adc_scan_sample_mv(adckey_handle_t handle, uint16_t *sampled_mv)
{
    uint8_t index;
    uint8_t pass;
    int64_t sum_mv[ADCKEY_SCAN_CHANNELS_MAX];
    uint16_t pass_mv[ADCKEY_SCAN_CHANNELS_MAX];

    if ((handle == NULL) || (sampled_mv == NULL) || (!handle->use_scan) || (handle->scan_channel_num == 0u) ||
        (handle->scan_channel_num > ADCKEY_SCAN_CHANNELS_MAX)) {
        return -1;
    }

    memset(sum_mv, 0, sizeof(sum_mv));
    for (pass = 0u; pass < (uint8_t)(ADCKEY_WARMUP_SAMPLES + ADCKEY_SAMPLE_COUNT_DEFAULT); pass++) {
        if (adckey_adc_scan_read_once(handle, pass_mv) != 0) {
            return -1;
        }

        if (pass < ADCKEY_WARMUP_SAMPLES) {
            continue;
        }

        for (index = 0u; index < handle->scan_channel_num; index++) {
            sum_mv[index] += pass_mv[index];
        }
    }

    for (index = 0u; index < handle->scan_channel_num; index++) {
        sampled_mv[index] = adckey_clamp_mv((int32_t)(sum_mv[index] / ADCKEY_SAMPLE_COUNT_DEFAULT));
    }

    return 0;
}

static int adckey_adc_sample_mv(adckey_handle_t handle, const adckey_item_config_t *item, uint16_t *sampled_mv)
{
    uint8_t index;
    int64_t sum_mv;

    if ((handle == NULL) || (item == NULL) || (sampled_mv == NULL)) {
        return -1;
    }

    if (adckey_adc_select_channel(handle, item) != 0) {
        return -1;
    }

    sum_mv = 0;
    adckey_adc_clear_fifo(handle->adc_dev);
    bflb_adc_start_conversion(handle->adc_dev);

    for (index = 0u; index < (uint8_t)(ADCKEY_WARMUP_SAMPLES + ADCKEY_SAMPLE_COUNT_DEFAULT); index++) {
        uint32_t raw_data;
        struct bflb_adc_result_s result;

        if (adckey_adc_wait_ready(handle) != 0) {
            bflb_adc_stop_conversion(handle->adc_dev);
            adckey_adc_clear_fifo(handle->adc_dev);
            return -1;
        }

        raw_data = bflb_adc_read_raw(handle->adc_dev);
        if (index < ADCKEY_WARMUP_SAMPLES) {
            continue;
        }

        bflb_adc_parse_result(handle->adc_dev, &raw_data, &result, 1u);
        sum_mv += adckey_clamp_mv(result.millivolt);
    }

    bflb_adc_stop_conversion(handle->adc_dev);
    adckey_adc_clear_fifo(handle->adc_dev);

    *sampled_mv = adckey_clamp_mv((int32_t)(sum_mv / ADCKEY_SAMPLE_COUNT_DEFAULT));
    return 0;
}

static int adckey_adc_hw_init(adckey_handle_t handle)
{
    struct bflb_adc_config_s adc_cfg;
    uint8_t index;

    if (handle == NULL) {
        return -1;
    }

#if defined(BL618DG)
    adckey_adc_common_init();
#endif

    handle->gpio_dev = bflb_device_get_by_name(BFLB_NAME_GPIO);
    handle->adc_dev = bflb_device_get_by_name(adckey_get_default_adc_name());
    if ((handle->gpio_dev == NULL) || (handle->adc_dev == NULL)) {
        return -1;
    }

    for (index = 0u; index < handle->config.item_num; index++) {
        if (handle->config.items[index].gpio_pin != ADCKEY_GPIO_UNUSED) {
            bflb_gpio_init(handle->gpio_dev,
                           handle->config.items[index].gpio_pin,
                           GPIO_ANALOG | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0);
        }
    }

    memset(&adc_cfg, 0, sizeof(adc_cfg));
    adckey_adc_fill_default_config(&adc_cfg, handle->use_scan);
    bflb_adc_init(handle->adc_dev, &adc_cfg);
    if (handle->use_scan && (adckey_adc_configure_scan_channels(handle) != 0)) {
        bflb_adc_deinit(handle->adc_dev);
        return -1;
    }
    adckey_adc_update_vbat_feature(handle, true);
    adckey_adc_clear_fifo(handle->adc_dev);
    return 0;
}

static void adckey_adc_hw_deinit(adckey_handle_t handle)
{
    if ((handle == NULL) || (handle->adc_dev == NULL)) {
        return;
    }

    bflb_adc_stop_conversion(handle->adc_dev);
    adckey_adc_clear_fifo(handle->adc_dev);
    adckey_adc_update_vbat_feature(handle, false);
    bflb_adc_deinit(handle->adc_dev);
}

static uint8_t adckey_find_key_index(const adckey_key_item_cfg_t *key_cfg, uint16_t sampled_mv)
{
    uint32_t threshold_mv;
    uint8_t index;

    if ((key_cfg == NULL) || (key_cfg->target_mv == NULL) || (key_cfg->key_num == 0u)) {
        return ADCKEY_INVALID_KEY_INDEX;
    }

    threshold_mv = ((uint32_t)ADCKEY_IDLE_MV + key_cfg->target_mv[0]) / 2u;

#if defined(CONFIG_ADCKEY_DEFAULT_LOW)
    if (sampled_mv <= threshold_mv) {
        return ADCKEY_INVALID_KEY_INDEX;
    }

    for (index = 0u; index < (uint8_t)(key_cfg->key_num - 1u); index++) {
        threshold_mv = ((uint32_t)key_cfg->target_mv[index] + key_cfg->target_mv[index + 1u]) / 2u;
        if (sampled_mv <= threshold_mv) {
            return index;
        }
    }
#else
    if (sampled_mv >= threshold_mv) {
        return ADCKEY_INVALID_KEY_INDEX;
    }

    for (index = 0u; index < (uint8_t)(key_cfg->key_num - 1u); index++) {
        threshold_mv = ((uint32_t)key_cfg->target_mv[index] + key_cfg->target_mv[index + 1u]) / 2u;
        if (sampled_mv >= threshold_mv) {
            return index;
        }
    }
#endif

    return (uint8_t)(key_cfg->key_num - 1u);
}

static void adckey_emit_key_event(adckey_handle_t handle,
                                  uint8_t item_index,
                                  uint8_t key_index,
                                  adckey_event_e event,
                                  uint16_t sampled_mv)
{
    adckey_event_data_t event_data;
    const adckey_item_config_t *item;

    if ((handle == NULL) || (item_index >= handle->config.item_num)) {
        return;
    }

    item = &handle->config.items[item_index];
    if ((item->type != ADCKEY_ITEM_TYPE_KEY) || (item->config.key.callback == NULL)) {
        return;
    }
    if ((key_index == ADCKEY_INVALID_KEY_INDEX) || (key_index >= item->config.key.key_num)) {
        return;
    }

    event_data.item_id = item_index;
    event_data.key_id = key_index;
    event_data.event = event;
    event_data.sampled_mv = sampled_mv;
    item->config.key.callback(&event_data);
}

static void adckey_emit_adc_data(adckey_handle_t handle, uint8_t item_index, uint16_t sampled_mv)
{
    adckey_adc_data_t adc_data;
    const adckey_item_config_t *item;

    if ((handle == NULL) || (item_index >= handle->config.item_num)) {
        return;
    }

    item = &handle->config.items[item_index];
    if ((item->type != ADCKEY_ITEM_TYPE_ADC) || (item->config.adc.callback == NULL)) {
        return;
    }

    adc_data.item_id = item_index;
    adc_data.sampled_mv = sampled_mv;
    item->config.adc.callback(&adc_data);
}

static void adckey_confirm_key_transition(adckey_handle_t handle,
                                          uint8_t item_index,
                                          uint8_t new_key_index,
                                          uint16_t sampled_mv,
                                          uint64_t now_ms)
{
    struct adckey_key_state_s *state;
    uint8_t old_key_index;

    state = &handle->item_states[item_index].key_state;
    old_key_index = state->stable_key_index;

    state->stable_key_index = new_key_index;
    state->pending_key_index = ADCKEY_INVALID_KEY_INDEX;
    state->pending_count = 0u;

    if (old_key_index != ADCKEY_INVALID_KEY_INDEX) {
        adckey_emit_key_event(handle, item_index, old_key_index, ADCKEY_EVENT_RELEASE, state->active_sampled_mv);
    }

    if (new_key_index != ADCKEY_INVALID_KEY_INDEX) {
        state->active_sampled_mv = sampled_mv;
        state->press_start_ms = now_ms;
        state->long_press_reported = false;
#if defined(CONFIG_ADCKEY_SUPPORT_PRESS)
        adckey_emit_key_event(handle, item_index, new_key_index, ADCKEY_EVENT_PRESS, sampled_mv);
#endif
    } else {
        state->active_sampled_mv = 0u;
        state->press_start_ms = 0u;
        state->long_press_reported = false;
    }
}

static void adckey_process_key_item(adckey_handle_t handle, uint8_t item_index, uint16_t sampled_mv, uint64_t now_ms)
{
    const adckey_item_config_t *item;
    struct adckey_key_state_s *state;
    uint8_t detected_key_index;

    item = &handle->config.items[item_index];
    state = &handle->item_states[item_index].key_state;
    detected_key_index = adckey_find_key_index(&item->config.key, sampled_mv);

    if (detected_key_index == state->stable_key_index) {
        state->pending_key_index = ADCKEY_INVALID_KEY_INDEX;
        state->pending_count = 0u;

        if (detected_key_index != ADCKEY_INVALID_KEY_INDEX) {
            state->active_sampled_mv = sampled_mv;

            if ((!state->long_press_reported) &&
                (item->config.key.long_press_ms > 0u) &&
                ((now_ms - state->press_start_ms) >= item->config.key.long_press_ms)) {
                state->long_press_reported = true;
                adckey_emit_key_event(handle, item_index, detected_key_index, ADCKEY_EVENT_LONG_PRESS, sampled_mv);
            }
        }

        return;
    }

    if (detected_key_index != state->pending_key_index) {
        state->pending_key_index = detected_key_index;
        state->pending_count = 1u;
        return;
    }

    if (state->pending_count < 0xFFu) {
        state->pending_count++;
    }

    if (state->pending_count < ADCKEY_DEBOUNCE_COUNT_DEFAULT) {
        return;
    }

    adckey_confirm_key_transition(handle, item_index, detected_key_index, sampled_mv, now_ms);
}

static int adckey_process_due_items(adckey_handle_t handle)
{
    uint8_t index;
    uint64_t sample_now_ms;
    int status;
    bool any_due;

    status = 0;
    any_due = false;

    if ((handle == NULL) || (!handle->initialized)) {
        return -1;
    }

    sample_now_ms = bflb_mtimer_get_time_ms();
    for (index = 0u; index < handle->config.item_num; index++) {
        const adckey_item_config_t *item;
        struct adckey_item_state_s *state;

        item = &handle->config.items[index];
        state = &handle->item_states[index];
        state->due_this_round = false;

        if ((state->next_sample_ms != 0u) && (sample_now_ms < state->next_sample_ms)) {
            continue;
        }

        state->next_sample_ms = sample_now_ms + item->sample_period_ms;
        state->due_this_round = true;
        any_due = true;
    }

    if (!any_due) {
        return 0;
    }

    if (handle->use_scan) {
        if (adckey_adc_scan_sample_mv(handle, handle->sample_cache_mv) != 0) {
            return -1;
        }
    } else {
        for (index = 0u; index < handle->config.item_num; index++) {
            const adckey_item_config_t *item;
            struct adckey_item_state_s *state;

            item = &handle->config.items[index];
            state = &handle->item_states[index];
            if (!state->due_this_round) {
                continue;
            }

            if (adckey_adc_sample_mv(handle, item, &handle->sample_cache_mv[index]) != 0) {
                state->due_this_round = false;
                status = -1;
            }
        }
    }

    for (index = 0u; index < handle->config.item_num; index++) {
        const adckey_item_config_t *item;
        struct adckey_item_state_s *state;

        item = &handle->config.items[index];
        state = &handle->item_states[index];
        if (!state->due_this_round) {
            continue;
        }

        state->due_this_round = false;
        if (item->type == ADCKEY_ITEM_TYPE_KEY) {
            adckey_process_key_item(handle, index, handle->sample_cache_mv[index], sample_now_ms);
        } else {
            adckey_emit_adc_data(handle, index, handle->sample_cache_mv[index]);
        }
    }

    return status;
}

static uint64_t adckey_get_next_due_ms(adckey_handle_t handle, uint64_t now_ms)
{
    uint8_t index;
    uint64_t next_due_ms;

    if ((handle == NULL) || (handle->config.item_num == 0u)) {
        return now_ms + 1u;
    }

    next_due_ms = 0u;
    for (index = 0u; index < handle->config.item_num; index++) {
        uint64_t item_next_ms;

        item_next_ms = handle->item_states[index].next_sample_ms;
        if (item_next_ms == 0u) {
            return now_ms;
        }
        if ((next_due_ms == 0u) || (item_next_ms < next_due_ms)) {
            next_due_ms = item_next_ms;
        }
    }

    return next_due_ms;
}

#if defined(CONFIG_FREERTOS)
static void adckey_task_entry(void *argument)
{
    adckey_handle_t handle;

    handle = (adckey_handle_t)argument;

    while (1) {
        uint64_t current_ms;
        uint64_t next_due_ms;
        TickType_t delay_ticks;

        (void)adckey_process_due_items(handle);

        current_ms = bflb_mtimer_get_time_ms();
        next_due_ms = adckey_get_next_due_ms(handle, current_ms);
        if (next_due_ms <= current_ms) {
            delay_ticks = 1u;
        } else {
            delay_ticks = pdMS_TO_TICKS((uint32_t)(next_due_ms - current_ms));
            if (delay_ticks == 0u) {
                delay_ticks = 1u;
            }
        }

        vTaskDelay(delay_ticks);
    }
}
#endif

int adckey_init(const adckey_config_t *config, adckey_handle_t *handle)
{
    adckey_handle_t new_handle;
    uint8_t index;

    if ((handle == NULL) || (adckey_validate_config(config) != 0)) {
        return -1;
    }

    new_handle = (adckey_handle_t)calloc(1u, sizeof(*new_handle));
    if (new_handle == NULL) {
        return -1;
    }

    new_handle->sample_cache_mv = (uint16_t *)calloc(config->item_num, sizeof(*new_handle->sample_cache_mv));
    if (new_handle->sample_cache_mv == NULL) {
        free(new_handle);
        return -1;
    }

    new_handle->item_states = (struct adckey_item_state_s *)calloc(config->item_num, sizeof(*new_handle->item_states));
    if (new_handle->item_states == NULL) {
        free(new_handle->sample_cache_mv);
        free(new_handle);
        return -1;
    }

    new_handle->config = *config;
    new_handle->use_scan = adckey_can_use_scan(config);
    if (new_handle->use_scan) {
        new_handle->scan_channel_num = config->item_num;
        new_handle->scan_channels =
            (struct bflb_adc_channel_s *)calloc(new_handle->scan_channel_num, sizeof(*new_handle->scan_channels));
        if (new_handle->scan_channels == NULL) {
            free(new_handle->item_states);
            free(new_handle->sample_cache_mv);
            free(new_handle);
            return -1;
        }
    }
#if defined(CONFIG_FREERTOS)
    if (new_handle->config.task_stack_size == 0u) {
        new_handle->config.task_stack_size = ADCKEY_TASK_STACK_SIZE_DEFAULT;
    }
    if (new_handle->config.task_priority == 0u) {
        new_handle->config.task_priority = ADCKEY_TASK_PRIORITY_DEFAULT;
    }
#endif

    for (index = 0u; index < config->item_num; index++) {
        new_handle->item_states[index].due_this_round = false;
        adckey_key_state_reset(&new_handle->item_states[index].key_state);
        new_handle->item_states[index].next_sample_ms = 0u;
    }

    if (adckey_adc_hw_init(new_handle) != 0) {
        free(new_handle->scan_channels);
        free(new_handle->item_states);
        free(new_handle->sample_cache_mv);
        free(new_handle);
        return -1;
    }

    new_handle->initialized = true;
    *handle = new_handle;
    return 0;
}

int adckey_start(adckey_handle_t handle)
{
    uint8_t index;

    if ((handle == NULL) || (!handle->initialized)) {
        return -1;
    }
    if (handle->running) {
        return 0;
    }

    for (index = 0u; index < handle->config.item_num; index++) {
        handle->item_states[index].due_this_round = false;
        handle->item_states[index].next_sample_ms = 0u;
        adckey_key_state_reset(&handle->item_states[index].key_state);
    }

    handle->running = true;

#if defined(CONFIG_FREERTOS)
    if (xTaskCreate(adckey_task_entry,
                    "adckey",
                    handle->config.task_stack_size,
                    handle,
                    handle->config.task_priority,
                    &handle->task_handle) != pdPASS) {
        handle->running = false;
        handle->task_handle = NULL;
        return -1;
    }
#endif

    return 0;
}

int adckey_stop(adckey_handle_t handle)
{
    if (handle == NULL) {
        return -1;
    }

    handle->running = false;

#if defined(CONFIG_FREERTOS)
    if (handle->task_handle != NULL) {
        if (xTaskGetCurrentTaskHandle() == handle->task_handle) {
            handle->task_handle = NULL;
            vTaskDelete(NULL);
        } else {
            vTaskDelete(handle->task_handle);
            handle->task_handle = NULL;
        }
    }
#endif

    return 0;
}

int adckey_poll(adckey_handle_t handle)
{
    if ((handle == NULL) || (!handle->initialized)) {
        return -1;
    }
    if (!handle->running) {
        return 0;
    }

    return adckey_process_due_items(handle);
}

int adckey_deinit(adckey_handle_t handle)
{
    if (handle == NULL) {
        return -1;
    }

    (void)adckey_stop(handle);
    adckey_adc_hw_deinit(handle);

    free(handle->scan_channels);
    free(handle->item_states);
    free(handle->sample_cache_mv);
    free(handle);
    return 0;
}
