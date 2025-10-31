/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <string.h>
#include <bt_errno.h>
#include <stdbool.h>
#include <common/include/atomic.h>
#include <util.h>
#include <byteorder.h>

#include <net/buf.h>
#include <bluetooth.h>
#include <conn.h>
#include <include/mesh.h>
#include <mesh_config.h>
#include "include/cfg.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_NET)
#define LOG_MODULE_NAME bt_mesh_net
#include "bt_log.h"

#include "crypto.h"
#include "adv.h"
#include "mesh.h"
#include "net.h"
#include "lpn.h"
#include "friend.h"
#include "proxy.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "beacon.h"
#include "mesh_settings.h"
#include "prov.h"

/* Minimum valid Mesh Network PDU length. The Network headers
 * themselves take up 9 bytes. After that there is a minimum of 1 byte
 * payload for both CTL=1 and CTL=0 PDUs (smallest OpCode is 1 byte). CTL=1
 * PDUs must use a 64-bit (8 byte) NetMIC, whereas CTL=0 PDUs have at least
 * a 32-bit (4 byte) NetMIC and AppMIC giving again a total of 8 bytes.
 */
#define BT_MESH_NET_MIN_PDU_LEN (BT_MESH_NET_HDR_LEN + 1 + 8)

#define LOOPBACK_MAX_PDU_LEN (BT_MESH_NET_HDR_LEN + 16)
#define LOOPBACK_USER_DATA_SIZE sizeof(struct bt_mesh_subnet *)
#define LOOPBACK_BUF_SUB(buf) (*(struct bt_mesh_subnet **)net_buf_user_data(buf))

/* Seq limit after IV Update is triggered */
#define IV_UPDATE_SEQ_LIMIT 8000000

#define IVI(pdu)           ((pdu)[0] >> 7)
#define NID(pdu)           ((pdu)[0] & 0x7f)
#define CTL(pdu)           ((pdu)[1] >> 7)
#define TTL(pdu)           ((pdu)[1] & 0x7f)
#define SEQ(pdu)           (sys_get_be24(&pdu[2]))
#define SRC(pdu)           (sys_get_be16(&(pdu)[5]))
#define DST(pdu)           (sys_get_be16(&(pdu)[7]))

/* Determine how many friendship credentials we need */
#if defined(CONFIG_BT_MESH_FRIEND)
#define FRIEND_CRED_COUNT CONFIG_BT_MESH_FRIEND_LPN_COUNT
#elif defined(CONFIG_BT_MESH_LOW_POWER)
#define FRIEND_CRED_COUNT CONFIG_BT_MESH_SUBNET_COUNT
#else
#define FRIEND_CRED_COUNT 0
#endif

static struct friend_cred friend_cred[FRIEND_CRED_COUNT];

static struct {
	u32_t src : 15, /* MSb of source is always 0 */
	      seq : 17;
} msg_cache[CONFIG_BT_MESH_MSG_CACHE_SIZE];
static u16_t msg_cache_next;

/* Singleton network context (the implementation only supports one) */
struct bt_mesh_net bt_mesh = {
	.local_queue = SYS_SLIST_STATIC_INIT(&bt_mesh.local_queue),
	.sub = {
		[0 ... (CONFIG_BT_MESH_SUBNET_COUNT - 1)] = {
			.net_idx = BT_MESH_KEY_UNUSED,
		}
	},
	.app_keys = {
		[0 ... (CONFIG_BT_MESH_APP_KEY_COUNT - 1)] = {
			.net_idx = BT_MESH_KEY_UNUSED,
			.app_idx = BT_MESH_KEY_UNUSED,
		}
	},
#if defined(CONFIG_BT_MESH_V1d1) && defined(CONFIG_BT_MESH_PRIV_BEACONS)
	.priv_beacon_int = 0x3c,
#endif /* CONFIG_BT_MESH_V1d1 && CONFIG_BT_MESH_PRIV_BEACONS */
};

#if !defined(BFLB_DYNAMIC_ALLOC_MEM)
NET_BUF_POOL_DEFINE(loopback_buf_pool, CONFIG_BT_MESH_LOOPBACK_BUFS,
		    LOOPBACK_MAX_PDU_LEN, LOOPBACK_USER_DATA_SIZE, NULL);
#else
struct net_buf_pool loopback_buf_pool;
#endif
#if !defined(BFLB_BLE_MESH_PATCH_NET_DECODE)
static u32_t dup_cache[CONFIG_BT_MESH_MSG_CACHE_SIZE];
static int   dup_cache_next;

static bool check_dup(struct net_buf_simple *data)
{
	const u8_t *tail = net_buf_simple_tail(data);
	u32_t val;
	int i;

	val = sys_get_be32(tail - 4) ^ sys_get_be32(tail - 8);

	for (i = 0; i < ARRAY_SIZE(dup_cache); i++) {
		if (dup_cache[i] == val) {
			return true;
		}
	}

	dup_cache[dup_cache_next++] = val;
	dup_cache_next %= ARRAY_SIZE(dup_cache);

	return false;
}
#endif

static bool msg_cache_match(struct net_buf_simple *pdu)
{
	u16_t i;

	for (i = 0U; i < ARRAY_SIZE(msg_cache); i++) {
		if (msg_cache[i].src == SRC(pdu->data) &&
		    msg_cache[i].seq == (SEQ(pdu->data) & BIT_MASK(17))) {
			return true;
		}
	}

	return false;
}

static void msg_cache_add(struct bt_mesh_net_rx *rx)
{
	rx->msg_cache_idx = msg_cache_next++;
	msg_cache[rx->msg_cache_idx].src = rx->ctx.addr;
	msg_cache[rx->msg_cache_idx].seq = rx->seq;
	msg_cache_next %= ARRAY_SIZE(msg_cache);
}

/* Added by bouffalolab to clear msg cache indicated by addr */
void bt_mesh_msg_cache_del(u16_t addr)
{
	u16_t i;

	for (i = 0U; i < ARRAY_SIZE(msg_cache); i++) {
		if (msg_cache[i].src == addr) {
			msg_cache[i].src = 0;
		}
	}

	return false;
}

struct bt_mesh_subnet *bt_mesh_subnet_get(u16_t net_idx)
{
	int i;

	if (net_idx == BT_MESH_KEY_ANY) {
		return &bt_mesh.sub[0];
	}

	for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		if (bt_mesh.sub[i].net_idx == net_idx) {
			return &bt_mesh.sub[i];
		}
	}

	return NULL;
}

int bt_mesh_net_keys_create(struct bt_mesh_subnet_keys *keys,
			    const u8_t key[16])
{
	u8_t p[] = { 0 };
	u8_t nid;
	int err;

	err = bt_mesh_k2(key, p, sizeof(p), &nid, keys->enc, keys->privacy);
	if (err) {
		BT_ERR("Unable to generate NID, EncKey & PrivacyKey");
		return err;
	}

	memcpy(keys->net, key, 16);

	keys->nid = nid;

	BT_DBG("NID 0x%02x EncKey %s", keys->nid, bt_hex(keys->enc, 16));
	BT_DBG("PrivacyKey %s", bt_hex(keys->privacy, 16));

	err = bt_mesh_k3(key, keys->net_id);
	if (err) {
		BT_ERR("Unable to generate Net ID");
		return err;
	}

	BT_DBG("NetID %s", bt_hex(keys->net_id, 8));

#if defined(CONFIG_BT_MESH_GATT_PROXY)
#if defined(CONFIG_BT_MESH_V1d1)
	err = bt_mesh_identity_key(key, keys->identity.key);
#else
	err = bt_mesh_identity_key(key, keys->identity);
#endif /* CONFIG_BT_MESH_V1d1 */
	if (err) {
		BT_ERR("Unable to generate IdentityKey");
		return err;
	}
#if defined(CONFIG_BT_MESH_V1d1)
	BT_DBG("IdentityKey %s", bt_hex(keys->identity.key, 16));
#else
	BT_DBG("IdentityKey %s", bt_hex(keys->identity, 16));
#endif /* CONFIG_BT_MESH_V1d1 */
#endif /* GATT_PROXY */

	err = bt_mesh_beacon_key(key, keys->beacon);
	if (err) {
		BT_ERR("Unable to generate beacon key");
		return err;
	}

	BT_DBG("BeaconKey %s", bt_hex(keys->beacon, 16));

#if defined(CONFIG_BT_MESH_V1d1) && defined(CONFIG_BT_MESH_PRIV_BEACONS)
	err = bt_mesh_private_beacon_key(key, &keys->priv_beacon);
	if (err) {
		LOG_ERR("Unable to generate private beacon key");
		return err;
	}
#if defined(CONFIG_AUTP_PTS)
	BT_PTS("PrivateBeaconKey %s", bt_hex(&keys->priv_beacon, sizeof(struct bt_mesh_key)));
#else
	LOG_DBG("PrivateBeaconKey %s", bt_hex(&keys->priv_beacon, sizeof(struct bt_mesh_key)));
#endif /* CONFIG_AUTP_PTS */

	//keys->valid = 1U;
#endif /* CONFIG_BT_MESH_V1d1 && CONFIG_BT_MESH_PRIV_BEACONS */
	return 0;
}

int friend_cred_set(struct friend_cred *cred, u8_t idx, const u8_t net_key[16])
{
	u16_t lpn_addr, frnd_addr;
	int err;
	u8_t p[9];

#if defined(CONFIG_BT_MESH_LOW_POWER)
	if (cred->addr == bt_mesh.lpn.frnd) {
		lpn_addr = bt_mesh_primary_addr();
		frnd_addr = cred->addr;
	} else {
		lpn_addr = cred->addr;
		frnd_addr = bt_mesh_primary_addr();
	}
#else
	lpn_addr = cred->addr;
	frnd_addr = bt_mesh_primary_addr();
#endif

	BT_DBG("LPNAddress 0x%04x FriendAddress 0x%04x", lpn_addr, frnd_addr);
	BT_DBG("LPNCounter 0x%04x FriendCounter 0x%04x", cred->lpn_counter,
	       cred->frnd_counter);

	p[0] = 0x01;
	sys_put_be16(lpn_addr, p + 1);
	sys_put_be16(frnd_addr, p + 3);
	sys_put_be16(cred->lpn_counter, p + 5);
	sys_put_be16(cred->frnd_counter, p + 7);

	err = bt_mesh_k2(net_key, p, sizeof(p), &cred->cred[idx].nid,
			 cred->cred[idx].enc, cred->cred[idx].privacy);
	if (err) {
		BT_ERR("Unable to generate NID, EncKey & PrivacyKey");
		return err;
	}

	BT_DBG("Friend NID 0x%02x EncKey %s", cred->cred[idx].nid,
	       bt_hex(cred->cred[idx].enc, 16));
	BT_DBG("Friend PrivacyKey %s", bt_hex(cred->cred[idx].privacy, 16));

	return 0;
}

void friend_cred_refresh(u16_t net_idx)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
		struct friend_cred *cred = &friend_cred[i];

		if (cred->addr != BT_MESH_ADDR_UNASSIGNED &&
		    cred->net_idx == net_idx) {
			memcpy(&cred->cred[0], &cred->cred[1],
			       sizeof(cred->cred[0]));
		}
	}
}

int friend_cred_update(struct bt_mesh_subnet *sub)
{
	int err, i;

	BT_DBG("net_idx 0x%04x", sub->net_idx);

	for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
		struct friend_cred *cred = &friend_cred[i];

		if (cred->addr == BT_MESH_ADDR_UNASSIGNED ||
		    cred->net_idx != sub->net_idx) {
			continue;
		}

		err = friend_cred_set(cred, 1, sub->keys[1].net);
		if (err) {
			return err;
		}
	}

	return 0;
}

struct friend_cred *friend_cred_create(struct bt_mesh_subnet *sub, u16_t addr,
				       u16_t lpn_counter, u16_t frnd_counter)
{
	struct friend_cred *cred;
	int i, err;

	BT_DBG("net_idx 0x%04x addr 0x%04x", sub->net_idx, addr);

	for (cred = NULL, i = 0; i < ARRAY_SIZE(friend_cred); i++) {
		if ((friend_cred[i].addr == BT_MESH_ADDR_UNASSIGNED) ||
		    (friend_cred[i].addr == addr &&
		     friend_cred[i].net_idx == sub->net_idx)) {
			cred = &friend_cred[i];
			break;
		}
	}

	if (!cred) {
		BT_WARN("No free friend credential slots");
		return NULL;
	}

	cred->net_idx = sub->net_idx;
	cred->addr = addr;
	cred->lpn_counter = lpn_counter;
	cred->frnd_counter = frnd_counter;

	err = friend_cred_set(cred, 0, sub->keys[0].net);
	if (err) {
		friend_cred_clear(cred);
		return NULL;
	}

	if (sub->kr_flag) {
		err = friend_cred_set(cred, 1, sub->keys[1].net);
		if (err) {
			friend_cred_clear(cred);
			return NULL;
		}
	}

	return cred;
}

void friend_cred_clear(struct friend_cred *cred)
{
	cred->net_idx = BT_MESH_KEY_UNUSED;
	cred->addr = BT_MESH_ADDR_UNASSIGNED;
	cred->lpn_counter = 0U;
	cred->frnd_counter = 0U;
	(void)memset(cred->cred, 0, sizeof(cred->cred));
}

int friend_cred_del(u16_t net_idx, u16_t addr)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
		struct friend_cred *cred = &friend_cred[i];

		if (cred->addr == addr && cred->net_idx == net_idx) {
			friend_cred_clear(cred);
			return 0;
		}
	}

	return -ENOENT;
}

int friend_cred_get(struct bt_mesh_subnet *sub, u16_t addr, u8_t *nid,
		    const u8_t **enc, const u8_t **priv)
{
	int i;

	BT_DBG("net_idx 0x%04x addr 0x%04x", sub->net_idx, addr);

	for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
		struct friend_cred *cred = &friend_cred[i];

		if (cred->net_idx != sub->net_idx) {
			continue;
		}

		if (addr != BT_MESH_ADDR_UNASSIGNED && cred->addr != addr) {
			continue;
		}

		if (nid) {
			*nid = cred->cred[sub->kr_flag].nid;
		}

		if (enc) {
			*enc = cred->cred[sub->kr_flag].enc;
		}

		if (priv) {
			*priv = cred->cred[sub->kr_flag].privacy;
		}

		return 0;
	}

	return -ENOENT;
}

u8_t bt_mesh_net_flags(struct bt_mesh_subnet *sub)
{
	u8_t flags = 0x00;

	if (sub && sub->kr_flag) {
		flags |= BT_MESH_NET_FLAG_KR;
	}

	if (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS)) {
		flags |= BT_MESH_NET_FLAG_IVU;
	}

	return flags;
}

#if !defined(CONFIG_BT_MESH_V1d1)
int bt_mesh_net_beacon_update(struct bt_mesh_subnet *sub)
{
	u8_t flags = bt_mesh_net_flags(sub);
	struct bt_mesh_subnet_keys *keys;

	if (sub->kr_flag) {
		BT_DBG("NetIndex %u Using new key", sub->net_idx);
		keys = &sub->keys[1];
	} else {
		BT_DBG("NetIndex %u Using current key", sub->net_idx);
		keys = &sub->keys[0];
	}

	BT_DBG("flags 0x%02x, IVI 0x%08lx", flags, bt_mesh.iv_index);

	return bt_mesh_beacon_auth(keys->beacon, flags, keys->net_id,
				   bt_mesh.iv_index, sub->auth);
}
#endif /* CONFIG_BT_MESH_V1d1 */

int bt_mesh_net_create(u16_t idx, u8_t flags, const u8_t key[16],
		       u32_t iv_index)
{
	struct bt_mesh_subnet *sub;
	int err;

	BT_DBG("idx %u flags 0x%02x iv_index %lu", idx, flags, iv_index);

	BT_DBG("NetKey %s", bt_hex(key, 16));

	(void)memset(msg_cache, 0, sizeof(msg_cache));
	msg_cache_next = 0U;

	sub = &bt_mesh.sub[0];

	sub->kr_flag = BT_MESH_KEY_REFRESH(flags);
	if (sub->kr_flag) {
		err = bt_mesh_net_keys_create(&sub->keys[1], key);
		if (err) {
			return -EIO;
		}

		sub->kr_phase = BT_MESH_KR_PHASE_2;
	} else {
		err = bt_mesh_net_keys_create(&sub->keys[0], key);
		if (err) {
			return -EIO;
		}
	}

	sub->net_idx = idx;

	if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
		sub->node_id = BT_MESH_NODE_IDENTITY_STOPPED;
	} else {
		sub->node_id = BT_MESH_NODE_IDENTITY_NOT_SUPPORTED;
	}

	bt_mesh.iv_index = iv_index;
	atomic_set_bit_to(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS,
			  BT_MESH_IV_UPDATE(flags));

	/* Set minimum required hours, since the 96-hour minimum requirement
	 * doesn't apply straight after provisioning (since we can't know how
	 * long has actually passed since the network changed its state).
	 */
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	/* MESH/NODE/IVU/BI-01-C */
	if(BT_MESH_IV_UPDATE(flags)){
#endif
		bt_mesh.ivu_duration = BT_MESH_IVU_MIN_HOURS;
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	}
#endif

	/* Make sure we have valid beacon data to be sent */
	bt_mesh_net_beacon_update(sub);

	return 0;
}

void bt_mesh_net_revoke_keys(struct bt_mesh_subnet *sub)
{
	int i;

	BT_DBG("idx 0x%04x", sub->net_idx);

	memcpy(&sub->keys[0], &sub->keys[1], sizeof(sub->keys[0]));
#if defined(BFLB_BLE_MESH_PATCH_NET_REVOKE_KEYS)
	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		BT_DBG("Storing Updated NetKey persistently");
		bt_mesh_store_subnet(sub);
	}
#endif

	for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
		struct bt_mesh_app_key *key = &bt_mesh.app_keys[i];

		if (key->net_idx != sub->net_idx || !key->updated) {
			continue;
		}

		memcpy(&key->keys[0], &key->keys[1], sizeof(key->keys[0]));
		key->updated = false;
#if defined(BFLB_BLE_MESH_PATCH_NET_REVOKE_KEYS)
		if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
			BT_DBG("Storing Updated AppKey persistently");
			bt_mesh_store_app_key(key);
		}
#endif
	}
}

bool bt_mesh_kr_update(struct bt_mesh_subnet *sub, u8_t new_kr, bool new_key)
{
	if (new_kr != sub->kr_flag && sub->kr_phase == BT_MESH_KR_NORMAL) {
		BT_WARN("KR change in normal operation. Are we blacklisted?");
		return false;
	}

	sub->kr_flag = new_kr;

	if (sub->kr_flag) {
		if (sub->kr_phase == BT_MESH_KR_PHASE_1) {
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
			BT_PTS("[PTS] Key Refresh: Phase 1 -> Phase 2");
#endif
			BT_DBG("Phase 1 -> Phase 2");
			sub->kr_phase = BT_MESH_KR_PHASE_2;
			if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
				BT_DBG("Storing krp phase persistently");
				bt_mesh_store_subnet(sub);
			}

			return true;
		}
	} else {
		switch (sub->kr_phase) {
		case BT_MESH_KR_PHASE_1:
			if (!new_key) {
				/* Ignore */
				break;
			}
		/* Upon receiving a Secure Network beacon with the KR flag set
		 * to 0 using the new NetKey in Phase 1, the node shall
		 * immediately transition to Phase 3, which effectively skips
		 * Phase 2.
		 *
		 * Intentional fall-through.
		 */
			__attribute__((fallthrough));/* Fix compile error by bouffalo  */
		case BT_MESH_KR_PHASE_2:
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
			BT_PTS("[PTS] Key Refresh: Phase %d -> Normal", sub->kr_phase);
#endif

			BT_DBG("KR Phase 0x%02x -> Normal", sub->kr_phase);
			bt_mesh_net_revoke_keys(sub);
			if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER) ||
			    IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
				friend_cred_refresh(sub->net_idx);
			}

			sub->kr_phase = BT_MESH_KR_NORMAL;
			return true;
		}
	}

	return false;
}

void bt_mesh_rpl_reset(void)
{
	int i;

	/* Discard "old old" IV Index entries from RPL and flag
	 * any other ones (which are valid) as old.
	 */
	for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
		struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];

		if (rpl->src) {
			if (rpl->old_iv) {
				(void)memset(rpl, 0, sizeof(*rpl));
			} else {
				rpl->old_iv = true;
			}

			if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
				bt_mesh_store_rpl(rpl);
			}
		}
	}
}

#if defined(CONFIG_BT_MESH_IV_UPDATE_TEST)
void bt_mesh_iv_update_test(bool enable)
{
	atomic_set_bit_to(bt_mesh.flags, BT_MESH_IVU_TEST, enable);
	/* Reset the duration variable - needed for some PTS tests */
	bt_mesh.ivu_duration = 0U;
}

bool bt_mesh_iv_update(void)
{
	if (!bt_mesh_is_provisioned()) {
		BT_ERR("Not yet provisioned");
		return false;
	}

	if (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS)) {
		bt_mesh_net_iv_update(bt_mesh.iv_index, false);
	} else {
		bt_mesh_net_iv_update(bt_mesh.iv_index + 1, true);
	}

	bt_mesh_net_sec_update(NULL);

	return atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS);
}
#endif /* CONFIG_BT_MESH_IV_UPDATE_TEST */

/* Used for sending immediate beacons to Friend queues and GATT clients */
void bt_mesh_net_sec_update(struct bt_mesh_subnet *sub)
{
	if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
		bt_mesh_friend_sec_update(sub ? sub->net_idx : BT_MESH_KEY_ANY);
	}

	if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) &&
	    (bt_mesh_gatt_proxy_get() == BT_MESH_GATT_PROXY_ENABLED
		#if defined(CONFIG_BT_MESH_V1d1)
		|| bt_mesh_priv_gatt_proxy_get() == BT_MESH_PRIV_GATT_PROXY_ENABLED
		#endif /* CONFIG_BT_MESH_V1d1 */
		)){
		bt_mesh_proxy_beacon_send(sub);
	}
}

bool bt_mesh_net_iv_update(u32_t iv_index, bool iv_update)
{
	int i;

	if (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS)) {
		/* We're currently in IV Update mode */
		#if defined(CONFIG_BT_MESH_V1d1)
		/* model 1.1 -> 3.11.6 IV Index Recovery procedure */
		if (iv_index == bt_mesh.iv_index + 1) {
			BT_WARN("In Progress Performing IV Index Recovery");
			(void)memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));
			bt_mesh.iv_index = iv_index;
			bt_mesh.seq = 0U;
			goto do_update;
		}
		#endif /* CONFIG_BT_MESH_V1d1 */

		if (iv_index != bt_mesh.iv_index) {
			BT_WARN("IV Index mismatch: 0x%08lx != 0x%08lx",
				iv_index, bt_mesh.iv_index);
			return false;
		}

		if (iv_update) {
			/* Nothing to do */
			BT_DBG("Already in IV Update in Progress state");
			return false;
		}
	} else {
		/* We're currently in Normal mode */

		if (iv_index == bt_mesh.iv_index) {
			BT_DBG("Same IV Index in normal mode");
			return false;
		}

		if (iv_index < bt_mesh.iv_index ||
		    iv_index > bt_mesh.iv_index + 42) {
			BT_ERR("IV Index out of sync: 0x%08lx != 0x%08lx",
			       iv_index, bt_mesh.iv_index);
			return false;
		}

		if (iv_index > bt_mesh.iv_index + 1) {
			BT_WARN("Performing IV Index Recovery");
			(void)memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));
			bt_mesh.iv_index = iv_index;
			bt_mesh.seq = 0U;
			goto do_update;
		}

		if (iv_index == bt_mesh.iv_index + 1 && !iv_update) {
			BT_WARN("Ignoring new index in normal mode");
			return false;
		}

		if (!iv_update) {
			/* Nothing to do */
			BT_DBG("Already in Normal state");
			return false;
		}
	}

	if (!(IS_ENABLED(CONFIG_BT_MESH_IV_UPDATE_TEST) &&
	      atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_TEST))) {
		if (bt_mesh.ivu_duration < BT_MESH_IVU_MIN_HOURS) {
			BT_WARN("IV Update before minimum duration");
			return false;
		}
	}

	/* Defer change to Normal Operation if there are pending acks */
	if (!iv_update && bt_mesh_tx_in_progress()) {
		BT_WARN("IV Update deferred because of pending transfer");
		atomic_set_bit(bt_mesh.flags, BT_MESH_IVU_PENDING);
		return false;
	}

do_update:
	atomic_set_bit_to(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS, iv_update);
	bt_mesh.ivu_duration = 0U;

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	if (iv_update) {
		BT_PTS("[PTS] IV Update: transitioning to IV Update in Progress state");
	} else {
		BT_PTS("[PTS] IV Update: transitioning to Normal Operation state");
	}
#endif

	if (iv_update) {
		bt_mesh.iv_index = iv_index;
		BT_DBG("IV Update state entered. New index 0x%08lx",
		       bt_mesh.iv_index);

		bt_mesh_rpl_reset();
	} else {
		BT_DBG("Normal mode entered");
		bt_mesh.seq = 0U;
	}

	k_delayed_work_submit(&bt_mesh.ivu_timer, BT_MESH_IVU_TIMEOUT);

	for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		if (bt_mesh.sub[i].net_idx != BT_MESH_KEY_UNUSED) {
			bt_mesh_net_beacon_update(&bt_mesh.sub[i]);
		}
	}

	#if defined(CONFIG_BT_MESH_V1d1)
	if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) &&
		(bt_mesh_gatt_proxy_get() == BT_MESH_GATT_PROXY_ENABLED ||
		 bt_mesh_priv_gatt_proxy_get() == BT_MESH_PRIV_GATT_PROXY_ENABLED)) {
		bt_mesh_proxy_beacon_send(NULL);
	}
	#endif /* CONFIG_BT_MESH_V1d1 */
	
	if (IS_ENABLED(CONFIG_BT_MESH_CDB)) {
		bt_mesh_cdb_iv_update(iv_index, iv_update);
	}

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_iv(false);
	}

	return true;
}

u32_t bt_mesh_next_seq(void)
{
	u32_t seq = bt_mesh.seq++;

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_seq();
	}

	if (!atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS) &&
	    bt_mesh.seq > IV_UPDATE_SEQ_LIMIT &&
	    bt_mesh_subnet_get(BT_MESH_KEY_PRIMARY)) {
		bt_mesh_beacon_ivu_initiator(true);
		bt_mesh_net_iv_update(bt_mesh.iv_index + 1, true);
		bt_mesh_net_sec_update(NULL);
	}

	return seq;
}

static void bt_mesh_net_local(struct k_work *work)
{
	struct net_buf *buf;

	while ((buf = net_buf_slist_get(&bt_mesh.local_queue))) {
		struct bt_mesh_subnet *sub = LOOPBACK_BUF_SUB(buf);
		struct bt_mesh_net_rx rx = {
			.ctx = {
				.net_idx = sub->net_idx,
				/* Initialize AppIdx to a sane value */
				.app_idx = BT_MESH_KEY_UNUSED,
				.recv_ttl = TTL(buf->data),
				/* TTL=1 only goes to local IF */
				.send_ttl = 1U,
				.addr = SRC(buf->data),
				.recv_dst = DST(buf->data),
				.recv_rssi = 0,
			},
			.net_if = BT_MESH_NET_IF_LOCAL,
			.sub = sub,
			.old_iv = (IVI(buf->data) != (bt_mesh.iv_index & 0x01)),
			.ctl = CTL(buf->data),
			.seq = SEQ(buf->data),
			.new_key = sub->kr_flag,
			.local_match = 1U,
			.friend_match = 0U,
		};

		BT_DBG("src: 0x%04x dst: 0x%04x seq 0x%06lx sub %p", rx.ctx.addr,
		       rx.ctx.addr, rx.seq, sub);

		(void) bt_mesh_trans_recv(&buf->b, &rx);
		net_buf_unref(buf);
	}
}

static void net_tx_cred_get(struct bt_mesh_net_tx *tx, u8_t *nid,
			    const u8_t **enc, const u8_t **priv)
{
	if (!IS_ENABLED(CONFIG_BT_MESH_LOW_POWER) || !tx->friend_cred) {
		tx->friend_cred = 0U;
		*nid = tx->sub->keys[tx->sub->kr_flag].nid;
		*enc = tx->sub->keys[tx->sub->kr_flag].enc;
		*priv = tx->sub->keys[tx->sub->kr_flag].privacy;
		return;
	}

	if (friend_cred_get(tx->sub, BT_MESH_ADDR_UNASSIGNED, nid, enc, priv)) {
		BT_WARN("Falling back to master credentials");

		tx->friend_cred = 0U;

		*nid = tx->sub->keys[tx->sub->kr_flag].nid;
		*enc = tx->sub->keys[tx->sub->kr_flag].enc;
		*priv = tx->sub->keys[tx->sub->kr_flag].privacy;
	}
}

static int net_header_encode(struct bt_mesh_net_tx *tx, u8_t nid,
			     struct net_buf_simple *buf)
{
	const bool ctl = (tx->ctx->app_idx == BT_MESH_KEY_UNUSED);

	if (ctl && net_buf_simple_tailroom(buf) < 8) {
		BT_ERR("Insufficient MIC space for CTL PDU");
		return -EINVAL;
	} else if (net_buf_simple_tailroom(buf) < 4) {
		BT_ERR("Insufficient MIC space for PDU");
		return -EINVAL;
	}

	BT_DBG("src 0x%04x dst 0x%04x ctl %u seq 0x%06lx",
	       tx->src, tx->ctx->addr, ctl, bt_mesh.seq);

	net_buf_simple_push_be16(buf, tx->ctx->addr);
	net_buf_simple_push_be16(buf, tx->src);
	net_buf_simple_push_be24(buf, bt_mesh_next_seq());

	if (ctl) {
		net_buf_simple_push_u8(buf, tx->ctx->send_ttl | 0x80);
	} else {
		net_buf_simple_push_u8(buf, tx->ctx->send_ttl);
	}

	net_buf_simple_push_u8(buf, (nid | (BT_MESH_NET_IVI_TX & 1) << 7));

	return 0;
}

int bt_mesh_net_encode(struct bt_mesh_net_tx *tx, struct net_buf_simple *buf,
		       bool proxy)
{
	const u8_t *enc, *priv;
	u8_t nid;
	int err;

	net_tx_cred_get(tx, &nid, &enc, &priv);

	err = net_header_encode(tx, nid, buf);
	if (err) {
		return err;
	}

	err = bt_mesh_net_encrypt(enc, buf, BT_MESH_NET_IVI_TX, proxy);
	if (err) {
		return err;
	}

	return bt_mesh_net_obfuscate(buf->data, BT_MESH_NET_IVI_TX, priv);
}

static int loopback(const struct bt_mesh_net_tx *tx, const u8_t *data,
		    size_t len)
{
	struct net_buf *buf;

	buf = net_buf_alloc(&loopback_buf_pool, K_NO_WAIT);
	if (!buf) {
		BT_WARN("Unable to allocate loopback");
		return -ENOMEM;
	}

	BT_DBG("");

	LOOPBACK_BUF_SUB(buf) = tx->sub;

	net_buf_add_mem(buf, data, len);

	net_buf_slist_put(&bt_mesh.local_queue, buf);

	k_work_submit(&bt_mesh.local_work);

	return 0;
}
#if defined(BFLB_BLE_MESH_FIX_MESH_ENCRYPT_ERR_DURING_ENCRYPT_USING_APPKEY)
int bt_mesh_net_send(struct bt_mesh_net_tx *tx, struct net_buf *buf,
		     const struct bt_mesh_send_cb *cb, void *cb_data, reencrypt_appdata_cb recb, struct net_buf_simple *sdu)
#else
int bt_mesh_net_send(struct bt_mesh_net_tx *tx, struct net_buf *buf,
		     const struct bt_mesh_send_cb *cb, void *cb_data)
#endif /* BFLB_BLE_MESH_FIX_MESH_ENCRYPT_ERR_DURING_ENCRYPT_USING_APPKEY */
{
	const u8_t *enc, *priv;
	u8_t nid;
	int err;

	BT_DBG("src 0x%04x dst 0x%04x len %u headroom %zu tailroom %zu",
	       tx->src, tx->ctx->addr, buf->len, net_buf_headroom(buf),
	       net_buf_tailroom(buf));
	BT_DBG("Payload len %u: %s", buf->len, bt_hex(buf->data, buf->len));
	BT_DBG("Seq 0x%06lx", bt_mesh.seq);

	if (tx->ctx->send_ttl == BT_MESH_TTL_DEFAULT) {
		tx->ctx->send_ttl = bt_mesh_default_ttl_get();
	}

	net_tx_cred_get(tx, &nid, &enc, &priv);
	#if defined(BFLB_BLE_MESH_FIX_MESH_ENCRYPT_ERR_DURING_ENCRYPT_USING_APPKEY)
	unsigned int key = irq_lock();
	if(recb && bt_mesh.seq != tx->seq){
		recb(tx, sdu, buf);
	}
	err = net_header_encode(tx, nid, &buf->b);
	irq_unlock(key);
	#else
	err = net_header_encode(tx, nid, &buf->b);
	#endif /* BFLB_BLE_MESH_FIX_MESH_ENCRYPT_ERR_DURING_ENCRYPT_USING_APPKEY */
	if (err) {
		goto done;
	}

	/* Deliver to local network interface if necessary */
	if (bt_mesh_fixed_group_match(tx->ctx->addr) ||
	    bt_mesh_elem_find(tx->ctx->addr)) {
		err = loopback(tx, buf->data, buf->len);

		/* Local unicast messages should not go out to network */
		if (BT_MESH_ADDR_IS_UNICAST(tx->ctx->addr) ||
		    tx->ctx->send_ttl == 1U) {
			if (!err) {
				send_cb_finalize(cb, cb_data);
			}

			goto done;
		}
	}

	/* Mesh spec 3.4.5.2: "The output filter of the interface connected to
	 * advertising or GATT bearers shall drop all messages with TTL value
	 * set to 1." If a TTL=1 packet wasn't for a local interface, it is
	 * invalid.
	 */
	if (tx->ctx->send_ttl == 1U) {
		err = -EINVAL;
		goto done;
	}

	err = bt_mesh_net_encrypt(enc, &buf->b, BT_MESH_NET_IVI_TX, false);
	if (err) {
		goto done;
	}

	err = bt_mesh_net_obfuscate(buf->data, BT_MESH_NET_IVI_TX, priv);
	if (err) {
		goto done;
	}

	/* Deliver to GATT Proxy Clients if necessary. */
	if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) &&
	    bt_mesh_proxy_relay(&buf->b, tx->ctx->addr) &&
	    BT_MESH_ADDR_IS_UNICAST(tx->ctx->addr)) {
		/* Notify completion if this only went through the Mesh Proxy */
		send_cb_finalize(cb, cb_data);

		err = 0;
		goto done;
	}

	bt_mesh_adv_send(buf, cb, cb_data);

done:
	net_buf_unref(buf);
	return err;
}

void bt_mesh_net_loopback_clear(u16_t net_idx)
{
	sys_slist_t new_list;
	struct net_buf *buf;

	BT_DBG("0x%04x", net_idx);

	sys_slist_init(&new_list);

	while ((buf = net_buf_slist_get(&bt_mesh.local_queue))) {
		struct bt_mesh_subnet *sub = LOOPBACK_BUF_SUB(buf);

		if (net_idx == BT_MESH_KEY_ANY || net_idx == sub->net_idx) {
			BT_DBG("Dropped 0x%06lx", SEQ(buf->data));
			net_buf_unref(buf);
		} else {
			net_buf_slist_put(&new_list, buf);
		}
	}

	bt_mesh.local_queue = new_list;
}

#if defined(CONFIG_BT_MESH_V1d1)
struct bt_mesh_subnet *bt_mesh_subnet_find(bool (*cb)(struct bt_mesh_subnet *sub, void *cb_data),
					   void *cb_data)
{
	for (int i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		if (bt_mesh.sub[i].net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		if (!cb || cb(&bt_mesh.sub[i], cb_data)) {
			return &bt_mesh.sub[i];
		}
	}

	return NULL;
}
#else
static bool auth_match(struct bt_mesh_subnet_keys *keys,
		       const u8_t net_id[8], u8_t flags,
		       u32_t iv_index, const u8_t auth[8])
{
	u8_t net_auth[8];

	if (memcmp(net_id, keys->net_id, 8)) {
		return false;
	}

	bt_mesh_beacon_auth(keys->beacon, flags, keys->net_id, iv_index,
			    net_auth);

	if (memcmp(auth, net_auth, 8)) {
		BT_WARN("Authentication Value %s", bt_hex(auth, 8));
		BT_WARN(" != %s", bt_hex(net_auth, 8));
		return false;
	}

	return true;
}

struct bt_mesh_subnet *bt_mesh_subnet_find(const u8_t net_id[8], u8_t flags,
					   u32_t iv_index, const u8_t auth[8],
					   bool *new_key)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		struct bt_mesh_subnet *sub = &bt_mesh.sub[i];

		if (sub->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		if (auth_match(&sub->keys[0], net_id, flags, iv_index, auth)) {
			*new_key = false;
			return sub;
		}

		if (sub->kr_phase == BT_MESH_KR_NORMAL) {
			continue;
		}

		if (auth_match(&sub->keys[1], net_id, flags, iv_index, auth)) {
			*new_key = true;
			return sub;
		}
	}

	return NULL;
}
#endif /* CONFIG_BT_MESH_V1d1 */

#if defined(CONFIG_BT_MESH_V1d1)
size_t bt_mesh_subnet_foreach(void (*cb)(struct bt_mesh_subnet *sub))
{
	size_t count = 0;

	for (int i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		if (bt_mesh.sub[i].net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		cb(&bt_mesh.sub[i]);
		count++;
	}

	return count;
}

struct bt_mesh_subnet *bt_mesh_subnet_next(struct bt_mesh_subnet *sub)
{
	if (sub) {
		sub++;
	} else {
		sub = &bt_mesh.sub[0];
	}

	for (int i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++, sub++) {
		/* Roll over once we reach the end */
		if (sub == &bt_mesh.sub[ARRAY_SIZE(bt_mesh.sub)]) {
			sub = &bt_mesh.sub[0];
		}

		if (sub->net_idx != BT_MESH_KEY_UNUSED) {
			return sub;
		}
	}

	return NULL;
}
#endif /* CONFIG_BT_MESH_V1d1 */

#if defined(CONFIG_BT_MESH_V1d1)
uint8_t bt_mesh_subnet_node_id_set(uint16_t net_idx,
				   enum bt_mesh_feat_state node_id)
{
	struct bt_mesh_subnet *sub;

	if (node_id == BT_MESH_FEATURE_NOT_SUPPORTED) {
		return STATUS_CANNOT_SET;
	}

	sub = bt_mesh_subnet_get(net_idx);
	if (!sub) {
		return STATUS_INVALID_NETKEY;
	}

	if (!IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
		return STATUS_FEAT_NOT_SUPP;
	}

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
	/* Implements binding from MshPRTv1.1: 4.2.46.1. When enabling non-private node
	 * identity state, disable its private counterpart.
	 */
	for (int i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		if (bt_mesh.sub[i].net_idx != BT_MESH_KEY_UNUSED &&
		    bt_mesh.sub[i].node_id == BT_MESH_FEATURE_ENABLED &&
		    bt_mesh.sub[i].priv_beacon_ctx.node_id) {
			bt_mesh_proxy_identity_stop(&bt_mesh.sub[i]);
		}
	}
#endif

	if (node_id) {
		bt_mesh_proxy_identity_start(sub, false);
	} else {
		bt_mesh_proxy_identity_stop(sub);
	}

	bt_mesh_adv_update();//origin:bt_mesh_adv_gatt_update();

	return STATUS_SUCCESS;
}

uint8_t bt_mesh_subnet_node_id_get(uint16_t net_idx,
				   enum bt_mesh_feat_state *node_id)
{
	struct bt_mesh_subnet *sub;

	sub = bt_mesh_subnet_get(net_idx);
	if (!sub) {
		*node_id = 0x00;
		return STATUS_INVALID_NETKEY;
	}

	*node_id = sub->node_id;
#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
	*node_id &= !sub->priv_beacon_ctx.node_id;
#endif

	return STATUS_SUCCESS;
}


uint8_t bt_mesh_subnet_priv_node_id_set(uint16_t net_idx,
					enum bt_mesh_feat_state priv_node_id)
{
	struct bt_mesh_subnet *sub;

	if (priv_node_id == BT_MESH_FEATURE_NOT_SUPPORTED) {
		return STATUS_CANNOT_SET;
	}

	sub = bt_mesh_subnet_get(net_idx);
	if (!sub) {
		return STATUS_INVALID_NETKEY;
	}

	if (!IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) ||
	    !IS_ENABLED(CONFIG_BT_MESH_PRIV_BEACONS)) {
		return STATUS_FEAT_NOT_SUPP;
	}

#if defined(CONFIG_BT_MESH_PRIV_BEACONS)
	/* Reverse binding from MshPRTv1.1: 4.2.46.1 doesn't
	 * allow to set private state if non-private state is enabled.
	 */
	for (int i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		if (bt_mesh.sub[i].net_idx != BT_MESH_KEY_UNUSED &&
		    bt_mesh.sub[i].node_id == BT_MESH_FEATURE_ENABLED &&
		    !bt_mesh.sub[i].priv_beacon_ctx.node_id) {
			return STATUS_CANNOT_SET;
		}
	}
#endif

	if (priv_node_id) {
		bt_mesh_proxy_identity_start(sub, true);
	} else {
		bt_mesh_proxy_identity_stop(sub);
	}

	bt_mesh_adv_update();//origin:bt_mesh_adv_gatt_update();

	return STATUS_SUCCESS;
}

uint8_t bt_mesh_subnet_priv_node_id_get(uint16_t net_idx,
					enum bt_mesh_feat_state *priv_node_id)
{
	struct bt_mesh_subnet *sub;

	sub = bt_mesh_subnet_get(net_idx);
	if (!sub) {
		*priv_node_id = 0x00;
		return STATUS_INVALID_NETKEY;
	}

#if CONFIG_BT_MESH_GATT_PROXY && CONFIG_BT_MESH_PRIV_BEACONS
	if (sub->node_id == BT_MESH_FEATURE_ENABLED && sub->priv_beacon_ctx.node_id) {
		*priv_node_id = sub->node_id;
	} else {
		*priv_node_id = BT_MESH_FEATURE_DISABLED;
	}
#else
	*priv_node_id = BT_MESH_FEATURE_NOT_SUPPORTED;
#endif

	return STATUS_SUCCESS;
}

enum bt_mesh_subnets_node_id_state bt_mesh_subnets_node_id_state_get(void)
{
	for (int i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		if (bt_mesh.sub[i].node_id) {
#if CONFIG_BT_MESH_PRIV_BEACONS
			if (bt_mesh.sub[i].priv_beacon_ctx.node_id) {
				return BT_MESH_SUBNETS_NODE_ID_STATE_ENABLED_PRIVATE;
			}
#endif
			return BT_MESH_SUBNETS_NODE_ID_STATE_ENABLED;
		}
	}

	return BT_MESH_SUBNETS_NODE_ID_STATE_NONE;
}
#endif /* CONFIG_BT_MESH_V1d1 */

static int net_decrypt(struct bt_mesh_subnet *sub, const u8_t *enc,
		       const u8_t *priv, const u8_t *data,
		       size_t data_len, struct bt_mesh_net_rx *rx,
		       struct net_buf_simple *buf)
{
	BT_DBG("NID 0x%02x net_idx 0x%04x", NID(data), sub->net_idx);
	BT_DBG("IVI %u net->iv_index 0x%08lx", IVI(data), bt_mesh.iv_index);

	rx->old_iv = (IVI(data) != (bt_mesh.iv_index & 0x01));

	net_buf_simple_reset(buf);
	memcpy(net_buf_simple_add(buf, data_len), data, data_len);

	if (bt_mesh_net_obfuscate(buf->data, BT_MESH_NET_IVI_RX(rx), priv)) {
		return -ENOENT;
	}

	rx->ctx.addr = SRC(buf->data);
	if (!BT_MESH_ADDR_IS_UNICAST(rx->ctx.addr)) {
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
		BT_PTS("[PTS] Invalid address (SRC = 0x%04X)", rx->ctx.addr);
#endif
		BT_DBG("Ignoring non-unicast src addr 0x%04x", rx->ctx.addr);
		return -EINVAL;
	}

	if (bt_mesh_elem_find(rx->ctx.addr)) {
		BT_DBG("Dropping locally originated packet");
		return -EBADMSG;
	}

	if (rx->net_if == BT_MESH_NET_IF_ADV && msg_cache_match(buf)) {
		BT_DBG("Duplicate found in Network Message Cache");
		return -EALREADY;
	}

	BT_DBG("src 0x%04x", rx->ctx.addr);

	if (IS_ENABLED(CONFIG_BT_MESH_PROXY) &&
	    rx->net_if == BT_MESH_NET_IF_PROXY_CFG) {
		return bt_mesh_net_decrypt(enc, buf, BT_MESH_NET_IVI_RX(rx),
					   true);
	}

	return bt_mesh_net_decrypt(enc, buf, BT_MESH_NET_IVI_RX(rx), false);
}

static int friend_decrypt(struct bt_mesh_subnet *sub, const u8_t *data,
			  size_t data_len, struct bt_mesh_net_rx *rx,
			  struct net_buf_simple *buf)
{
	int i;

	BT_DBG("NID 0x%02x net_idx 0x%04x", NID(data), sub->net_idx);

	for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
		struct friend_cred *cred = &friend_cred[i];

		if (cred->net_idx != sub->net_idx) {
			continue;
		}

		if (NID(data) == cred->cred[0].nid &&
		    !net_decrypt(sub, cred->cred[0].enc, cred->cred[0].privacy,
				 data, data_len, rx, buf)) {
			return 0;
		}

		if (sub->kr_phase == BT_MESH_KR_NORMAL) {
			continue;
		}

		if (NID(data) == cred->cred[1].nid &&
		    !net_decrypt(sub, cred->cred[1].enc, cred->cred[1].privacy,
				 data, data_len, rx, buf)) {
			rx->new_key = 1U;
			return 0;
		}
	}

	return -ENOENT;
}

static bool net_find_and_decrypt(const u8_t *data, size_t data_len,
				 struct bt_mesh_net_rx *rx,
				 struct net_buf_simple *buf)
{
	struct bt_mesh_subnet *sub;
	int i;

	BT_DBG("");

	for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
		sub = &bt_mesh.sub[i];
		if (sub->net_idx == BT_MESH_KEY_UNUSED) {
			continue;
		}

		if ((IS_ENABLED(CONFIG_BT_MESH_LOW_POWER) ||
		     IS_ENABLED(CONFIG_BT_MESH_FRIEND)) &&
		    !friend_decrypt(sub, data, data_len, rx, buf)) {
			rx->friend_cred = 1U;
			rx->ctx.net_idx = sub->net_idx;
			rx->sub = sub;
			return true;
		}

		if (NID(data) == sub->keys[0].nid &&
		    !net_decrypt(sub, sub->keys[0].enc, sub->keys[0].privacy,
				 data, data_len, rx, buf)) {
			rx->ctx.net_idx = sub->net_idx;
			rx->sub = sub;
			return true;
		}

		if (sub->kr_phase == BT_MESH_KR_NORMAL) {
			continue;
		}

		if (NID(data) == sub->keys[1].nid &&
		    !net_decrypt(sub, sub->keys[1].enc, sub->keys[1].privacy,
				 data, data_len, rx, buf)) {
			rx->new_key = 1U;
			rx->ctx.net_idx = sub->net_idx;
			rx->sub = sub;
			return true;
		}
	}

	return false;
}

/* Relaying from advertising to the advertising bearer should only happen
 * if the Relay state is set to enabled. Locally originated packets always
 * get sent to the advertising bearer. If the packet came in through GATT,
 * then we should only relay it if the GATT Proxy state is enabled.
 */
static bool relay_to_adv(enum bt_mesh_net_if net_if)
{
	switch (net_if) {
	case BT_MESH_NET_IF_ADV:
		return (bt_mesh_relay_get() == BT_MESH_RELAY_ENABLED);
	case BT_MESH_NET_IF_PROXY:
		return (bt_mesh_gatt_proxy_get() == BT_MESH_GATT_PROXY_ENABLED
			#if defined(CONFIG_BT_MESH_V1d1)
			|| bt_mesh_priv_gatt_proxy_get() == BT_MESH_PRIV_GATT_PROXY_ENABLED
			#endif /* CONFIG_BT_MESH_V1d1 */
			);
	default:
		return false;
	}
}

static void bt_mesh_net_relay(struct net_buf_simple *sbuf,
			      struct bt_mesh_net_rx *rx)
{
	const u8_t *enc, *priv;
	struct net_buf *buf;
	u8_t nid, transmit;

	if (rx->ctx.recv_ttl <= 1U) {
		return;
	}

	if (rx->net_if == BT_MESH_NET_IF_ADV &&
	    bt_mesh_relay_get() != BT_MESH_RELAY_ENABLED &&
	    bt_mesh_gatt_proxy_get() != BT_MESH_GATT_PROXY_ENABLED
		#if defined(CONFIG_BT_MESH_V1d1)
		&& bt_mesh_priv_gatt_proxy_get() != BT_MESH_PRIV_GATT_PROXY_ENABLED
		#endif /* CONFIG_BT_MESH_V1d1 */
		) {
		return;
	}

	BT_DBG("TTL %u CTL %u dst 0x%04x", rx->ctx.recv_ttl, rx->ctl,
	       rx->ctx.recv_dst);

	/* The Relay Retransmit state is only applied to adv-adv relaying.
	 * Anything else (like GATT to adv, or locally originated packets)
	 * use the Network Transmit state.
	 */
	if (rx->net_if == BT_MESH_NET_IF_ADV) {
		transmit = bt_mesh_relay_retransmit_get();
	} else {
		transmit = bt_mesh_net_transmit_get();
	}

	buf = bt_mesh_adv_create(BT_MESH_ADV_DATA, transmit, K_NO_WAIT);
	if (!buf) {
		BT_ERR("Out of relay buffers");
		return;
	}

	/* Leave CTL bit intact */
	sbuf->data[1] &= 0x80;
	sbuf->data[1] |= rx->ctx.recv_ttl - 1U;

	net_buf_add_mem(buf, sbuf->data, sbuf->len);

	enc = rx->sub->keys[rx->sub->kr_flag].enc;
	priv = rx->sub->keys[rx->sub->kr_flag].privacy;
	nid = rx->sub->keys[rx->sub->kr_flag].nid;

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	if (rx->net_if != BT_MESH_NET_IF_LOCAL) {
		BT_PTS("[PTS] Relaying packet (TTL = 0x%02X)", TTL(buf->data));
	}
#endif

	BT_DBG("Relaying packet. TTL is now %u", TTL(buf->data));

	/* Update NID if RX or RX was with friend credentials */
	if (rx->friend_cred) {
		buf->data[0] &= 0x80; /* Clear everything except IVI */
		buf->data[0] |= nid;
	}

	/* We re-encrypt and obfuscate using the received IVI rather than
	 * the normal TX IVI (which may be different) since the transport
	 * layer nonce includes the IVI.
	 */
	if (bt_mesh_net_encrypt(enc, &buf->b, BT_MESH_NET_IVI_RX(rx), false)) {
		BT_ERR("Re-encrypting failed");
		goto done;
	}

	if (bt_mesh_net_obfuscate(buf->data, BT_MESH_NET_IVI_RX(rx), priv)) {
		BT_ERR("Re-obfuscating failed");
		goto done;
	}

	/* Sending to the GATT bearer should only happen if GATT Proxy
	 * is enabled.
	 */
	if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) &&
	    (bt_mesh_gatt_proxy_get() == BT_MESH_GATT_PROXY_ENABLED
		#if defined(CONFIG_BT_MESH_V1d1)
		|| bt_mesh_priv_gatt_proxy_get() == BT_MESH_PRIV_GATT_PROXY_ENABLED
		#endif /* CONFIG_BT_MESH_V1d1 */
		)) {
		if (bt_mesh_proxy_relay(&buf->b, rx->ctx.recv_dst) &&
		    BT_MESH_ADDR_IS_UNICAST(rx->ctx.recv_dst)) {
			goto done;
		}
	}
	/* Fix by bouffalolab for MESH/NODE/FRND/FN/BV-23-C*/
	if (relay_to_adv(rx->net_if) 
	#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
		|| rx->friend_cred
	#endif 
		) {
		bt_mesh_adv_send(buf, NULL, NULL);
	}

done:
	net_buf_unref(buf);
}

void bt_mesh_net_header_parse(struct net_buf_simple *buf,
			      struct bt_mesh_net_rx *rx)
{
	rx->old_iv = (IVI(buf->data) != (bt_mesh.iv_index & 0x01));
	rx->ctl = CTL(buf->data);
	rx->ctx.recv_ttl = TTL(buf->data);
	rx->seq = SEQ(buf->data);
	rx->ctx.addr = SRC(buf->data);
	rx->ctx.recv_dst = DST(buf->data);
}

int bt_mesh_net_decode(struct net_buf_simple *data, enum bt_mesh_net_if net_if,
		       struct bt_mesh_net_rx *rx, struct net_buf_simple *buf)
{
	if (data->len < BT_MESH_NET_MIN_PDU_LEN) {
		BT_WARN("Dropping too short mesh packet (len %u)", data->len);
		BT_WARN("%s", bt_hex(data->data, data->len));
		return -EINVAL;
	}

#if !defined(BFLB_BLE_MESH_PATCH_NET_DECODE)
	if (net_if == BT_MESH_NET_IF_ADV && check_dup(data)) {
		return -EINVAL;
	}
#endif

	BT_DBG("%u bytes: %s", data->len, bt_hex(data->data, data->len));

	rx->net_if = net_if;

	if (!net_find_and_decrypt(data->data, data->len, rx, buf)) {
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
		/* Comment by boufflalo */
		//BT_PTS("[PTS] Fail to decrypt packet");
#endif

		BT_DBG("Unable to find matching net for packet");
		return -ENOENT;
	}
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	/* Comment by boufflalo */
	BT_PTS("[PTS] NID[%x]\n", NID(data->data));
#endif

	/* Initialize AppIdx to a sane value */
	rx->ctx.app_idx = BT_MESH_KEY_UNUSED;

	rx->ctx.recv_ttl = TTL(buf->data);

	/* Default to responding with TTL 0 for non-routed messages */
	if (rx->ctx.recv_ttl == 0U) {
		rx->ctx.send_ttl = 0U;
	} else {
		rx->ctx.send_ttl = BT_MESH_TTL_DEFAULT;
	}

	rx->ctl = CTL(buf->data);
	rx->seq = SEQ(buf->data);
	rx->ctx.recv_dst = DST(buf->data);

	BT_DBG("Decryption successful. Payload len %u", buf->len);

	if (net_if != BT_MESH_NET_IF_PROXY_CFG &&
	    rx->ctx.recv_dst == BT_MESH_ADDR_UNASSIGNED) {
#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
		BT_PTS("[PTS] Invalid address (DST = 0x%04X)", rx->ctx.recv_dst);
#endif

		BT_ERR("Destination address is unassigned; dropping packet");
		return -EBADMSG;
	}

	BT_DBG("src 0x%04x dst 0x%04x ttl %u", rx->ctx.addr, rx->ctx.recv_dst,
	       rx->ctx.recv_ttl);
	BT_DBG("PDU: %s", bt_hex(buf->data, buf->len));

	msg_cache_add(rx);

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	if (net_if != BT_MESH_NET_IF_LOCAL) {
		BT_PTS("[PTS] Network packet received");
		BT_PTS("[PTS] - TTL: [0x%02X]", rx->ctx.recv_ttl);
		BT_PTS("[PTS] - CTL: [0x%02X]", rx->ctl);
		BT_PTS("[PTS] - SRC: [0x%04X]", rx->ctx.addr);
		BT_PTS("[PTS] - DST: [0x%04X]", rx->ctx.recv_dst);
		BT_PTS("[PTS] - TransportPDU: [%s]", bt_hex(buf->data + BT_MESH_NET_HDR_LEN, buf->len - BT_MESH_NET_HDR_LEN));
	}
#endif

	return 0;
}

void bt_mesh_net_recv(struct net_buf_simple *data, s8_t rssi,
		      enum bt_mesh_net_if net_if)
{
	NET_BUF_SIMPLE_DEFINE(buf, 29);
	struct bt_mesh_net_rx rx = { .ctx.recv_rssi = rssi };
	struct net_buf_simple_state state;

	BT_DBG("rssi %d net_if %u", rssi, net_if);

	if (!bt_mesh_is_provisioned()) {
		return;
	}

	if (bt_mesh_net_decode(data, net_if, &rx, &buf)) {
		return;
	}

	/* Save the state so the buffer can later be relayed */
	net_buf_simple_save(&buf, &state);

	rx.local_match = (bt_mesh_fixed_group_match(rx.ctx.recv_dst) ||
			  bt_mesh_elem_find(rx.ctx.recv_dst));

	if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) &&
	    net_if == BT_MESH_NET_IF_PROXY) {
		bt_mesh_proxy_addr_add(data, rx.ctx.addr);

		if (bt_mesh_gatt_proxy_get() == BT_MESH_GATT_PROXY_DISABLED &&
			#if defined(CONFIG_BT_MESH_V1d1)
			bt_mesh_priv_gatt_proxy_get() == BT_MESH_GATT_PROXY_DISABLED &&
			#endif
		    !rx.local_match) {
			BT_INFO("Proxy is disabled; ignoring message");
			return;
		}
	}

	/* The transport layer has indicated that it has rejected the message,
	 * but would like to see it again if it is received in the future.
	 * This can happen if a message is received when the device is in
	 * Low Power mode, but the message was not encrypted with the friend
	 * credentials. Remove it from the message cache so that we accept
	 * it again in the future.
	 */
	if (bt_mesh_trans_recv(&buf, &rx) == -EAGAIN) {
		BT_WARN("Removing rejected message from Network Message Cache");
		msg_cache[rx.msg_cache_idx].src = BT_MESH_ADDR_UNASSIGNED;
		/* Rewind the next index now that we're not using this entry */
		msg_cache_next = rx.msg_cache_idx;
	}

	/* Relay if this was a group/virtual address, or if the destination
	 * was neither a local element nor an LPN we're Friends for.
	 */
	if (!BT_MESH_ADDR_IS_UNICAST(rx.ctx.recv_dst) ||
	    (!rx.local_match && !rx.friend_match)) {
		net_buf_simple_restore(&buf, &state);
		bt_mesh_net_relay(&buf, &rx);
	}
}

static void ivu_refresh(struct k_work *work)
{
	bt_mesh.ivu_duration += BT_MESH_IVU_HOURS;

	BT_DBG("%s for %u hour%s",
	       atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS) ?
	       "IVU in Progress" : "IVU Normal mode",
	       bt_mesh.ivu_duration, bt_mesh.ivu_duration == 1U ? "" : "s");

	if (bt_mesh.ivu_duration < BT_MESH_IVU_MIN_HOURS) {
		if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
			bt_mesh_store_iv(true);
		}

		k_delayed_work_submit(&bt_mesh.ivu_timer, BT_MESH_IVU_TIMEOUT);
		return;
	}

	if (atomic_test_bit(bt_mesh.flags, BT_MESH_IVU_IN_PROGRESS)) {
		bt_mesh_beacon_ivu_initiator(true);
		bt_mesh_net_iv_update(bt_mesh.iv_index, false);
	} else if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_store_iv(true);
	}
}

void bt_mesh_net_start(void)
{
	if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED) {
		bt_mesh_beacon_enable();
	} else {
		bt_mesh_beacon_disable();
	}

	if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) &&
	    bt_mesh_gatt_proxy_get() != BT_MESH_GATT_PROXY_NOT_SUPPORTED) {
		bt_mesh_proxy_gatt_enable();
		bt_mesh_adv_update();
	}

	if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
		bt_mesh_lpn_init();
	} else {
		bt_mesh_scan_enable();
	}

	if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
		bt_mesh_friend_init();
	}

	if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
		u16_t net_idx = bt_mesh.sub[0].net_idx;
		u16_t addr = bt_mesh_primary_addr();
		#if defined(CONFIG_BLE_MESH_CERT_BASED_PROV)
		extern int bt_mesh_prov_cert_deinit();
		bt_mesh_prov_cert_deinit();
		#endif/* CONFIG_BLE_MESH_CERT_BASED_PROV */
		bt_mesh_prov_complete(net_idx, addr);
	}
}

void bt_mesh_net_init(void)
{
#if defined(BFLB_DYNAMIC_ALLOC_MEM)
	net_buf_init(&loopback_buf_pool, CONFIG_BT_MESH_LOOPBACK_BUFS, LOOPBACK_MAX_PDU_LEN, NULL);
#endif

	k_delayed_work_init(&bt_mesh.ivu_timer, ivu_refresh);

	k_work_init(&bt_mesh.local_work, bt_mesh_net_local);
}
