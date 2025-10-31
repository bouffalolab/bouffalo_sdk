#ifndef __NETBUS_TTY_H__
#define __NETBUS_TTY_H__

#include <stdio.h>
#include <stdint.h>
#include <utils_list.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stream_buffer.h>
#include <stdint.h>

typedef void (*netbus_tty_cb_func_t)(void *arg);

typedef struct netbus_tty_ctx {
    StreamBufferHandle_t txbuf;
    StreamBufferHandle_t rxbuf;
    SemaphoreHandle_t w_mutex;
    uint8_t buffer[128];
}netbus_tty_ctx_t;

int netbus_tty_init(netbus_tty_ctx_t *pctx, uint32_t txbuf_size, uint32_t rxbuf_size);
int netbus_tty_send(netbus_tty_ctx_t *pctx, const uint8_t *p_data, uint32_t len, uint32_t timeout);
int netbus_tty_receive(netbus_tty_ctx_t *pctx, uint8_t *p_buffer, uint32_t buf_len, uint32_t timeout);
int netbus_tty_deinit(netbus_tty_ctx_t *ctx);

#endif
