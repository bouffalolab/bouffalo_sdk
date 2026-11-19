/**
  ******************************************************************************
  * @file    at_mqtt_config.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_MQTT_CONFIG_H
#define AT_MQTT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define AT_MQTT_SSL_CONF "MQTT_SSL"
//#define AT_MQTT_SSL_CLIENT_ID "MQTT_CLIENTID"
//#define AT_MQTT_SSL_USER_NAME "MQTT_NAME"
//#define AT_MQTT_SSL_PASSWORD "MQTT_PSWD"

typedef struct {
    char ca_file[32];
    char cert_file[32];
    char key_file[32];
} mqtt_ssl_conf_t;

typedef struct {
    mqtt_ssl_conf_t ssl_conf;
} mqtt_config;

extern mqtt_config *at_mqtt_config;

int at_mqtt_config_init(void);

int at_mqtt_config_save(const char *key);

int at_mqtt_config_default(void);

#ifdef __cplusplus
}
#endif

#endif/* AT_MQTT_CONFIG_H */

