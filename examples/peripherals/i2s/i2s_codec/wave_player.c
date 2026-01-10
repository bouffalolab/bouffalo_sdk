#include "wave_player.h"

#include "bflb_l1c.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define WAVE_FRAME_MS 20U

static uint32_t wave_calc_samples_per_ch(uint32_t hz)
{
    uint32_t s = (hz * WAVE_FRAME_MS) / 1000U;
    return (s == 0) ? 1U : s;
}

static uint32_t wave_read_pcm_bytes(wave_player_cfg_t *cfg, uint8_t *dst, uint32_t want)
{
    if (cfg == NULL || cfg->src_rb == NULL || dst == NULL || want == 0) {
        return 0;
    }

    uint32_t got = 0;
    while (got < want) {
        uint32_t avail = Ring_Buffer_Get_Length(cfg->src_rb);
        if (avail == 0) {
            if (cfg->src_done != NULL && *cfg->src_done) {
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        uint32_t to_read = want - got;
        if (to_read > avail) {
            to_read = avail;
        }

        uint32_t r = Ring_Buffer_Read(cfg->src_rb, dst + got, to_read);
        if (r > 0) {
            got += r;
            if (cfg->sem_rb_space != NULL) {
                xSemaphoreGive(cfg->sem_rb_space);
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }

    return got;
}

void wave_player_task(void *arg)
{
    wave_player_cfg_t *cfg = (wave_player_cfg_t *)arg;

    if (cfg == NULL || cfg->src_rb == NULL || cfg->src_done == NULL || cfg->pcm_ping == NULL || cfg->pcm_pong == NULL ||
        cfg->pcm_fill == NULL || cfg->reconfig_txrx == NULL || cfg->audio_stop == NULL || cfg->sem_dma_done == NULL) {
        printf("wave_player: bad cfg\r\n");
        vTaskDelete(NULL);
        return;
    }

    uint8_t src_channels = cfg->src_channels;
    if (src_channels == 0) {
        src_channels = 1;
    }
    if (src_channels > 2) {
        src_channels = 2;
    }

    uint8_t out_channels = cfg->out_channels;
    if (out_channels == 0) {
        out_channels = 1;
    }
    if (out_channels > 2) {
        out_channels = 2;
    }

    uint32_t hz = cfg->sample_rate_hz;
    if (hz == 0) {
        hz = 16000;
    }

    const uint32_t samples_per_ch = wave_calc_samples_per_ch(hz);
    const uint32_t src_block_bytes = samples_per_ch * (uint32_t)src_channels * (uint32_t)sizeof(int16_t);
    const uint32_t tx_block_bytes = samples_per_ch * (uint32_t)out_channels * (uint32_t)sizeof(int16_t);

    int16_t *src_pcm = (int16_t *)pvPortMalloc((size_t)samples_per_ch * (size_t)src_channels * sizeof(int16_t));
    if (src_pcm == NULL) {
        printf("wave_player: alloc scratch failed\r\n");
        vTaskDelete(NULL);
        return;
    }

    bool ping = true;
    bool need_prime_pong = false;
    bool dma_started = false;
    uint32_t drain = 0;

    while (1) {
        uint32_t got = wave_read_pcm_bytes(cfg, (uint8_t *)src_pcm, src_block_bytes);
        if (got < src_block_bytes) {
            memset(((uint8_t *)src_pcm) + got, 0, src_block_bytes - got);
        }

        bool eos_now =
            (cfg->src_done != NULL && *cfg->src_done && Ring_Buffer_Get_Length(cfg->src_rb) == 0 && got == 0);

        if (!dma_started) {
            cfg->pcm_fill(cfg->pcm_ping, src_pcm, (int)samples_per_ch, (int)src_channels, (int)out_channels,
                          tx_block_bytes);
            dma_started = true;
            ping = true;
            need_prime_pong = true;
            if (eos_now) {
                drain = 1;
            }
            continue;
        }

        if (need_prime_pong) {
            cfg->pcm_fill(cfg->pcm_pong, src_pcm, (int)samples_per_ch, (int)src_channels, (int)out_channels,
                          tx_block_bytes);
            cfg->reconfig_txrx(hz, tx_block_bytes, out_channels);
            need_prime_pong = false;
            if (eos_now) {
                drain = 1;
            }
            continue;
        }

        xSemaphoreTake(cfg->sem_dma_done, portMAX_DELAY);

        int16_t *out = ping ? cfg->pcm_ping : cfg->pcm_pong;
        if (eos_now) {
            cfg->pcm_fill(out, NULL, 0, 0, (int)out_channels, tx_block_bytes);
            drain++;
        } else {
            cfg->pcm_fill(out, src_pcm, (int)samples_per_ch, (int)src_channels, (int)out_channels, tx_block_bytes);
            drain = 0;
        }
        ping = !ping;

        if (drain >= 2) {
            break;
        }
    }

    printf("wave playback done\r\n");
    cfg->audio_stop();
    if (cfg->done_notify_task != NULL) {
        (void)xTaskNotify(cfg->done_notify_task, AUDIO_NOTIFY_WAVE_DONE, eSetBits);
    }

    vPortFree(src_pcm);
    vTaskDelete(NULL);
}
