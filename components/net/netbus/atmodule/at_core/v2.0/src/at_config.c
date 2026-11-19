/**
  ******************************************************************************
  * @file    at_config.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
#include "lfs_kv.h"
#endif
#include "at_main.h"
#include "at_core.h"
#include "at_port.h"
#include "at_kv.h"
#include "at_wifi_config.h"
#include "at_wifi_mgmr.h"
#include "wifi_country_config.h"
#include "wifi_mgmr_ext.h"

#define AT_CONFIG_PRINTF AT_CMD_PRINTF
    
int at_config_read(const char *key, void *config, int len)
{
    if (!key || !config || len <= 0) {
        AT_CONFIG_PRINTF("Invalid arguments to at_config_read\r\n");
        return 0;
    }

#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    size_t ret, value_len;

    memset(config, 0, len);
    ret = lfs_kv_get_blob(key, config, len, &value_len);
    if (ret > 0 && ret == value_len && value_len == len) {
        AT_CONFIG_PRINTF("'%s' (%d) read success\r\n", key, len);
        return 1;
    }

    AT_CONFIG_PRINTF("'%s' (%d) read failed\r\n", key, len);
#endif
    return 0;
}

int at_config_write(const char *key, void *config, int len)
{
    if (!key || !config || len <= 0) {
        AT_CONFIG_PRINTF("Invalid arguments to at_config_write\r\n");
        return 0;
    }
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    int ret = lfs_kv_set_blob(key, config, len);
    if (ret != 0) {
        AT_CONFIG_PRINTF("lfs_kv_set_blob failed for '%s' (%d)\r\n", key, len);
        return 0;
    }
    return 1;
#else
    return 0;
#endif
}

int at_config_delete(const char *key)
{
    if (!key) {
        AT_CONFIG_PRINTF("Invalid arguments to at_config_delete\r\n");
        return 0;
    }
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    int ret = lfs_kv_del(key);
    if (ret != 0) {
        AT_CONFIG_PRINTF("lfs_kv_del failed for '%s'\r\n", key);
        return 0;
    }
    return 1;
#else
    return 0;
#endif
}

int at_config_write_with_id(const char *key, int id, void *config, int len)
{
    char key_id[128];
    snprintf(key_id, sizeof(key_id), "%s%d", key, id);

    return at_config_write(key_id, config, len);
}

int at_config_read_with_id(const char *key, int id, void *config, int len)
{
    if (!key || !config || len <= 0) {
        AT_CONFIG_PRINTF("Invalid arguments to at_config_read_with_id\r\n");
        return 0;
    }

#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    size_t ret, value_len;
    char key_id[128];
    snprintf(key_id, sizeof(key_id), "%s%d", key, id);

    ret = lfs_kv_get_blob(key_id, config, len, &value_len);
    if (ret > 0 && ret == value_len) {
        AT_CONFIG_PRINTF("'%s' (%d) read success\r\n", key_id, value_len);
        return 1;
    }
#endif
    return 0;
}

int at_config_delete_with_id(const char *key, int id)
{
    char key_id[128];
    snprintf(key_id, sizeof(key_id), "%s%d", key, id);

    return at_config_delete(key_id);
}

static int at_factory_config_read_number(const char *key, uint8_t type, int *value)
{
    char buffer[12];
    uint8_t saved_type;
    void *data;
    int ret = at_kv_get("factory_param", key, &saved_type, &data);

    if (ret < 0) {
        return 0;
    }
    if (saved_type != type || ret >= (int)sizeof(buffer)) {
        free(data);
        return 0;
    }
    memcpy(buffer, data, ret);
    buffer[ret] = '\0';
    free(data);
    return at_arg_get_number(buffer, value);
}

int at_factory_config_init(void)
{
    int store = 1;

    if (at_kv_init() != 0) {
        return -1;
    }

    at_factory_config_read_number("sys_store", AT_KV_U8, &store);
    at->store = store != 0;
#ifdef CONFIG_ATMODULE_UART
    int baudrate = 2000000;
    int port = -1, tx_pin = -1, rx_pin = -1;

    at_factory_config_read_number("uart_baudrate", AT_KV_I32, &baudrate);
    at_factory_config_read_number("uart_port", AT_KV_I8, &port);
    at_factory_config_read_number("uart_tx_pin", AT_KV_I32, &tx_pin);
    at_factory_config_read_number("uart_rx_pin", AT_KV_I32, &rx_pin);
    return at_port_uart_config_set(port, baudrate, tx_pin, rx_pin);
#endif
    return 0;
}

int at_factory_wifi_config_init(void)
{
    const char *country_codes[] = AT_WIFI_COUNTRY_CODE;
    void *country_code = NULL;
    uint8_t country_type;
    int start_channel, channel_num, country_count;
    int country_length;
    int country_index = 0, driver_index = 0;

    at_wifi_config->wifi_country.country_policy = AT_WIFI_COUNTRY_POLICY_AUTO;
    at_wifi_config->wifi_country.country_code = WIFI_COUNTRY_CODE_WORLD;

    country_length = at_kv_get("factory_param", "country_code", &country_type, &country_code);
    if (country_length != 2 || country_type != AT_KV_STRING) {
        goto apply_country;
    }
    if (!at_factory_config_read_number("start_channel", AT_KV_U8, &start_channel) ||
        !at_factory_config_read_number("channel_num", AT_KV_U8, &channel_num) ||
        start_channel < 1 || channel_num < 1 || channel_num > 15 - start_channel) {
        goto apply_country;
    }

    while (country_index < WIFI_COUNTRY_CODE_MAX &&
           memcmp(country_code, country_codes[country_index], 2) != 0) {
        country_index++;
    }
    if (country_index == WIFI_COUNTRY_CODE_MAX) {
        goto apply_country;
    }

    wifi_country_get_stats(&country_count, NULL);
    while (driver_index < country_count &&
           memcmp(country_code, country_list[driver_index].code, 2) != 0) {
        driver_index++;
    }
    if (driver_index == country_count) {
        goto apply_country;
    }

    country_list[driver_index].channel24G_start = start_channel;
    country_list[driver_index].channel24G_num = channel_num;
    at_wifi_config->wifi_country.country_code = country_index;
    at_wifi_config->wifi_country.country_policy = AT_WIFI_COUNTRY_POLICY_MANUAL;

apply_country:
    free(country_code);
    if (at_wifi_mgmr_set_country_code(country_codes[at_wifi_config->wifi_country.country_code]) != 0) {
        AT_CONFIG_PRINTF("Failed to set default Wi-Fi country\r\n");
        return -1;
    }
    wifi_mgmr_ieee80211d_enable(at_wifi_config->wifi_country.country_policy == AT_WIFI_COUNTRY_POLICY_AUTO);
    return 0;
}
