/**
 * @file hb_sender.h
 * @brief HiBooster video transmission sender API
 * @version 8.2
 * @date 2026-02-25
 * 
 * Usage:
 *   1. Call hb_sender_create() to create sender instance
 *   2. Register callback with hb_sender_on_msg() (optional)
 *   3. Push frames with hb_sender_push() when camera has new frame
 *   4. Internal task handles START/STOP from receiver and transmission
 *   5. Call hb_sender_destroy() to cleanup
 */

#ifndef HB_SENDER_H
#define HB_SENDER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration */
struct hb_sender;
typedef struct hb_sender hb_sender_t;

/* Frame release callback */
typedef void (*hb_release_fn)(void *arg);

/* User message callback (called when flyctl data received from ACK) */
typedef void (*hb_user_msg_cb)(hb_sender_t *s, const uint8_t *data,
                               uint16_t len, void *arg);

/**
 * @brief Sender configuration
 */
typedef struct {
    /* Network configuration */
    uint16_t local_port; // Local UDP port for receiving/sending messages

    /* Optional: Advanced configuration */
    uint16_t frame_depth;       // Ring buffer depth, must be power of 2, 0: default 4
    uint16_t burst_count;       // Burst send count per cycle, 0: auto
    uint16_t task_priority;     // Sender task priority, 0: default
    void (*loop_cb)(void *arg); // The loop callback function of the main thread
    void *loop_cb_arg;          // argument of the loop callback function
} hb_sender_config_t;

/**
 * @brief Frame information (user push)
 */
typedef struct {
    uint8_t *data;         // Image data
    uint32_t size;         // Image size
    uint16_t width;        // Image width
    uint16_t height;       // Image height
    uint8_t quality;       // JPEG quality
    uint64_t seq_total;    // Total sequence from sensor (0 = auto-increment)
    hb_release_fn release; // Release callback (called when frame is done)
    void *release_arg;     // Release callback argument
} hb_frame_info_t;

/**
 * @brief Statistics structure
 */
typedef struct {
    /* Frame statistics */
    uint32_t frames_sent;    // Frames successfully sent (acked)
    uint32_t frames_dropped; // Frames dropped (timeout or error)
    uint32_t frames_pending; // Frames waiting in queue

    /* Packet statistics */
    uint32_t pkts_sent;     // New fragments sent
    uint32_t pkts_resent;   // Fragments retransmitted
    uint32_t acks_received; // ACK messages received

    /* Latency statistics */
    uint16_t avg_latency_ms; // Average send latency (ms)
    uint16_t max_latency_ms; // Maximum send latency (ms)

    /* Timing */
    uint32_t start_time_ms; // Statistics start time (ms)
    uint32_t duration_ms;   // Duration since statistics started (ms)

    /* Internal parameters */
    uint32_t ack_delay_avg;
    uint32_t timeout_avg;
} hb_sender_stats_t;

/* ==================== Lifecycle ==================== */

/**
 * @brief Create sender instance
 * 
 * This creates sockets and starts internal task for:
 * - Listening for START/STOP/ACK messages from receiver
 * - Handling frame transmission and resend
 * 
 * @param cfg Configuration (must not be NULL)
 * @return Sender instance or NULL on failure
 */
hb_sender_t *hb_sender_create(const hb_sender_config_t *cfg);

/**
 * @brief Destroy sender instance
 * 
 * Stops internal task, closes sockets, frees all resources.
 * 
 * @param s Sender instance
 */
void hb_sender_destroy(hb_sender_t *s);

/* ==================== Transmission Control ==================== */

typedef enum {
    HB_SEND_STATE_IDLE = 0, /* Sender handle is NULL or destroying */
    HB_SEND_STATE_STOP,     /* Waiting for START */
    HB_SEND_STATE_SEND,     /* Sending/Resending */
} hb_sender_state_t;

/**
 * @brief Get sender state
 *
 * @param s Sender instance
 * @return Current state (NULL handle returns HB_SEND_STATE_IDLE)
 */
hb_sender_state_t hb_sender_get_state(hb_sender_t *s);

/* ==================== Frame Push ==================== */

/**
 * @brief Push frame to send queue
 * 
 * Thread-safe. Can be called from camera callback.
 * Frame will be sent after receiver sends START message.
 * 
 * @param s Sender instance
 * @param info Frame information
 * @return 0 on success, -1 on failure (queue full or invalid)
 */
int hb_sender_push(hb_sender_t *s, const hb_frame_info_t *info);

/**
 * @brief Get available frame buffer slots in sender queue
 *
 * @param s Sender instance
 * @return Available slots count (0 when invalid)
 */
uint16_t hb_send_buf_available(hb_sender_t *s);

/* ==================== Control Message ==================== */

/**
 * @brief Send control message to peer (MCU -> Peer)
 * 
 * Control messages are only sent when transmission is active.
 * Will retry until ACK received from peer.
 * 
 * @param s Sender instance
 * @param data Control message data
 * @param len Data length (max 128 bytes)
 * @param seq Output sequence number (optional, can be NULL)
 * @return 0 on success, -1 on invalid params, -2 if previous msg pending
 */
int hb_sender_send_ctl(hb_sender_t *s, const uint8_t *data, uint16_t len, uint32_t *seq);

/* ==================== Callback ==================== */

/**
 * @brief Register user message callback
 * 
 * Called when flyctl data is received from ACK message.
 * This is how receiver sends control commands to MCU.
 * 
 * @param s Sender instance
 * @param cb Callback function
 * @param arg Callback argument
 */
// void hb_sender_on_msg(hb_sender_t *s, hb_user_msg_cb cb, void *arg);

/* ==================== Statistics ==================== */

/**
 * @brief Get statistics (copy to user buffer)
 * 
 * Copies current statistics to the provided buffer.
 * Does not modify internal statistics.
 * 
 * @param s Sender instance
 * @param stats Output buffer for statistics
 * @return 0 on success, -1 on invalid params
 */
int hb_sender_get_stats(hb_sender_t *s, hb_sender_stats_t *stats);

/**
 * @brief Clear statistics
 * 
 * Resets all counters to zero and updates start time.
 * 
 * @param s Sender instance
 * @return 0 on success, -1 on invalid params
 */
int hb_sender_clear_stats(hb_sender_t *s);

#ifdef __cplusplus
}
#endif

#endif /* HB_SENDER_H */
