#ifndef __SPISYNC_HW_H__
#define __SPISYNC_HW_H__

#if SPISYNC_MASTER_ENABLE
#else
#include "bflb_gpio.h"
#endif

typedef struct _spisync_hw {
    const char *spi_name;
    const char *spi_tx_dmach;
    const char *spi_rx_dmach;

    uint8_t  spi_port;
    uint8_t  spi_mode;
    uint8_t  spi_3pin_mode;
    uint32_t spi_speed;
    uint8_t  data_rdy_pin;

    uint32_t spi_dma_req_tx;
    uint32_t spi_dma_req_rx;
    uint32_t spi_dma_tdr;
    uint32_t spi_dma_rdr;
} spisync_hw_t;

/* global hdr */
typedef struct __spisync_opscb_arg {
    uint8_t *buf;
    uint16_t buf_len;
    uint32_t flag;      // resv
} spisync_opscb_arg_t;
typedef void (*spisync_opscb_cb_t)(void *pri, spisync_opscb_arg_t *msg);
typedef struct spisync_ops_t {
    spisync_opscb_cb_t  cb;    // pbuf_free or complete used
    void                *cb_pri;// pbuf_free or complete used
    uint8_t type;
} spisync_ops_t;
typedef struct _spisync_config {
    const spisync_hw_t      *hw;
    const spisync_ops_t     ops[6];
    void (*reset_cb)(void *arg);
    void *reset_arg;
} spisync_config_t;

#endif
