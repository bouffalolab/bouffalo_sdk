/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if defined(BFLB_BLE)
#include <sys/types.h>
#include "addr.h"
#endif

/* Max settings key length (with all components) */
#define BT_SETTINGS_KEY_MAX 36

/* Base64-encoded string buffer size of in_size bytes */
#define BT_SETTINGS_SIZE(in_size) ((((((in_size) - 1) / 3) * 4) + 4) + 1)

/**
 * Function used to read the data from the settings storage in
 * h_set handler implementations.
 *
 * @param[in] cb_arg  arguments for the read function. Appropriate cb_arg is
 *                    transferred to h_set handler implementation by
 *                    the backend.
 * @param[out] data  the destination buffer
 * @param[in] len    length of read
 *
 * @return positive: Number of bytes read, 0: key-value pair is deleted.
 *                   On error returns -ERRNO code.
 */ /* Added by bouffalo */
typedef ssize_t (*settings_read_cb)(void *cb_arg, void *data, size_t len);

/* Helpers for keys containing a bdaddr */
void bt_settings_encode_key(char *path, size_t path_size, const char *subsys,
			    bt_addr_le_t *addr, const char *key);
int bt_settings_decode_key(const char *key, bt_addr_le_t *addr);

void bt_settings_save_id(void);

int bt_settings_init(void);

#if defined(BFLB_BLE)
#define NV_LOCAL_NAME      "LOCAL_NAME"
#define NV_LOCAL_ID_ADDR   "LOCAL_ID_ADDR"
#define NV_LOCAL_IRK       "LOCAL_IRK"
#define NV_KEY_POOL        "KEY_POOL"
#define NV_IMG_info        "IMG_INFO"
#define NV_ATVV_CCC        "ATVV_CCC"

int bt_settings_get_bin(const char *key, u8_t *value, size_t exp_len, size_t *real_len);
int bt_settings_set_bin(const char *key, const u8_t *value, size_t length);
int settings_delete(const char *key);
int settings_save_one(const char *key, const u8_t *value, size_t length);
void bt_settings_save_name(void);
void bt_local_info_load(void);
#endif
