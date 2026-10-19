#ifndef APPRTC_SIGNALING_H
#define APPRTC_SIGNALING_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    APPRTC_MESSAGE_OFFER = 0,
    APPRTC_MESSAGE_ANSWER,
    APPRTC_MESSAGE_CANDIDATE,
    APPRTC_MESSAGE_BYE,
} apprtc_message_type_t;

typedef void (*apprtc_message_handler_t)(apprtc_message_type_t type,
                                         const char *data,
                                         void *user_data);

typedef struct apprtc_signaling apprtc_signaling_t;

apprtc_signaling_t *apprtc_signaling_join(const char *join_url);
int apprtc_signaling_connect(apprtc_signaling_t *signaling,
                             apprtc_message_handler_t handler,
                             void *user_data);
int apprtc_signaling_poll(apprtc_signaling_t *signaling);
int apprtc_signaling_send_sdp(apprtc_signaling_t *signaling,
                              int offer,
                              const char *sdp);
void apprtc_signaling_leave(apprtc_signaling_t *signaling);
void apprtc_signaling_destroy(apprtc_signaling_t *signaling);

int apprtc_signaling_is_initiator(const apprtc_signaling_t *signaling);
const char *apprtc_signaling_room_id(const apprtc_signaling_t *signaling);
const char *apprtc_signaling_stun_url(const apprtc_signaling_t *signaling);
const char *apprtc_signaling_turn_url(const apprtc_signaling_t *signaling);
const char *apprtc_signaling_ice_username(const apprtc_signaling_t *signaling);
const char *apprtc_signaling_ice_credential(const apprtc_signaling_t *signaling);

#ifdef __cplusplus
}
#endif

#endif
