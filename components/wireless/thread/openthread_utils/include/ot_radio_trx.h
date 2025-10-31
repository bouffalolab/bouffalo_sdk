#ifndef __OT_RADIO_TRX_EXT_
#define __OT_RADIO_TRX_EXT_

#include <utils_list.h>
#include <openthread/platform/radio.h>

#ifdef BL616
#define OT_RADIO_TRX_AES_CACHE_SIZE 128
#else
#define OT_RADIO_TRX_AES_CACHE_SIZE 64
#endif

typedef struct _otRadio_rxFrame_t {
    utils_dlist_t       dlist;
    otRadioFrame        frame;
} otRadio_rxFrame_t;

typedef struct _otlm {
    utils_dlist_t                   dlist;
    uint8_t                         ieLinkMetric[11];
    int8_t                          noisefloor;
} otlm_t;

typedef struct _otRadio_t {
    otInstance                      *aInstance;

    struct otExtAddress             extAddress;
    struct otMacKeyMaterial         currentKey;
    struct otMacKeyMaterial         previousKey;
    struct otMacKeyMaterial         nextKey;
    uint32_t                        macFrameCounter;
    uint8_t                         macKeyId;
    uint8_t                         macKeyType;

    otRadio_opt_t                   opt;

    uint8_t                         unused[2];
    int8_t                          maxED;
    uint8_t                         ackKeyId;
    uint32_t                        ackFrameCounter;        /** for enh-ack security */

    uint8_t                         immAckFrame[4];
    utils_dlist_t                   rxFrameList;
    utils_dlist_t                   frameList;
    otRadioFrame                    *pTxFrame;
    otRadioFrame                    *pRxAckFrame;
    otRadioFrame                    *pTxAckFrame;           /** used for imm-ack & enh-ack contruction */

    uint32_t                        cslPeriod;
    uint32_t                        cslSampleTime;
    uint32_t                        cslUpdateTime;
    uint32_t                        cslTxRequestTime;
    uint16_t                        cslUpdatePhase;

    uint32_t                        (*ot_findAddresses_ptr)(uint32_t , uint8_t *, uint8_t *);

    otRadioIeInfo                   transmitIeInfo;
    otlm_t                          linkMetrics;

    uint32_t                        aesOutput[OT_RADIO_TRX_AES_CACHE_SIZE];
} otRadio_t;

extern otRadio_t                    *otRadioVar_ptr;

#define OTRADIO_MAX_PSDU                    (128)

#define FRAME_OVERHEAD_SIZE                 ((sizeof(otRadio_rxFrame_t) + 3) & 0xfffffffc)
#define MAC_FRAME_SIZE                      (FRAME_OVERHEAD_SIZE + OTRADIO_MAX_PSDU)
#define MAX_ACK_FRAME_SIZE                  (FRAME_OVERHEAD_SIZE + 64)

#define OTRADIO_ACK_FRAME_BUFFER_NUM        2

#endif
