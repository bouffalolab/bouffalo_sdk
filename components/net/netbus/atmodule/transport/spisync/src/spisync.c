#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include "stream_buffer.h"
#include <event_groups.h>

#include <utils_crc.h>
#include <spisync.h>
#include <spisync_config.h>
#include <ramsync.h>
#include <spisync_port.h>
#include <spisync_internal.h>

#include <shell.h>

spisync_t *g_spisync_current = NULL;

/* hardcode */
#define D_STREAM_OFFSET             (3)
#define D_QUEUE_OFFSET              (0)
#define PAYLOADBUF0_SEG_OFFSET      (4)

void spisync_s2m_init(int pin)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    g_spisync_current->gpio = gpio;
    bflb_gpio_init(g_spisync_current->gpio, pin, GPIO_OUTPUT | GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);

    bflb_gpio_set(g_spisync_current->gpio, pin);

    if (g_spisync_current) {
        g_spisync_current->ps_gpioactive = 1;
    }
}
void spisyn_s2m_set(int pin)
{
    bflb_gpio_set(g_spisync_current->gpio, pin);
    if (g_spisync_current) {
        g_spisync_current->ps_gpioactive = 1;
    }

    spisync_trace("spisyn_s2m_set\r\n");
}
void spisync_s2m_reset(int pin)
{
    bflb_gpio_reset(g_spisync_current->gpio, pin);
    if (g_spisync_current) {
        g_spisync_current->ps_gpioactive = 0;
    }

    spisync_trace("spisyn_s2m_reset\r\n");
}
bool spisync_s2m_get(int pin)
{
    return bflb_gpio_read(g_spisync_current->gpio, pin);
}

static int _is_streamtype(uint8_t type)
{
    if ((3 == type) || (4 == type) || (5 == type)) {
        return 1;
    }

    return 0;
}
static int _is_msgtype(uint8_t type)
{
    if ((0 == type) || (1 == type) || (2 == type)) {
        return 1;
    }

    return 0;
}

static void spisync_task(void *arg);
void spisync_setevent_internal(spisync_t *spisync, uint32_t evt)
{
    EventBits_t res;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (xPortIsInsideInterrupt()) {
        res = xEventGroupSetBitsFromISR(spisync->event, evt, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    } else {
        res = xEventGroupSetBits(spisync->event, evt);
    }
    spisync_trace("int spisync:%p, evt:%08lx, res:%08lx\r\n", spisync, evt, res);
}
static void _spisync_pattern_process(spisync_t *spisync)
{
    int res;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (xPortIsInsideInterrupt()) {
        if (xTimerStartFromISR(spisync->timer, &xHigherPriorityTaskWoken) != pdPASS) {
            printf("Failed to start timer!\r\n");
        }
        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    } else {
        res = xTimerStart(spisync->timer, 1000);
        if (res != pdPASS) {
            printf("Failed to start timer!\r\n");
        }
    }
}

int _spisync_reset_clamp(spisync_t *spisync)
{
    // FIXME: hardcode 11.
    for (int i = 0; i < SPISYNC_TXSLOT_COUNT; i++) {
#ifdef CONFIG_SPISYNC_NETUSETYPE1_ENABLE
        spisync->p_tx->slot[i].tag.clamp[0] = SPISYNC_RXMSG_PBUF_ITEMS;
#endif
        spisync->p_tx->slot[i].tag.clamp[1] = SPISYNC_RXMSG_SYSCTRL_ITEMS;
        spisync->p_tx->slot[i].tag.clamp[2] = SPISYNC_RXMSG_USER1_ITEMS;
        spisync->p_tx->slot[i].tag.clamp[3] = SPISYNC_RXSTREAM_AT_BYTES;
        spisync->p_tx->slot[i].tag.clamp[4] = SPISYNC_RXSTREAM_USER1_BYTES;
        spisync->p_tx->slot[i].tag.clamp[5] = SPISYNC_RXSTREAM_USER2_BYTES;
    }

    return 0;
}
int _spisync_update_clamp(spisync_t *spisync, uint8_t type, uint16_t cnt)
{
    spisync_trace("-------- update clamp type:%d, cnt:%d\r\n", type, cnt);

    for (int i = 0; i < SPISYNC_TXSLOT_COUNT; i++) {
        if (_is_streamtype(type)) {
            spisync->p_tx->slot[i].tag.clamp[type] += cnt;
        } else if (_is_msgtype(type)) {
            spisync->p_tx->slot[i].tag.clamp[type] += cnt;
        } else {
            return -1;
        }
    }

    return 0;
}
int spisync_set_clamp(spisync_t *spisync, uint8_t type, uint32_t cnt)
{
    spisync_trace("-------- update clamp type:%d, cnt:%d\r\n", type, cnt);

    for (int i = 0; i < SPISYNC_TXSLOT_COUNT; i++) {
        if (_is_streamtype(type)) {
            spisync->p_tx->slot[i].tag.clamp[type] = cnt;
        } else if (_is_msgtype(type)) {
            spisync->p_tx->slot[i].tag.clamp[type] = cnt;
        } else {
            return -1;
        }
    }

    return 0;
}
int _spisync_reset_localvar(spisync_t *spisync, char init)
{
    /* global val */
    spisync->tx_seq             = 0;
    spisync->rxtag_errtimes     = 0;
    spisync->rxpattern_checked  = 0;

    for (int i = 0; i < 6; i++) {
        spisync->clamp[i]       = 0;
    }

    /* global debug */
    if (init) {
        spisync->isr_rxcnt        = 0;
        spisync->isr_txcnt        = 0;
    }
    spisync->rxcrcerr_cnt     = 0;
    if (init) {
        spisync->tsk_rstcnt   = 0;
    }
    spisync->tsk_writecnt     = 0;
    spisync->tsk_readcnt      = 0;

    spisync->dbg_write_slotblock         = 0;
    for (int i = 0; i < 6; i++) {
        spisync->dbg_write_clampblock[i] = 0;
        spisync->dbg_read_flowblock[i]   = 0;
    }

    /* global val */
    if (init) {
        spisync->ps_status      = 0;
        spisync->ps_keepactive  = 0;
        spisync->ps_gpioactive  = 0;
    }

    return 0;
}
static int _spisync_is_active(spisync_t *spisync)
{
    /* set version */
    for (int i = 0; i < SPISYNC_TXSLOT_COUNT; i++) {
        if (spisync->p_tx->slot[i].tag.flag & SPISYNC_TAGFLAGBIT_RXERR != 0) {
            return 0;
        }
    }
    return 1;
}

static int _spisync_reset_tag(spisync_t *spisync, char init)
{
    /* set version */
    for (int i = 0; i < SPISYNC_TXSLOT_COUNT; i++) {
        spisync->p_tx->slot[i].tag.version = SPISYNC_SLOT_VERSION;
        if (init) {
            spisync->p_tx->slot[i].tag.flag |= SPISYNC_TAGFLAGBIT_RXERR;
        }
    }

    return 0;
}
static int _spisync_reset_queuestream(spisync_t *spisync)
{
    spisync_trace("_spisync_reset_queuestream start\r\n");
    /* Reset Queue */
    for (int i = 0; i < 3; i++) {
        while (uxQueueMessagesWaiting(spisync->prx_queue[i]) > 0) {
            spisync_msg_t msg;
            int res;
            res = xQueueReceive(spisync->prx_queue[i], &msg, 0);
            if ((res == pdTRUE) && (msg.cb)) {
                // free all queue msg.
                msg.cb(msg.cb_arg);
            } else {
                // This point should not be reached under normal circumstances
                spisync_err("This point should not be reached under normal circumstances.\r\n");
            }
        }
    }
#if 0
    for (int i = 0; i < 3; i++) {
        while (uxQueueMessagesWaiting(spisync->ptx_queue[i]) > 0) {
            spisync_msg_t msg;
            int res;
            res = xQueueReceive(spisync->ptx_queue[i], &msg, 0);
            if ((res == pdTRUE) && (msg.cb)) {
                // free all queue msg.
                msg.cb(msg.cb_arg);
            } else {
                // This point should not be reached under normal circumstances
                spisync_err("This point should not be reached under normal circumstances.\r\n");
            }
        }
    }
#endif

    /* Reset Stream */
    xStreamBufferReset(spisync->prx_streambuffer[0]);
    xStreamBufferReset(spisync->prx_streambuffer[1]);
    xStreamBufferReset(spisync->prx_streambuffer[2]);
#if 0
    xStreamBufferReset(spisync->ptx_streambuffer[0]);
    xStreamBufferReset(spisync->ptx_streambuffer[1]);
    xStreamBufferReset(spisync->ptx_streambuffer[2]);
#endif

    spisync_trace("_spisync_reset_queuestream end\r\n");

    return 0;
}

static uint16_t get_latest_ackseq(spisync_t *spisync);
#if SPISYNC_SAVE2CACHE_WHENRESET
static int _spisync_reset_txcache_list(spisync_t *spisync)
{
    uint32_t lentmp;
    uint32_t crctmp;
    uint16_t seqtmp;
    uint8_t pack_type;
    struct crc32_stream_ctx crc_ctx;
    spisync_slot_t *node;

    uint16_t latest_txseq, latest_txseqack;
    int16_t diff;
    uint8_t slot_needsave[SPISYNC_TXSLOT_COUNT];

    int i, m, n;
    int curr_slot = -1;

    memset(slot_needsave, 0, sizeof(slot_needsave));
    if (utils_list_is_empty(&spisync->txcache_list)) {
        latest_txseq = spisync->tx_seq;
        latest_txseqack = spisync->rx_ackseq_fortxcache;//get_latest_ackseq(spisync);
        spisync_log("txseq:%d, txseq_ack:%d\r\n", latest_txseq, latest_txseqack);
        if (latest_txseq == latest_txseqack) {
            return 0;
        }

        for (i = 0; i < SPISYNC_TXSLOT_COUNT; i++) {
            // check version
            if (SPISYNC_SLOT_VERSION != spisync->p_tx->slot[i].tag.version) {
                spisync_log("reset txcache i:%d, version error\r\n", i);
                continue;
            }

            // check seq
            if (spisync->p_tx->slot[i].tag.seq != spisync->p_tx->slot[i].tail.seq_mirror) {
                spisync_log("reset txcache i:%d, seq error\r\n", i);
                continue;
            }
            diff = SEQ_GT(spisync->p_tx->slot[i].tag.seq, latest_txseqack);
            if ((diff <= 0) || (diff > SPISYNC_TXSLOT_COUNT)) {
                spisync_log("reset txcache i:%d, drop it, seq:%d, ack_seq:%d\r\n",
                        i, spisync->p_tx->slot[i].tag.seq, latest_txseqack);
                continue;
            }
            spisync_log("reset txcache i:%d, save it, seq:%d, ack_seq:%d\r\n",
                    i, spisync->p_tx->slot[i].tag.seq, latest_txseqack);

            // check crc
#if SPISYNC_TXCRC_ENABLE
#if SPISYNC_TXCRC32_ENABLE
            utils_crc32_stream_init(&crc_ctx);
            utils_crc32_stream_feed_block(&crc_ctx,
                                  (const uint8_t *)&spisync->p_tx->slot[i].payload.tot_len,
                                  spisync->p_tx->slot[i].payload.tot_len + 4);
            crctmp = utils_crc32_stream_results(&crc_ctx);
#else
            crctmp = spisync_checksum( (const uint8_t *)&spisync->p_tx->slot[i].payload.tot_len,
                                  spisync->p_tx->slot[i].payload.tot_len + 4);
#endif
            if (spisync->p_tx->slot[i].payload.crc != crctmp) {
                spisync_log("reset txcache i:%d, crc err, seq:%d, ack_seq:%d, cal=0x%08X != 0x%08X\r\n",
                        i, spisync->p_tx->slot[i].tag.seq, latest_txseqack, crctmp, spisync->p_tx->slot[i].payload.crc);
                continue;
            }
#endif
            slot_needsave[i] = 1;
            spisync_trace("find valid slot:%d\r\n", i);
        }

        for (m = 0; m < SPISYNC_TXSLOT_COUNT; m++) {
            int curr_slot = -1;
            // get min
            for (n = 0; n < SPISYNC_TXSLOT_COUNT; n++) {
                if (slot_needsave[n] == 1) {
                    if (curr_slot < 0) {
                        curr_slot = n;
                    } else {
                        if (SEQ_LT(spisync->p_tx->slot[n].tag.seq, spisync->p_tx->slot[curr_slot].tag.seq)) {
                            curr_slot = n;
                        }
                    }
                }
            }
            if (curr_slot == -1) {
                break;
            }
            // push to list
            node = malloc(sizeof(spisync_slot_t));
            if (node) {
                memcpy(node, &(spisync->p_tx->slot[curr_slot]), sizeof(spisync_slot_t));
                utils_list_push_back(&spisync->txcache_list, (struct utils_list_hdr *)node);
            }
            spisync_log("====== malloc node:%p\r\n", node);
            spisync_log("reset txcache curr_slot:%d, push ok, seq:%d, ack_seq:%d\r\n",
                    curr_slot, spisync->p_tx->slot[curr_slot].tag.seq, latest_txseqack);
            slot_needsave[curr_slot] = 2;
        }

        }

    return 0;
}
#endif

static int _spisync_reset(spisync_t *spisync)
{
    if (spisync->reset_cb) {
        spisync->reset_cb(spisync->reset_arg);
    }
    spisync_s2m_init(spisync->config->data_rdy_pin);
    arch_delay_ms(5);

    /* reset low */
    lramsync_reset(&spisync->hw);
    /* reset low */

    /* check for copy to txcache_list */
#if SPISYNC_SAVE2CACHE_WHENRESET
    spisync_log("--------------------------------------------------\r\n");
    _spisync_reset_txcache_list(spisync);
    spisync_log("--------------------------------------------------\r\n");
#endif
    spisync->rx_ackseq_fortxcache = 0;

    // FIXME: check if there is valid bytes(stream/queue/list_empty)
    if ((utils_list_is_empty(&spisync->txcache_list)) &&
        (_spisync_is_active(spisync))) {
        spisync_s2m_reset(spisync->config->data_rdy_pin);
    }

    /* clear slot */
    memset(spisync->p_tx, 0, sizeof(spisync_txbuf_t));
    memset(spisync->p_rx, 0, sizeof(spisync_rxbuf_t));

    /* set tag */
    _spisync_reset_tag(spisync, 0);
    /* set clamp */
    _spisync_reset_clamp(spisync);

    /* for rx notify */
    xEventGroupClearBits(spisync->event, EVT_SPISYNC_ALL_BIT);

    /* for timer stop */
    /* Starting a timer ensures the timer is in the active state.  If the timer
     * is not stopped, deleted, or reset in the mean time, the callback function
     * associated with the timer will get called 'n' ticks after xTimerStart() was
     * called, where 'n' is the timers defined period.
     *
     * xTimerStart(...)
     */
    //xTimerReset(spisync->timer, 0);
    //xTimerStop(spisync->timer, 0);

    /* Reset StreamBuffer */
    _spisync_reset_queuestream(spisync);

    /* local val sequence+debug */
    _spisync_reset_localvar(spisync, 0);// 1 init, 0 reset

    //int spisync_clamp_update(void);
    //spisync_clamp_update();

    //int spisync_reset_dnldbuf(void);
    //spisync_reset_dnldbuf();

    /* start low */
    lramsync_start(&spisync->hw);

    return 0;
}

static int __check_tag(spisync_t *spisync)
{
    for (int i = 0; i < SPISYNC_RXSLOT_COUNT; i++) {
        if (SPISYNC_SLOT_VERSION != spisync->p_rx->slot[i].tag.version) {
            return 1;
        }
    }
    return 0;
}
static int __check_pattern(uint32_t *data, uint32_t data_len)
{
    spisync_trace("%08x,%08x,%08x,%08x,%08x,%08x\r\n", data[0], data[1], data[2], data[3], data[4], data[5]);
    if (((data[0] == 0x55555555) && (data[1] == 0x55555555) && (data[2] == 0x55555555) &&
         (data[3] == 0x55555555) && (data[4] == 0x55555555) && (data[5] == 0x55555555)) ||
        ((data[0] == 0xAAAAAAAA) && (data[1] == 0xAAAAAAAA) && (data[2] == 0xAAAAAAAA) &&
         (data[3] == 0xAAAAAAAA) && (data[4] == 0xAAAAAAAA) && (data[5] == 0xAAAAAAAA))
        ) {
        return 1;
    }
    return 0;
}
static int __check_slot(spisync_t *spisync)
{
    int res = 0;

    /* tag check */
    if (__check_tag(spisync)) {
        spisync->rxtag_errtimes++;
        if (spisync->rxtag_errtimes > SPISYNC_TAGERR_TIMES) {
            for (int i = 0; i < SPISYNC_RXSLOT_COUNT; i++) {
                spisync->p_tx->slot[i].tag.flag |= SPISYNC_TAGFLAGBIT_RXERR;
            }
            res = 1;
        }
    } else {
        spisync->rxtag_errtimes = 0;
    }

    /* pattern check */
    if (__check_pattern((uint32_t *)(&spisync->p_rx->slot[0]), sizeof(spisync_slot_t))) {
        spisync->rxpattern_checked++;
        spisync_trace("pattern_checked:%d\r\n", spisync->rxpattern_checked);
        if (spisync->rxpattern_checked == SPISYNC_PATTERN_TIMES) {
            spisync_log("check pattern times:%ld, delay to %ld ms reset\r\n",
                spisync->rxpattern_checked, SPISYNC_PATTERN_DELAYMS);
            _spisync_pattern_process(spisync);
        }

        return 1;
    } else {
        if (spisync->rxpattern_checked) {
            printf("rxpattern_checked:%ld, throhold:%ld\r\n", spisync->rxpattern_checked, SPISYNC_PATTERN_TIMES);
            spisync->rxpattern_checked = 0;
        }
    }

    return res;
}
static void __ramsync_low_rx_cb(spisync_t *spisync)
{
    if (1 == spisync->ps_status) {
        return;
    }

    /* debug */
    if (spisync) {
        spisync->isr_rxcnt++;
    }

    if (__check_slot(spisync)) {
        return;
    }

    spisync->rx_ackseq_fortxcache = get_latest_ackseq(spisync);

    /* Trigger a read event every time */
    spisync_setevent_internal(spisync, EVT_SPISYNC_READ_BIT);
}
static void __ramsync_low_tx_cb(void *arg)
{
    spisync_t *spisync = (spisync_t *)arg;
    int i;
    int have_empty_slot = 0;

    if (1 == spisync->ps_status) {
        return;
    }

    /* debug */
    if (spisync) {
        spisync->isr_txcnt++;
    }

    /* Trigger a read event every time */
    spisync_setevent_internal(spisync, EVT_SPISYNC_WRITE_BIT);
}
static void __ramsync_low_reset_cb(void *spisync)
{
}

void timer_callback(TimerHandle_t xTimer)
{
    spisync_t *spisync = (spisync_t *)pvTimerGetTimerID(xTimer);

    spisync_setevent_internal(spisync, EVT_SPISYNC_RESET_BIT);
}

int spisync_init(spisync_t *spisync, const spisync_config_t *config)
{
    int i;
    BaseType_t ret;

    if (NULL == spisync) {
        return -1;
    }

    node_mem_t node_txbuf[SPISYNC_TXSLOT_COUNT];
    node_mem_t node_rxbuf[SPISYNC_RXSLOT_COUNT];

    g_spisync_current = spisync;

    /* slot */
    memset(spisync, 0, sizeof(spisync_t));
    spisync->config = config->hw;
    memcpy(spisync->sync_ops, config->ops, sizeof(config->ops));
    spisync->p_tx = (spisync_txbuf_t *)malloc_aligned_with_padding_nocache(sizeof(spisync_txbuf_t), 32);
	if (!spisync->p_tx) {
    	spisync_err("no mem for tx buffer\r\n");
    	return -1;
    }
    spisync->p_rx = (spisync_rxbuf_t *)malloc_aligned_with_padding_nocache(sizeof(spisync_rxbuf_t), 32);
    if (!spisync->p_rx) {
    	spisync_err("no mem for rx buffer\r\n");
    	return -1;
    }
    spisync->p_rxcache = (spisync_slot_t *)pvPortMalloc(sizeof(spisync_slot_t));
    if (!spisync->p_rxcache) {
    	spisync_err("no mem for rxcache\r\n");
    	return -1;
    }
    memset(spisync->p_tx, 0, sizeof(spisync_txbuf_t));
    memset(spisync->p_rx, 0, sizeof(spisync_rxbuf_t));
    memset(spisync->p_rxcache, 0, sizeof(spisync_slot_t));
    utils_list_init(&spisync->txcache_list);

    /* dma node list update */
    for (i = 0; i < SPISYNC_TXSLOT_COUNT; i++) {
        node_txbuf[i].buf = &spisync->p_tx->slot[i];
        node_txbuf[i].len = sizeof(spisync_slot_t);
    }
    for (i = 0; i < SPISYNC_RXSLOT_COUNT; i++) {
        node_rxbuf[i].buf = &spisync->p_rx->slot[i];
        node_rxbuf[i].len = sizeof(spisync_slot_t);
    }
    // update version
    _spisync_reset_tag(spisync, 1);
    // spisync_reset_pbufclamp(spisync);

    /* global init*/
    _spisync_reset_localvar(spisync, 1);// 1 init, 0 reset

    /* sys hdl */
    // FIXME: Add NULL check
    spisync->event         = xEventGroupCreate();
    spisync->timer         = xTimerCreate("delay_rst",
                                        pdMS_TO_TICKS(SPISYNC_PATTERN_DELAYMS), // per 10s
                                        pdFALSE,                                // auto reload
                                        (void *)spisync,
                                        timer_callback);
    if (!spisync->timer) {
    	spisync_err("failed to create timer\r\n");
    	return -1;
    }
    spisync->write_lock     = xSemaphoreCreateMutex();
	if (!spisync->write_lock) {
    	spisync_err("failed to create write lock\r\n");
    	return -1;
    }
    spisync->opspin_log     = xSemaphoreCreateMutex();
	    if (!spisync->opspin_log) {
    	spisync_err("failed to create opspin_log");
    	return -1;
    }

    // FIXME: Add NULL check
    spisync->prx_queue[0] = xQueueCreate(SPISYNC_RXMSG_PBUF_ITEMS, sizeof(spisync_msg_t));
    if (!spisync->prx_queue[0]) {
    	spisync_err("failed to create rx queue0\r\n");
    	return -1;
    }
    spisync->prx_queue[1] = xQueueCreate(SPISYNC_RXMSG_SYSCTRL_ITEMS, sizeof(spisync_msg_t));
    if (!spisync->prx_queue[1]) {
        spisync_err("failed to create rx queue1\r\n");
        return -1;
    }
    spisync->prx_queue[2] = xQueueCreate(SPISYNC_RXMSG_USER1_ITEMS, sizeof(spisync_msg_t));
    if (!spisync->prx_queue[2]) {
        spisync_err("failed to create rx queue2\r\n");
        return -1;
    }
    spisync->ptx_queue[0] = xQueueCreate(SPISYNC_TXMSG_PBUF_ITEMS, sizeof(spisync_msg_t));
    if (!spisync->ptx_queue[0]) {
    	spisync_err("failed to create tx queue0\r\n");
        return -1;
    }
    spisync->ptx_queue[1] = xQueueCreate(SPISYNC_TXMSG_SYSCTRL_ITEMS, sizeof(spisync_msg_t));
    if (!spisync->ptx_queue[1]) {
    	spisync_err("failed to create tx queue1\r\n");
        return -1;
    }
    spisync->ptx_queue[2] = xQueueCreate(SPISYNC_TXMSG_USER1_ITEMS, sizeof(spisync_msg_t));
    if (!spisync->ptx_queue[2]) {
        spisync_err("failed to create tx queue2\r\n");
        return -1;
    }
    spisync->prx_streambuffer[0] = xStreamBufferCreate(SPISYNC_RXSTREAM_AT_BYTES, 1);
    if (!spisync->prx_streambuffer[0]) {
    	spisync_err("failed to create rx streambuffer0\r\n");
    	return -1;
    }
    spisync->prx_streambuffer[1] = xStreamBufferCreate(SPISYNC_RXSTREAM_USER1_BYTES, 1);
    if (!spisync->prx_streambuffer[1]) {
        spisync_err("failed to create rx streambuffer1\r\n");
        return -1;
    }
    spisync->prx_streambuffer[2] = xStreamBufferCreate(SPISYNC_RXSTREAM_USER2_BYTES, 1);
    if (!spisync->prx_streambuffer[2]) {
        spisync_err("failed to create rx streambuffer2\r\n");
        return -1;
    }
    spisync->ptx_streambuffer[0] = xStreamBufferCreate(SPISYNC_TXSTREAM_AT_BYTES, 1);
    if (!spisync->ptx_streambuffer[0]) {
        spisync_err("failed to create tx streambuffer0\r\n");
        return -1;
    }
    spisync->ptx_streambuffer[1] = xStreamBufferCreate(SPISYNC_TXSTREAM_USER1_BYTES, 1);
    if (!spisync->ptx_streambuffer[1]) {
        spisync_err("failed to create tx streambuffer1\r\n");
        return -1;
    }
    spisync->ptx_streambuffer[2] = xStreamBufferCreate(SPISYNC_TXSTREAM_USER2_BYTES, 1);
    if (!spisync->ptx_streambuffer[2]) {
        spisync_err("failed to create tx streambuffer2\r\n");
        return -1;
    }

    spisync->reset_cb = config->reset_cb;
    spisync->reset_arg = config->reset_arg;

    ret = xTaskCreate(spisync_task,
                (char*)"spisync",
                SPISYNC_STACK_SIZE/sizeof(StackType_t), spisync,
                SPISYNC_TASK_PRI,
                &spisync->taskhdr);
    if (ret != pdPASS) {
        printf("failed to create spisync task, %ld\r\n", ret);
        return -1;
    }

    /* set type , and set name */
    lramsync_init(
        spisync->config,
        &spisync->hw,
        node_txbuf, SPISYNC_TXSLOT_COUNT,
        __ramsync_low_tx_cb, spisync,
        node_rxbuf, SPISYNC_RXSLOT_COUNT,
        __ramsync_low_rx_cb, spisync,
        __ramsync_low_reset_cb, spisync
        );

    /* to notify host inited */
    spisync_s2m_init(spisync->config->data_rdy_pin);
    spisyn_s2m_set(spisync->config->data_rdy_pin);
    arch_delay_ms(10);
    spisync_trace("spisync_init spisync_s2m_reset\r\n");
    if (_spisync_is_active(spisync)) {
        spisync_s2m_reset(spisync->config->data_rdy_pin);
    }

    {
    extern int syscmd_init(void);
    syscmd_init();
    }

    return 0;
}

int spisync_deinit(spisync_t *spisync)
{
    /* NOT SUPPORT */

    return 0;
}

int spisync_read(spisync_t *spisync, spisync_msg_t *msg, uint32_t flags)
{
    int ret = 0;
    int res = 0;
    uint32_t clamp_offset;

    if ((NULL == spisync) || (NULL == msg)) {
        spisync_log("arg err, spisync:%p\r\n", spisync);
        return -1;
    }

    if (_is_streamtype(msg->type)) {
        ret = xStreamBufferReceive(spisync->prx_streambuffer[msg->type-D_STREAM_OFFSET], msg->buf, msg->buf_len, msg->timeout);

        // update clamp_offset arg
        if ((ret > 0) && (ret <= msg->buf_len)) {
            clamp_offset = ret;
        } else {
            clamp_offset = 0;
        }
        res = ret;
    } else if (_is_msgtype(msg->type)) {
        spisync_msg_t tmpmsg;
        memcpy(&tmpmsg, msg, sizeof(tmpmsg));
        ret = xQueueReceive(spisync->prx_queue[msg->type-D_QUEUE_OFFSET], &tmpmsg, msg->timeout);
        spisync_trace("spisync1:%p, ret:%d, buf_len:%d, cb:%p, arg:%p\r\n", spisync, ret, tmpmsg.buf_len, tmpmsg.cb, tmpmsg.cb_arg);
        if (ret == pdTRUE) {
            if ((msg->buf) && (msg->buf_len >= tmpmsg.buf_len)) {
                msg->buf_len = tmpmsg.buf_len;
                memcpy(msg->buf, tmpmsg.buf, msg->buf_len);
                res = msg->buf_len;
            }

            clamp_offset = 1;
            if (tmpmsg.cb) {
                tmpmsg.cb(tmpmsg.cb_arg);
            }
        } else {
            spisync_trace("spisync2:%p, ret:%d, buf_len:%d\r\n", spisync, ret, tmpmsg.buf_len);

            clamp_offset = 0;
            res = 0;
        }
    }

    /* update clamp */
    _spisync_update_clamp(spisync, msg->type, clamp_offset);

    return res;
}

int spisync_fakewrite_forread(spisync_t *spisync, spisync_msg_t *msg, uint32_t flags)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    int res;

    if ((NULL == spisync) || (NULL == msg) || (msg->buf_len > SPISYNC_PAYLOADBUF_LEN)) {
        spisync_log("arg err, spisync:%p, buf_len:%d, max_len:%d\r\n",
                spisync, msg->buf_len, SPISYNC_PAYLOADBUF_LEN);
        return -1;
    }

    // FIXME: only support AT
    if (xPortIsInsideInterrupt()) {
        res = xStreamBufferSendFromISR(spisync->prx_streambuffer[3-D_STREAM_OFFSET], msg->buf, msg->buf_len, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    } else {
        res = xStreamBufferSend(spisync->prx_streambuffer[3-D_STREAM_OFFSET], msg->buf, msg->buf_len, msg->timeout);
    }

    return res;
}

static void _msg_free(void *p)
{
    spisync_trace("----------> free:%p\r\n", p);
    vPortFree(p);
}
static void *_msg_malloc(int len)
{
    void *p = pvPortMalloc(len);
    spisync_trace("++++++++++> malloc:%p, len:%d\r\n", p, len);

    return p;
}

int spisync_write(spisync_t *spisync, spisync_msg_t *pmsg, uint32_t flags)
{
    uint8_t type = pmsg->type;
    void *buf    = pmsg->buf;
    uint32_t len = pmsg->buf_len;
    uint32_t timeout_ms = pmsg->timeout;

    spisync_trace("spisync_write pmsg type:%d buf:%p buf_len:%d cb:%p cb_arg:%p\r\n",
        pmsg->type, pmsg->buf, pmsg->buf_len, pmsg->cb, pmsg->cb_arg);

    int ret = 0;
    uint32_t rclamp, clamp;
    int32_t diff;
    if ((NULL == spisync) || (type >= 6)) {
        spisync_err("arg error. spisync:%p, len:%d, type:%d\r\n", spisync, len, type);
        return -1;
    }

    if (spisync->ps_status) {
        spisync_ps_wakeup(spisync, NULL);
    }
    // todo: add clamp here.

    if (xSemaphoreTake(spisync->write_lock, timeout_ms) == pdTRUE) {
        if (_is_streamtype(type)) {
            ret = xStreamBufferSend(spisync->ptx_streambuffer[type-D_STREAM_OFFSET], buf, len, timeout_ms);
            if (ret != len) {
                spisync_trace("spisync_write ret:%d\r\n", ret);
            }
        } else if (_is_msgtype(type)) {
            if (pmsg->copy) {
                pmsg->buf = _msg_malloc(pmsg->buf_len);
                if (pmsg->buf) {
                    memcpy(pmsg->buf, buf, pmsg->buf_len);
                    /* for cb free */
                    pmsg->cb     = _msg_free;
                    pmsg->cb_arg = pmsg->buf;
                } else {
                    ret = -2;
                }
            } else {
                ret = -3;
            }

            if (!ret) {
                ret = xQueueSendToBack(spisync->ptx_queue[type-D_QUEUE_OFFSET], pmsg, timeout_ms);
                if (ret ==  pdPASS) {
                    ret = pmsg->buf_len;
                } else {
                    spisync_log("Queue overflow, free it.\r\n");
                    if (pmsg->copy) {
                        pmsg->cb(pmsg->cb_arg);
                    }
                    ret = -4;
                }
            }

            spisync_trace(" queuesend 0x%02X, ret:%d\r\n", pmsg->buf[100], ret);
            spisync_trace("xQueueSendToBack MSG type:%d timeout:%d buf:%p buf_len:%d cb:%p cb_arg:%p, ret:%d\r\n",
                pmsg->type, pmsg->timeout, pmsg->buf, pmsg->buf_len, pmsg->cb, pmsg->cb_arg, ret);
        } else {
            spisync_trace("never here.\r\n");
        }
        //spisync_setevent_internal(spisync, EVT_SPISYNC_WRITE_BIT);
        xSemaphoreGive(spisync->write_lock);
    } else {
        spisync_log("take mutex error, so write NULL.");
        ret = -5;
    }

    /* ops pin */
    if (xSemaphoreTake(spisync->opspin_log, portMAX_DELAY) == pdTRUE) {
        spisyn_s2m_set(spisync->config->data_rdy_pin);
    }
    xSemaphoreGive(spisync->opspin_log);
    spisync_setevent_internal(spisync, EVT_SPISYNC_WRITE_BIT);

    return ret;
}
/*---------------------- moudule ------------------------*/
static void _reset_process(spisync_t *spisync)
{
    _spisync_reset(spisync);
}
static void _ctrl_process(spisync_t *spisync)
{
}
static uint32_t get_latest_clamp(spisync_t *spisync, uint8_t type)
{
    uint32_t latest = spisync->p_rx->slot[0].tag.clamp[type];

    for (uint32_t i = 1; i < SPISYNC_RXSLOT_COUNT; i++) {
        if ((spisync->p_rx->slot[i].tag.clamp[type] > latest && (spisync->p_rx->slot[i].tag.clamp[type] - latest) <= CALC_CLAMP_THRESHOLD) ||
            (spisync->p_rx->slot[i].tag.clamp[type] < latest && (latest - spisync->p_rx->slot[i].tag.clamp[type]) > CALC_CLAMP_THRESHOLD)) {
            latest = spisync->p_rx->slot[i].tag.clamp[type];
        }
    }

    return latest;
}
static uint16_t get_latest_ackseq(spisync_t *spisync)
{
    uint16_t latest = spisync->p_rx->slot[0].tag.ackseq;

    for (uint32_t i = 1; i < SPISYNC_RXSLOT_COUNT; i++) {
        if ((spisync->p_rx->slot[i].tag.ackseq > latest && (spisync->p_rx->slot[i].tag.ackseq - latest) <= CALC_ACKSEQ_THRESHOLD) ||
            (spisync->p_rx->slot[i].tag.ackseq < latest && (latest - spisync->p_rx->slot[i].tag.ackseq) > CALC_ACKSEQ_THRESHOLD)) {
            latest = spisync->p_rx->slot[i].tag.ackseq;
        }
    }

    return latest;
}

#define BYTES_BEFOR_TO_WRITE    (128)
int get_next_slot(uint32_t startaddr, uint32_t curraddr)
{
    // Calculate the DMA address after 500us
    uint32_t future_dma_address = curraddr + BYTES_BEFOR_TO_WRITE;

    // Calculate the overflow offset if the address exceeds the end address
    uint32_t offset = (future_dma_address - startaddr) % (SPISYNC_TXSLOT_COUNT * sizeof(spisync_slot_t));

    // Determine which slot the DMA will be in after 500us
    uint32_t current_slot = offset / sizeof(spisync_slot_t);

    // Calculate the next slot
    int next_slot = (current_slot + 1) % SPISYNC_TXSLOT_COUNT;

    return next_slot;
}
void _write_process(spisync_t *spisync)
{
    int empty_slot, empty_flag;
    uint16_t current, latest_rseq, s_txseq;

    int available_bytes;
    uint32_t data_len;
    StreamBufferHandle_t current_stream_buffer;
    uint32_t current_stream_len;
    uint8_t current_pack_type;
    struct crc32_stream_ctx crc_ctx;
    uint32_t rclamp, clamp;
    int32_t diff;
    int tx_invalid = 0;
    uint8_t type;
    spisync_msg_t msg;
    int res;
    int slot_start = 0;
    uint32_t start_addr, curr_addr;
    spisync_slot_t *node = NULL;

    /* Check if there is an empty slot */
    empty_slot = 0;
    empty_flag = 0;

    lramsync_get_info(&spisync->hw, &start_addr, &curr_addr);
    slot_start = get_next_slot(start_addr, curr_addr);

    //spisync_log("start_addr:%08X, curr_addr:%08X, slot_start:%d\r\n", start_addr, curr_addr, slot_start);

    for (int m = slot_start; m < (slot_start+SPISYNC_TXSLOT_COUNT); m++) {
        latest_rseq = spisync->rx_ackseq_fortxcache;//get_latest_ackseq(spisync);
        s_txseq = spisync->p_tx->slot[m%SPISYNC_TXSLOT_COUNT].tag.seq;

        if (SEQ_GEQ(latest_rseq, s_txseq)) {
            empty_slot = m%SPISYNC_TXSLOT_COUNT;
            empty_flag = 1;
            spisync_trace("latest_rseq:%d, s_txseq:%d, empty_slot:%d\r\n",
                        latest_rseq, s_txseq, empty_slot);
            break;
        }
    }
    if (!empty_flag) {
        // Refineme
        spisync->dbg_write_slotblock++;
        return;
    }

    /* check if there is valid bytes */
    if (xSemaphoreTake(spisync->opspin_log, portMAX_DELAY) == pdTRUE) {
        if (!utils_list_is_empty(&spisync->txcache_list)) {
            node = (spisync_slot_t *)utils_list_pop_front(&spisync->txcache_list);
            type = node->payload.seg[0].type;
            current_pack_type = type;
            spisync_trace("------ type:%d, node:%p, remain:%s\r\n",
                    type, node,
                    (utils_list_is_empty(&spisync->txcache_list))?"empty":"valid"
                    );
        } else {
        for (type = 0; type < 6; type++) {
            if (_is_streamtype(type)) {
                available_bytes = xStreamBufferBytesAvailable(spisync->ptx_streambuffer[type-D_STREAM_OFFSET]);
                if (available_bytes <= 0) {
                    tx_invalid++;
                    continue;
                }

                rclamp = get_latest_clamp(spisync, type);
                spisync_trace("rclamp:%d, local_clamp:%d\r\n", rclamp, spisync->clamp[type]);
                if (CLAMP_LEQ(rclamp, spisync->clamp[type])) {
                    spisync->dbg_write_clampblock[type]++;
                    continue;
                }

                diff = rclamp - spisync->clamp[type];
                current_pack_type = type;
                current_stream_buffer = spisync->ptx_streambuffer[type-D_STREAM_OFFSET];
                current_stream_len = UINT32_MIN((UINT32_MIN(diff, available_bytes)), SPISYNC_PAYLOADBUF_LEN);

                spisync_trace("type:%d, len:%ld, recv_clamp:%d, local_clamp:%d, available:%d\r\n",
                        current_pack_type, current_stream_len, rclamp, spisync->clamp[type], available_bytes);
            } else if (_is_msgtype(type)) {
                if (uxQueueMessagesWaiting(spisync->ptx_queue[type-D_QUEUE_OFFSET]) == 0) {
                    tx_invalid++;
                    continue;
                }

                rclamp = get_latest_clamp(spisync, type);
                spisync_trace("rclamp:%d, local_clamp:%d\r\n", rclamp, spisync->clamp[type]);
                if (CLAMP_LEQ(rclamp, spisync->clamp[type])) {
                    spisync->dbg_write_clampblock[type]++;
                    continue;
                }

                current_pack_type = type;
            } else {
                spisync_err("never here type:%d.\r\n", type);
                continue;
            }
            break;
        }

        if ((6 == tx_invalid) &&
            (spisync->tx_seq == latest_rseq) &&
            utils_list_is_empty(&spisync->txcache_list) &&
            _spisync_is_active(spisync)) {
            spisync_trace("write_process spisync_s2m_reset\r\n");
            spisync_s2m_reset(spisync->config->data_rdy_pin);
        }
        }
    }
    xSemaphoreGive(spisync->opspin_log);

    if (type == 6) {// can't write to slot
        if (node) {
            free(node);
            spisync_log("====== free node:%p\r\n", node);
        }
        return;
    }

    /* pop from msgbuffer to slot */
    if (node) {
        data_len = node->payload.tot_len - 4;
        memcpy(spisync->p_tx->slot[empty_slot].payload.seg[0].buf, node->payload.seg[0].buf, data_len);
        spisync_log("====== free node:%p\r\n", node);
        if (node) {
            free(node);
        }
        if (_is_streamtype(current_pack_type)) {
            spisync->clamp[current_pack_type] += data_len;
        } else if (_is_msgtype(current_pack_type)) {
            spisync->clamp[current_pack_type] += 1;
        }
    } else
    if (_is_streamtype(current_pack_type)) {
        data_len = xStreamBufferReceive(
                current_stream_buffer,
                spisync->p_tx->slot[empty_slot].payload.seg[0].buf,
                current_stream_len,
                0);
        spisync_trace("------------- write-> ackseq:%d, seq:%d, slot:%d, len:%d, avail:%d, current:%d, oldseq:%d\r\n",
                latest_rseq, (spisync->tx_seq + 1), empty_slot, data_len, available_bytes, current, s_txseq);
        if ((data_len <= 0) || (data_len > SPISYNC_PAYLOADBUF_LEN)) {
            spisync_err("write process never here. except spi reset?, data_len:%d\r\n", data_len);
            return;
        }
        // update clamp
        spisync->clamp[current_pack_type] += data_len;
    } else if (_is_msgtype(current_pack_type)) {
        res = xQueueReceive(spisync->ptx_queue[type-D_QUEUE_OFFSET], &msg, 0);
        uint8_t *buf2 = msg.buf;
        spisync_trace(" queuerecv ---> low write 0x%02X, res:%d, len:%d\r\n",
                buf2[100],
                res,
                msg.buf_len);
        if (pdTRUE != res) {
            spisync_err("never here type:%d.\r\n", type);
            return;
        }

        spisync_trace("xQueueReceive res:%d, msg type:%d buf:%p buf_len:%d cb:%p cb_arg:%p\r\n",
                res, msg.type, msg.buf, msg.buf_len, msg.cb, msg.cb_arg);
        //return;
#if 1
        if ((msg.buf_len > 0) && (msg.buf_len <= SPISYNC_PAYLOADBUF_LEN)) {
            static uint8_t old_val = 0;
            memcpy(spisync->p_tx->slot[empty_slot].payload.seg[0].buf, msg.buf, msg.buf_len);
            spisync_trace(" write2slot ---> low write 0x%02X, slot:%d, old_val:0x%02X\r\n",
                    spisync->p_tx->slot[empty_slot].payload.seg[0].buf[100],
                    empty_slot,
                    old_val);
            if (((uint8_t)(old_val+1)) != spisync->p_tx->slot[empty_slot].payload.seg[0].buf[100]) {
                spisync_trace(" waring data is discotinuous old_val:0x%02X\r\n", old_val);
            }
            old_val = spisync->p_tx->slot[empty_slot].payload.seg[0].buf[100];
        } else {
            spisync_err("write process never here. except spi reset?, data_len:%d\r\n", data_len);
            if (msg.cb) {
                msg.cb(msg.cb_arg);
            }
            return;
        }
        if (msg.cb) {
            msg.cb(msg.cb_arg);
        }
        data_len = msg.buf_len;
        // update clamp
        spisync->clamp[current_pack_type] += 1;
#endif
    } else {
        spisync_err("never here type:%d.\r\n", type);
        return;
    }
    // update clamp
    spisync->tx_seq++;

    spisync->p_tx->slot[empty_slot].tag.seq = spisync->tx_seq;
    spisync->p_tx->slot[empty_slot].payload.tot_len = data_len + 4;// seg_len_type_res
    spisync->p_tx->slot[empty_slot].payload.seg[0].len = data_len;
    spisync->p_tx->slot[empty_slot].payload.seg[0].type = current_pack_type;
#if SPISYNC_TXCRC_ENABLE
#if SPISYNC_TXCRC32_ENABLE
    // CRC check
    utils_crc32_stream_init(&crc_ctx);
    spisync_trace("utils_crc32_stream_feed_block slot:%d, buf:%p, len:%d\r\n",
            empty_slot,
            (const uint8_t *)&spisync->p_tx->slot[empty_slot].payload,
            spisync->p_tx->slot[empty_slot].payload.seg[0].len);
    utils_crc32_stream_feed_block(&crc_ctx,
                                  (const uint8_t *)&spisync->p_tx->slot[empty_slot].payload.tot_len,
                                  spisync->p_tx->slot[empty_slot].payload.tot_len + 4);
    spisync->p_tx->slot[empty_slot].payload.crc = utils_crc32_stream_results(&crc_ctx);
#else
    // CRC16 check
    spisync->p_tx->slot[empty_slot].payload.crc =
        spisync_checksum((const uint8_t *)&spisync->p_tx->slot[empty_slot].payload.tot_len,
            spisync->p_tx->slot[empty_slot].payload.tot_len + 4);
#endif
#endif
    spisync->p_tx->slot[empty_slot].tail.seq_mirror = spisync->tx_seq;

    // debug
    spisync->tsk_writecnt += 1;

    spisync_trace("write slot:%d, latest_rseq:%d, Txseq:%d, len:%d,  %d-%d-%d-%d  %d-%d-%d-%d\r\n",
            empty_slot, latest_rseq, spisync->tx_seq, data_len,
            spisync->p_tx->slot[0].tag.seq,
            spisync->p_tx->slot[1].tag.seq,
            spisync->p_tx->slot[2].tag.seq,
            spisync->p_tx->slot[3].tag.seq,
            spisync->p_rx->slot[0].tag.ackseq,
            spisync->p_rx->slot[1].tag.ackseq,
            spisync->p_rx->slot[2].tag.ackseq,
            spisync->p_rx->slot[3].tag.ackseq
            );
}

static void _read_process(spisync_t *spisync)
{
    uint32_t lentmp;
    uint32_t crctmp;
    uint16_t seqtmp, rseqtmp;
    struct crc32_stream_ctx crc_ctx;
    int space_valid;
    int read_times = SPISYNC_RX_READTIMES;
    uint8_t pack_type;
    int res;

    while (read_times) {
        for (int i = 0; i < SPISYNC_RXSLOT_COUNT; i++) {
            // check version
            if (SPISYNC_SLOT_VERSION != spisync->p_rx->slot[i].tag.version) {
                continue;
            }

            // check seq
            if (spisync->p_rx->slot[i].tag.seq != spisync->p_rx->slot[i].tail.seq_mirror) {
                continue;
            }

            // update tmp
            rseqtmp = spisync->p_tx->slot[i].tag.ackseq;
            seqtmp  = spisync->p_rx->slot[i].tag.seq;
            lentmp  = spisync->p_rx->slot[i].payload.tot_len;
            pack_type = (spisync->p_rx->slot[i].payload.seg[0].type)&0xFF;

            // check seq len valid
            if ((lentmp > SPISYNC_PAYLOADLEN_MAX) || ((uint16_t)seqtmp != (uint16_t)(rseqtmp + 1)) || (pack_type >= 6)) {
                continue;
            }

            // Check available spaces in the buffer in for rx

            // copy data and crc
            memcpy(spisync->p_rxcache, &spisync->p_rx->slot[i], lentmp+sizeof(slot_tag_t)+4);
            spisync->p_rxcache->payload.crc = spisync->p_rx->slot[i].payload.crc;
            // Check if the relevant parameters are within the expected range
            if ((spisync->p_rxcache->tag.seq != seqtmp) ||
                (spisync->p_rxcache->payload.tot_len != lentmp) ||
                ((spisync->p_rxcache->payload.seg[0].type & 0xFF) != pack_type)) {
                continue;
            }

            // check crc
#if SPISYNC_RXCRC_ENABLE
#if SPISYNC_RXCRC32_ENABLE
            utils_crc32_stream_init(&crc_ctx);
            utils_crc32_stream_feed_block(&crc_ctx,
                                  (const uint8_t *)&spisync->p_rxcache->payload.tot_len,
                                  spisync->p_rxcache->payload.tot_len + 4);
            crctmp = utils_crc32_stream_results(&crc_ctx);
#else
            crctmp = spisync_checksum((const uint8_t *)&spisync->p_rxcache->payload.tot_len,
                                  spisync->p_rxcache->payload.tot_len + 4);
#endif

            if (spisync->p_rxcache->payload.crc != crctmp) {
                spisync->rxcrcerr_cnt++;
                //printf("crc err, i:%d, 0x%08X != 0x%08X\r\n", i, crctmp, spisync->p_rxcache->payload.crc);
                spisync_trace("========= crc err, i:%d, seq:%d, cal_rest:0x%08X != recv:0x%08X\r\n",
                    i, seqtmp, crctmp, spisync->p_rxcache->payload.crc);
                continue;
            }
            spisync_trace("========= crc  ok, i:%d, seq:%d, cal_rest:0x%08X != recv:0x%08X\r\n", i, seqtmp, crctmp, spisync->p_rxcache->payload.crc);
#endif
            // push
            spisync->tsk_readcnt++;

            spisync_trace("recv slot %d rx seq:  %d,%d,%d,%d  %d,%d,%d,%d\r\n",
                    i,
                    spisync->p_rx->slot[0].tag.seq,
                    spisync->p_rx->slot[1].tag.seq,
                    spisync->p_rx->slot[2].tag.seq,
                    spisync->p_rx->slot[3].tag.seq,
                    spisync->p_rx->slot[0].tag.ackseq,
                    spisync->p_rx->slot[1].tag.ackseq,
                    spisync->p_rx->slot[2].tag.ackseq,
                    spisync->p_rx->slot[3].tag.ackseq
                    );

            /* handle by buf buf_len */
            if (spisync->sync_ops[pack_type].cb && spisync->sync_ops[pack_type].cb) {
                // zero copy
                spisync_opscb_arg_t msg;
                msg.buf     = spisync->p_rxcache->payload.seg[0].buf;
                msg.buf_len = (lentmp > PAYLOADBUF0_SEG_OFFSET) ? (lentmp - PAYLOADBUF0_SEG_OFFSET) : 0;
                spisync->sync_ops[pack_type].cb(spisync->sync_ops[pack_type].cb_pri, &msg);
                // _spisync_update_clamp(spisync, pack_type, 1);// clamp++
            } else {
                // normal to stream or queue
                if (_is_streamtype(pack_type)) {
                    if ((lentmp - PAYLOADBUF0_SEG_OFFSET) != xStreamBufferSend(spisync->prx_streambuffer[pack_type-D_STREAM_OFFSET],
                            spisync->p_rxcache->payload.seg[0].buf, (lentmp - PAYLOADBUF0_SEG_OFFSET), 0)) {
                        spisync_err("flowctrl warning. except spi reset? buf_len:%d\r\n", (lentmp > PAYLOADBUF0_SEG_OFFSET));
                        spisync->dbg_read_flowblock[pack_type]++;
                        //spisync_status(spisync);
                        //vTaskDelay(500);
                        continue;
                    }
                } else {
                    spisync_msg_t msg;
                    uint8_t       *abuf;
                    abuf = _msg_malloc(lentmp - PAYLOADBUF0_SEG_OFFSET);
                    if (msg.buf) {
                        SPISYNC_MSGINIT(&msg,
                            pack_type,              /* type */
                            1,                      /* copy */
                            0,                      /* timeout */
                            abuf, (lentmp - PAYLOADBUF0_SEG_OFFSET),  /* buf buf_len */
                            _msg_free,              /* cb */
                            abuf);                  /* cb_arg */
                        memcpy(msg.buf, spisync->p_rxcache->payload.seg[0].buf, (lentmp - PAYLOADBUF0_SEG_OFFSET));
                        if (xQueueSendToBack(spisync->prx_queue[pack_type-D_QUEUE_OFFSET], &msg, 0) != pdPASS) {
                            spisync->dbg_read_flowblock[pack_type]++;
                            _msg_free(msg.buf);
                            continue;
                        }
                    } else {
                        spisync->dbg_read_flowblock[pack_type]++;
                        spisync_err("mem error --> drop msg %d\r\n", (lentmp - PAYLOADBUF0_SEG_OFFSET));
                    }
                }
            }

            /* update tx st ackseq */
            for (int m = 0; m < SPISYNC_TXSLOT_COUNT; m++) {
                spisync->p_tx->slot[m].tag.ackseq = spisync->p_rxcache->tag.seq;
            }
        }
        read_times--;
    }
}

static void spisync_task(void *arg)
{
    spisync_t *spisync = (spisync_t *)arg;
    EventBits_t eventBits;

    while (1) {
        spisync_trace("wait bit.\r\n");
        eventBits = xEventGroupWaitBits(spisync->event,
        EVT_SPISYNC_RESET_BIT|EVT_SPISYNC_CTRL_BIT|EVT_SPISYNC_WRITE_BIT| \
        EVT_SPISYNC_READ_BIT| \
        EVT_SPISYNC_DUMP_BIT,
        pdTRUE, pdFALSE,
        portMAX_DELAY);
        spisync_trace("get bit 0x%08x.\r\n", eventBits);
        if (eventBits  & EVT_SPISYNC_RESET_BIT) {
            spisync_log("recv bit EVT_SPISYNC_RESET_BIT start\r\n");
            spisync->tsk_rstcnt++;
            _reset_process(spisync);
            spisync_log("recv bit EVT_SPISYNC_RESET_BIT end\r\n");
        }
        if (eventBits  & EVT_SPISYNC_CTRL_BIT) {
            spisync_log("recv bit EVT_SPISYNC_CTRL_BIT start\r\n");
            _ctrl_process(spisync);
            spisync_log("recv bit EVT_SPISYNC_CTRL_BIT end\r\n");
        }
        if (eventBits  & EVT_SPISYNC_DUMP_BIT) {
#if SPISYNC_DEBUG_MOUDULE_EN
            spisync_status(spisync);
#endif
        }
        if (eventBits  & EVT_SPISYNC_WRITE_BIT) {
            spisync_trace("recv bit EVT_SPISYNC_WRITE_BIT start\r\n");
            _write_process(spisync);
            spisync_trace("recv bit EVT_SPISYNC_WRITE_BIT end\r\n");
        }
        if (eventBits  & EVT_SPISYNC_READ_BIT) {
            spisync_trace("recv bit EVT_SPISYNC_READ_BIT start\r\n");
            _read_process(spisync);
            spisync_trace("recv bit EVT_SPISYNC_READ_BIT end\r\n");
        }
    }
}

#if SPISYNC_DEBUG_MOUDULE_EN
/*--------------------- debug moudule ----------------------*/
/* debug */
int spisync_status(spisync_t *spisync)
{
    int i;

    if ((NULL == spisync) && (NULL == g_spisync_current)) {
        return -1;
    } else if (NULL == spisync) {
        spisync = g_spisync_current;
    }
    spisync_log("\r\n");

    printf("GPIO-00-01-02-03: 0x%08lX-0x%08lX-0x%08lX-0x%08lX\r\n",
        *(volatile uint32_t *)(0x200008C4 + 0*4),
        *(volatile uint32_t *)(0x200008C4 + 1*4),
        *(volatile uint32_t *)(0x200008C4 + 2*4),
        *(volatile uint32_t *)(0x200008C4 + 3*4));
    printf("GPIO-10-11-12-13: 0x%08lX-0x%08lX-0x%08lX-0x%08lX\r\n",
        *(volatile uint32_t *)(0x200008C4 + 10*4),
        *(volatile uint32_t *)(0x200008C4 + 11*4),
        *(volatile uint32_t *)(0x200008C4 + 12*4),
        *(volatile uint32_t *)(0x200008C4 + 13*4));
    printf("GPIO-14-15-16-17: 0x%08lX-0x%08lX-0x%08lX-0x%08lX\r\n",
        *(volatile uint32_t *)(0x200008C4 + 14*4),
        *(volatile uint32_t *)(0x200008C4 + 15*4),
        *(volatile uint32_t *)(0x200008C4 + 16*4),
        *(volatile uint32_t *)(0x200008C4 + 17*4));
    printf("GPIO-27-28-29-30: 0x%08lX-0x%08lX-0x%08lX-0x%08lX\r\n",
        *(volatile uint32_t *)(0x200008C4 + 27*4),
        *(volatile uint32_t *)(0x200008C4 + 28*4),
        *(volatile uint32_t *)(0x200008C4 + 29*4),
        *(volatile uint32_t *)(0x200008C4 + 30*4));

    lramsync_dump(&spisync->hw);
    spisync_dbg("sizeof(slot_tag_t)    =%d\r\n", sizeof(slot_tag_t));
    spisync_dbg("sizeof(slot_payload_t)=%d\r\n", sizeof(slot_payload_t));
    spisync_dbg("sizeof(slot_tail_t)   =%d\r\n", sizeof(slot_tail_t));
    spisync_dbg("sizeof(spisync_slot_t) =%d\r\n", sizeof(spisync_slot_t));
    spisync_dbg("CLAMP_THRESHOLD:%d, ACKSEQ_THRESHOLD:%d\r\n", CALC_CLAMP_THRESHOLD, CALC_ACKSEQ_THRESHOLD);
    spisync_dbg("TxSlot--------------------------------------------------\r\n");
    for (i = 0; i < SPISYNC_TXSLOT_COUNT; i++) {
        spisync_dbg("tx[%ld] 0x%04lX,seq:%d,ackseq:%ld,flag:%ld,clamp:%ld-%ld-%ld-%ld-%ld-%ld, len:%d,type:%02X,crc:0x%08lX, seq_mirror:%d, res:%d-%d\r\n",
                i,
                spisync->p_tx->slot[i].tag.version,
                spisync->p_tx->slot[i].tag.seq,
                spisync->p_tx->slot[i].tag.ackseq,
                spisync->p_tx->slot[i].tag.flag,
                spisync->p_tx->slot[i].tag.clamp[0], spisync->p_tx->slot[i].tag.clamp[1], spisync->p_tx->slot[i].tag.clamp[2],
                spisync->p_tx->slot[i].tag.clamp[3], spisync->p_tx->slot[i].tag.clamp[4], spisync->p_tx->slot[i].tag.clamp[5],
                spisync->p_tx->slot[i].payload.seg[0].len,
                spisync->p_tx->slot[i].payload.seg[0].type,
                spisync->p_tx->slot[i].payload.crc,
                spisync->p_tx->slot[i].tail.seq_mirror,
                spisync->p_tx->slot[i].tail.reserved[0],
                spisync->p_tx->slot[i].tail.reserved[1]);
        spisync_buf("     ", &spisync->p_tx->slot[i].payload, 32);
    }
    spisync_dbg("RxSlot--------------------------------------------------\r\n");
    for (i = 0; i < SPISYNC_RXSLOT_COUNT; i++) {
        spisync_dbg("rx[%ld] 0x%04lX,seq:%d,ackseq:%ld,flag:%ld,clamp:%ld-%ld-%ld-%ld-%ld-%ld, len:%d,type:%02X,crc:0x%08lX, seq_mirror:%d, res:%d-%d\r\n",
                i,
                spisync->p_rx->slot[i].tag.version,
                spisync->p_rx->slot[i].tag.seq,
                spisync->p_rx->slot[i].tag.ackseq,
                spisync->p_rx->slot[i].tag.flag,
                spisync->p_rx->slot[i].tag.clamp[0], spisync->p_rx->slot[i].tag.clamp[1], spisync->p_rx->slot[i].tag.clamp[2],
                spisync->p_rx->slot[i].tag.clamp[3], spisync->p_rx->slot[i].tag.clamp[4], spisync->p_rx->slot[i].tag.clamp[5],
                spisync->p_rx->slot[i].payload.seg[0].len,
                spisync->p_rx->slot[i].payload.seg[0].type,
                spisync->p_rx->slot[i].payload.crc,
                spisync->p_rx->slot[i].tail.seq_mirror,
                spisync->p_rx->slot[i].tail.reserved[0],
                spisync->p_rx->slot[i].tail.reserved[1]);
        spisync_buf("     ", &spisync->p_rx->slot[i].payload, 32);
    }

    spisync_dbg("Rx------------------------------------------------------\r\n");
    spisync_dbg("RxQueue[0] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPEMSG_PBUF,
                uxQueueSpacesAvailable(spisync->prx_queue[0]), uxQueueMessagesWaiting(spisync->prx_queue[0]));
    spisync_dbg("RxQueue[1] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPEMSG_SYSCTRL,
                uxQueueSpacesAvailable(spisync->prx_queue[1]), uxQueueMessagesWaiting(spisync->prx_queue[1]));
    spisync_dbg("RxQueue[2] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPEMSG_USER1,
                uxQueueSpacesAvailable(spisync->prx_queue[2]), uxQueueMessagesWaiting(spisync->prx_queue[2]));
    spisync_dbg("Rxstream[0] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPESTREAM_AT,
                xStreamBufferSpacesAvailable(spisync->prx_streambuffer[0]),
                (SPISYNC_RXSTREAM_AT_BYTES-xStreamBufferSpacesAvailable(spisync->prx_streambuffer[0])));
    spisync_dbg("Rxstream[1] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPESTREAM_USER1,
                xStreamBufferSpacesAvailable(spisync->prx_streambuffer[1]),
                (SPISYNC_RXSTREAM_USER1_BYTES-xStreamBufferSpacesAvailable(spisync->prx_streambuffer[1])));
    spisync_dbg("Rxstream[2] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPESTREAM_USER2,
                xStreamBufferSpacesAvailable(spisync->prx_streambuffer[2]),
                (SPISYNC_RXSTREAM_USER2_BYTES-xStreamBufferSpacesAvailable(spisync->prx_streambuffer[2])));
    spisync_dbg("Tx------------------------------------------------------\r\n");
    spisync_dbg("TxQueue[0] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPEMSG_PBUF,
                uxQueueSpacesAvailable(spisync->ptx_queue[0]), uxQueueMessagesWaiting(spisync->ptx_queue[0]));
    spisync_dbg("TxQueue[1] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPEMSG_SYSCTRL,
                uxQueueSpacesAvailable(spisync->ptx_queue[1]), uxQueueMessagesWaiting(spisync->ptx_queue[1]));
    spisync_dbg("TxQueue[2] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPEMSG_USER1,
                uxQueueSpacesAvailable(spisync->ptx_queue[2]), uxQueueMessagesWaiting(spisync->ptx_queue[2]));
    spisync_dbg("Txstream[0] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPESTREAM_AT,
                xStreamBufferSpacesAvailable(spisync->ptx_streambuffer[0]),
                (SPISYNC_TXSTREAM_AT_BYTES-xStreamBufferSpacesAvailable(spisync->ptx_streambuffer[0])));
    spisync_dbg("Txstream[1] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPESTREAM_USER1,
                xStreamBufferSpacesAvailable(spisync->ptx_streambuffer[1]),
                (SPISYNC_TXSTREAM_USER1_BYTES-xStreamBufferSpacesAvailable(spisync->ptx_streambuffer[1])));
    spisync_dbg("Txstream[2] tot:%ld, free_space:%ld, used:%ld\r\n", SPISYNC_TYPESTREAM_USER2,
                xStreamBufferSpacesAvailable(spisync->ptx_streambuffer[2]),
                (SPISYNC_TXSTREAM_USER2_BYTES-xStreamBufferSpacesAvailable(spisync->ptx_streambuffer[2])));

    spisync_dbg("txcache_list %s, rx_ackseq_fortxcache:%d\r\n",
        (utils_list_is_empty(&spisync->txcache_list))?"empty":"valid",
        spisync->rx_ackseq_fortxcache
        );
    spisync_dbg("------------------------------------------------------\r\n");

    spisync_dbg("Diff TXClamp:%ld-%ld-%ld-%ld-%ld-%ld\r\n",
            get_latest_clamp(spisync, 0)-spisync->clamp[0],
            get_latest_clamp(spisync, 1)-spisync->clamp[1],
            get_latest_clamp(spisync, 2)-spisync->clamp[2],
            get_latest_clamp(spisync, 3)-spisync->clamp[3],
            get_latest_clamp(spisync, 4)-spisync->clamp[4],
            get_latest_clamp(spisync, 5)-spisync->clamp[5]);
    spisync_dbg("Remote Clamp:%ld-%ld-%ld-%ld-%ld-%ld\r\n",
            get_latest_clamp(spisync, 0), get_latest_clamp(spisync, 1),
            get_latest_clamp(spisync, 2), get_latest_clamp(spisync, 3),
            get_latest_clamp(spisync, 4), get_latest_clamp(spisync, 5));
    spisync_dbg("LocalTxClamp:%ld-%ld-%ld-%ld-%ld-%ld\r\n",
            spisync->clamp[0], spisync->clamp[1], spisync->clamp[2],
            spisync->clamp[3], spisync->clamp[4], spisync->clamp[5]);
    spisync_dbg("Local RxTagE:%ld, RxPattern:%ld\r\n",
            spisync->rxtag_errtimes, spisync->rxpattern_checked);
    uint16_t latest_ackseq = get_latest_ackseq(spisync);
    spisync_dbg("Local Txseq:%ld(remote:%d,diff:%d), isrRx:%d, isrTx:%d, Rst:%d, CrcE:%d, tskW:%d, tskR:%d\r\n",
            spisync->tx_seq, latest_ackseq, (latest_ackseq-spisync->tx_seq),
            spisync->isr_rxcnt,
            spisync->isr_txcnt,
            spisync->tsk_rstcnt,
            spisync->rxcrcerr_cnt,
            spisync->tsk_writecnt, spisync->tsk_readcnt);
    spisync_dbg("Local Write slotblock:%d, clamp_block:%d-%d-%d-%d-%d-%d\r\n",
            spisync->dbg_write_slotblock,
            spisync->dbg_write_clampblock[0],spisync->dbg_write_clampblock[1],
            spisync->dbg_write_clampblock[2],spisync->dbg_write_clampblock[3],
            spisync->dbg_write_clampblock[4],spisync->dbg_write_clampblock[5]
            );
    spisync_dbg("Local Read flow_block:%d-%d-%d-%d-%d-%d\r\n",
            spisync->dbg_read_flowblock[0],spisync->dbg_read_flowblock[1],
            spisync->dbg_read_flowblock[2],spisync->dbg_read_flowblock[3],
            spisync->dbg_read_flowblock[4],spisync->dbg_read_flowblock[5]
            );
    spisync_dbg("Local GPIO:%d, ps_status:%d, gpio_active:%d, ps_keepactive:%d\r\n",
            spisync_s2m_get(spisync->config->data_rdy_pin),
            spisync->ps_status,
            spisync->ps_gpioactive,
            spisync->ps_keepactive);

    //void dnldbuf_status(char *str);
    //dnldbuf_status("dump");

    //void spi_ethernetif_status(void);
    //spi_ethernetif_status();

    return 0;
}
int spisync_status_internal(spisync_t *spisync)
{
    spisync_setevent_internal(spisync, EVT_SPISYNC_DUMP_BIT);

    return 0;
}

int cmd_spisync_write(int argc, char **argv)
{
    spisync_msg_t msg;
    uint8_t type;
    uint32_t len;
    uint32_t send_len = 0;
    uint32_t count = 1; // Default to sending once
    uint32_t timeout;
    uint8_t *buf; // Pointer for dynamically allocated memory

    if (argc < 3) {
        printf("Usage: ss_write [type 0-5] [str] [-s send_length] [-c count]\r\neg:ss_write 0 123456789abcdefg -s 2048 -c 10\r\n");
        return -1; // Return if there are not enough arguments
    }

    type = atoi(argv[1]);
    if (type < 0 || type > 5) {
        printf("Invalid type. Must be between 0 and 5.\r\n");
        return -1; // Return if the type is invalid
    }

    len = strlen(argv[2]);

    // Parse additional arguments
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            send_len = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            count = atoi(argv[++i]);
        }
    }

    if (send_len == 0) {
        send_len = len; // If no send length is specified, use the string length
    }

    buf = malloc(send_len + 1); // Allocate enough memory to store the data to be sent
    if (!buf) {
        printf("Memory allocation error.\r\n");
        return -1;
    }

    // Fill the buffer
    for (uint32_t i = 0; i < send_len; i++) {
        buf[i] = argv[2][i % len]; // Repeat the string data until the required length is reached
    }
    buf[send_len] = '\0'; // Ensure the string is null-terminated

    timeout = 100; // Initialize timeout to 100ms

    // Send data packets
    for (uint32_t i = 0; i < count; i++) {
#if 0
        /* Check memory availability before sending */
        struct meminfo info;
        bflb_mem_usage(KMEM_HEAP, &info);
        spisync_trace("fs:%d, us:%d\r\n", info.free_size, info.used_size);
        if (info.free_size < 50*1024) {
            break;
        }
#endif

        SPISYNC_MSGINIT(&msg,
                    type,           /* type */
                    1,              /* copy */
                    timeout,        /* timeout */
                    buf, send_len,  /* buf buf_len */
                    NULL,           /* cb */
                    NULL);          /* cb_arg */
        int ret = spisync_write(g_spisync_current, &msg, 0);
        if (ret != send_len) {
            printf("spisync_write ret:%d, drop it.\r\n", ret);
        }
    }

    free(buf); // Free the allocated memory

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_spisync_write, ss_write, ss_write [type 0-5] [str] [-s send_length] [-c count]);

int cmd_spisync_read(int argc, char **argv)
{
    int res;
    spisync_msg_t msg;
    uint8_t type;
    uint32_t len = SPISYNC_PAYLOADBUF_LEN; // Default buffer length is 2K
    uint32_t timeout = 0; // Default timeout is 0
    uint8_t *buf;

    // gpt start
    if (argc < 2) {
        printf("ss_read [type 0-5]\r\neg:ss_read 0\r\n");
        return -1; // Return if not enough arguments
    }

    type = atoi(argv[1]);
    if (type < 0 || type > 5) {
        printf("Invalid type. Must be between 0 and 5.\r\n");
        return -1; // Return if type is invalid
    }

    // Allocate enough memory for read buf
    buf = malloc(len + 1);
    if (!buf) {
        printf("Memory allocation error.\r\n");
        return -1;
    }

    // Prepare message
    SPISYNC_MSGINIT(&msg,
                    type,  /* type */
                    0,                      /* copy */
                    timeout,                /* timeout */
                    buf, len,               /* buf buf_len */
                    NULL,                   /* cb */
                    NULL);                  /* cb_arg */

    // gpt end
    res = spisync_read(g_spisync_current, &msg, 0);
    if (res < 0) {
        printf("Error reading from SPI sync, res:%d\r\n", res);
        free(buf);
        return -1;
    }

    // Print the received data
    if (res >  0) {
        printf("recv len[%d]\r\n", msg.buf_len);
#if 0
        for (uint32_t i = 0; i < UINT32_MIN(len, msg.buf_len); i++) {
            printf("%02x ", buf[i]);
        }
        printf("\r\n");
#endif
    }

    free(buf); // Free allocated memory

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spisync_read, ss_read, ss_read [type 0-5].);

int cmd_spisync_status(int argc, char **argv)
{
    spisync_status_internal(g_spisync_current);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spisync_status, ss_status, spisync status);

int cmd_spisync_perf(int argc, char **argv)
{
    uint32_t timeout = 5000;// 100ms
    if (argc < 3) {
        printf("Usage: ss_perf [type 0-5] [-t duration] [-R for read] [-i interval] [-s packetsize]\r\n"
                "eg 2: ss_perf 2 -t 60 -i 1 -s %d\r\n"
                "      ss_perf 2 -t 60 -i 1 -R\r\n"
                "eg 5: ss_perf 5 -t 60 -i 1 -s %d\r\n"
                "      ss_perf 5 -t 60 -i 1 -R\r\n"
                , SPISYNC_PAYLOADBUF_LEN, SPISYNC_PAYLOADBUF_LEN
                );
        return -1;
    }

    uint8_t type = atoi(argv[1]);
    if (type < 0 || type > 5) {
        printf("Invalid type. Must be between 0 and 5.\r\n");
        return -1;
    }

    uint32_t duration = 10; // Default duration of 10 seconds
    int is_read = 0;
    uint32_t interval = 1; // Default interval of 1 second for printing throughput
    uint32_t packet_size = SPISYNC_PAYLOADBUF_LEN; // Default packet size for writing is 1K
    uint32_t read_length = SPISYNC_PAYLOADBUF_LEN; // Default read length is 2K

    // Parse additional arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            duration = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-R") == 0) {
            is_read = 1;
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            interval = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            packet_size = atoi(argv[++i]);
        }
    }

    uint8_t *buf = malloc(is_read ? read_length : packet_size);
    if (!buf) {
        printf("Memory allocation error.\r\n");
        return -1;
    }

    // Initialize buffer with dummy data for write
    if (!is_read) {
        memset(buf, 'A', packet_size);
    }

    TickType_t start_time = xTaskGetTickCount();
    uint32_t total_bytes = 0;

    while ((xTaskGetTickCount() - start_time) / configTICK_RATE_HZ < duration) {
        TickType_t current_time = xTaskGetTickCount();
        uint32_t interval_bytes = 0;
        TickType_t interval_start_time = current_time;

        while ((xTaskGetTickCount() - interval_start_time) / configTICK_RATE_HZ < interval) {
            spisync_msg_t msg;
            int result;

            SPISYNC_MSGINIT(&msg,
                type,                                       /* type */
                (is_read?0:1),                                          /* copy */
                timeout,                                    /* timeout */
                buf, (is_read ? read_length : packet_size), /* buf buf_len */
                NULL,                                       /* cb */
                NULL);                                      /* cb_arg */

            spisync_trace("[%d] 1packet_size:%d-%d, interval_bytes:%d, total_bytes:%d\r\n",
                    xTaskGetTickCount(), packet_size, msg.buf_len, interval_bytes, total_bytes);
            result = is_read ? spisync_read(g_spisync_current, &msg, 0) : spisync_write(g_spisync_current, &msg, 0);

            spisync_trace("[%d] 2packet_size:%d-%d, res:%d, interval_bytes:%d, total_bytes:%d\r\n",
                    xTaskGetTickCount(), packet_size, msg.buf_len, result, interval_bytes, total_bytes);
            if (result < 0) {
                printf("Error %s to SPI sync.\r\n", is_read ? "reading" : "writing");
                free(buf);
                return -1;
            }

            if (result > 0) {
                interval_bytes += is_read ? read_length : packet_size;
                total_bytes += is_read ? read_length : packet_size;
            }
            current_time = xTaskGetTickCount();
        }

        double elapsed_interval = (double)(xTaskGetTickCount() - interval_start_time) / configTICK_RATE_HZ;
        uint32_t end_sec = (xTaskGetTickCount() - start_time) / configTICK_RATE_HZ;
        uint32_t start_sec = end_sec - interval;
        printf("Interval [%u-%u] sec: %.2f bytes/sec, %.2f bps\r\n", start_sec, end_sec, interval_bytes / elapsed_interval, (interval_bytes  / elapsed_interval) * 8);
        {
        //    void ps_cmd(char *buf, int len, int argc, char **argv);
        //    ps_cmd(NULL, 0, 0, NULL);
        }
    }

    double elapsed_time = (double)(xTaskGetTickCount() - start_time) / configTICK_RATE_HZ;
    printf("Test completed in %.2f seconds.\r\n", elapsed_time);
    printf("Total bytes %s: %u\r\n", is_read ? "read" : "written", total_bytes);
    printf("Average throughput: %.2f bytes/sec, %.2f bps\r\n", total_bytes / elapsed_time, ((total_bytes / elapsed_time) * 8));

    free(buf);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spisync_perf, ss_perf, ss_perf [type 0-5] [-t duration] [-R for read] [-i interval] [-s packetsize]);

static uint16_t get_latest_txackseq(spisync_t *spisync)
{
    uint16_t latest = spisync->p_tx->slot[0].tag.ackseq;

    for (uint32_t i = 1; i < SPISYNC_TXSLOT_COUNT; i++) {
        if ((spisync->p_tx->slot[i].tag.ackseq > latest && (spisync->p_tx->slot[i].tag.ackseq - latest) <= CALC_ACKSEQ_THRESHOLD) ||
            (spisync->p_tx->slot[i].tag.ackseq < latest && (latest - spisync->p_tx->slot[i].tag.ackseq) > CALC_ACKSEQ_THRESHOLD)) {
            latest = spisync->p_tx->slot[i].tag.ackseq;
        }
    }

    return latest;
}
void iperf_async_task(void *arg)
{
    uint32_t timeout = 1000;// 100ms

    uint8_t type = 3;
    if (type < 0 || type > 5) {
        printf("Invalid type. Must be between 0 and 5.\r\n");
        return -1;
    }

    uint32_t duration = 10; // Default duration of 10 seconds
    int is_read = 0;
    uint32_t interval = 1; // Default interval of 1 second for printing throughput
    uint32_t packet_size = SPISYNC_PAYLOADBUF_LEN; // Default packet size for writing is 1K
    uint32_t read_length = SPISYNC_PAYLOADBUF_LEN; // Default read length is 2K

    // Parse additional arguments
    type = 3;// ===============================
    duration = 3600;// ===============================
    is_read = 1;// ===============================
    interval = 1;// ===============================

    uint8_t *buf = malloc(is_read ? read_length : packet_size);
    if (!buf) {
        printf("Memory allocation error.\r\n");
        return -1;
    }

    // Initialize buffer with dummy data for write
    if (!is_read) {
        memset(buf, 'A', packet_size);
    }

    TickType_t start_time = xTaskGetTickCount();
    uint32_t total_bytes = 0;

    while ((xTaskGetTickCount() - start_time) / configTICK_RATE_HZ < duration) {
        TickType_t current_time = xTaskGetTickCount();
        uint32_t interval_bytes = 0;
        TickType_t interval_start_time = current_time;

        while ((xTaskGetTickCount() - interval_start_time) / configTICK_RATE_HZ < interval) {
            spisync_msg_t msg;
            int result;

            SPISYNC_MSGINIT(&msg,
                type,                                       /* type */
                0,                                          /* copy */
                timeout,                                    /* timeout */
                buf, (is_read ? read_length : packet_size), /* buf buf_len */
                NULL,                                       /* cb */
                NULL);                                      /* cb_arg */


            spisync_trace("[%d] 1packet_size:%d-%d, interval_bytes:%d, total_bytes:%d\r\n",
                    xTaskGetTickCount(), packet_size, msg.buf_len, interval_bytes, total_bytes);
            result = is_read ? spisync_read(g_spisync_current, &msg, 0) : spisync_write(g_spisync_current, &msg, 0);

            spisync_trace("[%d] 2packet_size:%d-%d, res:%d, interval_bytes:%d, total_bytes:%d\r\n",
                    xTaskGetTickCount(), packet_size, msg.buf_len, result, interval_bytes, total_bytes);
            if (result < 0) {
                printf("Error %s to SPI sync.\r\n", is_read ? "reading" : "writing");
                free(buf);
                return -1;
            }

            if (result > 0) {
                interval_bytes += is_read ? read_length : packet_size;
                total_bytes += is_read ? read_length : packet_size;
            }
            current_time = xTaskGetTickCount();
        }

        double elapsed_interval = (double)(xTaskGetTickCount() - interval_start_time) / configTICK_RATE_HZ;
        uint32_t end_sec = (xTaskGetTickCount() - start_time) / configTICK_RATE_HZ;
        uint32_t start_sec = end_sec - interval;
        //printf("Interval [%u-%u] sec: %.2f bytes/sec, %.2f bps\r\n", start_sec, end_sec, interval_bytes / elapsed_interval, (interval_bytes  / elapsed_interval) * 8);
        spisync_t *spisync = g_spisync_current;
#if 0
        printf("%.1f, %d-%d-%d-%d  %d\r\n",
                (interval_bytes  / elapsed_interval) * 8,
                spisync->p_rx->slot[0].tag.seq,
                spisync->p_rx->slot[1].tag.seq,
                spisync->p_rx->slot[2].tag.seq,
                spisync->p_rx->slot[3].tag.seq,
                get_latest_txackseq(spisync)
                );
#else
        {
            uint16_t seq[16];
            float tp = (interval_bytes  / elapsed_interval) * 8;
            seq[0] = spisync->p_rx->slot[0].tag.seq;
            seq[1] = spisync->p_rx->slot[1].tag.seq;
            seq[2] = spisync->p_rx->slot[2].tag.seq;
            seq[3] = spisync->p_rx->slot[3].tag.seq;
            seq[4] = spisync->p_tx->slot[0].tag.seq;
            seq[5] = spisync->p_tx->slot[1].tag.seq;
            seq[6] = spisync->p_tx->slot[2].tag.seq;
            seq[7] = spisync->p_tx->slot[3].tag.seq;
            seq[8] = get_latest_txackseq(spisync);

            printf("%.1f, -%d-%d-%d-%d  %d-%d-%d-%d  %d\r\n",
                tp, seq[0], seq[1], seq[2], seq[3], seq[4], seq[5], seq[6], seq[7], seq[8]
                );
        }
#endif
    }

    double elapsed_time = (double)(xTaskGetTickCount() - start_time) / configTICK_RATE_HZ;
    printf("Test completed in %.2f seconds.\r\n", elapsed_time);
    printf("Total bytes %s: %u\r\n", is_read ? "read" : "written", total_bytes);
    printf("Average throughput: %.2f bytes/sec, %.2f bps\r\n", total_bytes / elapsed_time, ((total_bytes / elapsed_time) * 8));

    free(buf);
    vTaskDelete(NULL);
    return 0;
}
#define SSPERF_STACK_SIZE (8192)
#define SSPERF_TASK_PRI   (30)
int cmd_spisync_perf2(int argc, char **argv)
{
    int ret;
    ret = xTaskCreate(iperf_async_task,
                (char*)"perf async",
                SSPERF_STACK_SIZE/sizeof(StackType_t), NULL,
                SSPERF_TASK_PRI,
                NULL);
    if (ret != pdPASS) {
        printf("failed to create spisync task, %ld\r\n", ret);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_spisync_perf2, ss_async, ss_perf2 [type 0-5] [-t duration] [-R for read] [-i interval] [-s packetsize]);

int cmd_spisync_echo(int argc, char **argv)
{
    int res;
    uint32_t timeout = 10000; // 100 ms

    if (argc < 2) {
        printf("Usage: ss_echo [type 0-5] [-t time_s] [-c for continuous]\r\neg: ss_echo 0 -t 5000\r\n");
        return -1;
    }

    uint8_t type = atoi(argv[1]);
    if (type < 0 || type > 5) {
        printf("Invalid type. Must be between 0 and 5.\r\n");
        return -1;
    }

    uint32_t time_limit = 10*1000; // Default to 10 seconds
    int continuous = 0;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            time_limit = atoi(argv[++i]) * 1000;
        } else if (strcmp(argv[i], "-c") == 0) {
            continuous = 1;
            time_limit = portMAX_DELAY; // Set to no time limit
        }
    }

    uint32_t buffer_size = SPISYNC_PAYLOADBUF_LEN; // Echo buffer size
    uint8_t *buf = malloc(buffer_size);
    if (!buf) {
        printf("Memory allocation error.\r\n");
        return -1;
    }

    TickType_t start_time = xTaskGetTickCount();
    TickType_t end_time = start_time + time_limit;

    while (continuous || xTaskGetTickCount() < end_time) {
        spisync_msg_t msg, msg2;
        SPISYNC_MSGINIT(&msg,
                type,             /* type */
                0,                /* copy */
                timeout,          /* timeout */
                buf, buffer_size, /* buf buf_len */
                NULL,             /* cb */
                NULL);            /* cb_arg */

        // Receive data
        res = spisync_read(g_spisync_current, &msg, 0);
        if (res <=0) {
            printf("low read 0x%02X, res:%d\r\n", buf[100], res);
            continue;
        }
        spisync_trace("app read 0x%02X, res:%d\r\n", buf[100], res);
        spisync_trace("====> read[%d] res:%d\r\n", msg.buf_len, res);

        // Echo back the received data
        if (_is_msgtype(msg.type)) {
            SPISYNC_MSGINIT(&msg2,
                        msg.type,               /* type */
                        1,                      /* copy */
                        timeout,                /* timeout */
                        msg.buf, msg.buf_len,   /* buf buf_len */
                        NULL,                   /* cb */
                        NULL);                  /* cb_arg */
            res = spisync_write(g_spisync_current, &msg2, 0);
        } else {
            res = spisync_write(g_spisync_current, &msg, 0);
        }
        spisync_trace("app write 0x%02X, res:%d, len:%d\r\n", buf[100], res, msg.buf_len);
        if (res <= 0) {
            spisync_log("app write 0x%02X, res:%d, len:%d\r\n", buf[100], res, msg.buf_len);
        }
    }

    free(buf);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spisync_echo, ss_echo, ss_echo [type 0-5] [-t time_s] [-c for continuous]);
#endif

