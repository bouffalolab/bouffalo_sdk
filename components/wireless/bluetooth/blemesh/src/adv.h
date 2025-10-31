/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ADV_H__
#define __ADV_H__

/* Maximum advertising data payload for a single data type */
#define BT_MESH_ADV_DATA_SIZE 29

/* The user data is a pointer (4 bytes) to struct bt_mesh_adv */
#define BT_MESH_ADV_USER_DATA_SIZE 4

#define BT_MESH_ADV(buf) (*(struct bt_mesh_adv **)net_buf_user_data(buf))

enum bt_mesh_adv_type {
	BT_MESH_ADV_PROV,
	BT_MESH_ADV_DATA,
	BT_MESH_ADV_BEACON,
	BT_MESH_ADV_URI,
};

typedef void (*bt_mesh_adv_func_t)(struct net_buf *buf, u16_t duration,
				   int err, void *user_data);

struct bt_mesh_adv {
	const struct bt_mesh_send_cb *cb;
	void *cb_data;

	u8_t      type:2,
		  busy:1;
	u8_t      xmit;
#if defined(CONFIG_BT_MESH_V1d1) && defined(CONFIG_AUTO_PTS)
	uint16_t options;
#endif /* CONFIG_BT_MESH_V1d1 && CONFIG_AUTO_PTS */
    #if defined(CONFIG_BLE_MULTI_ADV)
    struct k_delayed_work	d_work;
    int adv_id;
    struct net_buf* buf;
    #endif /* CONFIG_BLE_MULTI_ADV */
};

typedef struct bt_mesh_adv *(*bt_mesh_adv_alloc_t)(int id);

/* xmit_count: Number of retransmissions, i.e. 0 == 1 transmission */
struct net_buf *bt_mesh_adv_create(enum bt_mesh_adv_type type, u8_t xmit,
				   s32_t timeout);
#if defined(CONFIG_BT_MESH_V1d1) && defined(CONFIG_AUTO_PTS)
struct net_buf *bt_mesh_adv_create_with_options(enum bt_mesh_adv_type type, u8_t xmit,
				   s32_t timeout, u16_t options);
#endif /* CONFIG_BT_MESH_V1d1 && CONFIG_AUTO_PTS */

struct net_buf *bt_mesh_adv_create_from_pool(struct net_buf_pool *pool,
					     bt_mesh_adv_alloc_t get_id,
					     enum bt_mesh_adv_type type,
					     u8_t xmit, s32_t timeout);

void bt_mesh_adv_send(struct net_buf *buf, const struct bt_mesh_send_cb *cb,
		      void *cb_data);

void bt_mesh_adv_update(void);

void bt_mesh_adv_init(void);

int bt_mesh_scan_enable(void);

int bt_mesh_scan_disable(void);

#endif /*__ADV_H__*/
