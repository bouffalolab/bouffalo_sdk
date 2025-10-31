/**
  ******************************************************************************
  * @file    at_net_config.c
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
#include "at_config.h"
#include "at_mqtt_config.h"

mqtt_config *at_mqtt_config = NULL;

int at_mqtt_config_init(void)
{
    int i;

    at_mqtt_config = (mqtt_config *)pvPortMalloc(sizeof(mqtt_config));
    if (at_mqtt_config == NULL) {
        return -1;
    }

    memset(at_mqtt_config, 0, sizeof(mqtt_config));
 
    at_config_read(AT_MQTT_SSL_CONF, &at_mqtt_config->ssl_conf, sizeof(at_mqtt_config->ssl_conf));   
 
    return 0;
}

int at_mqtt_config_save(const char *key)
{
    if (strcmp(key, AT_MQTT_SSL_CONF) == 0)
        return at_config_write(key, &at_mqtt_config->ssl_conf, sizeof(at_mqtt_config->ssl_conf));

    return -1;
}

int at_mqtt_config_default(void)
{

    //at_config_delete(AT_CONFIG_KEY_NET_TRANS_LINK);
    return 0;
}

