#ifndef WEBRTC_PEER_MJPEG_H
#define WEBRTC_PEER_MJPEG_H

#include <stddef.h>
#include <stdint.h>

#include "webrtc_peer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WEBRTC_PEER_MJPEG_CHUNK_HEADER_SIZE 16U
#define WEBRTC_PEER_MJPEG_PARITY_HEADER_SIZE 16U
#define WEBRTC_PEER_MJPEG_LEGACY_HEADER_SIZE 8U

typedef int (*webrtc_peer_mjpeg_send_cb_t)(
    const uint8_t *data, size_t size, uint16_t sid, void *user_data);
typedef void (*webrtc_peer_mjpeg_frame_cb_t)(
    const uint8_t *jpeg, size_t size, uint32_t frame_id, void *user_data);

typedef struct {
    uint16_t sid;
    size_t chunk_payload_size;
    size_t max_frame_size;
    /* Zero sends without a frame or chunk pacing deadline. */
    unsigned int target_fps;
    /* Zero preserves realtime drop-on-pressure. Nonzero retries each blocked
     * data chunk this many times; retry_timeout_ms=0 uses 20 ms. */
    unsigned int max_chunk_retries;
    uint32_t retry_timeout_ms;
    int enable_xor_parity;
    uint8_t *chunk_buffer;
    size_t chunk_buffer_size;
    webrtc_peer_mjpeg_send_cb_t send;
    void *user_data;
} webrtc_peer_mjpeg_sender_config_t;

/*
 * The sender borrows the JPEG passed to sender_submit(). The caller must keep
 * it unchanged until sender_is_busy() becomes false or sender_reset() is
 * called. The chunk buffer is caller-owned and may be reused after each send
 * callback returns.
 */

typedef struct {
    uint32_t frames_submitted;
    uint32_t frames_sent;
    uint32_t frames_dropped;
    uint32_t invalid_frames;
    uint32_t chunks_sent;
    uint32_t parity_sent;
    uint32_t parity_send_failures;
    uint32_t send_failures;
    uint32_t send_retries;
    uint64_t frame_bytes_sent;
} webrtc_peer_mjpeg_sender_stats_t;

typedef struct {
    webrtc_peer_mjpeg_sender_config_t config;
    const uint8_t *frame;
    size_t frame_size;
    size_t frame_offset;
    uint32_t frame_id;
    uint32_t next_frame_id;
    uint32_t next_send_ms;
    uint32_t chunk_interval_ms;
    uint32_t retry_started_ms;
    unsigned int chunk_retries;
    int parity_pending;
    int active;
    int last_send_error;
    webrtc_peer_mjpeg_sender_stats_t stats;
} webrtc_peer_mjpeg_sender_t;

typedef enum {
    WEBRTC_PEER_MJPEG_PROCESS_IDLE = 0,
    WEBRTC_PEER_MJPEG_PROCESS_CHUNK_SENT = 1,
    WEBRTC_PEER_MJPEG_PROCESS_FRAME_SENT = 2,
    WEBRTC_PEER_MJPEG_PROCESS_FRAME_DROPPED = 3,
} webrtc_peer_mjpeg_process_result_t;

int webrtc_peer_mjpeg_sender_init(
    webrtc_peer_mjpeg_sender_t *sender,
    const webrtc_peer_mjpeg_sender_config_t *config);
int webrtc_peer_mjpeg_sender_set_target_fps(
    webrtc_peer_mjpeg_sender_t *sender, unsigned int target_fps);
int webrtc_peer_mjpeg_sender_submit(
    webrtc_peer_mjpeg_sender_t *sender,
    const uint8_t *jpeg, size_t size, uint32_t now_ms);
webrtc_peer_mjpeg_process_result_t webrtc_peer_mjpeg_sender_process(
    webrtc_peer_mjpeg_sender_t *sender, uint32_t now_ms);
int webrtc_peer_mjpeg_sender_is_busy(
    const webrtc_peer_mjpeg_sender_t *sender);
void webrtc_peer_mjpeg_sender_reset(webrtc_peer_mjpeg_sender_t *sender);
int webrtc_peer_mjpeg_sender_get_stats(
    const webrtc_peer_mjpeg_sender_t *sender,
    webrtc_peer_mjpeg_sender_stats_t *stats);

typedef struct {
    size_t max_frame_size;
    uint8_t *frame_buffer;
    size_t frame_buffer_size;
    webrtc_peer_mjpeg_frame_cb_t on_frame;
    void *user_data;
} webrtc_peer_mjpeg_receiver_config_t;

typedef struct {
    uint32_t frames_received;
    uint32_t frames_dropped;
    uint32_t invalid_messages;
    uint32_t chunks_received;
    uint64_t frame_bytes_received;
} webrtc_peer_mjpeg_receiver_stats_t;

typedef struct {
    webrtc_peer_mjpeg_receiver_config_t config;
    size_t frame_size;
    size_t next_offset;
    uint32_t frame_id;
    int active;
    webrtc_peer_mjpeg_receiver_stats_t stats;
} webrtc_peer_mjpeg_receiver_t;

typedef enum {
    WEBRTC_PEER_MJPEG_RECEIVE_CHUNK = 0,
    WEBRTC_PEER_MJPEG_RECEIVE_FRAME = 1,
} webrtc_peer_mjpeg_receive_result_t;

int webrtc_peer_mjpeg_receiver_init(
    webrtc_peer_mjpeg_receiver_t *receiver,
    const webrtc_peer_mjpeg_receiver_config_t *config);
int webrtc_peer_mjpeg_receiver_consume(
    webrtc_peer_mjpeg_receiver_t *receiver,
    const uint8_t *data, size_t size);
void webrtc_peer_mjpeg_receiver_reset(
    webrtc_peer_mjpeg_receiver_t *receiver);
int webrtc_peer_mjpeg_receiver_get_stats(
    const webrtc_peer_mjpeg_receiver_t *receiver,
    webrtc_peer_mjpeg_receiver_stats_t *stats);

#ifdef __cplusplus
}
#endif

#endif
