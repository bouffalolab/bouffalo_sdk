#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#if defined BL702L
#include <bl702l_phy.h>
#endif

#ifdef BOUFFALO_SDK
#include <bflb_common.h>
#include <bflb_l1c.h>
#include <bflb_efuse.h>
#include <bflb_flash.h>
#else
#include <MCU_Common/misc/misc.h>
#include <bl_wireless.h>
#endif

#include <lmac154.h>
#include <lmac154_frame.h>
#include <lmac154_fpt.h>
#include <zb_timer.h>

#include <openthread-core-config.h>
#include <openthread/platform/alarm-micro.h>
#include <openthread_port.h>
#include <ot_radio_trx.h>

#define VERSION_STRING_HELPER(x, y, z) #x "." #y "." #z
#define VERSION_STRING(x, y, z) VERSION_STRING_HELPER(x, y, z)

#define VERSION_OT_SRC_STRING VERSION_STRING(VERSION_OT_SRC_MAJOR, VERSION_OT_SRC_MINOR, VERSION_OT_SRC_PATCH)
#define VERSION_OT_SRC_NUMBER \
    ((VERSION_OT_SRC_MAJOR << 16) || (VERSION_OT_SRC_MINOR << 8) || VERSION_OT_SRC_PATCH)

#ifdef VERSION_OT_SRC_EXTRA_INFO
static const char *version_ot_br __attribute__((used, section(".version.ot_src"))) = \
    "component_version_ot_src_" VERSION_OT_SRC_STRING "_" VERSION_OT_SRC_EXTRA_INFO; 
#else
static const char *version_ot_utils __attribute__((used, section(".version.ot_src"))) = \
    "component_version_ot_src_" VERSION_OT_SRC_STRING; 
#endif

ot_radio_t                  ot_radio_ctx;
static uint8_t              otRadio_rx_buff[MAC_FRAME_SIZE * OTRADIO_RX_FRAME_BUFFER_NUM + MAX_FRAME_SIZE_ACK] __attribute__((aligned(4)));
static uint8_t              otRadio_tx_buff[MAX_SIZE_PSDU + MAX_BUFFER_SIZE_A_DATA + MAX_BUFFER_SIZE_M_DATA] __attribute__((aligned(16)));
static uint8_t              otRadio_ack_buff[MAX_BUFFER_SIZE_ACK * 2] __attribute__((aligned(16)));

#define SWAP32_TO_BIG_ENDIAN(x) \
    ((((x) & 0xFF000000) >> 24) | \
     (((x) & 0x00FF0000) >> 8)  | \
     (((x) & 0x0000FF00) << 8)  | \
     (((x) & 0x000000FF) << 24))

void ot_radioInit(void)
{
    otRadio_rxFrame_t *pframe = NULL;

    memset(&ot_radio_ctx, 0, sizeof(ot_radio_t));

    uint32_t tag = otrEnterCrit();
    utils_dlist_init(&ot_radio_ctx.frame_list);
    utils_dlist_init(&ot_radio_ctx.rx_frame_list);

    for (int i = 0; i < OTRADIO_RX_FRAME_BUFFER_NUM; i ++) {
        pframe = (otRadio_rxFrame_t *) (otRadio_rx_buff + MAC_FRAME_SIZE * i);
        pframe->frame.mPsdu = ((uint8_t *)pframe) + FRAME_OVERHEAD_SIZE;
        pframe->frame.mLength = 0;
        utils_dlist_add_tail(&pframe->dlist, &ot_radio_ctx.frame_list);
    }

    ot_radio_ctx.rx_ack_frame = (otRadioFrame *)(otRadio_rx_buff + MAC_FRAME_SIZE * OTRADIO_RX_FRAME_BUFFER_NUM);
    ot_radio_ctx.rx_ack_frame->mPsdu = ((uint8_t *)ot_radio_ctx.rx_ack_frame) + FRAME_OVERHEAD_SIZE;

    ot_link_metrics_init(IEEE802_15_4_RADIO_RECEIVE_SENSITIVITY);

    ot_radio_ctx.tx_param.csl_opt.csl_tx_offset_sym_cnt = 3;
    otrExitCrit(tag);
}

void ot_radioSetCslTxOffset(int8_t aOffsetSymCnt)
{
    ot_radio_ctx.tx_param.csl_opt.csl_tx_offset_sym_cnt = aOffsetSymCnt;
}

void ot_radioSetInstance(otInstance *instance)
{
    ot_radio_ctx.instance = instance;
}

#if OT_RADIO_CSL_DEBUG
/* cumulative accuracy stats of frames received with CSL enabled */
typedef struct {
    uint32_t            cnt;
    uint32_t            in_win;
    uint32_t            err;
    int32_t             dev_min_us;
    int32_t             dev_max_us;
    int64_t             dev_sum_us;
    uint32_t            cnt16;          /* frames since the last stats line */
    int64_t             dev_sum16_us;
    uint32_t            edge_front_min_us;
    uint32_t            edge_end_min_us;
} ot_radio_csl_rx_stats_t;

static ot_radio_csl_rx_stats_t ot_radio_csl_rx_stats;

/* cumulative timing stats of our own csl-delayed transmissions (we acting as
 * the csl transmitter, e.g. a parent sending to a sleepy child): actual tx
 * time vs the programmed target */
typedef struct {
    uint32_t            cnt;
    int32_t             dev_min_us;
    int32_t             dev_max_us;
    int64_t             dev_sum_us;
} ot_radio_csl_tx_stats_t;

static ot_radio_csl_tx_stats_t ot_radio_csl_tx_stats;

/* per-tx timing record: filled by the tx-done ISR, printed by the radio task.
 * only csl-delayed transmissions get a timing record - a csl receiver's own
 * tx (reply, data poll) is not csl-scheduled and has no timing to verify.
 * frames carrying a csl ie additionally report the next-rx-window phase they
 * advertised (the peer schedules its csl tx from it) */
typedef struct {
    uint8_t             valid;
    uint8_t             is_delayed;
    uint8_t             acked;
    uint8_t             seq;
    uint8_t             has_phase;
    uint16_t            ie_phase;       /* 10-symbol units, from lmac154_gen_csl_ie */
    uint32_t            actual_sym;
    uint32_t            expected_sym;
    int32_t             drift_us;
    uint32_t            desc_win_sym;   /* next-rx-window this frame advertised: sfd + phase*10 */
    int32_t             desc_diff_us;   /* advertised window vs our sample grid, mod period */
    int32_t             ie2sfd_us;      /* csl ie generation -> actual sfd (software tail latency) */
    int16_t             comp_avg_sym;   /* tail compensation lmac154 applied to this frame */
    int16_t             tail_sym;       /* this frame's measured raw software tail */
} ot_radio_tx_dbg_t;

static ot_radio_tx_dbg_t ot_radio_dbg_txd;

/* child's next-window anchor inferred from the latest enh-ack we received
 * (csl tx role): ack sfd (our rx timestamp of the ack) + the ie phase it
 * carried. the csl tx ref print diffs its own data-frame-derived window
 * against this anchor */
static uint32_t ot_radio_dbg_ackref_sym;
static uint8_t  ot_radio_dbg_ackref_valid;

/* last armed CSL RX window, from otPlatRadioReceiveAt; symbol counts on the
 * M154_SYM_CNT time base, channel in 11..26 */
static uint32_t ot_radio_dbg_win_start_sym;
static uint32_t ot_radio_dbg_win_end_sym;
static uint8_t  ot_radio_dbg_win_channel;

/* print the accumulated stats line once every so many frames */
#define OT_RADIO_CSL_STATS_EVERY 16

static inline void ot_radio_csl_tx_stats_update(ot_radio_csl_tx_stats_t *st, int32_t drift_us)
{
    st->cnt++;
    if (st->cnt == 1 || drift_us < st->dev_min_us) {
        st->dev_min_us = drift_us;
    }
    if (st->cnt == 1 || drift_us > st->dev_max_us) {
        st->dev_max_us = drift_us;
    }
    st->dev_sum_us += drift_us;
}

static lmac154_ie_csl_t *ot_radio_find_csl_ie(uint32_t *frame, uint8_t len, uint8_t mac_hdr_size);

/* radio task context: print the landing record of one received frame */
static void ot_radio_dbg_print_rx(otRadio_rxFrame_t *pframe)
{
    ot_radio_rx_dbg_t       *d = &pframe->dbg;
    ot_radio_csl_rx_stats_t *st = &ot_radio_csl_rx_stats;
    char                    src_str[6];

    if (!d->valid) {
        return;
    }
    d->valid = 0;

    if (d->has_csl_ie) {
        /* pure csl transmitter: a frame from our sleepy child carrying its
         * next-window advertisement - the reference we schedule from.
         * ackref: same window derived from the child's latest enh-ack
         * (our tx anchor); both mod the child period - 0 when they agree.
         * computed only when the two anchors are close in time (within two
         * periods) - a stale anchor folds into garbage */
        uint32_t w_df_sym = d->sfd_sym + (uint32_t)d->csl_phase * 10;
        int32_t  ackref_us = 0;

        if (ot_radio_dbg_ackref_valid && d->csl_period) {
            uint32_t period_sym = (uint32_t)d->csl_period * 10;
            /* wrap-safe signed diff folded to +-period/2: whole periods fold
             * out, what remains is the sub-period disagreement of the two
             * advertisements */
            int32_t  diff_sym = (int32_t)(w_df_sym - ot_radio_dbg_ackref_sym);

            if (diff_sym > (int32_t)period_sym / 2) diff_sym -= (int32_t)period_sym;
            else if (diff_sym < -(int32_t)period_sym / 2) diff_sym += (int32_t)period_sym;
            ackref_us = diff_sym * (int32_t)LMAC154_US_PER_SYMBOL;
        }
        printf("csl tx ref: seq %u phase %u period %u sfd %lu ackref %+ldus\r\n",
               d->seq, d->csl_phase, d->csl_period, d->sfd_sym, ackref_us);
        return;
    }

    if (d->src_is_saddr) {
        snprintf(src_str, sizeof(src_str), "%04x", d->src_saddr);
    }
    else {
        snprintf(src_str, sizeof(src_str), "ext");
    }

    if (d->csl_period) {
        /* ph: where the peer thinks our next rx window opens, relative to
         * this frame (10-symbol units); compare with the real sample time.
         * ackdiff: the window our enh-ack advertised vs the same grid */
        printf("csl rx frame: sfd %lu src %s seq %u drift %ldus win %u edge[%ld %ld]us ack_ph %u ph %u ackdiff %+ldus ack_exec %+ldus\r\n",
               d->sfd_sym, src_str, d->seq, d->drift_us, d->in_win, d->edge_start_us, d->edge_end_us,
               d->ack_phase, d->csl_phase, d->ack_desc_diff_us, d->ack_exec_err_us);
    }
    else {
        /* the peer (ot stack) never includes a csl ie in its frames - the
         * child resyncs from the frame arrival timestamp instead. an OT
         * parent frame without csl ie is EXPECTED, not a sync problem */
        printf("csl rx frame: sfd %lu src %s seq %u drift %ldus win %u edge[%ld %ld]us ack_ph %u ackdiff %+ldus ack_exec %+ldus no peer csl ie\r\n",
               d->sfd_sym, src_str, d->seq, d->drift_us, d->in_win, d->edge_start_us, d->edge_end_us,
               d->ack_phase, d->ack_desc_diff_us, d->ack_exec_err_us);
    }

    if ((st->cnt % OT_RADIO_CSL_STATS_EVERY) == 0 && st->cnt16) {
        printf("csl rx stats: n%lu win%lu/%lu err%lu dev[%ld %ld %ld]us dev16 %ldus edge_min[%lu %lu]us\r\n",
               st->cnt, st->in_win, st->cnt, st->err,
               st->dev_min_us, (int32_t)(st->dev_sum_us / st->cnt), st->dev_max_us,
               (int32_t)(st->dev_sum16_us / st->cnt16),
               st->edge_front_min_us, st->edge_end_min_us);
        st->cnt16 = 0;
        st->dev_sum16_us = 0;
    }
}

/* radio task context: print the timing record of one transmission */
static void ot_radio_dbg_print_tx(void)
{
    ot_radio_tx_dbg_t       *d = &ot_radio_dbg_txd;
    ot_radio_csl_tx_stats_t *st = &ot_radio_csl_tx_stats;

    if (!d->valid) {
        return;
    }
    d->valid = 0;

    if (d->is_delayed) {
        /* csl transmitter: csl-delayed tx conformance against the target
         * programmed by the scheduler */
        printf("csl tx txd: seq %u expected %lu actual %lu drift %ldus ph %u tdev[%ld %ld]us\r\n",
               d->seq, d->expected_sym, d->actual_sym, d->drift_us, d->ie_phase,
               st->dev_min_us, st->dev_max_us);
    }
    else {
        /* csl receiver: our own tx completed carrying the csl ie - the
         * next-rx-window advertisement our peer schedules from; window opens
         * ie_phase*160us after this frame. desc_win/diff: the advertised
         * window time vs our executing sample grid (mod period, 0 expected
         * up to the 160us ie phase resolution). ie2sfd: residual of the
         * tail compensation; comp: compensation applied to this frame;
         * tail: this frame's measured raw software tail (tail = comp +
         * ie2sfd) */
        printf("csl rx txd: seq %u ph %u (%luus) desc_win %lu diff %+ldus ie2sfd %+ldus comp %+ldus tail %+ldus\r\n",
               d->seq, d->ie_phase, (uint32_t)d->ie_phase * (uint32_t)LMAC154_US_PER_SYMBOL * 10U,
               d->desc_win_sym, d->desc_diff_us, d->ie2sfd_us,
               (int32_t)d->comp_avg_sym * (int32_t)LMAC154_US_PER_SYMBOL,
               (int32_t)d->tail_sym * (int32_t)LMAC154_US_PER_SYMBOL);
    }
}

/* tx-done ISR context: fill the per-tx timing record, integer ops only */
static void ot_radio_dbg_txd_capture(lmac154_tx_status_t tx_status)
{
    ot_radio_tx_dbg_t *d = &ot_radio_dbg_txd;
    /** real sfd end time of sent packet: tx done time stamp - (pkt len + fcs len) * 2 - phy header len.
     * pkt_length excludes fcs (the lmac writes phr = pkt_length + 2) */
    uint32_t actual_sfd = (uint32_t)(lmac154_get_tx_done_timestamp() >> 4) - ((uint32_t)(ot_radio_ctx.tx_param.pkt_length + 2) << 1) - 2;

    d->acked = (tx_status == LMAC154_TX_STATUS_ACKED);
    d->seq = ot_radio_ctx.tx_frame->mPsdu[2];
    d->actual_sym = actual_sfd;
    d->expected_sym = 0;
    d->drift_us = 0;
    /* next-rx-window phase this frame advertised (10-symbol units) */
    d->has_phase = (ot_radio_ctx.tx_param.csl_opt.csl_ie_offset != 0);
    d->ie_phase = ot_radio_ctx.tx_param.csl_opt.dbg_csl_tx_phase;
    if (!d->has_phase && ot_radio_ctx.tx_frame->mLength > 2) {
        /* OT may have updated the header itself (mIsHeaderUpdated), in which
         * case the platform skipped the ie search above - read the phase the
         * frame actually advertises straight from the tx buffer */
        lmac154_ie_csl_t *csl = ot_radio_find_csl_ie((uint32_t *)ot_radio_ctx.tx_frame->mPsdu,
                ot_radio_ctx.tx_frame->mLength - 2,
                (uint8_t)lmac154_get_mhr_len_2015((uint8_t *)ot_radio_ctx.tx_frame->mPsdu));
        if (csl) {
            d->has_phase = 1;
            d->ie_phase = csl->bf.phase;
        }
    }

    /* ie generation -> actual sfd residual: lmac154 pre-compensates the ie
     * sfd estimate with its internally tracked software tail, so a value
     * converged near zero here means the compensation is accurate */
    d->ie2sfd_us = 0;
    d->comp_avg_sym = (int16_t)ot_radio_ctx.tx_param.csl_opt.dbg_csl_ie_comp_avg;
    d->tail_sym = 0;
    if (d->has_phase && ot_radio_ctx.tx_param.csl_opt.dbg_sfd_est_sym) {
        d->ie2sfd_us = (int32_t)(actual_sfd - ot_radio_ctx.tx_param.csl_opt.dbg_sfd_est_sym) * (int32_t)LMAC154_US_PER_SYMBOL;
        d->tail_sym = (int16_t)ot_radio_ctx.tx_param.csl_opt.dbg_csl_ie_comp;
    }

    if (d->has_phase && ot_radio_ctx.tx_param.csl_opt.dbg_sfd_est_sym) {
        /* the ie advertises our next rx window at sfd + phase*10 (symbols);
         * wrap-safe signed diff vs our executing sample grid folded to
         * +-period/2: exposes the phase rounding of lmac154_gen_csl_ie
         * (ceil, 0..9 sym = 0..144us) plus any schedule drift. only for
         * frames whose ie lmac154 generated - for ot-prebuilt ie frames
         * (data poll) the platform sample copy is not the reference ot
         * used */
        uint32_t period_sym = (uint32_t)ot_radio_ctx.tx_param.csl_opt.csl_period * 10;
        int32_t  diff_sym = (int32_t)(actual_sfd + (uint32_t)d->ie_phase * 10 - ot_radio_ctx.tx_param.csl_opt.csl_sample_time);

        d->desc_win_sym = actual_sfd + (uint32_t)d->ie_phase * 10;
        if (diff_sym > (int32_t)period_sym / 2) diff_sym -= (int32_t)period_sym;
        else if (diff_sym < -(int32_t)period_sym / 2) diff_sym += (int32_t)period_sym;
        d->desc_diff_us = diff_sym * (int32_t)LMAC154_US_PER_SYMBOL;
    }
    else {
        d->desc_win_sym = 0;
        d->desc_diff_us = 0;
    }

    /* only a csl-delayed transmission (we are the csl transmitter,
     * e.g. parent sending into a sleepy child's window) has csl tx
     * timing to verify; discriminate on the tx_timing actually
     * programmed for THIS frame, not on our own csl_period, so a
     * node that is csl receiver and csl transmitter at the same time
     * is still measured correctly. positive drift = sent late. */
    if (ot_radio_ctx.tx_param.tx_timing.base_time || ot_radio_ctx.tx_param.tx_timing.delay_time) {
        uint32_t expected_shr = ot_radio_ctx.tx_param.tx_timing.base_time + ot_radio_ctx.tx_param.tx_timing.delay_time;
        int32_t drift_us = (int32_t)(actual_sfd - expected_shr) << LMAC154_US_PER_SYMBOL_BITS;

        d->valid = 1;
        d->is_delayed = 1;
        d->expected_sym = expected_shr;
        d->drift_us = drift_us;
        ot_radio_csl_tx_stats_update(&ot_radio_csl_tx_stats, drift_us);
    }
    else {
        d->valid = d->has_phase;
    }
}

/* rx-done ISR pass: snapshot the per-frame landing inputs. must run in this
 * pass - the lmac switches back to the resume channel and reuses the mpdu
 * buffer once the enh-ack exchange starts */
static void ot_radio_dbg_rxd_capture(otRadio_rxFrame_t *rx_frame, lmac154_receiveInfo_t *info, uint32_t *frame)
{
    rx_frame->dbg.on_win_ch =
        (lmac154_getChannel() + OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN) == ot_radio_dbg_win_channel;
    rx_frame->dbg.seq = (uint8_t)(frame[0] >> 16);
    rx_frame->dbg.src_is_saddr = info->is_src_saddr;
    rx_frame->dbg.src_saddr = info->src_addr.saddr;
    /* next-rx-window phase we just advertised to the peer in the enh-ack
     * (10-symbol units), computed by lmac154_gen_csl_ie above; the actual
     * advertisement error is measured at ACK_SENT in ot_radio_dbg_rxd_measure */
    rx_frame->dbg.ack_phase = info->dbg_csl_ack_phase;
    rx_frame->dbg.ack_desc_diff_us = 0;
    rx_frame->dbg.ack_exec_err_us = 0;
}

/* radio task context helper: short name of the lmac rf state */
static const char *ot_radio_dbg_rf_state(int rf_state)
{
    static const char * const names[] = {
        "?", "rx_trig", "rx", "rx_doing", "ack", "tx", "csma", "idle"
    };

    if (rf_state <= 0 || (size_t)rf_state >= sizeof(names) / sizeof(names[0])) {
        return names[0];
    }
    return names[rf_state];
}

/* radio task context: a csl-delayed transmission was just queued */
static void ot_radio_dbg_print_send(const otRadioFrame *aFrame, int iret)
{
    printf("csl tx submit: seq %u base %lu delay %lu now %lu ret %d\r\n",
           aFrame->mPsdu[2],
           ot_radio_ctx.tx_param.tx_timing.base_time, ot_radio_ctx.tx_param.tx_timing.delay_time, zb_timer_get_current_time(), iret);
}

/* find the CSL IE of a 802.15.4-2015 frame, NULL when absent */
static lmac154_ie_csl_t *ot_radio_find_csl_ie(uint32_t *frame, uint8_t len, uint8_t mac_hdr_size)
{
    uint16_t fcf = frame[0] & 0xFFFF;

    if (!LMAC154_FRAME_IS_FRAME_2015(fcf) || !LMAC154_FRAME_IS_IE_EXIST(fcf)) {
        return NULL;
    }

    uint8_t *ie_ptr = (uint8_t *)frame + mac_hdr_size;

    /* skip aux security header if present */
    if (fcf & LMAC154_FRAME_SECURITY_MASK) {
        lmac154_aux_hdr_t *aux = (lmac154_aux_hdr_t *)ie_ptr;
        static const uint8_t key_id_len[] = {0, 1, 5, 9};
        uint8_t kid_mode = aux->key_id_mode;
        if (kid_mode > 3) kid_mode = 3;
        ie_ptr += sizeof(lmac154_aux_hdr_t) - sizeof(aux->key_id) + key_id_len[kid_mode];
    }

    while (ie_ptr + sizeof(lmac154_ie_hdr_t) <= (uint8_t *)frame + len) {
        lmac154_ie_hdr_t *hdr = (lmac154_ie_hdr_t *)ie_ptr;
        uint8_t ie_len = hdr->length;

        if (ie_len == 0 || ie_ptr + sizeof(lmac154_ie_hdr_t) + ie_len > (uint8_t *)frame + len) {
            break;
        }

        if (hdr->element_id == LMAC154_IE_CSL_ID && ie_len >= 4) {
            return (lmac154_ie_csl_t *)ie_ptr;
        }

        ie_ptr += sizeof(lmac154_ie_hdr_t) + ie_len;
    }
    return NULL;
}

/* rx ACK_SENT ISR pass: measure where this frame landed against our csl
 * sample time and the last armed window, update the cumulative stats and
 * fill the frame's debug record for the radio task to print. integer ops
 * only, no printing */
static void ot_radio_dbg_rxd_measure(lmac154_receiveInfo_t *info, uint32_t *frame)
{
    uint32_t rx_sfd_sym = info->rx_done_symbol_cnt - ((uint32_t)(info->rx_length + 1) << 1);
    ot_radio_rx_dbg_t *d = NULL;

    if (!utils_dlist_empty(&ot_radio_ctx.frame_list)) {
        /* the frame filled in the RX_DONE pass still sits at the head of
         * the free list until it is queued for OpenThread below */
        d = &((otRadio_rxFrame_t *)ot_radio_ctx.frame_list.next)->dbg;
    }

    if (ot_radio_ctx.tx_param.csl_opt.csl_period) {
        if (d) {
            /* landing values against our csl sample time and the last
             * armed window, plain integer ops only */
            ot_radio_csl_rx_stats_t *st = &ot_radio_csl_rx_stats;
            uint32_t period_sym = (uint32_t)ot_radio_ctx.tx_param.csl_opt.csl_period * 10;
            uint32_t sample_phase = ot_radio_ctx.tx_param.csl_opt.csl_sample_time % period_sym;
            uint32_t rx_phase = rx_sfd_sym % period_sym;
            int32_t rx_drift_sym = (int32_t)(rx_phase - sample_phase);
            int32_t win_edge_start = (int32_t)(rx_sfd_sym - ot_radio_dbg_win_start_sym) << LMAC154_US_PER_SYMBOL_BITS;
            int32_t win_edge_end = (int32_t)(ot_radio_dbg_win_end_sym - rx_sfd_sym) << LMAC154_US_PER_SYMBOL_BITS;
            int32_t drift_us;
            lmac154_ie_csl_t *csl;

            if (rx_drift_sym > (int32_t)(period_sym / 2)) rx_drift_sym -= period_sym;
            if (rx_drift_sym < -(int32_t)(period_sym / 2)) rx_drift_sym += period_sym;
            drift_us = rx_drift_sym << LMAC154_US_PER_SYMBOL_BITS;

            /* in window: SFD inside the armed window and, per the RX_DONE
             * pass, the radio was actually on the window channel (an arm
             * deferred by an ongoing TX leaves the radio on the previous
             * channel until the TX ends) */
            d->valid = 1;
            d->has_csl_ie = 0;
            d->sfd_sym = rx_sfd_sym;
            d->drift_us = drift_us;
            d->edge_start_us = win_edge_start;
            d->edge_end_us = win_edge_end;
            d->in_win = (win_edge_start >= 0) && (win_edge_end >= 0) && d->on_win_ch;

            /* the csl ie of a frame from our transmitter states where the
             * peer thinks our next rx window opens - compare against the
             * real sample time to expose the peer's reference error */
            csl = ot_radio_find_csl_ie(frame, info->rx_length, info->mac_hdr_size);
            if (csl) {
                d->csl_phase = csl->bf.phase;
                d->csl_period = csl->bf.period;
            }
            else {
                d->csl_phase = 0;
                d->csl_period = 0;
            }

            /* enh-ack execution vs its generation-time target and the true
             * advertisement error: the ack ie was generated against
             * est = rx_done + enh_ack_time + shr - 2, its actual sfd is
             * reconstructed from the ack tx-done timestamp (dbg_ack_len is
             * the phr length, fcs included) */
            if (info->dbg_ack_len) {
                uint32_t ack_sfd_sym = (lmac154_get_tx_done_timestamp() >> 4) - ((uint32_t)(info->dbg_ack_len) << 1) - 2;
                uint32_t ack_est_sym = info->rx_done_symbol_cnt + info->enh_ack_time + LMAC154_SHR_LEN - 2;

                d->ack_exec_err_us = (int32_t)(ack_sfd_sym - ack_est_sym) * (int32_t)LMAC154_US_PER_SYMBOL;

                /* advertisement diff vs the sample grid, wrap-safe signed
                 * and folded to +-period/2; data-plane frames only
                 * (short-addr parent, attach-phase ext-addr frames excluded) */
                if (d->ack_phase && d->src_is_saddr) {
                    int32_t diff_sym = (int32_t)(ack_sfd_sym + (uint32_t)d->ack_phase * 10 - ot_radio_ctx.tx_param.csl_opt.csl_sample_time);

                    if (diff_sym > (int32_t)period_sym / 2) diff_sym -= (int32_t)period_sym;
                    else if (diff_sym < -(int32_t)period_sym / 2) diff_sym += (int32_t)period_sym;
                    d->ack_desc_diff_us = diff_sym * (int32_t)LMAC154_US_PER_SYMBOL;
                }
            }

            st->cnt++;
            st->cnt16++;
            st->dev_sum_us += drift_us;
            st->dev_sum16_us += drift_us;
            if (d->in_win) {
                st->in_win++;
                if (st->in_win == 1 || (uint32_t)win_edge_start < st->edge_front_min_us) {
                    st->edge_front_min_us = win_edge_start;
                }
                if (st->in_win == 1 || (uint32_t)win_edge_end < st->edge_end_min_us) {
                    st->edge_end_min_us = win_edge_end;
                }
            }
            if (st->cnt == 1 || drift_us < st->dev_min_us) {
                st->dev_min_us = drift_us;
            }
            if (st->cnt == 1 || drift_us > st->dev_max_us) {
                st->dev_max_us = drift_us;
            }
        }
    }
    else if (d) {
        lmac154_ie_csl_t *csl = ot_radio_find_csl_ie(frame, info->rx_length, info->mac_hdr_size);
        if (csl) {
            d->valid = 1;
            d->has_csl_ie = 1;
            d->in_win = 0;
            d->sfd_sym = rx_sfd_sym;
            d->csl_phase = csl->bf.phase;
            d->csl_period = csl->bf.period;
        }
    }
    info->dbg_csl_ack_phase = 0;
}
#endif

void ot_radioTask(ot_system_event_t trxEvent)
{
    otRadio_rxFrame_t   *pframe;
    otRadioFrame        *txframe = ot_radio_ctx.tx_frame;
    uint32_t            tag;

    if (!(trxEvent & OT_SYSTEM_EVENT_RADIO_ALL_MASK)) {
        return;
    }

    if ((trxEvent & OT_SYSTEM_EVENT_RADIO_TX_ALL_MASK) && txframe) {

        ot_radio_ctx.tx_frame = NULL;

#if OT_RADIO_CSL_DEBUG
        /* print the tx timing record captured by the tx-done ISR; done here,
         * in task context, before handing the event to OpenThread */
        ot_radio_dbg_print_tx();
#endif

        if (txframe) {
            if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ) {
                otPlatRadioTxDone(ot_radio_ctx.instance, txframe, NULL, OT_ERROR_NONE);
            }
            else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_ACKED) {
                otPlatRadioTxDone(ot_radio_ctx.instance, txframe, ot_radio_ctx.rx_ack_frame, OT_ERROR_NONE);
            }
            else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_NO_ACK) {
                otPlatRadioTxDone(ot_radio_ctx.instance, txframe, NULL, OT_ERROR_NO_ACK);
            }
            else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_ABORT) {
                otPlatRadioTxDone(ot_radio_ctx.instance, txframe, NULL, OT_ERROR_ABORT);
            }
            else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_ERROR) {
                otPlatRadioTxDone(ot_radio_ctx.instance, txframe, NULL, OT_ERROR_CHANNEL_ACCESS_FAILURE);
            }
        }
    }

    if (trxEvent & OT_SYSTEM_EVENT_RADIO_RX_DONE ) {

        pframe = NULL;
        tag = otrEnterCrit();
        if (!utils_dlist_empty(&ot_radio_ctx.rx_frame_list)) {
            pframe = (otRadio_rxFrame_t *)ot_radio_ctx.rx_frame_list.next;
            utils_dlist_del(&pframe->dlist);
        }
        otrExitCrit(tag);

        if (pframe) {
#if OT_RADIO_CSL_DEBUG
            /* print the rx landing record captured by the rx-done ISR; done
             * here, in task context, before handing the frame to OpenThread */
            ot_radio_dbg_print_rx(pframe);
#endif
            otPlatRadioReceiveDone(ot_radio_ctx.instance, &pframe->frame, OT_ERROR_NONE);

            tag = otrEnterCrit();
            pframe->frame.mLength = 0;
            utils_dlist_add_tail(&pframe->dlist, &ot_radio_ctx.frame_list);
            otrExitCrit(tag);

            if (!utils_dlist_empty(&ot_radio_ctx.rx_frame_list)) {
                /* post OT_SYSTEM_EVENT_RADIO_RX_DONE to handle next pending packet */
                otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_RX_DONE);
            }
        }
    }
    else if (trxEvent & OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF) {
        otPlatRadioReceiveDone(ot_radio_ctx.instance, NULL, OT_ERROR_NO_BUFS);
    }
}

bool otr_isStackIdle(void)
{
    bool is_trx_frames_empty = false;
    otRadioState radioState = OT_RADIO_STATE_DISABLED;

    if (ot_radio_ctx.instance == NULL) {
        return true;
    }

    uint32_t tag = otrEnterCrit();
    is_trx_frames_empty = (utils_dlist_empty(&ot_radio_ctx.rx_frame_list)) && (ot_radio_ctx.tx_frame == NULL);
    if (otThreadGetDeviceRole(ot_radio_ctx.instance) != OT_DEVICE_ROLE_DISABLED) {
        radioState = otPlatRadioGetState(ot_radio_ctx.instance);
    }
    otrExitCrit(tag);

    if (is_trx_frames_empty && (radioState == OT_RADIO_STATE_DISABLED || radioState == OT_RADIO_STATE_SLEEP)) {
        return true;
    }

    return false;
}

/************************************ LMAC 15.4 event function ***********************************/
void ot_radioTxDoneCallback (lmac154_tx_status_t tx_status, uint32_t * ack_frame, uint32_t ack_frame_len)
{
    if (ot_radio_ctx.tx_frame) {
        if (LMAC154_TX_STATUS_TX_FINISHED == tx_status) {
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ);
        }
        else if (LMAC154_TX_STATUS_TX_ABORTED == tx_status) {
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_ABORT);
        }
        else if (LMAC154_TX_STATUS_NO_ACK == tx_status) {
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_NO_ACK);
        }
        else if (LMAC154_TX_STATUS_ACKED == tx_status) {
            ot_radio_ctx.rx_ack_frame->mInfo.mRxInfo.mTimestamp = ot_radio_ctx.tx_param.ack_rx_done_symbol_cnt - ((ack_frame_len + 1) << 1);
            ot_radio_ctx.rx_ack_frame->mInfo.mRxInfo.mTimestamp = ot_radio_ctx.rx_ack_frame->mInfo.mRxInfo.mTimestamp << LMAC154_US_PER_SYMBOL_BITS;
            ot_radio_ctx.rx_ack_frame->mChannel = lmac154_getChannel() + OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;
            ot_radio_ctx.rx_ack_frame->mInfo.mRxInfo.mRssi = lmac154_getRSSI();
            ot_radio_ctx.rx_ack_frame->mInfo.mRxInfo.mLqi = lmac154_getLQI();

            arch_memcpy(ot_radio_ctx.rx_ack_frame->mPsdu, ack_frame, ack_frame_len);
            ot_radio_ctx.rx_ack_frame->mLength = ack_frame_len;

#if OT_RADIO_CSL_DEBUG
            {
                /* child's next-window anchor from this enh-ack: the ack sfd
                 * is measured by our own rx timestamp, the phase comes from
                 * the ie the ack carried */
                lmac154_ie_csl_t *csl = ot_radio_find_csl_ie((uint32_t *)ack_frame, ack_frame_len - 2,
                        (uint8_t)lmac154_get_mhr_len_2015((uint8_t *)ack_frame));

                if (csl) {
                    ot_radio_dbg_ackref_sym = (ot_radio_ctx.tx_param.ack_rx_done_symbol_cnt - ((ack_frame_len + 1) << 1)) +
                                              (uint32_t)csl->bf.phase * 10;
                    ot_radio_dbg_ackref_valid = 1;
                }
            }
#endif

            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_ACKED);
        }
        else {
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_ERROR);
        }

#if OT_RADIO_CSL_DEBUG
        if (tx_status == LMAC154_TX_STATUS_ACKED || tx_status == LMAC154_TX_STATUS_NO_ACK) {
            ot_radio_dbg_txd_capture(tx_status);
        }
#endif
    }
}

static inline uint32_t ot_radio_prepare_enh_ack(lmac154_receiveInfo_t *rx_info, uint32_t *rx_buf, 
                                                int8_t rssi, int8_t lqi, uint8_t ** p_ack)
{
    struct otMacKeyMaterial *pkey = NULL;
    lmac154_aux_hdr_t *p_rx_aux = NULL;
    lmac154_aux_hdr_t *p_ack_aux = NULL;
    uint8_t *rx_ptr = (uint8_t *)rx_buf;
    uint16_t fcf = rx_buf[0];
    uint8_t *ack_ptr = otRadio_ack_buff + MAX_BUFFER_SIZE_ACK;
    uint32_t ack_len;
    int ret = -1;

    *(uint32_t *)ack_ptr = 0x2002;
    ack_ptr[2] = rx_ptr[2];
    ack_len = 3;

    ack_ptr[ack_len] = rx_ptr[ack_len];
    ack_ptr[ack_len + 1] = rx_ptr[ack_len + 1];
    ack_len += 2;

    if (rx_info->is_src_saddr) {
        ack_ptr[ack_len] = rx_info->src_addr.addr[0];
        ack_ptr[ack_len + 1] = rx_info->src_addr.addr[1];
        ack_len += 2;

        (*(uint32_t *)ack_ptr) = (*(uint32_t *)ack_ptr) | LMAC154_FRAME_ADDR_DEST_SHORT;
    } else if (rx_info->is_src_xaddr) {
        ack_ptr[ack_len] = rx_info->src_addr.addr[0];
        ack_ptr[ack_len + 1] = rx_info->src_addr.addr[1];
        ack_ptr[ack_len + 2] = rx_info->src_addr.addr[2];
        ack_ptr[ack_len + 3] = rx_info->src_addr.addr[3];
        ack_ptr[ack_len + 4] = rx_info->src_addr.addr[4];
        ack_ptr[ack_len + 5] = rx_info->src_addr.addr[5];
        ack_ptr[ack_len + 6] = rx_info->src_addr.addr[6];
        ack_ptr[ack_len + 7] = rx_info->src_addr.addr[7];
        ack_len += 8;
        (*(uint32_t *)ack_ptr) = (*(uint32_t *)ack_ptr) | LMAC154_FRAME_ADDR_DEST_EXT;
    }

    if (fcf & LMAC154_FRAME_SECURITY_MASK) {
        p_rx_aux = (lmac154_aux_hdr_t *)(rx_ptr + rx_info->mac_hdr_size);
        if (p_rx_aux->sec_lvl) {
            uint32_t target_id = p_rx_aux->key_id[0];
            uint32_t rel_idx = target_id - ot_radio_ctx.mac_key_id + 1;

            if (rel_idx > 2)
                return 0;

            *(uint32_t *)ack_ptr = *((uint32_t *)ack_ptr) | LMAC154_FRAME_SECURITY_MASK;

            pkey = &ot_radio_ctx.keys[target_id & 3];
            p_ack_aux = (lmac154_aux_hdr_t *)(ack_ptr + ack_len);

            *(uint8_t *) p_ack_aux = *(uint8_t *) p_rx_aux;
            p_ack_aux->key_id[0] = target_id;
            ack_len += 6;
        }
    }

    if (rx_info->is_frame_pended)
        ack_ptr[0] |= LMAC154_FRAME_FRAME_PENDING_MASK;

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    uint8_t *p = ack_ptr + ack_len;
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (ot_radio_ctx.tx_param.csl_opt.csl_period > 0) {
        /* the hw fires the ack trigger 2 sym ahead of the programmed time
         * (measured: ack sfd = rx_done + enh_ack_time + shr - 2) */
        uint32_t tx_sfd_sym = rx_info->rx_done_symbol_cnt + rx_info->enh_ack_time + LMAC154_SHR_LEN - 2;
        rx_info->dbg_csl_ack_phase = lmac154_gen_csl_ie(ot_radio_ctx.tx_param.csl_opt.csl_period, ot_radio_ctx.tx_param.csl_opt.csl_sample_time, (lmac154_ie_csl_t *)p, tx_sfd_sym);
        p += sizeof(lmac154_ie_csl_t);
    }
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    p += ot_link_metrics_gen_enh_ack_data(rx_info, rssi, lqi, p);
#endif
    if (p != (ack_ptr + ack_len)) {
        *(uint32_t *)ack_ptr = *((uint32_t *)ack_ptr) | LMAC154_FRAME_IE_MASK;
        ack_len = p - ack_ptr;
    }
#endif

    if (pkey && p_ack_aux) {
        p_ack_aux->frame_cnt[0] = ot_radio_ctx.frame_cnt.bytes[0];
        p_ack_aux->frame_cnt[1] = ot_radio_ctx.frame_cnt.bytes[1];
        p_ack_aux->frame_cnt[2] = ot_radio_ctx.frame_cnt.bytes[2];
        p_ack_aux->frame_cnt[3] = ot_radio_ctx.frame_cnt.bytes[3];

        ot_radio_ctx.enh_ack_sec.a_data_length = ack_len;
        ot_radio_ctx.enh_ack_sec.m_data_length = 0;
        ot_radio_ctx.enh_ack_sec.m_data = NULL;
        ot_radio_ctx.enh_ack_sec.key = (uint32_t *)pkey->mKeyMaterial.mKey.m8;
        ot_radio_ctx.enh_ack_sec.nonce.frame_cnt = SWAP32_TO_BIG_ENDIAN(ot_radio_ctx.frame_cnt.value);
        ot_radio_ctx.enh_ack_sec.nonce.security_level = p_ack_aux->sec_lvl;
        ot_radio_ctx.frame_cnt.value ++;

#if defined (BOUFFALO_SDK) && defined (BL616)
        bflb_l1c_dcache_clean_invalidate_range(otRadio_ack_buff, sizeof(otRadio_ack_buff));
        ot_radio_ctx.enh_ack_sec.a_data = (uint32_t *)bflb_get_no_cache_addr(ack_ptr);
        ot_radio_ctx.enh_ack_sec.c_data = (uint32_t *)bflb_get_no_cache_addr(otRadio_ack_buff);
#else
        ot_radio_ctx.enh_ack_sec.a_data = (uint32_t *)ack_ptr;
        ot_radio_ctx.enh_ack_sec.c_data = (uint32_t *)otRadio_ack_buff;
#endif
        ret = lmac154_encryptPacket(&ot_radio_ctx.enh_ack_sec);
        if (ret < 0)
            return 0;

        uint8_t * a_data = (uint8_t*)ot_radio_ctx.enh_ack_sec.a_data + ack_len;
        uint8_t * c_data = (uint8_t*)ot_radio_ctx.enh_ack_sec.c_data;
        a_data[0] = c_data[0];
        a_data[1] = c_data[1];
        a_data[2] = c_data[2];
        a_data[3] = c_data[3];
        ack_len = ack_len + ret;
    }

    *p_ack = ack_ptr;

    return ack_len;
}


void ot_radioRxDoneCallback(lmac154_rx_status_t status, lmac154_receiveInfo_t *info, uint32_t *frame)
{
    otRadio_rxFrame_t *     rx_frame = NULL;
    int8_t                  lqi, rssi;
    uint8_t *               p_ack = NULL;
    int                     ack_len = 0;

    if (LMAC154_RX_STATUS_ACK_ERR == status) {
        return;
    }

    /* A frame received while transmitting carries no frame payload. */
    if (LMAC154_RX_STATUS_ERR_RX_IN_TX == status || frame == NULL) {
        return;
    }

    if (LMAC154_RX_STATUS_RX_DONE == status) {
        if (info->rx_error) {
#if OT_RADIO_CSL_DEBUG
            if (ot_radio_ctx.tx_param.csl_opt.csl_period) {
                ot_radio_csl_rx_stats.err++;
            }
#endif
            return;
        }

        lqi = lmac154_getLQI();
        rssi = lmac154_getRSSI();
        ack_len = 0;
        if (info->is_enh_ack_requested) {
            ack_len = ot_radio_prepare_enh_ack(info, frame, lqi, rssi, &p_ack);
            if (ack_len <= 2 || lmac154_writeTxAckFrame(p_ack, ack_len) < 0) {
                return;
            }
            info->dbg_ack_len = ack_len + 2;
        }

        if (utils_dlist_empty(&ot_radio_ctx.frame_list)) {
            return;
        }

        rx_frame = (otRadio_rxFrame_t *)ot_radio_ctx.frame_list.next;

        arch_memcpy(rx_frame->frame.mPsdu, frame, info->rx_length);

        rx_frame->frame.mInfo.mRxInfo.mTimestamp = info->rx_done_symbol_cnt - ((uint32_t)(info->rx_length + 1) << 1);
        rx_frame->frame.mInfo.mRxInfo.mTimestamp = rx_frame->frame.mInfo.mRxInfo.mTimestamp << LMAC154_US_PER_SYMBOL_BITS;

        rx_frame->frame.mInfo.mRxInfo.mRssi = rssi;
        rx_frame->frame.mInfo.mRxInfo.mLqi = lqi;
        rx_frame->frame.mChannel = lmac154_getChannel() + OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;
        rx_frame->frame.mLength = info->rx_length;
        rx_frame->frame.mInfo.mRxInfo.mAckedWithFramePending = info->is_frame_pended;
        rx_frame->frame.mInfo.mRxInfo.mAckedWithSecEnhAck =
            info->is_enh_ack_requested && (frame[0] & LMAC154_FRAME_SECURITY_MASK);

#if OT_RADIO_CSL_DEBUG
        ot_radio_dbg_rxd_capture(rx_frame, info, frame);
#endif
    }

    if (LMAC154_RX_STATUS_ACK_SENT == status || !info->is_tx_acking) {
#if OT_RADIO_CSL_DEBUG
        ot_radio_dbg_rxd_measure(info, frame);
#endif

        if (utils_dlist_empty(&ot_radio_ctx.frame_list)) {
            return;
        }

        rx_frame = (otRadio_rxFrame_t *)ot_radio_ctx.frame_list.next;
        if (rx_frame->frame.mLength) {
            utils_dlist_del(&rx_frame->dlist);
            utils_dlist_add_tail(&rx_frame->dlist, &ot_radio_ctx.rx_frame_list);
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_RX_DONE);
        }
    }
}

/*************************** Openthread radio interface implementation ***************************/
otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    static otRadioFrame tx_radio_frame;

    memset(&tx_radio_frame, 0, sizeof(otRadioFrame));
    tx_radio_frame.mPsdu = otRadio_tx_buff;
    tx_radio_frame.mInfo.mTxInfo.mIeInfo = &ot_radio_ctx.tx_ie_info;

    return &tx_radio_frame;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    int                 iret = 0;
    uint32_t            tag;
    uint32_t            mhr_len = 0;
    uint32_t            aux_len = 0;
    uint32_t            ie_len = 0;
    lmac154_aux_hdr_t * p_aux = NULL;
    uint8_t *           a_data = otRadio_tx_buff + MAX_SIZE_PSDU;
    uint8_t *           m_data = otRadio_tx_buff + MAX_SIZE_PSDU + MAX_BUFFER_SIZE_A_DATA;
    uint16_t            fcf;

    if (aFrame == NULL || ot_radio_ctx.tx_frame) {
        return OT_ERROR_INVALID_ARGS;
    }

    ot_radio_ctx.tx_param.is_base_param = false;
    ot_radio_ctx.tx_param.is_tx_security = 0;
    ot_radio_ctx.tx_param.tx_timing.base_time = 0;
    ot_radio_ctx.tx_param.tx_timing.delay_time = 0;
    ot_radio_ctx.tx_param.csl_opt.csl_ie_offset = 0;
#if OT_RADIO_CSL_DEBUG
    ot_radio_ctx.tx_param.csl_opt.dbg_csl_tx_phase = 0;
#endif
#if CONFIG_LMAC154_DBG
    ot_radio_ctx.tx_param.dbg_plat_entry_sym = zb_timer_get_current_time();
#endif

    fcf = *(uint16_t *)aFrame->mPsdu;
    if (LMAC154_FRAME_IS_FRAME_2015(fcf)) {
        mhr_len = lmac154_get_mhr_len_2015(aFrame->mPsdu);
    }
    else {
        mhr_len = lmac154_get_mhr_len(aFrame->mPsdu);
    }

    ot_radio_ctx.tx_param.pkt = (uint32_t *)aFrame->mPsdu;
    ot_radio_ctx.tx_param.pkt_length = aFrame->mLength - 2;
    ot_radio_ctx.tx_param.tx_channel = aFrame->mChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;
    ot_radio_ctx.tx_param.resume_channel = ot_radio_ctx.tx_param.tx_channel;
    ot_radio_ctx.tx_param.csma_ca_max_backoff = aFrame->mInfo.mTxInfo.mMaxCsmaBackoffs;
    ot_radio_ctx.tx_param.is_cca = aFrame->mInfo.mTxInfo.mCsmaCaEnabled;
    ot_radio_ctx.tx_param.tx_done_cb = ot_radioTxDoneCallback;
    ot_radio_ctx.tx_param.next = NULL;

    if (LMAC154_FRAME_IS_NORMAL_SECURITY(ot_radio_ctx.tx_param.pkt[0]) && 
        !aFrame->mInfo.mTxInfo.mIsSecurityProcessed) {

        aFrame->mInfo.mTxInfo.mAesKey = NULL;
        aux_len = lmac154_parse_aux_hdr(aFrame->mPsdu, mhr_len, &p_aux);
        if (aux_len > 0 && p_aux) {
            if ((uint8_t)(p_aux->key_id[0] - ot_radio_ctx.mac_key_id + 1) > 2) {
                return OT_ERROR_INVALID_ARGS;
            }

            p_aux->frame_cnt[0] = ot_radio_ctx.frame_cnt.bytes[0];
            p_aux->frame_cnt[1] = ot_radio_ctx.frame_cnt.bytes[1];
            p_aux->frame_cnt[2] = ot_radio_ctx.frame_cnt.bytes[2];
            p_aux->frame_cnt[3] = ot_radio_ctx.frame_cnt.bytes[3];
            ot_radio_ctx.tx_param.sec.nonce.security_level = p_aux->sec_lvl;
            ot_radio_ctx.tx_param.sec.nonce.frame_cnt = SWAP32_TO_BIG_ENDIAN(ot_radio_ctx.frame_cnt.value);
            ot_radio_ctx.frame_cnt.value ++;
            ot_radio_ctx.tx_param.sec.key = (uint32_t *)ot_radio_ctx.keys[p_aux->key_id[0] & 3].mKeyMaterial.mKey.m8;
            ot_radio_ctx.tx_param.pkt_length = ot_radio_ctx.tx_param.pkt_length - (4 << ((ot_radio_ctx.tx_param.sec.nonce.security_level & 3) - 1));
            ot_radio_ctx.tx_param.is_tx_security = true;
        }
    }

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (ot_radio_ctx.tx_param.csl_opt.csl_period > 0 && !aFrame->mInfo.mTxInfo.mIsHeaderUpdated) {
        uint8_t *ie_start = (uint8_t *)ot_radio_ctx.tx_param.pkt + mhr_len + aux_len;
        ie_len = ot_radio_ctx.tx_param.pkt_length - mhr_len - aux_len;
        lmac154_ie_hdr_t *csl_ie = NULL;

        ie_len = lmac154_traverse_ie(ie_start, ie_len, LMAC154_IE_CSL_ID, &csl_ie);
        if (ie_len > 0 && csl_ie) {
            ot_radio_ctx.tx_param.csl_opt.csl_ie_offset = (uint32_t)csl_ie - (uint32_t)(ot_radio_ctx.tx_param.pkt);
            aFrame->mInfo.mTxInfo.mIsHeaderUpdated = true;
        }
    }
    else if (ot_radio_ctx.tx_param.csl_opt.csl_period > 0) {
        /* ot built the ie itself - the lmac will not regenerate it, so no
         * fresh sfd estimate exists for the ie2sfd statistic */
        ot_radio_ctx.tx_param.csl_opt.dbg_sfd_est_sym = 0;
    }
#endif
    if (ot_radio_ctx.tx_param.is_tx_security) {
        aFrame->mInfo.mTxInfo.mIsHeaderUpdated = true;
        ot_radio_ctx.tx_param.sec.a_data_length = mhr_len + aux_len + ie_len;
        if (LMAC154_FRAME_IS_CMD(fcf) && !LMAC154_FRAME_IS_FRAME_2015(fcf)) {
            /** command id is is a_data in 2003/2006 */
            ot_radio_ctx.tx_param.sec.a_data_length += 1;
        }
        ot_radio_ctx.tx_param.sec.m_data_length = ot_radio_ctx.tx_param.pkt_length - ot_radio_ctx.tx_param.sec.a_data_length;

#if defined (BOUFFALO_SDK) && defined (BL616)
        bflb_l1c_dcache_clean_invalidate_range(otRadio_tx_buff, sizeof(otRadio_tx_buff));
        ot_radio_ctx.tx_param.sec.a_data = (uint32_t *)bflb_get_no_cache_addr(a_data);
        ot_radio_ctx.tx_param.sec.m_data = (uint32_t *)bflb_get_no_cache_addr(m_data);
        ot_radio_ctx.tx_param.sec.c_data = (uint32_t *)bflb_get_no_cache_addr(aFrame->mPsdu);
#else
        ot_radio_ctx.tx_param.sec.a_data = (uint32_t *)a_data;
        ot_radio_ctx.tx_param.sec.m_data = (uint32_t *)m_data;
        ot_radio_ctx.tx_param.sec.c_data = (uint32_t *)aFrame->mPsdu;
#endif
        ot_radio_ctx.tx_param.pkt = ot_radio_ctx.tx_param.sec.c_data;

        arch_memcpy(ot_radio_ctx.tx_param.sec.a_data, 
                    ot_radio_ctx.tx_param.pkt, 
                    ot_radio_ctx.tx_param.sec.a_data_length);
        arch_memcpy(ot_radio_ctx.tx_param.sec.m_data, 
                    (uint8_t*)ot_radio_ctx.tx_param.pkt + ot_radio_ctx.tx_param.sec.a_data_length, 
                    ot_radio_ctx.tx_param.sec.m_data_length);

        aFrame->mInfo.mTxInfo.mIsSecurityProcessed = true;
    }

    if (ot_radio_ctx.tx_param.csl_opt.csl_ie_offset && (ot_radio_ctx.tx_param.tx_timing.base_time || ot_radio_ctx.tx_param.tx_timing.delay_time)) {
        return OT_ERROR_INVALID_ARGS;
    }

    ot_radio_ctx.tx_frame = aFrame;

    tag = otrEnterCrit();

    if (aFrame->mInfo.mTxInfo.mTxDelayBaseTime || aFrame->mInfo.mTxInfo.mTxDelay) {

        uint32_t base_symbol = zb_timer_get_current_time();

        ot_radio_ctx.tx_param.tx_timing.delay_time = (aFrame->mInfo.mTxInfo.mTxDelay >> ZB_TIMER_BITNUM_PER_TICK) -
                                                     ((uint32_t)(base_symbol & 0xfffffff) -
                                                     ((uint32_t)(aFrame->mInfo.mTxInfo.mTxDelayBaseTime >> ZB_TIMER_BITNUM_PER_TICK)));
        ot_radio_ctx.tx_param.tx_timing.base_time = base_symbol;

        if (ot_radio_ctx.tx_param.tx_timing.delay_time > ((OPENTHREAD_CONFIG_MAC_CSL_REQUEST_AHEAD_US >> ZB_TIMER_BITNUM_PER_TICK) * 2)) {
            iret = -255;
        }

        int rf_state = lmac154_getRFState();
        if (rf_state == LMAC154_RF_STATE_RX_DOING) {
            iret = -254;
        }
        if (rf_state == LMAC154_RF_STATE_ACK_DOING) {
            iret = -253;
        }
    }

    if (iret >= 0) {
        iret = lmac154_triggerParamTxExt(&ot_radio_ctx.tx_param);
    }
    otrExitCrit(tag);
#if OT_RADIO_CSL_DEBUG
    /* only csl-delayed (scheduled) transmissions: plain frames would print
     * for every packet this device sends */
    if (aFrame->mInfo.mTxInfo.mTxDelayBaseTime || aFrame->mInfo.mTxInfo.mTxDelay) {
        ot_radio_dbg_print_send(aFrame, iret);
    }
#endif

    if (iret >= 0) {
        otPlatRadioTxStarted(aInstance, aFrame);
    }
    else {
        otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_ERROR);
    }

    return OT_ERROR_NONE;
}

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    return OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_CSMA_BACKOFF | OT_RADIO_CAPS_SLEEP_TO_TX | OT_RADIO_CAPS_TRANSMIT_TIMING | OT_RADIO_CAPS_TRANSMIT_SEC | OT_RADIO_CAPS_RECEIVE_TIMING;
}

const char *otPlatRadioGetVersionString(otInstance *aInstance) 
{
#define OT_VER_STRING_HELPER(x, y, z) #x "." #y "." #z
#define OT_VER_STRING(x, y, z) OT_VER_STRING_HELPER(x, y, z)

#ifdef VERSION_OT_SRC_EXTRA_INFO
    return OT_VER_STRING(VERSION_OT_SRC_MAJOR, VERSION_OT_SRC_MINOR, VERSION_OT_SRC_PATCH) 
        "_" VERSION_OT_SRC_EXTRA_INFO; 
#else
    return OT_VER_STRING(VERSION_OT_SRC_MAJOR, VERSION_OT_SRC_MINOR, VERSION_OT_SRC_PATCH);
#endif
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance) 
{
    return IEEE802_15_4_RADIO_RECEIVE_SENSITIVITY;
}

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64) 
{
#ifdef BOUFFALO_SDK
    int i;
    uint32_t flash_id = 0;

    memset(aIeeeEui64, 0, 8);
    for (i = MAC_ADDRESS_MAX_NUM - 1; i >= 0; i --) {
        if (!bflb_efuse_is_mac_address_slot_empty(i, 0)) {
            bflb_efuse_read_mac_address_opt(i, aIeeeEui64, 0);
            return;
        }
    }

    aIeeeEui64[0] = 0x8C;
    aIeeeEui64[1] = 0xFD;
    aIeeeEui64[2] = 0xF0;
    flash_id = bflb_flash_get_jedec_id();
    aIeeeEui64[3] = (flash_id >> 24) & 0xff;
    aIeeeEui64[4] = (flash_id >> 16) & 0xff;
    aIeeeEui64[5] = (flash_id >> 8) & 0xff;
    aIeeeEui64[6] = flash_id & 0xff;
#else
    uint8_t temp;

    bl_wireless_mac_addr_get(aIeeeEui64);

    for (int i = 0; i < OT_EXT_ADDRESS_SIZE / 2; i ++) {
        temp = aIeeeEui64[OT_EXT_ADDRESS_SIZE - i - 1];
        aIeeeEui64[OT_EXT_ADDRESS_SIZE - i - 1] = aIeeeEui64[i];
        aIeeeEui64[i] = temp;
    }

    if (aIeeeEui64[OT_EXT_ADDRESS_SIZE - 1] == 0 && aIeeeEui64[OT_EXT_ADDRESS_SIZE - 2] == 0) {
        aIeeeEui64[OT_EXT_ADDRESS_SIZE - 1] = aIeeeEui64[OT_EXT_ADDRESS_SIZE - 3];
        aIeeeEui64[OT_EXT_ADDRESS_SIZE - 2] = aIeeeEui64[OT_EXT_ADDRESS_SIZE - 4];
        aIeeeEui64[OT_EXT_ADDRESS_SIZE - 3] = aIeeeEui64[OT_EXT_ADDRESS_SIZE - 5];
    }
#endif
}

void otPlatRadioSetPanId(otInstance *aInstance, otPanId aPanId) 
{
    lmac154_setPanId(aPanId);
}
void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    lmac154_setLongAddr((uint8_t *) aExtAddress->m8);
    uint8_t reversed_addr[8];
    for (int i = 0; i < 8; i++) {
        reversed_addr[i] = aExtAddress->m8[7 - i];
    }
    arch_memcpy(ot_radio_ctx.tx_param.sec.nonce.ext_addr, reversed_addr, 8);
    arch_memcpy(ot_radio_ctx.enh_ack_sec.nonce.ext_addr, reversed_addr, 8);
}

void otPlatRadioSetShortAddress(otInstance *aInstance, otShortAddress aShortAddress) 
{
    lmac154_setShortAddr(aShortAddress);
}

otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower) 
{
    *aPower = lmac154_getTxPower();
    return OT_ERROR_NONE;
}
otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower) 
{
    lmac154_setTxPower(aPower);
    return OT_ERROR_NONE;
}

otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold) 
{
    *aThreshold = lmac154_getEDThreshold();
    return OT_ERROR_NONE;
}
otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold) 
{
    lmac154_setEDThreshold(aThreshold);
    return OT_ERROR_NONE;
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance) 
{
    return lmac154_isRxPromiscuousModeEnabled() == 1;
}
void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable) 
{
    if (aEnable) {
        lmac154_enableRxPromiscuousMode(true, false);
    }
    else {
        lmac154_disableRxPromiscuousMode();
    }
}

void otPlatRadioSetMacKey(otInstance *aInstance, uint8_t aKeyIdMode, uint8_t aKeyId,
                          const otMacKeyMaterial *aPrev, const otMacKeyMaterial *aCurr,
                          const otMacKeyMaterial *aNext, otRadioKeyType aKeyType)
{
    (void)aInstance; (void)aKeyIdMode; (void)aKeyType;

    if (aPrev && aCurr && aNext) {
        uint32_t tag = otrEnterCrit();

        ot_radio_ctx.mac_key_id = aKeyId;

        arch_memcpy(&ot_radio_ctx.keys[(uint8_t)(aKeyId - 1) & 3], aPrev, sizeof(otMacKeyMaterial));
        arch_memcpy(&ot_radio_ctx.keys[(uint8_t)(aKeyId)     & 3], aCurr, sizeof(otMacKeyMaterial));
        arch_memcpy(&ot_radio_ctx.keys[(uint8_t)(aKeyId + 1) & 3], aNext, sizeof(otMacKeyMaterial));

        otrExitCrit(tag);
    }
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    OT_UNUSED_VARIABLE(aInstance);

    uint32_t tag = otrEnterCrit();
    ot_radio_ctx.frame_cnt.value = aMacFrameCounter;
    otrExitCrit(tag);
}

otError otPlatRadioEnable(otInstance *aInstance)
{
    lmac154_enable1stStack();
    lmac154_registerEventCallback(LMAC154_STACK_1, ot_radioRxDoneCallback);

    return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance)
{
    lmac154_disable1stStack();
    lmac154_registerEventCallback(LMAC154_STACK_1, NULL);

    return OT_ERROR_NONE;
}

otRadioState otPlatRadioGetState(otInstance *aInstance) 
{
    if(lmac154_isDisabled()) {
        return OT_RADIO_STATE_DISABLED;
    }

    if (ot_radio_ctx.tx_frame) {
        return OT_RADIO_STATE_TRANSMIT;
    }

    switch (lmac154_getRFState())
    {
        case LMAC154_RF_STATE_RX_TRIG:
        case LMAC154_RF_STATE_RX:
        case LMAC154_RF_STATE_RX_DOING:
        case LMAC154_RF_STATE_ACK_DOING:
            return OT_RADIO_STATE_RECEIVE;
        case LMAC154_RF_STATE_TX:
        case LMAC154_RF_STATE_CSMA:
            return OT_RADIO_STATE_TRANSMIT;
        case LMAC154_RF_STATE_IDLE:
            return OT_RADIO_STATE_SLEEP;
    }

    return OT_RADIO_STATE_INVALID;
}

bool otPlatRadioIsEnabled(otInstance *aInstance) 
{
    return !lmac154_isDisabled();
}

otError otPlatRadioReceiveAt(otInstance *aInstance, uint8_t aChannel,
                             uint32_t aStart, uint32_t aDuration)
{
    uint32_t now_us;
    int32_t delta_end_us;
    int ret;
#if OT_RADIO_CSL_DEBUG
    /* sample before arming: lmac154_enableRxTimed turns the radio on and
     * would always report rx afterwards. the entry state explains how the
     * arm resolved (idle: immediate, tx/csma: deferred, rx: window overlay) */
    int rf_state = (int)lmac154_getRFState();
#endif

    OT_UNUSED_VARIABLE(aInstance);

    now_us = zb_timer_get_current_time() << ZB_TIMER_BITNUM_PER_TICK;
    delta_end_us = (int32_t)(aStart + aDuration - now_us);
    if (delta_end_us < 0) {
#if OT_RADIO_CSL_DEBUG
        if (ot_radio_ctx.tx_param.csl_opt.csl_period) {
            printf("csl rx win late: %ld ch%d start %lu dur %lu now %lu rf %s\r\n",
                    zb_timer_get_current_time(), aChannel, aStart, aDuration, now_us,
                    ot_radio_dbg_rf_state(rf_state));
        }
#endif
        return OT_ERROR_FAILED; /* window end already passed */
    }

    uint32_t tag = otrEnterCrit();
    ret = lmac154_enableRxTimed(delta_end_us >> ZB_TIMER_BITNUM_PER_TICK,
                                aChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN);
    otrExitCrit(tag);

#if OT_RADIO_CSL_DEBUG
    if (ot_radio_ctx.tx_param.csl_opt.csl_period) {
        if (ret == 0) {
            ot_radio_dbg_win_start_sym = aStart >> ZB_TIMER_BITNUM_PER_TICK;
            ot_radio_dbg_win_end_sym = (aStart + aDuration) >> ZB_TIMER_BITNUM_PER_TICK;
            ot_radio_dbg_win_channel = aChannel;
        }
        printf("csl rx win: %ld ch%d start %lu dur %lu now %lu ret %d rf %s\r\n",
                zb_timer_get_current_time(), aChannel, aStart, aDuration, now_us, ret,
                ot_radio_dbg_rf_state(rf_state));
    }
#endif

    return (ret == 0) ? OT_ERROR_NONE : OT_ERROR_FAILED;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    uint8_t ch = aChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    uint32_t tag = otrEnterCrit();
    lmac154_disableRxTimed(ch);
    otrExitCrit(tag);
#endif

#if (OPENTHREAD_FTD) || (OPENTHREAD_MTD)
    if (lmac154_isRxStateWhenIdle() != otThreadGetLinkMode(aInstance).mRxOnWhenIdle) {
        lmac154_setRxStateWhenIdle(otThreadGetLinkMode(aInstance).mRxOnWhenIdle);
    }
#endif

    if (lmac154_getChannel() != ch) {
        lmac154_disableRx();
        lmac154_setChannel(ch);
    }
    lmac154_enableRx();

    return OT_ERROR_NONE;
}

otError otPlatRadioSleep(otInstance *aInstance)
{
    bool is_rx_on_when_idle = false;

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    uint32_t tag = otrEnterCrit();
    lmac154_disableRxTimed(lmac154_getChannel());
    otrExitCrit(tag);
#endif

#if (OPENTHREAD_FTD) || (OPENTHREAD_MTD)
    is_rx_on_when_idle = otThreadGetLinkMode(aInstance).mRxOnWhenIdle;
#endif

    lmac154_setRxStateWhenIdle(is_rx_on_when_idle);
    lmac154_disableRx();

    return OT_ERROR_NONE;
}

void otPlatRadioSetRxOnWhenIdle(otInstance *aInstance, bool aEnable)
{
    lmac154_setRxStateWhenIdle(aEnable);
}

int8_t otPlatRadioGetRssi(otInstance *aInstance) 
{
    int rssi = 127;

    lmac154_runCCA(&rssi);

    return (int8_t)rssi;
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    return OT_ERROR_NONE;
}
void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable) 
{
    lmac154_setFramePendingSourceMatch(aEnable);
}
otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, otShortAddress aShortAddress) 
{
    return lmac154_fptSetShortAddrPending(aShortAddress, 1) == LMAC154_FPT_STATUS_SUCCESS ? OT_ERROR_NONE:OT_ERROR_NO_ADDRESS;
}
otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress) 
{
    uint32_t xaddr[2];
    arch_memcpy(xaddr, aExtAddress->m8, sizeof(aExtAddress->m8));
    return lmac154_fptSetLongAddrPending(xaddr, 1) == LMAC154_FPT_STATUS_SUCCESS ? OT_ERROR_NONE:OT_ERROR_NO_ADDRESS;
}
otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, otShortAddress aShortAddress) 
{
    return lmac154_fptRemoveShortAddr(aShortAddress) == LMAC154_FPT_STATUS_SUCCESS ? OT_ERROR_NONE:OT_ERROR_NO_ADDRESS;
}
otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress) 
{
    uint32_t xaddr[2];
    arch_memcpy(xaddr, aExtAddress->m8, sizeof(aExtAddress->m8));
    return lmac154_fptRemoveLongAddr(xaddr) == LMAC154_FPT_STATUS_SUCCESS ? OT_ERROR_NONE:OT_ERROR_NO_ADDRESS;
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance) 
{
    uint8_t num = 128;
    uint16_t * plist = (uint16_t *)malloc(sizeof(uint16_t) * num);

    lmac154_fptGetShortAddrList(plist, &num);
    for (uint32_t i = 0; i < num; i ++) 
    {
        lmac154_fptRemoveShortAddr(plist[i]);
    }
    free(plist);
}
void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance) 
{
    uint8_t num = 32;
    uint8_t * plist = (uint8_t *)malloc(sizeof(uint8_t) * 8 * num);
    uint32_t xaddr[2];

    lmac154_fptGetLongAddrList(plist, &num);
    for (uint32_t i = 0; i < num; i ++) 
    {
        memcpy(xaddr, plist + i * 8, sizeof(xaddr));
        lmac154_fptRemoveLongAddr(xaddr);
    }
    free(plist);
}
uint32_t otPlatRadioGetSupportedChannelMask(otInstance *aInstance) 
{
    return OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MASK;
}
uint32_t otPlatRadioGetPreferredChannelMask(otInstance *aInstance) 
{
    return OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MASK;
}

otError otPlatRadioSetCoexEnabled(otInstance *aInstance, bool aEnabled)
{
    if (aEnabled) {
        lmac154_enableCoex();
    }
    else {
        lmac154_disableCoex();
    }

    return OT_ERROR_NONE;
}

bool otPlatRadioIsCoexEnabled(otInstance *aInstance)
{
    (void)aInstance;
    return lmac154_isCoexEnabled();
}

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
otError otPlatRadioEnableCsl(otInstance *        aInstance,
                             uint32_t            aCslPeriod,
                             otShortAddress      aShortAddr,
                             const otExtAddress *aExtAddr)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aShortAddr);
    OT_UNUSED_VARIABLE(aExtAddr);

    ot_radio_ctx.tx_param.csl_opt.csl_period = aCslPeriod;
    lmac154_setRxFiltering(true);

    return OT_ERROR_NONE;
}

otError otPlatRadioResetCsl(otInstance *aInstance)
{
    ot_radio_ctx.tx_param.csl_opt.csl_period = 0;

    return OT_ERROR_NONE;
}

void otPlatRadioUpdateCslSampleTime(otInstance *aInstance, uint32_t aCslSampleTime)
{
    OT_UNUSED_VARIABLE(aInstance);

    ot_radio_ctx.tx_param.csl_opt.csl_sample_time = aCslSampleTime / LMAC154_US_PER_SYMBOL;
}

uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return BFLB_RADIO_CSL_ACCURACY;
}

uint8_t otPlatRadioGetCslUncertainty(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return BFLB_RADIO_CSL_UNCERTAINTY;
}
#endif
