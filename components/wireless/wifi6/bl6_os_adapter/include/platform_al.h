/**
 * @file platform_al.h
 * @brief
 *
 * Copyright (c) 2023 Bouffalolab team
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

#ifndef PLATFORM_AL_H_
#define PLATFORM_AL_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "timers.h"

#ifndef BOUFFALO_SDK
#include <bl_lp.h>
#else
enum PSM_EVENT{
    PSM_EVENT_SCAN = 0,
    PSM_EVENT_CONNECT,
    PSM_EVENT_PS,
    PSM_EVENT_APP,
};
#endif

typedef TimerHandle_t platform_event_handler_t;
typedef TimerCallbackFunction_t platform_event_func_t;

/**
 ****************************************************************************************
 * @brief Get MAC Address
 *
 * @param[out] mac Buffer of hold mac address.
 * @return 0 on success and OTHERS if error occurred.
 ****************************************************************************************
 */

int platform_get_mac(uint8_t *mac);

/**
****************************************************************************************
* @brief Wrapper of platform malloc/free
****************************************************************************************
*/
void *platform_malloc(uint32_t size);
void platform_free(void* mem_ptr);

/**
****************************************************************************************
* @brief Add and delete schedule event.
****************************************************************************************
*/
platform_event_handler_t platform_add_schedule_event(platform_event_func_t *handler, void *arg, uint32_t ms, uint8_t repeat);
int platform_delete_schedule_event(platform_event_func_t *handler);

/**
****************************************************************************************
* @brief Post Event to upper layer
*
* @param[in] catalogue Type of event.
* @param[in] code Code of event.
****************************************************************************************
*/

void platform_post_event(int catalogue, int code, ...);

#define PLATFORM_HOOK(x, ...) if( &platform_hook_##x ) {platform_hook_##x(__VA_ARGS__);}
/**
****************************************************************************************
* @brief hook for receive beacon
****************************************************************************************
*/
void platform_hook_beacon(uint32_t rhd,uint32_t tim_offset) __attribute__((weak));

/**
****************************************************************************************
* @brief hook for prevent sleep
****************************************************************************************
*/
void platform_hook_prevent_sleep(enum PSM_EVENT event, uint8_t prevent) __attribute__((weak));

#endif
