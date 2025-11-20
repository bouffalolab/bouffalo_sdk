/**
  ******************************************************************************
  * @file    at_config.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_CONFIG_H
#define AT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "at_main.h"
#include "at_core.h"

int at_config_read(const char *key, void *config, int len);

int at_config_write(const char *key, void *config, int len);

#ifdef __cplusplus
}
#endif

#endif/* AT_CONFIG_H */

