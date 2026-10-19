#ifndef LWIP_HDR_LWIPOPTS_SEL_H__
#define LWIP_HDR_LWIPOPTS_SEL_H__

#if IS_ENABLED(CONFIG_WL80211)
#include "lwipopts_user_80211.h"
#elif IS_ENABLED(CONFIG_FHOST)
#include "lwipopts_user_fhost.h"
#else
#error "Unknown Wi-Fi configuration"
#endif

#endif