/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef _BL_AUDIO_OUTPUT_H_
#define _BL_AUDIO_OUTPUT_H_

#include "bl_audio.h"
#include <msp/kernel.h>
// #include <xutils/xringbuffer.h>
#include <xutils/mringbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CODEC_OUTPUT_DEBUG_TRACE    (1)

typedef struct _auo_segment {
    uint32_t        dirty;    // 0-dirty; 1-clear; 2-valid data
    uint32_t        remain;   //
    _dmahw_cfg_t    dma_cfg;   // hw node cfg
} auo_segment_t;

typedef struct {
    uint32_t                sample_rate;                ///< Output data sample rate
    uint32_t                bit_width;                  ///< Onput data sample width
    uint8_t                 *buffer;                    ///< The buffer used to save audio data
    uint32_t                buffer_size;                ///< Output buffer size
    uint32_t                per_node_size;
    uint32_t                sound_channel_num;          ///< Number of soundchannel per channel
    uint32_t                pa_pin;
} auo_cfg_t;

/* dma*/
typedef struct _auo_dma {
    // uint8_t          id;        /*dma id*/
    // uint8_t          ch;        /*dma channel*/
    uint32_t         per_node_size;
    // uint32_t         maxcount;   /* max node num */
    uint32_t         dst_addr;      /* DMA Destination address */
    uint32_t         halt_reg;
    uint32_t         halt;
    uint32_t         unhalt;
    auo_segment_t    *node;
} auo_dma_t;

#if CODEC_OUTPUT_DEBUG_TRACE
typedef struct _auo_debug_t {
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
} auo_debug_t;
#endif

typedef struct _auo_ch auo_ch_t;

typedef void (*auo_cb_t)(auo_ch_t *context, audio_codec_event_t event, void *arg);

/* audio output ch */
struct _auo_ch {
    uint32_t            ch_idx;            ///< Codec output channel idx
    auo_cb_t            callback;
    void                *arg;

    msp_task_t          task;                          /* for isr handle */
    uint8_t             task_exit;                     /* for stop task flag;0-runding,1-stoped */
    msp_event_t         event;                         /* for task event stop or isr notify */
    uint32_t            event_timeout;
    msp_mutex_t         mutex;                         /* for ringbuffer&flag ... ops */
    int                 stack_size;
    int                 task_pri;

    m_ringbuf_t         *ringbuffer;       // hdr
    uint8_t             *buffer;         ///< The csi_ringbuf used to save audio data
    uint32_t            buffer_size;                ///< Output buffer size
    uint32_t            per_node_size;
    uint8_t             ctrl_id;
    uint8_t             ch_id;
    uint32_t            maxcount;         /* max node num */
    uint32_t            ridx;
    uint32_t            widx;
    uint32_t            sound_channel_num; ///< Number of sound channel
    uint32_t            bit_width;                  ///< Onput data sample width
    auo_dma_t           *dma;              ///< Dma channel handle
    //csi_state_t         state;             ///< Codec output channel current state
    uint8_t             st;
    uint32_t            pre_indx;
    void                *priv;
    uint32_t            head;
    uint32_t            pa_pin;

#if CODEC_OUTPUT_DEBUG_TRACE
    auo_debug_t         debug;
#endif
};

int auo_init(auo_ch_t *context);
int auo_start(auo_ch_t *context);
int auo_stop(auo_ch_t *context);
int auo_pause(auo_ch_t *context);
int auo_resume(auo_ch_t *context);
uint32_t auo_write(auo_ch_t *context, const void *data, uint32_t size);
uint32_t auo_write_async(auo_ch_t *context, const void *data, uint32_t size);



int auo_attach_callback(auo_ch_t *context, auo_cb_t callback, void *arg);

uint32_t auo_buffer_avail(auo_ch_t *context);
uint32_t auo_buffer_remain(auo_ch_t *context);

int auo_channel_config(auo_ch_t *context, auo_cfg_t *config);
int auo_tx_dma_link(auo_ch_t *context, void *dma);

void audio_poweron(uint32_t pa_pin);

 #ifdef __cplusplus
 }
 #endif

 #endif /* _BL_AUDIO_OUT_H_ */

