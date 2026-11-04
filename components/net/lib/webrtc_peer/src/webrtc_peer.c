#include "webrtc_peer.h"

#include <stdlib.h>
#include <string.h>

#include "peer.h"
#include "peer_connection.h"
#include "ice.h"
#include "sctp.h"

#if WEBRTC_PEER_MAX_ICE_SERVERS > PEER_CONNECTION_MAX_ICE_SERVERS
#error "The native PeerConfiguration cannot hold the facade ICE server list"
#endif

struct webrtc_peer {
    webrtc_peer_config_t config;
    PeerConnection *connection;
    webrtc_peer_role_t role;
    uint16_t active_sid;
    int active_sid_known;
    int data_association_open;
    int callback_error;
    webrtc_peer_data_channel_open_cb_t on_channel_open;
    webrtc_peer_stats_t stats;
};

static unsigned int g_peer_library_references;

int webrtc_peer_get_audio_packet_info(const webrtc_peer_t *peer,
                                     webrtc_peer_audio_packet_info_t *info, size_t size)
{
#if CONFIG_LIBPEER_RTP_PACKET_INFO
    PeerRtpPacketInfo native;
    if (!peer || !info || size < sizeof(*info)) return WEBRTC_PEER_ERR_INVALID_ARG;
    if (!peer->connection || peer_connection_get_audio_packet_info(peer->connection, &native))
        return WEBRTC_PEER_ERR_INVALID_STATE;
    *info = (webrtc_peer_audio_packet_info_t){.ssrc = native.ssrc,
        .timestamp = native.timestamp, .sequence = native.sequence,
        .payload_type = native.payload_type};
    return WEBRTC_PEER_OK;
#else
    (void)peer; (void)info; (void)size;
    return WEBRTC_PEER_ERR_UNSUPPORTED;
#endif
}

static char *duplicate_string(const char *value)
{
    size_t length;
    char *copy;

    if (!value) {
        return NULL;
    }
    length = strlen(value);
    copy = malloc(length + 1);
    if (copy) {
        memcpy(copy, value, length + 1);
    }
    return copy;
}

static void free_ice_servers(webrtc_peer_t *peer)
{
    size_t i;

    for (i = 0; i < WEBRTC_PEER_MAX_ICE_SERVERS; ++i) {
        free((char *)peer->config.ice_servers[i].url);
        free((char *)peer->config.ice_servers[i].username);
        free((char *)peer->config.ice_servers[i].credential);
        memset(&peer->config.ice_servers[i], 0,
               sizeof(peer->config.ice_servers[i]));
    }
}

static int copy_ice_servers(webrtc_peer_t *peer,
                            const webrtc_peer_config_t *config)
{
    size_t i;

    for (i = 0; i < WEBRTC_PEER_MAX_ICE_SERVERS; ++i) {
        const webrtc_peer_ice_server_t *source = &config->ice_servers[i];
        webrtc_peer_ice_server_t *target = &peer->config.ice_servers[i];

        target->url = duplicate_string(source->url);
        target->username = duplicate_string(source->username);
        target->credential = duplicate_string(source->credential);
        if ((source->url && !target->url) ||
            (source->username && !target->username) ||
            (source->credential && !target->credential)) {
            free_ice_servers(peer);
            return WEBRTC_PEER_ERR_NO_MEMORY;
        }
    }
    return WEBRTC_PEER_OK;
}

static int library_acquire(void)
{
    if (g_peer_library_references == 0 && peer_init() != 0) {
        return WEBRTC_PEER_ERR_LIBPEER;
    }
    g_peer_library_references++;
    return WEBRTC_PEER_OK;
}

static void library_release(void)
{
    if (g_peer_library_references == 0) {
        return;
    }
    g_peer_library_references--;
    if (g_peer_library_references == 0) {
        peer_deinit();
    }
}

static webrtc_peer_state_t map_state(PeerConnectionState state)
{
    switch (state) {
        case PEER_CONNECTION_CLOSED:
            return WEBRTC_PEER_STATE_CLOSED;
        case PEER_CONNECTION_NEW:
            return WEBRTC_PEER_STATE_NEW;
        case PEER_CONNECTION_CHECKING:
            return WEBRTC_PEER_STATE_CHECKING;
        case PEER_CONNECTION_CONNECTED:
            return WEBRTC_PEER_STATE_CONNECTED;
        case PEER_CONNECTION_COMPLETED:
            return WEBRTC_PEER_STATE_COMPLETED;
        case PEER_CONNECTION_FAILED:
            return WEBRTC_PEER_STATE_FAILED;
        case PEER_CONNECTION_DISCONNECTED:
            return WEBRTC_PEER_STATE_DISCONNECTED;
        default:
            return WEBRTC_PEER_STATE_FAILED;
    }
}

static MediaCodec map_codec(webrtc_peer_codec_t codec)
{
    switch (codec) {
        case WEBRTC_PEER_CODEC_H264:
            return CODEC_H264;
        case WEBRTC_PEER_CODEC_VP8:
            return CODEC_VP8;
        case WEBRTC_PEER_CODEC_MJPEG:
            return CODEC_MJPEG;
        case WEBRTC_PEER_CODEC_OPUS:
            return CODEC_OPUS;
        case WEBRTC_PEER_CODEC_PCMA:
            return CODEC_PCMA;
        case WEBRTC_PEER_CODEC_PCMU:
            return CODEC_PCMU;
        case WEBRTC_PEER_CODEC_NONE:
        default:
            return CODEC_NONE;
    }
}

static int valid_data_reliability(webrtc_peer_data_reliability_t reliability)
{
    switch (reliability) {
        case WEBRTC_PEER_DATA_RELIABLE:
        case WEBRTC_PEER_DATA_RELIABLE_UNORDERED:
        case WEBRTC_PEER_DATA_PARTIAL_REXMIT:
        case WEBRTC_PEER_DATA_PARTIAL_REXMIT_UNORDERED:
        case WEBRTC_PEER_DATA_PARTIAL_TIMED:
        case WEBRTC_PEER_DATA_PARTIAL_TIMED_UNORDERED:
            return 1;
        default:
            return 0;
    }
}

static void on_native_state(PeerConnectionState state, void *user_data)
{
    webrtc_peer_t *peer = user_data;

    if (!peer) {
        return;
    }
    peer->stats.state_changes++;
    if (peer->config.on_state) {
        peer->config.on_state(map_state(state), peer->config.user_data);
    }
}

static void on_native_local_sdp(char *sdp, void *user_data)
{
    webrtc_peer_t *peer = user_data;
    webrtc_peer_message_t message;

    if (!peer || !sdp || !peer->config.on_local_message) {
        return;
    }
    message.type = peer->role == WEBRTC_PEER_ROLE_OFFERER ?
        WEBRTC_PEER_MSG_SDP_OFFER : WEBRTC_PEER_MSG_SDP_ANSWER;
    message.payload = sdp;
    peer->callback_error =
        peer->config.on_local_message(&message, peer->config.user_data);
    if (peer->callback_error == 0) {
        peer->stats.signaling_tx_messages++;
    }
}

static void on_native_audio(uint8_t *data, size_t size, void *user_data)
{
    webrtc_peer_t *peer = user_data;

    if (!peer) {
        return;
    }
    peer->stats.audio_rx_frames++;
    if (peer->config.on_audio) {
        peer->config.on_audio(data, size, peer->config.user_data);
    }
}

static void on_native_video(uint8_t *data, size_t size, void *user_data)
{
    webrtc_peer_t *peer = user_data;

    if (!peer) {
        return;
    }
    peer->stats.video_rx_frames++;
    if (peer->config.on_video) {
        peer->config.on_video(data, size, peer->config.user_data);
    }
}

static void on_native_keyframe(void *user_data)
{
    webrtc_peer_t *peer = user_data;

    if (peer && peer->config.on_request_keyframe) {
        peer->config.on_request_keyframe(peer->config.user_data);
    }
}

static void on_native_data(char *data, size_t size, void *user_data,
                           uint16_t sid)
{
    webrtc_peer_t *peer = user_data;

    if (!peer) {
        return;
    }
    peer->active_sid = sid;
    peer->active_sid_known = 1;
    peer->stats.data_rx_messages++;
    if (peer->config.on_data) {
        peer->config.on_data((const uint8_t *)data, size, sid,
                             peer->config.user_data);
    }
}

static void on_native_data_open(void *user_data)
{
    webrtc_peer_t *peer = user_data;

    if (!peer) {
        return;
    }
    peer->data_association_open = 1;
    if (peer->config.on_data_association_open) {
        peer->config.on_data_association_open(peer->config.user_data);
    }
}

static void on_native_data_close(void *user_data)
{
    webrtc_peer_t *peer = user_data;

    if (!peer) {
        return;
    }
    peer->data_association_open = 0;
    peer->active_sid_known = 0;
    if (peer->config.on_data_association_close) {
        peer->config.on_data_association_close(peer->config.user_data);
    }
}

static void on_native_channel_open(uint16_t sid, const char *label, void *user_data)
{
    webrtc_peer_t *peer = user_data;
    if (peer && peer->on_channel_open) {
        peer->on_channel_open(sid, label, peer->config.user_data);
    }
}

int webrtc_peer_set_data_channel_open_callback(
    webrtc_peer_t *peer, webrtc_peer_data_channel_open_cb_t callback)
{
    if (!peer) return WEBRTC_PEER_ERR_INVALID_ARG;
    peer->on_channel_open = callback;
    return WEBRTC_PEER_OK;
}

static int map_send_result(webrtc_peer_t *peer, int result,
                           uint32_t *success_counter)
{
    if (result < 0) {
        peer->stats.send_failures++;
        return WEBRTC_PEER_ERR_WOULD_BLOCK;
    }
    (*success_counter)++;
    return WEBRTC_PEER_OK;
}

int webrtc_peer_open(const webrtc_peer_config_t *config, webrtc_peer_t **out_peer)
{
    webrtc_peer_t *peer;
    int result;

    if (!config || !out_peer || *out_peer) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    if (!config->on_local_message ||
        (config->audio_codec != WEBRTC_PEER_CODEC_NONE &&
         config->audio_codec != WEBRTC_PEER_CODEC_OPUS &&
         config->audio_codec != WEBRTC_PEER_CODEC_PCMA &&
         config->audio_codec != WEBRTC_PEER_CODEC_PCMU) ||
        (config->video_codec != WEBRTC_PEER_CODEC_NONE &&
         config->video_codec != WEBRTC_PEER_CODEC_H264 &&
         config->video_codec != WEBRTC_PEER_CODEC_VP8 &&
         config->video_codec != WEBRTC_PEER_CODEC_MJPEG) ||
        config->data_channel_type < WEBRTC_PEER_DATA_CHANNEL_NONE ||
        config->data_channel_type > WEBRTC_PEER_DATA_CHANNEL_BINARY ||
        config->ice_transport_policy < WEBRTC_PEER_ICE_TRANSPORT_ALL ||
        config->ice_transport_policy > WEBRTC_PEER_ICE_TRANSPORT_RELAY) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    result = library_acquire();
    if (result != WEBRTC_PEER_OK) {
        return result;
    }

    peer = calloc(1, sizeof(*peer));
    if (!peer) {
        library_release();
        return WEBRTC_PEER_ERR_NO_MEMORY;
    }
    peer->config = *config;
    /* Only owned copies may be freed if a later allocation fails. */
    memset(peer->config.ice_servers, 0, sizeof(peer->config.ice_servers));
    result = copy_ice_servers(peer, config);
    if (result != WEBRTC_PEER_OK) {
        free(peer);
        library_release();
        return result;
    }
    *out_peer = peer;
    return WEBRTC_PEER_OK;
}

int webrtc_peer_new_connection(webrtc_peer_t *peer, webrtc_peer_role_t role)
{
    PeerConfiguration config;
    const char *offer;
    size_t i;

    if (!peer || role < WEBRTC_PEER_ROLE_OFFERER ||
        role > WEBRTC_PEER_ROLE_ANSWERER) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    webrtc_peer_disconnect(peer);
    memset(&config, 0, sizeof(config));
    for (i = 0; i < WEBRTC_PEER_MAX_ICE_SERVERS; ++i) {
        config.ice_servers[i].urls = peer->config.ice_servers[i].url;
        config.ice_servers[i].username = peer->config.ice_servers[i].username;
        config.ice_servers[i].credential = peer->config.ice_servers[i].credential;
    }
    config.audio_codec = map_codec(peer->config.audio_codec);
    config.video_codec = map_codec(peer->config.video_codec);
    config.datachannel = (DataChannelType)peer->config.data_channel_type;
    config.ice_transport_policy = (IceTransportPolicy)peer->config.ice_transport_policy;
    config.onaudiotrack = on_native_audio;
    config.onvideotrack = on_native_video;
    config.on_request_keyframe = on_native_keyframe;
    config.user_data = peer;

    peer->role = role;
    peer->callback_error = 0;
    peer->connection = peer_connection_create(&config);
    if (!peer->connection) {
        return WEBRTC_PEER_ERR_LIBPEER;
    }
    peer_connection_onicecandidate(peer->connection, on_native_local_sdp);
    peer_connection_oniceconnectionstatechange(peer->connection,
                                               on_native_state);
    peer_connection_ondatachannel(peer->connection, on_native_data,
                                  on_native_data_open,
                                  on_native_data_close);
    peer_connection_ondatachannel_open(peer->connection, on_native_channel_open);
    on_native_state(PEER_CONNECTION_NEW, peer);
    peer->stats.connection_attempts++;

    if (role == WEBRTC_PEER_ROLE_ANSWERER) {
        return WEBRTC_PEER_OK;
    }
    offer = peer_connection_create_offer(peer->connection);
    if (!offer) {
        webrtc_peer_disconnect(peer);
        return WEBRTC_PEER_ERR_LIBPEER;
    }
    if (peer->callback_error != 0) {
        webrtc_peer_disconnect(peer);
        return WEBRTC_PEER_ERR_SIGNALING;
    }
    return WEBRTC_PEER_OK;
}

int webrtc_peer_send_msg(webrtc_peer_t *peer,
                       const webrtc_peer_message_t *message)
{
    const char *answer;
    int result;

    if (!peer || !peer->connection || !message) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    if (message->type != WEBRTC_PEER_MSG_BYE && !message->payload) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    switch (message->type) {
        case WEBRTC_PEER_MSG_SDP_OFFER:
            if (peer->role != WEBRTC_PEER_ROLE_ANSWERER ||
                peer_connection_get_state(peer->connection) !=
                    PEER_CONNECTION_NEW) {
                return WEBRTC_PEER_ERR_INVALID_STATE;
            }
            if (peer_connection_set_remote_description_checked(
                    peer->connection, message->payload, SDP_TYPE_OFFER) != 0) {
                return WEBRTC_PEER_ERR_LIBPEER;
            }
            peer->callback_error = 0;
            answer = peer_connection_create_answer(peer->connection);
            if (!answer) {
                return WEBRTC_PEER_ERR_LIBPEER;
            }
            result = peer->callback_error == 0 ?
                WEBRTC_PEER_OK : WEBRTC_PEER_ERR_SIGNALING;
            break;
        case WEBRTC_PEER_MSG_SDP_ANSWER:
            if (peer->role != WEBRTC_PEER_ROLE_OFFERER ||
                peer_connection_get_state(peer->connection) !=
                    PEER_CONNECTION_NEW) {
                return WEBRTC_PEER_ERR_INVALID_STATE;
            }
            result = peer_connection_set_remote_description_checked(
                peer->connection, message->payload, SDP_TYPE_ANSWER) == 0 ?
                WEBRTC_PEER_OK : WEBRTC_PEER_ERR_LIBPEER;
            break;
        case WEBRTC_PEER_MSG_ICE_CANDIDATE:
            result = peer_connection_add_ice_candidate(
                       peer->connection, (char *)message->payload) == 0 ?
                WEBRTC_PEER_OK : WEBRTC_PEER_ERR_LIBPEER;
            break;
        case WEBRTC_PEER_MSG_BYE:
            result = webrtc_peer_disconnect(peer);
            break;
        default:
            return WEBRTC_PEER_ERR_UNSUPPORTED;
    }
    if (result == WEBRTC_PEER_OK) {
        peer->stats.signaling_rx_messages++;
    }
    return result;
}

int webrtc_peer_send_audio(webrtc_peer_t *peer,
                         const uint8_t *data, size_t size)
{
    if (!peer || !peer->connection || !data || size == 0) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    return map_send_result(
        peer, peer_connection_send_audio(peer->connection, data, size),
        &peer->stats.audio_tx_frames);
}

int webrtc_peer_send_video(webrtc_peer_t *peer,
                         const uint8_t *data, size_t size)
{
    if (!peer || !peer->connection || !data || size == 0) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    return map_send_result(
        peer, peer_connection_send_video(peer->connection, data, size),
        &peer->stats.video_tx_frames);
}

int webrtc_peer_send_data(webrtc_peer_t *peer,
                        const uint8_t *data, size_t size)
{
    if (!peer || !peer->active_sid_known) {
        return WEBRTC_PEER_ERR_INVALID_STATE;
    }
    return webrtc_peer_send_data_on_channel(peer, data, size,
                                          peer->active_sid);
}

int webrtc_peer_send_data_on_channel(webrtc_peer_t *peer,
                                   const uint8_t *data, size_t size,
                                   uint16_t sid)
{
    if (!peer) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    /* Preserve the legacy NONE -> binary fallback. */
    webrtc_peer_data_channel_type_t type =
        peer->config.data_channel_type == WEBRTC_PEER_DATA_CHANNEL_STRING ?
        WEBRTC_PEER_DATA_CHANNEL_STRING : WEBRTC_PEER_DATA_CHANNEL_BINARY;
    return webrtc_peer_send_data_on_channel_typed(peer, data, size, sid, type);
}

int webrtc_peer_send_data_on_channel_typed(webrtc_peer_t *peer,
                                         const uint8_t *data, size_t size,
                                         uint16_t sid, webrtc_peer_data_channel_type_t type)
{
    int result;
    if (!peer || !peer->connection || !data || size == 0) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    if (type != WEBRTC_PEER_DATA_CHANNEL_STRING && type != WEBRTC_PEER_DATA_CHANNEL_BINARY) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    if (!webrtc_peer_is_data_channel_open(peer, sid)) {
        peer->stats.send_failures++;
        return WEBRTC_PEER_ERR_INVALID_STATE;
    }
    result = peer_connection_datachannel_send_sid_typed(
        peer->connection, data, size, sid, (DataChannelType)type);
    if (result < -1 || (result >= 0 && (size_t)result != size)) {
        /* A partially sent message must never be offered again as a retry. */
        peer->stats.send_failures++;
        return WEBRTC_PEER_ERR_LIBPEER;
    }
    return map_send_result(peer, result, &peer->stats.data_tx_messages);
}

int webrtc_peer_get_local_data_channel_sid(webrtc_peer_t *peer, uint16_t *sid)
{
    if (!peer || !peer->connection || !sid) return WEBRTC_PEER_ERR_INVALID_ARG;
    if (!peer->data_association_open) return WEBRTC_PEER_ERR_INVALID_STATE;
    *sid = peer_connection_get_local_datachannel_sid(peer->connection);
    return WEBRTC_PEER_OK;
}

int webrtc_peer_set_ice_direct_preference(webrtc_peer_t *peer, uint32_t wait_ms)
{
    if (!peer || !peer->connection) return WEBRTC_PEER_ERR_INVALID_ARG;
    return peer_connection_set_ice_direct_preference(peer->connection, wait_ms) == 0 ?
        WEBRTC_PEER_OK : WEBRTC_PEER_ERR_INVALID_STATE;
}
int webrtc_peer_is_unreliable_unordered(const webrtc_peer_t *peer, uint16_t sid)
{
    return peer && peer_connection_is_unreliable_unordered(peer->connection, sid);
}

int webrtc_peer_create_data_channel(
    webrtc_peer_t *peer,
    const webrtc_peer_data_channel_config_t *config)
{
    int result;

    if (!peer || !peer->connection || !config || !config->label ||
        !config->protocol ||
        !valid_data_reliability(config->reliability)) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    if (!peer->data_association_open) {
        return WEBRTC_PEER_ERR_INVALID_STATE;
    }
    result = peer_connection_create_datachannel_sid(
        peer->connection, (DecpChannelType)config->reliability,
        config->priority, config->reliability_parameter,
        (char *)config->label, (char *)config->protocol, config->sid);
    if (result < 0) {
        return WEBRTC_PEER_ERR_LIBPEER;
    }
    peer->active_sid = config->sid;
    peer->active_sid_known = 1;
    return WEBRTC_PEER_OK;
}

int webrtc_peer_lookup_data_channel(
    webrtc_peer_t *peer, const char *label, uint16_t *sid)
{
    if (!peer || !peer->connection || !label || !sid) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    if (peer_connection_lookup_sid(peer->connection, label, sid) != 0) {
        return WEBRTC_PEER_ERR_NOT_FOUND;
    }
    peer->active_sid = *sid;
    peer->active_sid_known = 1;
    return WEBRTC_PEER_OK;
}

int webrtc_peer_is_data_channel_open(
    const webrtc_peer_t *peer, uint16_t sid)
{
    Sctp *sctp;

    if (!peer || !peer->connection || !peer->data_association_open) {
        return 0;
    }
    sctp = (Sctp *)peer_connection_get_sctp(peer->connection);
    return sctp && sctp_is_stream_open(sctp, sid);
}

int webrtc_peer_main_loop(webrtc_peer_t *peer)
{
    if (!peer || !peer->connection) {
        return WEBRTC_PEER_ERR_INVALID_STATE;
    }
    peer->stats.loop_calls++;
    return peer_connection_loop(peer->connection) == 0 ?
        WEBRTC_PEER_OK : WEBRTC_PEER_ERR_LIBPEER;
}

int webrtc_peer_disconnect(webrtc_peer_t *peer)
{
    if (!peer) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    if (peer->connection) {
        peer_connection_close(peer->connection);
        peer_connection_destroy(peer->connection);
        peer->connection = NULL;
    }
    peer->data_association_open = 0;
    peer->active_sid_known = 0;
    return WEBRTC_PEER_OK;
}

int webrtc_peer_get_stats(
    const webrtc_peer_t *peer, webrtc_peer_stats_t *stats)
{
    int buffered;
    int capacity;

    if (!peer || !stats) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    *stats = peer->stats;
    if (!peer->connection) {
        return WEBRTC_PEER_OK;
    }
    buffered = peer_connection_get_data_buffered_amount(peer->connection);
    capacity = peer_connection_get_data_buffer_capacity(peer->connection);
    stats->data_buffered_bytes = buffered > 0 ? (uint32_t)buffered : 0;
    stats->data_buffer_capacity = capacity > 0 ? (uint32_t)capacity : 0;
    stats->transport_send_failures =
        peer_connection_get_transport_send_failures(peer->connection);
    stats->sctp_gap_reports =
        peer_connection_get_sctp_gap_reports(peer->connection);
    stats->sctp_forward_tsn_sent =
        peer_connection_get_sctp_forward_tsn_sent(peer->connection);
    stats->sctp_peer_supports_forward_tsn = (uint8_t)
        peer_connection_sctp_peer_supports_forward_tsn(peer->connection);
    return WEBRTC_PEER_OK;
}

int webrtc_peer_get_stats_ex(
    const webrtc_peer_t *peer, webrtc_peer_stats_ex_t *stats, size_t stats_size)
{
    webrtc_peer_stats_ex_t snapshot = {0};
    int result;

    if (!stats || stats_size < sizeof(snapshot.base)) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    result = webrtc_peer_get_stats(peer, &snapshot.base);
    if (result != WEBRTC_PEER_OK) return result;
    if (peer->connection) {
        snapshot.data_retransmissions =
            peer_connection_get_data_retransmissions(peer->connection);
        snapshot.data_retry_exhaustions =
            peer_connection_get_data_retry_exhaustions(peer->connection);
    }
    if (stats_size > sizeof(snapshot)) stats_size = sizeof(snapshot);
    memcpy(stats, &snapshot, stats_size);
    return WEBRTC_PEER_OK;
}

#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
/* Keep the public report independent of native headers. Enumerate mapped
 * fields once; the byte sums also catch a field added to BOTH structs without
 * updating this mapping. Layout checks deliberately live at the facade. */
#define RTP_DIAGNOSTIC_FIELDS(F) \
    F(ssrc) \
    F(clock_rate) \
    F(received_packets) \
    F(expected_packets) \
    F(packets_lost) \
    F(duplicates_recent) \
    F(reordered_packets) \
    F(untracked_old_packets) \
    F(sequence_rejects) \
    F(sequence_restarts) \
    F(jitter_us) \
    F(max_arrival_gap_ms) \
    F(timestamp_gaps) \
    F(timestamp_gap_samples) \
    F(sender_reports) \
    F(sender_packets) \
    F(sender_octets) \
    F(remote_reports) \
    F(remote_packets_lost) \
    F(remote_fraction_lost_256) \
    F(remote_jitter_us)
#define NATIVE_FIELD_BYTES(field) + sizeof(((RtpDiagnosticReport *)0)->field)
#define PUBLIC_FIELD_BYTES(field) + sizeof(((webrtc_peer_rtp_diagnostics_t *)0)->field)
_Static_assert(sizeof(RtpDiagnosticReport) ==
               (0 RTP_DIAGNOSTIC_FIELDS(NATIVE_FIELD_BYTES)),
               "Update native RTP diagnostic field mapping");
_Static_assert(sizeof(webrtc_peer_rtp_diagnostics_t) ==
               (0 RTP_DIAGNOSTIC_FIELDS(PUBLIC_FIELD_BYTES)),
               "Update public RTP diagnostic field mapping");
#undef NATIVE_FIELD_BYTES
#undef PUBLIC_FIELD_BYTES
#endif

int webrtc_peer_get_rtp_diagnostics(const webrtc_peer_t *peer, int video,
                                   webrtc_peer_rtp_diagnostics_t *stats, size_t size)
{
#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
    RtpDiagnosticReport native;
    webrtc_peer_rtp_diagnostics_t snapshot = {0};
    if (!peer || !stats || size < sizeof(snapshot) || (video != 0 && video != 1))
        return WEBRTC_PEER_ERR_INVALID_ARG;
    if (!peer->connection) return WEBRTC_PEER_ERR_INVALID_STATE;
    if (peer_connection_get_rtp_diagnostics(peer->connection, video, &native) != 0)
        return WEBRTC_PEER_ERR_LIBPEER;
#define COPY_DIAG(field) do { \
    _Static_assert(offsetof(RtpDiagnosticReport, field) == \
                   offsetof(webrtc_peer_rtp_diagnostics_t, field), \
                   "RTP diagnostic field offset: " #field); \
    _Static_assert(sizeof(native.field) == sizeof(snapshot.field), \
                   "RTP diagnostic field width: " #field); \
    snapshot.field = native.field; \
} while (0);
    RTP_DIAGNOSTIC_FIELDS(COPY_DIAG)
#undef COPY_DIAG
    memcpy(stats, &snapshot, sizeof(snapshot));
    return WEBRTC_PEER_OK;
#else
    (void)peer;
    (void)video;
    (void)stats;
    (void)size;
    return WEBRTC_PEER_ERR_UNSUPPORTED;
#endif
}
#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
#undef RTP_DIAGNOSTIC_FIELDS
#endif

webrtc_peer_state_t webrtc_peer_get_state(const webrtc_peer_t *peer)
{
    if (!peer || !peer->connection) {
        return WEBRTC_PEER_STATE_CLOSED;
    }
    return map_state(peer_connection_get_state(peer->connection));
}

static int map_ice_candidate_type(int native, webrtc_peer_ice_candidate_type_t *out)
{
    switch (native) {
        case ICE_CANDIDATE_TYPE_HOST: *out = WEBRTC_PEER_ICE_HOST; break;
        case ICE_CANDIDATE_TYPE_SRFLX: *out = WEBRTC_PEER_ICE_SERVER_REFLEXIVE; break;
        case ICE_CANDIDATE_TYPE_PRFLX: *out = WEBRTC_PEER_ICE_PEER_REFLEXIVE; break;
        case ICE_CANDIDATE_TYPE_RELAY: *out = WEBRTC_PEER_ICE_RELAY; break;
        default: return -1;
    }
    return 0;
}

int webrtc_peer_get_selected_ice_types(const webrtc_peer_t *peer,
    webrtc_peer_ice_candidate_type_t *local, webrtc_peer_ice_candidate_type_t *remote)
{
    int a, b;
    webrtc_peer_ice_candidate_type_t mapped_local, mapped_remote;
    if (!peer || !local || !remote) return WEBRTC_PEER_ERR_INVALID_ARG;
    if (!peer->connection || peer_connection_get_selected_ice_types(peer->connection, &a, &b))
        return WEBRTC_PEER_ERR_INVALID_STATE;
    if (map_ice_candidate_type(a, &mapped_local) || map_ice_candidate_type(b, &mapped_remote))
        return WEBRTC_PEER_ERR_LIBPEER;
    *local = mapped_local;
    *remote = mapped_remote;
    return WEBRTC_PEER_OK;
}

const char *webrtc_peer_state_to_string(webrtc_peer_state_t state)
{
    switch (state) {
        case WEBRTC_PEER_STATE_CLOSED:
            return "closed";
        case WEBRTC_PEER_STATE_NEW:
            return "new";
        case WEBRTC_PEER_STATE_CHECKING:
            return "checking";
        case WEBRTC_PEER_STATE_CONNECTED:
            return "connected";
        case WEBRTC_PEER_STATE_COMPLETED:
            return "completed";
        case WEBRTC_PEER_STATE_FAILED:
            return "failed";
        case WEBRTC_PEER_STATE_DISCONNECTED:
            return "disconnected";
        default:
            return "unknown";
    }
}

void webrtc_peer_close(webrtc_peer_t *peer)
{
    if (!peer) {
        return;
    }
    webrtc_peer_disconnect(peer);
    free_ice_servers(peer);
    free(peer);
    library_release();
}
