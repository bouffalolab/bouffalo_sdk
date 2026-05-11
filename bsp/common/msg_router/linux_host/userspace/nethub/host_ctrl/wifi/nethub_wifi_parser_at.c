#include "nethub_wifi_parser_at.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int next_csv_field(const char **cursor, char *out, size_t out_size)
{
    const char *p;
    size_t out_len = 0;

    if (!cursor || !*cursor || !out || out_size == 0) {
        return -1;
    }

    p = *cursor;
    if (*p == '"') {
        p++;
        while (*p != '\0') {
            char ch = *p++;

            if (ch == '\\' && *p != '\0') {
                ch = *p++;
            } else if (ch == '"') {
                break;
            }

            if (out_len + 1u < out_size) {
                out[out_len++] = ch;
            }
        }

        while (*p != '\0' && *p != ',') {
            p++;
        }
    } else {
        while (*p != '\0' && *p != ',') {
            if (out_len + 1u < out_size) {
                out[out_len++] = *p;
            }
            p++;
        }
    }

    if (*p == ',') {
        p++;
    }

    out[out_len] = '\0';
    *cursor = p;
    return 0;
}

int nethub_at_parse_cwlap(const char *line, nethub_wifi_ap_info_t *ap)
{
    const char *cursor;
    char field[64];

    if (!line || !ap || strncmp(line, "+CWLAP:", 7) != 0) {
        return -1;
    }

    memset(ap, 0, sizeof(*ap));
    cursor = strchr(line, ':');
    if (!cursor) {
        return -1;
    }
    cursor++;

    if (next_csv_field(&cursor, field, sizeof(field)) != 0) {
        return -1;
    }
    ap->enc = (nethub_wifi_enc_t)atoi(field);

    if (next_csv_field(&cursor, ap->ssid, sizeof(ap->ssid)) != 0) {
        return -1;
    }

    if (next_csv_field(&cursor, field, sizeof(field)) != 0) {
        return -1;
    }
    ap->rssi = (int8_t)atoi(field);

    if (next_csv_field(&cursor, ap->bssid, sizeof(ap->bssid)) != 0) {
        return -1;
    }

    if (next_csv_field(&cursor, field, sizeof(field)) != 0) {
        return -1;
    }
    ap->channel = (uint8_t)atoi(field);

    return 0;
}

int nethub_at_parse_cwjap_error(const char *line, int *wifi_reason)
{
    const char *value;
    char tail[2];
    int reason;

    if (!line || !wifi_reason || strncmp(line, "+CWJAP:", 7) != 0) {
        return -1;
    }

    value = line + 7;
    if (strchr(value, ',') || strchr(value, '"')) {
        return -1;
    }

    if (sscanf(value, "%d%1s", &reason, tail) != 1) {
        return -1;
    }

    *wifi_reason = reason;
    return 0;
}

int nethub_at_parse_cwjap(const char *line, nethub_wifi_status_t *status)
{
    const char *cursor;
    char field[64];

    if (!line || !status || strncmp(line, "+CWJAP:", 7) != 0) {
        return -1;
    }

    cursor = line + 7;

    if (next_csv_field(&cursor, status->ssid, sizeof(status->ssid)) != 0) {
        return -1;
    }

    if (next_csv_field(&cursor, status->bssid, sizeof(status->bssid)) != 0) {
        return -1;
    }

    if (next_csv_field(&cursor, field, sizeof(field)) != 0) {
        return -1;
    }
    status->channel = (uint8_t)atoi(field);

    if (next_csv_field(&cursor, field, sizeof(field)) != 0) {
        return -1;
    }
    status->rssi = (int8_t)atoi(field);
    status->sta_state = NETHUB_WIFI_STA_CONNECTED;
    return 0;
}

static void parse_ip_kv(char *dst, size_t dst_size, const char *line, const char *key)
{
    const char *p;
    size_t key_len = strlen(key);
    size_t len;

    if (!dst || dst_size == 0) {
        return;
    }

    p = strstr(line, key);
    if (!p) {
        return;
    }
    p += key_len;
    len = strcspn(p, ",\r\n");
    if (len >= dst_size) {
        len = dst_size - 1u;
    }
    memcpy(dst, p, len);
    dst[len] = '\0';
}

int nethub_at_parse_gotip(const char *line, nethub_wifi_status_t *status)
{
    if (!line || !status || strncmp(line, "+CW:GOTIP", 9) != 0) {
        return -1;
    }

    parse_ip_kv(status->ip, sizeof(status->ip), line, "IP:");
    parse_ip_kv(status->gateway, sizeof(status->gateway), line, "gw:");
    parse_ip_kv(status->netmask, sizeof(status->netmask), line, "mask:");
    parse_ip_kv(status->dns, sizeof(status->dns), line, "dns:");
    status->sta_state = NETHUB_WIFI_STA_GOT_IP;
    return 0;
}

int nethub_at_parse_wifi_error(const char *line)
{
    const char *p;

    if (!line || strncmp(line, "+CW:ERROR,", 10) != 0) {
        return 0;
    }

    p = strchr(line, ',');
    return p ? atoi(p + 1) : 0;
}
