#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "macsw.h"
#include "macsw/wl80211_mac.h"

#include "supplicant_api.h"

#define WG wl80211_glb

/* RX buffers */
STAILQ_HEAD(, wl80211_mac_rx_desc)
macsw_rx_q = STAILQ_HEAD_INITIALIZER(macsw_rx_q);

static struct wl80211_mac_rx_desc
    rx_buf_mem[(MACSW_MAX_BA_RX * MACSW_AMPDU_RX_BUF_SIZE + 2)] __SHAREDRAM;

/* wpa3 commit frame */
static void sme_external_auth_mgmt_rx(uint8_t vif_idx, uint8_t *frame,
                                      size_t len) {
  uint16_t auth_alg, auth_transaction, auth_status;
  uintptr_t frame_auth;
  struct sm_external_auth_required_rsp rsp;

  if (len < sizeof(struct mac_hdr) + MAC_AUTH_CHALLENGE_OFT) {
    printf("sae invalid auth len\n");
    rsp.status = MAC_ST_AUTH_CHALLENGE_FAILED;
    goto _RSP;
  }

  frame_auth = (uintptr_t)frame + sizeof(struct mac_hdr);

  auth_alg = co_wtohs(co_read16p(frame_auth + MAC_AUTH_ALGONBR_OFT));
  auth_transaction = co_wtohs(co_read16p(frame_auth + MAC_AUTH_SEQNBR_OFT));
  auth_status = co_wtohs(co_read16p(frame_auth + MAC_AUTH_STATUS_OFT));

  if (auth_alg != MAC_AUTH_ALGO_SAE) {
    printf("sae invalid auth alg\n");
    rsp.status = MAC_ST_AUTH_ALGO_NOT_SUPPORTED;
    goto _RSP;
  }

  frame_auth += MAC_AUTH_CHALLENGE_OFT;
  len -= sizeof(struct mac_hdr) + MAC_AUTH_CHALLENGE_OFT;

  if (wpa_cbs->wpa3_parse_sae_msg((uint8_t *)frame_auth, len, auth_transaction,
                                  auth_status) != 0) {
    printf("wpa3 parse sae faild\n");
    rsp.status = MAC_ST_FAILURE;
    goto _RSP;
  } else {
    if (auth_transaction == MAC_AUTH_SECOND_SEQ) {
      printf("wpa3 auth success!\n");
      // auth succcess
      rsp.status = MAC_ST_SUCCESSFUL;
      WG.authenticating = 0;
      goto _RSP;
    } else {
      _sae_send_frame(MAC_AUTH_SECOND_SEQ);
    }
  }

  return;

_RSP:
  rsp.vif_idx = vif_idx;
  macif_kmsg_call(-1, NULL, 0, SM_EXTERNAL_AUTH_REQUIRED_RSP, TASK_SM, &rsp,
                  sizeof(struct sm_external_auth_required_rsp));
}

/* called by macsw rx process */
// 管理帧也会通过这个接口上传
void macif_rx_desc_upload(struct co_list *desc_list) {
  struct rxu_stat_desc *rx_stat_desc =
      (struct rxu_stat_desc *)co_list_pick(desc_list);
  uint8_t vif_idx = -1;

  while ((rx_stat_desc != NULL) && (rx_stat_desc->rxdesc == NULL)) {
    struct wl80211_mac_rx_desc *rx_buf =
        (void *)(uintptr_t)rx_stat_desc->val.host_id;

    do {
      /* check len update at first */
      if (rx_stat_desc->val.status & RX_STAT_LEN_UPDATE) {
        rx_buf->info.vect.frmlen = rx_stat_desc->val.frame_len;

        if (rx_stat_desc->val.status & RX_STAT_ETH_LEN_UPDATE) {
          // Update Length Field inside the Ethernet Header
          struct mac_eth_hdr *hdr = (struct mac_eth_hdr *)rx_buf->payload;

          hdr->len = co_htons(rx_stat_desc->val.frame_len -
                              sizeof(struct mac_eth_hdr));
        }
      }

      if (rx_stat_desc->val.status & RX_STAT_FORWARD) {
        if (rx_buf->info.flags & RX_FLAGS_NON_MSDU_MSK) {
          struct mac_hdr *mgmt = (struct mac_hdr *)rx_buf->payload;
          uint16_t frame_control = mgmt->fctl;

          if ((frame_control & MAC_FCTRL_TYPE_MASK) == MAC_FCTRL_MGT_T &&
              (frame_control & MAC_FCTRL_SUBT_MASK) == MAC_FCTRL_AUTHENT_ST &&
              WG.authenticating) {
            sme_external_auth_mgmt_rx(vif_idx, (uint8_t *)mgmt,
                                      rx_buf->info.vect.frmlen);
          }

          wl80211_mac_rx_free(&rx_buf->info);
        } else {
          vif_idx = (rx_buf->info.flags & RX_FLAGS_VIF_INDEX_MSK) >>
                    RX_FLAGS_VIF_INDEX_OFT;

          // FIXME test for sta only
          assert(vif_idx == macsw_sta_vif_idx);
          assert(rx_buf->info.vect.frmlen != 0);

#define ETH_HDR_LEN 14
          const struct mac_eth_hdr *hdr =
              (const struct mac_eth_hdr *)rx_buf->payload;

          /* route eapol packet*/
          if (WG.associated && !WG.link_up && hdr->len /* type */ == 0x8e88) {
            WG.authenticating = 1;
            wpa_cbs->wpa_sta_rx_eapol((uint8_t *)hdr->sa.array,
                                      (uint8_t *)rx_buf->payload + ETH_HDR_LEN,
                                      rx_buf->info.vect.frmlen - ETH_HDR_LEN);
          } else {
            wl80211_tcpip_input(&rx_buf->info, rx_buf->payload,
                                rx_buf->info.vect.frmlen,
                                rx_stat_desc->val.status);
          }
        }
        break;
      }

      if (rx_stat_desc->val.status & RX_STAT_ALLOC) {
        break;
      }

      // others
      wl80211_mac_rx_free(&rx_buf->info);
      printf("%s: free unhandled rx buf: %x\n", __func__,
             rx_stat_desc->val.status);
    } while (0);

    // Extract the descriptor from the list
    co_list_pop_front(desc_list);

    // Free it by clearing its status
    rx_stat_desc->val.status = 0;

    // Get next descriptor
    rx_stat_desc = (struct rxu_stat_desc *)co_list_pick(desc_list);
  }
}

/* free rx buffer */
void wl80211_mac_rx_free(void *info) {
  assert(info != NULL);
  struct wl80211_mac_rx_desc *rx_buf =
      container_of(info, struct wl80211_mac_rx_desc, info);

#ifdef INVARIANTS
  int rx_buf_idx = rx_buf - rx_buf_mem;
  if (!(rx_buf_idx >= 0 && rx_buf_idx < CO_ARRAY_SIZE(rx_buf_mem))) {
    printf("payload is not in rx_buf_mem\r\n");
    abort();
  }
  if (rx_buf != &rx_buf_mem[rx_buf_idx]) {
    printf("payload is not align!!\r\n");
    abort();
  }
#endif

  GLOBAL_INT_DISABLE();
  STAILQ_INSERT_TAIL(&macsw_rx_q, rx_buf, entries);
  GLOBAL_INT_RESTORE();

  macif_rx_buf_ind();
}

bool macif_rx_buf_check(void) {
  // no need lock
  return !STAILQ_EMPTY(&macsw_rx_q);
}

uint32_t macif_rx_buf_get(uint32_t *host_id) {
  struct wl80211_mac_rx_desc *buf;

  // Pop a buffer from the RX buffer queue (without waiting)
  GLOBAL_INT_DISABLE();
  buf = STAILQ_FIRST(&macsw_rx_q);
  assert(buf != NULL);

  STAILQ_REMOVE_HEAD(&macsw_rx_q, entries);
  GLOBAL_INT_RESTORE();

  *host_id = (uintptr_t)buf;
  return (uintptr_t)&buf->info;
}

void wl80211_mac_rx_init(void) {
  STAILQ_INIT(&macsw_rx_q);

  // TODO support dynamic alloc buffer
  for (int i = 0; i < CO_ARRAY_SIZE(rx_buf_mem); i++) {
    STAILQ_INSERT_TAIL(&macsw_rx_q, &rx_buf_mem[i], entries);
  }
  macif_rx_buf_ind();
}
