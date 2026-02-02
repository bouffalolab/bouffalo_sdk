#ifndef __OT_FRAME_H
#define __OT_FRAME_H

#define IE_TERINATION2_ID                   0x7f
#define IE_CSL_ID                           0x1a

#define IE_VENDOR_ID                        VENDOR_IE_HEADER_ID
#define IE_THREAD_VENDOR_ID                 VENDOR_IE_THREAD_OUI
#define IE_THREAD_ENH_ACK_PROBING_IE_ID     VENDOR_IE_THREAD_ENH_ACK_PROBING
#define IE_THREAD_HDR_SIZE                  4

typedef enum {
    OTINTERNEL_ADDR_TYPE_NONE       = 0,
    OTINTERNEL_ADDR_TYPE_SHORT      = 1,
    OTINTERNEL_ADDR_TYPE_EXT        = 2,
} otInternel_addrType_t;

typedef union _otRadio_addr {
    uint8_t                     extAddress[8];
    uint16_t                    shortAddress;
} __packed otRadio_addr_t;

typedef struct _otRadio_ieHdr {
    uint16_t    length:7;
    uint16_t    elementId:8;
    uint16_t    type:1;
} __packed otRadio_ieHdr_t;

typedef struct _otRadio_frameControl {
    uint16_t frameType:3;
    uint16_t securityEnable:1;
    uint16_t framePending:1;
    uint16_t ackRequest:1;
    uint16_t panidCompression:1;
    uint16_t reserved:1;
    uint16_t seqNumberSup:1;
    uint16_t iePresent:1;
    uint16_t dstAddressMode:2;
    uint16_t frameVersion:2;
    uint16_t srcAddressMode:2;
} otRadio_frameControl_t;

typedef struct _otRadio_auxSecHdr {
    uint8_t  secLvl:3;
    uint8_t  keyIdMode:2;
    uint8_t  frameCounterSup:1;
    uint8_t  asnInNonce:1;
    uint8_t  reserved:1;
    uint32_t frameCounter;
    uint8_t  keyId;
} __packed otRadio_auxSecHdr_t;

typedef struct _otRadioEnhAck_shortAddr {
    otRadio_frameControl_t  frameCtrl;
    uint8_t                 seqNum;
    uint16_t                panid;
    uint16_t                dstShortAddr;
    otRadio_auxSecHdr_t     auxHdr;
} __packed otRadioShortAddr_EnhAck_t;

typedef struct _otRadioEnhAck_extAddr {
    otRadio_frameControl_t  frameCtrl;
    uint8_t                 seqNum;
    uint16_t                panid;
    uint8_t                 dstExtAddr[8];
    otRadio_auxSecHdr_t     auxHdr;
} __packed otRadioExtAddr_EnhAck_t;

typedef union {
    struct {
        otRadio_ieHdr_t hdr;
        uint16_t phase;
        uint16_t period;
    } bf;
    uint8_t bytes[6];
} __packed otThreadCslIE_t;

uint8_t * ot_parseMacHeader(uint8_t *rxbuf, otInternel_addrType_t *pDstAddrType, uint8_t **pDstAddr, 
    otInternel_addrType_t *pSrcAddrType, uint8_t **pSrcAddr, otRadio_auxSecHdr_t **pAuxHdr);

otInternel_addrType_t ot_getSourceAddress(uint8_t * rx_buf, uint32_t rx_len, uint8_t * p_srcAddr);

uint32_t ot_getIElength(uint8_t *buf, uint8_t *buf_end);

uint32_t ot_genCslIE(uint16_t period, uint16_t phase, uint8_t * pData);

uint8_t * ot_findIe(uint8_t *pkt, uint32_t len);

uint8_t * ot_findIeEntry(uint8_t *pkt, uint32_t len2end, uint32_t id);

uint8_t * ot_getPayload(uint8_t *pkt, uint32_t len);
uint32_t ot_getPayloadLen(uint8_t *pkt, uint32_t len2end);

uint8_t * ot_genAckMacHeader(uint8_t **rxbuf, uint32_t len, uint32_t *ackbuf, otInternel_addrType_t *pSrcAddrType, 
    uint8_t **pSrcAddr, otRadio_auxSecHdr_t **pAuxHdr, uint32_t macFrameCounter);

#endif
