#ifndef FHOST_WDS_H
#define FHOST_WDS_H
/* WDS declarations live here. A release drops fhost/src, and customer wpa and the adapter still include this header. */

#include <stdbool.h>
#include <stdint.h>

#include "wifi_wds.h"

struct fhost_vif_tag;
struct mac_addr;

bool fhost_wds_enabled(struct fhost_vif_tag *vif);
bool fhost_wds_allow_normal_sta(struct fhost_vif_tag *vif);
/* Multi-AP callbacks may run before the peer appears in the MACSW STA table. */
bool fhost_wds_auto_enabled(uint8_t vif_idx, wifi_wds_role_t role);
int fhost_wds_auto_peer_available(uint8_t vif_idx, const uint8_t peer_mac[6]);
/* Does not bind. Call before the driver deletes the old STA entry. */
int fhost_wds_auto_sta_admit(uint8_t vif_idx, const uint8_t peer_mac[6],
                             bool backhaul);
int fhost_wds_auto_peer_bind(uint8_t vif_idx, const uint8_t peer_mac[6],
                             bool enable);
/* peer returns INVALID_STA_IDX until the configured peer is authorized. */
uint8_t fhost_wds_peer(struct fhost_vif_tag *vif);
/* is_peer also identifies the configured association during WPA handshake. */
bool fhost_wds_is_peer(struct fhost_vif_tag *vif, uint8_t sta_idx);
uint8_t fhost_wds_tx_select(struct fhost_vif_tag *vif,
                           const struct mac_addr *da, uint8_t ordinary_sta_idx);
bool fhost_wds_tx_4addr(struct fhost_vif_tag *vif, uint8_t sta_idx);
void fhost_wds_tx_record(struct fhost_vif_tag *vif);
/* Call after MACSW security/reorder checks, separately for each A-MSDU child. */
bool fhost_wds_rx_accept(struct fhost_vif_tag *vif, uint8_t sta_idx,
                          bool four_addr, bool local_control);
void fhost_wds_learn(struct fhost_vif_tag *vif, uint8_t sta_idx,
                       const uint8_t sa[6]);
/* These lifecycle hooks preserve configuration and cumulative counters. */
void fhost_wds_link_reset(struct fhost_vif_tag *vif);
void fhost_wds_sta_removed(uint8_t mac_vif_idx, uint8_t sta_idx,
                           const uint8_t peer_mac[6]);
/* Caller holds rtos_protect(). ap_netif_busy is the manager AP-netif check. */
int fhost_wds_config_set_locked(uint8_t vif_idx, const wifi_wds_config_t *config,
                                bool ap_netif_busy);
int fhost_wds_config_get(uint8_t vif_idx, wifi_wds_config_t *config);
int fhost_wds_status_get(uint8_t vif_idx, wifi_wds_status_t *status);
int fhost_wds_fdb_get(uint8_t vif_idx, uint16_t start,
                      wifi_wds_fdb_entry_t *entries,
                      uint16_t capacity, uint16_t *count);
int fhost_wds_fdb_flush(uint8_t vif_idx);

#endif /* FHOST_WDS_H */
