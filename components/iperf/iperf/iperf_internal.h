/**
 * @file iperf_internal.h
 * @brief Private instance, backend, budget, and worker management contracts.
 * @note This header is private to components/iperf/iperf. It includes the
 * protocol/statistics contracts in iperf_common.h, never the reverse.
 */

#ifndef IPERF_INTERNAL_H
#define IPERF_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "iperf_common.h"

/** @name Backend contracts and private instance
 * @{
 */

/**
 * @brief Operations supplied by one protocol/backend implementation.
 *
 * The core allocates and zero-initializes context_size bytes for context.
 * launch initializes resources before creating a worker; failure must leave no
 * worker or runtime resources and emit no events. Success must reach started
 * and finished exactly once. request_stop is a cooperative transport hook:
 * TCP Raw notifies the worker; Socket and UDP Raw rely on their existing waits
 * returning or timing out. It must be safe after runtime resources are released.
 */
typedef struct {
    size_t context_size; /**< Bytes required for the private backend context. */
    /**
     * @brief Initialize backend resources and create its worker.
     * @param[in,out] iperf Validated instance, already published to its owner.
     * @param[in,out] context Zero-initialized storage of context_size bytes.
     * @return Zero on task creation, negative on synchronous failure.
     * @post Failure leaves no worker/resources/events. Success permits immediate
     * callbacks; launch must not write instance/context after task creation.
     */
    int (*launch)(bflb_iperf_t *iperf, void *context); /**< Create worker; zero on success, negative on failure. */
    /**
     * @brief Apply transport-specific cooperation after stop publication.
     * @param[in,out] iperf Live instance with stop_requested already set.
     * @param[in] context Core-owned storage, possibly without runtime resources.
     * @pre Owner task outside the lifecycle mutex; context has not been freed.
     * @note TCP Raw notifies; the other hooks rely on existing waits/timeouts.
     * No hook joins the worker or frees the instance.
     */
    void (*request_stop)(bflb_iperf_t *iperf, void *context); /**< Cooperate after stop_requested; not necessarily a wakeup. */
} iperf_backend_ops_t;

/**
 * @brief Complete private representation of the public opaque handle.
 * @note One external owner serializes API lifetime. lock protects state/error
 * and worker handle publication/notification/revocation. Statistics retain
 * their independent single-CPU IRQ guards.
 */
struct bflb_iperf {
    SemaphoreHandle_t lock;              /**< Mutex protecting public lifecycle state. */
    SemaphoreHandle_t worker_done;       /**< Initially empty binary completion semaphore. */
    TaskHandle_t worker;                 /**< Live worker, protected by lock; NULL before entry/after callbacks. */
    bflb_iperf_config_t config;           /**< Validated and normalized configuration copy. */
    volatile bflb_iperf_state_t state;    /**< Current lifecycle state. */
    volatile bool stop_requested;         /**< Cooperative stop flag observed by the backend. */
    int error;                            /**< Final backend error code. */
    iperf_stats_t stats;                  /**< Mutable traffic statistics. */
    const iperf_backend_ops_t *ops;        /**< Selected immutable backend operations. */
    void *backend_context;                /**< Core-owned private backend storage. */
};

/** @brief Blocking TCP Socket backend operations. */
extern const iperf_backend_ops_t g_iperf_tcp_socket_ops;
/** @brief Task-driven TCP Raw backend operations. */
extern const iperf_backend_ops_t g_iperf_tcp_raw_ops;
/** @brief Blocking UDP Socket backend operations. */
extern const iperf_backend_ops_t g_iperf_udp_socket_ops;
/** @brief Task-driven UDP Raw backend operations. */
extern const iperf_backend_ops_t g_iperf_udp_raw_ops;

/** @} */

/** @name Client budget and stop management
 * @{
 */

/**
 * @brief Read the remaining byte budget.
 * @param[in] iperf Live instance to inspect.
 * @return Remaining bytes, zero if exhausted, UINT64_MAX if no byte limit.
 * @note Core budget policy using an IRQ-protected statistics counter; a
 * duration limit or stop request may still apply.
 */
uint64_t iperf_bytes_remaining(const bflb_iperf_t *iperf);

/**
 * @brief Determine whether a client limit or stop request has been reached.
 * @param[in] iperf Instance to inspect.
 * @param[in] now_us Current monotonic timestamp in microseconds.
 * @return true when traffic generation should stop; false otherwise.
 * @note Servers ignore local duration and byte limits and terminate on peer events.
 */
bool iperf_limit_reached(const bflb_iperf_t *iperf, uint64_t now_us);

/** @} */

/** @name Worker lifecycle management
 * @{
 */

/**
 * @brief Publish the worker under the lifecycle mutex, then emit STARTED unlocked.
 * @param[in,out] iperf Instance entering the backend execution context.
 * @return true when traffic should run, false when an early stop was requested.
 */
bool iperf_worker_enter(bflb_iperf_t *iperf);

/**
 * @brief Notify the live worker under lock; never notify a deleted TCB.
 * @param[in,out] iperf Live instance; a NULL worker handle is ignored.
 * @pre Task context without the lifecycle mutex held.
 * @note Only task-notification waits are woken, not socket or queue waits.
 */
void iperf_worker_wake(bflb_iperf_t *iperf);

/**
 * @brief Freeze results, emit FINISHED, revoke worker, and give worker_done.
 * @param[in,out] iperf Instance being completed.
 * @param[in] error Final backend error; zero indicates successful completion.
 * @pre All transport callbacks are detached and runtime resources released.
 * @post FINISHED has returned before worker_done is given.
 * @note Giving worker_done is the last instance access. Only vTaskDelete(NULL)
 * may follow; neither instance nor context may be accessed again.
 */
void iperf_worker_exit(bflb_iperf_t *iperf, int error);

/** @} */

#endif /* IPERF_INTERNAL_H */