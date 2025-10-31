/****************************************************************************
 * components/network/wifi_hosal/include/wifi_hosal.h
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

#ifndef _WIFI_HOSAL_H_
#define _WIFI_HOSAL_H_

#include <sys/_stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "stdint.h"
#include "stddef.h"

/****************************************************************************
 * Definition
 ****************************************************************************/
typedef int (*hosal_pm_cb_t)(void *arg);

#define    NODE_CAP_BIT_UAPSD_MODE        (1 << 0)
#define    NODE_CAP_BIT_MAC_IDLE          (1 << 1)
#define    NODE_CAP_BIT_MAC_DOZE          (1 << 2)
#define    NODE_CAP_BIT_RF_ONOFF          (1 << 3)
#define    NODE_CAP_BIT_WLAN_BLE_ABORT    (1 << 4)
#define    NODE_CAP_BIT_FORCE_SLEEP       (1 << 5)
#define    NODE_CAP_BIT_ALL_ALLOWED       (0xffff)

enum PM_EVENT_ABLE {
    PM_DISABLE = 0,
    PM_ENABLE,
};

enum WLAN_CODE_SLEEP_CONTROL {
    WLAN_CODE_PM_NOTIFY_START = 0,
    WLAN_CODE_PM_NOTIFY_STOP,
    WLAN_CODE_PM_START,
    WLAN_CODE_PM_STOP,
};

enum WLAN_CODE_ENTER_SLEEP {
    WLAN_CODE_PM_ENTER_SLEEP_PRE = 0,
    WLAN_CODE_PM_ENTER_SLEEP,
};

enum WLAN_CODE_EXIT_SLEEP {
    WLAN_CODE_PM_ENTER_SLEEP_PRE_FAILED = 0,
    WLAN_CODE_PM_EXIT_SLEEP_PRE,
    WLAN_CODE_PM_EXIT_SLEEP,
};

enum WLAN_CODE_BEACON_LOSS {
    WLAN_CODE_BEACON_LOSS = 0,
};

enum WLAN_CODE_SEND_NULLDATA {
   WLAN_CODE_PM_PAUSE = 0,
   WLAN_CODE_PM_NULLDATA_NOACK,
   WLAN_CODE_PM_NULLDATA_SEND_ERROR,
};

enum BLE_CODE_BLE_CONTROL {
    BLE_CODE_PM_TURNON_RF = 0,
    BLE_CODE_PM_TURNOFF_RF,
};

enum PM_LEVEL{
    PM_MODE_STA_NONE = 0,
    PM_MODE_STA_IDLE,
    PM_MODE_STA_MESH,
    PM_MODE_STA_DOZE,
    PM_MODE_STA_COEX,
    PM_MODE_STA_DOWN,
    PM_MODE_AP_IDLE,
    PM_MODE_MAX,
};

enum PM_EVEMT{
    WLAN_PM_EVENT_CONTROL = 0,
    WLAN_PM_EVENT_ENTER_SLEEP,
    WLAN_PM_EVENT_EXIT_SLEEP,
    WLAN_PM_EVENT_BEACON_LOSS,
    WLAN_PM_EVENT_SEND_NULLDATA,
    BLE_PM_EVENT_CONTROL,
    PM_EVENT_MAX,
};

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct wifi_hosal_funcs
{
  int (*efuse_read_mac)(uint8_t mac[6]);
  int (*rf_turn_on)(void *arg);
  int (*rf_turn_off)(void *arg);
#ifdef CONF_ADC_ENABLE_TSEN
  hosal_adc_dev_t* (*adc_device_get)(void);
  int (*adc_tsen_value_get)(hosal_adc_dev_t *adc);
#endif
  int (*pm_init) (void);
  int (*pm_event_register) (enum PM_EVEMT event, uint32_t code, uint32_t cap_bit, uint16_t pirority, hosal_pm_cb_t ops, void *arg, enum PM_EVENT_ABLE enable);
  int (*pm_deinit) (void);
  int (*pm_state_run) (void);
  int (*pm_capacity_set) (enum PM_LEVEL level);
  int (*pm_post_event) (enum PM_EVEMT event, uint32_t code, uint32_t *retval);
  int (*pm_event_switch) (enum PM_EVEMT event, uint32_t code, enum PM_EVENT_ABLE enable);
};

typedef struct wifi_hosal_funcs wifi_hosal_funcs_t;

/****************************************************************************
 * Private Data
 ****************************************************************************/

extern wifi_hosal_funcs_t g_wifi_hosal_funcs;


/****************************************************************************
 * Private Function
 ****************************************************************************/

static inline int hosal_wifi_ret_zero(void *arg)
{
  return 0;
}

static inline int hosal_wifi_ret_non_zero(void *arg)
{
  return -1;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: wifi_hosal_efuse_read_mac
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int wifi_hosal_efuse_read_mac(uint8_t mac[6]);

/****************************************************************************
 * Name: wifi_hosal_pds_rf_turnon
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int wifi_hosal_rf_turn_on(void *arg);

/****************************************************************************
 * Name: wifi_hosal_pds_rf_turnoff
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int wifi_hosal_rf_turn_off(void *arg);

#ifdef CONF_ADC_ENABLE_TSEN
/****************************************************************************
 * Name: wifi_hosal_adc_device_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
hosal_adc_dev_t* wifi_hosal_adc_device_get(void);


/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_adc_tsen_value_get(hosal_adc_dev_t *adc);
#endif
/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int wifi_hosal_pm_init(void);
/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_event_register(enum PM_EVEMT event, uint32_t code, uint32_t cap_bit, uint16_t pirority, hosal_pm_cb_t ops, void *arg, enum PM_EVENT_ABLE enable);
/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_deinit(void);
/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_state_run(void);
/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_capacity_set(enum PM_LEVEL level);
/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_post_event(enum PM_EVEMT event, uint32_t code, uint32_t *retval);
/****************************************************************************
 * Name: wifi_hosal_adc_tsen_value_get
 *
 * Description:
 *   Disable irq num
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/
int wifi_hosal_pm_event_switch(enum PM_EVEMT event, uint32_t code, enum PM_EVENT_ABLE enable);


#ifdef __cplusplus
}
#endif

#endif /* _WIFI_HOSAL_H_ */
