/** @file
 *  @brief Custom logging over UART
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#if defined(CONFIG_BT_DEBUG_MONITOR)

#include <zephyr.h>
#include <buf.h>
#include "monitor.h"
#include "bt_log.h"

#define NUMERIC_CHARACTERS 10

static inline void putd(uint8_t data){
    char c;
    c = (data >> 4) & 0xf;
    if (c < NUMERIC_CHARACTERS) {
        c = '0' + c;
    } else {
        c = 'A' + (c - NUMERIC_CHARACTERS);
    }
    putchar(c);

    c = data & 0xf;
    if (c < NUMERIC_CHARACTERS) {
        c = '0' + c;
    } else {
        c = 'A' + (c - NUMERIC_CHARACTERS);
    }
    putchar(c);
}

void bt_monitor_send(uint16_t opcode, const void *data, size_t len)
{
    const uint8_t *buf = data;
    unsigned int key = irq_lock();

    #if 0
        BT_WARN("[Hci]:pkt_type:[0x%x],pkt_data:[%s]\r\n", opcode, bt_hex(buf, len));
    #else
        // HCI monitor string
        puts("[Hci]:pkt_type:[0x");
        // Print uint16 type of the opcode
        putd(opcode);
        puts("],pkt_data:[");

        // print data by putc
        for (size_t i = 0; i < len; i++) {
            putd(buf[i]);
        }

        puts("]\r\n");

    #endif

    irq_unlock(key);
}

void bt_monitor_new_index(uint8_t type, uint8_t bus, bt_addr_t *addr,
			  const char *name)
{

}
#endif
