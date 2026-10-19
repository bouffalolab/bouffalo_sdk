#include <assert.h>
#include <stdint.h>
#include <stdarg.h>

#include "macsw.h"

#include "fhost_rx_def.h"
#include "wifi_mgmr.h"

// MACSW_MAX_BA_RX is not defined by macsw.h; derive from config value
#ifdef CFG_BARX
#define MACSW_MAX_BA_RX CFG_BARX
#else
#define MACSW_MAX_BA_RX 1
#endif

  /// RX Packet Reordering Buffer Size
  #define MACSW_AMPDU_RX_BUF_SIZE CFG_REORD_BUF
  #if (MACSW_AMPDU_RX && ((MACSW_AMPDU_RX_BUF_SIZE < 4) || (MACSW_AMPDU_RX_BUF_SIZE > 64)))
     #error "Incorrect reordering buffer size"
  #endif

/// Number of RX buffers
#if MACSW_AMPDU_RX
#ifndef FHOST_RX_BUF_CNT
#ifndef CFG_ADHOC_ENABLE
#define FHOST_RX_BUF_CNT            (MACSW_MAX_BA_RX * MACSW_AMPDU_RX_BUF_SIZE + 2)
#else
#define FHOST_RX_BUF_CNT            (MACSW_AMPDU_RX_BUF_SIZE + 2)
#endif
#endif
#else
#define FHOST_RX_BUF_CNT             4
#endif

MACSW_CONFIG_DEF(uint16_t, FHOST_RX_BUF_CNT);

#ifdef CONFIG_FHOST_RX_BUF_SECTION
#define __FHOST_RX_SECTION __attribute__((section(CONFIG_FHOST_RX_BUF_SECTION)))
#else
#define __FHOST_RX_SECTION __SHAREDRAM
#endif
struct fhost_rx_buf_tag fhost_rx_buf_mem[FHOST_RX_BUF_CNT] __FHOST_RX_SECTION;

#if MACSW_UF_EN
struct fhost_rx_uf_buf_tag fhost_rx_uf_buf_mem[FHOST_RX_BUF_CNT] __SHAREDRAM;
#endif

#ifndef CONFIG_WIFI_MGMR_SCAN_ITEMS_MAX
#ifdef CONFIG_PSRAM
#define CONFIG_WIFI_MGMR_SCAN_ITEMS_MAX 100
#else
#define CONFIG_WIFI_MGMR_SCAN_ITEMS_MAX 50
#endif
#endif

#if CONFIG_WIFI_MGMR_SCAN_ITEMS_MAX < 1 || CONFIG_WIFI_MGMR_SCAN_ITEMS_MAX > 100
#error "CONFIG_WIFI_MGMR_SCAN_ITEMS_MAX must be between 1 and 100"
#endif

#ifdef CONFIG_PSRAM
#define WIFI_MGMR_SCAN_ITEMS_ATTR __attribute__((section(".psram_noinit"), aligned(32)))
#else
#define WIFI_MGMR_SCAN_ITEMS_ATTR
#endif

const uint16_t wifi_mgmr_scan_items_count = CONFIG_WIFI_MGMR_SCAN_ITEMS_MAX;
wifi_mgmr_scan_item_t wifi_mgmr_scan_items[CONFIG_WIFI_MGMR_SCAN_ITEMS_MAX] WIFI_MGMR_SCAN_ITEMS_ATTR;
