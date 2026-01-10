#ifndef MP3_PLAYER_H
#define MP3_PLAYER_H

#include "ring_buffer.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Max PCM per MP3 frame:
 * MPEG-1: 1152 samples per channel. Stereo => 2304 int16.
 */
#define PVMP3_MAX_SAMPLES_PER_CH 1152
#define PVMP3_MAX_SAMPLES_TOTAL  (PVMP3_MAX_SAMPLES_PER_CH * 2)

/* Task notification bits used by the demo flow controller (audio_flow_task).
 * Scheme A: per-track feeder task + per-track decoder task.
 */
#define AUDIO_NOTIFY_MP3_DONE (1UL << 0)
#define AUDIO_NOTIFY_SRC_DONE (1UL << 1)

typedef void (*mp3_player_pcm_fill_fn)(int16_t *dst, const int16_t *src, int samples_per_ch, int src_channels,
                                      int dst_channels, uint32_t dst_block_bytes);

typedef void (*mp3_player_reconfig_fn)(uint32_t hz, uint32_t tx_block_bytes, uint8_t channels);

typedef void (*mp3_player_stop_fn)(void);

typedef struct {
    /* MP3 bitstream source */
    Ring_Buffer_Type *src_rb;
    SemaphoreHandle_t sem_rb_space;
    volatile bool *src_done;

    /* I2S/DMA sink */
    uint8_t out_channels;
    SemaphoreHandle_t sem_dma_done;
    int16_t *pcm_ping;
    int16_t *pcm_pong;

    /* Platform hooks implemented in main.c */
    mp3_player_pcm_fill_fn pcm_fill;
    mp3_player_reconfig_fn reconfig_txrx;
    mp3_player_stop_fn audio_stop;

    /* Optional completion notify */
    TaskHandle_t done_notify_task;
} mp3_player_cfg_t;

void mp3_player_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* MP3_PLAYER_H */
