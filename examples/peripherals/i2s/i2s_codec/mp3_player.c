#include "mp3_player.h"
#include "bflb_l1c.h"
/* MP3 decode (pvmp3dec) */
#include "pvmp3dec/pvmp3decoder_api.h"

/* Explicit FreeRTOS includes for toolchain/IDE parsers. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* pvmp3dec prints version via msp_print_version(); provide a stub for this example. */
__attribute__((weak)) void msp_print_version(const char *format, ...)
{
    (void)format;
}

static int find_mp3_sync(const uint8_t *buf, size_t len)
{
    if (buf == NULL || len < 2) {
        return -1;
    }

    /* Basic MP3 frame sync search: 0xFF followed by 0xE0 mask set.
     * This is not a full header validation, but good enough to avoid
     * slow byte-by-byte retries on ID3 tags.
     */
    for (size_t i = 0; i + 1 < len; i++) {
        if (buf[i] == 0xFF && (buf[i + 1] & 0xE0) == 0xE0) {
            return (int)i;
        }
    }
    return -1;
}

void mp3_player_task(void *arg)
{
    int last_info_hz = -1;
    int last_info_ch = -1;
    int16_t *mp3_pcm_tmp = NULL;
    uint8_t *mp3_cache = NULL;
    size_t mp3_cache_len = 0;
    uint32_t current_tx_block_bytes = 0;
    int current_hz = 0;
    uint32_t msize;
    void *pvmp3_dbuf = NULL;
    tPVMP3DecoderExternal pvmp3_ext;
    mp3_player_cfg_t *cfg = (mp3_player_cfg_t *)arg;

    if (cfg == NULL || cfg->src_rb == NULL || cfg->pcm_ping == NULL || cfg->pcm_pong == NULL || cfg->pcm_fill == NULL ||
        cfg->reconfig_txrx == NULL || cfg->audio_stop == NULL || cfg->sem_dma_done == NULL || cfg->src_done == NULL) {
        printf("mp3_player: bad cfg\r\n");
        vTaskDelete(NULL);
        return;
    }

    msize = pvmp3_decoderMemRequirements();
    pvmp3_dbuf = pvPortMalloc(msize);
    if (pvmp3_dbuf == NULL) {
        printf("pvmp3: alloc %u failed\r\n", (unsigned)msize);
        vTaskDelete(NULL);
        return;
    }

    mp3_pcm_tmp = (int16_t *)pvPortMalloc((size_t)PVMP3_MAX_SAMPLES_TOTAL * sizeof(int16_t));
    mp3_cache = (uint8_t *)pvPortMalloc(4096);
    if (mp3_pcm_tmp == NULL || mp3_cache == NULL) {
        printf("mp3_player: alloc scratch failed\r\n");
        if (mp3_pcm_tmp != NULL) {
            vPortFree(mp3_pcm_tmp);
        }
        if (mp3_cache != NULL) {
            vPortFree(mp3_cache);
        }
        vPortFree(pvmp3_dbuf);
        vTaskDelete(NULL);
        return;
    }

    memset(&pvmp3_ext, 0, sizeof(pvmp3_ext));
    pvmp3_InitDecoder(&pvmp3_ext, pvmp3_dbuf);
    pvmp3_ext.crcEnabled = false;
    pvmp3_ext.equalizerType = flat;
    pvmp3_ext.pOutputBuffer = mp3_pcm_tmp;

    bool ping = true;
    bool dma_started = false;
    bool need_prime_pong = false;

    TickType_t eos_tick_start = 0;

    while (1) {
        /* If source finished and ringbuffer is drained, ensure we don't spin forever
         * on a truncated/garbage tail in mp3_cache.
         */
        if (*cfg->src_done && Ring_Buffer_Get_Length(cfg->src_rb) == 0) {
            if (eos_tick_start == 0) {
                eos_tick_start = xTaskGetTickCount();
            } else {
                TickType_t elapsed = xTaskGetTickCount() - eos_tick_start;
                if (elapsed > pdMS_TO_TICKS(300)) {
                    break;
                }
            }
        } else {
            eos_tick_start = 0;
        }

        /* keep cache filled */
        if (mp3_cache_len < 1024) {
            uint32_t avail = Ring_Buffer_Get_Length(cfg->src_rb);
            if (avail == 0) {
                /* No new bytes available right now.
                 * - If cache is empty: wait (or exit if source done)
                 * - If cache has data: try to sync/decode with what we have
                 */
                if (mp3_cache_len == 0) {
                    if (*cfg->src_done) {
                        break;
                    }
                    vTaskDelay(pdMS_TO_TICKS(1));
                    continue;
                }
                if (!(*cfg->src_done)) {
                    /* Avoid a tight loop while waiting for more input. */
                    vTaskDelay(pdMS_TO_TICKS(1));
                }
            }

            uint32_t space = (uint32_t)(4096U - mp3_cache_len);
            uint32_t to_read = (avail < space) ? avail : space;
            uint32_t got = Ring_Buffer_Read(cfg->src_rb, &mp3_cache[mp3_cache_len], to_read);
            if (got > 0) {
                mp3_cache_len += got;
                if (cfg->sem_rb_space != NULL) {
                    xSemaphoreGive(cfg->sem_rb_space);
                }
            }
        }

        int sync_off = find_mp3_sync(mp3_cache, mp3_cache_len);
        if (sync_off > 0) {
            mp3_cache_len -= (size_t)sync_off;
            memmove(mp3_cache, mp3_cache + sync_off, mp3_cache_len);
        } else if (sync_off < 0) {
            /* no sync yet: keep last byte (sync may straddle boundary) */
            if (mp3_cache_len > 1) {
                mp3_cache[0] = mp3_cache[mp3_cache_len - 1];
                mp3_cache_len = 1;
            }
            if (*cfg->src_done && Ring_Buffer_Get_Length(cfg->src_rb) == 0) {
                /* End-of-stream */
                break;
            }
            if (!(*cfg->src_done) && Ring_Buffer_Get_Length(cfg->src_rb) == 0) {
                /* Waiting for more bytes to find sync. */
                vTaskDelay(pdMS_TO_TICKS(1));
            }
            continue;
        }

        pvmp3_ext.pInputBuffer = mp3_cache;
        pvmp3_ext.inputBufferCurrentLength = (int32)mp3_cache_len;
        pvmp3_ext.inputBufferUsedLength = 0;
        pvmp3_ext.outputFrameSize = (int32)PVMP3_MAX_SAMPLES_TOTAL;
        pvmp3_ext.pOutputBuffer = mp3_pcm_tmp;

        ERROR_CODE ecode = pvmp3_framedecoder(&pvmp3_ext, pvmp3_dbuf);

        if (pvmp3_ext.inputBufferUsedLength > 0) {
            size_t consume = (size_t)pvmp3_ext.inputBufferUsedLength;
            if (consume > mp3_cache_len) {
                consume = mp3_cache_len;
            }
            mp3_cache_len -= consume;
            if (mp3_cache_len > 0) {
                memmove(mp3_cache, mp3_cache + consume, mp3_cache_len);
            }
        }

        if (ecode != NO_DECODING_ERROR || pvmp3_ext.outputFrameSize <= 0 || pvmp3_ext.num_channels <= 0 ||
            pvmp3_ext.samplingRate <= 0) {
            /* Not enough bytes yet or lost sync.
             * IMPORTANT: when source is fully consumed, the last MP3 frame may be truncated.
             * In that case pvmp3dec can keep returning an error forever unless we exit.
             */
            if (*cfg->src_done && Ring_Buffer_Get_Length(cfg->src_rb) == 0) {
                break;
            }
            if (mp3_cache_len == 0) {
                vTaskDelay(pdMS_TO_TICKS(1));
            }
            continue;
        }

        int samples_per_ch = (int)(pvmp3_ext.outputFrameSize / pvmp3_ext.num_channels);
        int hz = (int)pvmp3_ext.samplingRate;
        int src_ch = (int)pvmp3_ext.num_channels;

        if (hz != last_info_hz || src_ch != last_info_ch) {
            last_info_hz = hz;
            last_info_ch = src_ch;
            printf("mp3 info: samplerate=%d ch=%d\r\n", hz, src_ch);
        }
        const uint32_t tx_block_bytes =
            (uint32_t)samples_per_ch * (uint32_t)cfg->out_channels * (uint32_t)sizeof(int16_t);

        /* (Re)start TX+RX when needed (first start, sample rate changed, or block size changed).
         * Note: I2S DMA transfer size is configured in BYTES.
         */
        if (!dma_started || current_hz != hz || current_tx_block_bytes != tx_block_bytes) {
            /* Prime ping with current frame */
            cfg->pcm_fill(cfg->pcm_ping, mp3_pcm_tmp, samples_per_ch, src_ch, cfg->out_channels, tx_block_bytes);
            current_hz = hz;
            current_tx_block_bytes = tx_block_bytes;
            dma_started = true;
            ping = true;
            need_prime_pong = true;
            continue;
        }
        if (need_prime_pong) {
            cfg->pcm_fill(cfg->pcm_pong, mp3_pcm_tmp, samples_per_ch, src_ch, cfg->out_channels,
                          current_tx_block_bytes);
            cfg->reconfig_txrx(current_hz, current_tx_block_bytes, cfg->out_channels);
            need_prime_pong = false;
            continue;
        }

        /* DMA keeps running; wait one block done then update that buffer */
        xSemaphoreTake(cfg->sem_dma_done, portMAX_DELAY);

        int16_t *out = ping ? cfg->pcm_ping : cfg->pcm_pong;
        cfg->pcm_fill(out, mp3_pcm_tmp, samples_per_ch, src_ch, cfg->out_channels, current_tx_block_bytes);
        ping = !ping;
    }

    printf("mp3 playback done\r\n");
    cfg->audio_stop();
    if (cfg->done_notify_task != NULL) {
        (void)xTaskNotify(cfg->done_notify_task, AUDIO_NOTIFY_MP3_DONE, eSetBits);
    }

    if (pvmp3_dbuf != NULL) {
        vPortFree(pvmp3_dbuf);
        pvmp3_dbuf = NULL;
    }

    if (mp3_pcm_tmp != NULL) {
        vPortFree(mp3_pcm_tmp);
        mp3_pcm_tmp = NULL;
    }
    if (mp3_cache != NULL) {
        vPortFree(mp3_cache);
        mp3_cache = NULL;
    }

    vTaskDelete(NULL);
}
