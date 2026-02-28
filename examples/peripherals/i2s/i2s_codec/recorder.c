#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "board.h"
#include "bflb_mtimer.h"
#include "shell.h"
#include "wave_player.h"
#include "recorder.h"
#ifdef CONFIG_LIBFVAD
#include "fvad.h"
#endif

#if defined(CONFIG_FATFS) && CONFIG_FATFS && defined(CONFIG_FATFS_SDH_SDCARD) && CONFIG_FATFS_SDH_SDCARD && \
    defined(CONFIG_BSP_SDH_SD) && CONFIG_BSP_SDH_SD
#include "fatfs_diskio_register.h"
#include "ff.h"
#define RECORDER_ENABLE_SD_STORE 1
#else
#define RECORDER_ENABLE_SD_STORE 0
#endif

#define RECORDER_ENABLE_VAD      1

#define REC_VAD_MODE             2
#define REC_VAD_FRAME_MS         10U
#define REC_VAD_MAX_FRAME_SAMPLES 480U /* 48 kHz * 10 ms */

#define REC_SD_MOUNT_PATH         "/sd"
#define REC_SD_RECORD_DIR         "/sd/records"
#define REC_SD_SEGMENT_SECONDS    30U
#define REC_SD_STORE_DEFAULT_ENABLE 1U

typedef struct {
    SemaphoreHandle_t lock;
    uint32_t cb_count;
    uint32_t cb_lock_busy_drop_count;
    uint32_t cb_max_us;
    uint32_t cb_over_2ms_count;
    uint32_t cb_over_frame_count;
    uint32_t cb_peak_abs_max;
    uint32_t cb_clip_sample_count;
#if RECORDER_ENABLE_SD_STORE
    FATFS fs;
    bool sd_driver_ready;
    bool sd_init_done;
    bool sd_fs_ready;
    bool sd_store_enabled;
    FIL wav_file;
    bool wav_file_open;
    uint32_t wav_seq;
    uint32_t wav_sample_rate_hz;
    uint32_t wav_samples_written;
    uint32_t wav_data_bytes;
    uint32_t sd_write_calls;
    uint32_t sd_write_max_us;
    uint32_t sd_write_fail_count;
    uint32_t sd_write_bytes_total;
#endif
#ifdef CONFIG_LIBFVAD
    Fvad *vad;
    uint32_t vad_sample_rate_hz;
    uint32_t vad_frame_samples;
    uint32_t vad_frame_fill;
    int16_t vad_frame_buf[REC_VAD_MAX_FRAME_SAMPLES];
    bool speech_active;
    uint32_t vad_total_frames;
    uint32_t vad_speech_frames;
    uint32_t vad_silence_frames;
    uint32_t vad_err_frames;
    uint32_t vad_speech_start_events;
    uint32_t vad_speech_end_events;
#endif
} recorder_store_ctx_t;

static recorder_store_ctx_t g_rec_store = { 0 };

#ifdef CONFIG_LIBFVAD
static void rec_vad_diag_reset_locked(void)
{
    g_rec_store.vad_total_frames = 0U;
    g_rec_store.vad_speech_frames = 0U;
    g_rec_store.vad_silence_frames = 0U;
    g_rec_store.vad_err_frames = 0U;
    g_rec_store.vad_speech_start_events = 0U;
    g_rec_store.vad_speech_end_events = 0U;
}
#endif

static void rec_diag_reset_locked(void)
{
    g_rec_store.cb_count = 0U;
    g_rec_store.cb_lock_busy_drop_count = 0U;
    g_rec_store.cb_max_us = 0U;
    g_rec_store.cb_over_2ms_count = 0U;
    g_rec_store.cb_over_frame_count = 0U;
    g_rec_store.cb_peak_abs_max = 0U;
    g_rec_store.cb_clip_sample_count = 0U;
#if RECORDER_ENABLE_SD_STORE
    g_rec_store.sd_write_calls = 0U;
    g_rec_store.sd_write_max_us = 0U;
    g_rec_store.sd_write_fail_count = 0U;
    g_rec_store.sd_write_bytes_total = 0U;
#endif
#ifdef CONFIG_LIBFVAD
    rec_vad_diag_reset_locked();
#endif
}

#if RECORDER_ENABLE_SD_STORE
static uint32_t rec_u32_min(uint32_t a, uint32_t b)
{
    return (a < b) ? a : b;
}

static uint16_t rec_wav_get_le16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t rec_wav_get_le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

typedef struct {
    uint32_t sample_rate_hz;
    uint16_t channels;
    uint16_t bits_per_sample;
    uint32_t data_offset;
    uint32_t data_size;
} rec_wav_info_t;

static void rec_wav_put_le16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v & 0xffU);
    p[1] = (uint8_t)((v >> 8) & 0xffU);
}

static void rec_wav_put_le32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xffU);
    p[1] = (uint8_t)((v >> 8) & 0xffU);
    p[2] = (uint8_t)((v >> 16) & 0xffU);
    p[3] = (uint8_t)((v >> 24) & 0xffU);
}

static int rec_wav_write_header_locked(FIL *fp, uint32_t sample_rate_hz, uint16_t channels, uint16_t bits_per_sample,
                                       uint32_t data_bytes)
{
    if (fp == NULL || sample_rate_hz == 0U || channels == 0U || bits_per_sample == 0U) {
        return -1;
    }

    uint8_t hdr[44];
    UINT bw = 0;
    uint16_t block_align = (uint16_t)(channels * (bits_per_sample / 8U));
    uint32_t byte_rate = sample_rate_hz * (uint32_t)block_align;
    uint32_t riff_size = 36U + data_bytes;

    memcpy(&hdr[0], "RIFF", 4);
    rec_wav_put_le32(&hdr[4], riff_size);
    memcpy(&hdr[8], "WAVE", 4);
    memcpy(&hdr[12], "fmt ", 4);
    rec_wav_put_le32(&hdr[16], 16U);
    rec_wav_put_le16(&hdr[20], 1U);
    rec_wav_put_le16(&hdr[22], channels);
    rec_wav_put_le32(&hdr[24], sample_rate_hz);
    rec_wav_put_le32(&hdr[28], byte_rate);
    rec_wav_put_le16(&hdr[32], block_align);
    rec_wav_put_le16(&hdr[34], bits_per_sample);
    memcpy(&hdr[36], "data", 4);
    rec_wav_put_le32(&hdr[40], data_bytes);

    if (f_lseek(fp, 0) != FR_OK) {
        return -1;
    }
    if (f_write(fp, hdr, sizeof(hdr), &bw) != FR_OK || bw != sizeof(hdr)) {
        return -1;
    }
    return 0;
}

static void rec_sd_close_wav_locked(void)
{
    if (!g_rec_store.wav_file_open) {
        return;
    }

    if (rec_wav_write_header_locked(&g_rec_store.wav_file, g_rec_store.wav_sample_rate_hz, 1U, 16U,
                                    g_rec_store.wav_data_bytes) != 0) {
        printf("[REC] update wav header failed\r\n");
    }
    (void)f_sync(&g_rec_store.wav_file);
    (void)f_close(&g_rec_store.wav_file);

    g_rec_store.wav_file_open = false;
    g_rec_store.wav_sample_rate_hz = 0U;
    g_rec_store.wav_samples_written = 0U;
    g_rec_store.wav_data_bytes = 0U;
}

static bool rec_is_wav_name(const char *name, uint32_t *seq)
{
    if (name == NULL || seq == NULL) {
        return false;
    }
    if (strlen(name) < 9U) {
        return false;
    }
    if ((name[0] != 'r' && name[0] != 'R') || (name[1] != 'e' && name[1] != 'E') ||
        (name[2] != 'c' && name[2] != 'C') || name[3] != '_') {
        return false;
    }

    const char *dot = strrchr(name, '.');
    if (dot == NULL) {
        return false;
    }
    if (dot[1] == '\0' || dot[2] == '\0' || dot[3] == '\0' || dot[4] != '\0') {
        return false;
    }
    if ((dot[1] != 'w' && dot[1] != 'W') || (dot[2] != 'a' && dot[2] != 'A') || (dot[3] != 'v' && dot[3] != 'V')) {
        return false;
    }

    for (const char *p = &name[4]; p < dot; p++) {
        if (!isdigit((unsigned char)*p)) {
            return false;
        }
    }

    *seq = (uint32_t)strtoul(&name[4], NULL, 10);
    return true;
}

static void rec_sd_update_next_seq_locked(void)
{
    DIR dir;
    FILINFO fno;
    FRESULT ret;
    uint32_t max_seq = 0U;
    bool found = false;

    ret = f_opendir(&dir, REC_SD_RECORD_DIR);
    if (ret != FR_OK) {
        g_rec_store.wav_seq = 0U;
        return;
    }

    while (f_readdir(&dir, &fno) == FR_OK) {
        if (fno.fname[0] == '\0') {
            break;
        }
        if (fno.fattrib & AM_DIR) {
            continue;
        }
        uint32_t seq = 0U;
        if (rec_is_wav_name(fno.fname, &seq)) {
            if (!found || seq > max_seq) {
                max_seq = seq;
                found = true;
            }
        }
    }

    (void)f_closedir(&dir);
    g_rec_store.wav_seq = found ? (max_seq + 1U) : 0U;
}

static int rec_sd_try_init_locked(void)
{
    if (g_rec_store.sd_init_done) {
        return g_rec_store.sd_fs_ready ? 0 : -1;
    }
    g_rec_store.sd_init_done = true;

    if (!g_rec_store.sd_driver_ready) {
        board_sdh_gpio_init();
        fatfs_sdh_driver_register();
        g_rec_store.sd_driver_ready = true;
    }

    FRESULT ret = f_mount(&g_rec_store.fs, REC_SD_MOUNT_PATH, 1);
    if (ret != FR_OK) {
        printf("[REC] SD init failed: mount %s ret=%d, storage disabled\r\n", REC_SD_MOUNT_PATH, (int)ret);
        g_rec_store.sd_fs_ready = false;
        return -1;
    }

    ret = f_mkdir(REC_SD_RECORD_DIR);
    if (ret != FR_OK && ret != FR_EXIST) {
        printf("[REC] SD init failed: mkdir %s ret=%d, storage disabled\r\n", REC_SD_RECORD_DIR, (int)ret);
        g_rec_store.sd_fs_ready = false;
        return -1;
    }

    g_rec_store.sd_fs_ready = true;
    rec_sd_update_next_seq_locked();
    printf("[REC] SD ready, next file seq=%lu\r\n", (unsigned long)g_rec_store.wav_seq);
    return 0;
}

static int rec_sd_open_next_wav_locked(uint32_t sample_rate_hz)
{
    if (sample_rate_hz == 0U) {
        return -1;
    }
    if (!g_rec_store.sd_fs_ready) {
        return -1;
    }

    char path[64];
    FRESULT ret;
    UINT attempt;

    for (attempt = 0; attempt < 1000U; attempt++) {
        (void)snprintf(path, sizeof(path), REC_SD_RECORD_DIR "/rec_%06lu.wav", (unsigned long)g_rec_store.wav_seq);
        ret = f_open(&g_rec_store.wav_file, path, FA_CREATE_NEW | FA_WRITE);
        if (ret == FR_EXIST) {
            g_rec_store.wav_seq++;
            continue;
        }
        if (ret != FR_OK) {
            printf("[REC] open %s failed: %d\r\n", path, (int)ret);
            return -1;
        }

        if (rec_wav_write_header_locked(&g_rec_store.wav_file, sample_rate_hz, 1U, 16U, 0U) != 0) {
            printf("[REC] write wav header failed: %s\r\n", path);
            (void)f_close(&g_rec_store.wav_file);
            (void)f_unlink(path);
            return -1;
        }

        g_rec_store.wav_file_open = true;
        g_rec_store.wav_sample_rate_hz = sample_rate_hz;
        g_rec_store.wav_samples_written = 0U;
        g_rec_store.wav_data_bytes = 0U;
        g_rec_store.wav_seq++;
        return 0;
    }

    printf("[REC] no available rec_xxxxxx.wav slot\r\n");
    return -1;
}

static void rec_sd_store_samples_locked(const int16_t *samples, uint32_t sample_count, uint32_t sample_rate_hz)
{
    if (samples == NULL || sample_count == 0U || sample_rate_hz == 0U) {
        return;
    }
    if (!g_rec_store.sd_fs_ready) {
        return;
    }

    const uint32_t segment_samples = sample_rate_hz * REC_SD_SEGMENT_SECONDS;
    if (segment_samples == 0U) {
        return;
    }

    const int16_t *cur = samples;
    uint32_t remain = sample_count;

    while (remain > 0U) {
        if (!g_rec_store.wav_file_open || g_rec_store.wav_sample_rate_hz != sample_rate_hz ||
            g_rec_store.wav_samples_written >= segment_samples) {
            rec_sd_close_wav_locked();
            if (rec_sd_open_next_wav_locked(sample_rate_hz) != 0) {
                return;
            }
        }

        uint32_t seg_left = segment_samples - g_rec_store.wav_samples_written;
        uint32_t n = rec_u32_min(remain, seg_left);
        uint32_t bytes = n * (uint32_t)sizeof(int16_t);
        UINT bw = 0;
        uint64_t t0_us = bflb_mtimer_get_time_us();
        FRESULT ret = f_write(&g_rec_store.wav_file, cur, bytes, &bw);
        uint32_t write_us = (uint32_t)(bflb_mtimer_get_time_us() - t0_us);
        g_rec_store.sd_write_calls++;
        if (write_us > g_rec_store.sd_write_max_us) {
            g_rec_store.sd_write_max_us = write_us;
        }

        if (ret != FR_OK || bw != bytes) {
            printf("[REC] write failed ret=%d bw=%u exp=%lu\r\n", (int)ret, (unsigned int)bw, (unsigned long)bytes);
            g_rec_store.sd_write_fail_count++;
            rec_sd_close_wav_locked();
            g_rec_store.sd_fs_ready = false;
            printf("[REC] disable SD store after write error\r\n");
            return;
        }

        g_rec_store.wav_samples_written += n;
        g_rec_store.wav_data_bytes += bytes;
        g_rec_store.sd_write_bytes_total += bytes;
        cur += n;
        remain -= n;

        if (g_rec_store.wav_samples_written >= segment_samples) {
            rec_sd_close_wav_locked();
        }
    }
}

static int rec_sd_rm_all_locked(void)
{
    DIR dir;
    FILINFO fno;
    FRESULT ret;
    uint32_t removed = 0U;
    char path[96];

    if (!g_rec_store.sd_fs_ready) {
        printf("rec_rm_all: SD not ready\r\n");
        return -1;
    }

    rec_sd_close_wav_locked();

    ret = f_opendir(&dir, REC_SD_RECORD_DIR);
    if (ret != FR_OK) {
        return -1;
    }

    while (f_readdir(&dir, &fno) == FR_OK) {
        if (fno.fname[0] == '\0') {
            break;
        }
        if (fno.fattrib & AM_DIR) {
            continue;
        }
        (void)snprintf(path, sizeof(path), REC_SD_RECORD_DIR "/%s", fno.fname);
        if (f_unlink(path) == FR_OK) {
            removed++;
        }
    }

    (void)f_closedir(&dir);
    g_rec_store.wav_seq = 0U;
    printf("rec_rm_all: removed %lu file(s)\r\n", (unsigned long)removed);
    return 0;
}

static int rec_wav_parse_header_locked(FIL *fp, rec_wav_info_t *info)
{
    uint8_t hdr[12];
    UINT br = 0;
    bool got_fmt = false;
    bool got_data = false;
    FSIZE_t fsz;

    if (fp == NULL || info == NULL) {
        return -1;
    }

    memset(info, 0, sizeof(*info));
    if (f_lseek(fp, 0) != FR_OK) {
        return -1;
    }
    if (f_read(fp, hdr, sizeof(hdr), &br) != FR_OK || br != sizeof(hdr)) {
        return -1;
    }
    if (memcmp(&hdr[0], "RIFF", 4) != 0 || memcmp(&hdr[8], "WAVE", 4) != 0) {
        return -1;
    }

    fsz = f_size(fp);
    while ((got_fmt == false || got_data == false) && f_tell(fp) + 8U <= fsz) {
        uint8_t chdr[8];
        uint32_t csize;
        FSIZE_t cdata_pos;
        FSIZE_t cnext;

        if (f_read(fp, chdr, sizeof(chdr), &br) != FR_OK || br != sizeof(chdr)) {
            return -1;
        }
        csize = rec_wav_get_le32(&chdr[4]);
        cdata_pos = f_tell(fp);
        cnext = cdata_pos + (FSIZE_t)csize + ((csize & 1U) ? 1U : 0U);
        if (cnext < cdata_pos || cnext > fsz) {
            return -1;
        }

        if (memcmp(&chdr[0], "fmt ", 4) == 0) {
            uint8_t fmt[16];
            uint16_t audio_fmt;
            if (csize < 16U) {
                return -1;
            }
            if (f_read(fp, fmt, sizeof(fmt), &br) != FR_OK || br != sizeof(fmt)) {
                return -1;
            }
            audio_fmt = rec_wav_get_le16(&fmt[0]);
            info->channels = rec_wav_get_le16(&fmt[2]);
            info->sample_rate_hz = rec_wav_get_le32(&fmt[4]);
            info->bits_per_sample = rec_wav_get_le16(&fmt[14]);
            if (audio_fmt != 1U || (info->channels != 1U && info->channels != 2U) || info->sample_rate_hz == 0U ||
                info->bits_per_sample != 16U) {
                return -1;
            }
            got_fmt = true;
        } else if (memcmp(&chdr[0], "data", 4) == 0) {
            info->data_offset = (uint32_t)cdata_pos;
            info->data_size = csize;
            got_data = true;
        }

        if (f_lseek(fp, cnext) != FR_OK) {
            return -1;
        }
    }

    if (!got_fmt || !got_data) {
        return -1;
    }
    return 0;
}

static int rec_sd_list_files_locked(void)
{
    DIR dir;
    FILINFO fno;
    FRESULT ret;
    uint32_t cnt = 0U;

    if (!g_rec_store.sd_fs_ready) {
        printf("rec_ls: SD not ready\r\n");
        return -1;
    }

    ret = f_opendir(&dir, REC_SD_RECORD_DIR);
    if (ret != FR_OK) {
        printf("rec_ls: open dir failed: %d\r\n", (int)ret);
        return -1;
    }

    while (f_readdir(&dir, &fno) == FR_OK) {
        if (fno.fname[0] == '\0') {
            break;
        }
        if (fno.fattrib & AM_DIR) {
            continue;
        }
        printf("%s/%s (%lu bytes)\r\n", REC_SD_RECORD_DIR, fno.fname, (unsigned long)fno.fsize);
        cnt++;
    }

    (void)f_closedir(&dir);
    printf("rec_ls: %lu file(s)\r\n", (unsigned long)cnt);
    return 0;
}

static int rec_sd_play_file_locked(const char *path)
{
    FIL fp;
    FRESULT ret;
    rec_wav_info_t wav;
    uint32_t tx_block_bytes;
    uint32_t frame_bytes;
    uint32_t remain;
    uint8_t io_buf[512];

    if (path == NULL || path[0] == '\0') {
        return -1;
    }
    if (!g_rec_store.sd_fs_ready) {
        printf("rec_file_play: SD not ready\r\n");
        return -1;
    }

    ret = f_open(&fp, path, FA_READ);
    if (ret != FR_OK) {
        printf("rec_file_play: open %s failed: %d\r\n", path, (int)ret);
        return -1;
    }

    if (rec_wav_parse_header_locked(&fp, &wav) != 0) {
        printf("rec_file_play: unsupported wav: %s\r\n", path);
        (void)f_close(&fp);
        return -1;
    }

    frame_bytes = (uint32_t)wav.channels * sizeof(int16_t);
    if (frame_bytes == 0U) {
        (void)f_close(&fp);
        return -1;
    }

    tx_block_bytes = AUDIO_OUT_SAMPLES * AUDIO_OUT_CHANNELS * sizeof(int16_t);
    wave_player_stop();
    wave_player_config(wav.sample_rate_hz, tx_block_bytes, 0, (uint8_t)wav.channels);
    if (wave_player_start() != 0) {
        printf("rec_file_play: start audio failed\r\n");
        (void)f_close(&fp);
        return -1;
    }

    if (f_lseek(&fp, (FSIZE_t)wav.data_offset) != FR_OK) {
        printf("rec_file_play: seek data failed\r\n");
        wave_player_stop();
        (void)f_close(&fp);
        return -1;
    }

    remain = wav.data_size;
    while (remain > 0U) {
        UINT br = 0;
        uint32_t to_read = rec_u32_min((uint32_t)sizeof(io_buf), remain);
        uint32_t play_bytes;
        uint32_t wrote;

        if (f_read(&fp, io_buf, (UINT)to_read, &br) != FR_OK) {
            printf("rec_file_play: read failed\r\n");
            break;
        }
        if (br == 0U) {
            break;
        }

        play_bytes = ((uint32_t)br / frame_bytes) * frame_bytes;
        if (play_bytes > 0U) {
            wrote = wave_player_feed_blocking(io_buf, play_bytes);
            if (wrote < play_bytes) {
                printf("rec_file_play: feed short %lu/%lu\r\n", (unsigned long)wrote, (unsigned long)play_bytes);
                break;
            }
        }

        remain -= (uint32_t)br;
    }

    wave_player_stop();
    (void)f_close(&fp);
    return 0;
}
#endif

#ifdef CONFIG_LIBFVAD
static int rec_vad_set_sample_rate_locked(uint32_t sample_rate_hz)
{
    if (g_rec_store.vad == NULL) {
        g_rec_store.vad = fvad_new();
        if (g_rec_store.vad == NULL) {
            printf("[VAD] create failed\r\n");
            return -1;
        }
        if (fvad_set_mode(g_rec_store.vad, REC_VAD_MODE) != 0) {
            printf("[VAD] set mode failed\r\n");
            return -1;
        }
    }

    if (sample_rate_hz == g_rec_store.vad_sample_rate_hz) {
        return 0;
    }

    if (fvad_set_sample_rate(g_rec_store.vad, (int)sample_rate_hz) != 0) {
        printf("[VAD] unsupported sample rate: %lu\r\n", (unsigned long)sample_rate_hz);
        g_rec_store.vad_sample_rate_hz = 0U;
        g_rec_store.vad_frame_samples = 0U;
        g_rec_store.vad_frame_fill = 0U;
        g_rec_store.speech_active = false;
        return -1;
    }

    uint32_t frame_samples = (sample_rate_hz / 1000U) * REC_VAD_FRAME_MS;
    if (frame_samples == 0U || frame_samples > REC_VAD_MAX_FRAME_SAMPLES) {
        printf("[VAD] bad frame len: %lu\r\n", (unsigned long)frame_samples);
        g_rec_store.vad_sample_rate_hz = 0U;
        g_rec_store.vad_frame_samples = 0U;
        g_rec_store.vad_frame_fill = 0U;
        g_rec_store.speech_active = false;
        return -1;
    }

    g_rec_store.vad_sample_rate_hz = sample_rate_hz;
    g_rec_store.vad_frame_samples = frame_samples;
    g_rec_store.vad_frame_fill = 0U;
    g_rec_store.speech_active = false;
    return 0;
}

static void rec_vad_process_locked(const int16_t *samples, uint32_t sample_count, uint32_t sample_rate_hz)
{
    if (samples == NULL || sample_count == 0U || sample_rate_hz == 0U) {
        return;
    }

    if (rec_vad_set_sample_rate_locked(sample_rate_hz) != 0) {
        return;
    }

    while (sample_count > 0U) {
        uint32_t need = g_rec_store.vad_frame_samples - g_rec_store.vad_frame_fill;
        uint32_t n = (sample_count < need) ? sample_count : need;

        memcpy(&g_rec_store.vad_frame_buf[g_rec_store.vad_frame_fill], samples, n * sizeof(int16_t));
        g_rec_store.vad_frame_fill += n;
        samples += n;
        sample_count -= n;

        if (g_rec_store.vad_frame_fill == g_rec_store.vad_frame_samples) {
            int vad = fvad_process(g_rec_store.vad, g_rec_store.vad_frame_buf, g_rec_store.vad_frame_samples);
            g_rec_store.vad_total_frames++;
            if (vad > 0) {
                g_rec_store.vad_speech_frames++;
                if (!g_rec_store.speech_active) {
                    printf("[VAD] speech detected\r\n");
                    g_rec_store.vad_speech_start_events++;
                }
                g_rec_store.speech_active = true;
            } else if (vad == 0) {
                g_rec_store.vad_silence_frames++;
                if (g_rec_store.speech_active) {
                    g_rec_store.vad_speech_end_events++;
                }
                g_rec_store.speech_active = false;
            } else {
                g_rec_store.vad_err_frames++;
                if (g_rec_store.speech_active) {
                    g_rec_store.vad_speech_end_events++;
                }
                g_rec_store.speech_active = false;
            }
            g_rec_store.vad_frame_fill = 0U;
        }
    }
}
#endif

static void rec_store_on_frame(const int16_t *samples, uint32_t sample_count, uint32_t sample_rate_hz, void *user_data)
{
    (void)user_data;
    uint64_t t0_us = bflb_mtimer_get_time_us();

    if (samples == NULL || sample_count == 0U || sample_rate_hz == 0U || g_rec_store.lock == NULL) {
        return;
    }

    if (xSemaphoreTake(g_rec_store.lock, 0) != pdTRUE) {
        g_rec_store.cb_lock_busy_drop_count++;
        return;
    }

    {
        uint32_t peak_abs = 0U;
        uint32_t clip_count = 0U;
        for (uint32_t i = 0; i < sample_count; i++) {
            int32_t v = samples[i];
            uint32_t av = (uint32_t)((v < 0) ? -v : v);
            if (av > peak_abs) {
                peak_abs = av;
            }
            if (av >= 32760U) {
                clip_count++;
            }
        }
        if (peak_abs > g_rec_store.cb_peak_abs_max) {
            g_rec_store.cb_peak_abs_max = peak_abs;
        }
        g_rec_store.cb_clip_sample_count += clip_count;
    }

#ifdef CONFIG_LIBFVAD
    rec_vad_process_locked(samples, sample_count, sample_rate_hz);
#endif

#if RECORDER_ENABLE_SD_STORE
    if (g_rec_store.sd_store_enabled) {
        rec_sd_store_samples_locked(samples, sample_count, sample_rate_hz);
    }
#endif

    {
        uint32_t cb_us = (uint32_t)(bflb_mtimer_get_time_us() - t0_us);
        uint64_t frame_us = ((uint64_t)sample_count * 1000000ULL) / (uint64_t)sample_rate_hz;
        g_rec_store.cb_count++;
        if (cb_us > g_rec_store.cb_max_us) {
            g_rec_store.cb_max_us = cb_us;
        }
        if (cb_us > 2000U) {
            g_rec_store.cb_over_2ms_count++;
        }
        if ((uint64_t)cb_us > frame_us) {
            g_rec_store.cb_over_frame_count++;
        }
    }

    xSemaphoreGive(g_rec_store.lock);
}

static int cmd_rec_rm_all(int argc, char **argv)
{
    (void)argc;
    (void)argv;

#if RECORDER_ENABLE_SD_STORE
    if (g_rec_store.lock == NULL) {
        return -1;
    }
    if (xSemaphoreTake(g_rec_store.lock, pdMS_TO_TICKS(200)) != pdTRUE) {
        return -1;
    }
    int ret = rec_sd_rm_all_locked();
    xSemaphoreGive(g_rec_store.lock);
    return ret;
#else
    printf("rec_rm_all: SD storage disabled\r\n");
    return -1;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_rec_rm_all, rec_rm_all, "delete all recording wav files in /sd/records");

static int cmd_rec_ls(int argc, char **argv)
{
    (void)argc;
    (void)argv;

#if RECORDER_ENABLE_SD_STORE
    int ret;
    if (g_rec_store.lock == NULL) {
        return -1;
    }
    if (xSemaphoreTake(g_rec_store.lock, pdMS_TO_TICKS(200)) != pdTRUE) {
        return -1;
    }
    ret = rec_sd_list_files_locked();
    xSemaphoreGive(g_rec_store.lock);
    return ret;
#else
    printf("rec_ls: SD storage disabled\r\n");
    return -1;
#endif
}

static int cmd_rec_file_play(int argc, char **argv)
{
#if RECORDER_ENABLE_SD_STORE
    int ret;
    if (argc < 2) {
        printf("usage: rec_file_play <wav_path>\r\n");
        return -1;
    }
    if (g_rec_store.lock == NULL) {
        return -1;
    }
    if (xSemaphoreTake(g_rec_store.lock, pdMS_TO_TICKS(200)) != pdTRUE) {
        return -1;
    }
    ret = rec_sd_play_file_locked(argv[1]);
    xSemaphoreGive(g_rec_store.lock);
    return ret;
#else
    (void)argc;
    (void)argv;
    printf("rec_file_play: SD storage disabled\r\n");
    return -1;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_rec_ls, rec_ls, "list recording files in /sd/records");
SHELL_CMD_EXPORT_ALIAS(cmd_rec_file_play, rec_file_play, "play wav file: rec_file_play <wav_path>");

static int cmd_rec_sd_store(int argc, char **argv)
{
#if RECORDER_ENABLE_SD_STORE
    if (g_rec_store.lock == NULL) {
        return -1;
    }
    if (xSemaphoreTake(g_rec_store.lock, pdMS_TO_TICKS(200)) != pdTRUE) {
        return -1;
    }

    if (argc < 2) {
        printf("usage: rec_sd_store <0|1>\r\n");
        printf("rec_sd_store=%u\r\n", g_rec_store.sd_store_enabled ? 1U : 0U);
        xSemaphoreGive(g_rec_store.lock);
        return 0;
    }

    unsigned long en = strtoul(argv[1], NULL, 0);
    bool enable = (en != 0U);
    if (!enable) {
        rec_sd_close_wav_locked();
    }
    g_rec_store.sd_store_enabled = enable;
    printf("rec_sd_store=%u\r\n", g_rec_store.sd_store_enabled ? 1U : 0U);
    xSemaphoreGive(g_rec_store.lock);
    return 0;
#else
    (void)argc;
    (void)argv;
    printf("rec_sd_store: SD storage disabled\r\n");
    return -1;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_rec_sd_store, rec_sd_store, "enable/disable SD store: rec_sd_store <0|1>");

static int cmd_rec_vad_diag(int argc, char **argv)
{
#ifdef CONFIG_LIBFVAD
    if (g_rec_store.lock == NULL) {
        return -1;
    }
    if (xSemaphoreTake(g_rec_store.lock, pdMS_TO_TICKS(200)) != pdTRUE) {
        return -1;
    }

    if (argc >= 2 && strcmp(argv[1], "clr") == 0) {
        rec_vad_diag_reset_locked();
        printf("rec_vad_diag: cleared\r\n");
        xSemaphoreGive(g_rec_store.lock);
        return 0;
    }

    printf("rec_vad_diag: total=%lu speech=%lu silence=%lu err=%lu start=%lu end=%lu active=%u\r\n",
           (unsigned long)g_rec_store.vad_total_frames, (unsigned long)g_rec_store.vad_speech_frames,
           (unsigned long)g_rec_store.vad_silence_frames, (unsigned long)g_rec_store.vad_err_frames,
           (unsigned long)g_rec_store.vad_speech_start_events, (unsigned long)g_rec_store.vad_speech_end_events,
           g_rec_store.speech_active ? 1U : 0U);
    printf("rec_vad_diag: sample_rate=%lu frame_samples=%lu frame_fill=%lu\r\n",
           (unsigned long)g_rec_store.vad_sample_rate_hz, (unsigned long)g_rec_store.vad_frame_samples,
           (unsigned long)g_rec_store.vad_frame_fill);

    xSemaphoreGive(g_rec_store.lock);
    return 0;
#else
    (void)argc;
    (void)argv;
    printf("rec_vad_diag: VAD disabled (CONFIG_LIBFVAD=0)\r\n");
    return -1;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_rec_vad_diag, rec_vad_diag, "show/reset VAD diagnostics: rec_vad_diag [clr]");

static int cmd_rec_diag(int argc, char **argv)
{
    if (g_rec_store.lock == NULL) {
        return -1;
    }
    if (xSemaphoreTake(g_rec_store.lock, pdMS_TO_TICKS(200)) != pdTRUE) {
        return -1;
    }

    if (argc >= 2 && strcmp(argv[1], "clr") == 0) {
        rec_diag_reset_locked();
        printf("rec_diag: cleared\r\n");
        xSemaphoreGive(g_rec_store.lock);
        return 0;
    }

    printf("rec_diag: cb_count=%lu, lock_drop=%lu, cb_max_us=%lu, cb_over_2ms=%lu, cb_over_frame=%lu\r\n",
           (unsigned long)g_rec_store.cb_count, (unsigned long)g_rec_store.cb_lock_busy_drop_count,
           (unsigned long)g_rec_store.cb_max_us, (unsigned long)g_rec_store.cb_over_2ms_count,
           (unsigned long)g_rec_store.cb_over_frame_count);
    printf("rec_diag: peak_abs_max=%lu, clip_samples=%lu\r\n", (unsigned long)g_rec_store.cb_peak_abs_max,
           (unsigned long)g_rec_store.cb_clip_sample_count);
#if RECORDER_ENABLE_SD_STORE
    printf("rec_diag: sd_write_calls=%lu, sd_write_max_us=%lu, sd_write_fail=%lu, sd_write_bytes=%lu\r\n",
           (unsigned long)g_rec_store.sd_write_calls, (unsigned long)g_rec_store.sd_write_max_us,
           (unsigned long)g_rec_store.sd_write_fail_count, (unsigned long)g_rec_store.sd_write_bytes_total);
#else
    printf("rec_diag: SD store disabled\r\n");
#endif

    xSemaphoreGive(g_rec_store.lock);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_rec_diag, rec_diag, "show/reset recorder diagnostics: rec_diag [clr]");

int recorder_storage_init(void)
{
    g_rec_store.lock = xSemaphoreCreateMutex();
    if (g_rec_store.lock == NULL) {
        printf("[ERROR] create recorder lock failed\r\n");
        return -1;
    }
    rec_diag_reset_locked();

#if RECORDER_ENABLE_SD_STORE
    g_rec_store.sd_driver_ready = false;
    g_rec_store.sd_init_done = false;
    g_rec_store.sd_fs_ready = false;
    g_rec_store.sd_store_enabled = (REC_SD_STORE_DEFAULT_ENABLE != 0U);
    g_rec_store.wav_file_open = false;
    g_rec_store.wav_seq = 0U;
    g_rec_store.wav_sample_rate_hz = 0U;
    g_rec_store.wav_samples_written = 0U;
    g_rec_store.wav_data_bytes = 0U;

    if (rec_sd_try_init_locked() != 0) {
        printf("[WARN] SD init failed, continue with VAD only\r\n");
    }
    printf("[REC] SD store default=%u (set via rec_sd_store <0|1>)\r\n", g_rec_store.sd_store_enabled ? 1U : 0U);
#endif

#ifdef CONFIG_LIBFVAD
    g_rec_store.vad = NULL;
    g_rec_store.vad_sample_rate_hz = 0U;
    g_rec_store.vad_frame_samples = 0U;
    g_rec_store.vad_frame_fill = 0U;
    g_rec_store.speech_active = false;
    rec_vad_diag_reset_locked();
#endif

    if (wave_player_rec_set_callback(rec_store_on_frame, NULL) != 0) {
        printf("[ERROR] set record callback failed\r\n");
        return -1;
    }

    (void)wave_record_enable(true);

    return 0;
}
