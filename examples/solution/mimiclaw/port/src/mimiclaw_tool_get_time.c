#include "tools/tool_get_time.h"

#include "esp_err.h"
#include "esp_log.h"
#include "bflb_rtc.h"
#include "mimiclaw_ext_rtc.h"
#include "cJSON.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <sys/time.h>
#include <unistd.h>

static const char *TAG = "tool_time";

#define NTP_PORT_STR "123"
#define NTP_PACKET_LEN 48
#define NTP_TIMEOUT_MS 3000
#define NTP_UNIX_EPOCH_DELTA 2208988800UL

static const char *s_ntp_servers[] = {
    "pool.ntp.org",
    "time.google.com",
    "ntp.aliyun.com",
};

static bool rtc_time_plausible(void)
{
    struct bflb_tm tm;
    int year;
    int month;
    int day;

    bflb_rtc_get_utc_time(&tm);
    year = tm.tm_year + 1900;
    month = tm.tm_mon + 1;
    day = tm.tm_mday;

    if (year < 2024 || year > 2199) {
        return false;
    }
    if (month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }
    if (tm.tm_hour < 0 || tm.tm_hour > 23 ||
        tm.tm_min < 0 || tm.tm_min > 59 ||
        tm.tm_sec < 0 || tm.tm_sec > 59) {
        return false;
    }

    return true;
}

static int day_of_year(int year, int month, int day)
{
    static const int day_offset[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
    bool leap;
    int yday;

    leap = ((year % 4) == 0 && (year % 100) != 0) || ((year % 400) == 0);
    yday = day_offset[month - 1] + day - 1;
    if (leap && month > 2) {
        yday++;
    }

    return yday;
}

static void unix_to_tm_utc(uint32_t unix_sec, struct bflb_tm *tm)
{
    uint32_t sec_of_day = unix_sec % 86400U;
    int64_t days = unix_sec / 86400U;
    int64_t z = days + 719468;
    int64_t era = (z >= 0 ? z : z - 146096) / 146097;
    uint32_t doe = (uint32_t)(z - era * 146097);
    uint32_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int year = (int)yoe + (int)era * 400;
    uint32_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    uint32_t mp = (5 * doy + 2) / 153;
    int day = (int)(doy - (153 * mp + 2) / 5 + 1);
    int month = (int)(mp + (mp < 10 ? 3 : -9));
    int wday = (int)((days + 4) % 7);

    if (month <= 2) {
        year++;
    }
    if (wday < 0) {
        wday += 7;
    }

    tm->tm_sec = (int)(sec_of_day % 60U);
    tm->tm_min = (int)((sec_of_day / 60U) % 60U);
    tm->tm_hour = (int)(sec_of_day / 3600U);
    tm->tm_mday = day;
    tm->tm_mon = month - 1;
    tm->tm_year = year - 1900;
    tm->tm_wday = wday;
    tm->tm_yday = day_of_year(year, month, day);
}

static bool format_utc_time(char *out, size_t out_size)
{
    struct bflb_tm tm_utc;
    int n;

    bflb_rtc_get_utc_time(&tm_utc);
    n = snprintf(out,
                 out_size,
                 "%04d-%02d-%02d %02d:%02d:%02d UTC",
                 tm_utc.tm_year + 1900,
                 tm_utc.tm_mon + 1,
                 tm_utc.tm_mday,
                 tm_utc.tm_hour,
                 tm_utc.tm_min,
                 tm_utc.tm_sec);
    return n > 0 && (size_t)n < out_size;
}

static bool sync_from_external_rtc(void)
{
    struct bflb_tm tm;

    if (!mimiclaw_ext_rtc_is_available()) {
        return false;
    }

    if (!mimiclaw_ext_rtc_read_utc(&tm)) {
        return false;
    }

    bflb_rtc_set_utc_time(&tm);
    return true;
}

static void sync_to_external_rtc(void)
{
    struct bflb_tm tm;

    if (!mimiclaw_ext_rtc_is_available()) {
        return;
    }

    bflb_rtc_get_utc_time(&tm);
    (void)mimiclaw_ext_rtc_write_utc(&tm);
}

static esp_err_t sync_time_once_from_server(const char *server)
{
    struct addrinfo hints;
    struct addrinfo *res = NULL;
    struct addrinfo *ai;
    int gai_ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    gai_ret = getaddrinfo(server, NTP_PORT_STR, &hints, &res);
    if (gai_ret != 0 || !res) {
        return ESP_FAIL;
    }

    for (ai = res; ai; ai = ai->ai_next) {
        uint8_t req[NTP_PACKET_LEN] = { 0 };
        uint8_t resp[NTP_PACKET_LEN] = { 0 };
        struct timeval tv;
        int sockfd;
        int n;
        uint32_t ntp_sec;
        uint32_t unix_sec;
        struct bflb_tm utc_tm;

        sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sockfd < 0) {
            continue;
        }

        tv.tv_sec = NTP_TIMEOUT_MS / 1000;
        tv.tv_usec = (NTP_TIMEOUT_MS % 1000) * 1000;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

        req[0] = 0x1b;
        n = sendto(sockfd, req, sizeof(req), 0, ai->ai_addr, ai->ai_addrlen);
        if (n != NTP_PACKET_LEN) {
            close(sockfd);
            continue;
        }

        n = recv(sockfd, resp, sizeof(resp), 0);
        close(sockfd);
        if (n < NTP_PACKET_LEN) {
            continue;
        }

        ntp_sec = ((uint32_t)resp[40] << 24) |
                  ((uint32_t)resp[41] << 16) |
                  ((uint32_t)resp[42] << 8) |
                  ((uint32_t)resp[43] << 0);

        if (ntp_sec <= NTP_UNIX_EPOCH_DELTA) {
            continue;
        }

        unix_sec = ntp_sec - NTP_UNIX_EPOCH_DELTA;
        unix_to_tm_utc(unix_sec, &utc_tm);
        bflb_rtc_set_utc_time(&utc_tm);

        freeaddrinfo(res);
        return ESP_OK;
    }

    freeaddrinfo(res);
    return ESP_FAIL;
}

esp_err_t tool_get_time_execute(const char *input_json, char *output, size_t output_size)
{
    size_t i;
    bool force_sync = false;
    bool have_valid_rtc;
    esp_err_t ret = ESP_FAIL;
    cJSON *root = NULL;

    if (!output || output_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (input_json && input_json[0] != '\0') {
        root = cJSON_Parse(input_json);
        if (root) {
            cJSON *sync = cJSON_GetObjectItem(root, "sync");
            if (cJSON_IsBool(sync)) {
                force_sync = cJSON_IsTrue(sync);
            }
        }
    }

    have_valid_rtc = rtc_time_plausible();

    if (force_sync || !have_valid_rtc) {
        for (i = 0; i < sizeof(s_ntp_servers) / sizeof(s_ntp_servers[0]); i++) {
            ret = sync_time_once_from_server(s_ntp_servers[i]);
            if (ret == ESP_OK) {
                sync_to_external_rtc();
                have_valid_rtc = rtc_time_plausible();
                break;
            }
        }

        if (ret != ESP_OK && !have_valid_rtc) {
            if (sync_from_external_rtc()) {
                have_valid_rtc = rtc_time_plausible();
            }
        }
    }

    if (root) {
        cJSON_Delete(root);
    }

    if (!have_valid_rtc) {
        if (!sync_from_external_rtc() || !rtc_time_plausible()) {
            snprintf(output, output_size, "Error: failed to sync time from NTP and external RTC");
            ESP_LOGE(TAG, "%s", output);
            return ret;
        }
        ESP_LOGW(TAG, "RTC not initialized, fallback to external RTC");
    }

    if (!format_utc_time(output, output_size)) {
        snprintf(output, output_size, "Error: time synced but formatting failed");
        ESP_LOGE(TAG, "%s", output);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Time: %s", output);
    return ESP_OK;
}
