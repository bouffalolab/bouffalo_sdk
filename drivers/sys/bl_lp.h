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

#define LP_HOOK(x, ...)           \
    if (&lp_hook_##x) {           \
        lp_hook_##x(__VA_ARGS__); \
    }

#endif
