#ifndef WEBRTC_PEER_H
#define WEBRTC_PEER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WEBRTC_PEER_MAX_ICE_SERVERS 5

typedef struct webrtc_peer webrtc_peer_t;

typedef enum {
    WEBRTC_PEER_OK = 0,
    WEBRTC_PEER_ERR_INVALID_ARG = -1,
    WEBRTC_PEER_ERR_NO_MEMORY = -2,
    WEBRTC_PEER_ERR_INVALID_STATE = -3,
    WEBRTC_PEER_ERR_LIBPEER = -4,
    WEBRTC_PEER_ERR_WOULD_BLOCK = -5,
    WEBRTC_PEER_ERR_NOT_FOUND = -6,
    WEBRTC_PEER_ERR_UNSUPPORTED = -7,
    WEBRTC_PEER_ERR_SIGNALING = -8,
} webrtc_peer_err_t;

typedef enum {
    WEBRTC_PEER_STATE_CLOSED = 0,
    WEBRTC_PEER_STATE_NEW,
    WEBRTC_PEER_STATE_CHECKING,
    WEBRTC_PEER_STATE_CONNECTED,
    WEBRTC_PEER_STATE_COMPLETED,
    WEBRTC_PEER_STATE_FAILED,
    WEBRTC_PEER_STATE_DISCONNECTED,
} webrtc_peer_state_t;

typedef enum {
    WEBRTC_PEER_ROLE_OFFERER = 0,
    WEBRTC_PEER_ROLE_ANSWERER,
} webrtc_peer_role_t;

typedef enum {
    WEBRTC_PEER_MSG_SDP_OFFER = 0,
    WEBRTC_PEER_MSG_SDP_ANSWER,
    WEBRTC_PEER_MSG_ICE_CANDIDATE,
    WEBRTC_PEER_MSG_BYE,
} webrtc_peer_message_type_t;

typedef struct {
    webrtc_peer_message_type_t type;
    const char *payload;
} webrtc_peer_message_t;

typedef enum {
    WEBRTC_PEER_CODEC_NONE = 0,
    WEBRTC_PEER_CODEC_H264,
    WEBRTC_PEER_CODEC_VP8,
    WEBRTC_PEER_CODEC_MJPEG,
    WEBRTC_PEER_CODEC_OPUS,
    WEBRTC_PEER_CODEC_PCMA,
    WEBRTC_PEER_CODEC_PCMU,
} webrtc_peer_codec_t;

typedef enum {
    WEBRTC_PEER_DATA_CHANNEL_NONE = 0,
    WEBRTC_PEER_DATA_CHANNEL_STRING,
    WEBRTC_PEER_DATA_CHANNEL_BINARY,
} webrtc_peer_data_channel_type_t;

typedef enum {
    WEBRTC_PEER_DATA_RELIABLE = 0x00,
    WEBRTC_PEER_DATA_RELIABLE_UNORDERED = 0x80,
    WEBRTC_PEER_DATA_PARTIAL_REXMIT = 0x01,
    WEBRTC_PEER_DATA_PARTIAL_REXMIT_UNORDERED = 0x81,
    WEBRTC_PEER_DATA_PARTIAL_TIMED = 0x02,
    WEBRTC_PEER_DATA_PARTIAL_TIMED_UNORDERED = 0x82,
} webrtc_peer_data_reliability_t;

typedef struct {
    const char *url;
    const char *username;
    const char *credential;
} webrtc_peer_ice_server_t;

typedef struct {
    uint16_t sid;
    webrtc_peer_data_reliability_t reliability;
    uint16_t priority;
    uint32_t reliability_parameter;
    const char *label;
    const char *protocol;
} webrtc_peer_data_channel_config_t;

typedef struct {
    uint32_t connection_attempts;
    uint32_t loop_calls;
    uint32_t state_changes;
    uint32_t signaling_tx_messages;
    uint32_t signaling_rx_messages;
    uint32_t audio_tx_frames;
    uint32_t audio_rx_frames;
    uint32_t video_tx_frames;
    uint32_t video_rx_frames;
    uint32_t data_tx_messages;
    uint32_t data_rx_messages;
    uint32_t send_failures;
    uint32_t data_buffered_bytes;
    uint32_t data_buffer_capacity;
    uint32_t transport_send_failures;
    uint32_t sctp_gap_reports;
    uint32_t sctp_forward_tsn_sent;
    uint8_t sctp_peer_supports_forward_tsn;
} webrtc_peer_stats_t;

/* The original stats layout and get_stats() ABI are frozen. Future counters
 * are appended here and queried with an explicit caller buffer size. */
typedef struct {
    webrtc_peer_stats_t base;
    /* Reliable DATA retransmission attempts, excluding handshake/control
     * retries; counters survive association failure until the next session.
     * The optional usrsctp backend currently reports zero. */
    uint32_t data_retransmissions;
    uint32_t data_retry_exhaustions;
} webrtc_peer_stats_ex_t;

typedef int (*webrtc_peer_local_message_cb_t)(
    const webrtc_peer_message_t *message, void *user_data);
typedef void (*webrtc_peer_state_cb_t)(
    webrtc_peer_state_t state, void *user_data);
typedef void (*webrtc_peer_media_cb_t)(
    const uint8_t *data, size_t size, void *user_data);
typedef void (*webrtc_peer_keyframe_cb_t)(void *user_data);
typedef void (*webrtc_peer_data_message_cb_t)(
    const uint8_t *data, size_t size, uint16_t sid, void *user_data);
typedef void (*webrtc_peer_event_cb_t)(void *user_data);

typedef struct {
    webrtc_peer_ice_server_t ice_servers[WEBRTC_PEER_MAX_ICE_SERVERS];
    webrtc_peer_codec_t audio_codec;
    webrtc_peer_codec_t video_codec;
    webrtc_peer_data_channel_type_t data_channel_type;
    webrtc_peer_local_message_cb_t on_local_message;
    webrtc_peer_state_cb_t on_state;
    webrtc_peer_media_cb_t on_audio;
    webrtc_peer_media_cb_t on_video;
    webrtc_peer_keyframe_cb_t on_request_keyframe;
    webrtc_peer_data_message_cb_t on_data;
    webrtc_peer_event_cb_t on_data_association_open;
    webrtc_peer_event_cb_t on_data_association_close;
    void *user_data;
} webrtc_peer_config_t;

int webrtc_peer_open(const webrtc_peer_config_t *config, webrtc_peer_t **peer);
int webrtc_peer_new_connection(webrtc_peer_t *peer, webrtc_peer_role_t role);
int webrtc_peer_send_msg(webrtc_peer_t *peer,
                       const webrtc_peer_message_t *message);
int webrtc_peer_send_audio(webrtc_peer_t *peer,
                         const uint8_t *data, size_t size);
int webrtc_peer_send_video(webrtc_peer_t *peer,
                         const uint8_t *data, size_t size);
int webrtc_peer_send_data(webrtc_peer_t *peer,
                        const uint8_t *data, size_t size);
int webrtc_peer_send_data_on_channel(webrtc_peer_t *peer,
                                   const uint8_t *data, size_t size,
                                   uint16_t sid);
int webrtc_peer_create_data_channel(
    webrtc_peer_t *peer,
    const webrtc_peer_data_channel_config_t *config);
int webrtc_peer_lookup_data_channel(
    webrtc_peer_t *peer, const char *label, uint16_t *sid);
int webrtc_peer_is_data_channel_open(
    const webrtc_peer_t *peer, uint16_t sid);
int webrtc_peer_main_loop(webrtc_peer_t *peer);
int webrtc_peer_disconnect(webrtc_peer_t *peer);
int webrtc_peer_get_stats(
    const webrtc_peer_t *peer, webrtc_peer_stats_t *stats);
/* Writes at most min(stats_size, sizeof(*stats)), accepting the original
 * base prefix or a larger future buffer. Initialize the buffer to zero before
 * calling so counters unknown to an older library remain zero. */
int webrtc_peer_get_stats_ex(
    const webrtc_peer_t *peer, webrtc_peer_stats_ex_t *stats, size_t stats_size);
webrtc_peer_state_t webrtc_peer_get_state(const webrtc_peer_t *peer);
const char *webrtc_peer_state_to_string(webrtc_peer_state_t state);
void webrtc_peer_close(webrtc_peer_t *peer);

#ifdef __cplusplus
}
#endif

#endif
