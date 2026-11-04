#ifndef WIFI_WDS_H
#define WIFI_WDS_H
/* WDS role, peer, and FDB. The table holds at most 64 Ethernet hosts and ages entries after 5 minutes. */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_WDS_FDB_CAPACITY 64
#define WIFI_WDS_FDB_AGE_MS 300000U
#define WIFI_WDS_INVALID_STA_IDX 0xff

typedef enum {
    WIFI_WDS_ROLE_DISABLED = 0,
    WIFI_WDS_ROLE_AP,
    WIFI_WDS_ROLE_STA,
} wifi_wds_role_t;

typedef enum {
    WIFI_WDS_PEER_STATIC_MAC = 0,
    WIFI_WDS_PEER_AUTO_MULTI_AP,
} wifi_wds_peer_mode_t;

enum wifi_wds_error {
    WIFI_WDS_OK = 0,
    WIFI_WDS_ERR_INVALID_ARGUMENT = -1,
    WIFI_WDS_ERR_UNSUPPORTED = -2,
    WIFI_WDS_ERR_BUSY = -3,
    WIFI_WDS_ERR_NOT_READY = -4,
};

typedef struct {
    /* 0 keeps ordinary three-address frames. The released library implements both. */
    uint8_t enable;
    wifi_wds_role_t role;
    wifi_wds_peer_mode_t peer_mode;
    /* Static mode: AP uses wireless STA MAC; STA uses AP BSSID.
     * Auto mode: leave zero; the Multi-AP association resolves the peer. */
    uint8_t peer_mac[6];
    uint8_t allow_normal_sta;
} wifi_wds_config_t;

typedef struct {
    uint8_t enabled;
    wifi_wds_role_t role;
    uint8_t link_up;
    uint8_t peer_associated;
    uint8_t peer_mac_match;
    uint8_t tx_4addr_ready;
    uint8_t rx_4addr_seen;
    /* Local data path ready; this does not imply remote WDS negotiation. */
    uint8_t active;
    /* Auto mode: candidate was announced but is not yet authorized. */
    uint8_t discovery_pending;
    /* Associated wireless peer, zero until a candidate is resolved. */
    uint8_t resolved_peer_mac[6];
    uint8_t peer_sta_idx;
    uint16_t fdb_count;
    /* Ethernet frames submitted/accepted, including A-MSDU subframes. */
    uint32_t tx_4addr;
    uint32_t rx_4addr;
    uint32_t drop_unexpected_peer;
    uint32_t drop_3addr_on_wds;
    int32_t last_error;
} wifi_wds_status_t;

typedef struct {
    uint8_t mac[6];
    uint8_t peer_mac[6];
    uint8_t peer_sta_idx;
    uint32_t age_ms;
} wifi_wds_fdb_entry_t;

/* Calls are wifi_mgmr_wds_config_set() and the matching get, status, and FDB
 * functions in wifi_mgmr_ext.h. Configure only while the AP is stopped or the
 * STA is disconnected. The VIF index is a FHOST index. Only one VIF may enable
 * WDS at a time. FDB start is the ordinal of the first live entry. */

#ifdef __cplusplus
}
#endif

#endif /* WIFI_WDS_H */
