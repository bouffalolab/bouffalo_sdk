#include <stddef.h>
#include <string.h>

#include "https_fota.h"
#include "https_fota_server.h"
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

int app_https_fota_server_fill_config(struct https_fota_server_config *config)
{
    config->cert_pem = (const unsigned char *)https_server_ota_cert_pem;
    config->cert_len = https_server_ota_cert_pem_len;
    config->key_pem = (const unsigned char *)https_server_ota_key_pem;
    config->key_len = https_server_ota_key_pem_len;
    return 0;
}
