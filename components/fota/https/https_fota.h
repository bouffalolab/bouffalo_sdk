/**
  ******************************************************************************
  * @file    https_fota.h
  * @version V1.0
  * @date
  * @brief   This file is https OTA feature
  ******************************************************************************
  */

#ifndef __HTTPS_FOTA_H__
#define __HTTPS_FOTA_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HTTPS FOTA operation status codes
 */
typedef enum {
    HTTPS_FOTA_SUCCESS = 0,         /**< Operation completed successfully */
    HTTPS_FOTA_START,               /**< FOTA process started */
    HTTPS_FOTA_SERVER_CONNECTE_FAIL,/**< Failed to connect to server */
    HTTPS_FOTA_PROCESS_TRANSFER,    /**< Firmware transfer in progress */
    HTTPS_FOTA_TRANSFER_FINISH,     /**< Firmware transfer completed */
    HTTPS_FOTA_IMAGE_VERIFY,        /**< Firmware image verification started */
    HTTPS_FOTA_IMAGE_VERIFY_FAIL,   /**< Firmware image verification failed */
    HTTPS_FOTA_ABORT,               /**< FOTA process aborted */
} https_fota_status_t;

/**
 * @brief Callback function type for FOTA status notifications
 * @param arg User-provided context pointer
 * @param event Current FOTA status event
 */
typedef void (*pfn_https_fota_t)(void *arg, https_fota_status_t event);

#define HTTPS_FOTA_BUFFER_SIZE         (4096)      /**< Default buffer size for FOTA operations */
#define HTTPS_FOTA_REQUEST_TIMEOUT_MS  (8*1000)    /**< Default timeout for HTTP requests in milliseconds */

/**
 * @brief Configuration structure for HTTPS FOTA
 */
struct https_fota_config {
    pfn_https_fota_t callback;      /**< Status callback function */
    void *user_arg;                 /**< User context passed to callback */

    const char *ca_pem;             /**< SSL server CA certificate in PEM format (for server verification) */
    size_t      ca_len;             /**< Length of CA certificate */
    const char *client_cert_pem;    /**< SSL client certificate in PEM format (for mutual TLS) */
    size_t      client_cert_len;    /**< Length of client certificate */
    const char *client_key_pem;     /**< SSL client private key in PEM format (for mutual TLS) */
    size_t      client_key_len;     /**< Length of client private key */
};

/** Opaque handle for FOTA session */
typedef void *https_fota_handle_t;

/**
 * @brief Initialize a new FOTA session
 * @param url HTTPS URL of the firmware image
 * @param config Configuration parameters for the FOTA session
 * @return FOTA session handle on success, NULL on failure
 */
https_fota_handle_t https_fota_init(const char *url, const struct https_fota_config *config);

/**
 * @brief Start the firmware update process
 * @param fota FOTA session handle
 * @return 0 on success, negative error code on failure
 */
int https_fota_start(https_fota_handle_t fota);

/**
 * @brief Register a callback function for status updates
 * @param fota FOTA session handle
 * @param pfn Callback function pointer
 * @param arg User context to pass to callback
 * @return 0 on success, negative error code on failure
 */
int https_fota_callback_register(https_fota_handle_t fota, pfn_https_fota_t pfn, void *arg);

/**
 * @brief Finalize the FOTA process
 * @param fota FOTA session handle
 * @param reboot Whether to reboot after successful update
 * @return 0 on success, negative error code on failure
 */
int https_fota_finish(https_fota_handle_t fota, bool reboot);

/**
 * @brief Abort the FOTA process
 * @param fota FOTA session handle
 * @return 0 on success, negative error code on failure
 */
int https_fota_abort(https_fota_handle_t fota);

/**
 * @brief Single-call convenience function for FOTA update
 * @param url HTTPS URL of the firmware image
 * @param config Configuration parameters
 * @return 0 on success, negative error code on failure
 * @note This combines init, start, and finish operations
 */
int https_fota(const char *url, const struct https_fota_config *config);

/**
 * @brief Roll back to the firmware version in the backup partition
 * @return 0 on success, negative error code on failure
 *
 * @note [Working Principle]
 * 1. This function writes specific flags to the bootloader to:
 *    - Boot from the backup partition (Partition B) on next restart
 *    - Mark the current active partition (Partition A) as requiring repair
 *
 * 2. Typical use cases:
 *    - Automatic triggering when new firmware fails to boot
 *    - Manual version downgrade by user
 *
 * @note [Usage Constraints]
 * 1. Requires dual-partition system design:
 *    - Both A/B partitions must contain complete firmware
 *    - Bootloader must support rollback logic
 *
 * 2. Execution flow:
 *    +---------------------+
 *    | Call rollback()      |
 *    | Write boot flags     | <-- System still runs on old partition
 *    +---------------------+
 *            |
 *            v
 *    [Manual reboot REQUIRED] <-- Critical step!
 *            |
 *            v
 *    +---------------------+
 *    | Bootloader checks flag|
 *    | Switches to backup   |
 *    +---------------------+
 *
 * @note [Implementation Guide]
 * 1. Usage example:
 * @code
 * if(https_ota_rollback() == 0) {
 *     printf("Rollback scheduled. Please reboot manually");
 *     // Should prompt user to reboot here
 * }
 * @endcode
 *
 * 2. Recommended to use with watchdog timer:
 *    - Enable 5-second watchdog after calling
 *    - Prevents system corruption if power loss occurs during flag write
 *
 */
int https_ota_rollback(void);


#ifdef __cplusplus
}
#endif

#endif
