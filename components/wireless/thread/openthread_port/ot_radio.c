#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#if defined BL702L
#include <bl702l_phy.h>
#endif

#ifdef CONFIG_BOUFFALO_SDK
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
#if OT_RADIO_CSL_DEBUG
#include <lmac154_dbg.h>
#endif

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
    otRadio_rxFrame_t *pframe = NULL;
    otRadioState radioeState = -1;

    uint32_t tag = otrEnterCrit();
    if (!utils_dlist_empty(&ot_radio_ctx.rx_frame_list)) {
        pframe = (otRadio_rxFrame_t *)ot_radio_ctx.rx_frame_list.next;
        utils_dlist_del(&pframe->dlist);
    }
    otrExitCrit(tag);

    if (NULL == pframe) {
        radioeState = otPlatRadioGetState(ot_radio_ctx.instance);
        if ( radioeState == OT_RADIO_STATE_DISABLED || otPlatRadioGetState(ot_radio_ctx.instance) == OT_RADIO_STATE_SLEEP ) {
            return true;
        }
    }

    return false;
}

#if OT_RADIO_CSL_DEBUG
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
#endif

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

            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_ACKED);
        }
        else {
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_ERROR);
        }

#if OT_RADIO_CSL_DEBUG
        if (tx_status == LMAC154_TX_STATUS_ACKED || tx_status == LMAC154_TX_STATUS_NO_ACK) {
            uint32_t actual_shr = (uint32_t)(lmac154_get_tx_done_timestamp() >> 4) - (((uint32_t)ot_radio_ctx.tx_param.pkt_length + 1) << 1) - LMAC154_SHR_LEN;
            if (ot_radio_ctx.tx_param.csl_opt.csl_period) {
                lmac154_dbg_trace("csl child txd: %ld, tx seq: %d %d, tx ts: %d, period: %lu, tx phase: %u, tx drift: %d\r\n",
                        zb_timer_get_current_time(), ot_radio_ctx.tx_frame->mPsdu[2], tx_status, actual_shr,
                        ot_radio_ctx.tx_param.csl_opt.csl_period, ot_radio_ctx.tx_param.csl_opt.csl_sample_time,
                        ot_radio_ctx.tx_param.csl_opt.dbg_csl_tx_phase,
                        ot_radio_ctx.tx_param.csl_opt.dbg_csl_drift_tx);
            } 
            else {
                lmac154_ie_csl_t *csl = ot_radio_find_csl_ie(ack_frame, ack_frame_len, 3);
                if (tx_status == LMAC154_TX_STATUS_ACKED && csl != NULL) {
                    uint32_t expected_shr = ot_radio_ctx.tx_param.tx_timing.base_time + ot_radio_ctx.tx_param.tx_timing.delay_time;
                    int32_t drift = expected_shr? (int32_t)(expected_shr - actual_shr) : 0;
                    lmac154_dbg_trace("csl parent txd: %ld,  tx seq: %d, tx ts: %d, phase: %u, period: %lu, expected %lu, timing drift: %d sym\r\n",
                        zb_timer_get_current_time(), ot_radio_ctx.tx_frame->mPsdu[2], actual_shr,
                        csl->bf.phase, csl->bf.period, expected_shr, drift);
                }
            }
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
        uint32_t tx_sfd_sym = rx_info->rx_done_symbol_cnt + rx_info->enh_ack_time + LMAC154_SHR_LEN;
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

#if defined (CONFIG_BOUFFALO_SDK) && defined (BL616)
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

    if (LMAC154_RX_STATUS_RX_DONE == status) {
#if OT_RADIO_CSL_DEBUG
        if (info->rx_error) {
            if (ot_radio_ctx.tx_param.csl_opt.csl_period) {
                lmac154_dbg_trace("csl child rx error: %ld,  mpdu: %08lx, rx ts: %d, error: %x\r\n",
                        zb_timer_get_current_time(), frame[0], info->rx_done_symbol_cnt - ((uint32_t)(info->rx_length + 1) << 1), info->rx_error);
            }
            return;
        }
#endif

        if (info->rx_error) {
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
        rx_frame->frame.mChannel = lmac154_getChannel();
        rx_frame->frame.mLength = info->rx_length;
        rx_frame->frame.mInfo.mRxInfo.mAckedWithFramePending = info->is_frame_pended;
        rx_frame->frame.mInfo.mRxInfo.mAckedWithSecEnhAck =
            info->is_enh_ack_requested && (frame[0] & LMAC154_FRAME_SECURITY_MASK);
    }

    if (LMAC154_RX_STATUS_ACK_SENT == status || !info->is_tx_acking) {
#if OT_RADIO_CSL_DEBUG
        uint32_t rx_sfd_sym = info->rx_done_symbol_cnt - ((uint32_t)(info->rx_length + 1) << 1);
        if (ot_radio_ctx.tx_param.csl_opt.csl_period) {
            uint32_t actual_shr = (uint32_t)(lmac154_get_tx_done_timestamp() >> 4) - (((uint32_t)ot_radio_ctx.tx_param.pkt_length + 1) << 1) - LMAC154_SHR_LEN;
            uint32_t period_sym = (uint32_t)ot_radio_ctx.tx_param.csl_opt.csl_period * 10;
            uint32_t sample_phase = ot_radio_ctx.tx_param.csl_opt.csl_sample_time % period_sym;
            uint32_t rx_phase = rx_sfd_sym % period_sym;
            int32_t rx_drift_sym = (int32_t)(rx_phase - sample_phase);
            if (rx_drift_sym > (int32_t)(period_sym / 2)) rx_drift_sym -= period_sym;
            if (rx_drift_sym < -(int32_t)(period_sym / 2)) rx_drift_sym += period_sym;
            lmac154_dbg_trace("csl child rxd: %ld, seq: %u, acked: %u, period: %lu, ack phase: %u, sample time: %lu, rxd: %lu, ack drift: %d, rx drift: %d sym\r\n",
                   zb_timer_get_current_time(), (uint8_t)(frame[0] >> 16), actual_shr,
                   ot_radio_ctx.tx_param.csl_opt.csl_period, info->dbg_csl_ack_phase, ot_radio_ctx.tx_param.csl_opt.csl_sample_time, rx_sfd_sym, 
                   info->dbg_csl_drift_ack, rx_drift_sym);
        }
        else {
            lmac154_ie_csl_t *csl = ot_radio_find_csl_ie(frame, info->rx_length, info->mac_hdr_size);
            if (csl) {
                lmac154_dbg_trace("csl parent rxd: %d seq: %u, phase: %u, period: %u, rxd: %ld",
                    zb_timer_get_current_time(), (uint8_t)(frame[0] >> 16),
                    csl->bf.phase, csl->bf.period, rx_sfd_sym);
            }
        }
        info->dbg_csl_ack_phase = 0;
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
#endif
    if (ot_radio_ctx.tx_param.is_tx_security) {
        aFrame->mInfo.mTxInfo.mIsHeaderUpdated = true;
        ot_radio_ctx.tx_param.sec.a_data_length = mhr_len + aux_len + ie_len;
        if (LMAC154_FRAME_IS_CMD(fcf) && !LMAC154_FRAME_IS_FRAME_2015(fcf)) {
            /** command id is is a_data in 2003/2006 */
            ot_radio_ctx.tx_param.sec.a_data_length += 1;
        }
        ot_radio_ctx.tx_param.sec.m_data_length = ot_radio_ctx.tx_param.pkt_length - ot_radio_ctx.tx_param.sec.a_data_length;

#if defined (CONFIG_BOUFFALO_SDK) && defined (BL616)
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
                                                     ((uint32_t)(aFrame->mInfo.mTxInfo.mTxDelayBaseTime >> ZB_TIMER_BITNUM_PER_TICK)) - LMAC154_SHR_LEN);
        ot_radio_ctx.tx_param.tx_timing.base_time = base_symbol;
        if (ot_radio_ctx.tx_param.tx_timing.delay_time > (OPENTHREAD_CONFIG_MAC_CSL_REQUEST_AHEAD_US >> ZB_TIMER_BITNUM_PER_TICK)) {
            iret = -255;
        }
    }

    if (iret >= 0) {
        iret = lmac154_triggerParamTxExt(&ot_radio_ctx.tx_param);
    }
    otrExitCrit(tag);
#if OT_RADIO_CSL_DEBUG
    lmac154_dbg_trace("csl send: %ld, seq: %u, period: %lu, sample time: %lu, %s, %d\r\n",
           zb_timer_get_current_time(), aFrame->mPsdu[2], 
           ot_radio_ctx.tx_param.csl_opt.csl_period, ot_radio_ctx.tx_param.csl_opt.csl_sample_time,
           LMAC154_FRAME_IS_CMD(fcf)? "cmd": "data", iret);
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
    return OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_CSMA_BACKOFF | OT_RADIO_CAPS_SLEEP_TO_TX | OT_RADIO_CAPS_TRANSMIT_TIMING | OT_RADIO_CAPS_TRANSMIT_SEC; 
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
#ifdef CONFIG_BOUFFALO_SDK
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

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel) 
{
    uint8_t ch = lmac154_getChannel();

#if (OPENTHREAD_FTD) || (OPENTHREAD_MTD)
    if (lmac154_isRxStateWhenIdle() != otThreadGetLinkMode(aInstance).mRxOnWhenIdle) {
        lmac154_setRxStateWhenIdle(otThreadGetLinkMode(aInstance).mRxOnWhenIdle);
    }
#endif

    if (ch != (aChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN)) {

        lmac154_disableRx();

        ch = aChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;
        lmac154_setChannel(ch);

#if defined BL702L
        bz_phy_optimize_tx_channel(2405 + 5 * ch);
#endif
    }

    lmac154_enableRx();

#if OT_RADIO_CSL_DEBUG
    if (ot_radio_ctx.tx_param.csl_opt.csl_period) {
        lmac154_dbg_trace("m154_rx_open %10ld ch%d %d\n", zb_timer_get_current_time(), aChannel, lmac154_getRFState());
    }
#endif

    return OT_ERROR_NONE;
}

otError otPlatRadioSleep(otInstance *aInstance)
{
    bool is_rx_on_when_idle = false;
#if (OPENTHREAD_FTD) || (OPENTHREAD_MTD)
    is_rx_on_when_idle = otThreadGetLinkMode(aInstance).mRxOnWhenIdle;
#endif
    lmac154_setRxStateWhenIdle(is_rx_on_when_idle);

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    uint32_t tag = otrEnterCrit();
    lmac154_rf_state_t rf_state = lmac154_getRFState();
    if (!is_rx_on_when_idle && ot_radio_ctx.tx_param.csl_opt.csl_period) {

        /* Skip disableRx if RF is busy receiving to avoid truncating
         * an in-progress frame. This allows OPENTHREAD_CONFIG_MIN_RECEIVE_ON_AFTER
         * to cover the actual frame reception time rather than being cut short
         * by the timer-based sleep. */
        if (rf_state != LMAC154_RF_STATE_RX_DOING && rf_state != LMAC154_RF_STATE_ACK_DOING) {
            lmac154_disableRx();
        }
    }
    otrExitCrit(tag);
#if OT_RADIO_CSL_DEBUG
    if (ot_radio_ctx.tx_param.csl_opt.csl_period) {
        lmac154_dbg_trace("m154_rx_sleep %10ld %d=>%d ts=%ld",
            zb_timer_get_current_time(), rf_state, lmac154_getRFState(), ot_radio_ctx.tx_param.csl_opt.csl_sample_time);
    }
#endif
#else
    lmac154_disableRx();
#endif
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

otError otPlatRadioReceiveAt(otInstance *aInstance, uint8_t aChannel, uint32_t aStart, uint32_t aDuration)
{
    return OT_ERROR_NOT_IMPLEMENTED;
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
