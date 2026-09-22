#pragma once

#include <stdint.h>
#include <stdbool.h>

#define WIFI_MGMR_PMKID_LEN 16
#define WIFI_MGMR_PMK_LEN 32
#define WIFI_MGMR_ETH_ALEN 6
#define WIFI_MGMR_PMK_VIF_UNKNOWN (-1)

#define WIFI_MGMR_PMK_ATTR_WEAK __attribute__((weak))

typedef struct {
    uint8_t pmkid[WIFI_MGMR_PMKID_LEN];
    uint8_t pmk[WIFI_MGMR_PMK_LEN];
    uint8_t aa[WIFI_MGMR_ETH_ALEN];
} wifi_mgmr_pmk_cache_entry;

struct wifi_mgmr_pmk_tag {
    bool cache_entry_valid;
    int owner_vif_idx;
    const void *network_ctx;
    wifi_mgmr_pmk_cache_entry entry;
};

/**
 * @brief Update PMK cache entry to wifi_mgmr.
 *
 * @return 0 on success, -1 on error or 1 if existing entry is the same as provided
 */
int wifi_mgmr_pmk_cache_entry_update(const wifi_mgmr_pmk_cache_entry *entry);

/**
 * @brief Update a cache entry and bind it to a VIF and network context.
 */
int wifi_mgmr_pmk_cache_entry_update_for_network(
    const wifi_mgmr_pmk_cache_entry *entry, int owner_vif_idx,
    const void *network_ctx);

/**
 * @brief Restore PMK cache entry to wifi_mgmr.
 *
 * @return 0 on success, -1 on error or 1 if existing entry is the same as provided
 */
int wifi_mgmr_pmk_cache_entry_restore(const wifi_mgmr_pmk_cache_entry *entry);

/**
 * @brief Read PMK cache entry from wifi_mgmr.
 *
 * @return 0 on success, -1 if entry == NULL or PMK entry is invalid
 */
int wifi_mgmr_pmk_cache_entry_read(wifi_mgmr_pmk_cache_entry *entry);

/**
 * @brief Read and, if restored from storage, bind a matching cache entry.
 */
int wifi_mgmr_pmk_cache_entry_read_for_network(
    wifi_mgmr_pmk_cache_entry *entry, int owner_vif_idx,
    const void *network_ctx, const uint8_t *aa);

/**
 * @brief Invalidate the PMK cache entry in wifi_mgmr.
 */
void wifi_mgmr_pmk_cache_entry_invalidate(void);

/**
 * @brief Invalidate only an entry owned by the requested VIF/network/BSSID.
 *
 * NULL network_ctx and aa values act as wildcards. Returns 1 when an entry
 * was invalidated and 0 when there was no matching entry.
 */
int wifi_mgmr_pmk_cache_entry_invalidate_for_network(
    int owner_vif_idx, const void *network_ctx, const uint8_t *aa);

/**
 * @brief App hook called after PMK cache entry is updated.
 * App may define the function to save PMK to NVRAM.
 * The function is called in wpa_supplicant task.
 */
void wifi_mgmr_pmk_cache_entry_updated(void);

/**
 * @brief App hook called after PMK cache entry is invalidated.
 * App may define the function to remove PMK from NVRAM.
 * The function is called in wpa_supplicant task.
 */
void wifi_mgmr_pmk_cache_entry_invalidated(void);
