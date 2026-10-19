#include "nh_wifi_backend.h"

#include "nh_common.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "lwip/netif.h"

#include <wl80211.h>

/*
 * nethub only needs the minimal WL80211 TX/RX hook surface.
 * Keep those backend-private symbols local here instead of
 * pulling wl80211/macsw private headers into nethub.
 */
struct wl80211_tx_header;
int wl80211_mac_tx(uint8_t vif_type, struct wl80211_tx_header *desc, unsigned int flags,
                   struct iovec *seg, int seg_cnt, void (*cb)(void *), void *opaque);
void wl80211_mac_rx_free(void *info);

static void nh_wifi_wl80211_tx_free_cb(void *arg)
{
    pbuf_free((struct pbuf *)arg);
}

static err_t nh_wifi_wl80211_output(enum wl80211_vif_type vif, struct pbuf *buf)
{
    struct wl80211_tx_header *txhdr;
    struct iovec txseg[5];
    struct pbuf *buf_t;
    int seg_cnt;
    int payload_len = buf->tot_len;
    int remain_len = payload_len;
    int idx = 1;

    txseg[0].iov_base = buf->payload;
    txseg[0].iov_len = buf->len;
    remain_len -= buf->len;

    pbuf_ref(buf);

    if (pbuf_header_force(buf, PBUF_LINK_ENCAPSULATION_HLEN)) {
        abort();
    }

    txhdr = (void *)ALIGN4_HI((uint32_t)buf->payload);
    buf_t = buf->next;

    while (remain_len && buf_t && (idx < 5)) {
        txseg[idx].iov_base = buf_t->payload;
        txseg[idx].iov_len = buf_t->len;
        remain_len -= buf_t->len;
        idx++;
        buf_t = buf_t->next;
    }

    seg_cnt = idx;
    assert(remain_len == 0);

    if (wl80211_mac_tx(vif, txhdr, 0, txseg, seg_cnt, nh_wifi_wl80211_tx_free_cb, buf)) {
        pbuf_free(buf);
        return ERR_BUF;
    }

    return ERR_OK;
}

static int nh_wifi_wl80211_input_cb(void *prv, uint8_t vif_type, void *rxhdr,
                                    void *buf, uint32_t frm_len, uint32_t status)
{
    struct pbuf_custom *pc;
    struct pbuf *p;
    enum wl80211_vif_type vif = (enum wl80211_vif_type)vif_type;

    NETHUB_UNUSED(prv);
    NETHUB_UNUSED(status);

    pc = rxhdr;
    p = pbuf_alloced_custom(PBUF_RAW, frm_len,
                            PBUF_REF | PBUF_TYPE_FLAG_STRUCT_DATA_CONTIGUOUS,
                            pc, buf, frm_len);
    pc->custom_free_function = (void *)wl80211_mac_rx_free;

    p = nh_wifi_bridge_handle_rx(vif == WL80211_VIF_STA, p);
    if (p != NULL) {
        extern struct netif vif2netif[WL80211_VIF_MAX];

        if (vif2netif[vif].input(p, &vif2netif[vif])) {
            pbuf_free(p);
        }
    }

    return 0;
}

int nh_wifi_backend_init(void)
{
    return wl80211_register_input_cb(nh_wifi_wl80211_input_cb, NULL);
}

nh_wifi_backend_tx_result_t nh_wifi_backend_tx(struct pbuf *p, bool is_sta)
{
    enum wl80211_vif_type vif = is_sta ? WL80211_VIF_STA : WL80211_VIF_AP;

    if (nh_wifi_wl80211_output(vif, p) != ERR_OK) {
        pbuf_free(p);
        return NH_WIFI_BACKEND_TX_ERR_SEND;
    }

    pbuf_free(p);
    return NH_WIFI_BACKEND_TX_OK;
}
