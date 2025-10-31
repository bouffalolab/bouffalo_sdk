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
	/** Reserved */
	uint8_t rfa0:1;
	/** End Point usage status */
	uint8_t inuse:1;
	/** Stream End Point ID */
	uint8_t id:6;
	/** Reserved */
	uint8_t rfa1:3;
	/** TSEP of the End Point */
	uint8_t tsep:1;
	/** Media-type of the End Point */
	uint8_t media_type:4;
} __packed;

/** @brief AVDTP Local SEP*/
struct bt_avdtp_seid_lsep {
	/** Stream End Point information */
	struct bt_avdtp_seid_info sep;
	/** Pointer to next local Stream End Point structure */
	struct bt_avdtp_seid_lsep *next;
};

struct bt_avdtp_local_sep {
	struct bt_avdtp_stream *stream;
	struct bt_avdtp_seid_info info;
	uint8_t codec;
	uint8_t delay_reporting;
	void *caps;
	void *user_data;
};

/** @brief AVDTP Stream */
struct bt_avdtp_stream {
	struct bt_avdtp *avdtp;
	struct bt_l2cap_br_chan chan; /* Transport Channel*/
	struct bt_avdtp_seid_info *lsep; /* Configured Local SEP */
	struct bt_avdtp_seid_info rsep; /* Configured Remote SEP*/
	struct bt_avdtp_local_sep *lsep_ext;
	struct bt_avdtp_remote_sep *rsep_ext;
	enum bt_avdtp_stream_state state; /* current state of the stream */
	uint8_t delay_reporting:1;
	uint8_t open_acp:1;
	uint8_t close_int:1;
	uint8_t starting:1;
	uint8_t abort_int:1;
	uint16_t delay;
	struct k_delayed_work delay_work; /* Start timer */
	void *caps;
	struct bt_avdtp_stream *next;
	#if BR_EDR_PTS_TEST
	atomic_t    flags[1]; /* For internal use only */
	#endif
};


/* Initialize AVDTP layer*/
int bt_avdtp_init(void);

struct bt_avdtp_seid_info *bt_avdtp_find_sep_by_id(uint8_t seId);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AVDTP_H_ */
