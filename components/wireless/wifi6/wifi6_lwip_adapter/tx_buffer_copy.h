#ifndef __TX_BUFFER_COPY__
#define __TX_BUFFER_COPY__

#include "stdint.h"
#include "net_al.h"
#define TX_BUF_FRAME_LEN   1514
#define TX_BUF_PAYLOAD_LEN (PBUF_LINK_ENCAPSULATION_HLEN + TX_BUF_FRAME_LEN)

/// structure of a list element header
struct bc_list_hdr
{
    /// Pointer to the next element in the list
    struct bc_list_hdr *next;
};

/// structure of a list
struct bc_list
{
    /// pointer to first element of the list
    struct bc_list_hdr *first;
    /// pointer to the last element
    struct bc_list_hdr *last;
};

struct bc_tx_buf_tag
{
    struct bc_list_hdr hdr; // Must be first
    struct pbuf_custom c_pbuf;
    uint32_t payload_buf[(TX_BUF_PAYLOAD_LEN + 3) / 4];
};
struct bc_tx_info_tag
{
    struct bc_list_hdr hdr;
    struct net_al_tx_req req;
};
struct bc_tx_buf_tag *bc_tx_buf_alloc();
void bc_tx_init_tx_pool();
void bc_tx_queue_release_sta(uint8_t sta_id, release_buf_cb release_buf);
void bc_try_tx_queued_pkts();
void bc_tx_enqueue(struct bc_tx_info_tag *info, uint8_t std_id, struct net_al_tx_req req);

#endif
