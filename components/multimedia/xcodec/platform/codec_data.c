/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include "codec_data.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define EVENT_DMA 1U
#define EVENT_STOP 2U
#define ALIGN_UP(n) (((n) + BFLB_CACHE_LINE_SIZE - 1U) & ~(uintptr_t)(BFLB_CACHE_LINE_SIZE - 1U))

static void dma_stop(codec_data_stream_t *s)
{
    s->hw_enable(s, false);
    bflb_dma_channel_stop(s->dma);
    bflb_dma_channel_tcint_mask(s->dma, true);
    bflb_dma_channel_tcint_clear(s->dma);
    s->active = false;
}

static void ring_reset(codec_data_stream_t *s)
{
    dma_stop(s);
    s->hw_clear(s);
    mringbuffer_reset(&s->ring);
    s->pending_bytes = 0;
    memset(s->dma_buffer, 0, s->ring.buffer_size);
    bflb_l1c_dcache_clean_range(s->dma_buffer, s->ring.buffer_size);
    bflb_dma_feature_control(s->dma, DMA_CMD_SET_LLI_CONFIG, (size_t)s->nodes);
    s->last_count = (uint16_t)bflb_dma_feature_control(s->dma, DMA_CMD_GET_LLI_COUNT, 0);
    s->dma_index = 0;
    s->skip_input = false;
}

static void dma_start(codec_data_stream_t *s)
{
    bflb_dma_channel_tcint_mask(s->dma, false);
    bflb_dma_channel_start(s->dma);
    s->active = true;
    s->hw_enable(s, true);
}

void codec_data_complete(codec_data_stream_t *s)
{
    msp_mutex_lock(&s->mutex, MSP_WAIT_FOREVER);
    if (!s->running || !s->active) {
        msp_mutex_unlock(&s->mutex);
        return;
    }
    uint16_t count = (uint16_t)bflb_dma_feature_control(s->dma, DMA_CMD_GET_LLI_COUNT, 0);
    unsigned periods = (count - s->last_count) & 1023U;
    if (periods) {
        unsigned end = (s->dma_index + periods) % s->period_count;
        unsigned safe = periods < s->period_count ? periods : s->period_count - 1U;
        if (s->input && s->enabled && periods >= s->period_count) {
            s->error = -EOVERFLOW;
        }
        /* shortcut: worker must finish before DMA revisits a completed block; use ISR refill if a period cannot bound worker latency. */
        /* The current descriptor (end) belongs to DMA, even after merged IRQs. */
        for (unsigned i = safe; i > 0; --i) {
            unsigned index = (end + s->period_count - i) % s->period_count;
            uint8_t *native = s->dma_buffer + index * s->block_bytes;
            if (s->input) {
                if (s->enabled && !s->error && !s->skip_input) {
                    if (mringbuffer_space_len(&s->ring) < s->block_bytes) {
                        s->error = -EOVERFLOW;
                    } else {
                        bflb_l1c_dcache_invalidate_range(native, s->block_bytes);
                        mringbuffer_put(&s->ring, native, s->block_bytes);
                    }
                }
                s->skip_input = false;
            } else {
                unsigned bytes = s->enabled ? mringbuffer_data_len(&s->ring) : 0;
                if (bytes > s->block_bytes) {
                    bytes = s->block_bytes;
                }
                if (bytes) {
                    mringbuffer_get(&s->ring, native, bytes);
                }
                memset(native + bytes, 0, s->block_bytes - bytes);
                bflb_l1c_dcache_clean_range(native, s->block_bytes);
            }
        }
        s->last_count = count;
        s->dma_index = end;
        xSemaphoreGive(s->ready);
    }
    msp_mutex_unlock(&s->mutex);
}

static void data_worker(void *arg)
{
    codec_data_stream_t *s = arg;
    for (;;) {
        uint32_t flags = 0;
        xTaskNotifyWait(0, UINT32_MAX, &flags, portMAX_DELAY);
        if (flags & EVENT_STOP) {
            break;
        }
        if (flags & EVENT_DMA) {
            codec_data_complete(s);
        }
    }
    s->task_exit = true;
    vTaskDelete(NULL);
}

static void dma_irq(void *arg)
{
    codec_data_stream_t *s = arg;
    BaseType_t wake = pdFALSE;
    xTaskNotifyFromISR(s->task, EVENT_DMA, eSetBits, &wake);
    portYIELD_FROM_ISR(wake);
}

int codec_data_init(codec_data_stream_t *s, struct bflb_device_s *dma,
                    const struct bflb_dma_channel_config_s *dma_cfg, uint32_t fifo_addr,
                    uint16_t block_frames, uint16_t period_count, uint8_t slots, uint8_t word_bytes)
{
    if (period_count < 2 || period_count > 1023) {
        return -EINVAL;
    }
    s->input = dma_cfg->direction == DMA_PERIPH_TO_MEMORY;
    s->slots = slots;
    s->word_bytes = word_bytes;
    s->block_frames = block_frames;
    s->period_count = period_count;
    s->frame_bytes = slots * word_bytes;
    s->block_bytes = block_frames * s->frame_bytes;
    unsigned transfer_bytes = 1U << dma_cfg->src_width;
    if (s->block_bytes / transfer_bytes > 4095U || s->block_bytes % BFLB_CACHE_LINE_SIZE ||
        s->block_bytes % transfer_bytes) {
        return -EINVAL;
    }
    size_t descriptor_bytes = ALIGN_UP(s->period_count * sizeof(*s->nodes));
    size_t ring_bytes = s->period_count * s->block_bytes;
    s->allocation = malloc(descriptor_bytes + 2U * ring_bytes + BFLB_CACHE_LINE_SIZE - 1U);
    if (!s->allocation) {
        return -ENOMEM;
    }
    s->nodes = (void *)ALIGN_UP((uintptr_t)s->allocation);
    mringbuffer_init(&s->ring, (uint8_t *)s->nodes + descriptor_bytes, ring_bytes);
    s->dma_buffer = s->ring.buffer_ptr + ring_bytes;
    s->ready = xSemaphoreCreateBinaryStatic(&s->ready_storage);
    if (!s->ready || msp_mutex_new(&s->mutex)) {
        codec_data_deinit(s);
        return -ENOMEM;
    }
    union bflb_dma_lli_control_s control = {0};
    control.bits.TransferSize = s->block_bytes / transfer_bytes;
    control.bits.SBSize = dma_cfg->src_burst_count;
    control.bits.DBSize = dma_cfg->dst_burst_count;
    control.bits.SWidth = dma_cfg->src_width;
    control.bits.DWidth = dma_cfg->dst_width;
    control.bits.SI = dma_cfg->src_addr_inc;
    control.bits.DI = dma_cfg->dst_addr_inc;
    control.bits.I = 1;
    for (unsigned i = 0; i < s->period_count; ++i) {
        uint32_t address = (uint32_t)(uintptr_t)(s->dma_buffer + i * s->block_bytes);
        s->nodes[i] = (struct bflb_dma_channel_lli_pool_s){
            .src_addr = s->input ? fifo_addr : address,
            .dst_addr = s->input ? address : fifo_addr,
            .nextlli = (uint32_t)(uintptr_t)&s->nodes[(i + 1U) % s->period_count],
            .control = control,
        };
    }
    bflb_l1c_dcache_clean_range(s->nodes, descriptor_bytes);
    s->dma = dma;
    bflb_dma_channel_init(dma, dma_cfg);
    ring_reset(s);
    if (xTaskCreate(data_worker, "codec_io", 2048 / sizeof(StackType_t), s,
                    configMAX_PRIORITIES - 3, &s->task) != pdPASS) {
        codec_data_deinit(s);
        return -ENOMEM;
    }
    bflb_dma_channel_irq_attach(dma, dma_irq, s);
    return 0;
}

int codec_data_enable(codec_data_stream_t *s, bool enable)
{
    if (!s->allocation) {
        return -EBUSY;
    }
    msp_mutex_lock(&s->mutex, MSP_WAIT_FOREVER);
    if (s->running != enable) {
        s->running = enable;
        s->enabled = enable;
        s->error = 0;
        ring_reset(s);
        if (enable) {
            (void)xSemaphoreTake(s->ready, 0);
            dma_start(s);
        } else {
            xSemaphoreGive(s->ready);
        }
    }
    msp_mutex_unlock(&s->mutex);
    return 0;
}

void codec_data_deinit(codec_data_stream_t *s)
{
    if (s->dma) {
        (void)codec_data_enable(s, false);
        dma_stop(s);
        bflb_dma_channel_irq_detach(s->dma);
    }
    /* The worker may need the ring mutex to finish its pending completion. */
    if (s->task) {
        xTaskNotify(s->task, EVENT_STOP, eSetBits);
        while (!s->task_exit) {
            msp_msleep(1);
        }
    }
    if (s->dma) {
        bflb_dma_channel_deinit(s->dma);
    }
    if (s->mutex) {
        msp_mutex_free(&s->mutex);
    }
    if (s->ready) {
        vSemaphoreDelete(s->ready);
    }
    free(s->allocation);
    *s = (codec_data_stream_t){0};
}

void codec_data_set_enabled(codec_data_stream_t *s, bool enable)
{
    if (enable) {
        codec_data_complete(s);
    }
    msp_mutex_lock(&s->mutex, MSP_WAIT_FOREVER);
    if (s->enabled != enable) {
        s->enabled = enable;
        s->error = 0;
        mringbuffer_reset(&s->ring);
        s->pending_bytes = 0;
        /* Discard the capture period that straddles a logical open. */
        s->skip_input = s->input && enable && s->active;
        if (!s->input && !enable && s->active) {
            uint16_t count = (uint16_t)bflb_dma_feature_control(s->dma, DMA_CMD_GET_LLI_COUNT, 0);
            unsigned elapsed = (count - s->last_count) & 1023U;
            unsigned current = (s->dma_index + elapsed) % s->period_count;
            /* The active block is silenced at completion; queued blocks can be cleared now. */
            for (unsigned i = 0; i < s->period_count; ++i) {
                if (i != current) {
                    uint8_t *native = s->dma_buffer + i * s->block_bytes;
                    memset(native, 0, s->block_bytes);
                    bflb_l1c_dcache_clean_range(native, s->block_bytes);
                }
            }
        }
        xSemaphoreGive(s->ready);
    }
    msp_mutex_unlock(&s->mutex);
}

static TickType_t remaining_ticks(uint64_t start, uint32_t timeout_ms)
{
    uint64_t elapsed = (bflb_mtimer_get_time_us() - start) / 1000U;
    if (elapsed >= timeout_ms) {
        return 0;
    }
    TickType_t ticks = pdMS_TO_TICKS(timeout_ms - (uint32_t)elapsed);
    return ticks ? ticks : 1;
}

static void transfer_span(codec_data_stream_t *s, uint8_t *pcm, unsigned frames,
                          uint16_t mask)
{
    unsigned bytes = frames * s->frame_bytes;
    if (!s->copy) {
        if (s->input) {
            mringbuffer_get(&s->ring, pcm, bytes);
        } else {
            mringbuffer_put(&s->ring, pcm, bytes);
        }
        return;
    }
    unsigned index = s->input ? s->ring.read_index : s->ring.write_index;
    uint8_t *native = s->ring.buffer_ptr + index;
    s->copy(s, native, pcm, frames, mask);
    if (s->input) {
        mringbuffer_get(&s->ring, NULL, bytes);
    } else {
        mringbuffer_put(&s->ring, NULL, bytes);
    }
}

int codec_data_transfer(codec_data_stream_t *s, uint8_t *buffer, int size,
                        uint16_t mask, uint32_t timeout_ms)
{
    if (!s->allocation) {
        return -EBUSY;
    }
    unsigned channels = 0;
    for (unsigned i = 0; i < s->slots; ++i) {
        channels += !!(mask & (1U << i));
    }
    if (!buffer || size < 0) {
        return -EINVAL;
    }
    uint64_t start = bflb_mtimer_get_time_us();
    unsigned frame_bytes = channels * sizeof(int16_t);
    unsigned remaining = size;
    do {
        msp_mutex_lock(&s->mutex, MSP_WAIT_FOREVER);
        if (!s->running || !s->enabled || s->error) {
            int ret = s->error ? s->error : -EBUSY;
            msp_mutex_unlock(&s->mutex);
            return ret;
        }
        unsigned available = s->input ? mringbuffer_data_len(&s->ring) : mringbuffer_space_len(&s->ring);
        unsigned index = s->input ? s->ring.read_index : s->ring.write_index;
        unsigned span = s->ring.buffer_size - index;
        if (available > span) {
            available = span;
        }
        unsigned count = available / s->frame_bytes;
        if (count > s->block_frames) {
            count = s->block_frames;
        }
        unsigned bytes = 0;
        if (s->input && s->pending_bytes) {
            bytes = remaining < s->pending_bytes ? remaining : s->pending_bytes;
            memcpy(buffer, s->pending + frame_bytes - s->pending_bytes, bytes);
            s->pending_bytes -= bytes;
        } else if (!s->input && (s->pending_bytes || remaining < frame_bytes)) {
            unsigned needed = frame_bytes - s->pending_bytes;
            /* A complete frame must enter the ring before its final bytes are accepted. */
            if (remaining < needed || count) {
                bytes = remaining < needed ? remaining : needed;
                memcpy(s->pending + s->pending_bytes, buffer, bytes);
                s->pending_bytes += bytes;
                if (s->pending_bytes == frame_bytes) {
                    transfer_span(s, s->pending, 1, mask);
                    s->pending_bytes = 0;
                }
            }
        } else if (count) {
            if (count > remaining / frame_bytes) {
                count = remaining / frame_bytes;
            }
            if (count) {
                transfer_span(s, buffer, count, mask);
                bytes = count * frame_bytes;
            } else if (remaining) {
                transfer_span(s, s->pending, 1, mask);
                memcpy(buffer, s->pending, remaining);
                s->pending_bytes = frame_bytes - remaining;
                bytes = remaining;
            }
        }
        buffer += bytes;
        remaining -= bytes;
        msp_mutex_unlock(&s->mutex);
        if (!remaining) {
            return 0;
        }
        if (!bytes) {
            TickType_t wait = remaining_ticks(start, timeout_ms);
            if (!wait || xSemaphoreTake(s->ready, wait) != pdTRUE) {
                msp_mutex_lock(&s->mutex, MSP_WAIT_FOREVER);
                int ret = s->error ? s->error : (!s->running || !s->enabled ? -EBUSY : -ETIMEDOUT);
                msp_mutex_unlock(&s->mutex);
                return ret;
            }
        }
    } while (remaining);
    return 0;
}
