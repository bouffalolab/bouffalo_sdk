#ifndef LVGL_USB_VIDEO_LWIPOPTS_USER_H
#define LVGL_USB_VIDEO_LWIPOPTS_USER_H

/* This example uses the NetHub USB profile, so reuse NetHub's lwIP profile
 * instead of the Wi-Fi6 adapter fallback that expects MACSW private macros. */
#include "../../wifi/nethub/lwipopts_user.h"

#endif /* LVGL_USB_VIDEO_LWIPOPTS_USER_H */
