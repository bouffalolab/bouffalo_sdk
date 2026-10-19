#ifndef WEBRTC_PEER_SIGNALING_H
#define WEBRTC_PEER_SIGNALING_H

#include "webrtc_peer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct webrtc_peer_signaling webrtc_peer_signaling_t;

typedef enum {
    WEBRTC_PEER_SIGNALING_APPRTC = 0,
} webrtc_peer_signaling_backend_t;

typedef void (*webrtc_peer_signaling_message_cb_t)(
    const webrtc_peer_message_t *message, void *user_data);

typedef struct {
    webrtc_peer_signaling_backend_t backend;
    const char *url;
    webrtc_peer_signaling_message_cb_t on_message;
    void *user_data;
} webrtc_peer_signaling_config_t;

typedef struct {
    int initiator;
    const char *room_id;
    webrtc_peer_ice_server_t ice_servers[WEBRTC_PEER_MAX_ICE_SERVERS];
} webrtc_peer_signaling_info_t;

int webrtc_peer_signaling_start(
    const webrtc_peer_signaling_config_t *config,
    webrtc_peer_signaling_t **signaling);
int webrtc_peer_signaling_poll(webrtc_peer_signaling_t *signaling);
int webrtc_peer_signaling_send_msg(
    webrtc_peer_signaling_t *signaling,
    const webrtc_peer_message_t *message);
int webrtc_peer_signaling_get_info(
    const webrtc_peer_signaling_t *signaling,
    webrtc_peer_signaling_info_t *info);
void webrtc_peer_signaling_stop(
    webrtc_peer_signaling_t *signaling, int graceful);

#ifdef __cplusplus
}
#endif

#endif
