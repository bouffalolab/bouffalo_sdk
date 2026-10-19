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
