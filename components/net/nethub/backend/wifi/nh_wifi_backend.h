#ifndef NH_WIFI_BACKEND_H
#define NH_WIFI_BACKEND_H

#include <stdbool.h>

#include "lwip/pbuf.h"

typedef enum {
    NH_WIFI_BACKEND_TX_OK = 0,
    NH_WIFI_BACKEND_TX_ERR_SEND = -1,
    NH_WIFI_BACKEND_TX_ERR_NETIF_DOWN = -2,
} nh_wifi_backend_tx_result_t;

int nh_wifi_backend_init(void);
nh_wifi_backend_tx_result_t nh_wifi_backend_tx(struct pbuf *p, bool is_sta);

/* Common bridge RX path shared by all Wi-Fi backend implementations. */
struct pbuf *nh_wifi_bridge_handle_rx(bool is_sta, struct pbuf *p);

#endif
