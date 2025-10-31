/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */
#ifndef MULTIMEDIA_USER_H
#define MULTIMEDIA_USER_H

// xav config decoder
#define CONFIG_DECODER_ALAW                  (0)
#define CONFIG_DECODER_AMRNB                 (0)
#define CONFIG_DECODER_AMRWB                 (0)
#define CONFIG_DECODER_PCM                   (0)
#define CONFIG_DECODER_OPUS                  (0)
#define CONFIG_DECODER_VORBIS                (0)
#define CONFIG_DECODER_AAC                   (0)
#define CONFIG_DECODER_MULAW                 (0)
#define CONFIG_DECODER_PVMP3                 (0)
#define CONFIG_DECODER_SPEEX                 (0)
#define CONFIG_DECODER_ADPCM_MS              (0)
#define CONFIG_DECODER_FLAC                  (0)

// xav config streaming
#define CONFIG_STREAMER_FILE                 (0)
#define CONFIG_STREAMER_FIFO                 (0)
#define CONFIG_STREAMER_HLS                  (0)
#define CONFIG_STREAMER_HTTP                 (0)
#define CONFIG_STREAMER_CRYPTO               (0)
#define CONFIG_STREAMER_MEM                  (0)
#define CONFIG_STREAMER_DASH                 (0)
#define CONFIG_STREAMER_ASHX                 (0)
#define CONFIG_STREAMER_CUSTOM               (0)

// xav config filtering
/* Configuration switch for controlling audio speed feature.
 * If bt is enabled, CONFIG_ATEMPOER_SONIC must be enabled too */
#define CONFIG_ATEMPOER_SONIC                (0)
/* Configuration switch for controlling audio resampling with specified frequency.
 * If CONFIG_RESAMPLER_SPEEX set disabled, input audio channel only supports mono */
#define CONFIG_RESAMPLER_SPEEX               (0)
/* Configuration switch for controlling audio output mixing support. */
#define CONFIG_AO_MIXER_SUPPORT              (0)
/* Configuration switch for controlling audio effecter feature. */
#define CONFIG_AEF_EQ_ENABLE                 (0) // need sona lib

// player config settings
/* Size of the stream cache, when the value is set too low, playback stuttering may occur.*/
#define CONFIG_AV_STREAM_CACHE_SIZE_DEFAULT          (10*1024)
/* When the cache reaches the specified threshold
 * (CONFIG_AV_STREAM_CACHE_SIZE_DEFAULT*CONFIG_AV_STREAM_CACHE_THRESHOLD_DEFAULT%),
 * the player begins to play */
#define CONFIG_AV_STREAM_CACHE_THRESHOLD_DEFAULT     (80)
/* Size of the player task stack. When you're concerned about memory overhead,
 * you can reduce it, but make sure it still functions properly. */
#define CONFIG_PLAYER_TASK_STACK_SIZE                (20*1024)
/* Size of the stream task stack, used to continuously acquire streaming data. */
#define CONFIG_WEB_CACHE_TASK_STACK_SIZE             (6144)
/* Size of the buffer that demuxer used to determine the audio format. */
#define CONFIG_AV_PROBE_SIZE_MAX                     (2048)
/* Size of the sample num per frame. */
#define CONFIG_AV_SAMPLE_NUM_PER_FRAME_MAX           (320)

#define CONFIG_MSP_DEBUG                     (0)

#endif
