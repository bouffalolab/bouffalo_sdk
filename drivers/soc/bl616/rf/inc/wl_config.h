#ifndef _PHY_CONFIG_H_
#define _PHY_CONFIG_H_

#ifndef WL_PROFILE_ON
#define WL_PROFILE_ON  (0)
#endif

#ifndef WL_API_RMEM_EN
#define WL_API_RMEM_EN (1)
#endif

// #define NX_MDM_VER 32
// #define NX_MAC_VER 21
// #define NX_PLF_VER 30
// #define NX_UF_EN 0

#ifndef WL_BB_CE
#define WL_BB_CE (0)
#endif

#if !defined(WL_WB03) && !defined(WL_BL618M) && !defined(WL_BL616)
#pragma message "None of WL_WB03/WL_BL618M/WL_BL616 defined, compile for BL616 by default"
#define WL_BL616  (1)
#define WL_BL618M (0)
#define WL_WB03   (0)
#elif defined(WL_WB03) && (WL_WB03 == 1)
// #pragma message "Compile WL driver for WB03"
#undef  WL_BL616
#undef  WL_BL618M
#define WL_BL616  (0)
#define WL_BL618M (0)
#elif defined(WL_BL618M) && (WL_BL618M == 1)
// #pragma message "Compile WL driver for BL618M"
#undef  WL_WB03
#undef  WL_BL616
#define WL_WB03   (0)
#define WL_BL616  (0)
#else
// #pragma message "Compile WL driver for BL616"
#undef  WL_WB03
#undef  WL_BL618M
#undef  WL_BL616
#define WL_WB03    (0)
#define WL_BL618M  (0)
#define WL_BL616   (1)
#endif

#ifndef WL_BL616
#define WL_BL616  (1)
#define WL_BL618M (0)
#define WL_WB03   (0)
#endif

#ifndef WL_BB_LOG_EN
#define WL_BB_LOG_EN (0)
#endif

#ifndef WL_RF_LOG_EN
#define WL_RF_LOG_EN (0)
#endif

#if WL_BB_LOG_EN
#if WL_API_SDK
#include <stdio.h>
#define logprintf printf
// #warning ("PHYRF LOG is enabled")
#else
void dbg_test_print(const char *fmt, ...);
#define logprintf(fmt, ...)   dbg_test_print(fmt, ## __VA_ARGS__)
#endif
#else
#define logprintf(...)
#endif

// TODO
#define PHY_GLOBAL_INT_DISABLE()    
#define PHY_GLOBAL_INT_RESTORE()

#define PHY_MAX(a,b)                ((a) > (b) ? (a) : (b))
#define PHY_MIN(a,b)                ((a) < (b) ? (a) : (b))
#define PHY_ABS(a)                  ((a) < (0) ? (-a) : (a))

// #define CFG_FPGA                    (1)
// #define CFG_RF_ICAL                 (1)

#define DBG_PHY                     "[PHY]"
#define DBG_RFC                     "[RFC]"
#define DBG_RF                      "[RF] "
#define DBG_ERR                     "[ERROR]"

#define D_CRT       "\x9A"   ///< Prefix for critical
#define D_ERR       "\x9B"   ///< Prefix for error
#define D_WRN       "\x9C"   ///< Prefix for warning
#define D_INF       "\x9D"   ///< Prefix for info
#define D_VRB       "\x9E"   ///< Prefix for verbose debug
#define D_PHY       "\x87"   ///< Prefix for Modem / RF

#endif
