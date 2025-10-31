#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "macsw/wl80211_mac.h"

STAILQ_HEAD(, wl80211_mac_tx_desc) macsw_txq[TID_MAX];
static uint16_t macsw_txq_bitmap = 0;
static uint16_t macsw_hw_txq_desc_cnt = 0;
int8_t macsw_txq_credits[TID_MAX];

// Loop on all the descriptors ready for TX
int fhost_tx_buffer_get(struct txdesc_public *txdesc, int *qidx) {
  struct wl80211_mac_tx_desc *desc;
  uint16_t bitmap = macsw_txq_bitmap;
  uint8_t hi_tid;

  if (bitmap == 0) {
    return 1;
  }

  hi_tid = sizeof(unsigned int) * 8 - 1 - __builtin_clz((unsigned int)bitmap);
  assert(hi_tid < TID_MAX);

  if (macsw_hw_txq_desc_cnt == 0) {
    return 1;
  }

  if (macsw_txq_credits[hi_tid] == 0 || STAILQ_EMPTY(&macsw_txq[hi_tid])) {
    return 1;
  }

  // Get the descriptor from the queue
  GLOBAL_INT_DISABLE();
  desc = STAILQ_FIRST(&macsw_txq[hi_tid]);
  assert(desc != NULL);

  STAILQ_REMOVE_HEAD(&macsw_txq[hi_tid], entries);

  macsw_txq_credits[hi_tid]--;
  macsw_hw_txq_desc_cnt--;

  /* clear bitmap if txq empty */
  if (STAILQ_EMPTY(&macsw_txq[hi_tid])) {
    macsw_txq_bitmap &= ~(1 << hi_tid);
  }
  GLOBAL_INT_RESTORE();

  // Copy the descriptor
  memcpy(&txdesc->host, (void *)&desc->txdesc.host, sizeof(desc->txdesc.host));

  *qidx = mac_tid2ac[desc->txdesc.host.tid];

  txdesc_set_lmac_hwdesc(txdesc, (void *)&desc->txdesc.hw_desc);
  txdesc_set_lmac_buffer(txdesc, 0, (void *)&desc->txdesc.buffer);

  return 0;
}

// iovec(idx 0 must point to ether_header)
int wl80211_mac_tx(struct wl80211_tx_header *txhdr, unsigned int flags,
                   struct iovec *seg, int seg_cnt, void *txdone_cb,
                   void *opaque) {
  assert(txhdr != NULL);
  assert(seg != NULL);
  assert(seg_cnt > 0);

  struct wl80211_mac_tx_desc *desc = (void *)txhdr;

  struct hostdesc *host;
  size_t payload_len = seg[0].iov_len;

  for (int i = 1; i < seg_cnt; i++) {
    payload_len += seg[i].iov_len;
  }

  // install user private data
  desc->opaque = opaque;
  desc->txdone_cb = txdone_cb;

  host = &desc->txdesc.host;
  host->hostid = desc;
  host->flags = 0; // TODO NOCCK

  if (flags & WL80211_MAC_TX_FLAG_MGMT) {
    host->flags |= TXU_CNTRL_MGMT;
  }

  if (host->flags & TXU_CNTRL_MGMT) {
    struct mac_hdr *mac_hdr = (struct mac_hdr *)seg[0].iov_base;
    mac_hdr->seq = 0;
    host->eth_dest_addr = mac_hdr->addr1;
    host->tid = TID_MGT;
    #if MACSW_MFP
    if (mfp_is_robust_frame(mac_hdr->fctl, *((uint8_t *)(mac_hdr + 1))))
        host->flags |= TXU_CNTRL_MGMT_ROBUST;
    #endif
  } else {
    struct mac_eth_hdr *eth_hdr = (struct mac_eth_hdr *)seg[0].iov_base;
    host->eth_dest_addr = eth_hdr->da;
    host->eth_src_addr = eth_hdr->sa;
    host->ethertype = eth_hdr->len;

    if (co_ntohs(host->ethertype) == LLC_ETHERTYPE_IP) {
      uint8_t dscp;

      // Read DSCP field in the IP header to determine TID (2nd byte after the
      // header)
      dscp = *((uint8_t *)(eth_hdr + 1) + 1);

      host->tid = (dscp & 0xFC) >> 5;
    } else {
      host->tid = 0;
    }

    payload_len -= sizeof(*eth_hdr);

    seg[0].iov_base = (uint8_t *)seg[0].iov_base + sizeof(*eth_hdr);
    seg[0].iov_len -= sizeof(*eth_hdr);
  }

  host->vif_idx = mac_vif_get_index(macsw_sta_vif);
  host->staid = _macsw_get_staid(macsw_sta_vif, &host->eth_dest_addr,
                                 host->flags & TXU_CNTRL_MGMT);

  if (host->staid == INVALID_STA_IDX && !(host->flags & TXU_CNTRL_MGMT)) {
    return -1;
  }

  host->packet_addr[0] = (uint32_t)seg[0].iov_base;
  host->packet_len[0] = payload_len;

  // FIXME ugly hack; need modify macsw api
  do {
    uint32_t seg_addr[TX_PBD_CNT];
    uint16_t seg_len[TX_PBD_CNT];

    for (int i = 0; i < seg_cnt; i++) {
      seg_addr[i] = (uint32_t)seg[i].iov_base;
      seg_len[i] = seg[i].iov_len;
    }

    tx_desc_init_for_fhost(desc->txdesc.buffer, desc->txdesc.hw_desc, seg_cnt,
                           seg_addr, seg_len);
  } while (0);

  GLOBAL_INT_DISABLE();
  STAILQ_INSERT_TAIL(&macsw_txq[host->tid], desc, entries);
  macsw_txq_bitmap |= (1 << host->tid);
  GLOBAL_INT_RESTORE();

  // inform macsw to handle tx
  macif_tx_data_ind(-1);

  return 0;
}

void macif_tx_cfm_push(uint8_t access_category, struct txdesc_public *txdesc) {
  struct wl80211_mac_tx_desc *desc;
  struct tx_cfm_tag *cfm;
  desc = txdesc->host.hostid;
  cfm = &txdesc->host.cfm;
  uint8_t tid = txdesc->host.tid;

  /* release macsw txdesc */
  tx_txdesc_release(txdesc);

  GLOBAL_INT_DISABLE();
  /* update credits */
  macsw_txq_credits[tid] += cfm->credits;
  macsw_hw_txq_desc_cnt++;
  GLOBAL_INT_RESTORE();

  /* inform upper layer free desc */
  desc->txdone_cb(desc->opaque);

  /* credits update, tx now */
  macif_tx_data_ind(-1);
}

void wl80211_mac_tx_init(void) {
  int i;

  for (i = 0; i < TID_MAX; i++) {
    macsw_txq_credits[i] = MACSW_DEFAULT_TX_CREDIT_CNT;
    STAILQ_INIT(&macsw_txq[i]);
  }

  /* total number of txdesc */
  for (i = 0; i < MACSW_TXQ_CNT; i++) {
    macsw_hw_txq_desc_cnt += get_mac_txdesc_cnt(i);
  }
}
