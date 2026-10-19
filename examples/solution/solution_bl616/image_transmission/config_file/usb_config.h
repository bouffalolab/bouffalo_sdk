/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef SOLN_IMAGE_TRANSMISSION_USB_CONFIG_H
#define SOLN_IMAGE_TRANSMISSION_USB_CONFIG_H

#include "usb_uvc_uac_config_template.h"

/* image_transmission-specific USB overrides */
#undef CONFIG_USB_PRINTF
#define CONFIG_USB_PRINTF(...) printf(__VA_ARGS__)

#undef CONFIG_USB_DBG_LEVEL
#define CONFIG_USB_DBG_LEVEL USB_DBG_INFO

#undef CONFIG_USB_PRINTF_COLOR_ENABLE
#define CONFIG_USB_PRINTF_COLOR_ENABLE

#ifdef CONFIG_USB_DCACHE_ENABLE
#undef CONFIG_USB_ALIGN_SIZE
#define CONFIG_USB_ALIGN_SIZE 32
#else
#undef CONFIG_USB_ALIGN_SIZE
#define CONFIG_USB_ALIGN_SIZE 4
#endif

#endif /* SOLN_IMAGE_TRANSMISSION_USB_CONFIG_H */
