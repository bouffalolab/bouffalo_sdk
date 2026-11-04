#ifndef XCODEC_PCM_H
#define XCODEC_PCM_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    uint32_t channels;
    uint32_t bit_width;
    uint32_t slot_width;
    uint32_t fifo_sample_bytes;
    uint8_t tx_slots[4];
    uint8_t rx_stereo_slots[2];
    uint8_t recoverable_xrun;
} xcodec_pcm_layout_t;

typedef struct {
    xcodec_pcm_layout_t layout;
    uint32_t channels;
    uint32_t frame_bytes;
    uint32_t dma_frame_bytes;
    uint32_t period;
    uint32_t periods;
    uint32_t buffer_size;
} xcodec_pcm_format_t;

/* Buffer and period requests are logical PCM bytes; results include DMA storage
 * and descriptors. A zero layout keeps the existing native PCM representation. */
static inline int xcodec_pcm_format_prepare(xcodec_pcm_format_t *pcm, const xcodec_pcm_layout_t *layout,
                                            int input, uint32_t channels, uint32_t bit_width,
                                            uint32_t slot_width, uint32_t period, uint32_t buffer,
                                            uint32_t descriptor_bytes, uint32_t alignment)
{
    uint64_t dma_period, allocation;
    uint32_t frame_bytes, dma_frame_bytes;

    if (!pcm || !layout || !channels || !slot_width || slot_width % 8 ||
        !bit_width || bit_width > slot_width || !period || !buffer ||
        !alignment || (alignment & (alignment - 1))) {
        return -1;
    }
    if (layout->channels) {
        /* shortcut: only four-slot S16 in 32-bit FIFO words is adapted; extend when another layout is needed. */
        if (layout->channels != 4 || layout->bit_width != 16 || layout->slot_width != 16 ||
            layout->fifo_sample_bytes != 4 || bit_width != 16 || slot_width != 16 ||
            (input ? (channels != 2 && channels != 4) : (channels != 1 && channels != 2))) {
            return -1;
        }
        for (size_t i = 0; i < 4; i++) {
            if (layout->tx_slots[i] >= 2) {
                return -1;
            }
        }
        if (layout->rx_stereo_slots[0] >= 4 || layout->rx_stereo_slots[1] >= 4) {
            return -1;
        }
    }
    if (channels > UINT32_MAX / (slot_width / 8)) {
        return -1;
    }
    frame_bytes = channels * (slot_width / 8);
    dma_frame_bytes = layout->channels ? layout->channels * layout->fifo_sample_bytes : frame_bytes;
    if (period % frame_bytes || buffer % period || buffer / period < 2) {
        return -1;
    }
    dma_period = (uint64_t)(period / frame_bytes) * dma_frame_bytes;
    if (dma_period > UINT32_MAX || dma_period % alignment) {
        return -1;
    }
    allocation = (dma_period + descriptor_bytes) * (buffer / period) + 2ULL * (alignment - 1U);
    if (allocation > UINT32_MAX) {
        return -1;
    }
    *pcm = (xcodec_pcm_format_t){
        .layout = *layout, .channels = channels, .frame_bytes = frame_bytes,
        .dma_frame_bytes = dma_frame_bytes, .period = (uint32_t)dma_period,
        .periods = buffer / period, .buffer_size = (uint32_t)allocation,
    };
    return 0;
}

static inline void xcodec_pcm_pack(uint32_t *fifo, const void *data, size_t frames,
                                   const xcodec_pcm_format_t *pcm)
{
    const uint8_t *samples = data;
    for (size_t i = 0; i < frames; i++) {
        for (size_t slot = 0; slot < pcm->layout.channels; slot++) {
            uint16_t sample;
            size_t channel = pcm->channels == 1 ? 0 : pcm->layout.tx_slots[slot];
            memcpy(&sample, samples + (i * pcm->channels + channel) * sizeof(sample), sizeof(sample));
            fifo[i * pcm->layout.channels + slot] = sample;
        }
    }
}

static inline void xcodec_pcm_unpack(void *data, const uint32_t *fifo, size_t frames,
                                     const xcodec_pcm_format_t *pcm)
{
    uint8_t *samples = data;
    for (size_t i = 0; i < frames; i++) {
        for (size_t channel = 0; channel < pcm->channels; channel++) {
            size_t slot = pcm->channels == pcm->layout.channels ? channel : pcm->layout.rx_stereo_slots[channel];
            uint16_t sample = (uint16_t)fifo[i * pcm->layout.channels + slot];
            memcpy(samples + (i * pcm->channels + channel) * sizeof(sample), &sample, sizeof(sample));
        }
    }
}

#endif
