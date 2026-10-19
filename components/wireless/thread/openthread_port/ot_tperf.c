/**
 * @file ot_tperf.c
 * @brief Thread Performance Test (tperf) - Core Implementation and CLI
 *
 * This module provides network performance testing functionality for OpenThread
 * networks using native UDP and TCP APIs.
 *
 * CLI commands are only available when CONFIG_SHELL is enabled.
 */

#if defined(CONFIG_THREAD_TPERF)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <bflb_mtimer.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#if defined(CONFIG_SHELL)
#include <shell.h>
#endif

#include <openthread-core-config.h>
#include <openthread/udp.h>
#include <openthread/tcp.h>
#include <openthread/message.h>

#include "openthread_port.h"
#include "ot_tperf.h"

#define TPERF_MAX_SESSIONS          2
#define TPERF_DEFAULT_PORT          5001
#define TPERF_DEFAULT_DURATION       10
#define TPERF_DEFAULT_INTERVAL       0  // 0 = use shorter intervals (0.5s for server, calculated for client)
#define TPERF_DEFAULT_BUFFER_SIZE    1024
#define TPERF_DEFAULT_BANDWIDTH      160
#define TPERF_TASK_STACK_SIZE       2048
#define TPERF_TASK_PRIORITY         5

#define TPERF_LOG(fmt, ...) printf("[tperf] " fmt "\r\n", ##__VA_ARGS__)
#define TPERF_LOGE(fmt, ...) printf("[tperf] ERROR: " fmt "\r\n", ##__VA_ARGS__)

//=============================================================================
// Data Structures
//=============================================================================

/**
 * @brief tperf session structure
 */
struct tperf_session {
    bool               in_use;
    bool               running;
    tperf_mode_t       mode;
    otInstance        *instance;

    // Configuration
    tperf_config_t     config;

    // Network resources
    otUdpSocket        udp_socket;
    otTcpEndpoint     *tcp_endpoint;

    // Statistics
    tperf_stats_t      stats;
    uint32_t           interval_bytes;
    uint64_t           interval_start_ms;

    // Task management
    TaskHandle_t       task_handle;
    SemaphoreHandle_t done_sem;

    // Error handling
    char               last_error[128];
};

//=============================================================================
// Global State
//=============================================================================

static tperf_session_t g_sessions[TPERF_MAX_SESSIONS];
static bool g_initialized = false;

//=============================================================================
// Internal Helper Functions
//=============================================================================

static tperf_session_t *alloc_session(void) {
    for (int i = 0; i < TPERF_MAX_SESSIONS; i++) {
        if (!g_sessions[i].in_use) {
            memset(&g_sessions[i], 0, sizeof(g_sessions[i]));
            g_sessions[i].in_use = true;
            return &g_sessions[i];
        }
    }
    return NULL;
}

static void free_session(tperf_session_t *session) {
    if (session) {
        if (session->done_sem) {
            vSemaphoreDelete(session->done_sem);
        }
        memset(session, 0, sizeof(*session));
    }
}

static void set_error(tperf_session_t *session, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(session->last_error, sizeof(session->last_error), fmt, args);
    va_end(args);
    // ✅ Also print error immediately
    printf("[tperf] ERROR: %s\r\n", session->last_error);
}

//=============================================================================
// UDP Implementation
//=============================================================================

static void udp_receive_handler(void *aContext, otMessage *aMessage,
                                const otMessageInfo *aMessageInfo) {
    tperf_session_t *session = (tperf_session_t *)aContext;
    uint16_t         len = otMessageGetLength(aMessage);

    // Update statistics
    session->stats.total_bytes += len;
    session->stats.total_packets++;
    session->interval_bytes += len;
}

static void udp_client_task(void *arg) {
    tperf_session_t *session = (tperf_session_t *)arg;
    otMessage       *msg;
    otMessageInfo   msgInfo;
    uint8_t        *buffer = NULL;
    uint32_t        total_sent = 0;
    uint64_t        now;
    uint32_t        delay_us = 0;

    buffer = malloc(session->config.buffer_size);
    if (!buffer) {
        set_error(session, "Failed to allocate buffer");
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    // Fill buffer with pattern
    memset(buffer, 0xA5, session->config.buffer_size);

    // Setup message info
    memset(&msgInfo, 0, sizeof(msgInfo));
    memcpy(&msgInfo.mPeerAddr, &session->config.peer_addr, sizeof(otIp6Address));
    msgInfo.mPeerPort = session->config.peer_port;
    msgInfo.mSockPort = session->config.local_port;

    // Calculate packet interval if bandwidth limited
    if (session->config.bandwidth > 0) {
        delay_us = (session->config.buffer_size * 8 * 1000) / session->config.bandwidth;
    }

    session->stats.start_time_ms = bflb_mtimer_get_time_ms();
    session->interval_start_ms = session->stats.start_time_ms;

    TPERF_LOG("UDP client started to port %u", session->config.peer_port);

    uint32_t send_count = 0;
    uint32_t fail_count = 0;

    while (session->running) {
        msg = otUdpNewMessage(session->instance, NULL);
        if (!msg) {
            set_error(session, "Failed to create message");
            break;
        }

        if (otMessageAppend(msg, buffer, session->config.buffer_size) != OT_ERROR_NONE) {
            otMessageFree(msg);
            set_error(session, "Failed to append data");
            break;
        }

        otError error = otUdpSend(session->instance, &session->udp_socket, msg, &msgInfo);
        if (error != OT_ERROR_NONE) {
            otMessageFree(msg);
            fail_count++;
            set_error(session, "Failed to send UDP packet: %d", error);
            break;
        }

        send_count++;
        total_sent += session->config.buffer_size;
        session->stats.total_bytes += session->config.buffer_size;
        session->stats.total_packets++;
        session->interval_bytes += session->config.buffer_size;

        // Report progress
        now = bflb_mtimer_get_time_ms();
        uint32_t report_interval_ms = (session->config.interval > 0) ?
                                      (session->config.interval * 1000) : 1000;
        if (now - session->interval_start_ms >= report_interval_ms) {
            double interval_sec = (now - session->interval_start_ms) / 1000.0;
            double bandwidth_kbps = (session->interval_bytes * 8.0 / interval_sec) / 1024.0;
            double transfer_kb = session->interval_bytes / 1024.0;

            printf("[  0] %5.1f-%5.1f sec  %.2f KBytes  %.2f Kbits/sec\r\n",
                   (session->stats.start_time_ms / 1000.0),
                   (now / 1000.0), transfer_kb, bandwidth_kbps);

            session->interval_start_ms = now;
            session->interval_bytes = 0;
        }

        // Check duration
        if (session->config.duration > 0 &&
            (now - session->stats.start_time_ms) >= session->config.duration * 1000) {
            break;
        }

        // Rate limiting
        if (delay_us > 0) {
            bflb_mtimer_delay_us(delay_us);
        } else {
            taskYIELD();
        }
    }

    // Final report
    session->stats.end_time_ms = bflb_mtimer_get_time_ms();
    double total_duration = (session->stats.end_time_ms - session->stats.start_time_ms) / 1000.0;
    double total_kbytes = total_sent / 1024.0;
    double bandwidth_kbps = (total_sent * 8.0 / total_duration) / 1024.0;

    printf("[  0] %5.1f-%5.1f sec  %.2f KBytes  %.2f Kbits/sec\r\n",
           0.0, total_duration, total_kbytes, bandwidth_kbps);

    printf("[tperf] UDP client finished: %u sent, %u failed, %.2f seconds\r\n",
           send_count, fail_count, total_duration);
    if (fail_count > 0) {
        printf("[tperf] WARNING: %u send failures detected - check network connectivity\r\n", fail_count);
    }

    TPERF_LOG("UDP client finished");

    free(buffer);
    session->running = false;
    xSemaphoreGive(session->done_sem);
    vTaskDelete(NULL);
}

static void udp_server_task(void *arg) {
    tperf_session_t *session = (tperf_session_t *)arg;
    uint64_t        now;
    uint32_t        report_interval_ms = (session->config.interval > 0) ? (session->config.interval * 1000) : 500;

    session->stats.start_time_ms = bflb_mtimer_get_time_ms();
    session->interval_start_ms = session->stats.start_time_ms;

    TPERF_LOG("UDP server listening on port %u", session->config.local_port);

    while (session->running) {
        vTaskDelay(pdMS_TO_TICKS(500));  // Check every 500ms

        now = bflb_mtimer_get_time_ms();

        // Report progress
        if (now - session->interval_start_ms >= report_interval_ms) {
            double interval_sec = (now - session->interval_start_ms) / 1000.0;
            double bandwidth_kbps = (session->interval_bytes * 8.0 / interval_sec) / 1024.0;
            double transfer_kb = session->interval_bytes / 1024.0;

            printf("[  0] %5.1f-%5.1f sec  %.2f KBytes  %.2f Kbits/sec\r\n",
                   (session->interval_start_ms / 1000.0),
                   (now / 1000.0), transfer_kb, bandwidth_kbps);

            session->interval_start_ms = now;
            session->interval_bytes = 0;
        }
    }

    session->stats.end_time_ms = bflb_mtimer_get_time_ms();

    // Calculate final statistics
    double total_duration = (session->stats.end_time_ms - session->stats.start_time_ms) / 1000.0;
    double total_kbytes = session->stats.total_bytes / 1024.0;
    double bandwidth_kbps = (session->stats.total_bytes * 8.0 / total_duration) / 1024.0;

    // Print final statistics
    printf("[  0] %5.1f-%5.1f sec  %.2f KBytes  %.2f Kbits/sec\r\n",
           0.0, total_duration, total_kbytes, bandwidth_kbps);

    printf("[tperf] UDP server finished: %u packets received\r\n",
           session->stats.total_packets);

    TPERF_LOG("UDP server finished");

    session->running = false;
    xSemaphoreGive(session->done_sem);
    vTaskDelete(NULL);
}

//=============================================================================
// TCP Implementation
//=============================================================================

#define TPERF_TCP_RECV_BUFFER_SIZE  4157  // OT_TCP_RECEIVE_BUFFER_SIZE_MANY_HOPS
#define TPERF_TCP_SEND_BUFFER_SIZE  3000

// TCP context for callbacks
typedef struct {
    tperf_session_t *session;
    uint8_t          *recv_buffer;
    otTcpEndpoint    endpoint;
    otTcpListener    listener;
    bool             is_connected;
    bool             is_accepted;
    SemaphoreHandle_t send_sem;
} tcp_context_t;

// Forward declarations
static void tcp_established_callback(otTcpEndpoint *aEndpoint);
static void tcp_send_done_callback(otTcpEndpoint *aEndpoint, otLinkedBuffer *aData);
static void tcp_receive_available_callback(otTcpEndpoint *aEndpoint, size_t aBytesAvailable, bool aEndOfStream, size_t aBytesRemaining);
static void tcp_disconnected_callback(otTcpEndpoint *aEndpoint, otTcpDisconnectedReason aReason);
static otTcpIncomingConnectionAction tcp_accept_ready_callback(otTcpListener *aListener, const otSockAddr *aPeer, otTcpEndpoint **aAcceptInto);
static void tcp_accept_done_callback(otTcpListener *aListener, otTcpEndpoint *aEndpoint, const otSockAddr *aPeer);

static void tcp_established_callback(otTcpEndpoint *aEndpoint) {
    tcp_context_t *ctx = (tcp_context_t *)otTcpEndpointGetContext(aEndpoint);
    if (ctx && ctx->session) {
        TPERF_LOG("TCP connection established");
        ctx->is_connected = true;
    }
}

static void tcp_send_done_callback(otTcpEndpoint *aEndpoint, otLinkedBuffer *aData) {
    tcp_context_t *ctx = (tcp_context_t *)otTcpEndpointGetContext(aEndpoint);
    if (ctx && ctx->send_sem) {
        xSemaphoreGive(ctx->send_sem);
    }
}

static void tcp_receive_available_callback(otTcpEndpoint *aEndpoint, size_t aBytesAvailable, bool aEndOfStream, size_t aBytesRemaining) {
    tcp_context_t *ctx = (tcp_context_t *)otTcpEndpointGetContext(aEndpoint);
    if (!ctx || !ctx->session) {
        return;
    }

    const otLinkedBuffer *buffer;
    if (otTcpReceiveByReference(aEndpoint, &buffer) == OT_ERROR_NONE) {
        // Process all linked buffers
        while (buffer != NULL) {
            size_t len = buffer->mLength;
            ctx->session->stats.total_bytes += len;
            ctx->session->stats.total_packets++;
            ctx->session->interval_bytes += len;
            buffer = buffer->mNext;
        }

        // Commit all received data
        otTcpCommitReceive(aEndpoint, aBytesAvailable, 0);
    }

    if (aEndOfStream) {
        TPERF_LOG("TCP connection closed by peer");
        ctx->is_connected = false;
    }
}

static void tcp_disconnected_callback(otTcpEndpoint *aEndpoint, otTcpDisconnectedReason aReason) {
    tcp_context_t *ctx = (tcp_context_t *)otTcpEndpointGetContext(aEndpoint);
    if (ctx) {
        const char *reason_str = "UNKNOWN";
        switch (aReason) {
            case OT_TCP_DISCONNECTED_REASON_NORMAL:  reason_str = "NORMAL"; break;
            case OT_TCP_DISCONNECTED_REASON_REFUSED: reason_str = "REFUSED"; break;
            case OT_TCP_DISCONNECTED_REASON_RESET:   reason_str = "RESET"; break;
            case OT_TCP_DISCONNECTED_REASON_TIME_WAIT: reason_str = "TIME_WAIT"; break;
            case OT_TCP_DISCONNECTED_REASON_TIMED_OUT: reason_str = "TIMED_OUT"; break;
        }
        TPERF_LOG("TCP disconnected: %s", reason_str);
        ctx->is_connected = false;
    }
}

static otTcpIncomingConnectionAction tcp_accept_ready_callback(otTcpListener *aListener, const otSockAddr *aPeer, otTcpEndpoint **aAcceptInto) {
    tcp_context_t *ctx = (tcp_context_t *)otTcpListenerGetInstance(aListener);
    if (!ctx || !ctx->session) {
        return OT_TCP_INCOMING_CONNECTION_ACTION_REFUSE;
    }

    *aAcceptInto = &ctx->endpoint;
    return OT_TCP_INCOMING_CONNECTION_ACTION_ACCEPT;
}

static void tcp_accept_done_callback(otTcpListener *aListener, otTcpEndpoint *aEndpoint, const otSockAddr *aPeer) {
    tcp_context_t *ctx = (tcp_context_t *)otTcpEndpointGetContext(aEndpoint);
    if (ctx) {
        TPERF_LOG("TCP connection accepted");
        ctx->is_connected = true;
        ctx->is_accepted = true;
    }
}

static void tcp_server_task(void *arg) {
    tperf_session_t *session = (tperf_session_t *)arg;
    tcp_context_t *ctx = NULL;
    uint64_t        now;

    // Allocate TCP context
    ctx = malloc(sizeof(tcp_context_t));
    if (!ctx) {
        set_error(session, "Failed to allocate TCP context");
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    memset(ctx, 0, sizeof(tcp_context_t));
    ctx->session = session;
    ctx->recv_buffer = malloc(TPERF_TCP_RECV_BUFFER_SIZE);
    if (!ctx->recv_buffer) {
        set_error(session, "Failed to allocate TCP recv buffer");
        free(ctx);
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    ctx->send_sem = xSemaphoreCreateBinary();
    if (!ctx->send_sem) {
        set_error(session, "Failed to create TCP send semaphore");
        free(ctx->recv_buffer);
        free(ctx);
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    // Initialize TCP endpoint
    otTcpEndpointInitializeArgs endpoint_args = {
        .mContext = ctx,
        .mEstablishedCallback = tcp_established_callback,
        .mSendDoneCallback = tcp_send_done_callback,
        .mForwardProgressCallback = NULL,
        .mReceiveAvailableCallback = tcp_receive_available_callback,
        .mDisconnectedCallback = tcp_disconnected_callback,
        .mReceiveBuffer = ctx->recv_buffer,
        .mReceiveBufferSize = TPERF_TCP_RECV_BUFFER_SIZE,
    };

    otError error = otTcpEndpointInitialize(session->instance, &ctx->endpoint, &endpoint_args);
    if (error != OT_ERROR_NONE) {
        set_error(session, "Failed to initialize TCP endpoint: %d", error);
        vSemaphoreDelete(ctx->send_sem);
        free(ctx->recv_buffer);
        free(ctx);
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    // Initialize TCP listener
    otTcpListenerInitializeArgs listener_args = {
        .mContext = ctx,
        .mAcceptReadyCallback = tcp_accept_ready_callback,
        .mAcceptDoneCallback = tcp_accept_done_callback,
    };

    error = otTcpListenerInitialize(session->instance, &ctx->listener, &listener_args);
    if (error != OT_ERROR_NONE) {
        set_error(session, "Failed to initialize TCP listener: %d", error);
        otTcpEndpointDeinitialize(&ctx->endpoint);
        vSemaphoreDelete(ctx->send_sem);
        free(ctx->recv_buffer);
        free(ctx);
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    // Start listening (accept connections) on specified port
    otSockAddr sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.mPort = session->config.local_port;

    error = otTcpListen(&ctx->listener, &sockAddr);
    if (error != OT_ERROR_NONE) {
        set_error(session, "Failed to start TCP listen: %d", error);
        otTcpListenerDeinitialize(&ctx->listener);
        otTcpEndpointDeinitialize(&ctx->endpoint);
        vSemaphoreDelete(ctx->send_sem);
        free(ctx->recv_buffer);
        free(ctx);
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    session->stats.start_time_ms = bflb_mtimer_get_time_ms();
    session->interval_start_ms = session->stats.start_time_ms;

    TPERF_LOG("TCP server listening on port %u", session->config.local_port);

    // Server main loop - wait for connection, receive data
    while (session->running) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Report progress
        now = bflb_mtimer_get_time_ms();
        if (now - session->interval_start_ms >= session->config.interval * 1000) {
            double interval_sec = (now - session->interval_start_ms) / 1000.0;
            double bandwidth_kbps = (session->interval_bytes * 8.0 / interval_sec) / 1024.0;
            double transfer_kb = session->interval_bytes / 1024.0;

            printf("[  0] %5.1f-%5.1f sec  %.2f KBytes  %.2f Kbits/sec\r\n",
                   (session->stats.start_time_ms / 1000.0),
                   (now / 1000.0), transfer_kb, bandwidth_kbps);

            session->interval_start_ms = now;
            session->interval_bytes = 0;
        }

        // Check duration
        if (session->config.duration > 0 &&
            (now - session->stats.start_time_ms) >= session->config.duration * 1000) {
            break;
        }
    }

    // Cleanup
    session->stats.end_time_ms = bflb_mtimer_get_time_ms();
    TPERF_LOG("TCP server finished");

    otTcpAbort(&ctx->endpoint);
    otTcpListenerDeinitialize(&ctx->listener);
    otTcpEndpointDeinitialize(&ctx->endpoint);
    vSemaphoreDelete(ctx->send_sem);
    free(ctx->recv_buffer);
    free(ctx);

    session->running = false;
    xSemaphoreGive(session->done_sem);
    vTaskDelete(NULL);
}

static void tcp_client_task(void *arg) {
    tperf_session_t *session = (tperf_session_t *)arg;
    tcp_context_t *ctx = NULL;
    otLinkedBuffer  send_buffer;
    uint8_t         *data_buffer = NULL;
    uint32_t        total_sent = 0;
    uint64_t        now;
    uint32_t        delay_us = 0;

    // Allocate TCP context
    ctx = malloc(sizeof(tcp_context_t));
    if (!ctx) {
        set_error(session, "Failed to allocate TCP context");
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    memset(ctx, 0, sizeof(tcp_context_t));
    ctx->session = session;
    ctx->recv_buffer = malloc(TPERF_TCP_RECV_BUFFER_SIZE);
    if (!ctx->recv_buffer) {
        set_error(session, "Failed to allocate TCP recv buffer");
        free(ctx);
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    ctx->send_sem = xSemaphoreCreateBinary();
    if (!ctx->send_sem) {
        set_error(session, "Failed to create TCP send semaphore");
        free(ctx->recv_buffer);
        free(ctx);
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    // Allocate data buffer
    data_buffer = malloc(session->config.buffer_size);
    if (!data_buffer) {
        set_error(session, "Failed to allocate data buffer");
        vSemaphoreDelete(ctx->send_sem);
        free(ctx->recv_buffer);
        free(ctx);
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    // Fill with pattern
    memset(data_buffer, 0xAA, session->config.buffer_size);

    // Initialize TCP endpoint
    otTcpEndpointInitializeArgs endpoint_args = {
        .mContext = ctx,
        .mEstablishedCallback = tcp_established_callback,
        .mSendDoneCallback = tcp_send_done_callback,
        .mForwardProgressCallback = NULL,
        .mReceiveAvailableCallback = NULL,  // Client doesn't need receive
        .mDisconnectedCallback = tcp_disconnected_callback,
        .mReceiveBuffer = ctx->recv_buffer,
        .mReceiveBufferSize = TPERF_TCP_RECV_BUFFER_SIZE,
    };

    otError error = otTcpEndpointInitialize(session->instance, &ctx->endpoint, &endpoint_args);
    if (error != OT_ERROR_NONE) {
        set_error(session, "Failed to initialize TCP endpoint: %d", error);
        vSemaphoreDelete(ctx->send_sem);
        free(ctx->recv_buffer);
        free(data_buffer);
        free(ctx);
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    // Connect to server
    otSockAddr peerAddr;
    memset(&peerAddr, 0, sizeof(peerAddr));
    memcpy(&peerAddr.mAddress, &session->config.peer_addr, sizeof(otIp6Address));
    peerAddr.mPort = session->config.peer_port;

    TPERF_LOG("TCP client connecting to port %u...", session->config.peer_port);
    error = otTcpConnect(&ctx->endpoint, &peerAddr, OT_TCP_CONNECT_NO_FAST_OPEN);
    if (error != OT_ERROR_NONE) {
        set_error(session, "Failed to connect: %d", error);
        otTcpEndpointDeinitialize(&ctx->endpoint);
        vSemaphoreDelete(ctx->send_sem);
        free(ctx->recv_buffer);
        free(data_buffer);
        free(ctx);
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    // Wait for connection to be established
    int wait_count = 0;
    while (!ctx->is_connected && wait_count < 50) {
        vTaskDelay(pdMS_TO_TICKS(100));
        wait_count++;
    }

    if (!ctx->is_connected) {
        set_error(session, "Connection timeout");
        otTcpAbort(&ctx->endpoint);
        otTcpEndpointDeinitialize(&ctx->endpoint);
        vSemaphoreDelete(ctx->send_sem);
        free(ctx->recv_buffer);
        free(data_buffer);
        free(ctx);
        session->running = false;
        xSemaphoreGive(session->done_sem);
        vTaskDelete(NULL);
        return;
    }

    // Calculate packet interval if bandwidth limited
    if (session->config.bandwidth > 0) {
        delay_us = (session->config.buffer_size * 8 * 1000) / session->config.bandwidth;
    }

    // Prepare send buffer
    send_buffer.mNext = NULL;
    send_buffer.mData = data_buffer;
    send_buffer.mLength = session->config.buffer_size;

    session->stats.start_time_ms = bflb_mtimer_get_time_ms();
    session->interval_start_ms = session->stats.start_time_ms;

    TPERF_LOG("TCP client started sending to port %u", session->config.peer_port);

    // Send loop
    while (session->running && ctx->is_connected) {
        // Take semaphore first
        xSemaphoreTake(ctx->send_sem, 0);

        // Send data
        error = otTcpSendByReference(&ctx->endpoint, &send_buffer, 0);
        if (error != OT_ERROR_NONE) {
            // Send failed, maybe buffer full
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        // Wait for send done callback
        if (xSemaphoreTake(ctx->send_sem, pdMS_TO_TICKS(1000)) != pdTRUE) {
            // Timeout, but might still be sending
            continue;
        }

        total_sent += session->config.buffer_size;
        session->stats.total_bytes += session->config.buffer_size;
        session->stats.total_packets++;
        session->interval_bytes += session->config.buffer_size;

        // Report progress
        now = bflb_mtimer_get_time_ms();
        if (now - session->interval_start_ms >= session->config.interval * 1000) {
            double interval_sec = (now - session->interval_start_ms) / 1000.0;
            double bandwidth_kbps = (session->interval_bytes * 8.0 / interval_sec) / 1024.0;
            double transfer_kb = session->interval_bytes / 1024.0;

            printf("[  0] %5.1f-%5.1f sec  %.2f KBytes  %.2f Kbits/sec\r\n",
                   (session->stats.start_time_ms / 1000.0),
                   (now / 1000.0), transfer_kb, bandwidth_kbps);

            session->interval_start_ms = now;
            session->interval_bytes = 0;
        }

        // Check duration
        if (session->config.duration > 0 &&
            (now - session->stats.start_time_ms) >= session->config.duration * 1000) {
            break;
        }

        // Rate limiting
        if (delay_us > 0) {
            bflb_mtimer_delay_us(delay_us);
        } else {
            taskYIELD();
        }
    }

    // Final report
    session->stats.end_time_ms = bflb_mtimer_get_time_ms();
    double total_duration = (session->stats.end_time_ms - session->stats.start_time_ms) / 1000.0;
    double total_kbytes = total_sent / 1024.0;
    double bandwidth_kbps = (total_sent * 8.0 / total_duration) / 1024.0;

    printf("[  0] %5.1f-%5.1f sec  %.2f KBytes  %.2f Kbits/sec\r\n",
           0.0, total_duration, total_kbytes, bandwidth_kbps);

    TPERF_LOG("TCP client finished");

    // Cleanup
    otTcpAbort(&ctx->endpoint);
    otTcpEndpointDeinitialize(&ctx->endpoint);
    vSemaphoreDelete(ctx->send_sem);
    free(ctx->recv_buffer);
    free(data_buffer);
    free(ctx);

    session->running = false;
    xSemaphoreGive(session->done_sem);
    vTaskDelete(NULL);
}

//=============================================================================
// Public API Implementation
//=============================================================================

int tperf_init(otInstance *instance) {
    if (g_initialized) {
        return 0; // Already initialized
    }

    if (!instance) {
        return -1;
    }

    memset(g_sessions, 0, sizeof(g_sessions));
    g_initialized = true;

    return 0;
}

void tperf_deinit(void) {
    if (!g_initialized) {
        return;
    }

    // Stop all sessions
    for (int i = 0; i < TPERF_MAX_SESSIONS; i++) {
        if (g_sessions[i].in_use) {
            tperf_stop(&g_sessions[i]);
        }
    }

    g_initialized = false;
}

bool tperf_is_initialized(void) {
    return g_initialized;
}

tperf_session_t *tperf_start(const tperf_config_t *config) {
    tperf_session_t *session;
    otError          error;
    BaseType_t       ret;

    if (!g_initialized) {
        TPERF_LOGE("Not initialized");
        return NULL;
    }

    if (!config) {
        TPERF_LOGE("Invalid config");
        return NULL;
    }

    // Allocate session
    session = alloc_session();
    if (!session) {
        TPERF_LOGE("No free session");
        return NULL;
    }

    // Copy configuration
    session->config = *config;
    session->instance = otrGetInstance();
    session->mode = config->mode;

    // Create done semaphore
    session->done_sem = xSemaphoreCreateBinary();
    if (!session->done_sem) {
        set_error(session, "Failed to create semaphore");
        free_session(session);
        return NULL;
    }
    xSemaphoreTake(session->done_sem, 0);

    // Initialize statistics
    memset(&session->stats, 0, sizeof(session->stats));

    // UDP mode
    if (session->mode == TPERF_MODE_UDP_SERVER || session->mode == TPERF_MODE_UDP_CLIENT) {
        // Open UDP socket with receive callback (for server mode)
        otUdpReceive callback = (session->mode == TPERF_MODE_UDP_SERVER) ? udp_receive_handler : NULL;
        error = otUdpOpen(session->instance, &session->udp_socket, callback, session);
        if (error != OT_ERROR_NONE) {
            set_error(session, "Failed to open UDP socket: %d", error);
            free_session(session);
            return NULL;
        }

        otSockAddr sockAddr;
        memset(&sockAddr, 0, sizeof(sockAddr));
        sockAddr.mPort = session->config.local_port;

        error = otUdpBind(session->instance, &session->udp_socket,
                          &sockAddr, OT_NETIF_THREAD_INTERNAL);
        if (error != OT_ERROR_NONE) {
            set_error(session, "Failed to bind: %d", error);
            otUdpClose(session->instance, &session->udp_socket);
            free_session(session);
            return NULL;
        }
    }

    // Start task
    session->running = true;

    if (session->mode == TPERF_MODE_UDP_CLIENT) {
        ret = xTaskCreate(udp_client_task, "tperf_udp_cli",
                         TPERF_TASK_STACK_SIZE, session,
                         session->config.task_priority, &session->task_handle);
    } else if (session->mode == TPERF_MODE_UDP_SERVER) {
        ret = xTaskCreate(udp_server_task, "tperf_udp_srv",
                         TPERF_TASK_STACK_SIZE, session,
                         session->config.task_priority, &session->task_handle);
    } else if (session->mode == TPERF_MODE_TCP_CLIENT) {
        ret = xTaskCreate(tcp_client_task, "tperf_tcp_cli",
                         TPERF_TASK_STACK_SIZE * 2, session,  // TCP needs more stack
                         session->config.task_priority, &session->task_handle);
    } else if (session->mode == TPERF_MODE_TCP_SERVER) {
        ret = xTaskCreate(tcp_server_task, "tperf_tcp_srv",
                         TPERF_TASK_STACK_SIZE * 2, session,  // TCP needs more stack
                         session->config.task_priority, &session->task_handle);
    } else {
        set_error(session, "Mode not implemented");
        session->running = false;
        free_session(session);
        return NULL;
    }

    if (ret != pdPASS) {
        set_error(session, "Failed to create task");
        session->running = false;
        if (session->mode == TPERF_MODE_UDP_SERVER || session->mode == TPERF_MODE_UDP_CLIENT) {
            otUdpClose(session->instance, &session->udp_socket);
        }
        free_session(session);
        return NULL;
    }

    TPERF_LOG("Session started");
    return session;
}

int tperf_stop(tperf_session_t *session) {
    if (!session || !session->in_use) {
        return -1;
    }

    if (!session->running) {
        // Session was stopped but not properly freed, clean it up now
        TPERF_LOG("Cleaning up previously stopped session...");
        goto cleanup;
    }

    TPERF_LOG("Stopping session...");

    session->running = false;

    // Close socket
    if (session->mode == TPERF_MODE_UDP_SERVER || session->mode == TPERF_MODE_UDP_CLIENT) {
        otUdpClose(session->instance, &session->udp_socket);
    }

    // Wait for task to finish
    if (session->task_handle) {
        xSemaphoreTake(session->done_sem, pdMS_TO_TICKS(2000));
        session->task_handle = NULL;
    }

cleanup:
    // ✅ FIX: Properly release the session resources
    if (session->done_sem) {
        vSemaphoreDelete(session->done_sem);
        session->done_sem = NULL;
    }

    // ✅ FIX: Mark session as not in use so it can be reused
    session->in_use = false;

    // ✅ FIX: Clear the entire session structure (except we don't need to memset in_use again)
    memset(&session->running, 0, sizeof(*session) - offsetof(struct tperf_session, in_use));

    TPERF_LOG("Session stopped and released");

    return 0;
}

int tperf_get_stats(tperf_session_t *session, tperf_stats_t *stats) {
    if (!session || !stats) {
        return -1;
    }

    memcpy(stats, &session->stats, sizeof(*stats));
    return 0;
}

bool tperf_is_running(tperf_session_t *session) {
    return (session && session->running);
}

tperf_mode_t tperf_get_mode(tperf_session_t *session) {
    return session ? session->mode : TPERF_MODE_IDLE;
}

int tperf_wait_completion(tperf_session_t *session, uint32_t timeout_ms) {
    if (!session || !session->in_use) {
        return -1;
    }

    if (timeout_ms == 0) {
        xSemaphoreTake(session->done_sem, portMAX_DELAY);
    } else {
        if (xSemaphoreTake(session->done_sem, pdMS_TO_TICKS(timeout_ms)) != pdTRUE) {
            return -1; // Timeout
        }
        // Give back the semaphore for future use
        xSemaphoreGive(session->done_sem);
    }

    return 0;
}

const char *tperf_get_last_error(void) {
    // Return last error from first active session
    for (int i = 0; i < TPERF_MAX_SESSIONS; i++) {
        if (g_sessions[i].in_use && g_sessions[i].last_error[0] != '\0') {
            return g_sessions[i].last_error;
        }
    }
    return "No error";
}

//=============================================================================
// CLI Interface (only when CONFIG_SHELL is enabled)
//=============================================================================

#if defined(CONFIG_SHELL)

// Global session handle (simplified for single session use)
static tperf_session_t *g_cli_session = NULL;

// Helper to parse IPv6 address
static bool parse_ipv6_address(const char *str, otIp6Address *addr) {
    // Simple check for valid IPv6 format
    if (!str || !addr) {
        return false;
    }

    // For now, accept any non-empty string with ':'
    // OpenThread provides otIp6AddressFromString() for proper parsing
    if (strlen(str) == 0 || strchr(str, ':') == NULL) {
        printf("Error: Invalid IPv6 address format\r\n");
        return false;
    }

    // Use OpenThread's address parser
    if (otIp6AddressFromString(str, addr) != OT_ERROR_NONE) {
        printf("Error: Failed to parse IPv6 address\r\n");
        return false;
    }

    return true;
}

// tperf command
static int cmd_tperf(int argc, char **argv) {
    tperf_config_t config;
    otIp6Address   peer_addr;
    bool           have_peer = false;

    if (argc < 2) {
        printf("Thread Performance Test (tperf)\r\n");
        printf("\r\n");
        printf("Usage:\r\n");
        printf("  tperf -s                    : UDP server mode\r\n");
        printf("  tperf -S                    : TCP server mode\r\n");
        printf("  tperf -c <peer_address>     : UDP client mode\r\n");
        printf("  tperf -C <peer_address>     : TCP client mode\r\n");
        printf("  tperf -a                    : Abort running test\r\n");
        printf("\r\n");
        printf("Options:\r\n");
        printf("  -p <port>                   : Port number (default: %d)\r\n", TPERF_DEFAULT_PORT);
        printf("  -t <seconds>                : Test duration (default: %d)\r\n", TPERF_DEFAULT_DURATION);
        printf("  -i <seconds>                : Report interval (default: %d)\r\n", TPERF_DEFAULT_INTERVAL);
        printf("  -b <bandwidth_kbps>         : Bandwidth limit (default: %d)\r\n", TPERF_DEFAULT_BANDWIDTH);
        printf("  -l <buffer_size>            : Buffer size in bytes (default: %d)\r\n", TPERF_DEFAULT_BUFFER_SIZE);
        printf("\r\n");
        printf("Examples:\r\n");
        printf("  tperf -s -p 5001                           # UDP server\r\n");
        printf("  tperf -S -p 5001                           # TCP server\r\n");
        printf("  tperf -c fe80::1234:5678:90ab -t %d -b %d # UDP client\r\n", TPERF_DEFAULT_DURATION, TPERF_DEFAULT_BANDWIDTH);
        printf("  tperf -C fe80::1234:5678:90ab -t %d -b %d # TCP client\r\n", TPERF_DEFAULT_DURATION, TPERF_DEFAULT_BANDWIDTH);
        printf("\r\n");
        return 0;
    }

    // Abort command
    if (strcmp(argv[1], "-a") == 0) {
        if (g_cli_session && tperf_is_running(g_cli_session)) {
            printf("Aborting tperf session...\r\n");
            tperf_stop(g_cli_session);
            g_cli_session = NULL;
            printf("Aborted\r\n");
        } else {
            printf("No running session\r\n");
        }
        return 0;
    }

    // Check if already running
    if (g_cli_session && tperf_is_running(g_cli_session)) {
        printf("Error: Session already running. Use 'tperf -a' to abort first.\r\n");
        return -1;
    }

    // Initialize config with defaults
    memset(&config, 0, sizeof(config));
    config.mode = TPERF_MODE_IDLE;
    config.local_port = TPERF_DEFAULT_PORT;
    config.duration = TPERF_DEFAULT_DURATION;
    config.interval = TPERF_DEFAULT_INTERVAL;
    config.bandwidth = TPERF_DEFAULT_BANDWIDTH;
    config.buffer_size = TPERF_DEFAULT_BUFFER_SIZE;
    config.task_priority = TPERF_TASK_PRIORITY;

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            config.mode = TPERF_MODE_UDP_SERVER;
        }
        else if (strcmp(argv[i], "-S") == 0) {
            config.mode = TPERF_MODE_TCP_SERVER;
        }
        else if (strcmp(argv[i], "-c") == 0) {
            if (i + 1 >= argc) {
                printf("Error: -c requires an address\r\n");
                return -1;
            }
            config.mode = TPERF_MODE_UDP_CLIENT;
            if (!parse_ipv6_address(argv[++i], &peer_addr)) {
                return -1;
            }
            memcpy(&config.peer_addr, &peer_addr, sizeof(otIp6Address));
            config.peer_port = TPERF_DEFAULT_PORT;
            have_peer = true;
        }
        else if (strcmp(argv[i], "-C") == 0) {
            if (i + 1 >= argc) {
                printf("Error: -C requires an address\r\n");
                return -1;
            }
            config.mode = TPERF_MODE_TCP_CLIENT;
            if (!parse_ipv6_address(argv[++i], &peer_addr)) {
                return -1;
            }
            memcpy(&config.peer_addr, &peer_addr, sizeof(otIp6Address));
            config.peer_port = TPERF_DEFAULT_PORT;
            have_peer = true;
        }
        else if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 >= argc) {
                printf("Error: -p requires a port number\r\n");
                return -1;
            }
            config.local_port = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 >= argc) {
                printf("Error: -t requires duration\r\n");
                return -1;
            }
            config.duration = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 >= argc) {
                printf("Error: -i requires interval\r\n");
                return -1;
            }
            config.interval = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-b") == 0) {
            if (i + 1 >= argc) {
                printf("Error: -b requires bandwidth\r\n");
                return -1;
            }
            config.bandwidth = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-l") == 0) {
            if (i + 1 >= argc) {
                printf("Error: -l requires buffer size\r\n");
                return -1;
            }
            config.buffer_size = atoi(argv[++i]);
        }
        else {
            printf("Error: Unknown option '%s'\r\n", argv[i]);
            return -1;
        }
    }

    // Validate mode
    if (config.mode == TPERF_MODE_IDLE) {
        printf("Error: Must specify -s/-S (server) or -c/-C <addr> (client)\r\n");
        return -1;
    }

    if ((config.mode == TPERF_MODE_UDP_CLIENT || config.mode == TPERF_MODE_TCP_CLIENT) && !have_peer) {
        printf("Error: Client mode requires peer address\r\n");
        return -1;
    }

    // Start session
    g_cli_session = tperf_start(&config);
    if (!g_cli_session) {
        printf("Error: Failed to start session: %s\r\n", tperf_get_last_error());
        return -1;
    }

    printf("tperf session started\r\n");

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_tperf, tperf, Thread performance test);

#endif // CONFIG_SHELL

#endif // CONFIG_THREAD_TPERF
