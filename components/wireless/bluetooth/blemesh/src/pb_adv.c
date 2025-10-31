/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <string.h>
#include <conn.h>
#include <include/mesh.h>
#include <net/buf.h>
// #include "host/testing.h"
#include "net.h"
#include "prov.h"
#include "adv.h"
#include "crypto.h"
#include "../../blestack/src/include/bluetooth/crypto.h"
#include "beacon.h"
#include "prov_bearer.h"

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG_PROV)
#define LOG_MODULE_NAME bt_mesh_pb_adv
#include "bt_log.h"
#include "bt_errno.h"
#if defined(CONFIG_AUTO_PTS)
#include "testing.h"
#endif


#define GPCF(gpc)           (gpc & 0x03)
#define GPC_START(last_seg) (((last_seg) << 2) | 0x00)
#define GPC_ACK             0x01
#define GPC_CONT(seg_id)    (((seg_id) << 2) | 0x02)
#define GPC_CTL(op)         (((op) << 2) | 0x03)

#define START_PAYLOAD_MAX 20
#define CONT_PAYLOAD_MAX  23
#define RX_BUFFER_MAX     65

#define START_LAST_SEG(gpc) (gpc >> 2)
#define CONT_SEG_INDEX(gpc) (gpc >> 2)

#define BEARER_CTL(gpc) (gpc >> 2)
#define LINK_OPEN       0x00
#define LINK_ACK        0x01
#define LINK_CLOSE      0x02

#define XACT_SEG_OFFSET(_seg) (20 + ((_seg - 1) * 23))
#define XACT_SEG_DATA(_seg) (&link.rx.buf->data[XACT_SEG_OFFSET(_seg)])
#define XACT_SEG_RECV(_seg) (link.rx.seg &= ~(1 << (_seg)))

#define XACT_ID_MAX  0x7f
#define XACT_ID_NVAL 0xff
#define SEG_NVAL     0xff

#define RETRANSMIT_TIMEOUT  K_MSEC(500)
#define BUF_TIMEOUT         K_MSEC(400)
#define CLOSING_TIMEOUT     (3 * MSEC_PER_SEC)
#define TRANSACTION_TIMEOUT (30 * MSEC_PER_SEC)

/* Acked messages, will do retransmissions manually, taking acks into account:
 */
#define RETRANSMITS_RELIABLE   0
/* Unacked messages: */
#define RETRANSMITS_UNRELIABLE 2
/* PDU acks: */
#define RETRANSMITS_ACK        2

enum {
	LINK_ACTIVE,    /* Link has been opened */
	LINK_ACK_RECVD, /* Ack for link has been received */
	LINK_CLOSING,   /* Link is closing down */
	LINK_INVALID,   /* Error occurred during provisioning */
	ACK_PENDING,    /* An acknowledgment is being sent */
	PROVISIONER,    /* The link was opened as provisioner */

	NUM_FLAGS,
};

struct pb_adv {
	u32_t id; /* Link ID */

	ATOMIC_DEFINE(flags, NUM_FLAGS);

	const struct prov_bearer_cb *cb;
	void *cb_data;

	struct {
		u8_t id;       /* Most recent transaction ID */
		u8_t seg;      /* Bit-field of unreceived segments */
		u8_t last_seg; /* Last segment (to check length) */
		u8_t fcs;      /* Expected FCS value */
		struct net_buf_simple *buf;
	} rx;

	struct {
		/* Start timestamp of the transaction */
		s64_t start;

		/* Transaction id */
		u8_t id;

		/* Current ack id */
		u8_t pending_ack;

		/* Pending outgoing buffer(s) */
		struct net_buf *buf[3];

		prov_bearer_send_complete_t cb;

		void *cb_data;

		/* Retransmit timer */
		struct k_delayed_work retransmit;
	} tx;

	/* Protocol timeout */
	struct k_delayed_work prot_timer;
};

struct prov_rx {
	u32_t link_id;
	u8_t xact_id;
	u8_t gpc;
};

NET_BUF_SIMPLE_DEFINE_STATIC(rx_buf, RX_BUFFER_MAX);

static struct pb_adv link = { .rx = { .buf = &rx_buf } };

static void gen_prov_ack_send(u8_t xact_id);
static void link_open(struct prov_rx *rx, struct net_buf_simple *buf);
static void link_ack(struct prov_rx *rx, struct net_buf_simple *buf);
static void link_close(struct prov_rx *rx, struct net_buf_simple *buf);

/* Add declaration by bouffalo*/
static int bearer_ctl_send(u8_t op, const void *data, u8_t data_len,
			   bool reliable);

static void buf_sent(int err, void *user_data)
{
	if (!link.tx.buf[0]) {
		return;
	}

	k_delayed_work_submit(&link.tx.retransmit, RETRANSMIT_TIMEOUT);
}

static struct bt_mesh_send_cb buf_sent_cb = {
	.end = buf_sent,
};

static u8_t last_seg(u16_t len)
{
	if (len <= START_PAYLOAD_MAX) {
		return 0;
	}

	len -= START_PAYLOAD_MAX;

	return 1 + (len / CONT_PAYLOAD_MAX);
}

static void free_segments(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(link.tx.buf); i++) {
		struct net_buf *buf = link.tx.buf[i];

		if (!buf) {
			break;
		}

		link.tx.buf[i] = NULL;
		/* Mark as canceled */
		BT_MESH_ADV(buf)->busy = 0U;
		net_buf_unref(buf);
	}
}

static u8_t next_transaction_id(u8_t id)
{
	return (((id + 1) & XACT_ID_MAX) | (id & (XACT_ID_MAX+1)));
}

static void prov_clear_tx(void)
{
	BT_DBG("");

	k_delayed_work_cancel(&link.tx.retransmit);

	free_segments();
}

static void reset_adv_link(void)
{
	BT_DBG("");
	prov_clear_tx();

	k_delayed_work_cancel(&link.prot_timer);

	if (atomic_test_bit(link.flags, PROVISIONER)) {
		/* Clear everything except the retransmit and protocol timer
		 * delayed work objects.
		 */
		(void)memset(&link, 0, offsetof(struct pb_adv, tx.retransmit));
		link.rx.id = XACT_ID_NVAL;
	} else {
		/* Accept another provisioning attempt */
		link.id = 0;
		atomic_clear(link.flags);
		link.rx.id = XACT_ID_MAX;
		link.tx.id = XACT_ID_NVAL;
	}

	link.tx.pending_ack = XACT_ID_NVAL;
	link.rx.buf = &rx_buf;
	net_buf_simple_reset(link.rx.buf);
}

static void close_link(enum prov_bearer_link_status reason)
{
	const struct prov_bearer_cb *cb = link.cb;
	void *cb_data = link.cb_data;

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
	/* Added by bouffalo for MESH/PVNR/PBADV/BV-01-C PTS */
	u8_t sts = PROV_BEARER_LINK_STATUS_FAIL;
	bearer_ctl_send(LINK_CLOSE, &sts, 1, false);
#endif
	reset_adv_link();
	cb->link_closed(&pb_adv, cb_data, reason);
}

static struct net_buf *adv_buf_create(u8_t retransmits)
{
	struct net_buf *buf;

	buf = bt_mesh_adv_create(BT_MESH_ADV_PROV,
				 BT_MESH_TRANSMIT(retransmits, 20),
				 BUF_TIMEOUT);
	if (!buf) {
		BT_ERR("Out of provisioning buffers");
		return NULL;
	}

	return buf;
}

static void ack_complete(u16_t duration, int err, void *user_data)
{
	BT_DBG("xact 0x%x complete", (u8_t)link.tx.pending_ack);
	atomic_clear_bit(link.flags, ACK_PENDING);
}

static bool ack_pending(void)
{
	return atomic_test_bit(link.flags, ACK_PENDING);
}

static void prov_failed(u8_t err)
{
	BT_DBG("%u", err);
	link.cb->error(&pb_adv, link.cb_data, err);
	atomic_set_bit(link.flags, LINK_INVALID);
}

static void prov_msg_recv(void)
{
	k_delayed_work_submit(&link.prot_timer, PROTOCOL_TIMEOUT);

	if (!bt_mesh_fcs_check(link.rx.buf, link.rx.fcs)) {
		BT_ERR("Incorrect FCS");
		return;
	}

	gen_prov_ack_send(link.rx.id);

	if (atomic_test_bit(link.flags, LINK_INVALID)) {
		BT_WARN("Unexpected msg 0x%02x on invalidated link",
			link.rx.buf->data[0]);
		prov_failed(PROV_ERR_UNEXP_PDU);
		return;
	}

	link.cb->recv(&pb_adv, link.cb_data, link.rx.buf);
}

static void protocol_timeout(struct k_work *work)
{
	BT_DBG("");

	link.rx.seg = 0U;
	close_link(PROV_BEARER_LINK_STATUS_TIMEOUT);
}
/*******************************************************************************
 * Generic provisioning
 ******************************************************************************/

static void gen_prov_ack_send(u8_t xact_id)
{
	static const struct bt_mesh_send_cb cb = {
		.start = ack_complete,
	};
	const struct bt_mesh_send_cb *complete;
	struct net_buf *buf;
	bool pending = atomic_test_and_set_bit(link.flags, ACK_PENDING);

	BT_DBG("xact_id 0x%x", xact_id);

	if (pending && link.tx.pending_ack == xact_id) {
		BT_DBG("Not sending duplicate ack");
		return;
	}

	buf = adv_buf_create(RETRANSMITS_ACK);
	if (!buf) {
		atomic_clear_bit(link.flags, ACK_PENDING);
		return;
	}

	if (pending) {
		complete = NULL;
	} else {
		link.tx.pending_ack = xact_id;
		complete = &cb;
	}

	net_buf_add_be32(buf, link.id);
	net_buf_add_u8(buf, xact_id);
	net_buf_add_u8(buf, GPC_ACK);

	bt_mesh_adv_send(buf, complete, NULL);
	net_buf_unref(buf);
}

static void gen_prov_cont(struct prov_rx *rx, struct net_buf_simple *buf)
{
	u8_t seg = CONT_SEG_INDEX(rx->gpc);

	BT_DBG("len %u, seg_index %u", buf->len, seg);

	if (!link.rx.seg && link.rx.id == rx->xact_id) {
		if (!ack_pending()) {
			BT_DBG("Resending ack");
			gen_prov_ack_send(rx->xact_id);
		}

		return;
	}

	if (!link.rx.seg &&
	    next_transaction_id(link.rx.id) == rx->xact_id) {
		BT_DBG("Start segment lost");

		link.rx.id = rx->xact_id;

		net_buf_simple_reset(link.rx.buf);

		link.rx.seg = SEG_NVAL;
		link.rx.last_seg = SEG_NVAL;

		prov_clear_tx();
	} else if (rx->xact_id != link.rx.id) {
		BT_WARN("Data for unknown transaction (0x%x != 0x%x)",
				rx->xact_id, link.rx.id);
		return;
	}

	if (seg > link.rx.last_seg) {
		BT_ERR("Invalid segment index %u", seg);
		prov_failed(PROV_ERR_NVAL_FMT);
		return;
	}

	if (!(link.rx.seg & BIT(seg))) {
		BT_DBG("Ignoring already received segment");
		return;
	}

	if (XACT_SEG_OFFSET(seg) + buf->len > RX_BUFFER_MAX) {
		BT_WARN("Rx buffer overflow. Malformed generic prov frame?");
		return;
	}

	memcpy(XACT_SEG_DATA(seg), buf->data, buf->len);
	XACT_SEG_RECV(seg);

	if (seg == link.rx.last_seg && !(link.rx.seg & BIT(0))) {
		u8_t expect_len;

		expect_len = (link.rx.buf->len - 20U -
				((link.rx.last_seg - 1) * 23U));
		if (expect_len != buf->len) {
			BT_ERR("Incorrect last seg len: %u != %u", expect_len,
					buf->len);
			prov_failed(PROV_ERR_NVAL_FMT);
			return;
		}
	}

	if (!link.rx.seg) {
		prov_msg_recv();
	}
}

static void gen_prov_ack(struct prov_rx *rx, struct net_buf_simple *buf)
{
	BT_DBG("len %u", buf->len);

	if (!link.tx.buf[0]) {
		return;
	}

	if (rx->xact_id == link.tx.id) {
		/* Don't clear resending of LINK_CLOSE messages */
		if (!atomic_test_bit(link.flags, LINK_CLOSING)) {
			prov_clear_tx();
		}

		if (link.tx.cb) {
			link.tx.cb(0, link.tx.cb_data);
		}
	}
}

#if defined(BFLB_BLE)
extern unsigned int find_msb_set(uint32_t data);
#endif
static void gen_prov_start(struct prov_rx *rx, struct net_buf_simple *buf)
{
	u8_t seg = SEG_NVAL;

	if (rx->xact_id == link.rx.id) {
		if (!link.rx.seg) {
			if (!ack_pending()) {
				BT_DBG("Resending ack");
				gen_prov_ack_send(rx->xact_id);
			}

			return;
		}

		if (!(link.rx.seg & BIT(0))) {
			BT_DBG("Ignoring duplicate segment");
			return;
		}
	} else if (rx->xact_id != next_transaction_id(link.rx.id)) {
		BT_WARN("Unexpected xact 0x%x, expected 0x%x", rx->xact_id,
			next_transaction_id(link.rx.id));
		return;
	}

	net_buf_simple_reset(link.rx.buf);
	link.rx.buf->len = net_buf_simple_pull_be16(buf);
	link.rx.id = rx->xact_id;
	link.rx.fcs = net_buf_simple_pull_u8(buf);

	BT_DBG("len %u last_seg %u total_len %u fcs 0x%02x", buf->len,
	       START_LAST_SEG(rx->gpc), link.rx.buf->len, link.rx.fcs);

	if (link.rx.buf->len < 1) {
		BT_ERR("Ignoring zero-length provisioning PDU");
		prov_failed(PROV_ERR_NVAL_FMT);
		return;
	}

	if (link.rx.buf->len > link.rx.buf->size) {
		BT_ERR("Too large provisioning PDU (%u bytes)",
		       link.rx.buf->len);
		prov_failed(PROV_ERR_NVAL_FMT);
		return;
	}

	if (START_LAST_SEG(rx->gpc) > 0 && link.rx.buf->len <= 20U) {
		BT_ERR("Too small total length for multi-segment PDU");
		prov_failed(PROV_ERR_NVAL_FMT);
		return;
	}

	if (START_LAST_SEG(rx->gpc) != last_seg(link.rx.buf->len)) {
		BT_ERR("Invalid SegN (%u, calculated %u)", START_LAST_SEG(rx->gpc),
		       last_seg(link.rx.buf->len));
		prov_failed(PROV_ERR_NVAL_FMT);
		return;
	}

	prov_clear_tx();

	link.rx.last_seg = START_LAST_SEG(rx->gpc);

	if ((link.rx.seg & BIT(0)) &&
		/** Fix bug by bouffalolab, when continue seg received before start seg. 
		 *	The value of "~link.rx.seg" is unexpected. 
		 *  the msb of must Less than or equal to last_seg.
		 * */
	    (find_msb_set((u8_t)~link.rx.seg)-1 > link.rx.last_seg)) {
		BT_ERR("Invalid segment index %u", seg);
		prov_failed(PROV_ERR_NVAL_FMT);
		return;
	}

	if (link.rx.seg) {
		seg = link.rx.seg;
	}

	link.rx.seg = seg & ((1 << (START_LAST_SEG(rx->gpc) + 1)) - 1);
	memcpy(link.rx.buf->data, buf->data, buf->len);
	XACT_SEG_RECV(0);

	if (!link.rx.seg) {
		prov_msg_recv();
	}
}

static void gen_prov_ctl(struct prov_rx *rx, struct net_buf_simple *buf)
{
	BT_DBG("op 0x%02x len %u", BEARER_CTL(rx->gpc), buf->len);

	switch (BEARER_CTL(rx->gpc)) {
	case LINK_OPEN:
		link_open(rx, buf);
		break;
	case LINK_ACK:
		if (!atomic_test_bit(link.flags, LINK_ACTIVE)) {
			return;
		}

		link_ack(rx, buf);
		break;
	case LINK_CLOSE:
		if (!atomic_test_bit(link.flags, LINK_ACTIVE)) {
			return;
		}

		link_close(rx, buf);
		break;
	default:
		BT_ERR("Unknown bearer opcode: 0x%02x", BEARER_CTL(rx->gpc));

		#if defined(CONFIG_AUTO_PTS)
		/* if (IS_ENABLED(CONFIG_AUTO_PTS)) */{
			bt_test_mesh_prov_invalid_bearer(BEARER_CTL(rx->gpc));
		}
		#endif /* CONFIG_AUTO_PTS */

		return;
	}
}

static const struct {
	void (*func)(struct prov_rx *rx, struct net_buf_simple *buf);
	bool require_link;
	u8_t min_len;
} gen_prov[] = {
	{ gen_prov_start, true, 3 },
	{ gen_prov_ack, true, 0 },
	{ gen_prov_cont, true, 0 },
	{ gen_prov_ctl, false, 0 },
};

static void gen_prov_recv(struct prov_rx *rx, struct net_buf_simple *buf)
{
	if (buf->len < gen_prov[GPCF(rx->gpc)].min_len) {
		BT_ERR("Too short GPC message type %u", GPCF(rx->gpc));
		return;
	}

	if (!atomic_test_bit(link.flags, LINK_ACTIVE) &&
	    gen_prov[GPCF(rx->gpc)].require_link) {
		BT_DBG("Ignoring message that requires active link");
		return;
	}

	gen_prov[GPCF(rx->gpc)].func(rx, buf);
}

/*******************************************************************************
 * TX
 ******************************************************************************/

static void send_reliable(void)
{
	int i;

	link.tx.start = k_uptime_get();

	for (i = 0; i < ARRAY_SIZE(link.tx.buf); i++) {
		struct net_buf *buf = link.tx.buf[i];

		if (!buf) {
			break;
		}

		if (i + 1 < ARRAY_SIZE(link.tx.buf) && link.tx.buf[i + 1]) {
			bt_mesh_adv_send(buf, NULL, NULL);
		} else {
			bt_mesh_adv_send(buf, &buf_sent_cb, NULL);
		}
	}
}

static void prov_retransmit(struct k_work *work)
{
	s32_t timeout_ms;
	int i;

	BT_DBG("");

	if (!atomic_test_bit(link.flags, LINK_ACTIVE)) {
		BT_WARN("Link not active");
		return;
	}

	/*
	 * According to mesh profile spec (5.3.1.4.3), the close message should
	 * be restransmitted at least three times. Retransmit the LINK_CLOSE
	 * message until CLOSING_TIMEOUT has elapsed.
	 */
	if (atomic_test_bit(link.flags, LINK_CLOSING)) {
		timeout_ms = CLOSING_TIMEOUT;
	} else {
		timeout_ms = TRANSACTION_TIMEOUT;
	}

	if (k_uptime_get() - link.tx.start > timeout_ms) {
		if (atomic_test_bit(link.flags, LINK_CLOSING)) {
			close_link(PROV_BEARER_LINK_STATUS_SUCCESS);
		} else {
			BT_WARN("Giving up transaction");
			close_link(PROV_BEARER_LINK_STATUS_TIMEOUT);
		}

		return;
	}

	for (i = 0; i < ARRAY_SIZE(link.tx.buf); i++) {
		struct net_buf *buf = link.tx.buf[i];

		if (!buf) {
			break;
		}

		if (BT_MESH_ADV(buf)->busy) {
			continue;
		}

		BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

		if (i + 1 < ARRAY_SIZE(link.tx.buf) && link.tx.buf[i + 1]) {
			bt_mesh_adv_send(buf, NULL, NULL);
		} else {
			bt_mesh_adv_send(buf, &buf_sent_cb, NULL);
		}
	}
}

static int bearer_ctl_send(u8_t op, const void *data, u8_t data_len,
			   bool reliable)
{
	struct net_buf *buf;

	BT_DBG("op 0x%02x data_len %u", op, data_len);

	prov_clear_tx();
	k_delayed_work_submit(&link.prot_timer, PROTOCOL_TIMEOUT);

	buf = adv_buf_create(reliable ? RETRANSMITS_RELIABLE :
					RETRANSMITS_UNRELIABLE);
	if (!buf) {
		return -ENOBUFS;
	}

	net_buf_add_be32(buf, link.id);
	/* Transaction ID, always 0 for Bearer messages */
	net_buf_add_u8(buf, 0x00);
	net_buf_add_u8(buf, GPC_CTL(op));
	net_buf_add_mem(buf, data, data_len);

	if (reliable) {
		link.tx.buf[0] = buf;
		send_reliable();
	} else {
		bt_mesh_adv_send(buf, &buf_sent_cb, NULL);
		net_buf_unref(buf);
	}

	return 0;
}

static int prov_send_adv(struct net_buf_simple *msg,
			 prov_bearer_send_complete_t cb, void *cb_data)
{
	struct net_buf *start, *buf;
	u8_t seg_len, seg_id;

	prov_clear_tx();
	k_delayed_work_submit(&link.prot_timer, PROTOCOL_TIMEOUT);

	start = adv_buf_create(RETRANSMITS_RELIABLE);
	if (!start) {
		return -ENOBUFS;
	}

	link.tx.id = next_transaction_id(link.tx.id);
	net_buf_add_be32(start, link.id);
	net_buf_add_u8(start, link.tx.id);

	net_buf_add_u8(start, GPC_START(last_seg(msg->len)));
	net_buf_add_be16(start, msg->len);
	net_buf_add_u8(start, bt_mesh_fcs_calc(msg->data, msg->len));

	link.tx.buf[0] = start;
	link.tx.cb = cb;
	link.tx.cb_data = cb_data;

	BT_DBG("xact_id: 0x%x len: %u", link.tx.id, msg->len);

	seg_len = MIN(msg->len, START_PAYLOAD_MAX);
	BT_DBG("seg 0 len %u: %s", seg_len, bt_hex(msg->data, seg_len));
	net_buf_add_mem(start, msg->data, seg_len);
	net_buf_simple_pull(msg, seg_len);

	buf = start;
	for (seg_id = 1U; msg->len > 0; seg_id++) {
		if (seg_id >= ARRAY_SIZE(link.tx.buf)) {
			BT_ERR("Too big message");
			free_segments();
			return -E2BIG;
		}

		buf = adv_buf_create(RETRANSMITS_RELIABLE);
		if (!buf) {
			free_segments();
			return -ENOBUFS;
		}

		link.tx.buf[seg_id] = buf;

		seg_len = MIN(msg->len, CONT_PAYLOAD_MAX);

		BT_DBG("seg %u len %u: %s", seg_id, seg_len,
		       bt_hex(msg->data, seg_len));

		net_buf_add_be32(buf, link.id);
		net_buf_add_u8(buf, link.tx.id);
		net_buf_add_u8(buf, GPC_CONT(seg_id));
		net_buf_add_mem(buf, msg->data, seg_len);
		net_buf_simple_pull(msg, seg_len);
	}

	send_reliable();

	return 0;
}

/*******************************************************************************
 * Link management rx
 ******************************************************************************/

static void link_open(struct prov_rx *rx, struct net_buf_simple *buf)
{
	BT_DBG("len %u", buf->len);

	if (buf->len < 16) {
		BT_ERR("Too short bearer open message (len %u)", buf->len);
		return;
	}

	if (atomic_test_bit(link.flags, LINK_ACTIVE)) {
		/* Send another link ack if the provisioner missed the last */
		if (link.id == rx->link_id) {
			BT_DBG("Resending link ack");
			bearer_ctl_send(LINK_ACK, NULL, 0, false);
		} else {
			BT_DBG("Ignoring bearer open: link already active");
		}

		return;
	}

	if (memcmp(buf->data, bt_mesh_prov_get()->uuid, 16)) {
		BT_DBG("Bearer open message not for us");
		return;
	}

	link.id = rx->link_id;
	atomic_set_bit(link.flags, LINK_ACTIVE);
	net_buf_simple_reset(link.rx.buf);

	bearer_ctl_send(LINK_ACK, NULL, 0, false);

	link.cb->link_opened(&pb_adv, link.cb_data);
}

static void link_ack(struct prov_rx *rx, struct net_buf_simple *buf)
{
	BT_DBG("len %u", buf->len);

	if (atomic_test_bit(link.flags, PROVISIONER)) {
		if (atomic_test_and_set_bit(link.flags, LINK_ACK_RECVD)) {
			return;
		}

		prov_clear_tx();

		link.cb->link_opened(&pb_adv, link.cb_data);
	}
}

static void link_close(struct prov_rx *rx, struct net_buf_simple *buf)
{
	BT_DBG("len %u", buf->len);

	if (buf->len != 1) {
		return;
	}

	close_link(net_buf_simple_pull_u8(buf));
}

/*******************************************************************************
 * Higher level functionality
 ******************************************************************************/

void bt_mesh_pb_adv_recv(struct net_buf_simple *buf)
{
	struct prov_rx rx;

	if (!link.cb) {
		return;
	}

	if (buf->len < 6) {
		BT_WARN("Too short provisioning packet (len %u)", buf->len);
		return;
	}

	rx.link_id = net_buf_simple_pull_be32(buf);
	rx.xact_id = net_buf_simple_pull_u8(buf);
	rx.gpc = net_buf_simple_pull_u8(buf);

	if (atomic_test_bit(link.flags, LINK_ACTIVE) && link.id != rx.link_id) {
		return;
	}

	BT_DBG("link_id 0x%08lx xact_id 0x%x", rx.link_id, rx.xact_id);

	gen_prov_recv(&rx, buf);
}

static int prov_link_open(const u8_t uuid[16], s32_t timeout,
			  const struct prov_bearer_cb *cb, void *cb_data)
{
	BT_DBG("uuid %s", bt_hex(uuid, 16));

	if (atomic_test_and_set_bit(link.flags, LINK_ACTIVE)) {
		return -EBUSY;
	}

	atomic_set_bit(link.flags, PROVISIONER);

	bt_rand(&link.id, sizeof(link.id));
	link.tx.id = XACT_ID_MAX;
	link.rx.id = XACT_ID_NVAL;
	link.cb = cb;
	link.cb_data = cb_data;

	net_buf_simple_reset(link.rx.buf);

	bearer_ctl_send(LINK_OPEN, uuid, 16, true);

	return 0;
}

static int prov_link_accept(const struct prov_bearer_cb *cb, void *cb_data)
{
	if (atomic_test_bit(link.flags, LINK_ACTIVE)) {
		return -EBUSY;
	}

	link.rx.id = XACT_ID_MAX;
	link.tx.id = XACT_ID_NVAL;
	link.cb = cb;
	link.cb_data = cb_data;

	/* Make sure we're scanning for provisioning inviations */
	bt_mesh_scan_enable();
	/* Enable unprovisioned beacon sending */
	bt_mesh_beacon_enable();

	return 0;
}

static void prov_link_close(enum prov_bearer_link_status status)
{
	if (atomic_test_and_set_bit(link.flags, LINK_CLOSING)) {
		return;
	}

	bearer_ctl_send(LINK_CLOSE, &status, 1, true);
}

void pb_adv_init(void)
{
	k_delayed_work_init(&link.prot_timer, protocol_timeout);
	k_delayed_work_init(&link.tx.retransmit, prov_retransmit);
}

void pb_adv_reset(void)
{
	reset_adv_link();
}

const struct prov_bearer pb_adv = {
	.type = BT_MESH_PROV_ADV,
	.link_open = prov_link_open,
	.link_accept = prov_link_accept,
	.link_close = prov_link_close,
	.send = prov_send_adv,
	.clear_tx = prov_clear_tx,
};
