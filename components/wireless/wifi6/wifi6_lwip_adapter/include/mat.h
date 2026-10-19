#ifndef __MAT_H
#define __MAT_H

/* MAC Address Translation. */
#include <stdint.h>
#include "lwip/ip_addr.h"

#define MAT_ERR_OK      0
#define MAT_ERR_INVAL   -1
#define MAT_ERR_STATUS  -2
#define MAT_ERR_MEM     -3
#define MAT_ERR_DATA    -4
#define MAT_ERR_PROTO   -5

struct mat_tuple {
    uint8_t hwaddr[6];
    uint8_t used;
    /* Timestamp when this tuple is added or updated. */
    uint32_t ts;
    ip_addr_t ipaddr;
};

struct netif;
struct pbuf;

int mat_tuple_add(uint8_t *hwaddr, ip_addr_t *ip);

int mat_tuple_del(uint8_t *hwaddr, ip_addr_t *ip);

int mat_handle_egress(struct netif *netif, struct pbuf *pbuf, struct pbuf **out);

int mat_handle_ingress(struct netif *netif, struct pbuf *pbuf);

struct mat_tuple *mat_tuple_find(uint8_t *hwaddr, ip_addr_t *ip);

/* __MAT_H */
#endif
