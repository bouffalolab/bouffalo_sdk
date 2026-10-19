#include "webrtc_peer.h"

#include <stdlib.h>
#include <string.h>

#include "peer.h"
#include "peer_connection.h"
#include "sctp.h"

struct webrtc_peer {
    webrtc_peer_config_t config;
    PeerConnection *connection;
    webrtc_peer_role_t role;
    uint16_t active_sid;
    int active_sid_known;
    int data_association_open;
    int callback_error;
    webrtc_peer_stats_t stats;
};

static unsigned int g_peer_library_references;

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
        config->data_channel_type > WEBRTC_PEER_DATA_CHANNEL_BINARY) {
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
            peer_connection_set_remote_description_with_type(
                peer->connection, message->payload, SDP_TYPE_OFFER);
            if (peer_connection_get_state(peer->connection) ==
                    PEER_CONNECTION_FAILED) {
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
            peer_connection_set_remote_description_with_type(
                peer->connection, message->payload, SDP_TYPE_ANSWER);
            result = peer_connection_get_state(peer->connection) ==
                PEER_CONNECTION_FAILED ? WEBRTC_PEER_ERR_LIBPEER : WEBRTC_PEER_OK;
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
    if (!peer || !peer->connection || !data || size == 0) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    if (!webrtc_peer_is_data_channel_open(peer, sid)) {
        peer->stats.send_failures++;
        return WEBRTC_PEER_ERR_INVALID_STATE;
    }
    return map_send_result(
        peer,
        peer_connection_datachannel_send_sid(
            peer->connection, (char *)data, size, sid),
        &peer->stats.data_tx_messages);
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

webrtc_peer_state_t webrtc_peer_get_state(const webrtc_peer_t *peer)
{
    if (!peer || !peer->connection) {
        return WEBRTC_PEER_STATE_CLOSED;
    }
    return map_state(peer_connection_get_state(peer->connection));
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
