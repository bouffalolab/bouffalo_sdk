/**
 * @file bflb_iperf.h
 * @brief Compact Classic iPerf2 public API.
 *
 * This API provides independent IPv4 test instances with Socket and lwIP Raw
 * backends. TCP and UDP client/server roles are supported in Classic iPerf2
 * normal mode.
 */

#ifndef BFLB_IPERF_H
#define BFLB_IPERF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup bflb_iperf iPerf
 *  @brief Compact Classic iPerf2 throughput tester.
 *  @{
 */

#define BFLB_IPERF_DEFAULT_PORT         5001U    /**< Default server port. */
#define BFLB_IPERF_DEFAULT_TIME_S       10U      /**< Default test time in seconds. */
#define BFLB_IPERF_DEFAULT_INTERVAL_S   1U       /**< Default report interval in seconds. */
#define BFLB_IPERF_DEFAULT_TCP_LEN      4096U    /**< Default TCP I/O buffer size in bytes. */
#define BFLB_IPERF_DEFAULT_UDP_LEN      1470U    /**< Default UDP datagram size in bytes. */
#define BFLB_IPERF_DEFAULT_UDP_RATE_BPS 1000000U /**< Default UDP transmit rate in bits/s. */

#define BFLB_IPERF_OK                   0  /**< Operation completed successfully. */
#define BFLB_IPERF_ERR_INVALID          -1 /**< Invalid argument, state, or resource failure. */
#define BFLB_IPERF_ERR_SELF             -2 /**< Synchronous destruction from this instance's worker is forbidden. */

/**
 * @brief Opaque iPerf instance owned by its creator.
 * @note One external owner must serialize destroy with all other external
 * instance APIs, including create. No API may use the handle after destroy.
 * Worker callbacks may query state/results even while the owner is destroying.
 * APIs are task-only; never call them from an ISR or with the scheduler/core
 * lock held. Instances are single-CPU, not shared across SMP CPUs.
 */
typedef struct bflb_iperf bflb_iperf_t;

/**
 * @brief iPerf transport backend.
 */
typedef enum {
    BFLB_IPERF_BACKEND_SOCKET = 0, /**< lwIP Socket API backend. */
    BFLB_IPERF_BACKEND_RAW,        /**< lwIP Raw API backend. */
} bflb_iperf_backend_t;

/**
 * @brief iPerf endpoint role.
 */
typedef enum {
    BFLB_IPERF_ROLE_CLIENT = 0, /**< Generate traffic as a client. */
    BFLB_IPERF_ROLE_SERVER,     /**< Receive traffic as a server. */
} bflb_iperf_role_t;

/**
 * @brief Test transport protocol.
 */
typedef enum {
    BFLB_IPERF_PROTO_TCP = 0, /**< TCP throughput test. */
    BFLB_IPERF_PROTO_UDP,     /**< UDP throughput test. */
} bflb_iperf_proto_t;

/**
 * @brief Test lifecycle state.
 */
typedef enum {
    BFLB_IPERF_STATE_IDLE = 0, /**< No test has been started. */
    BFLB_IPERF_STATE_STARTING, /**< Backend startup is in progress. */
    BFLB_IPERF_STATE_RUNNING,  /**< Worker started; a peer need not be connected. */
    BFLB_IPERF_STATE_STOPPING, /**< A stop request is being processed. */
    BFLB_IPERF_STATE_DONE,     /**< Test completed successfully. */
    BFLB_IPERF_STATE_ERROR,    /**< Test terminated with an error. */
} bflb_iperf_state_t;

/**
 * @brief Snapshot of the current or completed test result.
 */
typedef struct {
    bflb_iperf_state_t state; /**< Lifecycle state at snapshot time. */
    int error;                /**< Final backend error in DONE/ERROR state; zero indicates success. */
    uint64_t bytes;           /**< Total payload bytes accounted. */
    uint64_t duration_us;     /**< Elapsed test time in microseconds. */
    uint64_t bits_per_second; /**< Average throughput in bits per second. */
    uint32_t datagrams;       /**< UDP datagrams sent, or expected by a completed receiver. */
    uint32_t lost;            /**< UDP datagrams considered lost. */
    uint32_t out_of_order;    /**< UDP datagrams received out of order. */
    uint32_t jitter_us;       /**< UDP inter-arrival jitter in microseconds. */
} bflb_iperf_result_t;

/**
 * @brief Worker lifecycle events, each emitted once for a successful launch.
 * @note Either event may precede the return from bflb_iperf_create().
 */
typedef enum {
    BFLB_IPERF_EVENT_STARTED = 0, /**< Worker entered, not necessarily connected. */
    BFLB_IPERF_EVENT_FINISHED,    /**< Runtime resources released and result frozen. */
} bflb_iperf_event_t;

/**
 * @brief Direct worker event callback.
 *
 * Runs without the instance mutex, possibly before create returns. STARTED
 * carries an initial snapshot (possibly STOPPING after early cancellation);
 * FINISHED carries the immutable final snapshot, including failures/cancellation.
 * The result pointer is valid only during the callback; copy it to retain it.
 *
 * @param[in] iperf Live instance; state/result queries are allowed.
 * @param[in] event STARTED or FINISHED.
 * @param[in] result Immutable event-time snapshot; never NULL.
 * @param[in] user_data User value copied from the configuration.
 *
 * @note Synchronous destroy of this instance returns BFLB_IPERF_ERR_SELF.
 * Do not wait for another task to destroy it: destroy waits for this callback.
 * Synchronous create failure emits no events. Callbacks must eventually return.
 */
typedef void (*bflb_iperf_event_cb_t)(bflb_iperf_t *iperf,
                                    bflb_iperf_event_t event,
                                    const bflb_iperf_result_t *result,
                                    void *user_data);

/**
 * @brief Configuration copied when an iPerf instance is created.
 *
 * Call bflb_iperf_config_init() before overriding individual fields. All
 * configuration fields are copied by bflb_iperf_create().
 *
 * @note Only the user_data pointer value is copied. The caller must keep the
 * object referenced by user_data valid until FINISHED returns (synchronous
 * destroy provides that guarantee).
 * @note buffer_len must be 80--1470 bytes for UDP, at most 16384 bytes for
 * TCP Socket, and at most 4096 bytes for TCP Raw. task_priority must be less
 * than configMAX_PRIORITIES after default selection.
 * @note In UDP byte-limit mode, amount_bytes must contain the setup datagram;
 * any partial final datagram must be large enough for the UDP sequence header.
 */
typedef struct {
    bflb_iperf_backend_t backend; /**< Transport backend. */
    bflb_iperf_role_t role;       /**< Client or server role. */
    bflb_iperf_proto_t proto;     /**< TCP or UDP protocol. */
    uint32_t remote_ip4;          /**< Required client destination IPv4 address in network byte order. */
    uint32_t local_ip4;           /**< Local IPv4 address in network byte order; zero binds any. */
    uint16_t port;                /**< Server destination/listen port; zero selects the default. */
    uint16_t local_port;          /**< Client source port; zero selects an ephemeral port. */
    uint16_t buffer_len;          /**< TCP I/O buffer or UDP datagram size; zero selects a protocol default. */
    uint16_t interval_s;          /**< Report interval in seconds; zero disables periodic reports. */
    uint32_t duration_s;          /**< Client duration in seconds when amount_bytes is zero. */
    uint64_t amount_bytes;        /**< Client byte limit; zero selects duration mode. */
    uint32_t bandwidth_bps;       /**< UDP client rate in bits/s; zero selects the default. */
    uint8_t tos;                  /**< IPv4 type-of-service value. */
    uint8_t tcp_nodelay;          /**< Nonzero disables Nagle for a TCP client. */
    uint8_t task_priority;        /**< Backend worker priority; zero selects the default. */
    bflb_iperf_event_cb_t event_cb; /**< Optional direct worker event callback. */
    void *user_data;              /**< Opaque value passed to event_cb. */
} bflb_iperf_config_t;

/**
 * @brief Initialize a configuration with default values.
 *
 * The defaults select a TCP Raw server on port 5001. A NULL pointer is
 * ignored.
 *
 * @param[out] config Configuration structure to initialize.
 *
 * @note Protocol-dependent buffer and bandwidth defaults may be selected
 * later by bflb_iperf_create() when their fields remain zero.
 */
void bflb_iperf_config_init(bflb_iperf_config_t *config);

/**
 * @brief Create and immediately launch an independent iPerf instance.
 *
 * The caller owns the instance and must eventually destroy it, even after
 * natural completion or failure. Instances never free themselves. One instance
 * represents one test; there is no separate public start or stop operation.
 * All synchronization objects and the output handle are initialized before the
 * worker can run. This function does not wait for callbacks or connection setup.
 *
 * @param[in] config Test configuration.
 * @param[out] iperf Non-NULL output slot, published before worker launch;
 * set to NULL on synchronous failure. A NULL slot is rejected.
 * @retval BFLB_IPERF_OK Worker launched; subsequent failures arrive via FINISHED.
 * @retval BFLB_IPERF_ERR_INVALID Invalid configuration or resource/launch failure;
 * all resources are released and no callback is emitted.
 * @pre Task context; the owner must serialize create with external handle APIs.
 * @note STARTED and FINISHED may both occur before this function returns.
 */
int bflb_iperf_create(const bflb_iperf_config_t *config, bflb_iperf_t **iperf);

/**
 * @brief Read a consistent snapshot of test statistics.
 * @note Copies all fields into caller-owned storage, does not clear counters.
 * Running snapshots evolve; DONE/ERROR snapshots remain frozen until destroy.
 *
 * @param[in] iperf Instance to inspect.
 * @param[out] result Destination for the result snapshot.
 * @retval BFLB_IPERF_OK Result returned successfully.
 * @retval BFLB_IPERF_ERR_INVALID An argument is NULL.
 */
int bflb_iperf_get_result(bflb_iperf_t *iperf, bflb_iperf_result_t *result);

/**
 * @brief Get the current iPerf lifecycle state.
 *
 * @param[in] iperf Instance to inspect.
 * @return Current test state, or BFLB_IPERF_STATE_ERROR when the instance is
 * NULL.
 */
bflb_iperf_state_t bflb_iperf_get_state(bflb_iperf_t *iperf);

/**
 * @brief Request stop, join worker completion, and free the owned instance.
 *
 * Waits without holding the instance mutex until runtime resources have been
 * released and FINISHED has returned, then frees context/synchronization/instance.
 * Safe before/during either event or after natural completion. Backend waits
 * remain transport-specific; this is not a fixed-latency operation. No external
 * API call may overlap destroy. Worker callbacks may still query the instance.
 *
 * @param[in] iperf Instance to destroy.
 * @retval BFLB_IPERF_OK Instance destroyed; the pointer is no longer valid.
 * @retval BFLB_IPERF_ERR_SELF Called from this worker; instance is unchanged.
 * @retval BFLB_IPERF_ERR_INVALID iperf is NULL.
 * @pre One external owner; task context, never an ISR or this worker's callback.
 * Do not hold the scheduler/core lock or a lock needed by an event callback.
 * @post On success no callback can access user_data again and the handle must
 * not be reused. The wait has no fixed real-time bound.
 */
int bflb_iperf_destroy(bflb_iperf_t *iperf);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BFLB_IPERF_H */
