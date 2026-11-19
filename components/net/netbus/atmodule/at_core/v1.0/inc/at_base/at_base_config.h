/**
  ******************************************************************************
  * @file    at_base_config.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_BASE_CONFIG_H
#define AT_BASE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define AT_CONFIG_KEY_SYS_MSG "SYSMSG"

typedef struct {
    int baudrate;
    uint8_t databits;
    uint8_t stopbits;
    uint8_t parity;
    uint8_t flow_control;
}base_uart_cfg;

typedef union {
    struct
    {
        uint8_t quit_throughput_msg:1;
        uint8_t link_msg_type:1;
        uint8_t link_state_msg:1;
    } bit;
    uint8_t byte;
    uint8_t syslog;
} base_sysmsg_cfg;

typedef int64_t base_systime_stamp;

typedef enum {
    BASE_SLEEP_MODE_DISABLE = 0,
    BASE_SLEEP_MODE_MODEM,
    BASE_SLEEP_MODE_LIGHT,
    BASE_SLEEP_MODE_MODEM_LISTEN_INTERVAL
} base_sleep_mode;

typedef struct {
    uint8_t wakeup_valid;
    uint8_t wakeup_source;
    uint8_t wakeup_gpio;
    uint8_t wakeup_level;
    uint32_t wakeup_sleep_time;
}base_sleepwk_cfg;

typedef struct {
    base_uart_cfg uart_cfg;
    base_sysmsg_cfg sysmsg_cfg;
    base_systime_stamp systime_stamp;
    base_sleep_mode sleep_mode;
    base_sleepwk_cfg sleepwk_cfg;
}base_config;

extern base_config *at_base_config;
    
int at_base_config_init(void);

int at_base_config_save(const char *key);

int at_base_config_default(void);

#ifdef __cplusplus
}
#endif

#endif/* AT_BASE_CONFIG_H */

