#ifndef __RAMSYNC_LOW_H__
#define __RAMSYNC_LOW_H__

#include <stdio.h>
#include <stdint.h>
#include <utils_list.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stream_buffer.h>
#include <stdint.h>

typedef void (*netbus_uart_cb_func_t)(void *arg);

typedef struct __node_mem {
    void        *buf;
    uint32_t    len;
} node_mem_t;

typedef struct netbus_uart_config {
    const char *name;

    uint32_t speed;

    uint8_t databits;
    uint8_t stopbits;
    uint8_t parity;
    uint8_t flow_control;

#if NETBUS_UART_DMA
    const char *tx_dmach;
    const char *rx_dmach;

    uint32_t dma_req_tx;
    uint32_t dma_req_rx;
    uint32_t dma_tdr;
    uint32_t dma_rdr;
#endif
} netbus_uart_config_t;

typedef struct netbus_uart_ctx {
    const netbus_uart_config_t *config;

#if NETBUS_UART_DMA
    uint8_t *txslot_addr;
    uint8_t *rxslot_addr;
    uint16_t txslot_cur;
    uint16_t rxslot_cur;

    node_mem_t *node_tx;
    node_mem_t *node_rx;
    uint32_t   items_tx;
    uint32_t   items_rx;

    void *dma_tx_chan;
    void *dma_rx_chan;
#endif

    netbus_uart_cb_func_t tx_cb;
    void               *tx_arg;
    netbus_uart_cb_func_t rx_cb;
    void               *rx_arg;
    netbus_uart_cb_func_t reset_signal_cb;
    void               *reset_signal_arg;

    void *priv;

    StreamBufferHandle_t txbuf;
    StreamBufferHandle_t rxbuf;
    TaskHandle_t task;
    SemaphoreHandle_t w_mutex;
    uint8_t buffer[128];
} netbus_uart_ctx_t;

int netbus_uart_init(netbus_uart_ctx_t *pctx, const netbus_uart_config_t *hwcfg, uint32_t txbuf_size, uint32_t rxbuf_size);
int netbus_uart_send(netbus_uart_ctx_t *pctx, const uint8_t *p_data, uint32_t len, uint32_t timeout);
int netbus_uart_receive(netbus_uart_ctx_t *pctx, uint8_t *p_buffer, uint32_t buf_len, uint32_t timeout);
int netbus_uart_deinit(netbus_uart_ctx_t *ctx);

#endif



