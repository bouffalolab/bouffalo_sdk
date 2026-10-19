#include "webrtc_peer_signaling.h"

#include <stdlib.h>
#include <string.h>

#if defined(CONFIG_WEBRTC_PEER_SIGNALING_APPRTC)
#include "apprtc_signaling.h"
#endif

struct webrtc_peer_signaling {
    webrtc_peer_signaling_config_t config;
#if defined(CONFIG_WEBRTC_PEER_SIGNALING_APPRTC)
    apprtc_signaling_t *apprtc;
#endif
    int transport_connected;
};

#if defined(CONFIG_WEBRTC_PEER_SIGNALING_APPRTC)
static void on_apprtc_message(apprtc_message_type_t type,
                              const char *data, void *user_data)
{
    webrtc_peer_signaling_t *signaling = user_data;
    webrtc_peer_message_t message;

    if (!signaling || !signaling->config.on_message) {
        return;
    }
    switch (type) {
        case APPRTC_MESSAGE_OFFER:
            message.type = WEBRTC_PEER_MSG_SDP_OFFER;
            break;
        case APPRTC_MESSAGE_ANSWER:
            message.type = WEBRTC_PEER_MSG_SDP_ANSWER;
            break;
        case APPRTC_MESSAGE_CANDIDATE:
            message.type = WEBRTC_PEER_MSG_ICE_CANDIDATE;
            break;
        case APPRTC_MESSAGE_BYE:
            message.type = WEBRTC_PEER_MSG_BYE;
            break;
        default:
            return;
    }
    message.payload = data;
    signaling->config.on_message(&message,
                                 signaling->config.user_data);
}
#endif

int webrtc_peer_signaling_start(
    const webrtc_peer_signaling_config_t *config,
    webrtc_peer_signaling_t **out_signaling)
{
    webrtc_peer_signaling_t *signaling;

    if (!config || !config->url || !config->on_message ||
        !out_signaling || *out_signaling) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    signaling = calloc(1, sizeof(*signaling));
    if (!signaling) {
        return WEBRTC_PEER_ERR_NO_MEMORY;
    }
    signaling->config = *config;

    switch (config->backend) {
        case WEBRTC_PEER_SIGNALING_APPRTC:
#if defined(CONFIG_WEBRTC_PEER_SIGNALING_APPRTC)
            signaling->apprtc = apprtc_signaling_join(config->url);
            if (!signaling->apprtc) {
                free(signaling);
                return WEBRTC_PEER_ERR_SIGNALING;
            }
            break;
#else
            free(signaling);
            return WEBRTC_PEER_ERR_UNSUPPORTED;
#endif
        default:
            free(signaling);
            return WEBRTC_PEER_ERR_UNSUPPORTED;
    }
    *out_signaling = signaling;
    return WEBRTC_PEER_OK;
}

int webrtc_peer_signaling_poll(webrtc_peer_signaling_t *signaling)
{
    if (!signaling) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    switch (signaling->config.backend) {
        case WEBRTC_PEER_SIGNALING_APPRTC:
#if defined(CONFIG_WEBRTC_PEER_SIGNALING_APPRTC)
            if (!signaling->transport_connected) {
                signaling->transport_connected = 1;
                if (apprtc_signaling_connect(
                        signaling->apprtc, on_apprtc_message,
                        signaling) != 0) {
                    signaling->transport_connected = 0;
                    return WEBRTC_PEER_ERR_SIGNALING;
                }
                return WEBRTC_PEER_OK;
            }
            return apprtc_signaling_poll(signaling->apprtc) == 0 ?
                WEBRTC_PEER_OK : WEBRTC_PEER_ERR_SIGNALING;
#else
            return WEBRTC_PEER_ERR_UNSUPPORTED;
#endif
        default:
            return WEBRTC_PEER_ERR_UNSUPPORTED;
    }
}

int webrtc_peer_signaling_send_msg(
    webrtc_peer_signaling_t *signaling,
    const webrtc_peer_message_t *message)
{
    if (!signaling || !message || !message->payload) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    switch (signaling->config.backend) {
        case WEBRTC_PEER_SIGNALING_APPRTC:
#if defined(CONFIG_WEBRTC_PEER_SIGNALING_APPRTC)
            if (message->type != WEBRTC_PEER_MSG_SDP_OFFER &&
                message->type != WEBRTC_PEER_MSG_SDP_ANSWER) {
                return WEBRTC_PEER_ERR_UNSUPPORTED;
            }
            return apprtc_signaling_send_sdp(
                       signaling->apprtc,
                       message->type == WEBRTC_PEER_MSG_SDP_OFFER,
                       message->payload) == 0 ?
                WEBRTC_PEER_OK : WEBRTC_PEER_ERR_SIGNALING;
#else
            return WEBRTC_PEER_ERR_UNSUPPORTED;
#endif
        default:
            return WEBRTC_PEER_ERR_UNSUPPORTED;
    }
}

int webrtc_peer_signaling_get_info(
    const webrtc_peer_signaling_t *signaling,
    webrtc_peer_signaling_info_t *info)
{
    if (!signaling || !info) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    memset(info, 0, sizeof(*info));
    switch (signaling->config.backend) {
        case WEBRTC_PEER_SIGNALING_APPRTC:
#if defined(CONFIG_WEBRTC_PEER_SIGNALING_APPRTC)
            info->initiator =
                apprtc_signaling_is_initiator(signaling->apprtc);
            info->room_id =
                apprtc_signaling_room_id(signaling->apprtc);
            info->ice_servers[0].url =
                apprtc_signaling_stun_url(signaling->apprtc);
            /*
             * libpeer can allocate TURN relays but cannot yet transport
             * media through TURN permissions/channels. Keep TURN out of
             * the advertised ICE list until that path is implemented.
             */
            return WEBRTC_PEER_OK;
#else
            return WEBRTC_PEER_ERR_UNSUPPORTED;
#endif
        default:
            return WEBRTC_PEER_ERR_UNSUPPORTED;
    }
}

void webrtc_peer_signaling_stop(
    webrtc_peer_signaling_t *signaling, int graceful)
{
    if (!signaling) {
        return;
    }
    switch (signaling->config.backend) {
        case WEBRTC_PEER_SIGNALING_APPRTC:
#if defined(CONFIG_WEBRTC_PEER_SIGNALING_APPRTC)
            if (graceful) {
                apprtc_signaling_leave(signaling->apprtc);
            }
            apprtc_signaling_destroy(signaling->apprtc);
#endif
            break;
        default:
            break;
    }
    free(signaling);
}
