/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#ifndef XCODEC_CODEC_DATA_H
#define XCODEC_CODEC_DATA_H

#include <stdbool.h>
#include <string.h>
#include "bflb_dma.h"
#include "msp/kernel.h"
#include "xutils/mringbuffer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

typedef struct codec_data_stream codec_data_stream_t;

struct codec_data_stream {
    struct bflb_device_s *peripheral;
    struct bflb_device_s *dma;
    void (*hw_enable)(codec_data_stream_t *s, bool enable);
    void (*hw_clear)(codec_data_stream_t *s);
    void (*copy)(codec_data_stream_t *s, uint8_t *native, uint8_t *pcm,
                 unsigned frames, uint16_t mask);
    m_ringbuf_t ring;
    uint8_t *dma_buffer;
    struct bflb_dma_channel_lli_pool_s *nodes;
    void *allocation;
    TaskHandle_t task;
    msp_mutex_t mutex;
    StaticSemaphore_t ready_storage;
    SemaphoreHandle_t ready;
    uint32_t block_bytes;
    uint32_t frame_bytes;
    uint16_t block_frames;
    uint16_t period_count;
    uint16_t last_count;
    uint16_t dma_index;
    uint8_t slots;
    uint8_t word_bytes;
    uint8_t pending_bytes;
    uint8_t pending[16 * sizeof(int16_t)];
    bool input;
    bool running;
    bool active;
    bool enabled;
    bool skip_input;
    volatile bool task_exit;
    int error;
};

int codec_data_init(codec_data_stream_t *s, struct bflb_device_s *dma,
                    const struct bflb_dma_channel_config_s *dma_cfg, uint32_t fifo_addr,
                    uint16_t block_frames, uint16_t period_count, uint8_t slots, uint8_t word_bytes);
void codec_data_deinit(codec_data_stream_t *s);
int codec_data_enable(codec_data_stream_t *s, bool enable);
int codec_data_transfer(codec_data_stream_t *s, uint8_t *buffer, int size,
                        uint16_t channel_mask, uint32_t timeout_ms);
void codec_data_complete(codec_data_stream_t *s);
void codec_data_set_enabled(codec_data_stream_t *s, bool enable);

/* Convert at the software queue boundary; applications never touch DMA memory. */
static inline void codec_data_copy_pcm16(codec_data_stream_t *s, uint8_t *native, uint8_t *pcm,
                                         unsigned frames, uint16_t mask)
{
    if (s->word_bytes == sizeof(int16_t) && mask == (1U << s->slots) - 1U) {
        memcpy(s->input ? pcm : native, s->input ? native : pcm, frames * s->frame_bytes);
        return;
    }
    if (!s->input) {
        memset(native, 0, frames * s->frame_bytes);
    }
    for (unsigned frame = 0; frame < frames; ++frame) {
        for (unsigned slot = 0; slot < s->slots; ++slot) {
            if (!(mask & (1U << slot))) {
                continue;
            }
            uint8_t *sample = native + frame * s->frame_bytes + slot * s->word_bytes;
            if (s->input) {
                memcpy(pcm, sample, sizeof(int16_t));
            } else {
                memcpy(sample, pcm, sizeof(int16_t));
            }
            pcm += sizeof(int16_t);
        }
    }
}

#endif
