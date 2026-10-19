/**
 * @file adckey.h
 * @brief ADC key component
 *
 * Copyright (c) 2026 Bouffalolab team
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

#ifndef _ADCKEY_H_
#define _ADCKEY_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Set gpio_pin to this value when an ADC item uses an internal channel such as VBAT. */
#define ADCKEY_GPIO_UNUSED (0xFFFFu)

typedef enum {
    ADCKEY_ITEM_TYPE_KEY = 0,
    ADCKEY_ITEM_TYPE_ADC,
} adckey_item_type_e;

typedef enum {
#if defined(CONFIG_ADCKEY_SUPPORT_PRESS)
    ADCKEY_EVENT_PRESS = 0,
    ADCKEY_EVENT_LONG_PRESS,
    ADCKEY_EVENT_RELEASE,
#else
    ADCKEY_EVENT_LONG_PRESS = 0,
    ADCKEY_EVENT_RELEASE,
#endif
} adckey_event_e;

typedef struct {
    uint8_t item_id;
    uint8_t key_id;
    adckey_event_e event;
    uint16_t sampled_mv;
} adckey_event_data_t;

typedef struct {
    uint8_t item_id;
    uint16_t sampled_mv;
} adckey_adc_data_t;

typedef void (*adckey_key_callback_t)(const adckey_event_data_t *event_data);
typedef void (*adckey_adc_callback_t)(const adckey_adc_data_t *adc_data);

typedef struct {
    /* target_mv[] is not copied internally and must remain valid while the handle is alive. */
    const uint16_t *target_mv;
    uint8_t key_num;
    uint32_t long_press_ms;
    adckey_key_callback_t callback;
} adckey_key_item_cfg_t;

typedef struct {
    adckey_adc_callback_t callback;
} adckey_adc_item_cfg_t;

typedef struct {
    adckey_item_type_e type;
    uint16_t gpio_pin;
    uint8_t adc_channel;
    uint32_t sample_period_ms;
    union {
        adckey_key_item_cfg_t key;
        adckey_adc_item_cfg_t adc;
    } config;
} adckey_item_config_t;

typedef struct {
#if defined(CONFIG_FREERTOS)
    uint32_t task_stack_size;
    uint8_t task_priority;
#endif
    /* items[] is not copied internally and must remain valid while the handle is alive. */
    const adckey_item_config_t *items;
    uint8_t item_num;
} adckey_config_t;

typedef struct adckey_handle_s *adckey_handle_t;

int adckey_init(const adckey_config_t *config, adckey_handle_t *handle);
int adckey_start(adckey_handle_t handle);
int adckey_stop(adckey_handle_t handle);
int adckey_poll(adckey_handle_t handle);
int adckey_deinit(adckey_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif
