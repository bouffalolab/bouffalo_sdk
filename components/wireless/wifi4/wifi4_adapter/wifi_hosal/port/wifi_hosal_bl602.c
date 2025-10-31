/****************************************************************************
 * components/network/wifi_hosal/port/wifi_hosal_bl602.c
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
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "wifi_hosal.h"

#ifdef BL602_MATTER_SUPPORT
#include <hosal_adc.c>
#endif

/****************************************************************************
 * Definition
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

int platform_get_mac(uint8_t mac[6])
{
    if (0 == bflb_efuse_read_mac_address_opt(0, mac, 1)) {
        return 0;
    }
    return -1;
}

int platform_pds_rf_turnon(void *arg)
{
    AON_LowPower_Exit_PDS0();
    return 0;
}

int platform_pds_rf_turnoff(void *arg)
{
    AON_LowPower_Enter_PDS0();
    return 0;
}

wifi_hosal_funcs_t g_wifi_hosal_funcs =
{
  .efuse_read_mac = platform_get_mac,
  .rf_turn_on = platform_pds_rf_turnon,
  .rf_turn_off = platform_pds_rf_turnoff,
  #ifdef CONF_ADC_ENABLE_TSEN
  .adc_device_get = hosal_adc_device_get,
  .adc_tsen_value_get = hosal_adc_tsen_value_get,
  #endif
  .pm_init = hosal_wifi_ret_zero,
  .pm_event_register = hosal_wifi_ret_zero,
  .pm_deinit = hosal_wifi_ret_zero,
  .pm_state_run = hosal_wifi_ret_zero,
  .pm_capacity_set = hosal_wifi_ret_zero,
  .pm_post_event = hosal_wifi_ret_zero,
  .pm_event_switch = hosal_wifi_ret_zero,
};

#ifdef __cplusplus
}
#endif

