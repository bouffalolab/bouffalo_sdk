/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <bt_errno.h>
#include <util.h>

#include <net/buf.h>
#include <bluetooth.h>
#include <conn.h>
#include <include/mesh.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_BEACON)
#define LOG_MODULE_NAME bt_mesh_beacon
#include "bt_log.h"

#include "adv.h"
#include "mesh.h"
#include "net.h"
#include "prov.h"
#include "crypto.h"
#include "../../blestack/src/include/bluetooth/crypto.h"
#include "beacon.h"
#include "foundation.h"
#include "include/cfg.h"

#define UNPROVISIONED_INTERVAL     K_SECONDS(5)
#define PROVISIONED_INTERVAL       K_SECONDS(10)

#define BEACON_TYPE_UNPROVISIONED  0x00
#define BEACON_TYPE_SECURE         0x01
#define BEACON_TYPE_PRIVATE        0x02

/* 3 transmissions, 20ms interval */
#define UNPROV_XMIT                BT_MESH_TRANSMIT(2, 20)

/* 1 transmission, 20ms interval */
#define PROV_XMIT                  BT_MESH_TRANSMIT(0, 20)

static struct k_delayed_work beacon_timer;
#if defined(CONFIG_BT_MESH_V1d1)
#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
static struct {
	/**
	 * Identifier for the current Private beacon random-value.
	 * Each time we regenerate the random-value, we'll update this idx.
	 * Whenever it's time for a subnet to create a beacon, it'll compare
	 * the subnet's beacon idx to determine whether the random value has
	 * changed since the last beacon was sent. If this is the case, we'll
	 * regenerate the beacon based on the new random value.
	 */
	uint16_t idx;
	uint8_t val[13];
	uint64_t timestamp;
} priv_random;
#endif /* CONFIG_BT_MESH_PRIV_BEACONS */

struct beacon_params {
	bool private;
	union {
		const uint8_t *net_id;
		struct {
			const uint8_t *data;
			const uint8_t *random;
		};
	};
	const uint8_t *auth;
	uint32_t iv_index;
	uint8_t flags;

	bool new_key;
};

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
static int private_beacon_create(struct bt_mesh_subnet *sub,
				 struct net_buf_simple *buf);
static int private_beacon_update(struct bt_mesh_subnet *sub);
#endif

static struct bt_mesh_beacon *subnet_beacon_get_by_type(struct bt_mesh_subnet *sub, bool priv)
{
#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
	return priv ? &sub->priv_beacon : &sub->secure_beacon;
#else
	return &sub->secure_beacon;
#endif
}

static bool beacon_cache_match(struct bt_mesh_subnet *sub, void *data)
{
	struct beacon_params *params;
	struct bt_mesh_beacon *beacon;

	params = data;
	beacon = subnet_beacon_get_by_type(sub, params->private);

	return !memcmp(beacon->cache, params->auth, sizeof(beacon->cache));
}
#else
static struct bt_mesh_subnet *cache_check(u8_t data[21])
{
	int i;

	for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

		if (sub->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		if (!memcmp(sub->beacon_cache, data, 21)) {
			return sub;
		}
	}

	return NULL;
}
#endif /* CONFIG_BT_MESH_V1d1 */

#if defined(CONFIG_BT_MESH_V1d1)
static void cache_add(const uint8_t auth[8], struct bt_mesh_beacon *beacon)
{
	memcpy(beacon->cache, auth, sizeof(beacon->cache));
}
#else
static void cache_add(u8_t data[21], struct bt_mesh_subnet *sub)
{
	memcpy(sub->beacon_cache, data, 21);
}
#endif /* CONFIG_BT_MESH_V1d1 */

static void beacon_complete(int err, void *user_data)
{
#if defined(CONFIG_BT_MESH_V1d1)
	struct bt_mesh_beacon *beacon = user_data;

	LOG_DBG("err %d", err);

	beacon->sent = k_uptime_get_32();
#else
	struct bt_mesh_subnet *sub = user_data;

	BT_DBG("err %d", err);

	sub->beacon_sent = k_uptime_get_32();
#endif /*  */
}

#if defined(CONFIG_BT_MESH_V1d1)
static int secure_beacon_create(struct bt_mesh_subnet *sub,
				struct net_buf_simple *buf)
{
	uint8_t flags = bt_mesh_net_flags(sub);
	struct bt_mesh_subnet_keys *keys;

	net_buf_simple_add_u8(buf, BEACON_TYPE_SECURE);

	/*if (sub->kr_flag) {
		keys = &sub->keys[1];
	} else {
		keys = &sub->keys[0];
	} old code */
	keys = &sub->keys[SUBNET_KEY_TX_IDX(sub)];

	net_buf_simple_add_u8(buf, flags);

	/* Network ID */
	net_buf_simple_add_mem(buf, keys->net_id, 8);

	/* IV Index */
	net_buf_simple_add_be32(buf, bt_mesh.iv_index);

	/*net_buf_simple_add_mem(buf, sub->auth, 8); old code*/
	net_buf_simple_add_mem(buf, sub->secure_beacon.auth, 8);

	LOG_DBG("net_idx 0x%04x flags 0x%02x NetID %s", sub->net_idx, flags,
		bt_hex(keys->net_id, 8));
	LOG_DBG("IV Index 0x%08lx Auth %s", bt_mesh.iv_index, bt_hex(sub->secure_beacon.auth, 8));

	return 0;
}

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
static int private_random_update(void)
{
	uint8_t interval = bt_mesh_priv_beacon_update_interval_get();
	uint64_t uptime = k_uptime_get();
	int err;

	/* The Private beacon random value should change every N seconds to maintain privacy.
	 * N = (10 * interval) seconds, or on every beacon creation, if the interval is 0.
	 */
	if (bt_mesh_priv_beacon_get() == BT_MESH_FEATURE_ENABLED &&
	    interval &&
	    uptime - priv_random.timestamp < (10 * interval * MSEC_PER_SEC) &&
	    priv_random.timestamp != 0) {
		/* Not time yet */
		return 0;
	}

	err = bt_rand(priv_random.val, sizeof(priv_random.val));
	if (err) {
		return err;
	}

	/* Update the index to indicate to all subnets that the private beacon must be regenerated.
	 * Each subnet maintains the random index their private beacon data was generated with.
	 */
	priv_random.idx++;
	priv_random.timestamp = uptime;

	return 0;
}

static int private_beacon_update(struct bt_mesh_subnet *sub)
{
	struct bt_mesh_subnet_keys *keys = &sub->keys[SUBNET_KEY_TX_IDX(sub)];
	uint8_t flags = bt_mesh_net_flags(sub);
	int err;
	BT_PTS("");
	err = bt_mesh_beacon_encrypt(&keys->priv_beacon, flags, bt_mesh.iv_index,
				     priv_random.val, sub->priv_beacon_ctx.data,
				     sub->priv_beacon.auth);
	if (err) {
		LOG_ERR("Can't encrypt private beacon");
		return err;
	}

	sub->priv_beacon_ctx.idx = priv_random.idx;
	return 0;
}

static int private_beacon_create(struct bt_mesh_subnet *sub,
				 struct net_buf_simple *buf)
{
	int err;

	/* Refresh beacon data */
	err = private_random_update();
	if (err) {
		return err;
	}
	BT_PTS("0x%03x ub->priv_beacon_ctx.idx:%d priv_random.idx:%d", 
			sub->net_idx, sub->priv_beacon_ctx.idx, priv_random.idx);
	if (sub->priv_beacon_ctx.idx != priv_random.idx) {
		err = private_beacon_update(sub);
		if (err) {
			return err;
		}
	}

	net_buf_simple_add_u8(buf, BEACON_TYPE_PRIVATE);
	net_buf_simple_add_mem(buf, priv_random.val, 13);
	net_buf_simple_add_mem(buf, sub->priv_beacon_ctx.data, 5);
	net_buf_simple_add_mem(buf, sub->priv_beacon.auth, 8);

	BT_PTS("0x%03x", sub->net_idx);
	return 0;
}
#endif /* CONFIG_BT_MESH_PRIV_BEACONS */

int bt_mesh_beacon_create(struct bt_mesh_subnet *sub,
			   struct net_buf_simple *buf, bool priv)
{
#if defined(CONFIG_AUTO_PTS)
	BT_PTS("priv:%d", priv);
#endif /* CONFIG_AUTO_PTS */

	if (priv) {
		return private_beacon_create(sub, buf);
	}

	secure_beacon_create(sub, buf);
	return 0;
}
#else
void bt_mesh_beacon_create(struct bt_mesh_subnet *sub,
			   struct net_buf_simple *buf)
{
	u8_t flags = bt_mesh_net_flags(sub);
	struct bt_mesh_subnet_keys *keys;

	net_buf_simple_add_u8(buf, BEACON_TYPE_SECURE);

	if (sub->kr_flag) {
		keys = &sub->keys[1];
	} else {
		keys = &sub->keys[0];
	}

	net_buf_simple_add_u8(buf, flags);

	/* Network ID */
	net_buf_simple_add_mem(buf, keys->net_id, 8);

	/* IV Index */
	net_buf_simple_add_be32(buf, bt_mesh.iv_index);

	net_buf_simple_add_mem(buf, sub->auth, 8);

	BT_DBG("net_idx 0x%04x flags 0x%02x NetID %s", sub->net_idx,
	       flags, bt_hex(keys->net_id, 8));
	BT_DBG("IV Index 0x%08lx Auth %s", bt_mesh.iv_index,
	       bt_hex(sub->auth, 8));
}
#endif /* CONFIG_BT_MESH_V1d1 */

/* If the interval has passed or is within 5 seconds from now send a beacon */
#if defined(CONFIG_BT_MESH_V1d1)
#define BEACON_THRESHOLD(beacon) \
	((10 * ((beacon)->last + 1)) * MSEC_PER_SEC - (5 * MSEC_PER_SEC))
#else
#define BEACON_THRESHOLD(sub) \
	((10 * ((sub)->beacons_last + 1)) * MSEC_PER_SEC - (5 * MSEC_PER_SEC))
#endif /* CONFIG_BT_MESH_V1d1 */

#if defined(CONFIG_BT_MESH_V1d1)
static bool secure_beacon_is_running(void)
{
	return bt_mesh_beacon_enabled() ||
	       atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR);
}
#endif /* CONFIG_BT_MESH_V1d1 */

#if !defined(CONFIG_BT_MESH_V1d1)
static int secure_beacon_send(void)
{
	static const struct bt_mesh_send_cb send_cb = {
		.end = beacon_complete,
	};
	u32_t now = k_uptime_get_32();
	int i;

	BT_DBG("");

	for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		struct bt_mesh_subnet *sub = &bt_mesh.sub[i];
		struct net_buf *buf;
		u32_t time_diff;

		if (sub->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		time_diff = now - sub->beacon_sent;
		if (time_diff < (600 * MSEC_PER_SEC) &&
		    time_diff < BEACON_THRESHOLD(sub)) {
			continue;
		}

		buf = bt_mesh_adv_create(BT_MESH_ADV_BEACON, PROV_XMIT,
					 K_NO_WAIT);
		if (!buf) {
			BT_ERR("Unable to allocate beacon buffer");
			return -ENOBUFS;
		}

		bt_mesh_beacon_create(sub, &buf->b);

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
		static u32_t pts_cnt = 0;
		BT_PTS("[PTS] Sending secure network beacon (Flags = 0x%02X, IV Index = 0x%08lX) %lu", bt_mesh_net_flags(sub), bt_mesh.iv_index, ++pts_cnt);
#endif

		bt_mesh_adv_send(buf, &send_cb, sub);
		net_buf_unref(buf);
	}

	return 0;
}
#endif /* CONFIG_BT_MESH_V1d1 */

#if defined(CONFIG_BT_MESH_V1d1)
static bool net_beacon_send(struct bt_mesh_subnet *sub, struct bt_mesh_beacon *beacon,
			    void *cb_data, int (*beacon_create)(struct bt_mesh_subnet *sub,
								struct net_buf_simple *buf))
{
	static const struct bt_mesh_send_cb send_cb = {
		.end = beacon_complete,
	};
	uint32_t now = k_uptime_get_32();
	struct net_buf *adv; //struct bt_mesh_adv *adv; origin code 
	uint32_t time_diff;
	uint32_t time_since_last_recv;
	int err;

	LOG_DBG("");

	time_diff = now - beacon->sent;
	time_since_last_recv = now - beacon->recv;// this line is new, why shall greater than 10 s
	if (time_diff < (600 * MSEC_PER_SEC) &&
		(time_diff < BEACON_THRESHOLD(beacon) ||
		 time_since_last_recv < (10 * MSEC_PER_SEC))) { 
			LOG_DBG("1");
		//return false;
	}

	#if defined(CONFIG_BT_MESH_V1d1) && defined(CONFIG_AUTO_PTS)
	adv = bt_mesh_adv_create_with_options(BT_MESH_ADV_BEACON, PROV_XMIT,
					 K_NO_WAIT, BT_LE_ADV_OPT_USE_NRPA);
	#else
	adv = bt_mesh_adv_create(BT_MESH_ADV_BEACON, PROV_XMIT,
					 K_NO_WAIT);
	#endif /* CONFIG_BT_MESH_V1d1 && CONFIG_AUTO_PTS */
	if (!adv) {
		LOG_ERR("Unable to allocate beacon adv");
		return true; /* Bail out */
	}
	
	err = beacon_create(sub, &adv->b);

	/* Add by bouffallalab */
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	static u32_t pts_cnt = 0;
	if(beacon_create == private_beacon_create){
		BT_PTS("[PTS] Sending private network beacon (Flags = 0x%02X, IV Index = 0x%08lX) %lu", bt_mesh_net_flags(sub), bt_mesh.iv_index, ++pts_cnt);
	}
	else{
		BT_PTS("[PTS] Sending secure network beacon (Flags = 0x%02X, IV Index = 0x%08lX) %lu", bt_mesh_net_flags(sub), bt_mesh.iv_index, ++pts_cnt);
	}
	
#endif

	if (!err) {
		bt_mesh_adv_send(adv, &send_cb, beacon);
	}

	net_buf_unref(adv);//bt_mesh_adv_unref(adv); origin code

	return err != 0;
}

static bool net_beacon_for_subnet_send(struct bt_mesh_subnet *sub, void *cb_data)
{
	bool res = true;

	struct {
		struct bt_mesh_beacon *beacon;
		bool enabled;
		int (*create_fn)(struct bt_mesh_subnet *sub, struct net_buf_simple *buf);
	} beacons[] = {
		[0] = {
			.beacon = &sub->secure_beacon,
			.enabled = secure_beacon_is_running(),
			.create_fn = secure_beacon_create,
		},
#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
		[1] = {
			.beacon = &sub->priv_beacon,
			.enabled = bt_mesh_priv_beacon_get() == BT_MESH_FEATURE_ENABLED,
			.create_fn = private_beacon_create,
		},
#endif
	};

	for (int i = 0; i < ARRAY_SIZE(beacons); i++) {
		if (!beacons[i].enabled) {
			continue;
		}
		#if defined(CONFIG_AUTO_PTS)
		BT_PTS("%d %d", i, beacons[i].enabled);
		#endif /* CONFIG_AUTO_PTS */
		res = net_beacon_send(sub, beacons[i].beacon, cb_data, beacons[i].create_fn);
		if (res) {
			/* Bail out */
			break;
		}
	}

	return res;
}
#endif /* CONFIG_BT_MESH_V1d1 */

static int unprovisioned_beacon_send(void)
{
#if defined(CONFIG_BT_MESH_PB_ADV)
	const struct bt_mesh_prov *prov;
	u8_t uri_hash[16] = { 0 };
	struct net_buf *buf;
	u16_t oob_info;

	BT_DBG("");

	buf = bt_mesh_adv_create(BT_MESH_ADV_BEACON, UNPROV_XMIT, K_NO_WAIT);
	if (!buf) {
		BT_ERR("Unable to allocate beacon buffer");
		return -ENOBUFS;
	}

	prov = bt_mesh_prov_get();

	net_buf_add_u8(buf, BEACON_TYPE_UNPROVISIONED);
	net_buf_add_mem(buf, prov->uuid, 16);

	if (prov->uri && bt_mesh_s1(prov->uri, uri_hash) == 0) {
		oob_info = prov->oob_info | BT_MESH_PROV_OOB_URI;
	} else {
		oob_info = prov->oob_info;
	}

	net_buf_add_be16(buf, oob_info);
    #if defined(BFLB_BLE)
    if(prov->uri)
    #endif
	net_buf_add_mem(buf, uri_hash, 4);

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	static u32_t pts_cnt = 0;
	BT_PTS("[PTS] Sending unprovisioned device beacon %lu", ++pts_cnt);
#endif

	bt_mesh_adv_send(buf, NULL, NULL);
	net_buf_unref(buf);

	if (prov->uri) {
		size_t len;

		buf = bt_mesh_adv_create(BT_MESH_ADV_URI, UNPROV_XMIT,
					 K_NO_WAIT);
		if (!buf) {
			BT_ERR("Unable to allocate URI buffer");
			return -ENOBUFS;
		}

		len = strlen(prov->uri);
		if (net_buf_tailroom(buf) < len) {
			BT_WARN("Too long URI to fit advertising data");
		} else {
			net_buf_add_mem(buf, prov->uri, len);
			bt_mesh_adv_send(buf, NULL, NULL);
		}

		net_buf_unref(buf);
	}

#endif /* CONFIG_BT_MESH_PB_ADV */
	return 0;
}

static void unprovisioned_beacon_recv(struct net_buf_simple *buf)
{
	const struct bt_mesh_prov *prov;
	u8_t *uuid;
	u16_t oob_info;
	u32_t uri_hash_val;
	u32_t *uri_hash = NULL;

	if (buf->len != 18 && buf->len != 22) {
		BT_ERR("Invalid unprovisioned beacon length (%u)", buf->len);
		return;
	}

	uuid = net_buf_simple_pull_mem(buf, 16);
	oob_info = net_buf_simple_pull_be16(buf);

	if (buf->len == 4) {
		uri_hash_val = net_buf_simple_pull_be32(buf);
		uri_hash = &uri_hash_val;
	}

	BT_DBG("uuid %s", bt_hex(uuid, 16));

	prov = bt_mesh_prov_get();

	if (prov->unprovisioned_beacon) {
		prov->unprovisioned_beacon(uuid,
					   (bt_mesh_prov_oob_info_t)oob_info,
					   uri_hash);
	}
}

#if defined(CONFIG_BT_MESH_V1d1)
static void sub_update_beacon_observation(struct bt_mesh_subnet *sub)
{
	sub->secure_beacon.last = sub->secure_beacon.cur;
	sub->secure_beacon.cur = 0U;

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
	sub->priv_beacon.last = sub->priv_beacon.cur;
	sub->priv_beacon.cur = 0U;
#endif
}
#endif /* CONFIG_BT_MESH_V1d1 */

static void update_beacon_observation(void)
{
	static bool first_half;

	/* Observation period is 20 seconds, whereas the beacon timer
	 * runs every 10 seconds. We process what's happened during the
	 * window only after the seconnd half.
	 */
	first_half = !first_half;
	if (first_half) {
		return;
	}
#if defined(CONFIG_BT_MESH_V1d1)
	bt_mesh_subnet_foreach(sub_update_beacon_observation);
#else
	for (int i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

		if (sub->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		sub->beacons_last = sub->beacons_cur;
		sub->beacons_cur = 0U;
	}
#endif /* CONFIG_BT_MESH_V1d1 */
}
#if defined(CONFIG_BT_MESH_V1d1)
static bool net_beacon_is_running(void)
{
	return secure_beacon_is_running() ||
	       (bt_mesh_priv_beacon_get() == BT_MESH_FEATURE_ENABLED);
}
#endif /* CONFIG_BT_MESH_V1d1 */

static void beacon_send(struct k_work *work)
{
	/* Don't send anything if we have an active provisioning link */
	if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV) && bt_prov_active()) {
		k_delayed_work_submit(&beacon_timer, UNPROVISIONED_INTERVAL);
		return;
	}
#if defined(CONFIG_AUTO_PTS)
	BT_PTS("");
#else
	BT_DBG("");
#endif /* CONFIG_AUTO_PTS */

	if (bt_mesh_is_provisioned()) {
		#if defined(CONFIG_BT_MESH_V1d1)
		if (!net_beacon_is_running()) {
			return;
		}/* Add in new */
		#endif /* CONFIG_BT_MESH_V1d1 */

		update_beacon_observation();
		#if defined(CONFIG_BT_MESH_V1d1)
		(void)bt_mesh_subnet_find(net_beacon_for_subnet_send, NULL);
		#else
		secure_beacon_send();
		#endif /* CONFIG_BT_MESH_V1d1 */

		#if defined(CONFIG_BT_MESH_V1d1)
		k_delayed_work_submit(&beacon_timer, PROVISIONED_INTERVAL);
		#else
		/* Only resubmit if beaconing is still enabled */
		if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED ||
		    atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR)) {
			k_delayed_work_submit(&beacon_timer,
					      PROVISIONED_INTERVAL);
		}
		#endif /* CONFIG_BT_MESH_V1d1 */

		return;
	}

	if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV)) {
		unprovisioned_beacon_send();
		k_delayed_work_submit(&beacon_timer, UNPROVISIONED_INTERVAL);
	}
}

#if defined(CONFIG_BT_MESH_V1d1)
static bool auth_match(struct bt_mesh_subnet_keys *keys,
		       const struct beacon_params *params)
{
	uint8_t net_auth[8];

	if (memcmp(params->net_id, keys->net_id, 8)) {
		return false;
	}

	if (bt_mesh_beacon_auth(keys->beacon, params->flags, keys->net_id, params->iv_index,
				net_auth)) {
		return false;
	}

	if (memcmp(params->auth, net_auth, 8)) {
		LOG_WRN("Invalid auth value. Received auth: %s", bt_hex(params->auth, 8));
		LOG_WRN("Calculated auth: %s", bt_hex(net_auth, 8));
		return false;
	}

	return true;
}

static bool secure_beacon_authenticate(struct bt_mesh_subnet *sub, void *cb_data)
{
	struct beacon_params *params = cb_data;

	// Add by bouffalolab
	if(sub->net_idx == BT_MESH_KEY_UNUSED){ 
		return false;
	}

	for (int i = 0; i < ARRAY_SIZE(sub->keys); i++) {
		// Add by bouffalolab
		if(i == 1 && sub->kr_phase == BT_MESH_KR_NORMAL) {
			break;
		}

		if (/* sub->keys[i].valid && */auth_match(&sub->keys[i], params)) { //'sub->keys[i].valid' not present may equal 'sub->net_idx != BT_MESH_KEY_UNUSED')
			params->new_key = (i > 0);
			return true;
		}
	}

	return false;
}

static bool priv_beacon_decrypt(struct bt_mesh_subnet *sub, void *cb_data)
{
	struct beacon_params *params = cb_data;
	uint8_t out[5];
	int err;

	// Add by bouffalolab
	if(sub->net_idx == BT_MESH_KEY_UNUSED){ 
		return false;
	}

	for (int i = 0; i < ARRAY_SIZE(sub->keys); i++) {
		if (0/*!sub->keys[i].valid*/) { //'sub->keys[i].valid' not present may equal 'sub->net_idx != BT_MESH_KEY_UNUSED')
			continue;
		}
		// Add by bouffalolab
		if(i == 1 && sub->kr_phase == BT_MESH_KR_NORMAL) {
			break;
		}

		err = bt_mesh_beacon_decrypt(&sub->keys[i].priv_beacon, params->random,
					     params->data, params->auth, out);
		if (!err) {
			params->new_key = (i > 0);
			params->flags = out[0];
			params->iv_index = sys_get_be32(&out[1]);
			return true;
		}
	}

	return false;
}

static void net_beacon_register(struct bt_mesh_beacon *beacon, bool priv)
{
	if (((priv && bt_mesh_priv_beacon_get() == BT_MESH_PRIV_GATT_PROXY_ENABLED) ||
	     bt_mesh_beacon_enabled()) && beacon->cur < 0xff) {
		beacon->cur++;
		beacon->recv = k_uptime_get_32();
	}
}

static void net_beacon_recv(struct bt_mesh_subnet *sub,
			    const struct beacon_params *params)
{
	bool kr_change = bt_mesh_kr_update(sub, BT_MESH_KEY_REFRESH(params->flags),
			  params->new_key);
	// add by bouffalolab
	if (kr_change) {
		bt_mesh_net_beacon_update(sub);
		/* Key Refresh without IV Update only impacts one subnet */
		bt_mesh_net_sec_update(sub);
	}

	/* If we have NetKey0 accept IV index initiation only from it */
	if (bt_mesh_subnet_get(BT_MESH_KEY_PRIMARY) &&
	    sub->net_idx != BT_MESH_KEY_PRIMARY) {
#if !defined(BFLB_BLE_MESH_PATCH_IGNORE_SECURE_BEACON)// cherry from bouffalolab
		LOG_WRN("Ignoring secure beacon on non-primary subnet");
		return;
#else
		if (params->iv_index > bt_mesh.iv_index) {
			return;
		}
#endif
	}

	LOG_DBG("net_idx 0x%04x flags %u iv_index 0x%08lx, "
		"current iv_index 0x%08lx",
		sub->net_idx, params->flags, params->iv_index, bt_mesh.iv_index);

	if (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR) &&
	    (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS) ==
	     BT_MESH_IV_UPDATE(params->flags))) {
		bt_mesh_beacon_ivu_initiator(false);
	}

	bool iv_change = bt_mesh_net_iv_update(params->iv_index,
			      BT_MESH_IV_UPDATE(params->flags));
	if (iv_change) { //add by bouffalolab
		/* Update all subnets */
		bt_mesh_net_sec_update(NULL);
	}
}

static void net_beacon_resolve(struct beacon_params *params,
			       bool (*matcher)(struct bt_mesh_subnet *sub,
					       void *cb_data))
{
	struct bt_mesh_subnet *sub;
	struct bt_mesh_beacon *beacon;

	sub = bt_mesh_subnet_find(beacon_cache_match, (void *)params);
	if (sub) {
		beacon = subnet_beacon_get_by_type(sub, params->private);

		/* We've seen this beacon before - just update the stats */
		net_beacon_register(beacon, params->private);
		return;
	}

	sub = bt_mesh_subnet_find(matcher, params);
	if (!sub) {
		LOG_DBG("No subnet that matched beacon");
		return;
	}

	if (sub->kr_phase == BT_MESH_KR_PHASE_2 && !params->new_key) {
		LOG_WRN("Ignoring Phase 2 KR Update secured using old key");
		return;
	}

	beacon = subnet_beacon_get_by_type(sub, params->private);

	cache_add(params->auth, beacon);

	net_beacon_recv(sub, params);
	net_beacon_register(beacon, params->private);
}

static void secure_beacon_recv(struct net_buf_simple *buf)
{
	struct beacon_params params;

	if (buf->len < 21) {
		LOG_ERR("Too short secure beacon (len %u)", buf->len);
		return;
	}

	params.private = false;
	params.flags = net_buf_simple_pull_u8(buf);
	params.net_id = net_buf_simple_pull_mem(buf, 8);
	params.iv_index = net_buf_simple_pull_be32(buf);
	params.auth = buf->data;

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	BT_PTS("[PTS] Secure network beacon received");
	BT_PTS("[PTS] - Flags: [0x%02X]", params.flags);
	BT_PTS("[PTS] - IV Index: [0x%08lX]", params.iv_index);
#endif
	net_beacon_resolve(&params, secure_beacon_authenticate);
}

static void private_beacon_recv(struct net_buf_simple *buf)
{
	struct beacon_params params;

	if (buf->len < 26) {
		LOG_ERR("Too short private beacon (len %u)", buf->len);
		return;
	}

	params.private = true;
	params.random = net_buf_simple_pull_mem(buf, 13);
	params.data = net_buf_simple_pull_mem(buf, 5);
	params.auth = buf->data;

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	BT_PTS("[PTS] Private network beacon received");
#endif
	net_beacon_resolve(&params, priv_beacon_decrypt);
}
#else
static void secure_beacon_recv(struct net_buf_simple *buf)
{
	u8_t *data, *net_id, *auth;
	struct bt_mesh_subnet *sub;
	u32_t iv_index;
	bool new_key, kr_change, iv_change;
	u8_t flags;

	if (buf->len < 21) {
		BT_ERR("Too short secure beacon (len %u)", buf->len);
		return;
	}

	sub = cache_check(buf->data);
	if (sub) {
		/* We've seen this beacon before - just update the stats */
		goto update_stats;
	}

	/* So we can add to the cache if auth matches */
	data = buf->data;

	flags = net_buf_simple_pull_u8(buf);
	net_id = net_buf_simple_pull_mem(buf, 8);
	iv_index = net_buf_simple_pull_be32(buf);
	auth = buf->data;

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	BT_PTS("[PTS] Secure network beacon received");
	BT_PTS("[PTS] - Flags: [0x%02X]", flags);
	BT_PTS("[PTS] - IV Index: [0x%08lX]", iv_index);
#endif

	BT_DBG("flags 0x%02x id %s iv_index 0x%08lx",
	       flags, bt_hex(net_id, 8), iv_index);

	sub = bt_mesh_subnet_find(net_id, flags, iv_index, auth, &new_key);
	if (!sub) {
		BT_DBG("No subnet that matched beacon");
		return;
	}

	if (sub->kr_phase == BT_MESH_KR_PHASE_2 && !new_key) {
		BT_WARN("Ignoring Phase 2 KR Update secured using old key");
		return;
	}

	cache_add(data, sub);

	kr_change = bt_mesh_kr_update(sub, BT_MESH_KEY_REFRESH(flags), new_key);
	if (kr_change) {
		bt_mesh_net_beacon_update(sub);
		/* Key Refresh without IV Update only impacts one subnet */
		bt_mesh_net_sec_update(sub);
	}

	/* If we have NetKey0 accept initiation only from it */
	if (bt_mesh_subnet_get(BT_MESH_KEY_PRIMARY) &&
	    sub->net_idx != BT_MESH_KEY_PRIMARY) {
#if !defined(BFLB_BLE_MESH_PATCH_IGNORE_SECURE_BEACON)
		BT_WARN("Ignoring secure beacon on non-primary subnet");
		goto update_stats;
#else
		if (iv_index > bt_mesh.iv_index) {
			goto update_stats;
		}
#endif
	}

	BT_DBG("net_idx 0x%04x iv_index 0x%08lx, current iv_index 0x%08lx",
	       sub->net_idx, iv_index, bt_mesh.iv_index);

	if (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR) &&
	    (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS) ==
	     BT_MESH_IV_UPDATE(flags))) {
		bt_mesh_beacon_ivu_initiator(false);
	}

	iv_change = bt_mesh_net_iv_update(iv_index, BT_MESH_IV_UPDATE(flags));

	if (iv_change) {
		/* Update all subnets */
		bt_mesh_net_sec_update(NULL);
	}

update_stats:
	if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED &&
	    sub->beacons_cur < 0xff) {
		sub->beacons_cur++;
	}
}
#endif /* CONFIG_BT_MESH_V1d1 */

void bt_mesh_beacon_recv(struct net_buf_simple *buf)
{
	u8_t type;

	BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

	if (buf->len < 1) {
		BT_ERR("Too short beacon");
		return;
	}

	type = net_buf_simple_pull_u8(buf);
	switch (type) {
	case BEACON_TYPE_UNPROVISIONED:
		if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV)) {
			unprovisioned_beacon_recv(buf);
		}
		break;
	case BEACON_TYPE_SECURE:
		secure_beacon_recv(buf);
		break;
#if defined(CONFIG_BT_MESH_V1d1)
	case BEACON_TYPE_PRIVATE:
		private_beacon_recv(buf);
		break;
#endif /* CONFIG_BT_MESH_V1d1 */
	default:
		BT_WARN("Unknown beacon type 0x%02x", type);
		break;
	}
}

#if defined(CONFIG_BT_MESH_V1d1)
int bt_mesh_net_beacon_update(struct bt_mesh_subnet *sub)
{
	u8_t flags = bt_mesh_net_flags(sub);
	struct bt_mesh_subnet_keys *keys;
#if defined(CONFIG_BT_MESH_V1d1)
	keys = &sub->keys[SUBNET_KEY_TX_IDX(sub)];
#else
	if (sub->kr_flag) {
		BT_DBG("NetIndex %u Using new key", sub->net_idx);
		keys = &sub->keys[1];
	} else {
		BT_DBG("NetIndex %u Using current key", sub->net_idx);
		keys = &sub->keys[0];
	}
#endif /* CONFIG_BT_MESH_V1d1 */

	BT_DBG("flags 0x%02x, IVI 0x%08lx", flags, bt_mesh.iv_index);

#if defined(CONFIG_BT_MESH_V1d1) && defined(CONFIG_BT_MESH_PRIV_BEACONS)//old
	/* Invalidate private beacon to force regeneration: */
	sub->priv_beacon_ctx.idx = priv_random.idx - 1; //priv_random defined in beason.c
	priv_random.timestamp = 0;
	// origin:bt_mesh_beacon_auth(&keys->beacon, flags, keys->net_id, bt_mesh.iv_index,
	//		    sub->secure_beacon.auth);
	bt_mesh_beacon_auth(keys->beacon, flags, keys->net_id, bt_mesh.iv_index,
			    sub->secure_beacon.auth);
	return 0;
#else
	return bt_mesh_beacon_auth(keys->beacon, flags, keys->net_id,
				   bt_mesh.iv_index, sub->auth);
#endif /* CONFIG_BT_MESH_V1d1 && CONFIG_BT_MESH_PRIV_BEACONS */
}
#endif /* CONFIG_BT_MESH_V1d1 */

void bt_mesh_beacon_init(void)
{
	k_delayed_work_init(&beacon_timer, beacon_send);
#if defined(CONFIG_BT_MESH_V1d1) && defined(CONFIG_BT_MESH_PRIV_BEACONS)
	private_random_update();
#endif /* CONFIG_BT_MESH_V1d1 && CONFIG_BT_MESH_PRIV_BEACONS*/
}

void bt_mesh_beacon_ivu_initiator(bool enable)
{
	atomic_set_bit_to(bt_mesh.flags, BT_MESH_IVU_INITIATOR, enable);

	if (enable) {
		k_work_submit(&beacon_timer.work);
	} else if (bt_mesh_beacon_get() == BT_MESH_BEACON_DISABLED) {
		k_delayed_work_cancel(&beacon_timer);
	}
}

#if defined(CONFIG_BT_MESH_V1d1)
static void subnet_beacon_enable(struct bt_mesh_subnet *sub)
{
	sub->secure_beacon.last = 0U;
	sub->secure_beacon.cur = 0U;

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
	sub->priv_beacon.last = 0U;
	sub->priv_beacon.cur = 0U;
#endif

	bt_mesh_net_beacon_update(sub);// origin:bt_mesh_beacon_update(sub);
}
#endif /* CONFIG_BT_MESH_V1d1 */

void bt_mesh_beacon_enable(void)
{
	if (!bt_mesh_is_provisioned()) {
		k_work_submit(&beacon_timer.work);
		return;
	}

	#if defined(CONFIG_BT_MESH_V1d1)
	bt_mesh_subnet_foreach(subnet_beacon_enable);
	#else
	for (int i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

		if (sub->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		sub->beacons_last = 0U;
		sub->beacons_cur = 0U;

		bt_mesh_net_beacon_update(sub);
	}
	#endif /* CONFIG_BT_MESH_V1d1 */

	k_work_submit(&beacon_timer.work);
}

void bt_mesh_beacon_disable(void)
{
	if (!atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_INITIATOR)) {
		k_delayed_work_cancel(&beacon_timer);
	}
}
