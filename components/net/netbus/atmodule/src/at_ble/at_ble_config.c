/**
  ******************************************************************************
  * @file    at_ble_config.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <FreeRTOS.h>
//#ifdef EASYFLASH_ENABLE
#include <easyflash.h>
//#endif
//#include <wifi_mgmr_ext.h>
#include "at_config.h"
#include "at_ble_config.h"
#include "bluetooth.h"

ble_config *at_ble_config = NULL;

int at_ble_config_init(void)
{
    at_ble_config = (ble_config *)pvPortMalloc(sizeof(ble_config));
    if (at_ble_config == NULL) {
        return -1;
    }

    memset(at_ble_config, 0, sizeof(ble_config));
    at_ble_config->work_role = BLE_DISABLE;
    size_t value_len = 0;
    
    if (!at_config_read(AT_CONFIG_KEY_BLE_NAME, &at_ble_config->ble_name, sizeof(at_ble_config->ble_name))) {
        strlcpy(at_ble_config->ble_name, "BFLB-AT", sizeof(at_ble_config->ble_name));
        bt_set_name(at_ble_config->ble_name); 
    }
    else
    {
        strlcpy(at_ble_config->ble_name, "BFLB-AT", sizeof(at_ble_config->ble_name));
        bt_set_name(at_ble_config->ble_name); 
    }

    bt_set_name(at_ble_config->ble_name);

    at_ble_config->adv_param.adv_int_min = 0xA0;
    at_ble_config->adv_param.adv_int_max = 0xD0;
    at_ble_config->adv_param.adv_type = 0;
    at_ble_config->adv_param.channel_map = 0x07;

    at_ble_config->scan_param.scan_type = 1;
    at_ble_config->scan_param.own_addr_type = 0;
    at_ble_config->scan_param.filter_policy = 0;
    at_ble_config->scan_param.scan_interval = 40;
    at_ble_config->scan_param.scan_window = 40;
    at_ble_config->ble_sec_param = 0x03;
    return 0;
}


int at_ble_config_save(const char *key)
{
    if (strcmp(key, AT_CONFIG_KEY_BLE_NAME) == 0)
    {
        return at_config_write(key, &at_ble_config->ble_name, sizeof(at_ble_config->ble_name));
    }
    else
        return -1;
}

int at_ble_config_default(void)
{
    at_config_delete(AT_CONFIG_KEY_BLE_NAME);
    
    if (at_ble_config == NULL) {
        return -1;
    }

    memset(at_ble_config, 0, sizeof(ble_config));
    at_ble_config->work_role = BLE_DISABLE;
    
    strlcpy(at_ble_config->ble_name, "BFLB-AT", sizeof(at_ble_config->ble_name));
    bt_set_name(at_ble_config->ble_name); 

    at_ble_config->adv_param.adv_int_min = 0xA0;
    at_ble_config->adv_param.adv_int_max = 0xD0;
    at_ble_config->adv_param.adv_type = 0;
    at_ble_config->adv_param.channel_map = 0x07;

    at_ble_config->scan_param.scan_type = 1;
    at_ble_config->scan_param.own_addr_type = 0;
    at_ble_config->scan_param.filter_policy = 0;
    at_ble_config->scan_param.scan_interval = 40;
    at_ble_config->scan_param.scan_window = 40;
    at_ble_config->ble_sec_param = 0x03;
    return 0;
}

