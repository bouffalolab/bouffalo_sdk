/**
 * @file audio.h
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "bflb_core.h"
#include "audio_conf.h"

#if defined AUDIO_I2S_ES8388

#include "i2s_codec/es8388_i2s.h"

#define _AUDIO_FUNC_DEFINE(_func, ...) es8388_##_func(__VA_ARGS__)

#elif defined AUDIO_I2S_ES8311

#include "i2s_codec/es8311_i2s.h"

#else
#error "Please select a audio codec type"
#endif

// 音频状态枚举 audio status
#define AUDIO_IDLE      0x0  // 空闲状态
#define AUDIO_PLAYING   0x1  // 播放状态
#define AUDIO_PAUSED    0x2  // 暂停状态
#define AUDIO_RECORDING 0x3  // 录制状态
#define AUDIO_ECHO      0x4  // 边录边播状态

// 音频格式枚举 audio format
#define FORMAT_PCM 0x0    // PCM格式
#define FORMAT_WAV 0x1    // WAV格式
#define FORMAT_MP3 0x2    // MP3格式
#define FORMAT_AAC 0x3    // AAC格式

typedef struct {
    int16_t *data;  // PCM数据缓冲区指针
    size_t length;  // 数据长度
} pcm_data;

typedef struct {
    char chunk_id[4];      // "RIFF"
    uint32_t chunk_size;   // 文件总长度
    char format[4];        // "WAVE"
    char subchunk1_id[4];  // "fmt "
    uint32_t subchunk1_size;  // 子块1的长度
    uint16_t audio_format;    // 音频格式，如PCM为1
    uint16_t num_channels;    // 声道数
    uint32_t sample_rate;     // 采样率
    uint32_t byte_rate;       // 每秒字节数
    uint16_t block_align;     // 每个采样块的字节数
    uint16_t bits_per_sample;  // 每个样本的位数
    char subchunk2_id[4];  // "data"
    uint32_t subchunk2_size;  // 子块2的长度
    int16_t *data;   // 音频数据
} wav_file;

typedef struct {
    // MP3文件的标签头信息
    char tag[3];            // 标签标识，固定为"TAG"
    char title[30];         // 标题
    char artist[30];        // 艺术家
    char album[30];         // 专辑
    char year[4];           // 年份
    char comment[30];       // 注释
    uint8_t genre;          // 流派
    // MP3音频数据
    int16_t *data;          // 音频数据
    size_t length;          // 数据长度
} mp3_file;

typedef struct {
    // AAC文件的头信息
    uint16_t syncword;          // 同步字，固定为0xFFF
    uint8_t layer;              // MPEG音频层，固定为0
    uint8_t protection_absent;  // 标识数据是否进行了CRC校验
    uint8_t profile;            // AAC音频配置文件
    uint8_t sampling_freq_index;  // 采样率索引
    uint8_t private_bit;        // 保留位
    uint8_t channel_config;     // 声道配置
    uint8_t original_copy;      // 原始拷贝标志
    uint8_t home;               // 家庭相关标志
    // AAC音频数据
    int16_t *data;              // 音频数据
    size_t length;              // 数据长度
} aac_file;

// 缓冲 buffer
struct audio_buffer {
    uint32_t *tx_buf;
    size_t tx_len;
    uint32_t *rx_buf;
    size_t rx_len;
};

struct audio_cfg {
    int16_t *audio_data;
    size_t audio_length;
    uint32_t sample_rate;
    uint8_t audio_status;
    uint8_t audio_format;
    uint8_t audio_bit_depth;
    uint8_t audio_channels;
    uint32_t *audio_event_cb;
    struct audio_buffer buffers;
};

typedef void (*audio_event_callback)(uint8_t audio_status);

#ifdef __cplusplus
extern "C" {
#endif

int audio_init(struct audio_cfg *cfg);

int audio_play(struct audio_cfg *cfg);
// void audio_pause(void);
// void audio_resume(void);
// void audio_stop(void);
// void audio_set_volume(uint8_t volume);

// void audio_record(struct audio_cfg *cfg);
// void audio_stop_recording(void);

// void audio_decode(void *audio, struct audio_cfg *audio_cfg);
// void audio_encode(struct audio_cfg *audio_cfg, void *audio);

// // 初始化I2S接口及相关配置
// void i2s_init(void);

// // 控制I2S数据传输
// void i2s_start(void);
// void i2s_stop(void);

// // 从I2S接收数据和向I2S发送数据
// void i2s_write(uint8_t *data, size_t length);
// void i2s_read(uint8_t *buffer, size_t length);

// 获取当前音频状态
int audio_get_status(void);

#ifdef __cplusplus
}
#endif

#endif  // _AUDIO_H_