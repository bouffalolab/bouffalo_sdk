#include "stddef.h"
#include "assert.h"
#include "mac_frame.h"
#include "net_al.h"
#include "lwip/tcpip.h"
#include "tx_buffer_copy.h"

#define bc_dbg(fmt, ...) do{}while(0)

#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif //end container_of

void bc_list_init(struct bc_list *list)
{
    list->first = NULL;
    list->last = NULL;
}

static bool bc_list_is_empty(const struct bc_list *const list)
{
    bool listempty;
    listempty = (list->first == NULL);
    return (listempty);
}

void bc_list_push_back(struct bc_list *list,
                       struct bc_list_hdr *list_hdr)
{
    // Sanity check
    assert(list_hdr != NULL);

    // check if list is empty
    if (bc_list_is_empty(list))
    {
        // list empty => pushed element is also head
        list->first = list_hdr;
    }
    else
    {
        // list not empty => update next of last
        list->last->next = list_hdr;
    }

    // add element at the end of the list
    list->last = list_hdr;
    list_hdr->next = NULL;
}

static struct bc_list_hdr *bc_list_pop_front(struct bc_list *list)
{
    struct bc_list_hdr *element;

    // check if list is empty
    element = list->first;
    if (element != NULL)
    {
        // The list isn't empty : extract the first element
        list->first = list->first->next;
        if (list->last == element)
            list->last = NULL;
    }

    return element;
}


struct bc_tx_queue
{
    struct bc_list pending_tx_list;
    ssize_t pkt_queued;
};

#define BC_QUEUE_LEN (CFG_STA_MAX + CFG_VIF_MAX)
struct bc_tx_buf_pool
{
    size_t cnt_total;
    ssize_t cnt_free;
    struct bc_list free_list;
    ssize_t pkt_queued;
    struct bc_tx_queue queue[BC_QUEUE_LEN];
};

struct bc_tx_buf_pool tx_buf_pool;

void bc_tx_init_tx_pool()
{
    size_t i;
    struct bc_tx_buf_pool *pool = &tx_buf_pool;
    struct bc_tx_buf_tag *buf;
    size_t len;
    extern uint8_t _heap_wifi_start;
    extern uint8_t _heap_wifi_size;

    memset(pool, 0, sizeof(*pool));
    bc_list_init(&pool->free_list);
    for (i = 0; i < BC_QUEUE_LEN; ++i) {
        struct bc_tx_queue *queue = &pool->queue[i];
        bc_list_init(&queue->pending_tx_list);
    }

    buf = (struct bc_tx_buf_tag *)CO_ALIGN4_HI((uint32_t)&_heap_wifi_start);
    len = (size_t)&_heap_wifi_size;
    while (len >= sizeof(*buf)) {
        bc_list_push_back(&pool->free_list, (struct bc_list_hdr *)buf);
        ++pool->cnt_total;
        ++pool->cnt_free;
        len -= sizeof(*buf);
        ++buf;
    }
    bc_dbg("TX buf total %lu\r\n", pool->cnt_total);
}

static void bc_tx_buf_free(struct pbuf *p)
{
    struct bc_tx_buf_pool *pool = &tx_buf_pool;
    struct bc_tx_buf_tag *buf = container_of((struct pbuf_custom *)p, struct bc_tx_buf_tag, c_pbuf);

    bc_list_push_back(&pool->free_list, (struct bc_list_hdr *)buf);
    ++pool->cnt_free;
    ASSERT_ERR(pool->cnt_free <= pool->cnt_total);
}

void bc_try_tx_queued_pkts(void)
{
    struct bc_tx_buf_pool *pool = &tx_buf_pool;
    struct bc_tx_info_tag *info;

    // if any queued pkts
    while (pool->pkt_queued) {
        if (pool->cnt_free == 0)
            break;
        // TODO change scheduling algorithm
        // TODO faster
        for (size_t i = 0; i < BC_QUEUE_LEN; ++i) {
            struct bc_tx_queue *queue = &pool->queue[i];
            if (queue->pkt_queued == 0)
                continue;

            info = (struct bc_tx_info_tag *)bc_list_pop_front(&queue->pending_tx_list);
            --queue->pkt_queued;
            --pool->pkt_queued;
            bc_dbg("deQ, cnt %u", pool->pkt_queued);
            net_al_tx_req(info->req);
            break;
        }
    }
}

struct bc_tx_buf_tag *bc_tx_buf_alloc()
{
    struct bc_tx_buf_pool *pool = &tx_buf_pool;
    struct bc_tx_buf_tag *buf;

    buf = (struct bc_tx_buf_tag *)bc_list_pop_front(&pool->free_list);

    if (buf) {
        --pool->cnt_free;
        ASSERT_ERR(pool->cnt_free >= 0);
        buf->c_pbuf.custom_free_function = bc_tx_buf_free;
        pbuf_alloced_custom(PBUF_RAW_TX, TX_BUF_FRAME_LEN, PBUF_RAM, &buf->c_pbuf, buf->payload_buf, TX_BUF_PAYLOAD_LEN);
    }
    return buf;
}

void bc_tx_queue_release_sta(uint8_t sta_id, release_buf_cb release_buf)
{
    struct bc_tx_buf_pool *pool = &tx_buf_pool;
    struct bc_tx_queue *queue;

    ASSERT_ERR(sta_id < BC_QUEUE_LEN);
    queue = &pool->queue[sta_id];

    while (queue->pkt_queued) {
        struct bc_tx_info_tag *info;
        info = (struct bc_tx_info_tag *)bc_list_pop_front(&queue->pending_tx_list);
        --queue->pkt_queued;
        --pool->pkt_queued;
        release_buf(info->req.net_buf, 0, info->req.cfm_cb, info->req.cfm_cb_arg, info->req.buf_rx);
    }
}

void bc_tx_enqueue(struct bc_tx_info_tag *info, uint8_t sta_id, struct net_al_tx_req req)
{
        struct bc_tx_buf_pool *pool = &tx_buf_pool;
        struct bc_tx_queue *queue;

        queue = &pool->queue[sta_id];

        info->req = req;
        bc_list_push_back(&queue->pending_tx_list, &info->hdr);
        ++queue->pkt_queued;
        ++pool->pkt_queued;
        bc_dbg("STA %u enQ pkt, cnt %u", staid, pool->pkt_queued);
}
