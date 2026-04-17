#include <stddef.h>
#include <string.h>

#include "https_fota.h"
#include "https_ota_tls_material.h"

int app_https_ota_fill_config(const char *url, struct https_fota_config *config)
{
    if (!url || !config) {
        return -1;
    }

    if (strncmp(url, "https://", strlen("https://")) != 0) {
        return 0;
    }

#if defined(CONFIG_HTTPS_OTA_USE_CA_CERT)
    config->ca_pem = https_ota_ca_pem;
    config->ca_len = https_ota_ca_pem_len;
#endif

#if defined(CONFIG_HTTPS_OTA_USE_CLIENT_CERT)
    config->client_cert_pem = https_ota_client_cert_pem;
    config->client_cert_len = https_ota_client_cert_pem_len;
    config->client_key_pem = https_ota_client_key_pem;
    config->client_key_len = https_ota_client_key_pem_len;
#endif

    return 0;
}