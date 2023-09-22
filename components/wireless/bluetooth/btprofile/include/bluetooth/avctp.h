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

