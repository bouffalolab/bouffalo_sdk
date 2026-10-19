/**
 * @file tperf.h
 * @brief Thread Performance Test (tperf) - Control Interface
 *
 * This module provides network performance testing functionality for OpenThread
 * networks using native UDP and TCP APIs.
 */

#ifndef __TPERF_H_
#define __TPERF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <openthread/ip6.h>
#include <openthread/instance.h>

/**
 * @brief tperf mode
 */
typedef enum {
    TPERF_MODE_IDLE = 0,
    TPERF_MODE_UDP_SERVER,
    TPERF_MODE_UDP_CLIENT,
    TPERF_MODE_TCP_SERVER,
    TPERF_MODE_TCP_CLIENT,
} tperf_mode_t;

/**
 * @brief tperf configuration
 *
 * @note buffer_size limits:
 *       - UDP mode: Must be <= OPENTHREAD_CONFIG_MESSAGE_BUFFER_SIZE - 80
 *       - TCP mode: Can be larger (up to 8192 bytes recommended)
 *       - Minimum: 64 bytes
 *
 * @note For best results with UDP, use buffer_size around 1200 bytes or less
 *       to avoid fragmentation and ensure reliable delivery.
 */
typedef struct {
    tperf_mode_t   mode;
    uint32_t       duration;        // Test duration in seconds
    uint32_t       interval;        // Report interval in seconds
    uint32_t       bandwidth;       // Bandwidth limit in Kbps (0 = unlimited)
    uint16_t       local_port;      // Local port
    uint16_t       peer_port;       // Peer port
    otIp6Address   peer_addr;       // Peer IPv6 address
    uint8_t        tos;             // Type of Service
    uint32_t       buffer_size;     // Send/receive buffer size (see limits above)
    uint8_t        task_priority;   // Task priority
} tperf_config_t;

/**
 * @brief tperf statistics
 */
typedef struct {
    uint32_t       total_bytes;     // Total bytes transferred
    uint32_t       total_packets;   // Total packets (UDP only)
    uint32_t       lost_packets;    // Lost packets (UDP only)
    uint64_t       start_time_ms;   // Test start time
    uint64_t       end_time_ms;     // Test end time
    double         avg_mbps;        // Average throughput in Mbps
} tperf_stats_t;

/**
 * @brief tperf session handle
 */
typedef struct tperf_session tperf_session_t;

/**
 * @brief Initialize tperf module
 *
 * @param instance OpenThread instance
 * @return 0 on success, -1 on failure
 */
int tperf_init(otInstance *instance);

/**
 * @brief Deinitialize tperf module
 */
void tperf_deinit(void);

/**
 * @brief Check if tperf is initialized
 *
 * @return true if initialized, false otherwise
 */
bool tperf_is_initialized(void);

/**
 * @brief Start a tperf session
 *
 * @param config Pointer to tperf configuration
 * @return Session handle on success, NULL on failure
 */
tperf_session_t *tperf_start(const tperf_config_t *config);

/**
 * @brief Stop a tperf session
 *
 * @param session Session handle
 * @return 0 on success, -1 on failure
 */
int tperf_stop(tperf_session_t *session);

/**
 * @brief Get tperf session statistics
 *
 * @param session Session handle
 * @param stats Pointer to store statistics
 * @return 0 on success, -1 on failure
 */
int tperf_get_stats(tperf_session_t *session, tperf_stats_t *stats);

/**
 * @brief Check if session is running
 *
 * @param session Session handle
 * @return true if running, false otherwise
 */
bool tperf_is_running(tperf_session_t *session);

/**
 * @brief Get current mode
 *
 * @param session Session handle
 * @return Current mode
 */
tperf_mode_t tperf_get_mode(tperf_session_t *session);

/**
 * @brief Wait for session completion
 *
 * @param session Session handle
 * @param timeout_ms Timeout in milliseconds (0 = wait forever)
 * @return 0 on success, -1 on timeout or error
 */
int tperf_wait_completion(tperf_session_t *session, uint32_t timeout_ms);

/**
 * @brief Get last error message
 *
 * @return Error message string
 */
const char *tperf_get_last_error(void);

#ifdef __cplusplus
}
#endif

#endif // __TPERF_H_
