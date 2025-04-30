/** @file
 * @brief Advance Audio Distribution Profile - SBC Codec header.
 */
/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_A2DP_CODEC_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_A2DP_CODEC_H_

#include "sbc_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Sampling Frequency */
#define A2DP_SBC_SAMP_FREQ_16000 BIT(7)
#define A2DP_SBC_SAMP_FREQ_32000 BIT(6)
#define A2DP_SBC_SAMP_FREQ_44100 BIT(5)
#define A2DP_SBC_SAMP_FREQ_48000 BIT(4)

/* Channel Mode */
#define A2DP_SBC_CH_MODE_MONO  BIT(3)
#define A2DP_SBC_CH_MODE_DUAL  BIT(2)
#define A2DP_SBC_CH_MODE_STREO BIT(1)
#define A2DP_SBC_CH_MODE_JOINT BIT(0)

/* Block Length */
#define A2DP_SBC_BLK_LEN_4  BIT(7)
#define A2DP_SBC_BLK_LEN_8  BIT(6)
#define A2DP_SBC_BLK_LEN_12 BIT(5)
#define A2DP_SBC_BLK_LEN_16 BIT(4)

/* Subbands */
#define A2DP_SBC_SUBBAND_4 BIT(3)
#define A2DP_SBC_SUBBAND_8 BIT(2)

/* Allocation Method */
#define A2DP_SBC_ALLOC_MTHD_SNR      BIT(1)
#define A2DP_SBC_ALLOC_MTHD_LOUDNESS BIT(0)

#define BT_A2DP_SBC_SAMP_FREQ(preset)    ((preset->config[0] >> 4) & 0x0f)
#define BT_A2DP_SBC_CHAN_MODE(preset)    ((preset->config[0]) & 0x0f)
#define BT_A2DP_SBC_BLK_LEN(preset)      ((preset->config[1] >> 4) & 0x0f)
#define BT_A2DP_SBC_SUB_BAND(preset)     ((preset->config[1] >> 2) & 0x03)
#define BT_A2DP_SBC_ALLOC_MTHD(preset)   ((preset->config[1]) & 0x03)

/* Codec Types */
#define A2DP_CODEC_SBC		0x00
#define A2DP_CODEC_MPEG12	0x01
#define A2DP_CODEC_MPEG24	0x02
#define A2DP_CODEC_ATRAC	0x04
#define A2DP_CODEC_VENDOR	0xff

/** @brief SBC Codec */
struct bt_a2dp_codec_sbc_params {
	/** First two octets of configuration */
	uint8_t config[2];
	/** Minimum Bitpool Value */
	uint8_t min_bitpool;
	/** Maximum Bitpool Value */
	uint8_t max_bitpool;
} __packed;

typedef struct {
	uint8_t channel_mode:4;
	uint8_t frequency:4;
	uint8_t allocation_method:2;
	uint8_t subbands:2;
	uint8_t block_length:4;
	uint8_t min_bitpool;
	uint8_t max_bitpool;
} __packed a2dp_sbc_t;

typedef struct {
	SBC_ENC_PARAMS context;
	int num_data_bytes;
	uint8_t sbc_packet[1030];
} bt_encoder_t;

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_A2DP_CODEC_H_ */
