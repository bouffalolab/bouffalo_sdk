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

static at_base_adc_tsen_init(void)
{
    struct bflb_device_s *adc;
    
    struct bflb_adc_config_s cfg;
    struct bflb_adc_channel_s chan;
    
    adc = bflb_device_get_by_name("adc");

    /* adc clock = XCLK / 2 / 32 */
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_2P0V;

    chan.pos_chan = ADC_CHANNEL_TSEN_P;
    chan.neg_chan = ADC_CHANNEL_GND;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, &chan, 1);
    bflb_adc_tsen_init(adc, ADC_TSEN_MOD_INTERNAL_DIODE);
}

int at_base_config_init(void)
{
    at_base_config = (base_config *)pvPortMalloc(sizeof(base_config));
    if (at_base_config == NULL) {
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

    if (!at_config_read(AT_CONFIG_KEY_SYS_MSG, &at_base_config->sysmsg_cfg, sizeof(base_sysmsg_cfg))) {
        at_base_config->sysmsg_cfg.bit.quit_throughput_msg = 0;
        at_base_config->sysmsg_cfg.bit.link_msg_type = 0;
        at_base_config->sysmsg_cfg.bit.link_state_msg = 0;
        at_base_config->sysmsg_cfg.syslog = 0;
    }

    //bx_rtc_init();
    at_base_config->systime_stamp = 0;
    at_base_config->sleep_mode = BASE_SLEEP_MODE_DISABLE;
    at_base_adc_tsen_init();
    return 0;
}

int at_base_config_save(const char *key)
{
    if (strcmp(key, AT_CONFIG_KEY_SYS_MSG) == 0)
        return at_config_write(key, &at_base_config->sysmsg_cfg, sizeof(base_sysmsg_cfg));
    else
        return -1;
    return 0;
}

int at_base_config_default(void)
{
    at_config_delete(AT_CONFIG_KEY_SYS_MSG);
    return 0;
}

