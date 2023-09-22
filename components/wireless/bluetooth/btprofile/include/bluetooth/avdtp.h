/** @file
 * @brief Audio/Video Distribution Transport Protocol header.
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AVDTP_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_AVDTP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define AVDTP_SEID_OFFSET 0x02
enum bt_avdtp_stream_state{
    /*State machine has been initialized*/
    AVDTP_IDLE = 0x00,
    /*INT has successfully configured an ACP’s stream end-point*/
    AVDTP_CONFIGURED,
    /*INT and ACP have successfully opened a stream end-point*/
    AVDTP_OPEN,
    /*INT and ACP have successfully established a streaming
      session on a stream end-point*/
    AVDTP_STREAMING,
    /*INT and ACP are closing a stream end-point*/
    AVDTP_CLOSING,
    /*INT or ACP has requested to abort the stream establishment*/
    AVDTP_ABORTING 
};

/** @brief AVDTP SEID Information */
struct bt_avdtp_seid_info {
	/** Stream End Point ID */
	uint8_t id:6;
	/** End Point usage status */
	uint8_t inuse:1;
	/** Reserved */
	uint8_t rfa0:1;
	/** Media-type of the End Point */
	uint8_t media_type:4;
	/** TSEP of the End Point */
	uint8_t tsep:1;
	/** Reserved */
	uint8_t rfa1:3;
} __packed;

/** @brief AVDTP Local SEP*/
struct bt_avdtp_seid_lsep {
	/** Stream End Point information */
	struct bt_avdtp_seid_info sep;
	/** Pointer to next local Stream End Point structure */
	struct bt_avdtp_seid_lsep *next;
};

/** @brief AVDTP Stream */
struct bt_avdtp_stream {
	struct bt_l2cap_br_chan chan; /* Transport Channel*/
	struct bt_avdtp_seid_info *lsep; /* Configured Local SEP */
	struct bt_avdtp_seid_info rsep; /* Configured Remote SEP*/
	uint8_t state; /* current state of the stream */
	struct bt_avdtp_stream *next;
};


/* Initialize AVDTP layer*/
int bt_avdtp_init(void);

struct bt_avdtp_seid_info *bt_avdtp_find_sep_by_id(uint8_t seId);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AVDTP_H_ */
