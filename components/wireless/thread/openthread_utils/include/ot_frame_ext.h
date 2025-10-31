#ifndef __OT_FRAME_EXT_H
#define __OT_FRAME_EXT_H

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

#endif