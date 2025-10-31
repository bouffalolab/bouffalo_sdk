/* LWIP Porting */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include <lwip/etharp.h>
#include <lwip/mem.h>
#include <lwip/netifapi.h>
#include <lwip/pbuf.h>
#include <lwip/tcpip.h>

#include "macsw/wl80211_mac.h"

struct netif eth_wl80211_netif;

#ifndef CONFIG_WL80211_RX_ZEROCOPY_THRES
#define CONFIG_WL80211_RX_ZEROCOPY_THRES 500
#endif

// FIXME
extern void *macsw_sta_vif;
extern uint8_t macsw_sta_vif_idx;
extern int mfg_media_read_macaddr_with_lock(uint8_t mac[6], uint8_t reload);

void wl80211_tcpip_input(void *rxhdr, void *buf, uint32_t frm_len,
                         uint32_t status /* enum rx_status_bits */) {
  struct pbuf_custom *pc;
  struct pbuf *p;

  // TODO If the current packet is being forwarded to the host, use zerocopy.
  if (frm_len < CONFIG_WL80211_RX_ZEROCOPY_THRES) {
    pc = malloc(sizeof(struct pbuf_custom) + frm_len);
    if (pc == NULL) {
      /// TODO
      assert(0);
    }

    p = pbuf_alloced_custom(PBUF_RAW, frm_len,
                            PBUF_REF | PBUF_TYPE_FLAG_STRUCT_DATA_CONTIGUOUS,
                            pc, buf, frm_len);
    pc->custom_free_function = (void *)free;

    memcpy(p->payload, buf, frm_len);
    // free rx right now
    wl80211_mac_rx_free(rxhdr);
  } else {
    pc = rxhdr;
    p = pbuf_alloced_custom(PBUF_RAW, frm_len,
                            PBUF_REF | PBUF_TYPE_FLAG_STRUCT_DATA_CONTIGUOUS,
                            pc, buf, frm_len);
    pc->custom_free_function = (void *)wl80211_mac_rx_free;
  }

  assert(p != NULL);
  assert(p->ref > 0);

  assert(eth_wl80211_netif.input != NULL);
  if (eth_wl80211_netif.input(p, &eth_wl80211_netif)) {
    pbuf_free(p);
  }
}

/* lwip output function */
err_t wl80211_output(struct netif *net_if, struct pbuf *buf) {
  struct wl80211_tx_header *txhdr;

  struct iovec txseg[5];

  int seg_cnt;

  int payload_len = buf->tot_len;
  int remain_len = payload_len;

  // first segment record payload
  txseg[0].iov_base = buf->payload;
  txseg[0].iov_len = buf->len;
  remain_len -= buf->len;

  // Increase the ref count so that the buffer is not freed by the networking
  // stack until it is actually sent over the WiFi interface
  pbuf_ref(buf);

  if (pbuf_header(buf, PBUF_LINK_ENCAPSULATION_HLEN)) {
    abort();
  }

  /* get tx desc */
  txhdr = (void *)ALIGN4_HI((uint32_t)buf->payload);

  struct pbuf *buf_t = buf->next;
  int idx = 1;
  while (remain_len && buf_t && (idx < 5)) {
    txseg[idx].iov_base = buf->payload;
    txseg[idx].iov_len = buf->len;

    remain_len -= buf->len;
    idx++;
    buf_t = buf_t->next;
  }
  seg_cnt = idx;
  assert(remain_len == 0);

  if (wl80211_mac_tx(txhdr, 0, txseg, seg_cnt, pbuf_free, buf)) {
    pbuf_free(buf);
    return ERR_BUF;
  }

  return (ERR_OK);
}

/* raw packet send: eapol and mgmt */
struct pbuf_custom_raw {
  void *opaque;
  void (*cb)(void *);

  struct pbuf_custom pc;
};

static void pbuf_custom_raw_free(struct pbuf_custom *pc) {
  struct pbuf_custom_raw *p = container_of(pc, struct pbuf_custom_raw, pc);

  if (p->cb) {
    p->cb(p->opaque);
  }
  mem_free(p);
}

int wl80211_output_raw(void *buffer, uint16_t len, unsigned int flags,
                       void (*cb)(void *), void *opaque) {
  struct pbuf *p;
  struct pbuf_custom_raw *pc;
  struct wl80211_tx_header *txhdr;
  struct iovec txseg[1];

  // alloc tx packet in lwip heap (wifi ram)
  uint16_t payload_len =
      (uint16_t)(LWIP_MEM_ALIGN_SIZE(PBUF_RAW_TX) + LWIP_MEM_ALIGN_SIZE(len));

  pc = (struct pbuf_custom_raw *)mem_malloc(payload_len +
                                            sizeof(struct pbuf_custom_raw));
  if (!pc) {
    return -1;
  }

  // reinit pbuf as pbuf_custom_wpa
  p = pbuf_alloced_custom(PBUF_RAW_TX, len, PBUF_RAM, &pc->pc, pc + 1,
                          payload_len);
  assert(p != NULL);
  pc->pc.custom_free_function = (void *)pbuf_custom_raw_free;
  pc->cb = cb;
  pc->opaque = opaque;

  if (ERR_OK != pbuf_take(p, buffer, len)) {
    pbuf_free(p);
    return -2;
  }

  txseg[0].iov_base = p->payload;
  txseg[0].iov_len = p->len;

  if (pbuf_header(p, PBUF_LINK_ENCAPSULATION_HLEN)) {
    abort();
  }

  /* get tx desc */
  txhdr = (void *)ALIGN4_HI((uint32_t)p->payload);

  if (wl80211_mac_tx(txhdr, flags, txseg, 1, pbuf_free, p)) {
    pbuf_free(p);
    return -3;
  } else {
    return 0;
  }
}

static err_t sta_netif_init(struct netif *net_if) {
  err_t status = ERR_OK;

#if LWIP_NETIF_HOSTNAME
  {
    /* Initialize interface hostname */
    net_if->hostname = "ctrl";
  }
#endif /* LWIP_NETIF_HOSTNAME */

  net_if->name[0] = 's';
  net_if->name[1] = 't';

  net_if->output = etharp_output;

#ifdef CFG_IPV6
  net_if->flags = NETIF_FLAG_LINK_UP | NETIF_FLAG_BROADCAST |
                  NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP |
                  NETIF_FLAG_MLD6;
  net_if->output_ip6 = ethip6_output;
#else
  net_if->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
                  NETIF_FLAG_ETHERNET | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
#endif

  net_if->hwaddr_len = ETHARP_HWADDR_LEN;

  if (0 == mfg_media_read_macaddr_with_lock((uint8_t *)net_if->hwaddr, 1)) {
  }

  printf("mac addr: %02X:%02X:%02X:%02X:%02X:%02X\r\n", net_if->hwaddr[0],
         net_if->hwaddr[1], net_if->hwaddr[2], net_if->hwaddr[3],
         net_if->hwaddr[4], net_if->hwaddr[5]);

  // hwaddr is updated in net_if_add
  net_if->mtu = 1500;
  net_if->linkoutput = wl80211_output;

  return status;
}

int wl80211_lwip_init_sta(void) {
  err_t status;
  status =
      netifapi_netif_add(&eth_wl80211_netif, (const ip4_addr_t *)NULL,
                         (const ip4_addr_t *)NULL, (const ip4_addr_t *)NULL,
                         &eth_wl80211_netif, sta_netif_init, tcpip_input);

  assert(status == ERR_OK);
  return 0;
}
