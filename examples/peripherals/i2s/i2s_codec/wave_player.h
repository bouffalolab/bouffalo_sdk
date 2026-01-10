#ifndef WAVE_PLAYER_H
#define WAVE_PLAYER_H

#include "ring_buffer.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Task notification bit used by the demo flow controller (audio_flow_task). */
#define AUDIO_NOTIFY_WAVE_DONE (1UL << 2)

typedef void (*wave_player_pcm_fill_fn)(int16_t *dst, const int16_t *src, int samples_per_ch, int src_channels,
                                       int dst_channels, uint32_t dst_block_bytes);

typedef void (*wave_player_reconfig_fn)(uint32_t hz, uint32_t tx_block_bytes, uint8_t channels);

typedef void (*wave_player_stop_fn)(void);

typedef struct {
    /* RAW PCM source (16-bit LE, interleaved by channels) */
    Ring_Buffer_Type *src_rb;
    SemaphoreHandle_t sem_rb_space;
    volatile bool *src_done;

    /* PCM format to play */
    uint32_t sample_rate_hz;
    uint8_t src_channels;

    /* I2S/DMA sink */
    uint8_t out_channels;
    SemaphoreHandle_t sem_dma_done;
    int16_t *pcm_ping;
    int16_t *pcm_pong;

    /* Platform hooks implemented in main.c */
    wave_player_pcm_fill_fn pcm_fill;
    wave_player_reconfig_fn reconfig_txrx;
    wave_player_stop_fn audio_stop;

    /* Optional completion notify */
    TaskHandle_t done_notify_task;
} wave_player_cfg_t;

void wave_player_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* WAVE_PLAYER_H */
