#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#if defined (BL616)
#include <bflb_common.h>
#include <bflb_l1c.h>
#endif

#if defined BL702L
#include <bl702l_phy.h>
#endif

#include <lmac154.h>
#include <lmac154_fpt.h>
#ifdef CFG_OT_USE_ROM_CODE
#include <rom_lmac154_ext.h>
#endif

#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE
#include <openthread/platform/alarm-micro.h>
#include <openthread_port.h>
#include <ot_radio_trx.h>
#include <ot_utils_ext.h>

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

static otRadio_t                otRadioVar;
static lmac154_txParam_t        otRadio_txParam;
static uint8_t                  otRadio_buffPool[MAC_FRAME_SIZE * (OTRADIO_RX_FRAME_BUFFER_NUM + 1) + MAX_ACK_FRAME_SIZE * OTRADIO_ACK_FRAME_BUFFER_NUM];
#ifdef CFG_OT_USE_ROM_CODE
extern otRadio_t *              otRadioVar_ptr;
#else
otRadio_t *                     otRadioVar_ptr = &otRadioVar;
#endif

void ot_radioInit(otRadio_opt_t opt) 
{
    otRadio_rxFrame_t *pframe = NULL;
    otRadioVar_ptr = &otRadioVar;

    lmac154_init();
    lmac154_disableRx();

    memset(otRadioVar_ptr, 0, sizeof(otRadio_t));
    memset(&otRadio_txParam, 0, sizeof(lmac154_txParam_t));

    otRadioVar_ptr->opt.byte = opt.byte;
#if OPENTHREAD_RADIO
    otRadioVar_ptr->opt.bf.isFtd = true;
#endif

    uint32_t tag = otrEnterCrit();
    utils_dlist_init(&otRadioVar_ptr->frameList);
    utils_dlist_init(&otRadioVar_ptr->rxFrameList);

    for (int i = 0; i < OTRADIO_RX_FRAME_BUFFER_NUM; i ++) {
        pframe = (otRadio_rxFrame_t *) (otRadio_buffPool + MAC_FRAME_SIZE * (i + 1));
        pframe->frame.mPsdu = ((uint8_t *)pframe) + FRAME_OVERHEAD_SIZE;
        utils_dlist_add_tail(&pframe->dlist, &otRadioVar_ptr->frameList);
    }

    otRadioVar_ptr->pTxAckFrame = (otRadioFrame *)(otRadio_buffPool + MAC_FRAME_SIZE * (OTRADIO_RX_FRAME_BUFFER_NUM + 1));
    otRadioVar_ptr->pTxAckFrame->mPsdu = ((uint8_t *)otRadioVar_ptr->pTxAckFrame) + FRAME_OVERHEAD_SIZE;
    otRadioVar_ptr->pRxAckFrame = (otRadioFrame *)(((uint8_t *)otRadioVar_ptr->pTxAckFrame) + MAX_ACK_FRAME_SIZE);
    otRadioVar_ptr->pRxAckFrame->mPsdu = ((uint8_t *)otRadioVar_ptr->pRxAckFrame) + FRAME_OVERHEAD_SIZE;

    if (opt.bf.isLinkMetricEnable) {
        otLinkMetrics_init(IEEE802_15_4_RADIO_RECEIVE_SENSITIVITY);
    }

    otrExitCrit(tag);
}

void ot_radioTask(ot_system_event_t trxEvent) 
{
    otRadio_rxFrame_t   *pframe;
    otRadioFrame        *txframe = otRadioVar_ptr->pTxFrame;
    uint32_t            tag;

    if (!(trxEvent & OT_SYSTEM_EVENT_RADIO_ALL_MASK)) {
        return;
    }
    
    if ((trxEvent & OT_SYSTEM_EVENT_RADIO_TX_ALL_MASK) && txframe) {

        txframe = otRadioVar_ptr->pTxFrame;
        otRadioVar_ptr->pTxFrame = NULL;

        if (txframe) {
            if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ) {
                otPlatRadioTxDone(otRadioVar_ptr->aInstance, txframe, NULL, OT_ERROR_NONE);
            }
            else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_ACKED) {
                otPlatRadioTxDone(otRadioVar_ptr->aInstance, txframe, otRadioVar_ptr->pRxAckFrame, OT_ERROR_NONE);
            }
            else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_NO_ACK) {
                otPlatRadioTxDone(otRadioVar_ptr->aInstance, txframe, NULL, OT_ERROR_NO_ACK);
            }
            else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_ABORT) {
                otPlatRadioTxDone(otRadioVar_ptr->aInstance, txframe, NULL, OT_ERROR_ABORT);
            }
            else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_ERROR) {
                otPlatRadioTxDone(otRadioVar_ptr->aInstance, txframe, NULL, OT_ERROR_CHANNEL_ACCESS_FAILURE);
            }
        }
    }
    
    if (trxEvent & OT_SYSTEM_EVENT_RADIO_RX_DONE ) {

        pframe = NULL;
        tag = otrEnterCrit();
        if (!utils_dlist_empty(&otRadioVar_ptr->rxFrameList)) {
            pframe = (otRadio_rxFrame_t *)otRadioVar_ptr->rxFrameList.next;
            utils_dlist_del(&pframe->dlist);
        }
        otrExitCrit(tag);

        if (pframe) {
            otPlatRadioReceiveDone(otRadioVar_ptr->aInstance, &pframe->frame, OT_ERROR_NONE);

            tag = otrEnterCrit();
            utils_dlist_add_tail(&pframe->dlist, &otRadioVar_ptr->frameList);
            otrExitCrit(tag);

            if (!utils_dlist_empty(&otRadioVar_ptr->rxFrameList)) {
                /* post OT_SYSTEM_EVENT_RADIO_RX_DONE to handle next pending packet */
                otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_RX_DONE);
            }
        }
    }
    else if (trxEvent & OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF) {
        otPlatRadioReceiveDone(otRadioVar_ptr->aInstance, NULL, OT_ERROR_NO_BUFS);
    }
}

bool otr_isStackIdle(void)
{
    otRadio_rxFrame_t *pframe = NULL;
    otRadioState radioeState = -1;

    uint32_t tag = otrEnterCrit();
    if (!utils_dlist_empty(&otRadioVar_ptr->rxFrameList)) {
        pframe = (otRadio_rxFrame_t *)otRadioVar_ptr->rxFrameList.next;
        utils_dlist_del(&pframe->dlist);
    }
    otrExitCrit(tag);

    if (ot_system_event_var == 0 && NULL == pframe) {
        radioeState = otPlatRadioGetState(otRadioVar_ptr->aInstance);
        if ( radioeState == OT_RADIO_STATE_DISABLED || otPlatRadioGetState(otRadioVar_ptr->aInstance) == OT_RADIO_STATE_SLEEP ) {
            return true;
        }
    }

    return false;
}

/************************************ LMAC 15.4 event function ***********************************/
void ot_radioTxDoneCallback (lmac154_tx_status_t tx_status, uint32_t * ack_frame, uint32_t ack_frame_len) 
{
    if (otRadioVar_ptr->pTxFrame) {
        if (LMAC154_TX_STATUS_TX_FINISHED == tx_status) {
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ);
        }
        else if (LMAC154_TX_STATUS_TX_ABORTED == tx_status) {
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_NO_ACK);
        }
        else if (LMAC154_TX_STATUS_NO_ACK == tx_status) {
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_ABORT);
        }
        else if (LMAC154_TX_STATUS_ACKED == tx_status) {
            otRadioVar_ptr->pRxAckFrame->mInfo.mRxInfo.mTimestamp = lmac154_get_rx_done_timestamp() - ((uint32_t)(ack_frame_len + 1) << LMAC154_US_PER_SYMBOL_BITS);
            otRadioVar_ptr->pRxAckFrame->mChannel = lmac154_getChannel() + OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;
            otRadioVar_ptr->pRxAckFrame->mInfo.mRxInfo.mRssi = lmac154_getRSSI();
            otRadioVar_ptr->pRxAckFrame->mInfo.mRxInfo.mLqi = lmac154_getLQI();

            memcpy(otRadioVar_ptr->pRxAckFrame->mPsdu, ack_frame, ack_frame_len);
            otRadioVar_ptr->pRxAckFrame->mLength = ack_frame_len;

            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_ACKED);
        }
        else {
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_ERROR);
        }
    }
}

static uint16_t ot_radioGetCslPhase(void)
{
    uint32_t curTime       = (uint32_t)otPlatAlarmMicroGetNow();
    uint32_t cslPeriodInUs = otRadioVar_ptr->cslPeriod * OT_US_PER_TEN_SYMBOLS;
    uint32_t diff = ((otRadioVar_ptr->cslSampleTime % cslPeriodInUs) - (curTime % cslPeriodInUs) + cslPeriodInUs) % cslPeriodInUs;

    otRadioVar_ptr->cslUpdateTime = curTime;
    otRadioVar_ptr->cslUpdatePhase = (uint16_t)(diff / OT_US_PER_TEN_SYMBOLS);

    return otRadioVar_ptr->cslUpdatePhase;
}

#if defined (BL616)
static uint32_t ot_aesOpt(uint32_t isDecrypt, uint8_t *pbuf, uint32_t len, uint8_t *pAddr, 
    otRadio_auxSecHdr_t *pAuxHdr, uint8_t *pld, uint8_t *pkey)
{
    uint32_t pldlen;
    uint32_t aNonce[4];
    uint8_t miclen;
    uint8_t *p;
    
    #define AES_MEMORY_ALIGN_BIT    4
    #define AES_MEMORY_ALIGN_MASK   (((uint32_t)1 << AES_MEMORY_ALIGN_BIT) - 1)
    #define AES_MEMORY_ALIGN_UNMASK (~(((uint32_t)1 << AES_MEMORY_ALIGN_BIT) - 1))

    if (0 == pAuxHdr || (pAuxHdr->secLvl & 3) > 3) {
        return 0;
    }

    p = (uint8_t *)aNonce;
    memcpy(aNonce, pAddr, 8);

    p = (uint8_t *)&(pAuxHdr->frameCounter);
    aNonce[2] = ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | ((uint32_t)p[3]);
    aNonce[3] = pAuxHdr->secLvl;

    miclen = 4 << ((pAuxHdr->secLvl & 3) - 1);
    pldlen = len - (uint32_t)(pld - pbuf) - (miclen + 2);

    uint32_t *a_data = bflb_get_no_cache_addr(otRadioVar_ptr->aesOutput);
    uint32_t *output_data, *input_data;
    uint32_t a_data_word = ((uint8_t)(pld - pbuf) + AES_MEMORY_ALIGN_MASK) & AES_MEMORY_ALIGN_UNMASK;
    uint32_t input_data_word = (pldlen + AES_MEMORY_ALIGN_MASK) & AES_MEMORY_ALIGN_UNMASK;

    assert(((a_data_word + input_data_word * 2) >> AES_MEMORY_ALIGN_BIT) 
        <= ((sizeof(otRadioVar_ptr->aesOutput) - ((uint32_t)a_data & AES_MEMORY_ALIGN_MASK)) >> AES_MEMORY_ALIGN_BIT));

    a_data = (uint32_t *)(((uint32_t)a_data + AES_MEMORY_ALIGN_MASK) & AES_MEMORY_ALIGN_UNMASK);
    input_data = a_data + a_data_word / sizeof(uint32_t);
    output_data = input_data + input_data_word / sizeof(uint32_t);

    bflb_l1c_dcache_clean_invalidate_range(otRadioVar_ptr->aesOutput, sizeof(otRadioVar_ptr->aesOutput));

    memset(a_data, 0, a_data_word + input_data_word * 2);
    memcpy(a_data, pbuf, (uint8_t)(pld - pbuf));
    memcpy(input_data, pld, pldlen);

    if (LMAC154_AES_STATUS_SUCCESS == lmac154_runAESCCM((lmac154_aes_mode_t)isDecrypt, (const uint8_t *)a_data, (uint8_t)(pld - pbuf), 
                                        (uint8_t *)aNonce, (uint32_t *)pkey, 
                                        (lmac154_aes_mic_len_t)((pAuxHdr->secLvl & 3) - 1), aNonce,
                                        input_data, output_data, pldlen)) {

        if (isDecrypt) {
            if (memcmp(pld + pldlen, aNonce, miclen)) {
                return 0;
            }
        }
        else {
            memcpy(pld, output_data, pldlen);
            memcpy(pld + pldlen, aNonce, miclen);
        }

        return miclen;
    }

    return 0;
}
#elif defined (BL702) || defined (BL702L)
static uint32_t ot_aesOpt(uint32_t isDecrypt, uint8_t *pbuf, uint32_t len, uint8_t *pAddr, 
    otRadio_auxSecHdr_t *pAuxHdr, uint8_t *pld, uint8_t *pkey)
{
    uint32_t pldlen;
    uint32_t aNonce[4];
    uint8_t miclen;
    uint8_t *p;

    if (0 == pAuxHdr || (pAuxHdr->secLvl & 3) > 3) {
        return 0;
    }

    p = (uint8_t *)aNonce;
    memcpy(aNonce, pAddr, 8);

    p = (uint8_t *)&(pAuxHdr->frameCounter);
    aNonce[2] = ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | ((uint32_t)p[3]);
    aNonce[3] = pAuxHdr->secLvl;

    miclen = 4 << ((pAuxHdr->secLvl & 3) - 1);
    pldlen = len - (uint32_t)(pld - pbuf) - (miclen + 2);

    memcpy(otRadioVar_ptr->aesOutput, pld, pldlen);
    if (LMAC154_AES_STATUS_SUCCESS == lmac154_runAESCCM((lmac154_aes_mode_t)isDecrypt, pbuf, (uint8_t)(pld - pbuf), 
                                        (uint8_t *)aNonce, (uint32_t *)pkey, 
                                        (lmac154_aes_mic_len_t)((pAuxHdr->secLvl & 3) - 1), aNonce,
                                        (uint32_t *)otRadioVar_ptr->aesOutput, otRadioVar_ptr->aesOutput, pldlen)) {

        if (isDecrypt) {
            if (memcmp(pld + pldlen, aNonce, miclen)) {
                return 0;
            }
        }
        else {
            memcpy(pld, otRadioVar_ptr->aesOutput, pldlen);
            memcpy(pld + pldlen, aNonce, miclen);
        }

        return miclen;
    }

    return 0;
}
#endif

static uint32_t ot_radioHandleRecv(uint32_t *rx_buf, uint32_t rx_len, bool isFramePended, uint32_t * ack_buf) 
{
    struct otMacKeyMaterial *   pkey = NULL;
    otInternel_addrType_t       srcAddrType = OTINTERNEL_ADDR_TYPE_NONE;
    uint8_t *                   pSrcAddr = NULL;
    uint8_t *                   pld, *pbuf = (uint8_t *)rx_buf;
    otRadio_auxSecHdr_t *       pAuxHdr, *pAckAuxHdr = NULL;
    uint32_t                    ret;
    uint8_t *                   p;
    uint32_t                    ack_length = 0;

    pld = ot_genAckMacHeader(&pbuf, rx_len, ack_buf, &srcAddrType, &pSrcAddr, 
                             &pAuxHdr, otRadioVar_ptr->macFrameCounter);
    if (NULL == pld) {
        return 0;
    }

    if (pAuxHdr && pAuxHdr->secLvl) {
        if (pAuxHdr->keyId == otRadioVar_ptr->macKeyId - 1) {
            pkey = &otRadioVar_ptr->previousKey;
        }
        else if (pAuxHdr->keyId == otRadioVar_ptr->macKeyId) {
            pkey = &otRadioVar_ptr->currentKey;
        }
        else if (pAuxHdr->keyId == otRadioVar_ptr->macKeyId + 1) {
            pkey = &otRadioVar_ptr->nextKey;
        }
        else {
            return 0;
        }
        otRadioVar_ptr->macFrameCounter ++;
        pAckAuxHdr = (otRadio_auxSecHdr_t *)pld;
        pld += sizeof(otRadio_auxSecHdr_t);
    }

    if (isFramePended) {
        ack_buf[0] |= LMAC154_FRAME_FRAME_PENDING_MASK;
    }

    if (otRadioVar_ptr->opt.bf.isLinkMetricEnable || otRadioVar_ptr->opt.bf.isCSLReceiverEnable) {
        p = pld;
        if (otRadioVar_ptr->opt.bf.isCSLReceiverEnable && otRadioVar_ptr->cslPeriod > 0) {
            p = p + ot_genCslIE((uint16_t)otRadioVar_ptr->cslPeriod, ot_radioGetCslPhase(), p);
        }

        if (otRadioVar_ptr->opt.bf.isLinkMetricEnable) {
            p = p + otLinkMetrics_genEnhAckData(srcAddrType, (uint8_t *)pSrcAddr, lmac154_getRSSI(), lmac154_getLQI(), p);
        }

        if (p != pld) {
            ack_buf[0] |= LMAC154_FRAME_IE_MASK;
            pld = p;
        }
    }

    if (pkey && pAuxHdr) {
        ack_length = pld - (uint8_t *)ack_buf;
        ack_length += ((4 << ((pAuxHdr->secLvl & 3) - 1)) + 2);
        ret = ot_aesOpt(0, (uint8_t *)ack_buf, ack_length, 
            otRadioVar_ptr->extAddress.m8, pAckAuxHdr, pld, pkey->mKeyMaterial.mKey.m8);

        if (ret) {
            return ack_length;
        }
    }

    return pld - (uint8_t *)ack_buf + 2;
}

uint32_t * ot_radioRxDoneCallback (lmac154_receiveInfo_t * recvInfo, uint32_t * frame) 
{
    int                     enhAckFrameLength = 0;
    uint8_t *               enhAckFrame = NULL;
    otRadio_rxFrame_t *     rxFrame = NULL;

    if (recvInfo->rx_error) {
        return NULL;
    }

    if (utils_dlist_empty(&otRadioVar_ptr->frameList)) {
        otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF);
        return NULL;
    }
    rxFrame = (otRadio_rxFrame_t *)otRadioVar_ptr->frameList.next;

    if (recvInfo->is_rx_buf) {

        assert(frame == (uint32_t *)rxFrame->frame.mPsdu);

        rxFrame->frame.mInfo.mRxInfo.mTimestamp = recvInfo->rx_timestamp;
        rxFrame->frame.mInfo.mRxInfo.mRssi = recvInfo->rssi;
        rxFrame->frame.mInfo.mRxInfo.mLqi = recvInfo->lqi;
        rxFrame->frame.mChannel = recvInfo->channel;
        rxFrame->frame.mLength = recvInfo->rx_length;

        rxFrame->frame.mInfo.mRxInfo.mAckedWithFramePending = recvInfo->is_frame_pended;
        rxFrame->frame.mInfo.mRxInfo.mAckedWithSecEnhAck = recvInfo->is_enh_ack_requested && (LMAC154_FRAME_SECURITY_MASK | frame[0]);

        utils_dlist_del(&rxFrame->dlist);
        utils_dlist_add_tail(&rxFrame->dlist, &otRadioVar_ptr->rxFrameList);

        otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_RX_DONE);

        return NULL;
    }

    if (recvInfo->is_enh_ack_requested) {

        enhAckFrame = otRadioVar_ptr->pTxAckFrame->mPsdu;
        enhAckFrameLength = ot_radioHandleRecv(frame, 
                                                     recvInfo->rx_length, 
                                                     recvInfo->is_frame_pended, 
                                                     (uint32_t *)enhAckFrame);
        enhAckFrameLength = lmac154_writeTxAckFrame(enhAckFrame, enhAckFrameLength - 2);
        if (enhAckFrameLength < 0) {
            return NULL;
        }
    }

    recvInfo->rx_timestamp = lmac154_get_rx_done_timestamp() - ((uint32_t)(recvInfo->rx_length + 1) << LMAC154_US_PER_SYMBOL_BITS);
    recvInfo->rssi = lmac154_getRSSI();
    recvInfo->lqi = lmac154_getLQI();
    recvInfo->channel = lmac154_getChannel() + OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;

    return (uint32_t *)rxFrame->frame.mPsdu;
}

/*************************** Openthread radio interface implementation ***************************/
otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance) 
{
    otRadioVar_ptr->aInstance = aInstance;

    otRadioFrame * txframe = (otRadioFrame *)otRadio_buffPool;
    txframe->mPsdu = otRadio_buffPool + FRAME_OVERHEAD_SIZE;

    txframe->mInfo.mTxInfo.mIeInfo = &otRadioVar_ptr->transmitIeInfo;

    return txframe;
}

int ot_encrytTxFrame(otRadioFrame *aFrame)
{
    uint16_t                fcf = *(uint16_t *)aFrame->mPsdu;
    uint8_t                 *pld = NULL;
    otInternel_addrType_t   dstAddrType, srcAddrType;
    uint8_t                 *pDstAddr = NULL, *pSrcAddr = NULL;
    otRadio_auxSecHdr_t     *pAuxHdr = NULL;
    uint32_t                ret;
    uint32_t                framelen = 0;

    if (LMAC154_FRAME_IS_NORMAL_SECURITY(fcf) && !aFrame->mInfo.mTxInfo.mIsSecurityProcessed) {
        pld = ot_parseMacHeader(aFrame->mPsdu, &dstAddrType, &pDstAddr, &srcAddrType, &pSrcAddr, &pAuxHdr);
        if (pld && (pld - aFrame->mPsdu) < aFrame->mLength && pAuxHdr && pAuxHdr->keyIdMode == 1) {

            aFrame->mInfo.mTxInfo.mAesKey = NULL;
            pAuxHdr->keyId = otRadioVar_ptr->macKeyId;
            memcpy(&pAuxHdr->frameCounter, &otRadioVar_ptr->macFrameCounter, 4);
            otRadioVar_ptr->macFrameCounter++;

            if (0 == otRadioVar_ptr->macFrameCounter) {
                return -1;
            }
            aFrame->mInfo.mTxInfo.mIsHeaderUpdated = true;

            if ((fcf & (LMAC154_FRAME_VERSION_2015 | LMAC154_FRAME_IE_MASK)) == (LMAC154_FRAME_VERSION_2015 | LMAC154_FRAME_IE_MASK)) {
                framelen = aFrame->mLength - 2 - (4 << ((pAuxHdr->secLvl & 3) - 1));
                if ((ret = ot_getIElength(pld, aFrame->mPsdu + framelen)) == 0) {
                    return -2;
                }
                pld += ret;
            }

            if (!(LMAC154_FRAME_VERSION_2015 & fcf) && LMAC154_FRAME_IS_CMD(fcf)) {
                /** the command id field in 2003/2006 command frame is open payload field*/
                pld += 1;
            }

            uint32_t tag = otrEnterCrit();
            ret = ot_aesOpt(0, aFrame->mPsdu, aFrame->mLength, otRadioVar_ptr->extAddress.m8, pAuxHdr, pld, 
                            otRadioVar_ptr->currentKey.mKeyMaterial.mKey.m8);
            otrExitCrit(tag);
            aFrame->mInfo.mTxInfo.mIsSecurityProcessed = true;

            if (ret == 0) {
                return -3;
            }
        }
    }

    return 0;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame) 
{
    uint8_t *   pkt = NULL;
    int         iret;
    uint32_t    tag;

    if (otRadioVar_ptr->pTxFrame == NULL) {

        // Seek the time sync offset and update the rendezvous time
        if (otRadioVar_ptr->opt.bf.isTimeSyncEnable && aFrame->mInfo.mTxInfo.mIeInfo && aFrame->mInfo.mTxInfo.mIeInfo->mTimeIeOffset != 0 && !aFrame->mInfo.mTxInfo.mIsHeaderUpdated) {
            uint8_t *timeIe = aFrame->mPsdu + aFrame->mInfo.mTxInfo.mIeInfo->mTimeIeOffset;
            uint64_t time   = otPlatAlarmMicroGetNow() + aFrame->mInfo.mTxInfo.mIeInfo->mNetworkTimeOffset;

            *timeIe ++ = aFrame->mInfo.mTxInfo.mIeInfo->mTimeSyncSeq;
            memcpy(timeIe, &time, sizeof(uint64_t));
            *(++timeIe) = (uint8_t)(time & 0xff);
        }

        if (!aFrame->mInfo.mTxInfo.mIsHeaderUpdated) {
            otRadioVar_ptr->opt.bf.isTxTimestampValid = false;
        }

        pkt = otRadioVar_ptr->pTxFrame->mPsdu;
        if (otRadioVar_ptr->opt.bf.isCSLReceiverEnable && otRadioVar_ptr->cslPeriod > 0 
            && !aFrame->mInfo.mTxInfo.mIsHeaderUpdated) {

            // Update IE data in the 802.15.4 header with the newest CSL period / phase
            pkt = ot_findIe(aFrame->mPsdu, aFrame->mLength);
            if (pkt && (pkt = ot_findIeEntry(pkt, aFrame->mLength - (pkt - aFrame->mPsdu), IE_CSL_ID)) != NULL) {
                ot_genCslIE((uint16_t)otRadioVar_ptr->cslPeriod, ot_radioGetCslPhase(), pkt);
                otRadioVar_ptr->opt.bf.isCslPhaseUpdated = true;
            }
            else {
                otRadioVar_ptr->opt.bf.isTxTimestampValid = false;
            }
        }

        otRadio_txParam.pkt = (uint32_t *)aFrame->mPsdu;
        otRadio_txParam.pkt_length = aFrame->mLength - 2;
        otRadio_txParam.tx_channel = aFrame->mChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;
        otRadio_txParam.resume_channel = aFrame->mInfo.mTxInfo.mRxChannelAfterTxDone;
        if (aFrame->mInfo.mTxInfo.mTxDelayBaseTime || aFrame->mInfo.mTxInfo.mTxDelay) {
            otRadio_txParam.csma_ca_max_backoff = 0;
        }
        else {
            otRadio_txParam.csma_ca_max_backoff = aFrame->mInfo.mTxInfo.mMaxCsmaBackoffs;
        }
        otRadio_txParam.is_cca = aFrame->mInfo.mTxInfo.mCsmaCaEnabled;
        otRadio_txParam.base_time = aFrame->mInfo.mTxInfo.mTxDelayBaseTime >> LMAC154_US_PER_SYMBOL_BITS;
        otRadio_txParam.delay_time = aFrame->mInfo.mTxInfo.mTxDelay >> LMAC154_US_PER_SYMBOL_BITS;
        otRadio_txParam.tx_done_cb = ot_radioTxDoneCallback;
        otRadio_txParam.next = NULL;

        if (ot_encrytTxFrame(aFrame) != 0) {
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_ERROR);
            return OT_ERROR_NONE;
        }

        otRadioVar_ptr->pTxFrame = aFrame;

        tag = otrEnterCrit();
        iret = lmac154_triggerParamTx(&otRadio_txParam);
        otrExitCrit(tag);

        if (iret >= 0) {
            otPlatRadioTxStarted(aInstance, aFrame);
        }
        else {
            otrNotifyEvent(OT_SYSTEM_EVENT_RADIO_TX_ERROR);
        }


        return OT_ERROR_NONE;
    }

    return OT_ERROR_INVALID_STATE;
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

void otPlatRadioSetPanId(otInstance *aInstance, otPanId aPanId) 
{
    lmac154_setPanId(aPanId);
}
void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aExtAddress) 
{
    lmac154_setLongAddr((uint8_t *) aExtAddress->m8);
#ifdef CFG_OT_USE_ROM_CODE
    memcpy(otRadioVar_ptr->extAddress.m8, aExtAddress->m8, 8);
#else
    for (int i = 0; i < 8; i ++) {
        otRadioVar_ptr->extAddress.m8[i] = aExtAddress->m8[7 - i];
    }
#endif
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

void otPlatRadioSetMacKey(otInstance *            aInstance,
                          uint8_t                 aKeyIdMode,
                          uint8_t                 aKeyId,
                          const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey,
                          const otMacKeyMaterial *aNextKey,
                          otRadioKeyType          aKeyType)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKeyIdMode);
    OT_UNUSED_VARIABLE(aKeyType);

    if (aPrevKey && aCurrKey && aNextKey) {
        uint32_t tag = otrEnterCrit();

        otRadioVar_ptr->macKeyType = aKeyType;
        otRadioVar_ptr->macKeyId = aKeyId;

        memcpy(&otRadioVar_ptr->previousKey, aPrevKey, sizeof(otMacKeyMaterial));
        memcpy(&otRadioVar_ptr->currentKey, aCurrKey, sizeof(otMacKeyMaterial));
        memcpy(&otRadioVar_ptr->nextKey, aNextKey, sizeof(otMacKeyMaterial));

        otrExitCrit(tag);
    }
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    OT_UNUSED_VARIABLE(aInstance);

    uint32_t tag = otrEnterCrit();
    otRadioVar_ptr->macFrameCounter = aMacFrameCounter;
    otrExitCrit(tag);
}

otRadioState otPlatRadioGetState(otInstance *aInstance) 
{
    if(lmac154_isDisabled()) {
        return OT_RADIO_STATE_DISABLED;
    }

    if (otRadioVar_ptr->pTxFrame) {
        /** mac layer retring doesn't get done */
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

    return OT_ERROR_NONE;
}

otError otPlatRadioSleep(otInstance *aInstance) 
{
#if (OPENTHREAD_FTD) || (OPENTHREAD_MTD)
    lmac154_setRxStateWhenIdle(otThreadGetLinkMode(aInstance).mRxOnWhenIdle);
#endif

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
    return lmac154_fptSetLongAddrPending((uint8_t *)aExtAddress->m8, 1) == LMAC154_FPT_STATUS_SUCCESS ? OT_ERROR_NONE:OT_ERROR_NO_ADDRESS;
}
otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, otShortAddress aShortAddress) 
{
    return lmac154_fptRemoveShortAddr(aShortAddress) == LMAC154_FPT_STATUS_SUCCESS ? OT_ERROR_NONE:OT_ERROR_NO_ADDRESS;
}
otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress) 
{
    return lmac154_fptRemoveLongAddr((uint8_t *)aExtAddress->m8) == LMAC154_FPT_STATUS_SUCCESS ? OT_ERROR_NONE:OT_ERROR_NO_ADDRESS;
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

    lmac154_fptGetLongAddrList(plist, &num);
    for (uint32_t i = 0; i < num; i ++) 
    {
        lmac154_fptRemoveLongAddr(plist + i * 8);
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

    otRadioVar_ptr->opt.bf.isCoexEnable = aEnabled;
    return OT_ERROR_NONE;
}

bool otPlatRadioIsCoexEnabled(otInstance *aInstance) 
{
    return otRadioVar_ptr->opt.bf.isCoexEnable;
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

    otRadioVar_ptr->cslPeriod = aCslPeriod;

    return OT_ERROR_NONE;
}

otError otPlatRadioResetCsl(otInstance *aInstance)
{
    otRadioVar_ptr->cslPeriod = 0;

    return OT_ERROR_NONE;
}

void otPlatRadioUpdateCslSampleTime(otInstance *aInstance, uint32_t aCslSampleTime)
{
    OT_UNUSED_VARIABLE(aInstance);

    otRadioVar_ptr->cslSampleTime = aCslSampleTime;
}

uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return 20;
}

uint8_t otPlatRadioGetCslUncertainty(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return 20;
}
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
otError otPlatRadioConfigureEnhAckProbing(otInstance *         aInstance,
                                          otLinkMetrics        aLinkMetrics,
                                          const otShortAddress aShortAddress,
                                          const otExtAddress * aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    return otLinkMetrics_configEnhAckProbing(aShortAddress, aExtAddress, aLinkMetrics);
}
#endif
