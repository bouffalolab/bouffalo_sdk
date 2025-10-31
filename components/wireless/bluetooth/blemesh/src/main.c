/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdbool.h>
#include <bt_errno.h>

#include <net/buf.h>
#include <bluetooth.h>
#include <conn.h>
#include <hci_core.h>
#include "../../blestack/src/include/bluetooth/bt_uuid.h"

#include <include/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG)
#define LOG_MODULE_NAME bt_mesh_main
#include "bt_log.h"

#include "test.h"
#include "adv.h"
#include "prov.h"
#include "net.h"
#include "beacon.h"
#include "lpn.h"
#include "friend.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "proxy.h"
#include "mesh_settings.h"
#include "mesh.h"

int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx,
		      u8_t flags, u32_t iv_index, u16_t addr,
		      const u8_t dev_key[16])
{
	bool pb_gatt_enabled;
	int err;

	if (atomic_test_and_set_bit(bt_mesh.flags, BT_MESH_VALID)) {
		return -EALREADY;
	}

	/* Added debug infomation by bouffalo */
	printf("mesh provision, network key : %s\n", bt_hex(net_key, 16));
	printf("mesh provision, device key : %s\n", bt_hex(dev_key, 16));
	printf("mesh provison, network key index :0x%04x flags :0x%02x iv index :0x%04lx\n", 
				net_idx, flags, iv_index);

	BT_INFO("Primary Element: 0x%04x", addr);
	BT_DBG("net_idx 0x%04x flags 0x%02x iv_index 0x%04lx",
	       net_idx, flags, iv_index);

	if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
		if (bt_mesh_proxy_prov_disable(false) == 0) {
			pb_gatt_enabled = true;
		} else {
			pb_gatt_enabled = false;
		}
	} else {
		pb_gatt_enabled = false;
	}

	/*
	 * FIXME:
	 * Should net_key and iv_index be over-ridden?
	 */
	if (IS_ENABLED(CONFIG_BT_MESH_CDB)
#if defined(CONFIG_BT_MESH_PROVISIONER)
		&& bt_mesh_prov_get()->role == 1 /* Added by bouffalo lab */
#endif
		) {
		const struct bt_mesh_comp *comp;
		const struct bt_mesh_prov *prov;
		struct bt_mesh_cdb_node *node;

		if (!atomic_test_bit(bt_mesh_cdb.flags,
				     BT_MESH_CDB_VALID)) {
			BT_ERR("No valid network");
			atomic_clear_bit(bt_mesh.flags, BT_MESH_VALID);
			return -EINVAL;
		}

		comp = bt_mesh_comp_get();
		if (comp == NULL) {
			BT_ERR("Failed to get node composition");
			atomic_clear_bit(bt_mesh.flags, BT_MESH_VALID);
			return -EINVAL;
		}

		if (!bt_mesh_cdb_subnet_get(net_idx)) {
			BT_ERR("No subnet with idx %d", net_idx);
			atomic_clear_bit(bt_mesh.flags, BT_MESH_VALID);
			return -ENOENT;
		}

		prov = bt_mesh_prov_get();
		node = bt_mesh_cdb_node_alloc(prov->uuid, addr,
					      comp->elem_count, net_idx);
		if (node == NULL) {
			BT_ERR("Failed to allocate database node");
			atomic_clear_bit(bt_mesh.flags, BT_MESH_VALID);
			return -ENOMEM;
		}

		addr = node->addr;
		iv_index = bt_mesh_cdb.iv_index;
		memcpy(node->dev_key, dev_key, 16);

		if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
			bt_mesh_store_cdb_node(node);
		}
	}

	err = bt_mesh_net_create(net_idx, flags, net_key, iv_index);
	if (err) {
		atomic_clear_bit(bt_mesh.flags, BT_MESH_VALID);

		if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) && pb_gatt_enabled) {
			bt_mesh_proxy_prov_enable();
		}

		return err;
	}

	/* Modified by bouffalo, don't refresh seq */
	//bt_mesh.seq = 0U;
	BT_WARN("mesh seq %lu", bt_mesh.seq);

	bt_mesh_comp_provision(addr);

	memcpy(bt_mesh.dev_key, dev_key, 16);

	if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER) &&
	    IS_ENABLED(CONFIG_BT_MESH_LPN_SUB_ALL_NODES_ADDR)) {
		bt_mesh_lpn_group_add(BT_MESH_ADDR_ALL_NODES);
	}

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		BT_DBG("Storing network information persistently");
		bt_mesh_store_net();
		bt_mesh_store_subnet(&bt_mesh.sub[0]);
		bt_mesh_store_iv(false);
	}

	bt_mesh_start();

	return 0;
}

int bt_mesh_provision_adv(const u8_t uuid[16], u16_t net_idx, u16_t addr,
			  u8_t attention_duration)
{
	if (!atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
		return -EINVAL;
	}

	if (bt_mesh_subnet_get(net_idx) == NULL) {
		return -EINVAL;
	}

	if (IS_ENABLED(CONFIG_BT_MESH_PROVISIONER) &&
	    IS_ENABLED(CONFIG_BT_MESH_PB_ADV)) {
		return bt_mesh_pb_adv_open(uuid, net_idx, addr,
					   attention_duration);
	}

	return -ENOTSUP;
}

void bt_mesh_reset(void)
{
	if (!atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
		return;
	}

	bt_mesh.iv_index = 0U;
	/* Comment by bouffalo */
	//bt_mesh.seq = 0U;

	memset(bt_mesh.flags, 0, sizeof(bt_mesh.flags));

	k_delayed_work_cancel(&bt_mesh.ivu_timer);

	bt_mesh_cfg_reset();

	bt_mesh_rx_reset();
	bt_mesh_tx_reset();

	bt_mesh_net_loopback_clear(BT_MESH_KEY_ANY);

	if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
		if (IS_ENABLED(CONFIG_BT_MESH_LPN_SUB_ALL_NODES_ADDR)) {
			u16_t group = BT_MESH_ADDR_ALL_NODES;

			bt_mesh_lpn_group_del(&group, 1);
		}

		bt_mesh_lpn_disable(true);
	}

	if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
		bt_mesh_friend_clear_net_idx(BT_MESH_KEY_ANY);
	}

	if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
		bt_mesh_proxy_gatt_disable();
	}

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_clear_net();
	}

	(void)memset(bt_mesh.dev_key, 0, sizeof(bt_mesh.dev_key));

	bt_mesh_scan_disable();
	bt_mesh_beacon_disable();

	bt_mesh_comp_unprovision();

	if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
		bt_mesh_prov_reset();
	}
}

bool bt_mesh_is_provisioned(void)
{
	return atomic_test_bit(bt_mesh.flags, BT_MESH_VALID);
}

static void model_suspend(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
			  bool vnd, bool primary, void *user_data)
{
	if (mod->pub && mod->pub->update) {
		mod->pub->count = 0U;
		k_delayed_work_cancel(&mod->pub->timer);
	}
}

int bt_mesh_suspend(void)
{
	int err;

	if (!atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
		return -EINVAL;
	}

	if (atomic_test_and_set_bit(bt_mesh.flags, BT_MESH_SUSPENDED)) {
		return -EALREADY;
	}

#if defined(BFLB_BLE)
	bt_mesh_adv_update();
	err = bt_le_adv_stop();
	if (err) {
		atomic_clear_bit(bt_mesh.flags, BT_MESH_SUSPENDED);
		BT_WARN("Disabling advertising failed (err %d)", err);
		return err;
	}
#endif /*BFLB_BLE*/

	err = bt_mesh_scan_disable();
	if (err) {
		atomic_clear_bit(bt_mesh.flags, BT_MESH_SUSPENDED);
		BT_WARN("Disabling scanning failed (err %d)", err);
		return err;
	}

	bt_mesh_hb_pub_disable();

	if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED) {
		bt_mesh_beacon_disable();
	}

	bt_mesh_model_foreach(model_suspend, NULL);

	return 0;
}

static void model_resume(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
			  bool vnd, bool primary, void *user_data)
{
	if (mod->pub && mod->pub->update) {
		s32_t period_ms = bt_mesh_model_pub_period_get(mod);

		if (period_ms) {
			k_delayed_work_submit(&mod->pub->timer,
					      K_MSEC(period_ms));
		}
	}
}

int bt_mesh_resume(void)
{
	int err;

	if (!atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
		return -EINVAL;
	}

	if (!atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_SUSPENDED)) {
		return -EALREADY;
	}

	err = bt_mesh_scan_enable();
	if (err) {
		BT_WARN("Re-enabling scanning failed (err %d)", err);
		atomic_set_bit(bt_mesh.flags, BT_MESH_SUSPENDED);
		return err;
	}

	if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED) {
		bt_mesh_beacon_enable();
	}

	bt_mesh_model_foreach(model_resume, NULL);

	return err;
}

int bt_mesh_init(const struct bt_mesh_prov *prov,
		 const struct bt_mesh_comp *comp)
{
	int err;

	err = bt_mesh_test();
	if (err) {
		return err;
	}

	err = bt_mesh_comp_register(comp);
	if (err) {
		return err;
	}

	if (IS_ENABLED(CONFIG_BT_MESH_PROXY)) {
		bt_mesh_proxy_init();
	}

	if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
		err = bt_mesh_prov_init(prov);
		if (err) {
			return err;
		}
	}

	bt_mesh_net_init();
	bt_mesh_trans_init();
	bt_mesh_beacon_init();
	bt_mesh_adv_init();

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_settings_init();
	}

	return 0;
}

static void model_start(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
			bool vnd, bool primary, void *user_data)
{
	if (mod->cb && mod->cb->start) {
		mod->cb->start(mod);
	}
}

int bt_mesh_start(void)
{
	bt_mesh_net_start();
	bt_mesh_model_foreach(model_start, NULL);

	return 0;
}

#ifdef CONFIG_BT_MESH_MOD_BIND_CB
void bt_mesh_mod_bind_cb(struct bt_mesh_model *model,u16_t net_idx, u16_t mod_app_idx)
{
    bt_mesh_prov_mod_bind_cb(model, net_idx, mod_app_idx);
}
#endif /* CONFIG_BT_MESH_MOD_BIND_CB */

#ifdef CONFIG_BT_MESH_APPKEY_ADD_CB
void bt_mesh_app_key_add_cb(u16_t net_idx, u16_t mod_app_idx)
{
    bt_mesh_prov_app_key_add_cb(net_idx, mod_app_idx);
}
#endif /* CONFIG_BT_MESH_APPKEY_ADD_CB */

#ifdef CONFIG_BT_MESH_MOD_SUB_ADD_CB
void bt_mesh_mod_sub_add_cb(struct bt_mesh_model *model, u16_t elem_addr, u16_t group_addr)
{
    bt_mesh_prov_mod_sub_add_cb(model, elem_addr, group_addr);
}
#endif /* CONFIG_BT_MESH_MOD_SUB_ADD_CB */

