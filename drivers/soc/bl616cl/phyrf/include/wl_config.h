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

#include <stdio.h>
#if defined(CFG_MFG_SDIO) || defined(CFG_MFG_USB) 
extern void wl_logprintf(const char *fmt, ...);
#else
#define wl_logprintf(fmt, ...) do { (void)printf(fmt, ##__VA_ARGS__); } while(0)
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
