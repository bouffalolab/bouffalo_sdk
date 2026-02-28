#ifndef _VIRTUALCHAN_H_
#define _VIRTUALCHAN_H_

#include "mr_virtualchan.h"

#define NETHUB_VCHAN_SLOT_CNT       (4)
#define NETHUB_VCHAN_MAX_DATA_LEN   (1500)// Must eq with host config


// virtualchan data Type
#define NETHUB_VCHAN_TYPE_USER      (1)
#define NETHUB_VCHAN_TYPE_AT        (2)
#define NETHUB_VCHAN_TYPE_SYSTEM    (3)
#define NETHUB_VCHAN_TYPE_MAX       (4)

/**
 * @struct nethub_vchan_data_hdr
 * @brief Internal message header for virtual channel payload
 *
 * Layout:
 * +----------+-----------+-------+--------+
 * | data_type| reserved  | len   | data[] |
 * | 1 byte   | 1 byte    | 2 bytes| N bytes|
 * +----------+-----------+-------+--------+
 *
 * @note len field stores the actual user data length to solve Netlink 4-byte alignment issue
 */
#pragma pack(push, 1)
struct nethub_vchan_data_hdr {
    uint8_t  data_type;     /* Data type (see NETHUB_VCHAN_DATA_TYPE_* defines) */
    uint8_t  reserved;      /* Reserved field (initialized to 0) */
    uint16_t len;           /* Actual user data length (exclude header) */
    uint8_t  data[];        /* Actual user data (flexible array) */
} __attribute__((packed));
#pragma pack(pop)

#define VIRTUALCHAN_FRAME_SIZE (NETHUB_VCHAN_MAX_DATA_LEN \
                                + sizeof(struct nethub_vchan_data_hdr) \
                                + FRAME_BUFF_HEADER_ROOM \
                                + sizeof(mr_virtualchan_msg_t))

typedef int (*virtualchan_dnld_cb_t)(void *arg, uint8_t *data_buff, uint16_t data_size);

extern mr_virtualchan_priv_t *g_virtualchan_priv;

/* Initialization function */
int nethub_vchan_init(mr_msg_ctrl_priv_t *msg_ctrl);

/* Generic send and receive register functions */
int nethub_vchan_send(uint8_t type, const void *data, uint16_t len);
int nethub_vchan_recv_register(uint8_t type, virtualchan_dnld_cb_t dnld_cb, void *cb_arg);

/* User data channel wrapper functions */
int nethub_vchan_user_send(const void *data, uint16_t len);
int nethub_vchan_user_recv_register(virtualchan_dnld_cb_t dnld_cb, void *cb_arg);

/* AT command data channel wrapper functions */
int nethub_vchan_at_send(const void *data, uint16_t len);
int nethub_vchan_at_recv_register(virtualchan_dnld_cb_t dnld_cb, void *cb_arg);

#endif
