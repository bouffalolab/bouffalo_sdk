#ifndef _WL80211_ASYNC_EVENT_H_
#define _WL80211_ASYNC_EVENT_H_

#include <stdint.h>

#include "async_event.h"

typedef struct async_input_event_mac {
    struct async_input_event event;
    uint8_t mac[6];
} *async_input_event_mac_t;

int async_post_event_with_mac(uintptr_t type, uint16_t code, unsigned long value, const uint8_t mac[6]);

#endif
