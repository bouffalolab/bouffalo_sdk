#ifndef __MACSW_CONFIG_H__
#define __MACSW_CONFIG_H__

#define CFG_RTOS
#define CFG_FHOST
#define CFG_UMAC
#define CFG_STATS
#define CFG_WIFI_DBG 2
#define CFG_REC
#define CFG_COEX
#define CFG_BCN
#define CFG_AGG
#define CFG_AMSDU_4K

#define CFG_VIF_MAX 2
#define CFG_STA_MAX 4
#define CFG_SPC 16

#define CFG_TXDESC0 1
#define CFG_TXDESC1 32
#define CFG_TXDESC2 1
#define CFG_TXDESC3 1
#define CFG_TXDESC4 4

#define CFG_MAC_VER_V21
#define CFG_MDM_VER_V30
#define CFG_PLF_VER_V30

#define CFG_PS

#define CFG_MU_CNT 1
#define CFG_KEYCFG
#define CFG_P2P 0
#define CFG_P2P_GO
#define CFG_BWLEN
#define CFG_HE

#define CFG_TWT 0
#define CFG_BARX 2
#define CFG_BATX 1
#define CFG_REORD_BUF 12

#define CFG_UF 1
#define CFG_MFP 1
#define CFG_FTM_INIT 1
#define CFG_FTM_RSP 1
#define CFG_HSU 0
#define CFG_CRYPTO
#define CFG_FHOST_MONITOR
#define TX_RETRY_ENABLE 1

#define WIFI_STATISTIC_ENABLE 0

#ifdef BL616L
#define CFG_PLATFORM_DMA_ENABLE 0
#else
#define CFG_PLATFORM_DMA_ENABLE 1
#endif

#if defined(MACSW_CONFIG_SELECT)
#include MACSW_CONFIG_SELECT
#endif

#ifndef CFG_BL_WIFI_PS_ENABLE
#undef CFG_BL_WIFI_PS_ENABLE
#endif

/* XXX hacks start */
// 这些宏需要简化成1-2个。
//#define CFG_LPM
//#define CFG_HE_TB ??
#ifndef RC_EZ23Q4
#define RC_EZ23Q4 0
#endif

#ifndef RC_EZ23Q4_BCN
#define RC_EZ23Q4_BCN 0
#endif

#ifndef WL_BB_TPC
#define WL_BB_TPC 1
#endif

#ifndef WL_BB_PD
#define WL_BB_PD 1
#endif

#ifndef WL_BB_CE
#define WL_BB_CE 0
#endif

#ifndef WL_NIC
#define WL_NIC 0
#endif

#ifndef BL_RA_EN
#define BL_RA_EN 0
#endif

#ifndef WL_BB_TPC_LOG
#define WL_BB_TPC_LOG 0 // TODO REMOVE
#endif

#ifndef CFG_RC_BL_CHANGE
#define CFG_RC_BL_CHANGE 0
#endif

#ifndef BL_HD_RETRY_MAX
#define BL_HD_RETRY_MAX 0
#endif

#ifndef BL_WIFI_LP_FW
#define BL_WIFI_LP_FW 0
#endif
/* hacks end */

/* coex */
#ifndef CONFIG_COEX_BT_MODE
#define CONFIG_COEX_BT_MODE 0
#endif

#ifndef CONFIG_COEX_WIFI_MODE
#define CONFIG_COEX_WIFI_MODE 0
#endif

#ifndef CONFIG_COEX_THREAD_MOD
#define CONFIG_COEX_THREAD_MOD 0
#endif

#ifndef CONFIG_COEX_TDMA_NONE
#define CONFIG_COEX_TDMA_NONE 0
#endif
/* coex end */

#endif
