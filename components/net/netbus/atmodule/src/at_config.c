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
#if CONFIG_ATMODULE_CONFIG_STORAGE
#include <easyflash.h>
#endif

#define AT_CONFIG_PRINTF printf
    
int at_config_read(const char *key, void *config, int len)
{
#if CONFIG_ATMODULE_CONFIG_STORAGE
    size_t ret, value_len;

    memset(config, 0, len);
    ret = ef_get_env_blob(key, config, len, &value_len);
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
#if CONFIG_ATMODULE_CONFIG_STORAGE
    ef_set_env_blob(key, config, len);
#endif
    return 1;
}

int at_config_delete(const char *key)
{
#if CONFIG_ATMODULE_CONFIG_STORAGE
    ef_del_env(key);
#endif
    return 0;
}

