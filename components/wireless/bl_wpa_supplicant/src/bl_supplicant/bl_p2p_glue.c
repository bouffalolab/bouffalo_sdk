#include <stddef.h>
#include <stdint.h>

#include "wl80211.h"
#include "macsw/wl80211_mac.h"

/*
 * Thin wl80211-facing helpers for the staged P2P bring-up.
 *
 * This keeps the first integration step small: upstream P2P builders/parsers
 * can be compiled in, while future control-plane work gets stable helpers for
 * off-channel dwell, management frame RX registration, and management TX.
 */

int bl_p2p_glue_register_mgmt_rx(wl80211_mgmt_rx_cb_t cb, void *prv)
{
    return wl80211_register_mgmt_rx_cb(cb, prv);
}

int bl_p2p_glue_unregister_mgmt_rx(void)
{
    return wl80211_unregister_mgmt_rx_cb();
}

int bl_p2p_glue_remain_on_channel_start(unsigned int freq, unsigned int duration_ms)
{
    return wl80211_remain_on_channel_start(WL80211_VIF_STA, (uint16_t) freq, duration_ms);
}

int bl_p2p_glue_remain_on_channel_cancel(void)
{
    return wl80211_cancel_remain_on_channel(WL80211_VIF_STA);
}

int bl_p2p_glue_tx_action(const uint8_t *frame, size_t frame_len)
{
    if (frame == NULL || frame_len == 0 || frame_len > UINT16_MAX) {
        return -1;
    }

    return wl80211_output_raw(WL80211_VIF_STA, (void *) frame, (uint16_t) frame_len,
                              WL80211_MAC_TX_FLAG_MGMT, NULL, NULL);
}

int bl_p2p_glue_tx_action_offchannel(const uint8_t *frame, size_t frame_len,
                                     unsigned int freq, unsigned int wait_time,
                                     void (*cb)(void *), void *opaque)
{
    struct wl80211_inject_frame_params params;

    if (frame == NULL || frame_len == 0 || frame_len > UINT16_MAX || freq == 0) {
        return -1;
    }

    memset(&params, 0, sizeof(params));
    params.duration_ms = wait_time ? wait_time : 200U;
    params.frame = (void *) frame;
    params.len = (uint16_t) frame_len;
    params.freq = (uint16_t) freq;
    params.wait_rx = true;
    params.cb = cb;
    params.opaque = opaque;

    return wl80211_inject_frame(&params);
}
