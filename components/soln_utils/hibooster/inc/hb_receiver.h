/**
 * @file hb_receiver.h
 * @brief HiBooster video transmission receiver API
 * @version 1.0
 * @date 2026-03-04
 * 
 * Usage:
 *   1. Call hb_receiver_create() to create receiver instance
 *   2. Push buffer descriptors with hb_recv_buf_push()
 *   3. Call hb_receiver_start() to start receiving
 *   4. Receive callback will be called when frame complete
 *   5. Call hb_receiver_stop() to stop receiving
 *   6. Call hb_receiver_destroy() to cleanup
 */

#ifndef HB_RECEIVER_H
#define HB_RECEIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== Status Codes ==================== */

#define HB_RECV_STATUS_OK       0    /* Successfully received */
#define HB_RECV_STATUS_DROP     1    /* Dropped (buffer too small) */
#define HB_RECV_STATUS_TIMEOUT  2    /* Receive timeout */
#define HB_RECV_STATUS_STOP     3    /* Stopped by user */

/* ==================== Data Structures ==================== */

/**
 * @brief User buffer descriptor
 * 
 * User fills data, size, user_arg and on_complete.
 * Receiver fills other fields before calling callback.
 */
typedef struct hb_recv_buf_desc {
    /* === User fills === */
    uint8_t *data;           /* Buffer address */
    uint32_t size;           /* Buffer size */
    void *user_arg;          /* User argument (passed to callback) */
    /* === Callback === */
    void (*on_complete)(struct hb_recv_buf_desc *desc);
    
    /* === Receiver fills === */
    uint32_t received_size;  /* Actual received size */
    uint8_t status;
    uint8_t quality;         /* JPEG quality */
    uint16_t width;          /* Image width */
    uint16_t height;         /* Image height */
    uint64_t seq;            /* Frame sequence number */
    
} hb_recv_buf_desc_t;

/**
 * @brief Receiver configuration
 */
typedef struct {
    uint16_t local_port;         /* Local port (receive fragments and send ACK/START/STOP) */
    uint16_t peer_port;          /* Peer port */
    ip_addr_t peer_addr;         /* Peer address */
    uint16_t frame_depth;        /* Frame queue depth, must be power of 2 (0=default 4) */
    uint16_t ack_interval_ms;    /* ACK interval (0=default 20ms) */
    uint16_t frame_timeout_ms;   /* Frame timeout (0=default 2000ms) */
    uint16_t task_priority;      /* Receiver task priority (0=default) */
    void (*loop_cb)(void *arg);  /* Main loop callback */
    void *loop_cb_arg;           /* Callback argument */
} hb_recv_config_t;

/**
 * @brief Receiver statistics
 */
typedef struct {
    /* Frame statistics */
    uint32_t frames_ok;          /* Successfully received frames */
    uint32_t frames_drop;        /* Dropped frames */
    uint32_t frames_pending;     /* Frames in receiving or waiting state */
    /* Packet statistics */
    uint32_t frags_received;     /* Received fragments */
    uint32_t frags_invalid;      /* Invalid packets */
    uint32_t acks_sent;          /* ACKs sent */
    /* Timing */
    uint32_t start_time_ms;      /* Statistics start time */
    uint32_t duration_ms;        /* Duration since start */
} hb_recv_stats_t;

/* Forward declaration */
typedef struct hb_receiver hb_receiver_t;

/* ==================== Lifecycle ==================== */

/**
 * @brief Create receiver instance
 * 
 * @param cfg Configuration (must not be NULL)
 * @return Receiver instance or NULL on failure
 */
hb_receiver_t *hb_receiver_create(const hb_recv_config_t *cfg);

/**
 * @brief Destroy receiver instance
 * 
 * Stops receiving, frees all resources, calls callback for pending buffers (status=STOP)
 * 
 * @param r Receiver instance
 */
void hb_receiver_destroy(hb_receiver_t *r);

/* ==================== Transmission Control ==================== */

typedef enum {
    HB_RECV_STATE_IDLE = 0,      /* Receiver handle is NULL or destroying */
    HB_RECV_STATE_START,         /* Sending START and waiting for first frame */
    HB_RECV_STATE_RECEIVING,     /* Receiving fragments */
    HB_RECV_STATE_STOP,          /* Stopped */
} hb_receiver_state_t;

/**
 * @brief Start receiving
 * 
 * Sends START command periodically until first fragment received.
 * 
 * @param r Receiver instance
 * @return 0 on success, -1 on failure
 */
int hb_receiver_start(hb_receiver_t *r);

/**
 * @brief Stop receiving
 * 
 * Sends STOP command, calls callback for all pending buffers.
 * 
 * @param r Receiver instance
 * @return 0 on success, -1 on failure
 */
int hb_receiver_stop(hb_receiver_t *r);

/**
 * @brief Get receiver state
 *
 * @param r Receiver instance
 * @return Current state (NULL handle returns HB_RECV_STATE_IDLE)
 */
hb_receiver_state_t hb_receiver_get_state(hb_receiver_t *r);

/* ==================== Buffer Management ==================== */

/**
 * @brief Push buffer descriptor
 * 
 * Submits buffer for receiving. Internal hb_recv_frame_t will be created.
 * When new frame arrives, buffer will be taken from queue.
 * 
 * @param r Receiver instance
 * @param desc Buffer descriptor (will be copied)
 * @return 0 on success, -1 if queue full or invalid params
 */
int hb_recv_buf_push(hb_receiver_t *r, const hb_recv_buf_desc_t *desc);

/**
 * @brief Get available buffer slots
 * 
 * @param r Receiver instance
 * @return Available slots count
 */
uint16_t hb_recv_buf_available(hb_receiver_t *r);

/* ==================== Statistics ==================== */

/**
 * @brief Get statistics
 * 
 * @param r Receiver instance
 * @param stats Output statistics
 * @return 0 on success, -1 on invalid params
 */
int hb_receiver_get_statistics(hb_receiver_t *r, hb_recv_stats_t *stats);

/**
 * @brief Clear statistics
 * 
 * @param r Receiver instance
 * @return 0 on success, -1 on invalid params
 */
int hb_receiver_clear_statistics(hb_receiver_t *r);

#ifdef __cplusplus
}
#endif

#endif /* HB_RECEIVER_H */
