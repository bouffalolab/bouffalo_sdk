/**
 * @file bflb_iperf.c
 * @brief Multi-instance iPerf public API and backend dispatch.
 */

#include <stdlib.h>
#include <string.h>

#include <bflb_irq.h>

#include "iperf_internal.h"

/** @brief Default worker priority, clamped to the configured FreeRTOS range. */
#define IPERF_DEFAULT_TASK_PRIORITY (configMAX_PRIORITIES / 2 - 1)
/** @brief Maximum generic Socket I/O buffer size, in bytes. */
#define IPERF_MAX_BUFFER_LEN        8192U
/** @brief Maximum UDP datagram size accepted by all backends, in bytes. */
#define IPERF_MAX_UDP_LEN           4096U

/**
 * @brief Immutable protocol/backend operation dispatch table.
 * @note The first index is bflb_iperf_proto_t and the second index is
 * bflb_iperf_backend_t; iperf_validate_config() validates both before lookup.
 */
static const iperf_backend_ops_t *const s_backends[][2] = {
    [BFLB_IPERF_PROTO_TCP] = {
        [BFLB_IPERF_BACKEND_SOCKET] = &g_iperf_tcp_socket_ops,
        [BFLB_IPERF_BACKEND_RAW] = &g_iperf_tcp_raw_ops,
    },
    [BFLB_IPERF_PROTO_UDP] = {
        [BFLB_IPERF_BACKEND_SOCKET] = &g_iperf_udp_socket_ops,
        [BFLB_IPERF_BACKEND_RAW] = &g_iperf_udp_raw_ops,
    },
};

/**
 * @brief Validate and normalize a copied configuration.
 *
 * @param[in,out] config Configuration copy to validate and populate with defaults.
 *
 * @retval BFLB_IPERF_OK The normalized configuration is valid.
 * @retval BFLB_IPERF_ERR_INVALID A field or cross-field constraint is invalid.
 *
 * @note config must not be NULL.
 */
static int iperf_validate_config(bflb_iperf_config_t *config)
{
    if ((unsigned int)config->backend > BFLB_IPERF_BACKEND_RAW ||
        (unsigned int)config->role > BFLB_IPERF_ROLE_SERVER ||
        (unsigned int)config->proto > BFLB_IPERF_PROTO_UDP) {
        return BFLB_IPERF_ERR_INVALID;
    }
    if (config->role == BFLB_IPERF_ROLE_CLIENT && config->remote_ip4 == 0U) {
        return BFLB_IPERF_ERR_INVALID;
    }
    if (config->port == 0U) {
        config->port = BFLB_IPERF_DEFAULT_PORT;
    }
    if (config->role == BFLB_IPERF_ROLE_CLIENT &&
        config->duration_s == 0U && config->amount_bytes == 0U) {
        config->duration_s = BFLB_IPERF_DEFAULT_TIME_S;
    }
    if (config->amount_bytes != 0U) {
        config->duration_s = 0U;
    }
    if (config->buffer_len == 0U) {
        config->buffer_len = config->proto == BFLB_IPERF_PROTO_UDP ?
                                 BFLB_IPERF_DEFAULT_UDP_LEN :
                                 BFLB_IPERF_DEFAULT_TCP_LEN;
    }
    if (config->buffer_len > IPERF_MAX_BUFFER_LEN ||
        (config->proto == BFLB_IPERF_PROTO_UDP &&
         config->buffer_len > IPERF_MAX_UDP_LEN) ||
        (config->backend == BFLB_IPERF_BACKEND_RAW &&
         config->proto == BFLB_IPERF_PROTO_TCP &&
         config->buffer_len > BFLB_IPERF_RAW_TCP_BUFFER_LEN)) {
        return BFLB_IPERF_ERR_INVALID;
    }
    if (config->proto == BFLB_IPERF_PROTO_UDP &&
        config->buffer_len < BFLB_IPERF_UDP_CLIENT_HEADER_SIZE) {
        return BFLB_IPERF_ERR_INVALID;
    }
    if (config->task_priority == 0U) {
        config->task_priority = IPERF_DEFAULT_TASK_PRIORITY;
    }
    if (config->task_priority >= configMAX_PRIORITIES) {
        return BFLB_IPERF_ERR_INVALID;
    }
    if (config->amount_bytes != 0U && config->proto == BFLB_IPERF_PROTO_UDP &&
        (config->amount_bytes < BFLB_IPERF_UDP_CLIENT_HEADER_SIZE ||
         (config->amount_bytes % config->buffer_len != 0U &&
          config->amount_bytes % config->buffer_len < BFLB_IPERF_UDP_HEADER_SIZE))) {
        return BFLB_IPERF_ERR_INVALID;
    }
    return BFLB_IPERF_OK;
}

/**
 * @brief Initialize defaults for a TCP Raw server.
 * @param[out] config Configuration to reset; NULL is ignored.
 * @note Zero buffer length defers protocol-specific sizing until create.
 */
void bflb_iperf_config_init(bflb_iperf_config_t *config)
{
    if (config == NULL) {
        return;
    }
    memset(config, 0, sizeof(*config));
    config->backend = BFLB_IPERF_BACKEND_RAW;
    config->role = BFLB_IPERF_ROLE_SERVER;
    config->proto = BFLB_IPERF_PROTO_TCP;
    config->port = BFLB_IPERF_DEFAULT_PORT;
    config->interval_s = BFLB_IPERF_DEFAULT_INTERVAL_S;
    config->duration_s = BFLB_IPERF_DEFAULT_TIME_S;
    config->task_priority = IPERF_DEFAULT_TASK_PRIORITY;
}

/**
 * @brief Allocate, publish and immediately launch one owned test instance.
 * @param[in] config Settings copied and normalized before launch.
 * @param[out] iperf Output slot; set to NULL on failure when non-NULL.
 * @retval BFLB_IPERF_OK Worker created; connection/traffic may not yet exist.
 * @retval BFLB_IPERF_ERR_INVALID Invalid arguments or allocation/launch failure.
 * @pre Task context; the external owner serializes creation and handle lifetime.
 * @post Synchronous failure leaves no worker, runtime resources or events.
 * @note The output slot is written before launch. Either event may run before
 * this function returns; successful launch performs no further context writes.
 * The owner must destroy even a naturally completed instance.
 */
int bflb_iperf_create(const bflb_iperf_config_t *config,
                      bflb_iperf_t **iperf)
{
    bflb_iperf_config_t checked;
    bflb_iperf_t *created;

    if (iperf == NULL) {
        return BFLB_IPERF_ERR_INVALID;
    }
    *iperf = NULL;
    if (config == NULL) {
        return BFLB_IPERF_ERR_INVALID;
    }
    checked = *config;
    if (iperf_validate_config(&checked) != BFLB_IPERF_OK) {
        return BFLB_IPERF_ERR_INVALID;
    }

    created = calloc(1, sizeof(*created));
    if (created == NULL) {
        return BFLB_IPERF_ERR_INVALID;
    }
    created->lock = xSemaphoreCreateMutex();
    if (created->lock == NULL) {
        free(created);
        return BFLB_IPERF_ERR_INVALID;
    }
    /* xSemaphoreCreateBinary() creates an EMPTY semaphore, unlike the legacy
     * vSemaphoreCreateBinary() macro. Completion cannot precede worker exit. */
    created->worker_done = xSemaphoreCreateBinary();
    if (created->worker_done == NULL) {
        vSemaphoreDelete(created->lock);
        free(created);
        return BFLB_IPERF_ERR_INVALID;
    }
    created->config = checked;
    created->state = BFLB_IPERF_STATE_STARTING;
    created->ops = s_backends[checked.proto][checked.backend];
    created->backend_context = calloc(1, created->ops->context_size);
    if (created->backend_context == NULL) {
        vSemaphoreDelete(created->worker_done);
        vSemaphoreDelete(created->lock);
        free(created);
        return BFLB_IPERF_ERR_INVALID;
    }

    /* Publish before launch: a higher-priority worker may run immediately.
     * launch must not write task handles/context after successful xTaskCreate;
     * the worker publishes its own handle before its first event. */
    *iperf = created;
    if (created->ops->launch(created, created->backend_context) < 0) {
        *iperf = NULL;
        free(created->backend_context);
        vSemaphoreDelete(created->worker_done);
        vSemaphoreDelete(created->lock);
        free(created);
        return BFLB_IPERF_ERR_INVALID;
    }
    return BFLB_IPERF_OK;
}

/**
 * @brief Copy lifecycle state and an IRQ-protected statistics snapshot.
 * @param[in] iperf Live instance to inspect.
 * @param[out] result Caller-owned snapshot destination.
 * @retval BFLB_IPERF_OK Snapshot copied without clearing counters.
 * @retval BFLB_IPERF_ERR_INVALID Either argument is NULL.
 * @pre Task context; external callers must not overlap destroy.
 * @note Worker event callbacks may query while the owner waits in destroy.
 */
int bflb_iperf_get_result(bflb_iperf_t *iperf,
                          bflb_iperf_result_t *result)
{
    if (result == NULL || iperf == NULL) {
        return BFLB_IPERF_ERR_INVALID;
    }
    xSemaphoreTake(iperf->lock, portMAX_DELAY);
    iperf_result_snapshot(iperf, result);
    xSemaphoreGive(iperf->lock);
    return BFLB_IPERF_OK;
}

/**
 * @brief Read the lifecycle state under the instance mutex.
 * @param[in] iperf Live instance, or NULL.
 * @return Current state, or BFLB_IPERF_STATE_ERROR for NULL.
 * @pre Task context; external callers serialize access against destroy.
 */
bflb_iperf_state_t bflb_iperf_get_state(bflb_iperf_t *iperf)
{
    bflb_iperf_state_t state;

    if (iperf == NULL) {
        return BFLB_IPERF_STATE_ERROR;
    }
    xSemaphoreTake(iperf->lock, portMAX_DELAY);
    state = iperf->state;
    xSemaphoreGive(iperf->lock);
    return state;
}

/**
 * @brief Request cooperative stop, join completion and release the instance.
 * @param[in] iperf Owned live instance; invalid after successful destruction.
 * @retval BFLB_IPERF_OK FINISHED returned and all owned storage was freed.
 * @retval BFLB_IPERF_ERR_INVALID iperf is NULL.
 * @retval BFLB_IPERF_ERR_SELF Called by this instance's worker; no change made.
 * @pre One external owner serializes this call with every external handle API,
 * including create and other destroy calls; no ISR, scheduler or core lock.
 * @note Waits on worker_done without the lifecycle mutex, allowing callbacks
 * to query results. Backend notification/timeout behavior determines latency;
 * there is no fixed completion deadline.
 * @warning Never destroy from a worker callback or wait there for a destroying
 * task: completion is signalled only after FINISHED returns.
 */
int bflb_iperf_destroy(bflb_iperf_t *iperf)
{
    if (iperf == NULL) {
        return BFLB_IPERF_ERR_INVALID;
    }
    xSemaphoreTake(iperf->lock, portMAX_DELAY);
    if (iperf->worker == xTaskGetCurrentTaskHandle()) {
        xSemaphoreGive(iperf->lock);
        return BFLB_IPERF_ERR_SELF;
    }
    iperf->stop_requested = true;
    if (iperf->state == BFLB_IPERF_STATE_STARTING ||
        iperf->state == BFLB_IPERF_STATE_RUNNING) {
        iperf->state = BFLB_IPERF_STATE_STOPPING;
    }
    xSemaphoreGive(iperf->lock);

    /* Never hold lock while joining: both event callbacks may query results.
     * Context remains allocated even if the worker has already finished. */
    iperf->ops->request_stop(iperf, iperf->backend_context);
    xSemaphoreTake(iperf->worker_done, portMAX_DELAY);
    free(iperf->backend_context);
    vSemaphoreDelete(iperf->worker_done);
    vSemaphoreDelete(iperf->lock);
    free(iperf);
    return BFLB_IPERF_OK;
}

/** @name Client budget and stop management
 * @{
 */

/**
 * @brief Read the remaining byte budget using an IRQ-protected counter.
 * @param[in] iperf Live instance with immutable normalized configuration.
 * @return Remaining bytes, zero when exhausted, or UINT64_MAX without a byte
 * limit (a duration limit may still apply).
 */
uint64_t iperf_bytes_remaining(const bflb_iperf_t *iperf)
{
    uint64_t bytes;
    uintptr_t irq_flags;

    if (iperf->config.amount_bytes == 0U) {
        return UINT64_MAX;
    }
    irq_flags = bflb_irq_save();
    bytes = iperf->stats.bytes;
    bflb_irq_restore(irq_flags);
    return bytes < iperf->config.amount_bytes ?
               iperf->config.amount_bytes - bytes :
               0U;
}

/**
 * @brief Determine whether traffic generation must stop.
 * @param[in] iperf Instance whose stop flag and configured client limits are
 * inspected.
 * @param[in] now_us Current monotonic timestamp in microseconds.
 * @retval true A stop request, byte limit, or duration limit was reached.
 * @retval false Traffic may continue.
 * @note Servers ignore local duration and byte limits and terminate on peer
 * protocol events.
 */
bool iperf_limit_reached(const bflb_iperf_t *iperf, uint64_t now_us)
{
    uint64_t bytes;
    uint64_t start_us;
    uintptr_t irq_flags;

    if (iperf->stop_requested) {
        return true;
    }
    /* Servers terminate on TCP close or UDP FIN rather than local client limits. */
    if (iperf->config.role == BFLB_IPERF_ROLE_SERVER) {
        return false;
    }
    irq_flags = bflb_irq_save();
    bytes = iperf->stats.bytes;
    start_us = iperf->stats.start_us;
    bflb_irq_restore(irq_flags);
    if (iperf->config.amount_bytes != 0U && bytes >= iperf->config.amount_bytes) {
        return true;
    }
    if (iperf->config.duration_s == 0U || start_us == 0U) {
        return false;
    }
    if (now_us >= start_us && (now_us - start_us) >= (uint64_t)iperf->config.duration_s * 1000000ULL) {
        return true;
    }
    return false;
}

/** @} */

/**
 * @brief Publish the worker and emit STARTED outside the lifecycle mutex.
 * @param[in,out] iperf Instance owned by the entering worker.
 * @return true if execution may proceed after the callback; false on stop.
 * @pre Called exactly once by a successfully launched worker.
 * @note STARTED is not a connection notification and can carry STOPPING.
 * Cancellation is rechecked after the callback, which may precede create return.
 */
bool iperf_worker_enter(bflb_iperf_t *iperf)
{
    bflb_iperf_result_t result;
    bool run;

    xSemaphoreTake(iperf->lock, portMAX_DELAY);
    iperf->worker = xTaskGetCurrentTaskHandle();
    iperf->state = iperf->stop_requested ? BFLB_IPERF_STATE_STOPPING : BFLB_IPERF_STATE_RUNNING;
    iperf_result_snapshot(iperf, &result);
    xSemaphoreGive(iperf->lock);

    if (iperf->config.event_cb != NULL) {
        iperf->config.event_cb(iperf, BFLB_IPERF_EVENT_STARTED, &result,
                               iperf->config.user_data);
    }
    /* Recheck after the callback: destroy may have requested cancellation
     * before STARTED, while it ran, or before the worker published its handle. */
    xSemaphoreTake(iperf->lock, portMAX_DELAY);
    run = !iperf->stop_requested;
    xSemaphoreGive(iperf->lock);
    return run;
}

/**
 * @brief Notify a published worker while preventing concurrent handle revocation.
 * @param[in,out] iperf Live instance whose worker may be NULL.
 * @pre Task context, without the lifecycle mutex held.
 * @note This wakes task-notification waits, not arbitrary socket or queue waits.
 */
void iperf_worker_wake(bflb_iperf_t *iperf)
{
    xSemaphoreTake(iperf->lock, portMAX_DELAY);
    if (iperf->worker != NULL) {
        /* Revocation uses this same mutex, so this TCB cannot be deleted
         * between the handle check and notification. No blocking I/O here. */
        xTaskNotifyGive(iperf->worker);
    }
    xSemaphoreGive(iperf->lock);
}

/**
 * @brief Freeze results, emit FINISHED, revoke the worker and signal completion.
 * @param[in,out] iperf Instance completing its single worker lifecycle.
 * @param[in] error Final backend result; zero selects DONE, otherwise ERROR.
 * @pre Worker context outside the core lock; transport callbacks detached,
 * runtime resources released and all traffic accounting complete.
 * @post FINISHED has returned before worker_done is given.
 * @warning The completion give is the final instance-related access and allows
 * immediate owner destruction. After this call only task self-deletion may
 * follow; neither instance nor private context may be dereferenced.
 */
void iperf_worker_exit(bflb_iperf_t *iperf, int error)
{
    bflb_iperf_result_t result;
    SemaphoreHandle_t worker_done;

    /* All transport callbacks have been detached; no further accounting. */
    iperf_test_end(iperf);
    iperf_test_report_finish(iperf);
    xSemaphoreTake(iperf->lock, portMAX_DELAY);
    iperf->error = error;
    iperf->state = error == 0 ? BFLB_IPERF_STATE_DONE : BFLB_IPERF_STATE_ERROR;
    iperf_result_snapshot(iperf, &result);
    xSemaphoreGive(iperf->lock);

    if (iperf->config.event_cb != NULL) {
        iperf->config.event_cb(iperf, BFLB_IPERF_EVENT_FINISHED, &result,
                               iperf->config.user_data);
    }

    xSemaphoreTake(iperf->lock, portMAX_DELAY);
    iperf->worker = NULL;
    worker_done = iperf->worker_done;
    xSemaphoreGive(iperf->lock);
    /* The owner may free everything as soon as this give wakes it. Only the
     * worker's own stack and vTaskDelete(NULL) may be used from here onward. */
    xSemaphoreGive(worker_done);
}