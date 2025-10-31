/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef _BL_AUDIO_INPUT_H_
#define _BL_AUDIO_INPUT_H_

#include "bl_audio.h"
#include <msp/kernel.h>
// #include <xutils/xringbuffer.h>
#include <xutils/mringbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CODEC_INPUT_DEBUG_TRACE           (1)

#define DEF_AUDIO_ANALOG_GAIN_INPUT1      (33)//33db,   check 0 ? or 6~42(step 3 db)
#define DEF_AUDIO_ANALOG_GAIN_INPUT2      (33)//33db,   check 0 ? or 6~42(step 3 db)
#define DEF_AUDIO_ANALOG_GAIN_INPUT3      (0) // 0db,   check 0 ? or 6~42(step 3 db)

#define DEF_AUDIO_DIGITAL_VOLUME_INPUT1   (0)
#define DEF_AUDIO_DIGITAL_VOLUME_INPUT2   (0)
#define DEF_AUDIO_DIGITAL_VOLUME_INPUT3   (0)

typedef struct _aui_segment {
    uint32_t        dirty;    // 0-dirty; 1-clear; 2-valid data
    uint32_t        remain;   //
    _dmahw_cfg_t    dma_cfg;   // hw node cfg
} aui_segment_t;

typedef struct {
    uint32_t                sample_rate;                ///< Output data sample rate
    uint32_t                bit_width;                  ///< Onput data sample width
    uint8_t                 *buffer;                    ///< The buffer used to save audio data
    uint32_t                buffer_size;                ///< Output buffer size
    uint32_t                per_node_size;
    uint32_t                sound_channel_num;          ///< Number of soundchannel per channel
    uint32_t                mic_type;                   ///< MIC type: 1 is digital, 0 is analog
    uint32_t                pdm_channel;                ///< used for digital,pdm0, pdm1
    uint32_t                pdm_in_pin;                 ///< used for digital
    uint32_t                pdm_clk_pin;                ///< used for digital
} aui_cfg_t;

/* dma*/
typedef struct _aui_dma {
    // uint8_t          id;         /*dma id*/
    // uint8_t          ch;         /*dma channel*/
    uint32_t         per_node_size;
    // uint32_t         maxcount;   /* max node num */
    uint32_t         src_addr;      /* DMA source address */
    aui_segment_t    *node;
} aui_dma_t;

typedef struct _aui_ch aui_ch_t;

typedef void (*aui_cb_t)(aui_ch_t *context, audio_codec_event_t event, void *arg);

#if CODEC_INPUT_DEBUG_TRACE
typedef struct _aui_debug_t {
    uint32_t            count_init;
    uint32_t            count_config;
    uint32_t            count_reset;
    uint32_t            count_start;
    uint32_t            count_stop;
    uint32_t            count_read;
    uint32_t            count_pause;
    uint32_t            count_resume;
    uint32_t            bytes_read;
    uint32_t            bytes_write;
} aui_debug_t;
#endif

/* audio input ch */
struct _aui_ch {
    uint32_t            ch_idx;            ///< Codec output channel idx
    aui_cb_t            callback;
    void                *arg;

    msp_task_t          task;                          /* for isr handle */
    uint8_t             task_exit;                     /* for stop task flag;0-runding,1-stoped */
    msp_event_t         event;                         /* for task event stop or isr notify */
    uint32_t            event_timeout;
    msp_mutex_t         mutex;                         /* for ringbuffer&flag ... ops */
    int                 stack_size;
    int                 task_pri;

    m_ringbuf_t         *ringbuffer;
    uint8_t             *buffer;         ///< The csi_ringbuf used to save audio data
    uint32_t            buffer_size;                ///< Output buffer size
    uint32_t            per_node_size;
    uint8_t             ctrl_id;
    uint8_t             ch_id;
    uint32_t            maxcount;         /* max node num */
    uint32_t            sound_channel_num; ///< Number of sound channel
    uint32_t            bit_width;                  ///< Onput data sample width
    aui_dma_t           *dma;              ///< Dma channel handle
    //csi_state_t               state;             ///< Codec output channel current state
    uint8_t             st;
    uint32_t            pre_indx;
    void                *priv;
    uint32_t            head;
    uint32_t            mic_type;                   ///< MIC type: 1 is digital, 0 is analog
    uint32_t            pdm_channel;                ///< used for digital,pdm0, pdm1
    uint32_t            pdm_in_pin;                 ///< used for digital
    uint32_t            pdm_clk_pin;                ///< used for digital

#if CODEC_INPUT_DEBUG_TRACE
    aui_debug_t         debug;
#endif
};

int aui_init(aui_ch_t *context);
int aui_start(aui_ch_t *context);
int aui_stop(aui_ch_t *context);
int aui_pause(aui_ch_t *context);
int aui_resume(aui_ch_t *context);
uint32_t aui_write(aui_ch_t *context, const void *data, uint32_t size);
uint32_t aui_write_async(aui_ch_t *context, const void *data, uint32_t size);

int aui_attach_callback(aui_ch_t *context, aui_cb_t callback, void *arg);

uint32_t aui_buffer_avail(aui_ch_t *context);
uint32_t aui_buffer_remain(aui_ch_t *context);

int aui_channel_config(aui_ch_t *context, aui_cfg_t *config);
int aui_rx_dma_link(aui_ch_t *context, void *dma);
uint32_t aui_read(aui_ch_t *context, const void *data, uint32_t size);

 #ifdef __cplusplus
 }
 #endif

 #endif /* _BL_AUDIO_INPUT_H_ */

