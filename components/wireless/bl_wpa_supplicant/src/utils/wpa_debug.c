/*
 * wpa_supplicant/hostapd / Debug prints
 * Copyright (c) 2002-2007, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */
#include "utils/includes.h"
#include "utils/common.h"
#include "utils/wpa_debug.h"
#include "timeout.h"
#include "wl80211_platform.h"

#ifndef ELOOP_ALL_CTX
#define ELOOP_ALL_CTX ((void *) -1)
#endif

struct eloop_timeout_entry {
    TAILQ_ENTRY(eloop_timeout_entry) entry;
    struct timeout_s timer;
    eloop_timeout_handler handler;
    void *eloop_data;
    void *user_data;
};

static TAILQ_HEAD(eloop_timeout_head, eloop_timeout_entry) g_eloop_timeouts =
    TAILQ_HEAD_INITIALIZER(g_eloop_timeouts);

static void eloop_timeout_wrapper(struct timeout_s *timeout)
{
    struct eloop_timeout_entry *entry;

    if (timeout == NULL) {
        return;
    }

    entry = container_of(timeout, struct eloop_timeout_entry, timer);

    rtos_lock();
    TAILQ_REMOVE(&g_eloop_timeouts, entry, entry);
    rtos_unlock();

    entry->handler(entry->eloop_data, entry->user_data);
    free(entry);
}

static inline int
_wpa_snprintf_hex(char *buf, size_t buf_size, const u8 *data, size_t len,
                  int uppercase, int whitespace)
{
    size_t i;
    char *pos = buf, *end = buf + buf_size;
    int ret;

    static const char *fmt_upper = "%02X";
    static const char *fmt_lower = "%02x";
    static const char *fmt_upper_ws = "%02X ";
    static const char *fmt_lower_ws = "%02x ";
    const char *fmt = uppercase ? (whitespace ? fmt_upper_ws : fmt_upper) :
                                  (whitespace ? fmt_lower_ws : fmt_lower);

    if (buf_size == 0)
        return 0;

    for (i = 0; i < len; i++) {
        ret = snprintf(pos, end - pos, fmt, data[i]);
        if (ret < 0 || ret >= end - pos) {
            end[-1] = '\0';
            return pos - buf;
        }
        pos += ret;
    }
    end[-1]='\0';
    return pos - buf;
}

int  wpa_snprintf_hex_uppercase(char *buf, size_t buf_size, const u8 *data, size_t len)
{
	return _wpa_snprintf_hex(buf, buf_size, data, len, 1, 0);
}

int  wpa_snprintf_hex(char *buf, size_t buf_size, const u8 *data, size_t len)
{
	return _wpa_snprintf_hex(buf, buf_size, data, len, 0, 0);
}

#ifdef DEBUG_PRINT
void  wpa_dump_mem(char* desc, uint8_t *addr, uint16_t len)
{
    char output[50];
    wpa_printf(MSG_DEBUG, "%s", desc);
    if (addr){
        uint16_t i=0;
        for (i = 0; i < len / 16; i++) {
            _wpa_snprintf_hex(output, 50, addr + i * 16, 16, 0, 1);
            wpa_printf(MSG_DEBUG, "%s", output);
        }
        if (len % 16) {
            int bytes_printed = (len / 16) * 16;
            _wpa_snprintf_hex(output, 50, addr + bytes_printed,
                              len - bytes_printed, 0, 1);
            wpa_printf(MSG_DEBUG, "%s", output);
        }
    }
}

void  wpa_debug_print_timestamp(void)
{
#ifdef DEBUG_PRINT
    struct os_time tv;
    os_get_time(&tv);
    wpa_printf(MSG_DEBUG, "%ld.%06u: ", (long) tv.sec, (unsigned int) tv.usec);
#endif
}

void  wpa_hexdump(int level, const char *title, const u8 *buf, size_t len)
{
#ifdef DEBUG_PRINT
	size_t i;
	char output[50];

	wpa_printf(level, "%s - hexdump(len=%lu):", title, (unsigned long) len);
	if (buf == NULL) {
		wpa_printf(level, " [NULL]");
	} else {
		for (i = 0; i < len / 16; i++) {
			_wpa_snprintf_hex(output, 50, buf + i * 16, 16, 0, 1);
			wpa_printf(level, "%s", output);
		}
		if (len % 16) {
			int bytes_printed = (len / 16) * 16;
			_wpa_snprintf_hex(output, 50, buf + bytes_printed,
							  len - bytes_printed, 0, 1);
			wpa_printf(level, "%s", output);
		}
	}
#endif
}

void  wpa_hexdump_key(int level, const char *title, const u8 *buf, size_t len)
{
     wpa_hexdump(level, title, buf, len);
}
#endif

int  eloop_cancel_timeout(eloop_timeout_handler handler,
			 void *eloop_data, void *user_data)
{
    struct eloop_timeout_entry *entry;
    struct eloop_timeout_entry *tmp;
    int removed = 0;

    if (handler == NULL) {
        return 0;
    }

    rtos_lock();
    TAILQ_FOREACH_SAFE(entry, &g_eloop_timeouts, entry, tmp)
    {
        bool eloop_match;
        bool user_match;

        if (entry->handler != handler) {
            continue;
        }

        eloop_match = eloop_data == ELOOP_ALL_CTX ||
                      entry->eloop_data == eloop_data;
        user_match = user_data == ELOOP_ALL_CTX ||
                     entry->user_data == user_data;
        if (!eloop_match || !user_match) {
            continue;
        }

        TAILQ_REMOVE(&g_eloop_timeouts, entry, entry);
        rtos_unlock();
        timeout_stop(&entry->timer);
        free(entry);
        removed++;
        rtos_lock();
    }
    rtos_unlock();

    return removed;
}

int  eloop_register_timeout(unsigned int secs, unsigned int usecs,
			   eloop_timeout_handler handler,
			   void *eloop_data, void *user_data)
{
    struct eloop_timeout_entry *entry;
    uint64_t delay_ms;

    if (handler == NULL) {
        return -1;
    }

    entry = calloc(1, sizeof(*entry));
    if (entry == NULL) {
        return -1;
    }

    delay_ms = (uint64_t) secs * 1000ULL;
    if (usecs != 0) {
        delay_ms += (usecs + 999U) / 1000U;
    }
    if (delay_ms == 0) {
        delay_ms = 1;
    }

    entry->handler = handler;
    entry->eloop_data = eloop_data;
    entry->user_data = user_data;
    entry->timer.callback = eloop_timeout_wrapper;
    entry->timer.opaque = NULL;

    rtos_lock();
    TAILQ_INSERT_TAIL(&g_eloop_timeouts, entry, entry);
    rtos_unlock();

    timeout_start(&entry->timer, (unsigned int) delay_ms);
    return 0;
}
