#ifndef __HCI_H4_H
#define __HCI_H4_H

#include <stdint.h>
#include "buf.h"

/* H4 packet type indicators */
#define H4_IND_COMMAND   0x01
#define H4_IND_ACL_DATA  0x02
#define H4_IND_EVENT     0x04

/* Little-endian to host 16-bit conversion */
#define H4_LE16(ptr) ((uint16_t)( ((uint16_t)*((uint8_t *)(ptr) + 1) << 8) | \
                                   ((uint16_t)*((uint8_t *)(ptr))) ))

/* H4 parser receive states */
typedef enum {
    H4_STATE_TYPE   = 0,
    H4_STATE_HEADER = 1,
    H4_STATE_DATA   = 2,
} h4_state_t;

/* Upward callbacks from H4 parser to HCI upper layer */
typedef void (*h4_rx_handler_t)(struct net_buf *rx_buf);
typedef struct net_buf *(*h4_alloc_buf_t)(uint8_t rx_type, uint8_t sub_evt, uint16_t len);

typedef struct {
    h4_rx_handler_t   rx_handler;
    h4_alloc_buf_t    alloc_buf;
} h4_callbacks_t;

/* H4 parser context. One instance per transport channel. */
typedef struct {
    h4_state_t     state;
    uint8_t        pkt_type;
    uint8_t        header[4];
    uint8_t        header_len;
    uint8_t        header_recv;
    uint16_t       data_len;
    struct net_buf *rx_buf;
    uint8_t       *data_ptr;
    uint16_t       data_remaining;
} h4_context_t;

/**
 * Initialize H4 parser context. Call once before first use.
 */
void h4_init(h4_context_t *ctx);

/**
 * Feed received bytes into the H4 parser.
 *
 * @param ctx       H4 parser context
 * @param data      Pointer to received bytes
 * @param len       Number of bytes available
 * @param cb        Upper-layer callbacks
 * @return Number of bytes consumed. Caller should retry remaining bytes
 *         if return value < len.
 */
uint16_t h4_recv(h4_context_t *ctx, const uint8_t *data, uint16_t len,
                 const h4_callbacks_t *cb);

#endif /* __HCI_H4_H */
