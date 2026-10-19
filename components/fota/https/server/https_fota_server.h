#ifndef HTTPS_FOTA_SERVER_H
#define HTTPS_FOTA_SERVER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Opaque handle for FOTA server */
typedef void *https_fota_server_handle_t;

#define OTA_FIRMWARE_URL_MAX              512
#define HTTPS_FOTA_SERVER_URI_MAX         63
#define HTTPS_FOTA_SERVER_DEFAULT_UPLOAD_URI "/api/ota"
#define HTTPS_FOTA_SERVER_DEFAULT_SUCCESS_URI "/ota-success.json"
#define HTTPS_FOTA_SERVER_DEFAULT_ERROR_URI "/ota-error.json"

/**
 * @brief HTTPS FOTA server operation status codes
 */
typedef enum {
    HTTPS_FOTA_SERVER_SUCCESS = 0,       /**< Operation completed successfully */
    HTTPS_FOTA_SERVER_START,             /**< FOTA process started */
    HTTPS_FOTA_SERVER_PROCESS_TRANSFER,  /**< Firmware transfer in progress */
    HTTPS_FOTA_SERVER_TRANSFER_FINISH,   /**< Firmware transfer completed */
    HTTPS_FOTA_SERVER_IMAGE_VERIFY,      /**< Firmware image verification started */
    HTTPS_FOTA_SERVER_IMAGE_VERIFY_FAIL, /**< Firmware image verification failed */
    HTTPS_FOTA_SERVER_ABORT,             /**< FOTA process aborted */
} https_fota_server_status_t;

/**
 * @brief Callback function type for FOTA server status notifications
 * @param arg User-provided context pointer
 * @param event Current FOTA server status event
 */
typedef void (*pfn_https_fota_server_t)(void *arg,
                                        https_fota_server_status_t event);

/**
 * @brief Configuration structure for HTTPS FOTA server
 */
struct https_fota_server_config {
    pfn_https_fota_server_t callback; /**< Status callback function */
    void *user_arg;                   /**< User context passed to callback */
    bool reboot;                      /**< Reboot after a successful update */

    const unsigned char *cert_pem;    /**< SSL server certificate in PEM format */
    size_t cert_len;                  /**< Length of server certificate */
    const unsigned char *key_pem;     /**< SSL server private key in PEM format */
    size_t key_len;                   /**< Length of server private key */

    char upload_uri[HTTPS_FOTA_SERVER_URI_MAX];  /**< URI prefix for OTA upload requests */
    char success_uri[HTTPS_FOTA_SERVER_URI_MAX]; /**< URI for a successful OTA response */
    char error_uri[HTTPS_FOTA_SERVER_URI_MAX];   /**< URI for a failed OTA response */
};

/**
 * @brief Initialize a new HTTPS FOTA server session
 * @param url Firmware URL shown to the OTA web client
 * @param config Configuration parameters for the FOTA server
 * @return FOTA server handle on success, NULL if a server is active or on failure
 * @note The handle is released after finish or abort and must not be reused.
 */
https_fota_server_handle_t https_fota_server_init(
    const char *url, const struct https_fota_server_config *config);

/**
 * @brief Register the FOTA session and start HTTP/HTTPS listeners once
 * @param server FOTA server handle
 * @return 0 on success, negative error code on failure
 * @note The first session supplies the certificate used by the persistent HTTPS listener.
 */
int https_fota_server_start(https_fota_server_handle_t server);

/**
 * @brief Pass a received firmware block to the OTA writer
 * @param server FOTA server handle
 * @param data Firmware block data
 * @param size Firmware block size
 * @return 0 on success, negative error code on failure
 * @note A failed update ends the session; the handle must not be reused.
 */
int https_fota_server_update(https_fota_server_handle_t server,
                             const uint8_t *data, uint32_t size);

/**
 * @brief Register a callback function for status updates
 * @param server FOTA server handle
 * @param callback Callback function pointer
 * @param user_arg User context to pass to the callback
 * @return 0 on success, negative error code on failure
 */
int https_fota_server_callback_register(https_fota_server_handle_t server,
                                        pfn_https_fota_server_t callback,
                                        void *user_arg);

/**
 * @brief Finalize the FOTA process
 * @param server FOTA server handle
 * @param reboot Whether to reboot after a successful update response
 * @return 0 on success, negative error code on failure
 * @note Ends and releases the session. HTTPD cleanup is deferred until the response is opened.
 *       The handle must not be reused; HTTP/HTTPS listeners remain running.
 */
int https_fota_server_finish(https_fota_server_handle_t server, bool reboot);

/**
 * @brief Abort and release the current FOTA server session
 * @param server FOTA server handle
 * @return 0 on success, negative error code on failure
 * @note The handle must not be reused; HTTP/HTTPS listeners remain running.
 */
int https_fota_server_abort(https_fota_server_handle_t server);

/**
 * @brief Get the firmware URL of the active FOTA server
 * @return Firmware URL, or an empty string if no server is active
 */
const char *https_fota_server_url(void);

/**
 * @brief Get the configuration of the active FOTA server
 * @return Active server configuration, or NULL if no server is active
 */
const struct https_fota_server_config *https_fota_server_get_config(void);

#ifdef __cplusplus
}
#endif

#endif
