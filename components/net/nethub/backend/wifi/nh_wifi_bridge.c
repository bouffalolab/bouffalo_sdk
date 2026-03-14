#include "nh_wifi_bridge.h"
#include "nh_wifi_backend.h"
#include "nh_pbuf_dump.h"
#include "nh_filter.h"
#include "nh_hub.h"
#include "nh_runtime.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "lwip/pbuf.h"

#define DEBUG_BYPASS_WIFI_TX      0
#define DEBUG_BYPASS_WIFI_RX      0
#define DEBUG_DUMP_WIFITX_ENABLE  0
#define DEBUG_DUMP_WIFIRX_ENABLE  0

typedef struct {
    struct pbuf_custom pbuf;
    void (*free_cb)(void *arg);
    void *cb_arg;
} nh_wifi_trans_desc_t;

static bool g_wifi_rx_hook_registered;

#if (DEBUG_DUMP_WIFITX_ENABLE || DEBUG_DUMP_WIFIRX_ENABLE)
static const char *nh_wifi_dump_action_name(nh_filter_action_t action)
{
    switch (action) {
        case NH_FILTER_ACTION_DROP:
            return "drop";
        case NH_FILTER_ACTION_LOCAL:
            return "local";
        case NH_FILTER_ACTION_HOST:
            return "host";
        case NH_FILTER_ACTION_BOTH:
            return "both";
        default:
            return "unknown";
    }
}

static void nh_wifi_dump_pbuf(const char *tag, const struct pbuf *p)
{
    nh_diag_dump_pbuf(tag, p);
}

static void nh_wifi_dump_rx_pbuf(bool is_sta, nh_filter_action_t action, const struct pbuf *p)
{
    char tag[32];

    snprintf(tag, sizeof(tag), "%s_rx[%s]",
             is_sta ? "wifi_sta" : "wifi_ap",
             nh_wifi_dump_action_name(action));
    nh_wifi_dump_pbuf(tag, p);
}
#endif

static int nh_wifi_tx_to_air(struct pbuf *msg, bool is_sta)
{
    nh_wifi_backend_tx_result_t ret;
    nethub_statistics_t *stats = nh_runtime_stats();
    struct pbuf *p = (struct pbuf *)msg;

    ret = nh_wifi_backend_tx(p, is_sta);
    if (ret == NH_WIFI_BACKEND_TX_OK) {
        stats->dnld_total_success++;
    } else {
        if (ret == NH_WIFI_BACKEND_TX_ERR_NETIF_DOWN) {
            stats->dnld_consumer_netif_down++;
        } else {
            stats->dnld_consumer_tx_fail++;
        }

        stats->dnld_total_dropped++;
    }

    return 0;
}

static void nh_wifi_dnld_custom_free(struct pbuf *p)
{
    nethub_statistics_t *stats = nh_runtime_stats();
    nh_wifi_trans_desc_t *desc = (nh_wifi_trans_desc_t *)p;

    stats->dnld_consumer_free_normal++;
    desc->free_cb(desc->cb_arg);
}

#define FRAME_BUFF_ELEM_SIZE        32
#define FRAME_BUFF_PBUFCUSTOM       32
#define FRAME_BUFF_HWDESCTAG        388
#define FRAME_BUFF_MSGSTRUCT        8
#define FRAME_BUFF_MSGSTRUCT_OFFSET (FRAME_BUFF_ELEM_SIZE + FRAME_BUFF_PBUFCUSTOM + FRAME_BUFF_HWDESCTAG)
#define FRAME_BUFF_PAYLOAD_OFFSET   (FRAME_BUFF_MSGSTRUCT_OFFSET + FRAME_BUFF_MSGSTRUCT)
#define FRAME_BUFF_PAYLOAD          1536

static int nh_wifi_host_to_air(nethub_frame_t *frame, bool is_sta)
{
    nh_wifi_trans_desc_t *desc;
    nethub_statistics_t *stats = nh_runtime_stats();

    stats->dnld_total_packets++;

#if DEBUG_BYPASS_WIFI_TX
    frame->free_cb(frame->cb_arg);
    return 0;
#endif

    desc = (nh_wifi_trans_desc_t *)((uintptr_t)frame->cb_arg);
    desc->free_cb = frame->free_cb;
    desc->cb_arg = frame->cb_arg;

    pbuf_alloced_custom(0,
                        frame->len,
                        (PBUF_ALLOC_FLAG_DATA_CONTIGUOUS | PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP),
                        &desc->pbuf,
                        NULL,
                        FRAME_BUFF_PAYLOAD);

    desc->pbuf.pbuf.payload = frame->data;
    desc->pbuf.custom_free_function = nh_wifi_dnld_custom_free;

#if DEBUG_DUMP_WIFITX_ENABLE
    nh_wifi_dump_pbuf(is_sta ? "wifi_sta_tx" : "wifi_ap_tx", &desc->pbuf.pbuf);
#endif

    nh_wifi_tx_to_air((struct pbuf *)desc, is_sta);
    return 0;
}

static void nh_wifi_upld_custom_free(void *arg)
{
    nethub_statistics_t *stats = nh_runtime_stats();
    struct pbuf *p = (struct pbuf *)arg;

    stats->upld_free_count++;
    pbuf_free(p);
}

struct pbuf *nh_wifi_bridge_handle_rx(bool is_sta, struct pbuf *p)
{
    nh_filter_action_t action;
    struct pbuf *local_pkt = NULL;
    nethub_frame_t frame;
    nethub_route_result_t result;
    nethub_statistics_t *stats = nh_runtime_stats();

    action = nh_filter_wifi_rx(is_sta ? NETHUB_CHANNEL_WIFI_STA
                                      : NETHUB_CHANNEL_WIFI_AP,
                               p);
#if DEBUG_DUMP_WIFIRX_ENABLE
    nh_wifi_dump_rx_pbuf(is_sta, action, p);
#endif
    if (action == NH_FILTER_ACTION_DROP) {
        pbuf_free(p);
        return NULL;
    }

    if (action == NH_FILTER_ACTION_BOTH) {
        local_pkt = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
        if (local_pkt == NULL) {
            pbuf_free(p);
            return NULL;
        }
        pbuf_copy(local_pkt, p);
    }

    if (nh_filter_should_deliver_host(action)) {
        frame.data = p->payload;
        frame.len = p->len;
        frame.free_cb = nh_wifi_upld_custom_free;
        frame.cb_arg = p;
        frame.next = NULL;

        stats->upld_producer_count++;
        stats->upld_total_packets++;

#if DEBUG_BYPASS_WIFI_RX
        nh_wifi_upld_custom_free(p);
#else
        result = nethub_process_input(&frame,
                                      is_sta ? NETHUB_CHANNEL_WIFI_STA
                                             : NETHUB_CHANNEL_WIFI_AP);
        NETHUB_UNUSED(result);
#endif
    }

    if (nh_filter_should_deliver_local(action)) {
        if (local_pkt != NULL) {
            return local_pkt;
        }
        return p;
    }

    return NULL;
}

static int nh_wifi_bridge_init(void)
{
    if (g_wifi_rx_hook_registered) {
        return NETHUB_OK;
    }

    if (nh_wifi_backend_init() != 0) {
        return NETHUB_ERR_INTERNAL;
    }

    g_wifi_rx_hook_registered = true;
    return NETHUB_OK;
}

int nethub_set_active_wifi_channel(nethub_channel_t channel)
{
    return nh_runtime_set_active_wifi_channel(channel);
}

static nethub_route_result_t nh_wifi_sta_input(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(frame);
    NETHUB_UNUSED(arg);
    return NETHUB_ROUTE_CONTINUE;
}

static nethub_route_result_t nh_wifi_sta_output(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(arg);
    nh_wifi_host_to_air(frame, true);
    return NETHUB_ROUTE_CONTINUE;
}

static nethub_route_result_t nh_wifi_ap_input(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(frame);
    NETHUB_UNUSED(arg);
    return NETHUB_ROUTE_CONTINUE;
}

static nethub_route_result_t nh_wifi_ap_output(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(arg);
    nh_wifi_host_to_air(frame, false);
    return NETHUB_ROUTE_CONTINUE;
}

const struct nhif_ops nhsta_ops = {
    .type = NETHUB_CHANNEL_WIFI_STA,
    .init = nh_wifi_bridge_init,
    .deinit = NULL,
    .input = nh_wifi_sta_input,
    .output = nh_wifi_sta_output,
};

const struct nhif_ops nhap_ops = {
    .type = NETHUB_CHANNEL_WIFI_AP,
    .init = NULL,
    .deinit = NULL,
    .input = nh_wifi_ap_input,
    .output = nh_wifi_ap_output,
};

const struct nhif_ops *nh_wifi_endpoint_get_ops(size_t index)
{
    switch (index) {
        case 0:
            return &nhsta_ops;
        case 1:
            return &nhap_ops;
        default:
            return NULL;
    }
}

size_t nh_wifi_endpoint_get_count(void)
{
    return 2;
}
