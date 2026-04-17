/**
  ******************************************************************************
  * @file    https_fota_shell.c
  * @version V1.0
  * @date
  * @brief   Shell commands for HTTPS OTA
  ******************************************************************************
  */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "https_fota.h"
#include "shell.h"
#include "bl_sys.h"

__attribute__((weak)) int app_https_ota_fill_config(const char *url, struct https_fota_config *config)
{
    (void)url;
    (void)config;
    return 0;
}

static int https_ota_validate_config(const char *url, const struct https_fota_config *config)
{
    if (strncmp(url, "https://", strlen("https://")) != 0) {
        return 0;
    }

    if ((config->client_cert_pem && !config->client_key_pem) ||
        (!config->client_cert_pem && config->client_key_pem)) {
        printf("HTTPS OTA client certificate and key must be configured together\r\n");
        return -1;
    }

    printf("HTTPS OTA server verification: %s\r\n", config->ca_pem ? "enabled" : "disabled");
    printf("HTTPS OTA client authentication: %s\r\n",
           (config->client_cert_pem && config->client_key_pem) ? "enabled" : "disabled");
    return 0;
}

static void __ota_status_cb(void *arg, https_fota_status_t event)
{
    uint8_t *ota_progress = (uint8_t *)arg;

    switch (event) {
        case HTTPS_FOTA_START:
            printf("HTTPS_FOTA_START\r\n");
            break;

        case HTTPS_FOTA_SERVER_CONNECTE_FAIL:
            *ota_progress = 0;
            printf("HTTPS_FOTA_SERVER_CONNECTE_FAIL\r\n");
            break;

        case HTTPS_FOTA_PROCESS_TRANSFER:
            printf("HTTPS_FOTA_PROCESS_TRANSFER\r\n");
            break;

        case HTTPS_FOTA_TRANSFER_FINISH:
            printf("HTTPS_FOTA_TRANSFER_FINISH\r\n");
            break;

        case HTTPS_FOTA_IMAGE_VERIFY:
            printf("HTTPS_FOTA_IMAGE_VERIFY\r\n");
            break;

        case HTTPS_FOTA_IMAGE_VERIFY_FAIL:
            *ota_progress = 0;
            printf("HTTPS_FOTA_IMAGE_VERIFY_FAIL\r\n");
            break;

        case HTTPS_FOTA_SUCCESS:
            printf("HTTPS_FOTA_SUCCESS\r\n");
            *ota_progress = 0;
            vTaskDelay(pdMS_TO_TICKS(100));
            bl_sys_reset_por();
            break;

        case HTTPS_FOTA_ABORT:
            *ota_progress = 0;
            printf("HTTPS_FOTA_ABORT\r\n");
            break;

        default:
            break;
    }
}

/**
 * @brief Shell command: https_ota_start <url>
 *   e.g. https_ota_start http://192.168.31.112:5000/build/build_out/fw.bin.ota
 */
int cmd_https_ota_start(int argc, char **argv)
{
    int ret;
    char *url;
    static uint8_t ota_progress = 0;

    struct https_fota_config config = {0};

    if (argc <= 1) {
        printf("Usage: https_ota_start <url>\r\n");
        return -1;
    }
    if (ota_progress) {
        printf("OTA upgrade in progress\r\n");
        return -1;
    }

    ota_progress = 1;
    url = argv[1];

    config.callback = __ota_status_cb;
    config.user_arg = &ota_progress;

    ret = app_https_ota_fill_config(url, &config);
    if (ret != 0) {
        ota_progress = 0;
        printf("HTTPS OTA TLS configuration failed: %d\r\n", ret);
        return ret;
    }

    ret = https_ota_validate_config(url, &config);
    if (ret != 0) {
        ota_progress = 0;
        return ret;
    }

    ret = https_fota(url, &config);
    if (ret < 0) {
        ota_progress = 0;
        printf("https_fota fail\r\n");
    }

    return ret;
}

/**
 * @brief Shell command: https_ota_rollback
 */
int cmd_https_ota_rollback(int argc, char **argv)
{
    https_ota_rollback();
    return 0;
}

/* Keep backward-compatible command names from the old wifi_ota_by_http example */
SHELL_CMD_EXPORT_ALIAS(cmd_https_ota_start, wifi_ota_test, HTTPS OTA update);
SHELL_CMD_EXPORT_ALIAS(cmd_https_ota_start, https_ota_start, HTTPS OTA update);
SHELL_CMD_EXPORT_ALIAS(cmd_https_ota_rollback, wifi_ota_rollback, HTTPS OTA rollback);
SHELL_CMD_EXPORT_ALIAS(cmd_https_ota_rollback, https_ota_rollback, HTTPS OTA rollback);
