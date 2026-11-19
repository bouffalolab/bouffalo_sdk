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

#include <stddef.h>
#include <stdint.h>
#include <regex.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AT_CONFIG_KEY_SYS_MSG "SYSMSG"
#define AT_CONFIG_KEY_TRANSINTVL "TRANSINTVL"
#define AT_CONFIG_KEY_UART_DEF "UART_DEF"
#define BASE_TRANSINTVL_DEFAULT_MS 20U
#define BASE_TRANSINTVL_MAX_MS 1000U

typedef struct {
    int baudrate;
    uint8_t databits;
    uint8_t stopbits;
    uint8_t parity;
    uint8_t flow_control;
}base_uart_cfg;

typedef struct {
    union {
        struct {
            uint8_t quit_throughput_msg:1;
            uint8_t link_msg_type:1;
            uint8_t link_state_msg:1;
        } bit;
        uint8_t byte;
    };
    uint8_t syslog;
} base_sysmsg_cfg;

typedef uint8_t base_sysmsgfilter_cfg;


#define BASE_SYSMSGFILTER_PART_MAX 64
#define BASE_SYSMSGFILTER_CFLAGS_MAX 7

typedef struct {
    uint16_t head_len;
    uint16_t tail_len;
    uint8_t cflags;
    uint8_t head[BASE_SYSMSGFILTER_PART_MAX + 1];
    uint8_t tail[BASE_SYSMSGFILTER_PART_MAX + 1];
} base_sysmsgfilter_rule;

typedef struct base_sysmsgfilter_entry {
    base_sysmsgfilter_rule rule;
    regex_t head;
    regex_t tail;
    struct base_sysmsgfilter_entry *next;
} base_sysmsgfilter_entry;

typedef struct {
    uint8_t direct;
    uint32_t address;
    uint32_t write_value;
} base_sysreg_cfg;

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
    base_uart_cfg uart_def_cfg;
    base_uart_cfg uart_factory_cfg;
    uint8_t uart_def_valid;
    base_sysmsg_cfg sysmsg_cfg;
    base_sysmsgfilter_cfg sysmsgfilter_cfg;
    base_sysmsgfilter_entry *sysmsgfilter_rules;
    base_systime_stamp systime_stamp;
    uint16_t trans_interval_ms;
    base_sleep_mode sleep_mode;
    base_sleepwk_cfg sleepwk_cfg;
}base_config;

extern base_config *at_base_config;
    
int at_base_config_init(void);

int at_base_config_save(const char *key);

int at_base_config_default(void);

void at_base_sysmsgfilter_reset(void);

/* Command helpers require successful base configuration initialization.
 * Set/update arguments are validated by the command handlers. */
int at_base_sysmsgfilter_set_enabled(unsigned int enabled);

int at_base_sysmsgfilter_get_enabled(void);

int at_base_sysmsgfilter_update(int operation, const base_sysmsgfilter_rule *rule);

int at_base_sysmsgfilter_get(unsigned int index, base_sysmsgfilter_rule *rule);

/* May run before base init. Valid data/output pointers are supplied by the write paths.
 * Return the unfiltered middle of this write; never combine separate writes. */
int at_base_sysmsgfilter_apply(const uint8_t *data, size_t length, size_t *offset, size_t *remaining);

#ifdef __cplusplus
}
#endif

#endif/* AT_BASE_CONFIG_H */

