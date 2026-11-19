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
#include <limits.h>
#include <regex.h>
#include <FreeRTOS.h>
#include <semphr.h>
//#ifdef EASYFLASH_ENABLE
#include "lfs_kv.h"
//#endif
//#include <bx_rtc.h>

#include "at_main.h"
#include "at_core.h"
#include "at_config.h"
#include "at_base_config.h"
#include "at_pal.h"
#include "at_port.h"

base_config *at_base_config = NULL;
static SemaphoreHandle_t sysmsgfilter_mutex;

int at_base_config_init(void)
{
    if (!sysmsgfilter_mutex) {
        sysmsgfilter_mutex = xSemaphoreCreateMutex();
        if (!sysmsgfilter_mutex) {
            return -1;
        }
    }

    at_base_config = (base_config *)at_malloc(sizeof(base_config));
    if (at_base_config == NULL) {
        AT_CMD_PRINTF("Failed to allocate memory for at_base_config\r\n");
        vSemaphoreDelete(sysmsgfilter_mutex);
        sysmsgfilter_mutex = NULL;
        return -1;
    }

    memset(at_base_config, 0, sizeof(base_config));
#if defined(CONFIG_ATMODULE_UART) && CONFIG_ATMODULE_UART
    if (at_port_para_get(&at_base_config->uart_cfg.baudrate,
                         &at_base_config->uart_cfg.databits,
                         &at_base_config->uart_cfg.stopbits,
                         &at_base_config->uart_cfg.parity,
                         &at_base_config->uart_cfg.flow_control) != 0) {
        at_free(at_base_config);
        at_base_config = NULL;
        return -1;
    }

    at_base_config->uart_factory_cfg = at_base_config->uart_cfg;
    at_base_config->uart_def_cfg = at_base_config->uart_factory_cfg;
    at_base_config->uart_def_valid = 1;

#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && CONFIG_ATMODULE_CONFIG_STORAGE
    base_uart_cfg saved_uart;

    if (at_config_read(AT_CONFIG_KEY_UART_DEF, &saved_uart, sizeof(saved_uart))) {
        if (at_port_uart_config_validate(saved_uart.baudrate,
                                         saved_uart.databits,
                                         saved_uart.stopbits,
                                         saved_uart.parity,
                                         saved_uart.flow_control) == 0) {
            at_base_config->uart_def_cfg = saved_uart;
        } else {
            AT_CMD_PRINTF(
                "UART_DEF: invalid saved configuration; "
                "using boot defaults\r\n");
        }
    }
#endif
#endif

#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && CONFIG_ATMODULE_CONFIG_STORAGE
    base_sysmsg_cfg saved_sysmsg = {0};

    if (at_config_read(AT_CONFIG_KEY_SYS_MSG, &saved_sysmsg, sizeof(saved_sysmsg))) {
        if (saved_sysmsg.byte <= 7 && saved_sysmsg.syslog <= 1) {
            at_base_config->sysmsg_cfg = saved_sysmsg;
        }
    } else {
        uint8_t legacy_sysmsg = 0;

        /* Old records contain only SYSMSG; SYSLOG keeps its default value. */
        if (at_config_read(AT_CONFIG_KEY_SYS_MSG, &legacy_sysmsg, sizeof(legacy_sysmsg)) && legacy_sysmsg <= 7) {
            at_base_config->sysmsg_cfg.byte = legacy_sysmsg;
        }
    }
#endif

    //bx_rtc_init();
    at_base_config->trans_interval_ms = BASE_TRANSINTVL_DEFAULT_MS;
    #if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && CONFIG_ATMODULE_CONFIG_STORAGE
        if (!at_config_read(AT_CONFIG_KEY_TRANSINTVL, &at_base_config->trans_interval_ms,
                            sizeof(at_base_config->trans_interval_ms)) ||
            at_base_config->trans_interval_ms > BASE_TRANSINTVL_MAX_MS) {
            at_base_config->trans_interval_ms = BASE_TRANSINTVL_DEFAULT_MS;
        }
    #endif
    at_base_config->systime_stamp = 0;
    at_base_config->sleep_mode = BASE_SLEEP_MODE_DISABLE;
    return 0;
}

int at_base_config_save(const char *key)
{
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    if (strcmp(key, AT_CONFIG_KEY_UART_DEF) == 0) {
        return at_config_write(key, &at_base_config->uart_def_cfg, sizeof(base_uart_cfg));
    }
    if (strcmp(key, AT_CONFIG_KEY_SYS_MSG) == 0)
        return at_config_write(key, &at_base_config->sysmsg_cfg, sizeof(base_sysmsg_cfg));
    if (strcmp(key, AT_CONFIG_KEY_TRANSINTVL) == 0) {
        return at_config_write(key, &at_base_config->trans_interval_ms, sizeof(at_base_config->trans_interval_ms));
    }
#endif

    return 0;
}

int at_base_config_default(void)
{
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && CONFIG_ATMODULE_CONFIG_STORAGE
    /* Delete UART_DEF last to keep its cache valid on earlier failures. */
    if (at_config_delete(AT_CONFIG_KEY_SYS_MSG) != 1 ||
        at_config_delete(AT_CONFIG_KEY_TRANSINTVL) != 1 ||
        at_config_delete(AT_CONFIG_KEY_UART_DEF) != 1) {
        return -1;
    }
#endif

    at_base_config->uart_def_cfg = at_base_config->uart_factory_cfg;
    at_base_config->uart_def_valid = 1;
    memset(&at_base_config->sysmsg_cfg, 0, sizeof(base_sysmsg_cfg));
    at->syslog = 0;
    at_base_config->trans_interval_ms = BASE_TRANSINTVL_DEFAULT_MS;
    at_base_sysmsgfilter_reset();

    return 0;
}

static void at_base_sysmsgfilter_free(base_sysmsgfilter_entry *entry)
{
    if (entry->rule.head_len) {
        regfree(&entry->head);
    }
    if (entry->rule.tail_len) {
        regfree(&entry->tail);
    }
    at_free(entry);
}

/* Caller holds sysmsgfilter_mutex. */
static void at_base_sysmsgfilter_clear(void)
{
    base_sysmsgfilter_entry *entry = at_base_config->sysmsgfilter_rules;

    while (entry) {
        base_sysmsgfilter_entry *next = entry->next;
        at_base_sysmsgfilter_free(entry);
        entry = next;
    }
    at_base_config->sysmsgfilter_rules = NULL;
}

void at_base_sysmsgfilter_reset(void)
{
    if (!at_base_config) {
        return;
    }

    xSemaphoreTake(sysmsgfilter_mutex, portMAX_DELAY);
    at_base_config->sysmsgfilter_cfg = 0;
    at_base_sysmsgfilter_clear();
    xSemaphoreGive(sysmsgfilter_mutex);
}

int at_base_sysmsgfilter_set_enabled(unsigned int enabled)
{
    xSemaphoreTake(sysmsgfilter_mutex, portMAX_DELAY);
    at_base_config->sysmsgfilter_cfg = enabled;
    xSemaphoreGive(sysmsgfilter_mutex);
    return AT_RESULT_CODE_OK;
}

int at_base_sysmsgfilter_get_enabled(void)
{
    int enabled;

    xSemaphoreTake(sysmsgfilter_mutex, portMAX_DELAY);
    enabled = at_base_config->sysmsgfilter_cfg;
    xSemaphoreGive(sysmsgfilter_mutex);
    return enabled;
}

static int at_base_sysmsgfilter_name_equal(const uint8_t *name, size_t length, const char *expected)
{
    return strlen(expected) == length && memcmp(name, expected, length) == 0;
}

/* C-locale collating-element names supported by the SDK's Spencer regex library. */
static int at_base_sysmsgfilter_collating_value(const uint8_t *name, size_t length)
{
    static const struct {
        const char *name;
        uint8_t value;
    } names[] = {
        { "NUL", 0 }, { "SOH", 1 }, { "STX", 2 }, { "ETX", 3 },
        { "EOT", 4 }, { "ENQ", 5 }, { "ACK", 6 }, { "BEL", 7 }, { "alert", 7 },
        { "BS", 8 }, { "backspace", 8 }, { "HT", 9 }, { "tab", 9 },
        { "LF", 10 }, { "newline", 10 }, { "VT", 11 }, { "vertical-tab", 11 },
        { "FF", 12 }, { "form-feed", 12 }, { "CR", 13 }, { "carriage-return", 13 },
        { "SO", 14 }, { "SI", 15 }, { "DLE", 16 }, { "DC1", 17 }, { "DC2", 18 },
        { "DC3", 19 }, { "DC4", 20 }, { "NAK", 21 }, { "SYN", 22 }, { "ETB", 23 },
        { "CAN", 24 }, { "EM", 25 }, { "SUB", 26 }, { "ESC", 27 },
        { "IS4", 28 }, { "FS", 28 }, { "IS3", 29 }, { "GS", 29 },
        { "IS2", 30 }, { "RS", 30 }, { "IS1", 31 }, { "US", 31 },
        { "space", 32 }, { "exclamation-mark", 33 }, { "quotation-mark", 34 },
        { "number-sign", 35 }, { "dollar-sign", 36 }, { "percent-sign", 37 },
        { "ampersand", 38 }, { "apostrophe", 39 }, { "left-parenthesis", 40 },
        { "right-parenthesis", 41 }, { "asterisk", 42 }, { "plus-sign", 43 },
        { "comma", 44 }, { "hyphen", 45 }, { "hyphen-minus", 45 },
        { "period", 46 }, { "full-stop", 46 }, { "slash", 47 }, { "solidus", 47 },
        { "zero", 48 }, { "one", 49 }, { "two", 50 }, { "three", 51 },
        { "four", 52 }, { "five", 53 }, { "six", 54 }, { "seven", 55 },
        { "eight", 56 }, { "nine", 57 }, { "colon", 58 }, { "semicolon", 59 },
        { "less-than-sign", 60 }, { "equals-sign", 61 }, { "greater-than-sign", 62 },
        { "question-mark", 63 }, { "commercial-at", 64 }, { "left-square-bracket", 91 },
        { "backslash", 92 }, { "reverse-solidus", 92 }, { "right-square-bracket", 93 },
        { "circumflex", 94 }, { "circumflex-accent", 94 }, { "underscore", 95 },
        { "low-line", 95 }, { "grave-accent", 96 }, { "left-brace", 123 },
        { "left-curly-bracket", 123 }, { "vertical-line", 124 }, { "right-brace", 125 },
        { "right-curly-bracket", 125 }, { "tilde", 126 }, { "DEL", 127 },
    };

    if (length == 1) {
        return name[0];
    }
    for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
        if (at_base_sysmsgfilter_name_equal(name, length, names[i].name)) {
            return names[i].value;
        }
    }
    return -1;
}

/* Read one character, collating element, equivalence class, or character class.
 * The caller supplies a nonempty input span. */
static int at_base_sysmsgfilter_bracket_item(const uint8_t **cursor, const uint8_t *end,
                                           int *value, int *is_class)
{
    static const char *const classes[] = {
        "alnum", "alpha", "blank", "cntrl", "digit", "graph",
        "lower", "print", "punct", "space", "upper", "xdigit",
    };
    const uint8_t *p = *cursor;

    *is_class = 0;
    if (end - p >= 2 && p[0] == '[' && (p[1] == ':' || p[1] == '.' || p[1] == '=')) {
        uint8_t delimiter = p[1];
        const uint8_t *name = p + 2;

        for (p = name; end - p >= 2; p++) {
            if (p[0] == delimiter && p[1] == ']') {
                if (delimiter == ':') {
                    for (size_t i = 0; i < sizeof(classes) / sizeof(classes[0]); i++) {
                        if (at_base_sysmsgfilter_name_equal(name, p - name, classes[i])) {
                            *is_class = 1;
                            *value = 0;
                            *cursor = p + 2;
                            return 1;
                        }
                    }
                    return 0;
                }
                *value = at_base_sysmsgfilter_collating_value(name, p - name);
                *is_class = delimiter == '=';
                *cursor = p + 2;
                return *value >= 0;
            }
        }
        return 0;
    }

    *value = *p++;
    *cursor = p;
    return 1;
}

/*
 * Picolibc 1.8.10's bracket parser can use character sets after seterr() frees
 * them. Reject malformed bracket expressions before entering regcomp(), for
 * both head and tail patterns. The regex library still compiles and matches
 * valid BRE/ERE expressions; backslashes inside brackets are ordinary bytes.
 */
static int at_base_sysmsgfilter_brackets_valid(const uint8_t *pattern, size_t length)
{
    const uint8_t *p = pattern;
    const uint8_t *end = pattern + length;

    while (p < end) {
        if (*p == '\\') {
            p++;
            if (p < end) {
                p++;
            }
            continue;
        }
        if (*p++ != '[') {
            continue;
        }
        /* Spencer's word-boundary extensions: [[:<:]] and [[:>:]]. */
        if (end - p >= 6 && (memcmp(p, "[:<:]]", 6) == 0 || memcmp(p, "[:>:]]", 6) == 0)) {
            p += 6;
            continue;
        }
        if (p < end && *p == '^') {
            p++;
        }
        if (p < end && *p == ']') {
            p++;
        }
        if (p < end && *p == '-') {
            p++;
        }
        while (p < end && *p != ']') {
            int first, last, is_class;

            if (*p == '-') {
                if (end - p < 2 || p[1] != ']') {
                    return 0;
                }
                p++;
                continue;
            }
            if (!at_base_sysmsgfilter_bracket_item(&p, end, &first, &is_class)) {
                return 0;
            }
            if (!is_class && end - p >= 2 && p[0] == '-' && p[1] != ']') {
                p++;
                if (!at_base_sysmsgfilter_bracket_item(&p, end, &last, &is_class) ||
                    is_class || first > last) {
                    return 0;
                }
            }
        }
        if (p == end) {
            return 0;
        }
        p++;
    }
    return 1;
}

int at_base_sysmsgfilter_update(int operation, const base_sysmsgfilter_rule *rule)
{
    base_sysmsgfilter_entry *entry = NULL;
    base_sysmsgfilter_entry **link;
    int result;

    if (operation == 0) {
        xSemaphoreTake(sysmsgfilter_mutex, portMAX_DELAY);
        at_base_sysmsgfilter_clear();
        xSemaphoreGive(sysmsgfilter_mutex);
        return AT_RESULT_CODE_OK;
    }

    if (memchr(rule->head, '\0', rule->head_len) ||
        memchr(rule->tail, '\0', rule->tail_len) ||
        !at_base_sysmsgfilter_brackets_valid(rule->head, rule->head_len) ||
        !at_base_sysmsgfilter_brackets_valid(rule->tail, rule->tail_len)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (operation == 1) {
        int flags = 0;

        entry = at_calloc(1, sizeof(*entry));
        if (!entry) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
        }
        entry->rule = *rule;
        entry->rule.head[rule->head_len] = '\0';
        entry->rule.tail[rule->tail_len] = '\0';

        /* ESP-AT bit 2 is REG_NEWLINE, not this libc's REG_NOSUB. */
        if (rule->cflags & 1) {
            flags |= REG_EXTENDED;
        }
        if (rule->cflags & 2) {
            flags |= REG_ICASE;
        }
        if (rule->cflags & 4) {
            flags |= REG_NEWLINE;
        }

        /* Compile only additions, outside the lock. Failed updates preserve existing rules. */
        if (rule->head_len) {
            result = regcomp(&entry->head, (const char *)entry->rule.head, flags);
            if (result != 0) {
                at_free(entry);
                return AT_RESULT_WITH_SUB_CODE(result == REG_ESPACE ? AT_SUB_NO_MEMORY :
                                                                     AT_SUB_PARA_VALUE_INVALID);
            }
        }
        if (rule->tail_len) {
            result = regcomp(&entry->tail, (const char *)entry->rule.tail, flags);
            if (result != 0) {
                if (rule->head_len) {
                    regfree(&entry->head);
                }
                at_free(entry);
                return AT_RESULT_WITH_SUB_CODE(result == REG_ESPACE ? AT_SUB_NO_MEMORY :
                                                                     AT_SUB_PARA_VALUE_INVALID);
            }
        }
    }

    xSemaphoreTake(sysmsgfilter_mutex, portMAX_DELAY);
    for (link = &at_base_config->sysmsgfilter_rules; *link; link = &(*link)->next) {
        const base_sysmsgfilter_rule *saved = &(*link)->rule;

        if (saved->head_len == rule->head_len && saved->tail_len == rule->tail_len &&
            saved->cflags == rule->cflags && memcmp(saved->head, rule->head, rule->head_len) == 0 &&
            memcmp(saved->tail, rule->tail, rule->tail_len) == 0) {
            break;
        }
    }

    result = AT_RESULT_CODE_OK;
    if (operation == 1) {
        /* Keep insertion order; adding an identical rule is idempotent. */
        if (!*link) {
            *link = entry;
            entry = NULL;
        }
    } else if (*link) {
        base_sysmsgfilter_entry *removed = *link;
        *link = removed->next;
        at_base_sysmsgfilter_free(removed);
    } else {
        result = AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    xSemaphoreGive(sysmsgfilter_mutex);
    if (entry) {
        at_base_sysmsgfilter_free(entry);
    }
    return result;
}

int at_base_sysmsgfilter_get(unsigned int index, base_sysmsgfilter_rule *rule)
{
    base_sysmsgfilter_entry *entry;
    int found = 0;

    xSemaphoreTake(sysmsgfilter_mutex, portMAX_DELAY);
    for (entry = at_base_config->sysmsgfilter_rules; entry && index; entry = entry->next) {
        index--;
    }
    if (entry) {
        *rule = entry->rule;
        found = 1;
    }
    xSemaphoreGive(sysmsgfilter_mutex);
    return found;
}

int at_base_sysmsgfilter_apply(const uint8_t *data, size_t length, size_t *offset, size_t *remaining)
{
    int matched = 0;

    *offset = 0;
    *remaining = length;

    if (!at_base_config || length == 0 || length > INT_MAX) {
        return 0;
    }

    xSemaphoreTake(sysmsgfilter_mutex, portMAX_DELAY);
    if (at_base_config->sysmsgfilter_cfg) {
        base_sysmsgfilter_entry *entry;
        for (entry = at_base_config->sysmsgfilter_rules; entry; entry = entry->next) {
            regmatch_t head = { .rm_so = 0, .rm_eo = (regoff_t)length };
            regmatch_t tail = { .rm_so = 0, .rm_eo = (regoff_t)length };
            size_t begin = 0;
            size_t end = length;

            /* REG_STARTEND handles explicit lengths, including binary data. */
            if (entry->rule.head_len) {
                if (regexec(&entry->head, (const char *)data, 1, &head, REG_STARTEND) != 0) {
                    continue;
                }
                begin = head.rm_eo;
            }
            if (entry->rule.tail_len) {
                if (regexec(&entry->tail, (const char *)data, 1, &tail, REG_STARTEND) != 0) {
                    continue;
                }
                end = tail.rm_so;
            }

            *offset = begin;
            *remaining = end > begin ? end - begin : 0;
            matched = 1;
            break;
        }
    }
    xSemaphoreGive(sysmsgfilter_mutex);
    return matched;
}
