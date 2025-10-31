/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __PROXY_H__
#define __PROXY_H__

#define BT_MESH_PROXY_NET_PDU   0x00
#define BT_MESH_PROXY_BEACON    0x01
#define BT_MESH_PROXY_CONFIG    0x02
#define BT_MESH_PROXY_PROV      0x03


struct bt_mesh_proxy_idle_cb {
	sys_snode_t n;
	void (*cb)(void);
};

int bt_mesh_proxy_send(struct bt_conn *conn, u8_t type,
		       struct net_buf_simple *msg);

int bt_mesh_proxy_prov_enable(void);
int bt_mesh_proxy_prov_disable(bool disconnect);

int bt_mesh_proxy_gatt_enable(void);
int bt_mesh_proxy_gatt_disable(void);
void bt_mesh_proxy_gatt_disconnect(void);

void bt_mesh_proxy_beacon_send(struct bt_mesh_subnet *sub);

struct net_buf_simple *bt_mesh_proxy_get_buf(void);

s32_t bt_mesh_proxy_adv_start(void);
void bt_mesh_proxy_adv_stop(void);

#if defined(CONFIG_BT_MESH_V1d1)
void bt_mesh_proxy_identity_start(struct bt_mesh_subnet *sub, bool private);
#else
void bt_mesh_proxy_identity_start(struct bt_mesh_subnet *sub);
#endif /* CONFIG_BT_MESH_V1d1 */
void bt_mesh_proxy_identity_stop(struct bt_mesh_subnet *sub);

bool bt_mesh_proxy_relay(struct net_buf_simple *buf, u16_t dst);
void bt_mesh_proxy_addr_add(struct net_buf_simple *buf, u16_t addr);

int bt_mesh_proxy_init(void);
void bt_mesh_proxy_on_idle(struct bt_mesh_proxy_idle_cb *cb);

#endif /*__PROXY_H__*/
