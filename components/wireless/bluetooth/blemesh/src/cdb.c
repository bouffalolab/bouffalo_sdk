/*
 * Copyright (c) 2019 Tobias Svehagen
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <string.h>

#include <include/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_CDB)
#define LOG_MODULE_NAME bt_mesh_cdb
#include "bt_log.h"

#include "mesh.h"
#include "net.h"
#include "mesh_settings.h"

#include "include/cdb.h"
#include "bt_errno.h"


struct bt_mesh_cdb bt_mesh_cdb = {
	.nodes = {
		[0 ... (CONFIG_BT_MESH_CDB_NODE_COUNT - 1)] = {
			.addr = BT_MESH_ADDR_UNASSIGNED,
		}
	},
	.subnets = {
		[0 ... (CONFIG_BT_MESH_CDB_SUBNET_COUNT - 1)] = {
			.net_idx = BT_MESH_KEY_UNUSED,
		}
	},
	.app_keys = {
		[0 ... (CONFIG_BT_MESH_CDB_APP_KEY_COUNT - 1)] = {
			.net_idx = BT_MESH_KEY_UNUSED,
		}
	},
};

/*
 * Check if an address range from addr_start for addr_start + num_elem - 1 is
 * free for use. When a conflict is found, next will be set to the next address
 * available after the conflicting range and -EAGAIN will be returned.
 */
static int addr_is_free(u16_t addr_start, u8_t num_elem, u16_t *next)
{
	u16_t addr_end = addr_start + num_elem - 1;
	u16_t other_start, other_end;
	int i;

	if (!BT_MESH_ADDR_IS_UNICAST(addr_start) ||
	    !BT_MESH_ADDR_IS_UNICAST(addr_end) ||
	    num_elem == 0) {
		return -EINVAL;
	}

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.nodes); i++) {
		struct bt_mesh_cdb_node *node = &bt_mesh_cdb.nodes[i];

		if (node->addr == BT_MESH_ADDR_UNASSIGNED) {
			continue;
		}

		other_start = node->addr;
		other_end = other_start + node->num_elem - 1;

		if (!(addr_end < other_start || addr_start > other_end)) {
			if (next) {
				*next = other_end + 1;
			}

			return -EAGAIN;
		}
	}

	return 0;
}

/*
 * Find the lowest possible starting address that can fit num_elem elements. If
 * a free address range cannot be found, BT_MESH_ADDR_UNASSIGNED will be
 * returned. Otherwise the first address in the range is returned.
 *
 * NOTE: This is quite an ineffective algorithm as it might need to look
 *       through the array of nodes N+2 times. A more effective algorithm
 *       could be used if the nodes were stored in a sorted list.
 */
static u16_t find_lowest_free_addr(u8_t num_elem)
{
	u16_t addr = 1, next;
	int err, i;

	/*
	 * It takes a maximum of node count + 2 to find a free address if there
	 * is any. +1 for our own address and +1 for making sure that the
	 * address range is valid.
	 */
	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.nodes) + 2; ++i) {
		err = addr_is_free(addr, num_elem, &next);
		if (err == 0) {
			break;
		} else if (err != -EAGAIN) {
			addr = BT_MESH_ADDR_UNASSIGNED;
			break;
		}

		addr = next;
	}

	return addr;
}

int bt_mesh_cdb_create(const u8_t key[16])
{
	struct bt_mesh_cdb_subnet *sub;

	if (atomic_test_and_set_bit(bt_mesh_cdb.flags,
				    BT_MESH_CDB_VALID)) {
		return -EALREADY;
	}

	sub = bt_mesh_cdb_subnet_alloc(BT_MESH_KEY_PRIMARY);
	if (sub == NULL) {
		return -ENOMEM;
	}

	memcpy(sub->keys[0].net_key, key, 16);
	bt_mesh_cdb.iv_index = 0;

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_cdb();
		bt_mesh_store_cdb_subnet(sub);
	}

	return 0;
}

void bt_mesh_cdb_clear(void)
{
	int i;

	atomic_clear_bit(bt_mesh_cdb.flags, BT_MESH_CDB_VALID);

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.nodes); ++i) {
		if (bt_mesh_cdb.nodes[i].addr != BT_MESH_ADDR_UNASSIGNED) {
			bt_mesh_cdb_node_del(&bt_mesh_cdb.nodes[i], true);
		}
	}

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.subnets); ++i) {
		if (bt_mesh_cdb.subnets[i].net_idx != BT_MESH_KEY_UNUSED) {
			bt_mesh_cdb_subnet_del(&bt_mesh_cdb.subnets[i], true);
		}
	}

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.app_keys); ++i) {
		if (bt_mesh_cdb.app_keys[i].net_idx != BT_MESH_KEY_UNUSED) {
			bt_mesh_cdb_app_key_del(&bt_mesh_cdb.app_keys[i], true);
		}
	}

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_cdb();
	}
}

void bt_mesh_cdb_iv_update(u32_t iv_index, bool iv_update)
{
	BT_DBG("Updating IV index to %d\n", iv_index);

	bt_mesh_cdb.iv_index = iv_index;

	atomic_set_bit_to(bt_mesh_cdb.flags, BT_MESH_CDB_IVU_IN_PROGRESS,
			  iv_update);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_cdb();
	}
}

struct bt_mesh_cdb_subnet *bt_mesh_cdb_subnet_alloc(u16_t net_idx)
{
	struct bt_mesh_cdb_subnet *sub;
	int i;

	if (bt_mesh_cdb_subnet_get(net_idx) != NULL) {
		return NULL;
	}

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.subnets); ++i) {
		sub = &bt_mesh_cdb.subnets[i];

		if (sub->net_idx != BT_MESH_KEY_UNUSED) {
			continue;
		}

		sub->net_idx = net_idx;

		return sub;
	}

	return NULL;
}

void bt_mesh_cdb_subnet_del(struct bt_mesh_cdb_subnet *sub, bool store)
{
	BT_DBG("NetIdx 0x%03x store %u", sub->net_idx, store);

	if (IS_ENABLED(CONFIG_BT_SETTINGS) && store) {
		bt_mesh_clear_cdb_subnet(sub);
	}

	sub->net_idx = BT_MESH_KEY_UNUSED;
	memset(sub->keys, 0, sizeof(sub->keys));
}

struct bt_mesh_cdb_subnet *bt_mesh_cdb_subnet_get(u16_t net_idx)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.subnets); ++i) {
		if (bt_mesh_cdb.subnets[i].net_idx == net_idx) {
			return &bt_mesh_cdb.subnets[i];
		}
	}

	return NULL;
}

void bt_mesh_cdb_subnet_store(const struct bt_mesh_cdb_subnet *sub)
{
	bt_mesh_store_cdb_subnet(sub);
}

u8_t bt_mesh_cdb_subnet_flags(const struct bt_mesh_cdb_subnet *sub)
{
	u8_t flags = 0x00;

	if (sub && sub->kr_flag) {
		flags |= BT_MESH_NET_FLAG_KR;
	}

	if (atomic_test_bit(bt_mesh_cdb.flags, BT_MESH_CDB_IVU_IN_PROGRESS)) {
		flags |= BT_MESH_NET_FLAG_IVU;
	}

	return flags;
}

struct bt_mesh_cdb_node *bt_mesh_cdb_node_alloc(const u8_t uuid[16], u16_t addr,
						u8_t num_elem, u16_t net_idx)
{
	int i;

	if (addr == BT_MESH_ADDR_UNASSIGNED) {
		addr = find_lowest_free_addr(num_elem);
		if (addr == BT_MESH_ADDR_UNASSIGNED) {
			return NULL;
		}
	} else if (addr_is_free(addr, num_elem, NULL) < 0) {
		BT_DBG("Address range 0x%04x-0x%04x is not free", addr,
		       addr + num_elem - 1);
		return NULL;
	}

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.nodes); i++) {
		struct bt_mesh_cdb_node *node = &bt_mesh_cdb.nodes[i];

		if (node->addr == BT_MESH_ADDR_UNASSIGNED) {
			memcpy(node->uuid, uuid, 16);
			node->addr = addr;
			node->num_elem = num_elem;
			node->net_idx = net_idx;
			atomic_set(node->flags, 0);
			return node;
		}
	}
	/* Added by bouffalo */
	BT_WARN("Nodes are full");
	return NULL;
}

void bt_mesh_cdb_node_del(struct bt_mesh_cdb_node *node, bool store)
{
	BT_DBG("Node addr 0x%04x store %u", node->addr, store);

	if (IS_ENABLED(CONFIG_BT_SETTINGS) && store) {
		bt_mesh_clear_cdb_node(node);
	}

	node->addr = BT_MESH_ADDR_UNASSIGNED;
	memset(node->dev_key, 0, sizeof(node->dev_key));
}

#ifdef CONFIG_BT_MESH_NODE_SEND_CFGCLI_MSG
/* Added by bouffalo lab, to save other node's device key */
struct bt_mesh_cdb_node *bt_mesh_cdb_node_set_fornode(u16_t addr,u8_t num_elem,
						const u8_t devkey[16])
{
	for (int i = 0; i < ARRAY_SIZE(bt_mesh_cdb.nodes); i++) {
		struct bt_mesh_cdb_node *node = &bt_mesh_cdb.nodes[i];

		if (node->addr == BT_MESH_ADDR_UNASSIGNED) {
			memcpy(node->dev_key, devkey, 16);
			node->addr = addr;
			node->num_elem = num_elem;
			//atomic_set(node->flags, 0);
			if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
				bt_mesh_store_cdb_node(node);
			}
			return node;
		}
	}

	/* Added by bouffalo */
	BT_WARN("Nodes are full");
	return NULL;
}

/* Added by bouffalo lab, to clear other node's device key */
bool bt_mesh_cdb_node_clr_fornode(u16_t addr)
{
	for (int i = 0; i < ARRAY_SIZE(bt_mesh_cdb.nodes); i++) {
		struct bt_mesh_cdb_node *node = &bt_mesh_cdb.nodes[i];

		if (node->addr != BT_MESH_ADDR_UNASSIGNED
			&& node->addr == addr) {
			node->addr = BT_MESH_ADDR_UNASSIGNED;
			if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
				bt_mesh_store_cdb_node(node);
			}
			return true;
		}
	}

	return false;
}
#endif /* CONFIG_BT_MESH_NODE_SEND_CFGCLI_MSG */

struct bt_mesh_cdb_node *bt_mesh_cdb_node_get(u16_t addr)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.nodes); i++) {
		struct bt_mesh_cdb_node *node = &bt_mesh_cdb.nodes[i];

		if (addr >= node->addr &&
		    addr <= node->addr + node->num_elem - 1) {
			return node;
		}
	}

	return NULL;
}

void bt_mesh_cdb_node_store(const struct bt_mesh_cdb_node *node)
{
	bt_mesh_store_cdb_node(node);
}

void bt_mesh_cdb_node_foreach(bt_mesh_cdb_node_func_t func, void *user_data)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.nodes); ++i) {
		if (bt_mesh_cdb.nodes[i].addr == BT_MESH_ADDR_UNASSIGNED) {
			continue;
		}

		if (func(&bt_mesh_cdb.nodes[i], user_data) ==
		    BT_MESH_CDB_ITER_STOP) {
			break;
		}
	}
}

struct bt_mesh_cdb_app_key *bt_mesh_cdb_app_key_alloc(u16_t net_idx,
						      u16_t app_idx)
{
	struct bt_mesh_cdb_app_key *key;
	int i;

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.app_keys); ++i) {
		key = &bt_mesh_cdb.app_keys[i];

		if (key->net_idx != BT_MESH_KEY_UNUSED) {
			continue;
		}

		key->net_idx = net_idx;
		key->app_idx = app_idx;

		return key;
	}

	return NULL;
}

void bt_mesh_cdb_app_key_del(struct bt_mesh_cdb_app_key *key, bool store)
{
	BT_DBG("AppIdx 0x%03x store %u", key->app_idx, store);

	if (IS_ENABLED(CONFIG_BT_SETTINGS) && store) {
		bt_mesh_clear_cdb_app_key(key);
	}

	key->net_idx = BT_MESH_ADDR_UNASSIGNED;
	memset(key->keys, 0, sizeof(key->keys));
}

struct bt_mesh_cdb_app_key *bt_mesh_cdb_app_key_get(u16_t app_idx)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(bt_mesh_cdb.app_keys); i++) {
		struct bt_mesh_cdb_app_key *key = &bt_mesh_cdb.app_keys[i];

		if (key->net_idx != BT_MESH_KEY_UNUSED &&
		    key->app_idx == app_idx) {
			return key;
		}
	}

	return NULL;
}

void bt_mesh_cdb_app_key_store(const struct bt_mesh_cdb_app_key *key)
{
	bt_mesh_store_cdb_app_key(key);
}
