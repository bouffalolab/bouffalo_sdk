/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <bt_errno.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/types.h>
#include <util.h>
#include <byteorder.h>

//#include <settings/settings.h>

#include <net/buf.h>

#include <hci_host.h>
#include <conn.h>
#include <include/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_SETTINGS)
#define LOG_MODULE_NAME bt_mesh_settings
#include "bt_log.h"

#include "mesh_config.h"
#include "mesh.h"
#include "net.h"
#include "crypto.h"
#include "transport.h"
#include "access.h"
#include "beacon.h"
#include "foundation.h"
#include "proxy.h"
#include "settings.h"
#include "lpn.h"
#if defined(BFLB_BLE)
#include "mesh_settings.h"
#ifdef CONFIG_BT_SETTINGS
#if defined(CONFIG_BT_SETTINGS)
#include "easyflash.h"
#endif
#endif
#endif

/* Added by bouffalolab for mesh sequence compensate, when power up */
#define BT_MESH_SEQ_PWRON_COMPENSATE (100)

/* Added by bouffalo */
static int settings_name_next(const char *name, const char **next);
/* Added by bouffalo */
void load_mesh_setting(void);

/* Tracking of what storage changes are pending for App and Net Keys. We
 * track this in a separate array here instead of within the respective
 * bt_mesh_app_key and bt_mesh_subnet structs themselves, since once a key
 * gets deleted its struct becomes invalid and may be reused for other keys.
 */
struct key_update {
	u16_t key_idx:12,    /* AppKey or NetKey Index */
	      valid:1,       /* 1 if this entry is valid, 0 if not */
	      app_key:1,     /* 1 if this is an AppKey, 0 if a NetKey */
	      clear:1;       /* 1 if key needs clearing, 0 if storing */
};

static struct key_update key_updates[CONFIG_BT_MESH_APP_KEY_COUNT +
				     CONFIG_BT_MESH_SUBNET_COUNT];

static struct k_delayed_work pending_store;

/* Mesh network storage information */
struct net_val {
	u16_t primary_addr;
	u8_t  dev_key[16];
} __packed;

/* Sequence number storage */
struct seq_val {
	u8_t val[3];
} __packed;

/* Heartbeat Publication storage */
struct hb_pub_val {
	u16_t dst;
	u8_t  period;
	u8_t  ttl;
	u16_t feat;
	u16_t net_idx:12,
	      indefinite:1;
};

/* Miscellaneous configuration server model states */
struct cfg_val {
	u8_t net_transmit;
	u8_t relay;
	u8_t relay_retransmit;
	u8_t beacon;
	u8_t gatt_proxy;
	u8_t frnd;
	u8_t default_ttl;
};

/* IV Index & IV Update storage */
struct iv_val {
	u32_t iv_index;
	u8_t  iv_update:1,
	      iv_duration:7;
} __packed;

/* Replay Protection List storage */
struct rpl_val {
	u32_t seq:24,
	      old_iv:1;
};

/* NetKey storage information */
struct net_key_val {
	u8_t kr_flag:1,
	     kr_phase:7;
	u8_t val[2][16];
} __packed;

/* AppKey storage information */
struct app_key_val {
	u16_t net_idx;
	bool  updated;
	u8_t  val[2][16];
} __packed;

struct mod_pub_val {
	u16_t addr;
	u16_t key;
	u8_t  ttl;
	u8_t  retransmit;
	u8_t  period;
	u8_t  period_div:4,
	      cred:1;
};

/* Virtual Address information */
struct va_val {
	u16_t ref;
	u16_t addr;
	u8_t uuid[16];
} __packed;

struct cdb_net_val {
	u32_t iv_index;
	bool  iv_update;
} __packed;

/* Node storage information */
struct node_val {
	u16_t net_idx;
	u8_t  num_elem;
	u8_t  flags;
#define F_NODE_CONFIGURED 0x01
	u8_t  uuid[16];
	u8_t  dev_key[16];
} __packed;

struct node_update {
	u16_t addr;
	bool clear;
};

#if defined(CONFIG_BT_MESH_CDB)
static struct node_update cdb_node_updates[CONFIG_BT_MESH_CDB_NODE_COUNT];
static struct key_update cdb_key_updates[CONFIG_BT_MESH_CDB_SUBNET_COUNT +
					 CONFIG_BT_MESH_CDB_APP_KEY_COUNT];
#else
/* Comment by bouffalo */
//static struct node_update cdb_node_updates[0];
//static struct key_update cdb_key_updates[0];
#endif

/* We need this so we don't overwrite app-hardcoded values in case FCB
 * contains a history of changes but then has a NULL at the end.
 */
static struct {
	bool valid;
	struct cfg_val cfg;
} stored_cfg;

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
int bt_mesh_settings_set(settings_read_cb read_cb, void *cb_arg,
			 void *out, size_t read_len)
{
	ssize_t len;

	len = read_cb(cb_arg, out, read_len);
	if (len < 0) {
		BT_ERR("Failed to read value (err %zd)", len);
		return len;
	}

	//BT_HEXDUMP_DBG(out, len, "val");

	if (len != read_len) {
		BT_ERR("Unexpected value length (%zd != %zu)", len, read_len);
		return -EINVAL;
	}

	return 0;
}
#endif

static inline int mesh_x_set(settings_read_cb read_cb, void *cb_arg, void *out,
			     size_t read_len)
{
	ssize_t len;

	len = read_cb(cb_arg, out, read_len);
	if (len < 0) {
		BT_ERR("Failed to read value (err %zu)", len);
		return len;
	}
	/* Comment by bouffalo */
	//BT_HEXDUMP_DBG(out, len, "val");

	if (len != read_len) {
		BT_ERR("Unexpected value length (%zu != %zu)", len, read_len);
		return -EINVAL;
	}

	return 0;
}

static int net_set(const char *name, size_t len_rd, settings_read_cb read_cb,
		   void *cb_arg)
{
	struct net_val net;
	int err;

	if (len_rd == 0) {
		BT_DBG("val (null)");

		bt_mesh_comp_unprovision();
		(void)memset(bt_mesh.dev_key, 0, sizeof(bt_mesh.dev_key));
		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &net, sizeof(net));
	if (err) {
		BT_ERR("Failed to set \'net\'");
		return err;
	}

	memcpy(bt_mesh.dev_key, net.dev_key, sizeof(bt_mesh.dev_key));
	bt_mesh_comp_provision(net.primary_addr);

	BT_DBG("Provisioned with primary address 0x%04x", net.primary_addr);
	BT_DBG("Recovered DevKey %s", bt_hex(bt_mesh.dev_key, 16));

	return 0;
}

static int iv_set(const char *name, size_t len_rd, settings_read_cb read_cb,
		  void *cb_arg)
{
	struct iv_val iv;
	int err;

	if (len_rd == 0) {
		BT_DBG("IV deleted");

		bt_mesh.iv_index = 0U;
		atomic_clear_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS);
		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &iv, sizeof(iv));
	if (err) {
		BT_ERR("Failed to set \'iv\'");
		return err;
	}

	bt_mesh.iv_index = iv.iv_index;
	atomic_set_bit_to(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS, iv.iv_update);
	bt_mesh.ivu_duration = iv.iv_duration;

	BT_DBG("IV Index 0x%04lx (IV Update Flag %u) duration %u hours",
	       iv.iv_index, iv.iv_update, iv.iv_duration);

	return 0;
}

static int seq_set(const char *name, size_t len_rd, settings_read_cb read_cb,
		   void *cb_arg)
{
	struct seq_val seq;
	int err;

	if (len_rd == 0) {
		BT_DBG("val (null)");

		bt_mesh.seq = 0U;
		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &seq, sizeof(seq));
	if (err) {
		BT_ERR("Failed to set \'seq\'");
		return err;
	}

	bt_mesh.seq = sys_get_le24(seq.val);

	if (CONFIG_BT_MESH_SEQ_STORE_RATE > 0) {
		/* Make sure we have a large enough sequence number. We
		 * subtract 1 so that the first transmission causes a write
		 * to the settings storage.
		 */
		bt_mesh.seq += (CONFIG_BT_MESH_SEQ_STORE_RATE -
				(bt_mesh.seq % CONFIG_BT_MESH_SEQ_STORE_RATE))
				+ BT_MESH_SEQ_PWRON_COMPENSATE; /* Add by bouffalolab */
		bt_mesh.seq--;
	}

	BT_DBG("Sequence Number 0x%06lx", bt_mesh.seq);

	return 0;
}

static struct bt_mesh_rpl *rpl_find(u16_t src)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
		if (bt_mesh.rpl[i].src == src) {
			return &bt_mesh.rpl[i];
		}
	}

	return NULL;
}

static struct bt_mesh_rpl *rpl_alloc(u16_t src)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
		if (!bt_mesh.rpl[i].src) {
			bt_mesh.rpl[i].src = src;
			return &bt_mesh.rpl[i];
		}
	}

	return NULL;
}

static int rpl_set(const char *name, size_t len_rd,
		   settings_read_cb read_cb, void *cb_arg)
{
	struct bt_mesh_rpl *entry;
	struct rpl_val rpl;
	int err;
	u16_t src;

	if (!name) {
		BT_ERR("Insufficient number of arguments");
		return -ENOENT;
	}

	src = strtol(name, NULL, 16);
	entry = rpl_find(src);

	if (len_rd == 0) {
		BT_DBG("val (null)");
		if (entry) {
			(void)memset(entry, 0, sizeof(*entry));
		} else {
			BT_WARN("Unable to find RPL entry for 0x%04x", src);
		}

		return 0;
	}

	if (!entry) {
		entry = rpl_alloc(src);
		if (!entry) {
			BT_ERR("Unable to allocate RPL entry for 0x%04x", src);
			return -ENOMEM;
		}
	}

	err = mesh_x_set(read_cb, cb_arg, &rpl, sizeof(rpl));
	if (err) {
		BT_ERR("Failed to set `net`");
		return err;
	}

	entry->seq = rpl.seq;
	entry->old_iv = rpl.old_iv;

	BT_DBG("RPL entry for 0x%04x: Seq 0x%06lx old_iv %u", entry->src,
	       entry->seq, entry->old_iv);

	return 0;
}

static int net_key_set(const char *name, size_t len_rd,
		       settings_read_cb read_cb, void *cb_arg)
{
	struct bt_mesh_subnet *sub;
	struct net_key_val key;
	int i, err;
	u16_t net_idx;

	if (!name) {
		BT_ERR("Insufficient number of arguments");
		return -ENOENT;
	}

	net_idx = strtol(name, NULL, 16);
	sub = bt_mesh_subnet_get(net_idx);

	if (len_rd == 0) {
		BT_DBG("val (null)");
		if (!sub) {
			BT_ERR("No subnet with NetKeyIndex 0x%03x", net_idx);
			return -ENOENT;
		}

		BT_DBG("Deleting NetKeyIndex 0x%03x", net_idx);
		bt_mesh_subnet_del(sub, false);
		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &key, sizeof(key));
	if (err) {
		BT_ERR("Failed to set \'net-key\'");
		return err;
	}

	if (sub) {
		BT_DBG("Updating existing NetKeyIndex 0x%03x", net_idx);

		sub->kr_flag = key.kr_flag;
		sub->kr_phase = key.kr_phase;
		memcpy(sub->keys[0].net, &key.val[0], 16);
		memcpy(sub->keys[1].net, &key.val[1], 16);

		return 0;
	}

	for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		if (bt_mesh.sub[i].net_idx == BT_MESH_KEY_UNUSED) {
			sub = &bt_mesh.sub[i];
			break;
		}
	}

	if (!sub) {
		BT_ERR("No space to allocate a new subnet");
		return -ENOMEM;
	}

	sub->net_idx = net_idx;
	sub->kr_flag = key.kr_flag;
	sub->kr_phase = key.kr_phase;
	memcpy(sub->keys[0].net, &key.val[0], 16);
	memcpy(sub->keys[1].net, &key.val[1], 16);

	BT_DBG("NetKeyIndex 0x%03x recovered from storage", net_idx);

	return 0;
}

static int app_key_set(const char *name, size_t len_rd,
		       settings_read_cb read_cb, void *cb_arg)
{
	struct bt_mesh_app_key *app;
	struct app_key_val key;
	u16_t app_idx;
	int err;

	if (!name) {
		BT_ERR("Insufficient number of arguments");
		return -ENOENT;
	}

	app_idx = strtol(name, NULL, 16);

	if (len_rd == 0) {
		BT_DBG("val (null)");
		BT_DBG("Deleting AppKeyIndex 0x%03x", app_idx);

		app = bt_mesh_app_key_find(app_idx);
		if (app) {
			bt_mesh_app_key_del(app, false);
		}

		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &key, sizeof(key));
	if (err) {
		BT_ERR("Failed to set \'app-key\'");
		return err;
	}

	app = bt_mesh_app_key_find(app_idx);
	if (!app) {
		app = bt_mesh_app_key_alloc(app_idx);
	}

	if (!app) {
		BT_ERR("No space for a new app key");
		return -ENOMEM;
	}

	app->net_idx = key.net_idx;
	app->app_idx = app_idx;
	app->updated = key.updated;
	memcpy(app->keys[0].val, key.val[0], 16);
	memcpy(app->keys[1].val, key.val[1], 16);

	bt_mesh_app_id(app->keys[0].val, &app->keys[0].id);
	bt_mesh_app_id(app->keys[1].val, &app->keys[1].id);

	BT_DBG("AppKeyIndex 0x%03x recovered from storage", app_idx);

	return 0;
}

static int hb_pub_set(const char *name, size_t len_rd,
		      settings_read_cb read_cb, void *cb_arg)
{
	struct bt_mesh_hb_pub *pub = bt_mesh_hb_pub_get();
	struct hb_pub_val hb_val;
	int err;

	if (!pub) {
		return -ENOENT;
	}

	if (len_rd == 0) {
		pub->dst = BT_MESH_ADDR_UNASSIGNED;
		pub->count = 0U;
		pub->ttl = 0U;
		pub->period = 0U;
		pub->feat = 0U;

		BT_DBG("Cleared heartbeat publication");
		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &hb_val, sizeof(hb_val));
	if (err) {
		BT_ERR("Failed to set \'hb_val\'");
		return err;
	}

	pub->dst = hb_val.dst;
	pub->period = hb_val.period;
	pub->ttl = hb_val.ttl;
	pub->feat = hb_val.feat;
	pub->net_idx = hb_val.net_idx;

	if (hb_val.indefinite) {
		pub->count = 0xffff;
	} else {
		pub->count = 0U;
	}

	BT_DBG("Restored heartbeat publication");

	return 0;
}

static int cfg_set(const char *name, size_t len_rd,
		   settings_read_cb read_cb, void *cb_arg)
{
	struct bt_mesh_cfg_srv *cfg = bt_mesh_cfg_get();
	int err;

	if (!cfg) {
		return -ENOENT;
	}

	if (len_rd == 0) {
		stored_cfg.valid = false;
		BT_DBG("Cleared configuration state");
		return 0;
	}


	err = mesh_x_set(read_cb, cb_arg, &stored_cfg.cfg,
			 sizeof(stored_cfg.cfg));
	if (err) {
		BT_ERR("Failed to set \'cfg\'");
		return err;
	}

	stored_cfg.valid = true;
	BT_DBG("Restored configuration state");

	return 0;
}

static int mod_set_bind(struct bt_mesh_model *mod, size_t len_rd,
			settings_read_cb read_cb, void *cb_arg)
{
	ssize_t len;
	int i;

	/* Start with empty array regardless of cleared or set value */
	for (i = 0; i < ARRAY_SIZE(mod->keys); i++) {
		mod->keys[i] = BT_MESH_KEY_UNUSED;
	}

	if (len_rd == 0) {
		BT_DBG("Cleared bindings for model");
		return 0;
	}

	len = read_cb(cb_arg, mod->keys, sizeof(mod->keys));
	if (len < 0) {
		BT_ERR("Failed to read value (err %zd)", len);
		return len;
	}


	BT_DBG("Decoded %zu bound keys for model", len / sizeof(mod->keys[0]));
	return 0;
}

static int mod_set_sub(struct bt_mesh_model *mod, size_t len_rd,
		       settings_read_cb read_cb, void *cb_arg)
{
	ssize_t len;

	/* Start with empty array regardless of cleared or set value */
	(void)memset(mod->groups, 0, sizeof(mod->groups));

	if (len_rd == 0) {
		BT_DBG("Cleared subscriptions for model");
		return 0;
	}

	len = read_cb(cb_arg, mod->groups, sizeof(mod->groups));
	if (len < 0) {
		BT_ERR("Failed to read value (err %zd)", len);
		return len;
	}

	BT_DBG("Decoded %zu subscribed group addresses for model",
	       len / sizeof(mod->groups[0]));
	return 0;
}

static int mod_set_pub(struct bt_mesh_model *mod, size_t len_rd,
		       settings_read_cb read_cb, void *cb_arg)
{
	struct mod_pub_val pub;
	int err;

	if (!mod->pub) {
		BT_WARN("Model has no publication context!");
		return -EINVAL;
	}

	if (len_rd == 0) {
		mod->pub->addr = BT_MESH_ADDR_UNASSIGNED;
		mod->pub->key = 0U;
		mod->pub->cred = 0U;
		mod->pub->ttl = 0U;
		mod->pub->period = 0U;
		mod->pub->retransmit = 0U;
		mod->pub->count = 0U;

		BT_DBG("Cleared publication for model");
		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &pub, sizeof(pub));
	if (err) {
		BT_ERR("Failed to set \'model-pub\'");
		return err;
	}

	mod->pub->addr = pub.addr;
	mod->pub->key = pub.key;
	mod->pub->cred = pub.cred;
	mod->pub->ttl = pub.ttl;
	mod->pub->period = pub.period;
	mod->pub->retransmit = pub.retransmit;
	mod->pub->count = 0U;

	BT_DBG("Restored model publication, dst 0x%04x app_idx 0x%03x",
	       pub.addr, pub.key);

	return 0;
}

static int mod_data_set(struct bt_mesh_model *mod,
			const char *name, size_t len_rd,
			settings_read_cb read_cb, void *cb_arg)
{
	const char *next;

	settings_name_next(name, &next);

	if (mod->cb && mod->cb->settings_set) {
		return mod->cb->settings_set(mod, next, len_rd,
			read_cb, cb_arg);
	}

	return 0;
}

static int mod_set(bool vnd, const char *name, size_t len_rd,
		   settings_read_cb read_cb, void *cb_arg)
{
	struct bt_mesh_model *mod;
	u8_t elem_idx, mod_idx;
	u16_t mod_key;
	int len;
	const char *next;

	if (!name) {
		BT_ERR("Insufficient number of arguments");
		return -ENOENT;
	}

	mod_key = strtol(name, NULL, 16);
	elem_idx = mod_key >> 8;
	mod_idx = mod_key;

	BT_DBG("Decoded mod_key 0x%04x as elem_idx %u mod_idx %u",
	       mod_key, elem_idx, mod_idx);

	mod = bt_mesh_model_get(vnd, elem_idx, mod_idx);
	if (!mod) {
		BT_ERR("Failed to get model for elem_idx %u mod_idx %u",
		       elem_idx, mod_idx);
		return -ENOENT;
	}

	len = settings_name_next(name, &next);

	if (!next) {
		BT_ERR("Insufficient number of arguments");
		return -ENOENT;
	}

	if (!strncmp(next, "bind", len)) {
		return mod_set_bind(mod, len_rd, read_cb, cb_arg);
	}

	if (!strncmp(next, "sub", len)) {
		return mod_set_sub(mod, len_rd, read_cb, cb_arg);
	}

	if (!strncmp(next, "pub", len)) {
		return mod_set_pub(mod, len_rd, read_cb, cb_arg);
	}

	if (!strncmp(next, "data", len)) {
		return mod_data_set(mod, next, len_rd, read_cb, cb_arg);
	}

	BT_WARN("Unknown module key %s", next);
	return -ENOENT;
}

static int sig_mod_set(const char *name, size_t len_rd,
		       settings_read_cb read_cb, void *cb_arg)
{
	return mod_set(false, name, len_rd, read_cb, cb_arg);
}

static int vnd_mod_set(const char *name, size_t len_rd,
		       settings_read_cb read_cb, void *cb_arg)
{
	return mod_set(true, name, len_rd, read_cb, cb_arg);
}

#if CONFIG_BT_MESH_LABEL_COUNT > 0
static int va_set(const char *name, size_t len_rd,
		  settings_read_cb read_cb, void *cb_arg)
{
	struct va_val va;
	struct label *lab;
	u16_t index;
	int err;

	if (!name) {
		BT_ERR("Insufficient number of arguments");
		return -ENOENT;
	}

	index = strtol(name, NULL, 16);

	if (len_rd == 0) {
		BT_WARN("Mesh Virtual Address length = 0");
		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &va, sizeof(va));
	if (err) {
		BT_ERR("Failed to set \'virtual address\'");
		return err;
	}

	if (va.ref == 0) {
		BT_WARN("Ignore Mesh Virtual Address ref = 0");
		return 0;
	}

	lab = get_label(index);
	if (lab == NULL) {
		BT_WARN("Out of labels buffers");
		return -ENOBUFS;
	}

	memcpy(lab->uuid, va.uuid, 16);
	lab->addr = va.addr;
	lab->ref = va.ref;

	BT_DBG("Restored Virtual Address, addr 0x%04x ref 0x%04x",
	       lab->addr, lab->ref);

	return 0;
}
#endif

#if defined(CONFIG_BT_MESH_CDB)
static int cdb_net_set(const char *name, size_t len_rd,
		       settings_read_cb read_cb, void *cb_arg)
{
	struct cdb_net_val net = {};/*Modified by bouffalo */
	int err;

	if (len_rd == 0) {
		BT_DBG("val (null)");
		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &net, sizeof(net));
	if (err) {
		BT_ERR("Failed to set \'cdb_net\'");
		return err;
	}

	bt_mesh_cdb.iv_index = net.iv_index;

	if (net.iv_update) {
		atomic_set_bit(bt_mesh_cdb.flags, BT_MESH_CDB_IVU_IN_PROGRESS);
	}

	atomic_set_bit(bt_mesh_cdb.flags, BT_MESH_CDB_VALID);

	return 0;
}

static int cdb_node_set(const char *name, size_t len_rd,
			settings_read_cb read_cb, void *cb_arg)
{
	struct bt_mesh_cdb_node *node;
	struct node_val val;
	u16_t addr;
	int err;

	if (!name) {
		BT_ERR("Insufficient number of arguments");
		return -ENOENT;
	}

	addr = strtol(name, NULL, 16);

	if (len_rd == 0) {
		BT_DBG("val (null)");
		BT_DBG("Deleting node 0x%04x", addr);

		node = bt_mesh_cdb_node_get(addr);
		if (node) {
			bt_mesh_cdb_node_del(node, false);
		}

		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &val, sizeof(val));
	if (err) {
		BT_ERR("Failed to set \'node\'");
		return err;
	}

	node = bt_mesh_cdb_node_get(addr);
	if (!node) {
		node = bt_mesh_cdb_node_alloc(val.uuid, addr, val.num_elem,
					      val.net_idx);
	}

	if (!node) {
		BT_ERR("No space for a new node");
		return -ENOMEM;
	}

	if (val.flags & F_NODE_CONFIGURED) {
		atomic_set_bit(node->flags, BT_MESH_CDB_NODE_CONFIGURED);
	}

	memcpy(node->uuid, val.uuid, 16);
	memcpy(node->dev_key, val.dev_key, 16);

	BT_DBG("Node 0x%04x recovered from storage", addr);

	return 0;
}

static int cdb_subnet_set(const char *name, size_t len_rd,
			  settings_read_cb read_cb, void *cb_arg)
{
	struct bt_mesh_cdb_subnet *sub;
	struct net_key_val key;
	u16_t net_idx;
	int err;

	if (!name) {
		BT_ERR("Insufficient number of arguments");
		return -ENOENT;
	}

	net_idx = strtol(name, NULL, 16);
	sub = bt_mesh_cdb_subnet_get(net_idx);

	if (len_rd == 0) {
		BT_DBG("val (null)");
		if (!sub) {
			BT_ERR("No subnet with NetKeyIndex 0x%03x", net_idx);
			return -ENOENT;
		}

		BT_DBG("Deleting NetKeyIndex 0x%03x", net_idx);
		bt_mesh_cdb_subnet_del(sub, false);
		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &key, sizeof(key));
	if (err) {
		BT_ERR("Failed to set \'net-key\'");
		return err;
	}

	if (sub) {
		BT_DBG("Updating existing NetKeyIndex 0x%03x", net_idx);

		sub->kr_flag = key.kr_flag;
		sub->kr_phase = key.kr_phase;
		memcpy(sub->keys[0].net_key, &key.val[0], 16);
		memcpy(sub->keys[1].net_key, &key.val[1], 16);

		return 0;
	}

	sub = bt_mesh_cdb_subnet_alloc(net_idx);
	if (!sub) {
		BT_ERR("No space to allocate a new subnet");
		return -ENOMEM;
	}

	sub->kr_flag = key.kr_flag;
	sub->kr_phase = key.kr_phase;
	memcpy(sub->keys[0].net_key, &key.val[0], 16);
	memcpy(sub->keys[1].net_key, &key.val[1], 16);

	BT_DBG("NetKeyIndex 0x%03x recovered from storage", net_idx);

	return 0;
}

static int cdb_app_key_set(const char *name, size_t len_rd,
			   settings_read_cb read_cb, void *cb_arg)
{
	struct bt_mesh_cdb_app_key *app;
	struct app_key_val key;
	u16_t app_idx;
	int err;

	if (!name) {
		BT_ERR("Insufficient number of arguments");
		return -ENOENT;
	}

	app_idx = strtol(name, NULL, 16);

	if (len_rd == 0) {
		BT_DBG("val (null)");
		BT_DBG("Deleting AppKeyIndex 0x%03x", app_idx);

		app = bt_mesh_cdb_app_key_get(app_idx);
		if (app) {
			bt_mesh_cdb_app_key_del(app, false);
		}

		return 0;
	}

	err = mesh_x_set(read_cb, cb_arg, &key, sizeof(key));
	if (err) {
		BT_ERR("Failed to set \'app-key\'");
		return err;
	}

	app = bt_mesh_cdb_app_key_get(app_idx);
	if (!app) {
		app = bt_mesh_cdb_app_key_alloc(key.net_idx, app_idx);
	}

	if (!app) {
		BT_ERR("No space for a new app key");
		return -ENOMEM;
	}

	memcpy(app->keys[0].app_key, key.val[0], 16);
	memcpy(app->keys[1].app_key, key.val[1], 16);

	BT_DBG("AppKeyIndex 0x%03x recovered from storage", app_idx);

	return 0;
}

static int cdb_set(const char *name, size_t len_rd,
		   settings_read_cb read_cb, void *cb_arg)
{
	int len = 0; /* Add by bouffalo */
	const char *next = NULL; /* Add by bouffalo */

	if (!name) {
		BT_ERR("Insufficient number of arguments");
		return -ENOENT;
	}

	if (!strcmp(name, "Net")) {
		return cdb_net_set(name, len_rd, read_cb, cb_arg);
	}


	len = settings_name_next(name, &next);

	if (!next) {
		BT_ERR("Insufficient number of arguments");
		return -ENOENT;
	}

	if (!strncmp(name, "Node", len)) {
		return cdb_node_set(next, len_rd, read_cb, cb_arg);
	}

	if (!strncmp(name, "Subnet", len)) {
		return cdb_subnet_set(next, len_rd, read_cb, cb_arg);
	}

	if (!strncmp(name, "AppKey", len)) {
		return cdb_app_key_set(next, len_rd, read_cb, cb_arg);
	}

	BT_WARN("Unknown module key %s", name);
	return -ENOENT;
}
#endif

const struct mesh_setting {
	const char *name;
	int (*func)(const char *name, size_t len_rd,
		    settings_read_cb read_cb, void *cb_arg);
} settings[] = {
	{ "Net", net_set },
	{ "IV", iv_set },
	{ "Seq", seq_set },
	{ "RPL", rpl_set },
	{ "NetKey", net_key_set },
	{ "AppKey", app_key_set },
	{ "HBPub", hb_pub_set },
	{ "Cfg", cfg_set },
	{ "s", sig_mod_set },
	{ "v", vnd_mod_set },
#if CONFIG_BT_MESH_LABEL_COUNT > 0
	{ "Va", va_set },
#endif
#if defined(CONFIG_BT_MESH_CDB)
	{ "cdb", cdb_set },
#endif
};
/* Modeifed by bouffalo */
int mesh_set(void)
{
    #if defined(CONFIG_BT_SETTINGS)
	load_mesh_setting();
	#endif
    return 0;
}

static int subnet_init(struct bt_mesh_subnet *sub)
{
	int err;

	err = bt_mesh_net_keys_create(&sub->keys[0], sub->keys[0].net);
	if (err) {
		BT_ERR("Unable to generate keys for subnet");
		return -EIO;
	}

	if (sub->kr_phase != BT_MESH_KR_NORMAL) {
		err = bt_mesh_net_keys_create(&sub->keys[1], sub->keys[1].net);
		if (err) {
			BT_ERR("Unable to generate keys for subnet");
			(void)memset(&sub->keys[0], 0, sizeof(sub->keys[0]));
			return -EIO;
		}
	}

	if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
		sub->node_id = BT_MESH_NODE_IDENTITY_STOPPED;
	} else {
		sub->node_id = BT_MESH_NODE_IDENTITY_NOT_SUPPORTED;
	}

	/* Make sure we have valid beacon data to be sent */
	bt_mesh_net_beacon_update(sub);

	return 0;
}

static void commit_mod(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
		       bool vnd, bool primary, void *user_data)
{
	if (mod->pub && mod->pub->update &&
	    mod->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
		s32_t ms = bt_mesh_model_pub_period_get(mod);

		if (ms > 0) {
			BT_DBG("Starting publish timer (period %lu ms)", ms);
			k_delayed_work_submit(&mod->pub->timer, K_MSEC(ms));
		}
	}

	if (!IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
		return;
	}

	for (int i = 0; i < ARRAY_SIZE(mod->groups); i++) {
		if (mod->groups[i] != BT_MESH_ADDR_UNASSIGNED) {
			bt_mesh_lpn_group_add(mod->groups[i]);
		}
	}
}
/* Modified by bouffalo */
int mesh_commit(void)
{
	struct bt_mesh_hb_pub *hb_pub;
	struct bt_mesh_cfg_srv *cfg;
	int i;

	BT_DBG("sub[0].net_idx 0x%03x", bt_mesh.sub[0].net_idx);

	if (bt_mesh.sub[0].net_idx == BT_MESH_KEY_UNUSED) {
		/* Nothing to do since we're not yet provisioned */
		return 0;
	}

	if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
		bt_mesh_proxy_prov_disable(true);
	}

	for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		struct bt_mesh_subnet *sub = &bt_mesh.sub[i];
		int err;

		if (sub->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		err = subnet_init(sub);
		if (err) {
			BT_ERR("Failed to init subnet 0x%03x", sub->net_idx);
		}
	}

	if (bt_mesh.ivu_duration < BT_MESH_IVU_MIN_HOURS) {
		k_delayed_work_submit(&bt_mesh.ivu_timer, BT_MESH_IVU_TIMEOUT);
	}

	bt_mesh_model_foreach(commit_mod, NULL);

	hb_pub = bt_mesh_hb_pub_get();
	if (hb_pub && hb_pub->dst != BT_MESH_ADDR_UNASSIGNED &&
	    hb_pub->count && hb_pub->period) {
		BT_DBG("Starting heartbeat publication");
		k_work_submit(&hb_pub->timer.work);
	}

	cfg = bt_mesh_cfg_get();
	if (cfg && stored_cfg.valid) {
		cfg->net_transmit = stored_cfg.cfg.net_transmit;
		cfg->relay = stored_cfg.cfg.relay;
		cfg->relay_retransmit = stored_cfg.cfg.relay_retransmit;
		cfg->beacon = stored_cfg.cfg.beacon;
		cfg->gatt_proxy = stored_cfg.cfg.gatt_proxy;
		cfg->frnd = stored_cfg.cfg.frnd;
		cfg->default_ttl = stored_cfg.cfg.default_ttl;
	}

	atomic_set_bit(bt_mesh.flags, BT_MESH_VALID);

	bt_mesh_start();

	return 0;
}

#if !defined(BFLB_BLE)
SETTINGS_STATIC_HANDLER_DEFINE(bt_mesh, "bt/mesh", NULL, mesh_set, mesh_commit,
			       NULL);
#endif

/* Pending flags that use K_NO_WAIT as the storage timeout */
#define NO_WAIT_PENDING_BITS (BIT(BT_MESH_NET_PENDING) |           \
			      BIT(BT_MESH_IV_PENDING) |            \
			      BIT(BT_MESH_SEQ_PENDING))

/* Pending flags that use CONFIG_BT_MESH_STORE_TIMEOUT */
#define GENERIC_PENDING_BITS (BIT(BT_MESH_KEYS_PENDING) |          \
			      BIT(BT_MESH_HB_PUB_PENDING) |        \
			      BIT(BT_MESH_CFG_PENDING) |           \
			      BIT(BT_MESH_MOD_PENDING))

static void schedule_store(int flag)
{
	s32_t timeout_ms, remaining;

	atomic_set_bit(bt_mesh.flags, flag);

	if (atomic_get(bt_mesh.flags) & NO_WAIT_PENDING_BITS) {
		timeout_ms = 0;
	} else if (atomic_test_bit(bt_mesh.flags, BT_MESH_RPL_PENDING) &&
		   (!(atomic_get(bt_mesh.flags) & GENERIC_PENDING_BITS) ||
		    (CONFIG_BT_MESH_RPL_STORE_TIMEOUT <
		     CONFIG_BT_MESH_STORE_TIMEOUT))) {
		timeout_ms = CONFIG_BT_MESH_RPL_STORE_TIMEOUT * MSEC_PER_SEC;
	} else {
		timeout_ms = CONFIG_BT_MESH_STORE_TIMEOUT * MSEC_PER_SEC;
	}

	remaining = k_delayed_work_remaining_get(&pending_store);
	if ((remaining > 0) && remaining < timeout_ms) {
		BT_DBG("Not rescheduling due to existing earlier deadline");
		return;
	}

	BT_DBG("Waiting %ld seconds", timeout_ms / MSEC_PER_SEC);

	k_delayed_work_submit(&pending_store, K_MSEC(timeout_ms));
}

static void clear_iv(void)
{
	int err;

	err = settings_delete("bt/mesh/IV");
	if (err) {
		BT_ERR("Failed to clear IV");
	} else {
		BT_DBG("Cleared IV");
	}
}

static void clear_net(void)
{
	int err;

	err = settings_delete("bt/mesh/Net");
	if (err) {
		/* Modifyed by bouffalo */
		BT_ERR("Failed to clear Network, err(%d)", err);
	} else {
		BT_DBG("Cleared Network");
	}
}

static void store_pending_net(void)
{
	struct net_val net;
	int err;

	BT_DBG("addr 0x%04x DevKey %s", bt_mesh_primary_addr(),
	       bt_hex(bt_mesh.dev_key, 16));

	net.primary_addr = bt_mesh_primary_addr();
	memcpy(net.dev_key, bt_mesh.dev_key, 16);
	/* Fix warnning by bouffalo */
	err = settings_save_one("bt/mesh/Net", (const u8_t *)&net, sizeof(net));
	if (err) {
		BT_ERR("Failed to store Network value");
	} else {
		BT_DBG("Stored Network value");
	}
}

void bt_mesh_store_net(void)
{
	schedule_store(BT_MESH_NET_PENDING);
}

static void store_pending_iv(void)
{
	struct iv_val iv;
	int err;

	iv.iv_index = bt_mesh.iv_index;
	iv.iv_update = atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS);
	iv.iv_duration = bt_mesh.ivu_duration;
	/* Fix warnning by bouffalo */
	err = settings_save_one("bt/mesh/IV", (const u8_t *)&iv, sizeof(iv));
	if (err) {
		BT_ERR("Failed to store IV value");
	} else {
		BT_DBG("Stored IV value");
	}
}

void bt_mesh_store_iv(bool only_duration)
{
	schedule_store(BT_MESH_IV_PENDING);

	if (!only_duration) {
		/* Always update Seq whenever IV changes */
		schedule_store(BT_MESH_SEQ_PENDING);
	}
}

static void store_pending_seq(void)
{
	struct seq_val seq;
	int err;

	sys_put_le24(bt_mesh.seq, seq.val);

	/* Fix warnning by bouffalo */
	err = settings_save_one("bt/mesh/Seq", (const u8_t *)&seq, sizeof(seq));
	if (err) {
		BT_ERR("Failed to stor Seq value");
	} else {
		BT_DBG("Stored Seq value");
	}
}

void bt_mesh_store_seq(void)
{
	if (CONFIG_BT_MESH_SEQ_STORE_RATE &&
	    (bt_mesh.seq % CONFIG_BT_MESH_SEQ_STORE_RATE)) {
		return;
	}

	schedule_store(BT_MESH_SEQ_PENDING);
}

#ifdef CONFIG_BT_SETTINGS /* Added by bouffalo */
static void store_rpl(struct bt_mesh_rpl *entry)
{
	struct rpl_val rpl;
	char path[18];
	int err;

	BT_DBG("src 0x%04x seq 0x%06lx old_iv %u", entry->src, entry->seq,
	       entry->old_iv);

	rpl.seq = entry->seq;
	rpl.old_iv = entry->old_iv;

	snprintk(path, sizeof(path), "bt/mesh/RPL/%x", entry->src);
	/* Fix warnning by bouffalo */
	err = settings_save_one(path, (u8_t*)&rpl, sizeof(rpl));
	if (err) {
		BT_ERR("Failed to store RPL %s value", log_strdup(path));
	} else {
		BT_DBG("Stored RPL %s value", log_strdup(path));
	}
}
#endif /*CONFIG_BT_SETTINGS*/
static void clear_rpl(void)
{
	int i, err;

	BT_DBG("");

	for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
		struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];
		char path[18];

		if (!rpl->src) {
			continue;
		}

		snprintk(path, sizeof(path), "bt/mesh/RPL/%x", rpl->src);
		err = settings_delete(path);
		if (err) {
			BT_ERR("Failed to clear RPL");
		} else {
			BT_DBG("Cleared RPL");
		}

		(void)memset(rpl, 0, sizeof(*rpl));
	}
}

static void store_pending_rpl(void)
{
	BT_DBG("");
#ifdef CONFIG_BT_SETTINGS
	for (int i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
		struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];

		if (rpl->store) {
			rpl->store = false;
			store_rpl(rpl);
		}
	}
#endif
}

static void store_pending_hb_pub(void)
{
	struct bt_mesh_hb_pub *pub = bt_mesh_hb_pub_get();
	struct hb_pub_val val;
	int err;

	if (!pub) {
		return;
	}

	if (pub->dst == BT_MESH_ADDR_UNASSIGNED) {
		err = settings_delete("bt/mesh/HBPub");
	} else {
		val.indefinite = (pub->count == 0xffff);
		val.dst = pub->dst;
		val.period = pub->period;
		val.ttl = pub->ttl;
		val.feat = pub->feat;
		val.net_idx = pub->net_idx;
        /* Fix warnning by bouffalo */
		err = settings_save_one("bt/mesh/HBPub", (const u8_t *)&val, sizeof(val));
	}

	if (err) {
		BT_ERR("Failed to store Heartbeat Publication");
	} else {
		BT_DBG("Stored Heartbeat Publication");
	}
}

static void store_pending_cfg(void)
{
	struct bt_mesh_cfg_srv *cfg = bt_mesh_cfg_get();
	struct cfg_val val;
	int err;

	if (!cfg) {
		return;
	}

	val.net_transmit = cfg->net_transmit;
	val.relay = cfg->relay;
	val.relay_retransmit = cfg->relay_retransmit;
	val.beacon = cfg->beacon;
	val.gatt_proxy = cfg->gatt_proxy;
	val.frnd = cfg->frnd;
	val.default_ttl = cfg->default_ttl;
	/* Fix warnning by bouffalo */
	err = settings_save_one("bt/mesh/Cfg", (const u8_t *)&val, sizeof(val));
	if (err) {
		BT_ERR("Failed to store configuration value");
	} else {
		BT_DBG("Stored configuration value");
        /* Comment by bouffalo */
		//BT_HEXDUMP_DBG(&val, sizeof(val), "raw value");
	}
}

static void clear_cfg(void)
{
	int err;

	err = settings_delete("bt/mesh/Cfg");
	if (err) {
		/* Modified by bouffalo */
		BT_ERR("Failed to clear configuration err(%d)", err);
	} else {
		BT_DBG("Cleared configuration");
	}
}

static void clear_app_key(u16_t app_idx)
{
	char path[20];
	int err;

	snprintk(path, sizeof(path), "bt/mesh/AppKey/%x", app_idx);
	err = settings_delete(path);
	if (err) {
		BT_ERR("Failed to clear AppKeyIndex 0x%03x", app_idx);
	} else {
		BT_DBG("Cleared AppKeyIndex 0x%03x", app_idx);
	}
}

static void clear_net_key(u16_t net_idx)
{
	char path[20];
	int err;

	BT_DBG("NetKeyIndex 0x%03x", net_idx);

	snprintk(path, sizeof(path), "bt/mesh/NetKey/%x", net_idx);
	err = settings_delete(path);
	if (err) {
		BT_ERR("Failed to clear NetKeyIndex 0x%03x", net_idx);
	} else {
		BT_DBG("Cleared NetKeyIndex 0x%03x", net_idx);
	}
}

static void store_net_key(struct bt_mesh_subnet *sub)
{
	struct net_key_val key;
	char path[20];
	int err;

	BT_DBG("NetKeyIndex 0x%03x NetKey %s", sub->net_idx,
	       bt_hex(sub->keys[0].net, 16));

	memcpy(&key.val[0], sub->keys[0].net, 16);
	memcpy(&key.val[1], sub->keys[1].net, 16);
	key.kr_flag = sub->kr_flag;
	key.kr_phase = sub->kr_phase;

	snprintk(path, sizeof(path), "bt/mesh/NetKey/%x", sub->net_idx);
	/* Fix warnning by bouffalo */
    err = settings_save_one(path, (const u8_t *)&key, sizeof(key));
	if (err) {
		BT_ERR("Failed to store NetKey value");
	} else {
		BT_DBG("Stored NetKey value");
	}
}

static void store_app_key(struct bt_mesh_app_key *app)
{
	struct app_key_val key;
	char path[20];
	int err;

	key.net_idx = app->net_idx;
	key.updated = app->updated;
	memcpy(key.val[0], app->keys[0].val, 16);
	memcpy(key.val[1], app->keys[1].val, 16);

	snprintk(path, sizeof(path), "bt/mesh/AppKey/%x", app->app_idx);
	/* Fix warnning by bouffalo */
	err = settings_save_one(path, (const u8_t *)&key, sizeof(key));
	if (err) {
		BT_ERR("Failed to store AppKey %s value", path);
	} else {
		BT_DBG("Stored AppKey %s value", path);
	}
}

static void store_pending_keys(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(key_updates); i++) {
		struct key_update *update = &key_updates[i];

		if (!update->valid) {
			continue;
		}

		if (update->clear) {
			if (update->app_key) {
				clear_app_key(update->key_idx);
			} else {
				clear_net_key(update->key_idx);
			}
		} else {
			if (update->app_key) {
				struct bt_mesh_app_key *key;

				key = bt_mesh_app_key_find(update->key_idx);
				if (key) {
					store_app_key(key);
				} else {
					BT_WARN("AppKeyIndex 0x%03x not found",
					       update->key_idx);
				}

			} else {
				struct bt_mesh_subnet *sub;

				sub = bt_mesh_subnet_get(update->key_idx);
				if (sub) {
					store_net_key(sub);
				} else {
					BT_WARN("NetKeyIndex 0x%03x not found",
					       update->key_idx);
				}
			}
		}

		update->valid = 0U;
	}
}

static void clear_cdb(void)
{
	int err;

	err = settings_delete("bt/mesh/cdb/Net");
	if (err) {
		BT_ERR("Failed to clear Network");
	} else {
		BT_DBG("Cleared Network");
	}
}

static void store_pending_cdb(void)
{
	struct cdb_net_val net;
	int err;

	BT_DBG("");

	net.iv_index = bt_mesh_cdb.iv_index;
	net.iv_update = atomic_test_bit(bt_mesh_cdb.flags,
					BT_MESH_CDB_IVU_IN_PROGRESS);
	/* Fix warnning by bouffalo */
	err = settings_save_one("bt/mesh/cdb/Net", (u8_t*)&net, sizeof(net));
	if (err) {
		BT_ERR("Failed to store Network value");
	} else {
		BT_DBG("Stored Network value");
	}
}

static void store_cdb_node(const struct bt_mesh_cdb_node *node)
{
	struct node_val val;
	char path[30];
	int err;

	val.net_idx = node->net_idx;
	val.num_elem = node->num_elem;
	val.flags = 0;

	if (atomic_test_bit(node->flags, BT_MESH_CDB_NODE_CONFIGURED)) {
		val.flags |= F_NODE_CONFIGURED;
	}

	memcpy(val.uuid, node->uuid, 16);
	memcpy(val.dev_key, node->dev_key, 16);

	snprintk(path, sizeof(path), "bt/mesh/cdb/Node/%x", node->addr);
	/* Fix warnning by bouffalo */
	err = settings_save_one(path, (u8_t*)&val, sizeof(val));
	if (err) {
		BT_ERR("Failed to store Node %s value", log_strdup(path));
	} else {
		BT_DBG("Stored Node %s value", log_strdup(path));
	}
}

static void clear_cdb_node(u16_t addr)
{
	char path[30];
	int err;

	BT_DBG("Node 0x%04x", addr);

	snprintk(path, sizeof(path), "bt/mesh/cdb/Node/%x", addr);
	err = settings_delete(path);
	if (err) {
		BT_ERR("Failed to clear Node 0x%04x", addr);
	} else {
		BT_DBG("Cleared Node 0x%04x", addr);
	}
}

static void store_pending_cdb_nodes(void)
{
/* Fix warnning by bouffalo */
#if defined(CONFIG_BT_MESH_CDB)
	int i;

	for (i = 0; i < ARRAY_SIZE(cdb_node_updates); ++i) {
		struct node_update *update = &cdb_node_updates[i];

		if (update->addr == BT_MESH_ADDR_UNASSIGNED) {
			continue;
		}

		BT_DBG("addr: 0x%04x, clear: %d", update->addr, update->clear);

		if (update->clear) {
			clear_cdb_node(update->addr);
		} else {
			struct bt_mesh_cdb_node *node;

			node = bt_mesh_cdb_node_get(update->addr);
			if (node) {
				store_cdb_node(node);
			} else {
				BT_WARN("Node 0x%04x not found", update->addr);
			}
		}

		update->addr = BT_MESH_ADDR_UNASSIGNED;
	}

/* Fix warnning by bouffalo */
#endif /* CONFIG_BT_MESH_CDB */
}

static void store_cdb_subnet(const struct bt_mesh_cdb_subnet *sub)
{
	struct net_key_val key;
	char path[30];
	int err;

	BT_DBG("NetKeyIndex 0x%03x NetKey %s", sub->net_idx,
	       bt_hex(sub->keys[0].net_key, 16));

	memcpy(&key.val[0], sub->keys[0].net_key, 16);
	memcpy(&key.val[1], sub->keys[1].net_key, 16);
	key.kr_flag = sub->kr_flag;
	key.kr_phase = sub->kr_phase;

	snprintk(path, sizeof(path), "bt/mesh/cdb/Subnet/%x", sub->net_idx);
	/* fix warnning by bouffalo */
	err = settings_save_one(path, (u8_t*)&key, sizeof(key));
	if (err) {
		BT_ERR("Failed to store Subnet value");
	} else {
		BT_DBG("Stored Subnet value");
	}
}

static void clear_cdb_subnet(u16_t net_idx)
{
	char path[30];
	int err;

	BT_DBG("NetKeyIndex 0x%03x", net_idx);

	snprintk(path, sizeof(path), "bt/mesh/cdb/Subnet/%x", net_idx);
	err = settings_delete(path);
	if (err) {
		BT_ERR("Failed to clear NetKeyIndex 0x%03x", net_idx);
	} else {
		BT_DBG("Cleared NetKeyIndex 0x%03x", net_idx);
	}
}

static void store_cdb_app_key(const struct bt_mesh_cdb_app_key *app)
{
	struct app_key_val key;
	char path[30];
	int err;

	key.net_idx = app->net_idx;
	key.updated = false;
	memcpy(key.val[0], app->keys[0].app_key, 16);
	memcpy(key.val[1], app->keys[1].app_key, 16);

	snprintk(path, sizeof(path), "bt/mesh/cdb/AppKey/%x", app->app_idx);
	/* Fix warnning by bouffalo */
	err = settings_save_one(path, (u8_t*)&key, sizeof(key));
	if (err) {
		BT_ERR("Failed to store AppKey %s value", log_strdup(path));
	} else {
		BT_DBG("Stored AppKey %s value", log_strdup(path));
	}
}

static void clear_cdb_app_key(u16_t app_idx)
{
	char path[30];
	int err;

	snprintk(path, sizeof(path), "bt/mesh/cdb/AppKey/%x", app_idx);
	err = settings_delete(path);
	if (err) {
		BT_ERR("Failed to clear AppKeyIndex 0x%03x", app_idx);
	} else {
		BT_DBG("Cleared AppKeyIndex 0x%03x", app_idx);
	}
}

static void store_pending_cdb_keys(void)
{
/* Fix warnning by bouffalo */
#if defined(CONFIG_BT_MESH_CDB)
	int i;

	for (i = 0; i < ARRAY_SIZE(cdb_key_updates); i++) {
		struct key_update *update = &cdb_key_updates[i];

		if (!update->valid) {
			continue;
		}

		if (update->clear) {
			if (update->app_key) {
				clear_cdb_app_key(update->key_idx);
			} else {
				clear_cdb_subnet(update->key_idx);
			}
		} else {
			if (update->app_key) {
				struct bt_mesh_cdb_app_key *key;

				key = bt_mesh_cdb_app_key_get(update->key_idx);
				if (key) {
					store_cdb_app_key(key);
				} else {
					BT_WARN("AppKeyIndex 0x%03x not found",
						update->key_idx);
				}
			} else {
				struct bt_mesh_cdb_subnet *sub;

				sub = bt_mesh_cdb_subnet_get(update->key_idx);
				if (sub) {
					store_cdb_subnet(sub);
				} else {
					BT_WARN("NetKeyIndex 0x%03x not found",
						update->key_idx);
				}
			}
		}

		update->valid = 0U;
	}
/* Fix warnning by bouffalo */
#endif /*CONFIG_BT_MESH_CDB*/
}

static struct node_update *cdb_node_update_find(u16_t addr,
					       struct node_update **free_slot)
{
	struct node_update *match;
/* Fix warnning by bouffalo */
#if defined(CONFIG_BT_MESH_CDB)
	int i;
#endif

	match = NULL;
	*free_slot = NULL;
/* Fix warnning by bouffalo */
#if defined(CONFIG_BT_MESH_CDB)

	for (i = 0; i < ARRAY_SIZE(cdb_node_updates); i++) {
		struct node_update *update = &cdb_node_updates[i];

		if (update->addr == BT_MESH_ADDR_UNASSIGNED) {
			*free_slot = update;
			continue;
		}

		if (update->addr == addr) {
			match = update;
		}
	}
/* Fix warnning by bouffalo */
#endif /*CONFIG_BT_MESH_CDB*/

	return match;
}

static void encode_mod_path(struct bt_mesh_model *mod, bool vnd,
			    const char *key, char *path, size_t path_len)
{
	u16_t mod_key = (((u16_t)mod->elem_idx << 8) | mod->mod_idx);

	if (vnd) {
		snprintk(path, path_len, "bt/mesh/v/%x/%s", mod_key, key);
	} else {
		snprintk(path, path_len, "bt/mesh/s/%x/%s", mod_key, key);
	}
}

static void store_pending_mod_bind(struct bt_mesh_model *mod, bool vnd)
{
	u16_t keys[CONFIG_BT_MESH_MODEL_KEY_COUNT];
	char path[20];
	int i, count, err = 0;

    #if defined(BFLB_BLE)
    memset(keys, 0, sizeof(keys));
    #endif

	for (i = 0, count = 0; i < ARRAY_SIZE(mod->keys); i++) {
		if (mod->keys[i] != BT_MESH_KEY_UNUSED) {
			keys[count++] = mod->keys[i];
			BT_DBG("model key 0x%04x", mod->keys[i]);
		}
	}

	encode_mod_path(mod, vnd, "bind", path, sizeof(path));

	if (count) {
		/*Fix warnning by bouffalo */
		err = settings_save_one(path, (u8_t*)keys, count * sizeof(keys[0]));
	} else {
		err = settings_delete(path);
	}

	if (err) {
		/* Modified by bouffalo */
        BT_ERR("Failed to store mod bind");
	} else {
		/* Modified by bouffalo */
        BT_DBG("Stored mod bind");
	}
}

static void store_pending_mod_sub(struct bt_mesh_model *mod, bool vnd)
{
	u16_t groups[CONFIG_BT_MESH_MODEL_GROUP_COUNT];
	char path[20];
	int i, count, err = 0;

    #if defined(BFLB_BLE)
    memset(groups, 0, sizeof(groups));
    #endif

	for (i = 0, count = 0; i < CONFIG_BT_MESH_MODEL_GROUP_COUNT; i++) {
		if (mod->groups[i] != BT_MESH_ADDR_UNASSIGNED) {
			groups[count++] = mod->groups[i];
		}
	}

	encode_mod_path(mod, vnd, "sub", path, sizeof(path));

	if (count) {
		/*Fix warnning by bouffalo */
		err = settings_save_one(path, (u8_t*)groups,
					count * sizeof(groups[0]));
	} else {
		err = settings_delete(path);
	}

	if (err) {
		/* Modified by bouffalo */
        BT_ERR("Failed to store mod sub");
	} else {
		/* Modified by bouffalo */
        BT_DBG("Stored mod sub");
	}
}

static void store_pending_mod_pub(struct bt_mesh_model *mod, bool vnd)
{
	struct mod_pub_val pub;
	char path[20];
	int err = 0;/* Init by bouffalo */

	encode_mod_path(mod, vnd, "pub", path, sizeof(path));

	if (!mod->pub || mod->pub->addr == BT_MESH_ADDR_UNASSIGNED) {
		err = settings_delete(path);
	} else {
		pub.addr = mod->pub->addr;
		pub.key = mod->pub->key;
		pub.ttl = mod->pub->ttl;
		pub.retransmit = mod->pub->retransmit;
		pub.period = mod->pub->period;
		pub.period_div = mod->pub->period_div;
		pub.cred = mod->pub->cred;

		/*Fix warnning by bouffalo */
	     err = settings_save_one(path, (u8_t*)&pub, sizeof(pub));
	}

	if (err) {
		/* Modified by bouffalo */
        BT_ERR("Failed to store mod sub");
	} else {
		/* Modified by bouffalo */
        BT_DBG("Stored value");
	}
}

static void store_pending_mod(struct bt_mesh_model *mod,
			      struct bt_mesh_elem *elem, bool vnd,
			      bool primary, void *user_data)
{
	if (!mod->flags) {
		return;
	}

	if (mod->flags & BT_MESH_MOD_BIND_PENDING) {
		mod->flags &= ~BT_MESH_MOD_BIND_PENDING;
		store_pending_mod_bind(mod, vnd);
	}

	if (mod->flags & BT_MESH_MOD_SUB_PENDING) {
		mod->flags &= ~BT_MESH_MOD_SUB_PENDING;
		store_pending_mod_sub(mod, vnd);
	}

	if (mod->flags & BT_MESH_MOD_PUB_PENDING) {
		mod->flags &= ~BT_MESH_MOD_PUB_PENDING;
		store_pending_mod_pub(mod, vnd);
	}
}

#define IS_VA_DEL(_label)	((_label)->ref == 0)
static void store_pending_va(void)
{
	struct label *lab;
	struct va_val va;
	char path[18];
	u16_t i;
	int err;

	for (i = 0; (lab = get_label(i)) != NULL; i++) {
		if (!atomic_test_and_clear_bit(lab->flags,
					       BT_MESH_VA_CHANGED)) {
			continue;
		}

		snprintk(path, sizeof(path), "bt/mesh/Va/%x", i);

		if (IS_VA_DEL(lab)) {
			err = settings_delete(path);
		} else {
			va.ref = lab->ref;
			va.addr = lab->addr;
			memcpy(va.uuid, lab->uuid, 16);
			/* Fix warnning by bouffalo */
			err = settings_save_one(path, (u8_t*)&va, sizeof(va));
		}

		if (err) {
			BT_ERR("Failed to %s %s value (err %d)",
			       IS_VA_DEL(lab) ? "delete" : "store",
			       log_strdup(path), err);
		} else {
			BT_DBG("%s %s value",
			       IS_VA_DEL(lab) ? "Deleted" : "Stored",
			       log_strdup(path));
		}
	}
}

static void store_pending(struct k_work *work)
{
	BT_DBG("");

	if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_RPL_PENDING)) {
		if (atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
			store_pending_rpl();
		} else {
			clear_rpl();
		}
	}

	if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_KEYS_PENDING)) {
		store_pending_keys();
	}

	if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_NET_PENDING)) {
		if (atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
			store_pending_net();
		} else {
			clear_net();
		}
	}

	if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_IV_PENDING)) {
		if (atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
			store_pending_iv();
		} else {
			clear_iv();
		}
	}

	if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_SEQ_PENDING)) {
		store_pending_seq();
	}

	if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_HB_PUB_PENDING)) {
		store_pending_hb_pub();
	}

	if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_CFG_PENDING)) {
		if (atomic_test_bit(bt_mesh.flags, BT_MESH_VALID)) {
			store_pending_cfg();
		} else {
			clear_cfg();
		}
	}

	if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_MOD_PENDING)) {
		bt_mesh_model_foreach(store_pending_mod, NULL);
	}

	if (atomic_test_and_clear_bit(bt_mesh.flags, BT_MESH_VA_PENDING)) {
		store_pending_va();
	}

	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		if (atomic_test_and_clear_bit(bt_mesh_cdb.flags,
					      BT_MESH_CDB_SUBNET_PENDING)) {
			if (atomic_test_bit(bt_mesh_cdb.flags,
					    BT_MESH_CDB_VALID)) {
				store_pending_cdb();
			} else {
				clear_cdb();
			}
		}

		if (atomic_test_and_clear_bit(bt_mesh_cdb.flags,
					      BT_MESH_CDB_NODES_PENDING)) {
			store_pending_cdb_nodes();
		}

		if (atomic_test_and_clear_bit(bt_mesh_cdb.flags,
					      BT_MESH_CDB_KEYS_PENDING)) {
			store_pending_cdb_keys();
		}
	}
}

void bt_mesh_store_rpl(struct bt_mesh_rpl *entry)
{
#if !defined(BFLB_BLE)
#ifdef CONFIG_BT_SETTINGS
	entry->store = true;
#endif
	schedule_store(BT_MESH_RPL_PENDING);
#endif /* BFLB_BLE */
}

static struct key_update *key_update_find(bool app_key, u16_t key_idx,
					  struct key_update **free_slot)
{
	struct key_update *match;
	int i;

	match = NULL;
	*free_slot = NULL;

	for (i = 0; i < ARRAY_SIZE(key_updates); i++) {
		struct key_update *update = &key_updates[i];

		if (!update->valid) {
			*free_slot = update;
			continue;
		}

		if (update->app_key != app_key) {
			continue;
		}

		if (update->key_idx == key_idx) {
			match = update;
		}
	}

	return match;
}

void bt_mesh_store_subnet(struct bt_mesh_subnet *sub)
{
	struct key_update *update, *free_slot;

	BT_DBG("NetKeyIndex 0x%03x", sub->net_idx);

	update = key_update_find(false, sub->net_idx, &free_slot);
	if (update) {
		update->clear = 0U;
		schedule_store(BT_MESH_KEYS_PENDING);
		return;
	}

	if (!free_slot) {
		store_net_key(sub);
		return;
	}

	free_slot->valid = 1U;
	free_slot->key_idx = sub->net_idx;
	free_slot->app_key = 0U;
	free_slot->clear = 0U;

	schedule_store(BT_MESH_KEYS_PENDING);
}

void bt_mesh_store_app_key(struct bt_mesh_app_key *key)
{
	struct key_update *update, *free_slot;

	BT_DBG("AppKeyIndex 0x%03x", key->app_idx);

	update = key_update_find(true, key->app_idx, &free_slot);
	if (update) {
		update->clear = 0U;
		schedule_store(BT_MESH_KEYS_PENDING);
		return;
	}

	if (!free_slot) {
		store_app_key(key);
		return;
	}

	free_slot->valid = 1U;
	free_slot->key_idx = key->app_idx;
	free_slot->app_key = 1U;
	free_slot->clear = 0U;

	schedule_store(BT_MESH_KEYS_PENDING);
}

void bt_mesh_store_hb_pub(void)
{
	schedule_store(BT_MESH_HB_PUB_PENDING);
}

void bt_mesh_store_cfg(void)
{
	schedule_store(BT_MESH_CFG_PENDING);
}

void bt_mesh_clear_net(void)
{
	schedule_store(BT_MESH_NET_PENDING);
	schedule_store(BT_MESH_IV_PENDING);
	schedule_store(BT_MESH_CFG_PENDING);
}

void bt_mesh_clear_subnet(struct bt_mesh_subnet *sub)
{
	struct key_update *update, *free_slot;

	BT_DBG("NetKeyIndex 0x%03x", sub->net_idx);

	update = key_update_find(false, sub->net_idx, &free_slot);
	if (update) {
		update->clear = 1U;
		schedule_store(BT_MESH_KEYS_PENDING);
		return;
	}

	if (!free_slot) {
		clear_net_key(sub->net_idx);
		return;
	}

	free_slot->valid = 1U;
	free_slot->key_idx = sub->net_idx;
	free_slot->app_key = 0U;
	free_slot->clear = 1U;

	schedule_store(BT_MESH_KEYS_PENDING);
}

void bt_mesh_clear_app_key(struct bt_mesh_app_key *key)
{
	struct key_update *update, *free_slot;

	BT_DBG("AppKeyIndex 0x%03x", key->app_idx);

	update = key_update_find(true, key->app_idx, &free_slot);
	if (update) {
		update->clear = 1U;
		schedule_store(BT_MESH_KEYS_PENDING);
		return;
	}

	if (!free_slot) {
		clear_app_key(key->app_idx);
		return;
	}

	free_slot->valid = 1U;
	free_slot->key_idx = key->app_idx;
	free_slot->app_key = 1U;
	free_slot->clear = 1U;

	schedule_store(BT_MESH_KEYS_PENDING);
}

void bt_mesh_clear_rpl(void)
{
	schedule_store(BT_MESH_RPL_PENDING);
}

/* Added by bouffalolab */
void bt_mesh_clear_node_rpl(u16_t src)
{
	int i;

	BT_DBG("");

	for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
		struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];

		if (rpl->src == src) {
			#ifdef CONFIG_BT_SETTINGS
			char path[18];
			snprintk(path, sizeof(path), "bt/mesh/RPL/%x", rpl->src);
			int err = settings_delete(path);
			if (err) {
				BT_ERR("Failed to clear RPL");
			} else {
				BT_DBG("Cleared RPL");
			}
			#endif

			(void)memset(rpl, 0, sizeof(*rpl));
			break;
		}
	}
}

void bt_mesh_store_mod_bind(struct bt_mesh_model *mod)
{
	mod->flags |= BT_MESH_MOD_BIND_PENDING;
	schedule_store(BT_MESH_MOD_PENDING);
}

void bt_mesh_store_mod_sub(struct bt_mesh_model *mod)
{
	mod->flags |= BT_MESH_MOD_SUB_PENDING;
	schedule_store(BT_MESH_MOD_PENDING);
}

void bt_mesh_store_mod_pub(struct bt_mesh_model *mod)
{
	mod->flags |= BT_MESH_MOD_PUB_PENDING;
	schedule_store(BT_MESH_MOD_PENDING);
}


void bt_mesh_store_label(void)
{
	schedule_store(BT_MESH_VA_PENDING);
}

static void schedule_cdb_store(int flag)
{
	atomic_set_bit(bt_mesh_cdb.flags, flag);
	k_delayed_work_submit(&pending_store, K_NO_WAIT);
}

void bt_mesh_store_cdb(void)
{
	schedule_cdb_store(BT_MESH_CDB_SUBNET_PENDING);
}

void bt_mesh_store_cdb_node(const struct bt_mesh_cdb_node *node)
{
	struct node_update *update, *free_slot;

	BT_DBG("Node 0x%04x", node->addr);

	update = cdb_node_update_find(node->addr, &free_slot);
	if (update) {
		update->clear = false;
		schedule_cdb_store(BT_MESH_CDB_NODES_PENDING);
		return;
	}

	if (!free_slot) {
		store_cdb_node(node);
		return;
	}

	free_slot->addr = node->addr;
	free_slot->clear = false;

	schedule_cdb_store(BT_MESH_CDB_NODES_PENDING);
}

void bt_mesh_clear_cdb_node(struct bt_mesh_cdb_node *node)
{
	struct node_update *update, *free_slot;

	BT_DBG("Node 0x%04x", node->addr);

	update = cdb_node_update_find(node->addr, &free_slot);
	if (update) {
		update->clear = true;
		schedule_cdb_store(BT_MESH_CDB_NODES_PENDING);
		return;
	}

	if (!free_slot) {
		clear_cdb_node(node->addr);
		return;
	}

	free_slot->addr = node->addr;
	free_slot->clear = true;

	schedule_cdb_store(BT_MESH_CDB_NODES_PENDING);
}

/* TODO: Could be shared with key_update_find? */
static struct key_update *cdb_key_update_find(bool app_key, u16_t key_idx,
					     struct key_update **free_slot)
{
	struct key_update *match;
#if defined(CONFIG_BT_MESH_CDB)
	int i;
#endif

	match = NULL;
	*free_slot = NULL;

/* Fix warnning by bouffalo */
#if defined(CONFIG_BT_MESH_CDB)

	for (i = 0; i < ARRAY_SIZE(cdb_key_updates); i++) {
		struct key_update *update = &cdb_key_updates[i];

		if (!update->valid) {
			*free_slot = update;
			continue;
		}

		if (update->app_key != app_key) {
			continue;
		}

		if (update->key_idx == key_idx) {
			match = update;
		}
	}
/* Fix warnning by bouffalo */
#endif /*CONFIG_BT_MESH_CDB*/

	return match;
}

void bt_mesh_store_cdb_subnet(const struct bt_mesh_cdb_subnet *sub)
{
	struct key_update *update, *free_slot;

	BT_DBG("NetKeyIndex 0x%03x", sub->net_idx);

	update = cdb_key_update_find(false, sub->net_idx, &free_slot);
	if (update) {
		update->clear = 0U;
		schedule_cdb_store(BT_MESH_CDB_KEYS_PENDING);
		return;
	}

	if (!free_slot) {
		store_cdb_subnet(sub);
		return;
	}

	free_slot->valid = 1U;
	free_slot->key_idx = sub->net_idx;
	free_slot->app_key = 0U;
	free_slot->clear = 0U;

	schedule_cdb_store(BT_MESH_CDB_KEYS_PENDING);
}

void bt_mesh_clear_cdb_subnet(struct bt_mesh_cdb_subnet *sub)
{
	struct key_update *update, *free_slot;

	BT_DBG("NetKeyIndex 0x%03x", sub->net_idx);

	update = cdb_key_update_find(false, sub->net_idx, &free_slot);
	if (update) {
		update->clear = 1U;
		schedule_cdb_store(BT_MESH_CDB_KEYS_PENDING);
		return;
	}

	if (!free_slot) {
		clear_cdb_subnet(sub->net_idx);
		return;
	}

	free_slot->valid = 1U;
	free_slot->key_idx = sub->net_idx;
	free_slot->app_key = 0U;
	free_slot->clear = 1U;

	schedule_cdb_store(BT_MESH_CDB_KEYS_PENDING);
}

void bt_mesh_store_cdb_app_key(const struct bt_mesh_cdb_app_key *key)
{
	struct key_update *update, *free_slot;

	BT_DBG("AppKeyIndex 0x%03x", key->app_idx);

	update = cdb_key_update_find(true, key->app_idx, &free_slot);
	if (update) {
		update->clear = 0U;
		schedule_cdb_store(BT_MESH_CDB_KEYS_PENDING);
		return;
	}

	if (!free_slot) {
		store_cdb_app_key(key);
		return;
	}

	free_slot->valid = 1U;
	free_slot->key_idx = key->app_idx;
	free_slot->app_key = 1U;
	free_slot->clear = 0U;

	schedule_cdb_store(BT_MESH_CDB_KEYS_PENDING);
}

void bt_mesh_clear_cdb_app_key(struct bt_mesh_cdb_app_key *key)
{
	struct key_update *update, *free_slot;

	BT_DBG("AppKeyIndex 0x%03x", key->app_idx);

	update = cdb_key_update_find(true, key->app_idx, &free_slot);
	if (update) {
		update->clear = 1U;
		schedule_cdb_store(BT_MESH_CDB_KEYS_PENDING);
		return;
	}

	if (!free_slot) {
		clear_cdb_app_key(key->app_idx);
		return;
	}

	free_slot->valid = 1U;
	free_slot->key_idx = key->app_idx;
	free_slot->app_key = 1U;
	free_slot->clear = 1U;

	schedule_cdb_store(BT_MESH_CDB_KEYS_PENDING);
}

int bt_mesh_model_data_store(struct bt_mesh_model *mod, bool vnd,
			const char *name, const void *data,
			size_t data_len)
{
	char path[30];
	int err;

	encode_mod_path(mod, vnd, "data", path, sizeof(path));
	if (name) {
		strlcat(path, "/", sizeof(path));
		strlcat(path, name, 8);
	}

	if (data_len) {
		err = settings_save_one(path, data, data_len);
	} else {
		err = settings_delete(path);
	}

	if (err) {
		BT_ERR("Failed to store %s value", log_strdup(path));
	} else {
		BT_DBG("Stored %s value", log_strdup(path));
	}
	return err;
}

void bt_mesh_settings_init(void)
{
	k_delayed_work_init(&pending_store, store_pending);
}

#if defined(CONFIG_BT_SETTINGS)

/* Added by bouffalo */
#define MESH_SETTINGS_DIR "bt/mesh"

/* Added by bouffalo */
static ssize_t mesh_settings_read_cb(void *cb_arg, void *data, size_t data_len)
{
	const char *key_name = (const char *)cb_arg;
	size_t read_len, tot_len;

	read_len = ef_get_env_blob(key_name, data, data_len, &tot_len);
	if (read_len == tot_len) {
		BT_DBG("read data[%s]", bt_hex(data, data_len));
		return read_len;
	}

	BT_ERR("Invalid value length");
	return 0;
}

/* Added by bouffalo */
#if defined(CONFIG_IOT_SDK)
static bool setting_env_cb(env_node_obj_t env, void *arg1, void *arg2)
#else
static EfErrCode setting_env_cb(const char *key_name, void *arg)
#endif /* CONFIG_IOT_SDK */
{
#if defined(CONFIG_IOT_SDK)
    if (!(env->crc_is_ok && env->status == ENV_WRITE)) {
		 return false;
	}
	const char *key_name = env->name;
	int key_name_len = env->name_len;
	BT_WARN("Env[%.*s] Data len[%lu]", env->name_len, env->name, env->value_len);
#else
	int key_name_len = strlen(key_name);
#endif /* CONFIG_IOT_SDK */
	
	char *pname, name[EF_ENV_NAME_MAX+1];
	const char* next;

	pname = name;
	memcpy(pname, key_name, key_name_len);
	pname[key_name_len] = 0;
	BT_DBG("len[%d] env:[%s]", key_name_len, pname);

	/* Check mesh index*/
	if(0 != memcmp(pname, MESH_SETTINGS_DIR, strlen(MESH_SETTINGS_DIR))){
		return EF_NO_ERR;
	}
	pname += strlen(MESH_SETTINGS_DIR);
	if(*pname != '/'){
		return EF_NO_ERR;
	}
	pname += 1;

	for(int i = 0; i < ARRAY_SIZE(settings); i++){
		int str_len = strlen(settings[i].name);
		if(0 == strncmp(pname, settings[i].name, str_len)
			&& str_len == settings_name_next(pname, &next) ){
			pname += str_len;
			BT_DBG("Total name[%s]", name);
			if(*pname != '/'){
				BT_DBG("current name[NULL]");
				settings[i].func(NULL, key_name_len, mesh_settings_read_cb, (void*)name);
			}
			else{
				BT_DBG("current name[%s]", pname+1);
				settings[i].func(pname+1, key_name_len, mesh_settings_read_cb, (void*)name);
			}
			break;
		}
	}
	return EF_NO_ERR;
}


/* Added by bouffalo */
void load_mesh_setting(void)
{
#if defined(CONFIG_IOT_SDK)
	ef_print_env_cb(setting_env_cb);
#else
	ef_foreach_env(setting_env_cb, NULL);
#endif /* CONFIG_IOT_SDK */
}
#endif /* CONFIG_BT_SETTINGS */


/* Added by bouffalo */
static int settings_name_next(const char *name, const char **next)
{
	int rc = 0;

	if (next) {
		*next = NULL;
	}

	if (!name) {
		return 0;
	}

	/* name might come from flash directly, in flash the name would end
	 * with '=' or '\0' depending how storage is done. Flash reading is
	 * limited to what can be read
	 */
	while ((*name != '\0') && (*name != '=') &&
	       (*name != '/')) {
		rc++;
		name++;
	}

	if (*name == '/') {
		if (next) {
			*next = name + 1;
		}
		return rc;
	}

	return rc;
}
