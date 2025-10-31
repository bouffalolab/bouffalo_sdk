#include "sdiowifi_tx_buf.h"

#include <stddef.h>
#include <utils_list.h>
#include <lwip/pbuf.h>
#include "sdiowifi_mgmr.h"
#include <platform_al.h>

#include "sdiowifi_platform_adapt.h"
#include "sdio_port.h"

#define PTR_SZ_MSK (sizeof(void *) - 1)
#define ALIGN_PTR_PADD(x) ((sizeof(void *) - ((uintptr_t)(x) & PTR_SZ_MSK)) & PTR_SZ_MSK)
#define ALIGN_PTR_HIGH(x) (((uintptr_t)(x) + PTR_SZ_MSK) & ~PTR_SZ_MSK)
#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#define TX_PBUF_FRAME_LEN   1514
#define TX_PBUF_PAYLOAD_LEN (PBUF_LINK_ENCAPSULATION_HLEN + TX_PBUF_FRAME_LEN)

#if 0
typedef struct {
    struct utils_list_hdr list_hdr;
    struct pbuf_custom pbuf;
    struct sdiowifi_tx_buf *tb;
    uint32_t payload_buf[(TX_PBUF_PAYLOAD_LEN + 3) / 4];
} tx_pbuf;
#endif

typedef struct {
    struct utils_list_hdr list_hdr;
    struct pbuf_custom pbuf;
    struct sdiowifi_tx_buf *tb;
    uint32_t *payload_buf;//[(TX_PBUF_PAYLOAD_LEN + 3) / 4];
} tx_pbuf_t;

typedef struct {
    uint32_t  txpbuf_payload[(TX_PBUF_PAYLOAD_LEN + 3) / 4];
} sdio_txbuf_payload_t;

#ifdef CONFIG_BOUFFALO_SDK
#include "bflb_core.h"
#if defined(SDIO_RX_MAX_PORT_NUM) && (SDIO_RX_MAX_PORT_NUM == 8)
#define SDIO_TX_NORMAL_BUFFER_COUNT (11)
#elif defined(SDIO_RX_MAX_PORT_NUM) && (SDIO_RX_MAX_PORT_NUM == 4)
#define SDIO_TX_NORMAL_BUFFER_COUNT (15)
#endif
sdio_txbuf_payload_t ATTR_NOCACHE_NOINIT_LPFW_SHARE __ALIGNED(32) g_txpbuf_payload_normal[SDIO_TX_NORMAL_BUFFER_COUNT];

#else
/* 38kb ram, sdio rx buffer used 16kb, left ram used as tx buffer,calculate: 11 = (38 * 1024 - 16 * 1024) / (((TX_PBUF_PAYLOAD_LEN + 3)/4) * 4) */
#if defined(SDIO_RX_MAX_PORT_NUM) && (SDIO_RX_MAX_PORT_NUM == 8)
#define SDIO_TX_NORMAL_BUFFER_COUNT (11)
#define TXBUF_PAYLOAD_START_ADDR    (0x2302A800)
#elif defined(SDIO_RX_MAX_PORT_NUM) && (SDIO_RX_MAX_PORT_NUM == 4)
#define SDIO_TX_NORMAL_BUFFER_COUNT (15)
#define TXBUF_PAYLOAD_START_ADDR    (0x23028800)
#endif

static sdio_txbuf_payload_t *g_txpbuf_payload_normal = (sdio_txbuf_payload_t *)ALIGN_PTR_HIGH(TXBUF_PAYLOAD_START_ADDR);
#endif

static tx_pbuf_t g_txpbuf_context[SDIO_TX_NORMAL_BUFFER_COUNT];

struct sdiowifi_tx_buf *s_tb_printf = NULL;

static tx_pbuf_t *tx_buffer_ctx = NULL;
int g_tx_buffer_cnt = 0;

void sdiowifi_txbuf_stats_display(void)
{
    struct sdiowifi_tx_buf *tb = s_tb_printf;
#ifdef CONFIG_LWIP_PBUF_CUSTOM_EXT_ENABLE
    int i,m;
#endif

    if (tb) {
        printf("[%ld] hostrouter current:%ld, una_seq:%ld-%d, free_size:%ld, window:%ld\r\n",
            sdiowifi_tick_now(0),
            (tb->una_seq + tb->window_size),
            tb->una_seq, tb->una_seq%256,
            tb->free_size,
            tb->window_size);
    }
#ifdef CONFIG_LWIP_PBUF_CUSTOM_EXT_ENABLE
    m = 0;
    for (i = 0; i < SDIO_TX_NORMAL_BUFFER_COUNT; i++) {
        printf("[%ld] hostrouter idex:%d, 0x%08lX\r\n",
            sdiowifi_tick_now(0),
            m, g_txpbuf_context[i].pbuf.pbuf.custom_flags);
        m++;
    }
    for (i = 0; i < (g_tx_buffer_cnt-SDIO_TX_NORMAL_BUFFER_COUNT); i++) {
        printf("[%ld] hostrouter idex:%d, 0x%08lX\r\n",
            sdiowifi_tick_now(0),
            m, tx_buffer_ctx[i].pbuf.pbuf.custom_flags);
        m++;
    }
#endif
}

int sdiowifi_tx_buf_init(struct sdiowifi_tx_buf *tb)
{
    if (!tb) {
        return -1;
    }

    s_tb_printf = tb;
    sdiowifi_enter_critical();
    utils_list_init(&tb->buf_list);
    tb->una_seq = 0;
    tb->free_size = 0;
    sdiowifi_exit_critical(0);

    return 0;
}

void sdiowifi_tx_buf_update_credit(struct sdiowifi_tx_buf *tb)
{
    HR_LOGD("update:%ld, una_seq:%ld-%d, free_size:%ld, window:%ld\r\n",
        (tb->una_seq + tb->window_size),
        tb->una_seq, tb->una_seq%256,
        tb->free_size,
        tb->window_size);
    sdiowifi_update_tx_credit(tb);
}

void sdiowifi_tx_buf_init_done(struct sdiowifi_tx_buf *tb)
{
}

int sdiowifi_tx_buf_lpmem_register(struct sdiowifi_tx_buf *tb)
{
    if (!tb) {
        return -1;
    }

    // register lpmem
    sdiowifi_enter_critical();
    // context.payload point payload
    for (int i = 0; i < SDIO_TX_NORMAL_BUFFER_COUNT; i++) {
        g_txpbuf_context[i].payload_buf = &(g_txpbuf_payload_normal[i]);
        g_txpbuf_context[i].tb = tb;
        // add
        utils_list_push_back(&tb->buf_list, (struct utils_list_hdr *)(&(g_txpbuf_context[i])));
        // ++free_size
        ++tb->free_size;
        HR_LOGD("lpmem_register ctx:%p, pldbuf:%p, free_size:%d\r\n",
                (struct utils_list_hdr *)(&(g_txpbuf_context[i])),
                g_txpbuf_context[i].payload_buf,
                tb->free_size);
    }

    g_tx_buffer_cnt += SDIO_TX_NORMAL_BUFFER_COUNT;
    // update window_size
    tb->window_size = tb->free_size;
    sdiowifi_exit_critical(0);

    HR_LOGI("lpmem_register tx slot:%ld,unseq:%ld-%d, free:%ld, window:%ld\r\n",
            (tb->una_seq + tb->window_size),
            tb->una_seq, tb->una_seq%256,
            tb->free_size,
            tb->window_size);

    sdiowifi_tx_buf_update_credit(tb);

    return 0;
}

int sdiowifi_tx_buf_mem_register(struct sdiowifi_tx_buf *tb, void *mem, size_t size)
{
    if (!tb) {
        return -1;
    }

    if (!mem) {
        return 0;
    }
    long s = size;
    uint8_t *p = (uint8_t *)ALIGN_PTR_HIGH(mem);
    s -= ALIGN_PTR_PADD(mem);
    sdiowifi_enter_critical();
    
    uint32_t tx_count = s / sizeof(sdio_txbuf_payload_t);
    if (0 == tx_count) {
        HR_LOGI("no enough mem for high performance\r\n");
        return -1;
    }
    tx_buffer_ctx = pvPortMalloc(tx_count * sizeof(tx_pbuf_t));
    if (!tx_buffer_ctx) {
        return -1;
    }
    g_tx_buffer_cnt += tx_count;
    memset(tx_buffer_ctx, 0, tx_count * sizeof(tx_pbuf_t));
    for (int i = 0; i < tx_count; i++) {
        tx_buffer_ctx[i].payload_buf = p + sizeof(sdio_txbuf_payload_t) * i; 
        tx_buffer_ctx[i].tb = tb;
        utils_list_push_back(&tb->buf_list, (struct utils_list_hdr *)(&(tx_buffer_ctx[i])));
        // ++free_size
        ++tb->free_size;
        HR_LOGD("mem_register ctx:%p, pldbuf:%p, free_size:%d\r\n",
                (struct utils_list_hdr *)(&(tx_buffer_ctx[i])),
                tx_buffer_ctx[i].payload_buf,
                tb->free_size);
    }
    
    // update window size.
    tb->window_size = tb->free_size;

    sdiowifi_exit_critical(0);

    HR_LOGI("mem_register:%p-%u, slot:%ld,unseq:%ld-%d, free:%ld, window:%ld\r\n",
            mem, size,
            (tb->una_seq + tb->window_size),
            tb->una_seq, tb->una_seq%256,
            tb->free_size,
            tb->window_size);

    sdiowifi_tx_buf_update_credit(tb);

    return 0;
}

static inline void notify_tx_buf_status(struct sdiowifi_tx_buf *tb)
{
//#ifdef SDIO_REUSE_LP_RAM
    uint8_t prevent = !sdiowifi_tx_buf_is_all_free(tb);
    PLATFORM_HOOK(prevent_sleep, PSM_EVENT_LP_BUF_REUSED, prevent);
//#endif
}

#ifdef CONFIG_LWIP_PBUF_CUSTOM_EXT_ENABLE
int net_if_buf_reset(void *buf)
{
    struct pbuf *p_buf = (struct pbuf *)buf;

    memset(&(p_buf->custom_flags), 0, sizeof(p_buf->custom_flags));
    return 0;
}
#endif
static void tx_buf_free(struct pbuf *p)
{
    tx_pbuf_t *tp = container_of((struct pbuf_custom *)p, tx_pbuf_t, pbuf);
    struct sdiowifi_tx_buf *tb = tp->tb;

    sdiowifi_enter_critical();
    {
#if CONFIG_LWIP_PBUF_CUSTOM_EXT_ENABLE
        int net_if_buf_reset(void *buf);
        net_if_buf_reset(p);
#endif
        utils_list_push_back(&tb->buf_list, &tp->list_hdr);
        ++tb->free_size;
        ++tb->una_seq;
        sdiowifi_tx_buf_update_credit(tb);
    }
    sdiowifi_exit_critical(0);
    //sdm_log("%s %u\r\n", __func__, tb->free_size);

    notify_tx_buf_status(tb);
}

struct pbuf *sdiowifi_tx_buf_alloc(struct sdiowifi_tx_buf *tb)
{
    tx_pbuf_t *tp;
    struct pbuf *p = NULL;

    sdiowifi_enter_critical();
    {
        tp = (tx_pbuf_t *)utils_list_pop_front(&tb->buf_list);
        if (tp) {
            --tb->free_size;
            //HR_LOGD("free:%d, total:%d\r\n", tb->free_size, tb->window_size);
        }
    }
    sdiowifi_exit_critical(0);

    if (tp) {
        tp->pbuf.custom_free_function = tx_buf_free;
#if 0
        p = pbuf_alloced_custom(PBUF_RAW_TX, TX_PBUF_FRAME_LEN, PBUF_RAM, &tp->pbuf, tp->payload_buf, TX_PBUF_PAYLOAD_LEN);
#else
        p = pbuf_alloced_custom(PBUF_RAW_TX, TX_PBUF_FRAME_LEN, 
                (PBUF_ALLOC_FLAG_DATA_CONTIGUOUS | PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP), 
                &tp->pbuf, tp->payload_buf, TX_PBUF_PAYLOAD_LEN);
#endif

    }
    //sdm_log("%s %u\r\n", __func__, tb->free_size);

    notify_tx_buf_status(tb);
    return p;
}

bool sdiowifi_tx_buf_is_all_free(struct sdiowifi_tx_buf *tb)
{
    sdiowifi_enter_critical();
    bool free = tb->window_size == tb->free_size;
    sdiowifi_exit_critical(0);
    return free;
}

