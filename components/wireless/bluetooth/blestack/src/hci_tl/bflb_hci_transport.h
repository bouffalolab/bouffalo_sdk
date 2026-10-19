#ifndef __BFLB_HCI_TRANSPORT_H
#define __BFLB_HCI_TRANSPORT_H

#include <stdint.h>
#include "buf.h"

/* Transport type identifiers */
typedef enum {
    BFLB_HCI_TRANSPORT_UART_INT = 0,
    BFLB_HCI_TRANSPORT_UART_DMA = 1,
} bflb_hci_transport_type_t;

/* Upward callbacks from transport to HCI upper layer */
typedef void (*bflb_hci_rx_handler_t)(struct net_buf *rx_buf);
typedef struct net_buf *(*bflb_hci_alloc_buf_t)(uint8_t rx_type, uint8_t sub_evt, uint16_t len);

typedef struct {
    bflb_hci_rx_handler_t  rx_handler;
    bflb_hci_alloc_buf_t   alloc_buf;
} bflb_hci_trans_callbacks_t;

/* HCI transport operations vtable */
struct bflb_hci_transport {
    const char *name;
    int32_t (*open)(void);
    int32_t (*close)(void);
    int32_t (*write)(const uint8_t *buf, uint32_t nbytes);
};

/**
 * Register a transport backend. Returns 0 on success.
 */
int bflb_hci_transport_register(bflb_hci_transport_type_t type,
                                const struct bflb_hci_transport *transport);

/**
 * Get the currently registered transport. Returns NULL if none.
 */
const struct bflb_hci_transport *bflb_hci_transport_get(void);

/**
 * Register upper-layer callbacks. Returns 0 on success.
 */
int bflb_hci_transport_register_cb(const bflb_hci_trans_callbacks_t *cb);

/**
 * Get the currently registered transport. Returns NULL if none.
 */
const struct bflb_hci_transport *bflb_hci_transport_get(void);

/**
 * Get the currently registered callbacks. Returns NULL if none.
 */
const bflb_hci_trans_callbacks_t *bflb_hci_transport_get_callbacks(void);

/**
 * Unregister transport and callbacks.
 */
void bflb_hci_transport_unregister(void);

#endif /* __BFLB_HCI_TRANSPORT_H */
