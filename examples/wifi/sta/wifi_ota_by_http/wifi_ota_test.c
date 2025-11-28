
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "https_fota.h"

const char ca_cert[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDAzCCAeugAwIBAgIUSIynf9c+C4TGvBNrpOGW0EsWKRUwDQYJKoZIhvcNAQEL\n"
    "BQAwETEPMA0GA1UEAwwGTXlDQV8xMB4XDTI0MDUyMzAxNTIzOFoXDTI1MDUyMzAx\n"
    "NTIzOFowETEPMA0GA1UEAwwGTXlDQV8xMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A\n"
    "MIIBCgKCAQEAj7Xp94L7VLyo6m4AdvKIa1pOjZQUzPnfgkAJhdHmupIMhdx8/L/i\n"
    "Yn55tZhPOzk7S+CYuYoH1Bx/tvmP+HJ9J8qeNOPm+QbiYNUSJcs2vXkKMFgiSqh4\n"
    "zE6B80+clSSR0de80s7td0PTL9k/emQSu+mV1q2JrXyhcP6/fumt1TzrmTb2SxiB\n"
    "rMQh76v8NfV0a0Y78QDd+S0JIhJ27S610DCSnl+B1pgI8sMuwtSSNOyNYFbym2PO\n"
    "LYUYaN9Lup7i2pxPi/p/tIwBq3mbs01JawBMFArTkidxIK+xkf4NVUT1weurw+s8\n"
    "3yC+pomUtvKpCNxdgSslnCxNhoUYxxCrawIDAQABo1MwUTAdBgNVHQ4EFgQUz1kI\n"
    "khTZxHjVYlOk16H7v57pJCowHwYDVR0jBBgwFoAUz1kIkhTZxHjVYlOk16H7v57p\n"
    "JCowDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEASyul5PoQ8V0M\n"
    "+lKOCxeL8qbYTTLJzcIGcN7FQzTWHYqfmdlc+9IZ7mHk1RR+fxNll3EpHaqA/U9J\n"
    "bRUiBIzsMRWaCl0g3NCcZOILQ/hfgh6TgFyo5bOUKNOjBKGrV4/mnOOyPvPc5n/H\n"
    "BW3LjqmfE2CFeUruXutOPIkY8YDiPRFwCSaNapO4Bc6+kgYQz7dPEWOF0b2IcJ7O\n"
    "vD2JPeKSoUxPs71TUNzGBQx+mbXxULrhFkSs9jRdPUJQpJqaq+SVLLXOU+RsQab0\n"
    "dqud+4N3VgjA1tI/Oi23sF4xMotaWH4VrWIxuu0O8I3GYx+ZhFyhRa33ma7JxZtu\n"
    "VlA0gHsH5Q==\n"
    "-----END CERTIFICATE-----\n";

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
        GLB_SW_POR_Reset();
        break;

        case HTTPS_FOTA_ABORT:
        *ota_progress = 0;
        printf("HTTPS_FOTA_ABORT\r\n");
        break;

        default:
        break;
    }
}

int cmd_wifi_ota_test(int argc, char **argv)
{
    int ret;
    char *url;
    static uint8_t ota_progress = 0;

    struct https_fota_config config = {0};

    /* default cmd like this: bl616 />wifi_ota_test http://192.168.31.112:5000/build/build_out/wifi_ota_bl616.bin.ota */
    if (argc <= 1) {
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
    config.ca_pem = ca_cert;
    config.ca_len = sizeof(ca_cert);
    ret = https_fota(url, &config);

    if (ret < 0) {
        printf("https_fota fail\r\n");
    }

    return 0;
}

int cmd_wifi_ota_rollback(int argc, char **argv)
{
    https_ota_rollback();
    return 0;
}

#ifdef CONFIG_SHELL
#include <shell.h>

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ota_test, wifi_ota_test, wifi ota test);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ota_rollback, wifi_ota_rollback, wifi ota rollback test);
#endif
