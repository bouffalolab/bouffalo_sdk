/** @file
 *  @brief GATT Device Information Service
 */

/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _DIS_H_
#define _DIS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define AR_VENDOR_ID    0x0001
#define AR_PRODUCT_ID   0x0002

enum{
    DIS_PNP_VID_SRC = 0x01,
    USB_IMPL_VID    
};
void dis_init(u8_t vid_src, u16_t vid, u16_t pid, u16_t pnp_ver);
void dis_deinit(void);

#if defined(CONFIG_BT_SETTINGS) && defined(CONFIG_BT_GATT_DIS_SETTINGS)

#if defined(BFLB_BLE)
int dis_settings_init(const char *name, char* setting,size_t setting_len);
#endif

#endif

#ifdef __cplusplus
}
#endif
#endif
