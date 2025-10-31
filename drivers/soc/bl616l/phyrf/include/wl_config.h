#ifndef _PHY_CONFIG_H_
#define _PHY_CONFIG_H_

#ifndef WL_PROFILE_ON
#define WL_PROFILE_ON  (0)
#endif

// #define NX_MDM_VER 32
// #define NX_MAC_VER 21
// #define NX_PLF_VER 30
// #define NX_UF_EN 0

#ifndef WL_BB_CE
#define WL_BB_CE (1)
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

#if 0
#ifndef WL_BB_LOG_EN
#define WL_BB_LOG_EN (0)
#endif

#ifndef WL_RF_LOG_EN
#define WL_RF_LOG_EN (0)
#endif

#if WL_BB_LOG_EN
#if WL_API_SDK
#include <stdio.h>
#define wl_logprintf printf
// #warning ("PHYRF LOG is enabled")
#else
void dbg_test_print(const char *fmt, ...);
#define wl_logprintf(fmt, ...)   dbg_test_print(fmt, ## __VA_ARGS__)
#endif
#else
#ifdef CONFIG_BL_MP
void mfg_print(const char *fmt, ...);
#define logprintf  mfg_print
#define wl_logprintf  mfg_print
#else
#define wl_logprintf(...)
#endif
#endif
#endif
#include <stdio.h>
#ifdef CONFIG_WLAN_PT_MAC
#define wl_logprintf(fmt, ...)
#else
#if defined(CFG_MFG_SDIO) || defined(CFG_MFG_USB) 
extern void wl_logprintf(const char *fmt, ...);
#else
#define wl_logprintf(fmt, ...) do { (void)printf(fmt, ##__VA_ARGS__); } while(0)
#endif
#endif

// TODO
#define PHY_GLOBAL_INT_DISABLE()    
#define PHY_GLOBAL_INT_RESTORE()

#define PHY_MAX(a,b)                ((a) > (b) ? (a) : (b))
#define PHY_MIN(a,b)                ((a) < (b) ? (a) : (b))
#define PHY_ABS(a)                  ((a) < (0) ? (-a) : (a))

#define DBG_PHY                     "[PHY]"
#define DBG_RFC                     "[RFC]"
#define DBG_RF                      "[RF] "
#define DBG_ERR                     "[ERROR]"
#endif
