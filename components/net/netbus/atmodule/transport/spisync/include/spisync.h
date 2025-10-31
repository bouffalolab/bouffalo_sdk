#ifndef __SPISYNC_H__
#define __SPISYNC_H__

#include <stdint.h>
#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include "stream_buffer.h"
#include <event_groups.h>
#include <utils_list.h>

#include "spisync_config.h"
#include <ramsync.h>

/* ===========================================================================
 * User Configurable Settings
 * ========================================================================= */

/* slot config*/
#define SPISYNC_SLOT_VERSION        (0x5501)
#define SPISYNC_TXSLOT_COUNT        (4)//Set to 2 to reduce memory (also update host configuration)
#define SPISYNC_RXSLOT_COUNT        (4)//Set to 2 to reduce memory (also update host configuration)
#define SPISYNC_PAYLOADBUF_LEN      (1536)
#define SPISYNC_PAYLOADLEN_MAX      (SPISYNC_PAYLOADBUF_LEN+4)//seg_len_type_res,

/* Check the number of tag errors during interruptions. If the number of errors
   exceeds the allowed limit, the resetflag will set. */
#define SPISYNC_TAGERR_TIMES        (SPISYNC_RXSLOT_COUNT*2)
/* Check for a specific pattern during interruptions. If the pattern occurs more
   than a certain number of times, the hardware will reinitialize after a delay. */
#define SPISYNC_PATTERN_TIMES       (SPISYNC_TXSLOT_COUNT*5)

/* Config msg/stream size */
#define SPISYNC_RXMSG_PBUF_ITEMS       (3)//Set to 2 to reduce memory (also update host configuration)
#define SPISYNC_RXMSG_SYSCTRL_ITEMS    (2)//Set to 2 to reduce memory (also update host configuration)
#define SPISYNC_RXMSG_USER1_ITEMS      (1)//Set to 2 to reduce memory (also update host configuration)
#define SPISYNC_RXSTREAM_AT_BYTES      (1536*6)//Set to 8 to reduce memory (also update host configuration)
#define SPISYNC_RXSTREAM_USER1_BYTES   (8)//Set to 8 to reduce memory (also update host configuration)
#define SPISYNC_RXSTREAM_USER2_BYTES   (8)//Set to 8 to reduce memory (also update host configuration)
#define SPISYNC_TXMSG_PBUF_ITEMS       (11)//Set to 2 to reduce memory (also update host configuration)
#define SPISYNC_TXMSG_SYSCTRL_ITEMS    (2)//Set to 2 to reduce memory (also update host configuration)
#define SPISYNC_TXMSG_USER1_ITEMS      (2)//Set to 2 to reduce memory (also update host configuration)
#define SPISYNC_TXSTREAM_AT_BYTES      (1536*6)//Set to 8 to reduce memory (also update host configuration)
#define SPISYNC_TXSTREAM_USER1_BYTES   (8)//Set to 8 to reduce memory (also update host configuration)
#define SPISYNC_TXSTREAM_USER2_BYTES   (8)//Set to 8 to reduce memory (also update host configuration)

/* task config */
#define SPISYNC_STACK_SIZE             (1024*3)
#define SPISYNC_TASK_PRI               (28)

/* feature config */
#define SPISYNC_TXCRC_ENABLE           (1) // set 0 if disable slave--->host crc
#define SPISYNC_RXCRC_ENABLE           (1) // set 0 if disable host--->slave crc
#define SPISYNC_TXCRC32_ENABLE         (0) // 0-checksum 1-crc32
#define SPISYNC_RXCRC32_ENABLE         (0) // 0-checksum 1-crc32
#define SPISYNC_SAVE2CACHE_WHENRESET   (0) // 0-no sav2cache when reset, 1-save2cache

/* ========================================================================= */

/* For system wait */
#define SPISYNC_WAIT_FOREVER           (0xFFFFFFFF)
#define SPISYNC_RX_READTIMES           (SPISYNC_RXSLOT_COUNT-1)

/* For Tag flag */
#define SPISYNC_TAGFLAGBIT_RXERR       (0x0001) // bit0 indicates whether the slave device has an error

/* For MSG flag */
#define SPISYNC_MSGFLAGBIT_COPY        (0x01) // bit0 indicates whether to reallocate memory and copy the incoming msg->buf

/* /Msg/Stream type */
#define SPISYNC_TYPEMSG_PBUF           (0)
#define SPISYNC_TYPEMSG_SYSCTRL        (1)
#define SPISYNC_TYPEMSG_USER1          (2)
#define SPISYNC_TYPESTREAM_AT          (3)
#define SPISYNC_TYPESTREAM_USER1       (4)
#define SPISYNC_TYPESTREAM_USER2       (5)
#define SPISYNC_TYPEMAX                (6)

/*
 * clock_10M--->1KBytes/ms
 * clock_20M--->2KBytes/ms
 * clock_40M--->4KBytes/ms
 */
#define SPISYNC_PATTERN_DELAYMS        (100)

//#pragma pack(push, 1)
/* slot.tag */
typedef struct __slot_tag {
    uint16_t version;
    uint16_t seq;
    uint16_t ackseq;
    /*
     * bit0: 1-bus_error
     */
    uint16_t flag;
    uint32_t clamp[6];
    uint8_t  resv[2];
    uint16_t crc;
} slot_tag_t;

/* slot.payload.seg */
typedef struct __payload {
    uint16_t len;
    uint8_t  type;
    uint8_t  resv[1];
    uint8_t  buf[1];
} slot_seg_t;

/* slot.payload */
typedef struct __slot_payload {
    uint16_t tot_len;
    uint8_t  res[2];
    union {
        uint8_t     raw[SPISYNC_PAYLOADBUF_LEN + 4];
        slot_seg_t  seg[1];
    };
    uint32_t crc;
} slot_payload_t;

/* slot.tail */
typedef struct __slot_tail {
    uint16_t    seq_mirror;
    uint8_t     reserved[2];
} slot_tail_t;

/* slot */
typedef struct __spisync_slot {
    slot_tag_t      tag;
    slot_payload_t  payload;
    slot_tail_t     tail;
} spisync_slot_t;
//#pragma pack(pop)

/* tx/rx buf */
typedef struct __spisync_txbuf {
    spisync_slot_t slot[SPISYNC_TXSLOT_COUNT];
} spisync_txbuf_t;
typedef struct __spisync_rxbuf {
    spisync_slot_t slot[SPISYNC_RXSLOT_COUNT];
} spisync_rxbuf_t;

/* writ or read arg*/
typedef struct __spisync_msg {
    uint8_t type;      // for spisync

    //uint32_t flags;     // [resv]
    union {
        uint32_t flags;   // Keep flags as 32 bits
        struct {
            uint32_t copy       : 1;   // bit0 indicates if copy is needed
            uint32_t reserved   : 31;  // Remaining 31 bits reserved
        };
    };

    uint32_t timeout;   // rtos send/recv timeout

    uint8_t  *buf;      // real buf addr
    uint32_t buf_len;   // read buf length
    // void *start;        // [resv] for zero copy
    // void *end;          // [resv] for zero copy

    void *(*cb)(void *arg);  // pbuf_free or complete used
    void *cb_arg;            // pbuf_free or complete used
} spisync_msg_t;

typedef struct __spisync {
    /* sys hdl */
    TaskHandle_t            taskhdr;
    EventGroupHandle_t      event;       // EVT wait
    TimerHandle_t           timer;       // Only for delay to reset
    SemaphoreHandle_t       write_lock;  // spisync_write lock
    SemaphoreHandle_t       opspin_log;  // spisync_write lock
    SemaphoreHandle_t       updateclamp; // updateclamp

    QueueHandle_t           prx_queue[3];
    StreamBufferHandle_t    prx_streambuffer[3];
    QueueHandle_t           ptx_queue[3];
    StreamBufferHandle_t    ptx_streambuffer[3];

    /* local global arg */
    uint16_t                tx_seq;
    uint32_t                rxtag_errtimes;
    uint32_t                rxpattern_checked;

    uint32_t                ps_status;     // 0:active,    1:sleeping
    uint32_t                ps_gpioactive; // 0:can sleep, 1:cant't sleep
    uint32_t                ps_keepactive; // 0:can sleep, 1:cant't sleep

    uint32_t                clamp[6];   // local for tx // local-reicved  // s->m flowctrl

    /* local cb */
    spisync_ops_t           sync_ops[6]; // for spisync zerocopy, eg:fhost_tx

    /* slot */
    /* Because the cache is aligned to 32 bytes, both the beginning and end
       need to consider cache alignment issues.
     */
    spisync_txbuf_t*        p_tx;
    spisync_rxbuf_t*        p_rx;
    /* calulate crc for rx */
    spisync_slot_t*         p_rxcache;
    /* To avoid tx_slot loss due to reset  */
    struct utils_list       txcache_list;
    uint16_t                rx_ackseq_fortxcache;

    /* ramsync */
    lramsync_ctx_t          hw;
	/* The config is only cleared to 0 during init, unchanged during reset */
    const spisync_hw_t      *config;

    void (*reset_cb)(void *arg);
    void *reset_arg;

    /* debug */
    uint32_t                isr_rxcnt;
    uint32_t                isr_txcnt;
    uint32_t                rxcrcerr_cnt;
    uint32_t                tsk_rstcnt;
    uint32_t                tsk_writecnt;
    uint32_t                tsk_readcnt;
    uint32_t                dbg_write_slotblock;
    uint32_t                dbg_write_clampblock[6];
    uint32_t                dbg_read_flowblock[6];
    void *gpio;
} spisync_t;
typedef struct __spisync_wakeuparg {
    int wakeup_reason;// 0-gpio, 1-task, 2-timer, 3-beacon, 4-dtim...
} spisync_wakeuparg_t;

#if 0
#define SPISYNC_MSGINIT(msg,type,copy,timeout,buf,len,cb,cb_arg) {  \
                            memset(&msg, 0, sizeof(spisync_msg_t)); \
                            msg.type       = type;                  \
                            msg.copy       = copy;                  \
                            msg.timeout    = timeout;               \
                            msg.buf        = buf;                   \
                            msg.buf_len    = len;                   \
                            msg.cb         = cb;                    \
                            msg.cb_arg     = cb_arg;                \
                        }
#else
static inline int SPISYNC_MSGINIT(spisync_msg_t *pmsg,
                            uint8_t type, uint32_t copy,
                            uint32_t timeout,
                            uint8_t *buf, uint32_t len,
                            void *(*cb)(void *arg),
                            void *cb_arg)
{
    memset(pmsg, 0, sizeof(spisync_msg_t));
    pmsg->type       = type;
    pmsg->copy       = copy;
    pmsg->timeout    = timeout;
    pmsg->buf        = buf;
    pmsg->buf_len    = len;
    pmsg->cb         = cb;
    pmsg->cb_arg     = cb_arg;

    return 0;
}
#endif

/* base api */
int spisync_init        (spisync_t *spisync, const spisync_config_t *config);
int spisync_read        (spisync_t *spisync, spisync_msg_t *msg, uint32_t flags);
int spisync_write       (spisync_t *spisync, spisync_msg_t *msg, uint32_t flags);

/* powersaving api */
int spisync_ps_enter    (spisync_t *spisync);
int spisync_ps_exit     (spisync_t *spisync);
int spisync_ps_wakeup   (spisync_t *spisync, spisync_wakeuparg_t *arg);
int spisync_ps_get      (spisync_t *spisync);//0-idle 1-busy

/* debug api */
int spisync_status              (spisync_t *spisync);
int spisync_status_internal     (spisync_t *spisync);
int spisync_fakewrite_forread   (spisync_t *spisync, spisync_msg_t *msg, uint32_t flags);

/* todo */
int spisync_set_clamp(spisync_t *spisync, uint8_t type, uint32_t cnt);

#endif

