#ifndef WEBRTC_PEER_H
#define WEBRTC_PEER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Configuration layout changed from five to six entries. Rebuild callers
 * together with the library; do not mix objects built from older headers. */
#define WEBRTC_PEER_MAX_ICE_SERVERS 6

typedef struct webrtc_peer webrtc_peer_t;

typedef struct {
    uint32_t ssrc, timestamp;
    uint16_t sequence;
    uint8_t payload_type;
} webrtc_peer_audio_packet_info_t;
/* Owner-thread accessor, valid only within the synchronous on_audio callback.
 * Available without consumer feature macros. A library built without
 * CONFIG_LIBPEER_RTP_PACKET_INFO returns ERR_UNSUPPORTED without output writes.
 * Existing config and stats layouts are unchanged. */
int webrtc_peer_get_audio_packet_info(const webrtc_peer_t *peer,
                                     webrtc_peer_audio_packet_info_t *info, size_t size);

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

/* Allows consumers to build against SDK releases before this query existed. */
#define WEBRTC_PEER_HAS_SELECTED_ICE_TYPES 1
typedef enum {
    WEBRTC_PEER_ICE_HOST = 0,
    WEBRTC_PEER_ICE_SERVER_REFLEXIVE,
    WEBRTC_PEER_ICE_PEER_REFLEXIVE,
    WEBRTC_PEER_ICE_RELAY,
} webrtc_peer_ice_candidate_type_t;

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

/* Debug-only, post-SRTP observations. Poll from the peer owner. Loss/expected
 * reset at a confirmed sequence restart; duplicates are tracked over 64 packets.
 * remote_* describes the opposite direction, reported by the remote receiver. */
typedef struct {
    uint32_t ssrc, clock_rate, received_packets, expected_packets;
    int32_t packets_lost;
    uint32_t duplicates_recent, reordered_packets, untracked_old_packets;
    uint32_t sequence_rejects, sequence_restarts;
    /* Converted jitter values saturate at UINT32_MAX microseconds. */
    uint32_t jitter_us, max_arrival_gap_ms;
    uint32_t timestamp_gaps, timestamp_gap_samples;
    uint32_t sender_reports, sender_packets, sender_octets;
    uint32_t remote_reports;
    int32_t remote_packets_lost;
    uint32_t remote_fraction_lost_256, remote_jitter_us;
} webrtc_peer_rtp_diagnostics_t;

/* video=0: audio; video=1: RTP video (not DataChannel MJPEG).
 * The type and function are available regardless of consumer build macros.
 * If the library was built without CONFIG_LIBPEER_MEDIA_DIAGNOSTICS, returns
 * WEBRTC_PEER_ERR_UNSUPPORTED without inspecting arguments or writing stats. */
int webrtc_peer_get_rtp_diagnostics(const webrtc_peer_t *peer, int video,
                                   webrtc_peer_rtp_diagnostics_t *stats, size_t size);

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
typedef void (*webrtc_peer_data_channel_open_cb_t)(
    uint16_t sid, const char *label, void *user_data);

typedef enum {
    WEBRTC_PEER_ICE_TRANSPORT_ALL = 0,
    WEBRTC_PEER_ICE_TRANSPORT_RELAY = 1
} webrtc_peer_ice_transport_policy_t;

/* Rebuild callers and library together after this configuration extension. */
typedef struct {
    webrtc_peer_ice_server_t ice_servers[WEBRTC_PEER_MAX_ICE_SERVERS];
    webrtc_peer_codec_t audio_codec;
    webrtc_peer_codec_t video_codec;
    webrtc_peer_data_channel_type_t data_channel_type;
    webrtc_peer_ice_transport_policy_t ice_transport_policy;
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
/* Send text and binary on different open channels of one association.
 * type must be STRING or BINARY; queued data retains its type on retry.
 * Existing untyped APIs continue to use config.data_channel_type. */
int webrtc_peer_send_data_on_channel_typed(webrtc_peer_t *peer,
                                         const uint8_t *data, size_t size,
                                         uint16_t sid, webrtc_peer_data_channel_type_t type);
int webrtc_peer_create_data_channel(
    webrtc_peer_t *peer,
    const webrtc_peer_data_channel_config_t *config);
int webrtc_peer_lookup_data_channel(
    webrtc_peer_t *peer, const char *label, uint16_t *sid);
int webrtc_peer_is_data_channel_open(
    const webrtc_peer_t *peer, uint16_t sid);
/* Owner-thread, after association opens: DTLS client even / server odd. */
int webrtc_peer_get_local_data_channel_sid(webrtc_peer_t *peer, uint16_t *sid);
/* True only for an open unordered channel with maxRetransmits=0. */
int webrtc_peer_is_unreliable_unordered(const webrtc_peer_t *peer, uint16_t sid);
/* Opt-in regular ICE nomination with a bounded wait for higher-priority direct
 * paths. Call after new_connection and before applying the remote description.
 * Existing gateway users retain their current policy unless they opt in. */
int webrtc_peer_set_ice_direct_preference(webrtc_peer_t *peer, uint32_t wait_ms);
/* Built-in SCTP local/remote channel OPEN/ACK completion; distinct from association
 * open. Borrowed label is valid during callback; defer destroy/disconnect. */
int webrtc_peer_set_data_channel_open_callback(
    webrtc_peer_t *peer, webrtc_peer_data_channel_open_cb_t callback);
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
/* Owner-thread snapshot; returns INVALID_STATE before ICE selects a pair. */
int webrtc_peer_get_selected_ice_types(const webrtc_peer_t *peer,
    webrtc_peer_ice_candidate_type_t *local, webrtc_peer_ice_candidate_type_t *remote);
const char *webrtc_peer_state_to_string(webrtc_peer_state_t state);
void webrtc_peer_close(webrtc_peer_t *peer);

#ifdef __cplusplus
}
#endif

#endif
