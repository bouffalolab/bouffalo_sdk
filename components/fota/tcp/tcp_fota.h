/**
  ******************************************************************************
  * @file    tcp_fota.h
  * @version V1.0
  * @date
  * @brief   This file is TCP OTA feature 
  ******************************************************************************
  */

#ifndef __TCP_FOTA_H__
#define __TCP_FOTA_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TCP_FOTA_DEFAULT_PORT "3365" /**< Default OTA server port */

#ifndef CONFIG_FOTA_TCP_RETRY_CNT_MAX
#define CONFIG_FOTA_TCP_RETRY_CNT_MAX 10
#endif

#ifndef CONFIG_FOTA_TCP_RETRY_DELAY_MS
#define CONFIG_FOTA_TCP_RETRY_DELAY_MS 1000
#endif

#ifndef CONFIG_FOTA_TCP_TASK_STACK_SIZE
#define CONFIG_FOTA_TCP_TASK_STACK_SIZE 2048
#endif

#ifndef CONFIG_FOTA_TCP_TASK_PRIORITY
#define CONFIG_FOTA_TCP_TASK_PRIORITY 20
#endif

/**
 * @brief TCP FOTA operation status codes
 */
typedef enum {
    TCP_FOTA_SUCCESS = 0,         /**< Operation completed successfully */
    TCP_FOTA_START,               /**< FOTA process started */
    TCP_FOTA_SERVER_CONNECT_FAIL, /**< Failed to connect to server */
    TCP_FOTA_PROCESS_TRANSFER,    /**< Firmware transfer in progress */
    TCP_FOTA_TRANSFER_FINISH,     /**< Firmware transfer completed */
    TCP_FOTA_IMAGE_VERIFY,        /**< Firmware image verification started */
    TCP_FOTA_IMAGE_VERIFY_FAIL,   /**< Firmware image verification failed */
    TCP_FOTA_ABORT,               /**< FOTA process aborted */
} tcp_fota_status_t;

/**
 * @brief Callback function type for TCP FOTA status notifications
 * @param arg User-provided context pointer
 * @param event Current FOTA status event
 */
typedef void (*pfn_tcp_fota_t)(void *arg, tcp_fota_status_t event);

/**
 * @brief Configuration structure for TCP FOTA
 */
struct tcp_fota_config {
    pfn_tcp_fota_t callback; /**< Status callback function */
    void *user_arg;          /**< User context passed to callback */
};

/** Opaque handle for TCP FOTA session */
typedef void *tcp_fota_handle_t;

/**
 * @brief Initialize a new TCP FOTA session
 * @param ip Server IP address or hostname
 * @param port Server port string (NULL for default 3365)
 * @param config Configuration parameters (can be NULL)
 * @return FOTA session handle on success, NULL on failure
 */
tcp_fota_handle_t tcp_fota_init(const char *ip, const char *port, const struct tcp_fota_config *config);

/**
 * @brief Start the firmware update process (runs in FreeRTOS task)
 * @param fota FOTA session handle
 * @return 0 on success, negative error code on failure
 */
int tcp_fota_start(tcp_fota_handle_t fota);

/**
 * @brief Finalize the FOTA process
 * @param fota FOTA session handle
 * @param reboot Whether to reboot after successful update
 * @return 0 on success, negative error code on failure
 */
int tcp_fota_finish(tcp_fota_handle_t fota, bool reboot);

/**
 * @brief Abort the FOTA process
 * @param fota FOTA session handle
 * @return 0 on success, negative error code on failure
 */
int tcp_fota_abort(tcp_fota_handle_t fota);

/**
 * @brief Single-call convenience function for TCP FOTA update
 * @param ip Server IP address or hostname
 * @param port Server port string (NULL for default 3365)
 * @param config Configuration parameters (can be NULL)
 * @return 0 on success, negative error code on failure
 * @note This combines init, start (blocking), finish, and reboot operations
 */
int tcp_fota(const char *ip, const char *port, const struct tcp_fota_config *config);

/**
 * @brief Roll back to the firmware version in the backup partition
 * @return 0 on success, negative error code on failure
 */
int tcp_ota_rollback(void);

#ifdef __cplusplus
}
#endif

#endif /* __TCP_FOTA_H__ */
