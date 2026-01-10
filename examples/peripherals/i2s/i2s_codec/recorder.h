#ifndef RECORDER_H
#define RECORDER_H

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Task notification bit used by the demo flow controller (audio_flow_task). */
#define AUDIO_NOTIFY_REC_DONE (1UL << 3)

typedef void (*recorder_pcm_fill_fn)(int16_t *dst, const int16_t *src, int samples_per_ch, int src_channels,
                                    int dst_channels, uint32_t dst_block_bytes);

typedef void (*recorder_reconfig_fn)(uint32_t hz, uint32_t tx_block_bytes, uint8_t channels);

typedef void (*recorder_stop_fn)(void);

typedef struct {
    /* Desired capture/playback clock domain */
    uint32_t sample_rate_hz;

    /* Recorder runs for this time, then stops (ms). */
    uint32_t duration_ms;

    /* I2S/DMA sink */
    uint8_t out_channels;
    SemaphoreHandle_t sem_dma_done;
    int16_t *pcm_ping;
    int16_t *pcm_pong;

    /* Platform hooks implemented in main.c */
    recorder_pcm_fill_fn pcm_fill;
    recorder_reconfig_fn reconfig_txrx;
    recorder_stop_fn audio_stop;

    /* Ring-buffer recording gate owned by main.c (consumed by audio_cap_proc). */
    volatile bool *rec_enable;

    /* Optional completion notify */
    TaskHandle_t done_notify_task;
} recorder_cfg_t;

void recorder_task(void *arg);

/* Linearize a ring-buffer audio snapshot in-place.
 *
 * buf: ring buffer (mono int16 samples)
 * len: total ring capacity (samples)
 * write_pos: next-write index when paused
 * filled: whether the ring wrapped at least once
 * out_samples: returns number of valid samples to play (<= len)
 */
void recorder_reorder_ring_s16(int16_t *buf, uint32_t len, uint32_t write_pos, bool filled, uint32_t *out_samples);

#ifdef __cplusplus
}
#endif

#endif /* RECORDER_H */
