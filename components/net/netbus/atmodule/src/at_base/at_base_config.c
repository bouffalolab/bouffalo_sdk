/**
  ******************************************************************************
  * @file    at_wifi_config.c
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
//#include <bx_rtc.h>

#include "bflb_adc.h"
#include "at_config.h"
#include "at_base_config.h"

base_config *at_base_config = NULL;

int at_base_config_init(void)
{
    at_base_config = (base_config *)at_malloc(sizeof(base_config));
    if (at_base_config == NULL) {
        AT_CMD_PRINTF("Failed to allocate memory for at_base_config\r\n");
        return -1;
    }

    memset(at_base_config, 0, sizeof(base_config));
#if 0
    if (!at_config_read(AT_CONFIG_KEY_UART_CFG, &at_base_config->uart_cfg, sizeof(base_uart_cfg))) {
        at_base_config->uart_cfg.baudrate = 115200;
        at_base_config->uart_cfg.databits = 8;
        at_base_config->uart_cfg.stopbits = 1;
        at_base_config->uart_cfg.parity = 0;
        at_base_config->uart_cfg.flow_control = 0;
    }
#endif

#if CONFIG_ATMODULE_CONFIG_STORAGE
    if (!at_config_read(AT_CONFIG_KEY_SYS_MSG, &at_base_config->sysmsg_cfg, sizeof(base_sysmsg_cfg))) {
        at_base_config->sysmsg_cfg.bit.quit_throughput_msg = 0;
        at_base_config->sysmsg_cfg.bit.link_msg_type = 0;
        at_base_config->sysmsg_cfg.bit.link_state_msg = 0;
        at_base_config->sysmsg_cfg.syslog = 0;
    }

    //bx_rtc_init();
    at_base_config->systime_stamp = 0;
#endif
    return 0;
}

int at_base_config_save(const char *key)
{
#if CONFIG_ATMODULE_CONFIG_STORAGE
    if (!at_base_config || !key) {
        AT_CMD_PRINTF("Invalid arguments to at_base_config_save\r\n");
        return -1;
    }
    if (strcmp(key, AT_CONFIG_KEY_SYS_MSG) == 0)
        return at_config_write(key, &at_base_config->sysmsg_cfg, sizeof(base_sysmsg_cfg));
#endif

    return 0;
}

int at_base_config_default(void)
{
#if CONFIG_ATMODULE_CONFIG_STORAGE
    at_config_delete(AT_CONFIG_KEY_SYS_MSG);
#endif
    return 0;
}

