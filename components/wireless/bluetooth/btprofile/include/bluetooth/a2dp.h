/** @file
 * @brief Advance Audio Distribution Profile header.
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_A2DP_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_A2DP_H_


#ifdef __cplusplus
extern "C" {
#endif

#define BT_A2DP_PROFILE_VERSION     0x0103
#include "avdtp.h"

/** @brief Stream Structure */
struct bt_a2dp_stream {
	/* TODO */
};

/** @brief Codec ID */
enum bt_a2dp_codec_id {
	/** Codec SBC */
	BT_A2DP_SBC = 0x00,
	/** Codec MPEG-1 */
	BT_A2DP_MPEG1 = 0x01,
	/** Codec MPEG-2 */
	BT_A2DP_MPEG2 = 0x02,
	/** Codec ATRAC */
	BT_A2DP_ATRAC = 0x04,
	/** Codec Non-A2DP */
	BT_A2DP_VENDOR = 0xff
};

/** @brief Media Codec Type */
enum MEDIA_CODEC_TYPE {
	/** SBC codec type */
	BT_A2DP_CODEC_TYPE_SBC = 0x00,
	/** AAC codec type */
	BT_A2DP_CODEC_TYPE_AAC = 0x02,
	/** AAC codec type */
	BT_A2DP_CODEC_TYPE_VENDOR = 0xff,
};

/** @brief Preset for the endpoint */
struct bt_a2dp_preset {
	/** Length of preset */
	uint8_t len;
	/** Preset */
	uint8_t preset[0];
};

/** @brief Stream End Point */
struct bt_a2dp_endpoint {
	/** Code ID */
	uint8_t codec_id;
	/** Stream End Point Information */
	struct bt_avdtp_seid_lsep info;
	/**Extend of struct bt_avdtp_seid_lsep */
	struct bt_avdtp_local_sep sep;
	/** Pointer to preset codec chosen */
	struct bt_a2dp_preset *preset;
	/** Capabilities */
	struct bt_a2dp_preset *caps;
};

struct bt_a2dp_source_endpoint {
	struct bt_a2dp_endpoint end_point;
	/**Extend of struct bt_avdtp_seid_lsep */
	struct bt_avdtp_local_sep sep;
};

/** @brief Stream Configuration */
struct bt_a2dp_config {
	/** Code Type */
	uint8_t codec_type;
	/** Channel Mode */
	uint8_t channel_mode;
	/** Samping frequency */
	uint8_t samp_freq;
	/** Allocation Method */
	uint8_t alloc_meth;
	/** Subbands */
	uint8_t sub_band;
	/** Block Length */
	uint8_t block_len;
};

/** @brief Stream End Point Media Type */
enum MEDIA_TYPE {
	/** Audio Media Type */
	BT_A2DP_AUDIO = 0x00,
	/** Video Media Type */
	BT_A2DP_VIDEO = 0x01,
	/** Multimedia Media Type */
	BT_A2DP_MULTIMEDIA = 0x02
};

/** @brief Stream End Point Role */
enum ROLE_TYPE {
	/** Source Role */
	BT_A2DP_SOURCE = 0,
	/** Sink Role */
	BT_A2DP_SINK = 1
};

/** @brief A2DP Callback State */
enum A2DP_CB_STATE {
	BT_A2DP_INVALID_STATE = 0,
	/** Chain State */
	BT_A2DP_CHAIN_CONNECTED = 1,
	/** Chain State */
	BT_A2DP_CHAIN_DISCONNECTED = 2,
    	/** Stream State */
	BT_A2DP_STREAM_START = 3,
    	/** Stream State */
	BT_A2DP_STREAM_SUSPEND = 4,
};

struct a2dp_callback {
	void (*chain)(struct bt_conn *conn, uint8_t state);
	void (*stream)(uint8_t state);
    void (*start_cfm)(void);
	void (*suspend_cfm)(void);
};

typedef struct{
    int (*write)(uint8_t *data,uint32_t size);
    int (*read)(uint8_t *data,uint32_t size);
    int (*start)(void);
    int (*stop)(void);
    int (*open)(int sample,int chl);
    int (*ioctl)(int cmd , void *arg);
    int (*close)(void);
}A2DP_PCM_PRCOESS;

void register_pcm(A2DP_PCM_PRCOESS *cb);

/** @brief A2DP structure */
struct bt_a2dp;

/** @brief Register A2DP callback.
 *
 */

/* To be called when first SEP is being registered */
int bt_a2dp_init(void);

void a2dp_cb_register(struct a2dp_callback *cb);

/** @brief A2DP Connect.
 *
 *  This function is to be called after the conn parameter is obtained by
 *  performing a GAP procedure. The API is to be used to establish A2DP
 *  connection between devices.
 *
 *  @param conn Pointer to bt_conn structure.
 *
 *  @return pointer to struct bt_a2dp in case of success or NULL in case
 *  of error.
 */
struct bt_a2dp *bt_a2dp_connect(struct bt_conn *conn);

#if BR_EDR_PTS_TEST
int bt_a2dp_disconnect(struct bt_conn *conn);
int bt_a2dp_sink_open_stream(struct bt_conn *conn);
int bt_a2dp_start_discovery(struct bt_conn *conn);
int bt_a2dp_get_cap(struct bt_conn *conn);
int bt_a2dp_set_conf(struct bt_conn *conn,uint8_t acp_seid);
int bt_a2dp_start_stream(struct bt_conn *conn);
int bt_a2dp_close_stream(struct bt_conn *conn);
int bt_a2dp_open_stream(struct bt_conn *conn);
int bt_a2dp_suspend_stream(struct bt_conn *conn);
int bt_a2dp_delay_report(struct bt_conn *conn);
#endif

/** @brief Endpoint Registration.
 *
 *  This function is used for registering the stream end points. The user has
 *  to take care of allocating the memory, the preset pointer and then pass the
 *  required arguments. Also, only one sep can be registered at a time.
 *
 *  @param endpoint Pointer to bt_a2dp_endpoint structure.
 *  @param media_type Media type that the Endpoint is.
 *  @param role Role of Endpoint.
 *
 *  @return 0 in case of success and error code in case of error.
 */
int bt_a2dp_register_endpoint(struct bt_a2dp_endpoint *endpoint,
			      uint8_t media_type, uint8_t role);

/** @brief SBC decode init.
 *
 *  @return 0 in case of success and error code in case of error.
 */
int a2dp_sbc_decode_init();

/** @brief SBC decode process.
 *
 *  @return 0 in case of success and error code in case of error.
 */
int a2dp_sbc_decode_process(uint8_t* media_data, uint16_t data_len);

void audio_run(void);
int bt_start_discovery(struct bt_conn *conn);
int bt_stream_resume(struct bt_conn *conn);
int bt_stream_suspend(struct bt_conn *conn);
int bt_a2dp_send_media(const void *buf, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_A2DP_H_ */
