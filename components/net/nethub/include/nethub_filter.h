#ifndef NETHUB_FILTER_H
#define NETHUB_FILTER_H

#include "nethub_defs.h"

struct pbuf;

typedef enum {
    NETHUB_WIFI_RX_FILTER_DROP = 0,
    NETHUB_WIFI_RX_FILTER_LOCAL = 1 << 0,
    NETHUB_WIFI_RX_FILTER_HOST = 1 << 1,
    NETHUB_WIFI_RX_FILTER_BOTH = NETHUB_WIFI_RX_FILTER_LOCAL |
                                 NETHUB_WIFI_RX_FILTER_HOST,
} nethub_wifi_rx_filter_action_t;

typedef nethub_wifi_rx_filter_action_t (*nethub_wifi_rx_filter_cb_t)(
    nethub_channel_t src_channel,
    const struct pbuf *pkt,
    void *user_ctx);

/*
 * Replace the builtin Wi-Fi RX filter with one user callback.
 *
 * Passing NULL restores the builtin policy.
 * Passing non-NULL fully bypasses the builtin Wi-Fi RX policy.
 * This API must be called before nethub_bootstrap().
 * The callback runs on the Wi-Fi RX hot path and must not block, sleep,
 * free/retain/modify pkt, or assume that the whole frame is contiguous.
 */
int nethub_set_wifi_rx_filter(nethub_wifi_rx_filter_cb_t filter_cb, void *user_ctx);

#endif
