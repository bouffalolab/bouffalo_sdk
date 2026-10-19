/*
 * SPP Throughput Test
 *
 * Per-link TX throughput benchmark for SPP. Each connected link runs its own
 * sender thread that calls bt_spp_send() back-to-back; because the underlying
 * RFCOMM buffer allocation blocks (K_FOREVER) when the pool is exhausted, the
 * sender naturally applies back-pressure instead of busy-spinning. A separate
 * stats thread prints per-link Mbps once per second.
 *
 * Copyright (c) 2021-2022 Bouffalolab Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <printk.h>
#include <conn.h>

#include "bt_log.h"
#include "spp.h"
#include "spp_throughput.h"

#ifndef CONFIG_SPP_TP_STACK_SIZE
#define CONFIG_SPP_TP_STACK_SIZE 1536
#endif

/* Sender priority must stay BELOW the RFCOMM TX thread
 * (CONFIG_BT_RFCOMM_TX_PRIO = configMAX_PRIORITIES - 5). bt_spp_send only
 * enqueues into the DLC tx_queue; the RFCOMM TX thread does the real sending.
 * If the senders run at/above that priority they starve the RFCOMM TX thread,
 * the queues and buffer pool jam, and multi-link throughput collapses.
 */
#ifndef CONFIG_SPP_TP_PRIO
#define CONFIG_SPP_TP_PRIO (configMAX_PRIORITIES - 6)
#endif

#define SPP_TP_DEFAULT_LEN 672

/* Rate-limit window. Each link sends its per-window byte budget, then sleeps
 * until the next window. A small window keeps the output smooth.
 */
#define SPP_TP_WINDOW_MS 100

struct spp_tp_link {
	struct k_thread thread;
	struct bt_conn *conn;
	volatile uint64_t tx_bytes;   /* total bytes accepted by bt_spp_send */
	volatile uint64_t last_bytes; /* snapshot for per-interval rate */
	volatile bool running;
	uint16_t len;
	uint32_t rate_bps;            /* target bytes/sec, 0 = unlimited */
	uint8_t idx;
};

static struct spp_tp_link spp_tp_links[CONFIG_SPP_MAX_CONN];
static struct k_thread spp_tp_stats_thread;
static volatile bool spp_tp_stats_running;
static uint8_t spp_tp_payload[1024];

/* The port's k_thread_create() passes NULL as the thread argument, so a newly
 * created thread cannot receive its context through p1. Hand the link pointer
 * over via a global guarded by a semaphore: the creator blocks until the new
 * thread has consumed spp_tp_thread_arg, preventing back-to-back starts from
 * overwriting it.
 */
static struct spp_tp_link *spp_tp_thread_arg;
static struct k_sem spp_tp_thread_sync;
static bool spp_tp_sync_inited;

static void spp_tp_sender(void *p1)
{
	struct spp_tp_link *link = spp_tp_thread_arg;

	(void)p1;
	k_sem_give(&spp_tp_thread_sync);

	BT_WARN("spp tp sender start idx %u len %u rate %lu B/s", link->idx,
		link->len, (unsigned long)link->rate_bps);

	/* Bytes allowed per SPP_TP_WINDOW_MS window (0 = no rate limit). */
	uint32_t budget = link->rate_bps ?
			  (link->rate_bps * SPP_TP_WINDOW_MS) / 1000 : 0;
	uint32_t sent = 0;

	while (link->running) {
		int ret = bt_spp_send(link->conn, spp_tp_payload, link->len);
		if (ret) {
			/* Link gone or fatal error: stop this sender. */
			BT_WARN("spp tp idx %u send err %d, stop", link->idx, ret);
			break;
		}

		link->tx_bytes += link->len;
		sent += link->len;

		/* Hold the target rate: once the window budget is spent, sleep
		 * to the next window instead of sending at full speed.
		 */
		if (budget && sent >= budget) {
			sent = 0;
			k_sleep(SPP_TP_WINDOW_MS);
		}
	}

	link->running = false;
	BT_WARN("spp tp sender exit idx %u", link->idx);
	k_thread_delete(&link->thread);
}

static void spp_tp_stats(void *p1)
{
	(void)p1;

	while (spp_tp_stats_running) {
		k_sleep(1000);

		bool any = false;

		for (int i = 0; i < CONFIG_SPP_MAX_CONN; i++) {
			struct spp_tp_link *link = &spp_tp_links[i];

			if (!link->running && link->tx_bytes == link->last_bytes) {
				continue;
			}

			uint64_t now = link->tx_bytes;
			uint64_t delta = now - link->last_bytes;
			link->last_bytes = now;
			any = true;

			/* delta bytes in 1s -> kbps = delta*8/1000 */
			uint32_t kbps = (uint32_t)((delta * 8) / 1000);
			printf("[spp_tp] idx %u: %lu.%03lu Mbps (%lu bytes/s, total %lu KB)\r\n",
			       link->idx,
			       (unsigned long)(kbps / 1000),
			       (unsigned long)(kbps % 1000),
			       (unsigned long)delta,
			       (unsigned long)(now / 1024));
		}

		if (!any) {
			/* Nothing active: stop the stats thread. */
			break;
		}
	}

	spp_tp_stats_running = false;
	k_thread_delete(&spp_tp_stats_thread);
}

static void spp_tp_start_stats(void)
{
	if (spp_tp_stats_running) {
		return;
	}
	spp_tp_stats_running = true;
	k_thread_create(&spp_tp_stats_thread, "spp_tp_stats",
			CONFIG_SPP_TP_STACK_SIZE, spp_tp_stats,
			CONFIG_SPP_TP_PRIO);
}

static int spp_tp_start_one(uint8_t idx, uint16_t len, uint32_t rate_bps)
{
	struct spp_tp_link *link;
	struct bt_conn *conn;

	if (idx >= CONFIG_SPP_MAX_CONN) {
		return -1;
	}

	conn = bt_spp_get_conn(idx);
	if (!conn) {
		printf("[spp_tp] idx %u not connected\r\n", idx);
		return -1;
	}

	link = &spp_tp_links[idx];
	if (link->running) {
		printf("[spp_tp] idx %u already running\r\n", idx);
		return -1;
	}

	link->conn = conn;
	link->idx = idx;
	link->len = len;
	link->rate_bps = rate_bps;
	link->tx_bytes = 0;
	link->last_bytes = 0;
	link->running = true;

	if (!spp_tp_sync_inited) {
		k_sem_init(&spp_tp_thread_sync, 0, 1);
		spp_tp_sync_inited = true;
	}

	spp_tp_thread_arg = link;
	k_thread_create(&link->thread, "spp_tp_tx",
			CONFIG_SPP_TP_STACK_SIZE, spp_tp_sender,
			CONFIG_SPP_TP_PRIO);
	/* Wait until the new sender thread has read spp_tp_thread_arg. */
	k_sem_take(&spp_tp_thread_sync, K_FOREVER);

	printf("[spp_tp] idx %u started, len %u\r\n", idx, len);
	return 0;
}

/* idx < 0 means all connected links. rate_bps is the per-link target in
 * bytes/sec; 0 means send as fast as possible (no rate limit).
 */
int spp_tp_start(int idx, uint16_t len, uint32_t rate_bps)
{
	int started = 0;

	if (len == 0 || len > sizeof(spp_tp_payload)) {
		len = SPP_TP_DEFAULT_LEN;
	}

	/* fill payload with a recognizable ramp */
	for (uint16_t i = 0; i < len; i++) {
		spp_tp_payload[i] = (i + 1) & 0xFF;
	}

	if (idx >= 0) {
		if (spp_tp_start_one((uint8_t)idx, len, rate_bps) == 0) {
			started++;
		}
	} else {
		for (int i = 0; i < CONFIG_SPP_MAX_CONN; i++) {
			if (bt_spp_get_conn(i) &&
			    spp_tp_start_one((uint8_t)i, len, rate_bps) == 0) {
				started++;
			}
		}
	}

	if (started > 0) {
		spp_tp_start_stats();
	} else {
		printf("[spp_tp] no link started\r\n");
	}

	return started;
}

/* idx < 0 means all links. */
void spp_tp_stop(int idx)
{
	if (idx >= 0) {
		if (idx < CONFIG_SPP_MAX_CONN) {
			spp_tp_links[idx].running = false;
		}
	} else {
		for (int i = 0; i < CONFIG_SPP_MAX_CONN; i++) {
			spp_tp_links[i].running = false;
		}
	}
	printf("[spp_tp] stop requested\r\n");
}
