/* log.c - logging helpers */

/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Helper for printk parameters to convert from binary to hex.
 * We declare multiple buffers so the helper can be used multiple times
 * in a single printk call.
 */

#include <stddef.h>
#include <zephyr/types.h>
#include <zephyr.h>
#include <misc/util.h>
#include <bt_errno.h>
#include <bluetooth.h>
#include <hci_host.h>
#include "bl_port.h"
#include "bt_log.h"

#if defined(CONFIG_BT_DEBUG_MONITOR)
#define HEX_STR_SIZE 512
#else
#define HEX_STR_SIZE 128
#endif

#if (CONFIG_BLE_USING_DYNAMIC_RAM)
static char *p_hex_str;
static char *p_addr_str;
static char *p_addr_le_str;
static uint8_t *g_log_mem_pool = NULL;
static bool log_initialized = false;
#endif /* CONFIG_BLE_USING_DYNAMIC_RAM */

const char *bt_hex_real(const void *buf, size_t len)
{
	static const char hex[] = "0123456789abcdef";
#if (CONFIG_BLE_USING_DYNAMIC_RAM)
	char *str = p_hex_str;
#else
	static char str[HEX_STR_SIZE];
#endif /* CONFIG_BLE_USING_DYNAMIC_RAM */
	const u8_t *b = buf;
	int i;

	len = MIN(len, (HEX_STR_SIZE - 1) / 2);

	for (i = 0; i < len; i++) {
		str[i * 2]     = hex[b[i] >> 4];
		str[i * 2 + 1] = hex[b[i] & 0xf];
	}

	str[i * 2] = '\0';

	return str;
}

const char *bt_addr_str_real(const bt_addr_t *addr)
{
#if (CONFIG_BLE_USING_DYNAMIC_RAM)
	char *str = p_addr_str;
#else
	static char str[BT_ADDR_STR_LEN];
#endif /* CONFIG_BLE_USING_DYNAMIC_RAM */

	bt_addr_to_str(addr, str, BT_ADDR_STR_LEN);

	return str;
}

const char *bt_addr_le_str_real(const bt_addr_le_t *addr)
{
#if (CONFIG_BLE_USING_DYNAMIC_RAM)
	char *str = p_addr_le_str;
#else
	static char str[BT_ADDR_LE_STR_LEN];
#endif

	bt_addr_le_to_str(addr, str, BT_ADDR_LE_STR_LEN);

	return str;
}

#if (CONFIG_BLE_USING_DYNAMIC_RAM)
int bt_log_init(void)
{
	if (log_initialized) {
		return 0;
	}

	const size_t hex_str_size = MEM_ALIGN_32(HEX_STR_SIZE * sizeof(char));
	const size_t addr_str_size = MEM_ALIGN_32(BT_ADDR_STR_LEN * sizeof(char));
	const size_t addr_le_str_size = MEM_ALIGN_32(BT_ADDR_LE_STR_LEN * sizeof(char));

	const size_t total_size = hex_str_size + addr_str_size + addr_le_str_size;

	g_log_mem_pool = (uint8_t *)k_malloc(total_size);
	if (!g_log_mem_pool) {
		BT_ERR("Failed to allocate log mem pool: %u bytes", total_size);
		return -ENOMEM;
	}
	p_hex_str = (char *)(g_log_mem_pool + 0);
	p_addr_str = (char *)(g_log_mem_pool + hex_str_size);
	p_addr_le_str = (char *)(g_log_mem_pool + hex_str_size + addr_str_size);

	log_initialized = true;

	return 0;
}

void bt_log_deinit(void)
{
	if (g_log_mem_pool) {
		k_free(g_log_mem_pool);
		g_log_mem_pool = NULL;
	}
	p_hex_str = NULL;
	p_addr_str = NULL;
	p_addr_le_str = NULL;

	log_initialized = false;
}

#endif /* CONFIG_BLE_USING_DYNAMIC_RAM */

