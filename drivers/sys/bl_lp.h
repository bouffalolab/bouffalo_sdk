#ifndef __BL_LP_H__
#define __BL_LP_H__
#include <stdint.h>

#if defined(BL702)
#include "bl702_lp.h"
#elif defined(BL602)
#include "bl602_lp.h"
#elif defined(BL702L)
#include "bl702l_lp.h"
#elif defined(BL616)
#include "bl616_lp.h"
#elif defined(BL606P)
#include "bl606p_lp.h"
#elif defined(BL808)
#include "bl808_lp.h"
#elif defined(BL628)
#include "bl628_lp.h"
#elif defined(BL616L)
#include "bl616l_lp.h"
#elif defined(BL616D)
#include "bl616d_lp.h"
#endif

#ifdef SHARED_FUNC_EN
    #define SHARE_HOOK(func_name, ...)   shared_##func_name(__VA_ARGS__)
#else
    #define SHARE_HOOK(func_name, ...)   func_name(__VA_ARGS__)
#endif

#define LP_HOOK(x, ...)           \
    if (&lp_hook_##x) {           \
        lp_hook_##x(__VA_ARGS__); \
    }

/* Cause of wakeup */
#define LPFW_WAKEUP_UNKOWN              (0)
#define LPFW_WAKEUP_TIME_OUT            (1 << 0)
#define LPFW_WAKEUP_WIFI                (1 << 1)
#define LPFW_WAKEUP_WIFI_BROADCAST      (1 << 2)
#define LPFW_WAKEUP_AP_LOSS             (1 << 3)
#define LPFW_WAKEUP_IO                  (1 << 4)
#define LPFW_WAKEUP_ACOMP               (1 << 5)
#define LPFW_WAKEUP_BLE                 (1 << 6)
#define LPFW_WAKEUP_LOSS_CFG_OVER       (1 << 7)


/* Conversion of units */
#define BL_US_TO_PDS_CNT(us)  ((us) * 512 / 15625)  /* us * 32768 / 1000 /1000 */
#define BL_MS_TO_PDS_CNT(ms)  ((ms) * 4096 / 125)   /* ms * 32768 / 1000 */
#define BL_PDS_CNT_TO_US(cnt) ((cnt) * 15625 / 512) /* cnt / 32768 * 1000 /1000 */
#define BL_PDS_CNT_TO_MS(cnt) ((cnt) * 125 / 4096)  /* cnt / 32768 * 1000 */

#endif
