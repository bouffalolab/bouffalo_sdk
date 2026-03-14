/**
 * @file bflbwifi_parser.c
 * @brief AT protocol parser implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/bflbwifi_log.h"
#include "bflbwifi_internal.h"

/* URC type definitions */
typedef enum {
    URC_UNKNOWN = 0,

    /* WiFi status URC */
    URC_WIFI_CONNECTING,
    URC_WIFI_CONNECTED,
    URC_WIFI_GOTIP,
    URC_WIFI_ERROR,
    URC_WIFI_DISCONNECTED,
    URC_WIFI_SCAN_DONE,

    /* AP mode URC */
    URC_AP_STA_CONNECTED,
    URC_AP_STA_DISCONNECTED,
    URC_AP_STA_IP_ASSIGNED,

    /* Scan results */
    URC_CWLAP,
} urc_type_t;

/**
 * @brief Determine if a line of data is URC
 * @return true=URC, false=response or other
 */
static bool is_urc(const char *line)
{
    if (!line || line[0] == '\0') {
        return false;
    }

    /* Simple responses: OK, ERROR, SEND OK, SEND FAIL */
    if (strcmp(line, "OK") == 0 ||
        strcmp(line, "ERROR") == 0 ||
        strcmp(line, "SEND OK") == 0 ||
        strcmp(line, "SEND FAIL") == 0) {
        return false;
    }

    /* Lines starting with + are usually URC */
    if (line[0] == '+') {
        return true;
    }

    /* busy p... is also URC */
    if (strncmp(line, "busy p", 6) == 0) {
        return true;
    }

    return false;
}

/**
 * @brief Identify URC type
 */
static urc_type_t identify_urc(const char *line)
{
    if (!line) {
        return URC_UNKNOWN;
    }

    /* WiFi status URC */
    if (strcmp(line, "+CW:CONNECTING") == 0) {
        return URC_WIFI_CONNECTING;
    }
    if (strcmp(line, "+CW:CONNECTED") == 0) {
        return URC_WIFI_CONNECTED;
    }
    if (strncmp(line, "+CW:GOTIP", 9) == 0) {
        return URC_WIFI_GOTIP;
    }
    if (strncmp(line, "+CW:ERROR,", 10) == 0) {
        return URC_WIFI_ERROR;
    }
    if (strcmp(line, "+CW:DISCONNECTED") == 0) {
        return URC_WIFI_DISCONNECTED;
    }
    if (strcmp(line, "+CW:SCAN_DONE") == 0) {
        return URC_WIFI_SCAN_DONE;
    }

    /* AP mode URC */
    if (strncmp(line, "+CW:STA_CONNECTED,", 18) == 0) {
        return URC_AP_STA_CONNECTED;
    }
    if (strncmp(line, "+CW:STA_DISCONNECTED,", 21) == 0) {
        return URC_AP_STA_DISCONNECTED;
    }
    if (strncmp(line, "+CW:DIST_STA_IP,", 16) == 0) {
        return URC_AP_STA_IP_ASSIGNED;
    }

    /* Scan results */
    if (strncmp(line, "+CWLAP:", 7) == 0) {
        return URC_CWLAP;
    }

    return URC_UNKNOWN;
}

/**
 * @brief Parse +CWLAP line
 * @param line Input line, format: +CWLAP:<ecn>,<ssid>,<rssi>,<mac>,<channel>,...
 * @param ap Output AP information
 * @return 0 for success, negative error code for failure
 */
static int parse_cwlap(const char *line, bflbwifi_ap_info_t *ap)
{
    char *token;
    char buf[512];
    int field = 0;

    if (!line || !ap) {
        return -1;
    }

    /* Copy line for parsing */
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    /* Skip +CWLAP: prefix */
    char *start = strchr(buf, ':');
    if (!start) {
        return -1;
    }
    start++;

    /* Parse each field */
    token = strtok(start, ",");
    while (token && field < 5) {
        switch (field) {
            case 0: /* ecn - Encryption mode */
                ap->enc = atoi(token);
                break;
            case 1: /* ssid - Surrounded by double quotes */
                ap->ssid[0] = '\0';  /* Initialize to empty string */
                if (token[0] == '"') {
                    strncpy(ap->ssid, token + 1, sizeof(ap->ssid) - 1);
                    /* Remove trailing double quote */
                    char *end = strchr(ap->ssid, '"');
                    if (end) *end = '\0';
                    else ap->ssid[sizeof(ap->ssid) - 1] = '\0';  /* Ensure termination */
                } else {
                    strncpy(ap->ssid, token, sizeof(ap->ssid) - 1);
                    ap->ssid[sizeof(ap->ssid) - 1] = '\0';  /* Ensure termination */
                }
                break;
            case 2: /* rssi - Signal strength */
                ap->rssi = atoi(token);
                break;
            case 3: /* mac - Surrounded by double quotes */
                ap->bssid[0] = '\0';  /* Initialize to empty string */
                if (token[0] == '"') {
                    strncpy(ap->bssid, token + 1, sizeof(ap->bssid) - 1);
                    char *end = strchr(ap->bssid, '"');
                    if (end) *end = '\0';
                    else ap->bssid[sizeof(ap->bssid) - 1] = '\0';  /* Ensure termination */
                } else {
                    strncpy(ap->bssid, token, sizeof(ap->bssid) - 1);
                    ap->bssid[sizeof(ap->bssid) - 1] = '\0';  /* Ensure termination */
                }
                break;
            case 4: /* channel - Channel */
                ap->channel = atoi(token);
                break;
        }
        field++;
        token = strtok(NULL, ",");
    }

    return 0;
}

/**
 * @brief Parse CW:ERROR line
 * @param line Input line, format: +CW:ERROR,<reason_code>
 * @return Error reason code
 */
static int parse_wifi_error(const char *line)
{
    if (!line) {
        return 0;
    }

    /* Find number after comma */
    const char *p = strchr(line, ',');
    if (p) {
        return atoi(p + 1);
    }

    return 0;
}

static int parse_cwjap_error_code(const char *line, int *error_code)
{
    const char *value;
    char tail[2];
    int parsed_error;

    if (!line || !error_code || strncmp(line, "+CWJAP:", 7) != 0) {
        return -1;
    }

    value = line + 7;
    if (strchr(value, ',') != NULL || strchr(value, '"') != NULL) {
        return -1;
    }

    if (sscanf(value, "%d%1s", &parsed_error, tail) != 1) {
        return -1;
    }

    *error_code = parsed_error;
    return 0;
}

/**
 * @brief Parse MAC address (AP mode station information)
 * @param line Input line, e.g.: +CW:STA_CONNECTED "18:b9:05:ed:59:cc"
 * @param mac Output MAC address
 * @return 0 for success, negative error code for failure
 */
static int parse_mac(const char *line, char *mac)
{
    const char *start, *end;

    if (!line || !mac) {
        return -1;
    }

    /* Find MAC surrounded by double quotes */
    start = strchr(line, '"');
    if (!start) {
        return -1;
    }
    start++;

    end = strchr(start, '"');
    if (!end) {
        return -1;
    }

    /* Copy MAC address */
    int len = end - start;
    if (len >= 18) {
        return -1;
    }
    memcpy(mac, start, len);
    mac[len] = '\0';

    return 0;
}

/* ========== External Interfaces (for state manager) ========== */

/* Forward declarations */
void bflbwifi_state_handle_urc(int type, const char *line);
void bflbwifi_state_handle_cwjap(const char *line);
void bflbwifi_state_handle_cwjap_error(int error_code);

/**
 * @brief Handle received data line
 * @param line Data line
 * @param user_data User data (pointing to state manager, unused)
 */
void bflbwifi_parser_process_line(const char *line, void *user_data)
{
    (void)user_data;  /* Unused parameter */
    urc_type_t type;

    if (!line || line[0] == '\0') {
        return;
    }

    BFLB_LOGD("Parser: %s", line);

    /* Special handling: +CWJAP response (not URC, but needs to update connection information) */
    if (strncmp(line, "+CWJAP:", 7) == 0) {
        int error_code;

        if (parse_cwjap_error_code(line, &error_code) == 0) {
            bflbwifi_state_handle_cwjap_error(error_code);
            return;
        }

        bflbwifi_state_handle_cwjap(line);
        return;
    }

    /* Determine if it is URC */
    if (!is_urc(line)) {
        /* Normal response, handled by command manager */
        BFLB_LOGD("Not a URC: %s", line);
        return;
    }

    /* Identify URC type */
    type = identify_urc(line);
    if (type == URC_UNKNOWN) {
        BFLB_LOGD("Unknown URC: %s", line);
        return;
    }

    /* Notify state manager */
    bflbwifi_state_handle_urc(type, line);
}

/**
 * @brief Helper function: Parse CWLAP
 */
int bflbwifi_parser_parse_cwlap(const char *line, bflbwifi_ap_info_t *ap)
{
    return parse_cwlap(line, ap);
}

/**
 * @brief Helper function: Parse WiFi error code
 */
int bflbwifi_parser_parse_wifi_error(const char *line)
{
    return parse_wifi_error(line);
}

/**
 * @brief Helper function: Parse MAC address
 */
int bflbwifi_parser_parse_mac(const char *line, char *mac)
{
    return parse_mac(line, mac);
}

/**
 * @brief Parse AT+CWJAP? response
 * @param line Response line, format: +CWJAP:<ssid>,<bssid>,<channel>,<rssi>,<wep_en>
 * @param info Output: WiFi connection information
 * @return 0=success, -1=failure
 */
int bflbwifi_parser_parse_cwjap(const char *line, bflbwifi_sta_connection_info_t *info)
{
    char *token;
    char *rest;
    char line_copy[256];

    if (!line || !info) {
        return -1;
    }

    /* Copy line content for strtok */
    strncpy(line_copy, line, sizeof(line_copy) - 1);
    line_copy[sizeof(line_copy) - 1] = '\0';

    /* Skip "+CWJAP:" prefix */
    rest = strstr(line_copy, "+CWJAP:");
    if (!rest) {
        return -1;
    }
    rest += 7;  /* Skip "+CWJAP:" */

    /* Parse SSID (quoted string) */
    token = strtok_r(rest, ",", &rest);
    if (!token) {
        return -1;
    }
    /* Remove quotes */
    if (token[0] == '"') {
        size_t len = strlen(token);
        memmove(token, token + 1, len);  /* Remove leading quote */
        if (token[len - 2] == '"') {
            token[len - 2] = '\0';  /* Remove trailing quote */
        }
    }
    strncpy(info->ssid, token, sizeof(info->ssid) - 1);
    info->ssid[sizeof(info->ssid) - 1] = '\0';

    /* Parse BSSID */
    token = strtok_r(rest, ",", &rest);
    if (!token) {
        return -1;
    }
    if (token[0] == '"') {
        size_t len = strlen(token);
        memmove(token, token + 1, len);
        if (token[len - 2] == '"') {
            token[len - 2] = '\0';
        }
    }
    strncpy(info->bssid, token, sizeof(info->bssid) - 1);
    info->bssid[sizeof(info->bssid) - 1] = '\0';

    /* Parse Channel */
    token = strtok_r(rest, ",", &rest);
    if (!token) {
        return -1;
    }
    info->channel = (uint8_t)atoi(token);

    /* Parse RSSI */
    token = strtok_r(rest, ",", &rest);
    if (!token) {
        return -1;
    }
    info->rssi = (int8_t)atoi(token);

    /* Parse wep_en (ignore, not needed) */

    return 0;
}

#ifdef BFLBWIFI_NETIF_AUTO_CONFIG
/**
 * @brief Parse +CW:GOTIP URC with parameters
 * @param line Input line, format: +CW:GOTIP,IP:xxx,gw:xxx,mask:xxx,dns:xxx
 * @param ip Output IP address buffer (at least 16 bytes)
 * @param gw Output gateway buffer (at least 16 bytes)
 * @param mask Output netmask buffer (at least 16 bytes)
 * @param dns Output DNS buffer (at least 16 bytes)
 * @return 0=success, -1=failure
 */
int bflbwifi_parser_parse_gotip(const char *line,
                                  char *ip,
                                  char *gw,
                                  char *mask,
                                  char *dns)
{
    const char *p;
    const char *value_start;
    int value_len;

    if (!line || !ip || !gw || !mask || !dns) {
        return -1;
    }

    /* 初始化输出缓冲区 */
    ip[0] = '\0';
    gw[0] = '\0';
    mask[0] = '\0';
    dns[0] = '\0';

    /* 检查是否是 GOTIP URC */
    if (strncmp(line, "+CW:GOTIP", 9) != 0) {
        return -1;
    }

    /* 跳过 "+CW:GOTIP," 前缀 */
    p = strchr(line, ',');
    if (!p) {
        /* 无参数的 GOTIP，返回成功但内容为空 */
        return 0;
    }
    p++;  /* 跳过逗号 */

    /* 解析 IP:xxx */
    if (strncmp(p, "IP:", 3) == 0) {
        value_start = p + 3;
        p = strchr(value_start, ',');
        if (p) {
            value_len = p - value_start;
        } else {
            value_len = strlen(value_start);
        }
        if (value_len > 0 && value_len < 16) {
            snprintf(ip, 16, "%.*s", value_len, value_start);
        }
        if (p) p++;  /* 跳过逗号 */
    } else {
        return -1;
    }

    /* 解析 gw:xxx */
    if (p && strncmp(p, "gw:", 3) == 0) {
        value_start = p + 3;
        p = strchr(value_start, ',');
        if (p) {
            value_len = p - value_start;
        } else {
            value_len = strlen(value_start);
        }
        if (value_len > 0 && value_len < 16) {
            snprintf(gw, 16, "%.*s", value_len, value_start);
        }
        if (p) p++;  /* 跳过逗号 */
    }

    /* 解析 mask:xxx */
    if (p && strncmp(p, "mask:", 5) == 0) {
        value_start = p + 5;
        p = strchr(value_start, ',');
        if (p) {
            value_len = p - value_start;
        } else {
            value_len = strlen(value_start);
        }
        if (value_len > 0 && value_len < 16) {
            snprintf(mask, 16, "%.*s", value_len, value_start);
        }
        if (p) p++;  /* 跳过逗号 */
    }

    /* 解析 dns:xxx */
    if (p && strncmp(p, "dns:", 4) == 0) {
        value_start = p + 4;
        value_len = strlen(value_start);
        if (value_len > 0 && value_len < 16) {
            snprintf(dns, 16, "%.*s", value_len, value_start);
        }
    }

    return 0;
}
#endif /* BFLBWIFI_NETIF_AUTO_CONFIG */
