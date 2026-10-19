#ifndef __RAMSYNC_LOW_H__
#define __RAMSYNC_LOW_H__

#include <stdio.h>
#include <stdint.h>
#include <utils_list.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdint.h>

#include "spisync_config.h"


typedef void (*lramsync_cb_func_t)(void *arg);

typedef struct __node_mem {
    void        *buf;
    uint32_t    len;
} node_mem_t;

typedef struct lramsync_ctx {
    const spisync_hw_t *config;

    node_mem_t *node_tx;
    node_mem_t *node_rx;
    uint32_t   items_tx;
    uint32_t   items_rx;

    lramsync_cb_func_t tx_cb;
    void               *tx_arg;
    lramsync_cb_func_t rx_cb;
    void               *rx_arg;
    lramsync_cb_func_t reset_signal_cb;
    void               *reset_signal_arg;

    void *dma_tx_chan;
    void *dma_rx_chan;

    void *priv;
} lramsync_ctx_t;

int lramsync_init(
    const spisync_hw_t *config,
    lramsync_ctx_t *ctx,
    node_mem_t *node_tx, uint32_t items_tx,
    lramsync_cb_func_t tx_cb, void *tx_arg,
    node_mem_t *node_rx, uint32_t items_rx,
    lramsync_cb_func_t rx_cb, void *rx_arg,
    lramsync_cb_func_t reset_signal_cb, void *reset_signal_arg
    );

int lramsync_start(lramsync_ctx_t *ctx);

int lramsync_reset(lramsync_ctx_t *ctx);
int lramsync_deinit(lramsync_ctx_t *ctx);
int lramsync_dump(lramsync_ctx_t *ctx);
int lramsync_get_info(lramsync_ctx_t *ctx, uint32_t *start_addr, uint32_t *curr_addr);

#endif



