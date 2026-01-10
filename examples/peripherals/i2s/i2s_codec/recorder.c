#include "recorder.h"

#include <string.h>

static void sem_drain_local(SemaphoreHandle_t sem)
{
    if (sem == NULL) {
        return;
    }
    while (xSemaphoreTake(sem, 0) == pdTRUE) {}
}

static uint32_t gcd_u32(uint32_t a, uint32_t b)
{
    while (b != 0U) {
        uint32_t t = a % b;
        a = b;
        b = t;
    }
    return a;
}

static void rotate_left_s16(int16_t *buf, uint32_t len, uint32_t shift)
{
    if (buf == NULL || len <= 1U) {
        return;
    }
    shift %= len;
    if (shift == 0U) {
        return;
    }

    uint32_t g = gcd_u32(len, shift);
    for (uint32_t start = 0; start < g; start++) {
        int16_t tmp = buf[start];
        uint32_t j = start;
        while (1) {
            uint32_t k = j + shift;
            if (k >= len) {
                k -= len;
            }
            if (k == start) {
                break;
            }
            buf[j] = buf[k];
            j = k;
        }
        buf[j] = tmp;
    }
}

void recorder_reorder_ring_s16(int16_t *buf, uint32_t len, uint32_t write_pos, bool filled, uint32_t *out_samples)
{
    if (out_samples != NULL) {
        *out_samples = 0;
    }
    if (buf == NULL || len == 0U) {
        return;
    }

    if (!filled) {
        if (write_pos > len) {
            write_pos = len;
        }
        if (out_samples != NULL) {
            *out_samples = write_pos;
        }
        return;
    }

    if (out_samples != NULL) {
        *out_samples = len;
    }

    /* Oldest data begins at write_pos (next write index). */
    write_pos %= len;
    if (write_pos != 0U) {
        rotate_left_s16(buf, len, write_pos);
    }
}

void recorder_task(void *arg)
{
    recorder_cfg_t *cfg = (recorder_cfg_t *)arg;
    if (cfg == NULL || cfg->sample_rate_hz == 0 || cfg->duration_ms == 0 || cfg->out_channels == 0 ||
        cfg->sem_dma_done == NULL || cfg->pcm_ping == NULL || cfg->pcm_pong == NULL || cfg->pcm_fill == NULL ||
        cfg->reconfig_txrx == NULL || cfg->audio_stop == NULL || cfg->rec_enable == NULL) {
        if (cfg != NULL) {
            vPortFree(cfg);
        }
        vTaskDelete(NULL);
        return;
    }

    const uint32_t block_ms = 20U;
    const uint32_t samples_per_ch = (cfg->sample_rate_hz * block_ms) / 1000U;
    const uint32_t tx_block_bytes = samples_per_ch * (uint32_t)cfg->out_channels * (uint32_t)sizeof(int16_t);

    /* Enable ring-recording for this capture window. */
    *(cfg->rec_enable) = true;

    /* Prime ping/pong with silence and start full-duplex IO. */
    cfg->pcm_fill(cfg->pcm_ping, NULL, 0, 0, (int)cfg->out_channels, tx_block_bytes);
    cfg->pcm_fill(cfg->pcm_pong, NULL, 0, 0, (int)cfg->out_channels, tx_block_bytes);

    sem_drain_local(cfg->sem_dma_done);
    cfg->reconfig_txrx(cfg->sample_rate_hz, tx_block_bytes, cfg->out_channels);

    const TickType_t ticks_total = pdMS_TO_TICKS(cfg->duration_ms);
    const TickType_t tick_start = xTaskGetTickCount();

    bool ping = true;
    while ((xTaskGetTickCount() - tick_start) < ticks_total) {
        (void)xSemaphoreTake(cfg->sem_dma_done, portMAX_DELAY);
        int16_t *out = ping ? cfg->pcm_ping : cfg->pcm_pong;
        cfg->pcm_fill(out, NULL, 0, 0, (int)cfg->out_channels, tx_block_bytes);
        ping = !ping;
    }

    /* Stop capture first to prevent reorder races. */
    *(cfg->rec_enable) = false;

    /* Drain 2 blocks so FIFO/DMA can flush out cleanly. */
    for (int i = 0; i < 2; i++) {
        (void)xSemaphoreTake(cfg->sem_dma_done, portMAX_DELAY);
    }

    cfg->audio_stop();

    if (cfg->done_notify_task != NULL) {
        (void)xTaskNotify(cfg->done_notify_task, AUDIO_NOTIFY_REC_DONE, eSetBits);
    }

    vPortFree(cfg);
    vTaskDelete(NULL);
}
