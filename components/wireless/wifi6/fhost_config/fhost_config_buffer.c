#include <assert.h>
#include <stdint.h>
#include <stdarg.h>

#include "macsw.h"
#include "fhost_rx_def.h"

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
