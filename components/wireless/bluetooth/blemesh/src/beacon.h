/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BEACON_H__
#define __BEACON_H__

void bt_mesh_beacon_enable(void);
void bt_mesh_beacon_disable(void);

void bt_mesh_beacon_ivu_initiator(bool enable);

void bt_mesh_beacon_recv(struct net_buf_simple *buf);
#if defined(CONFIG_BT_MESH_V1d1)
int bt_mesh_net_beacon_update(struct bt_mesh_subnet *sub);
#endif /* CONFIG_BT_MESH_V1d1 */
#if defined(CONFIG_BT_MESH_V1d1)
int bt_mesh_beacon_create(struct bt_mesh_subnet *sub,
			   struct net_buf_simple *buf, bool priv);
#else
void bt_mesh_beacon_create(struct bt_mesh_subnet *sub,
			   struct net_buf_simple *buf);
#endif /* CONFIG_BT_MESH_V1d1 */

void bt_mesh_beacon_init(void);

#endif /*__BEACON_H__*/

