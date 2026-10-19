/*
 * AVCTP Header File
 *
 * Copyright (c) 2021-2022 Bouffalolab Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef _INCLUDE_BLUETOOTH_AVCTP_H_
#define _INCLUDE_BLUETOOTH_AVCTP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BT_L2CAP_PSM_AVCTP                  0x0017

#define BT_AVCTP_MAX_MTU CONFIG_BT_L2CAP_RX_MTU

#define AVCTP_PID          0x0e11

#define AVCTP_IPID_POS  0x00
#define AVCTP_CR_POS     0x01
#define AVCTP_PKT_POS   0x02
#define AVCTP_TLB_POS   0x04

#define AVCTP_CR_CMD      0
#define AVCTP_CR_RSP     1

#define AVCTP_PKT_TYPE_SIGNLE  0x0
#define AVCTP_PKT_TYPE_START         0x1
#define AVCTP_PKT_TYPE_CONTINUE      0x2
#define AVCTP_PKT_TYPE_END           0x3


#define AVCTP_CHAN(_ch) CONTAINER_OF(_ch, struct bt_avctp, br_chan.chan)

/** @brief Global avctp session structure. */
struct bt_avctp {
	struct bt_l2cap_br_chan br_chan;
	//struct bt_avctp_req *req;
};

/* initialize avctp layer*/
int bt_avctp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_BLUETOOTH_AVCTP_H_ */

