#include <stdio.h>
#include <string.h>

#include "https_fota_server.h"
#include "shell.h"

__attribute__((weak)) int app_https_fota_server_fill_config(struct https_fota_server_config *config)
{
    (void)config;
    return 0;
}

static void __ota_status_cb(void *arg, https_fota_server_status_t event)
{
    (void)arg;

    switch (event) {
        case HTTPS_FOTA_SERVER_START:
            printf("HTTPS_FOTA_SERVER_START\r\n");
            break;
        case HTTPS_FOTA_SERVER_PROCESS_TRANSFER:
            printf("HTTPS_FOTA_SERVER_PROCESS_TRANSFER\r\n");
            break;
        case HTTPS_FOTA_SERVER_TRANSFER_FINISH:
            printf("HTTPS_FOTA_SERVER_TRANSFER_FINISH\r\n");
            break;
        case HTTPS_FOTA_SERVER_IMAGE_VERIFY:
            printf("HTTPS_FOTA_SERVER_IMAGE_VERIFY\r\n");
            break;
        case HTTPS_FOTA_SERVER_IMAGE_VERIFY_FAIL:
            printf("HTTPS_FOTA_SERVER_IMAGE_VERIFY_FAIL\r\n");
            break;
        case HTTPS_FOTA_SERVER_SUCCESS:
            printf("HTTPS_FOTA_SERVER_SUCCESS\r\n");
            break;
        case HTTPS_FOTA_SERVER_ABORT:
            printf("HTTPS_FOTA_SERVER_ABORT\r\n");
            break;
        default:
            break;
    }
}

static int cmd_https_server_ota_start(int argc, char **argv)
{
    https_fota_server_handle_t server;
    int ret;

    struct https_fota_server_config config = {0};

    if (argc < 2 || argc > 3) {
        printf("Usage: https_server_ota_start <url> [reboot: 0|1, default: 1]\r\n");
        return -1;
    }
    config.callback = __ota_status_cb;
    config.reboot = true;
    if (argc == 3) {
        if (!strcmp(argv[2], "0")) {
            config.reboot = false;
        } else if (strcmp(argv[2], "1")) {
            printf("Usage: https_server_ota_start <url> [reboot: 0|1, default: 1]\r\n");
            return -1;
        }
    }

    ret = app_https_fota_server_fill_config(&config);
    if (ret != 0) {
        printf("HTTPS server OTA TLS configuration failed: %d\r\n", ret);
        return ret;
    }
    server = https_fota_server_init(argv[1], &config);
    if (!server) {
        return -1;
    }
    ret = https_fota_server_start(server);
    if (ret != 0) {
        printf("HTTPS server OTA start failed\r\n");
        https_fota_server_abort(server);
    }
    return ret;
}

SHELL_CMD_EXPORT_ALIAS(cmd_https_server_ota_start, https_server_ota_start,
                       HTTPS server OTA);
