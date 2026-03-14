#include "mimiclaw_audio_player.h"

#include "esp_log.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <ff.h>
#include <fatfs_diskio_register.h>

#include "wave_player.h"
#include "mp3_player.h"

static const char *TAG = "mimi_audio";

#define AUDIO_PATH_MAX 192
#define WAV_IO_BUF_SIZE 512

#if defined(CONFIG_BSP_AUDIO_PLAYER) && defined(CONFIG_BSP_AUDIO_CODEC)
#define MIMICLAW_AUDIO_HW_AVAILABLE 1
#else
#define MIMICLAW_AUDIO_HW_AVAILABLE 0
#endif

#if defined(CONFIG_PVMP3DEC)
#define MIMICLAW_AUDIO_MP3_AVAILABLE 1
#else
#define MIMICLAW_AUDIO_MP3_AVAILABLE 0
#endif

typedef enum {
    AUDIO_FMT_NONE = 0,
    AUDIO_FMT_WAV,
    AUDIO_FMT_MP3,
} audio_format_t;

typedef struct {
    uint32_t data_offset;
    uint32_t data_size;
    uint32_t sample_rate_hz;
    uint16_t channels;
    uint16_t bits_per_sample;
} wav_info_t;

typedef struct {
    SemaphoreHandle_t lock;
    bool initialized;
    bool wave_hw_ready;

    bool fatfs_driver_ready;
    bool fatfs_mounted;
    FATFS fatfs;

    TaskHandle_t wav_task;
    bool stop_req;

    audio_format_t current_format;
    char current_path[AUDIO_PATH_MAX];
    esp_err_t last_err;

    FIL mp3_file;
    bool mp3_file_open;
    audio_mp3_task_cfg_t mp3_cfg;
} mimiclaw_audio_ctx_t;

static mimiclaw_audio_ctx_t s_audio;

/* Optional board hooks. */
__attribute__((weak)) void board_sdh_gpio_init(void);
__attribute__((weak)) void fatfs_sdh_driver_register(void);

static bool is_sd_path(const char *path)
{
    return path &&
           strncmp(path, "/sd", 3) == 0 &&
           (path[3] == '\0' || path[3] == '/');
}

static bool ends_with_ci(const char *s, const char *suffix)
{
    size_t ls;
    size_t le;
    size_t i;

    if (!s || !suffix) {
        return false;
    }

    ls = strlen(s);
    le = strlen(suffix);
    if (le == 0 || ls < le) {
        return false;
    }

    s += (ls - le);
    for (i = 0; i < le; i++) {
        if (tolower((unsigned char)s[i]) != tolower((unsigned char)suffix[i])) {
            return false;
        }
    }
    return true;
}

static const char *fmt_to_str(audio_format_t fmt)
{
    switch (fmt) {
        case AUDIO_FMT_WAV:
            return "wav";
        case AUDIO_FMT_MP3:
            return "mp3";
        default:
            return "none";
    }
}

static uint16_t wav_le16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t wav_le32(const uint8_t *p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static esp_err_t ensure_lock(void)
{
    if (s_audio.lock) {
        return ESP_OK;
    }

    s_audio.lock = xSemaphoreCreateMutex();
    if (!s_audio.lock) {
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

static esp_err_t ensure_fatfs_ready(void)
{
#if defined(CONFIG_FATFS) && defined(CONFIG_FATFS_SDH_SDCARD)
    FRESULT ret;

    if (s_audio.fatfs_mounted) {
        return ESP_OK;
    }

    if (!s_audio.fatfs_driver_ready) {
        if (board_sdh_gpio_init) {
            board_sdh_gpio_init();
        }

        if (fatfs_sdh_driver_register) {
            fatfs_sdh_driver_register();
        } else {
            ESP_LOGE(TAG, "fatfs_sdh_driver_register not available");
            return ESP_FAIL;
        }

        s_audio.fatfs_driver_ready = true;
    }

    ret = f_mount(&s_audio.fatfs, "/sd", 1);
    if (ret != FR_OK) {
        ESP_LOGE(TAG, "f_mount(/sd) failed: %d", (int)ret);
        return ESP_FAIL;
    }

    s_audio.fatfs_mounted = true;
    return ESP_OK;
#else
    ESP_LOGE(TAG, "FatFS SD backend not enabled");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static esp_err_t ensure_wave_hw_ready(void)
{
#if MIMICLAW_AUDIO_HW_AVAILABLE
    wave_player_hw_cfg_t hw_cfg;

    if (s_audio.wave_hw_ready) {
        return ESP_OK;
    }

    memset(&hw_cfg, 0, sizeof(hw_cfg));
    hw_cfg.i2s_dev_name = "i2s0";
    hw_cfg.i2c_dev_name = "i2c0";
    hw_cfg.dma_tx_dev_name = "dma0_ch0";
    hw_cfg.dma_rx_dev_name = "dma0_ch1";

    if (wave_player_init(&hw_cfg) != 0) {
        ESP_LOGE(TAG, "wave_player_init failed");
        return ESP_FAIL;
    }

    s_audio.wave_hw_ready = true;
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static int parse_wav_header(FIL *fp, wav_info_t *info)
{
    uint8_t hdr[12];
    UINT br = 0;
    bool got_fmt = false;
    bool got_data = false;
    uint16_t audio_fmt = 0;

    if (!fp || !info) {
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

    while (1) {
        uint8_t chdr[8];
        uint32_t csize;
        FSIZE_t next_pos;

        if (f_read(fp, chdr, sizeof(chdr), &br) != FR_OK || br != sizeof(chdr)) {
            return -1;
        }

        csize = wav_le32(&chdr[4]);
        next_pos = f_tell(fp) + (FSIZE_t)csize + (FSIZE_t)(csize & 1U);

        if (memcmp(&chdr[0], "fmt ", 4) == 0) {
            uint8_t fmt[16];
            if (csize < sizeof(fmt)) {
                return -1;
            }
            if (f_read(fp, fmt, sizeof(fmt), &br) != FR_OK || br != sizeof(fmt)) {
                return -1;
            }

            audio_fmt = wav_le16(&fmt[0]);
            info->channels = wav_le16(&fmt[2]);
            info->sample_rate_hz = wav_le32(&fmt[4]);
            info->bits_per_sample = wav_le16(&fmt[14]);
            got_fmt = true;
        } else if (memcmp(&chdr[0], "data", 4) == 0) {
            info->data_offset = (uint32_t)f_tell(fp);
            info->data_size = csize;
            got_data = true;
        }

        if (f_lseek(fp, next_pos) != FR_OK) {
            return -1;
        }

        if (got_fmt && got_data) {
            break;
        }
    }

    if (!got_fmt || !got_data) {
        return -1;
    }

    if (audio_fmt != 1U) {
        return -1;
    }
    if (info->bits_per_sample != 16U) {
        return -1;
    }
    if (info->channels < 1U || info->channels > 2U) {
        return -1;
    }
    if (info->sample_rate_hz == 0U) {
        return -1;
    }

    return 0;
}

static void wav_play_task(void *arg)
{
    char *path = (char *)arg;
    FIL fp;
    wav_info_t wav;
    uint8_t io_buf[WAV_IO_BUF_SIZE];
    esp_err_t ret = ESP_OK;
    FRESULT fr;
    UINT br = 0;
    uint32_t remain;
    uint32_t frame_bytes;

    if (!path) {
        ret = ESP_ERR_INVALID_ARG;
        goto out;
    }

    fr = f_open(&fp, path, FA_READ);
    if (fr != FR_OK) {
        ESP_LOGE(TAG, "open wav failed: %s ret=%d", path, (int)fr);
        ret = ESP_FAIL;
        goto out;
    }

    if (parse_wav_header(&fp, &wav) != 0) {
        ESP_LOGE(TAG, "unsupported wav: %s", path);
        ret = ESP_ERR_NOT_SUPPORTED;
        (void)f_close(&fp);
        goto out;
    }

    frame_bytes = (uint32_t)wav.channels * sizeof(int16_t);
    if (frame_bytes == 0U) {
        ret = ESP_FAIL;
        (void)f_close(&fp);
        goto out;
    }

    wave_player_stop();
    wave_player_config(wav.sample_rate_hz,
                       AUDIO_OUT_SAMPLES * AUDIO_OUT_CHANNELS * sizeof(int16_t),
                       0,
                       (uint8_t)wav.channels);
    if (wave_player_start() != 0) {
        ret = ESP_FAIL;
        (void)f_close(&fp);
        goto out;
    }

    if (f_lseek(&fp, (FSIZE_t)wav.data_offset) != FR_OK) {
        wave_player_stop();
        ret = ESP_FAIL;
        (void)f_close(&fp);
        goto out;
    }

    remain = wav.data_size;
    while (remain > 0U) {
        bool stop_now = false;
        uint32_t to_read;
        uint32_t play_bytes;
        uint32_t wrote;

        if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(50)) == pdTRUE) {
            stop_now = s_audio.stop_req;
            xSemaphoreGive(s_audio.lock);
        }
        if (stop_now) {
            break;
        }

        to_read = (remain < (uint32_t)sizeof(io_buf)) ? remain : (uint32_t)sizeof(io_buf);
        if (f_read(&fp, io_buf, (UINT)to_read, &br) != FR_OK || br == 0U) {
            break;
        }

        play_bytes = ((uint32_t)br / frame_bytes) * frame_bytes;
        if (play_bytes > 0U) {
            wrote = wave_player_feed_blocking(io_buf, play_bytes);
            if (wrote < play_bytes) {
                ret = ESP_FAIL;
                break;
            }
        }

        remain -= (uint32_t)br;
    }

    wave_player_stop();
    (void)f_close(&fp);

out:
    if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_audio.last_err = ret;
        s_audio.wav_task = NULL;
        s_audio.stop_req = false;
        if (s_audio.current_format == AUDIO_FMT_WAV) {
            s_audio.current_format = AUDIO_FMT_NONE;
            s_audio.current_path[0] = '\0';
        }
        xSemaphoreGive(s_audio.lock);
    }

    free(path);
    vTaskDelete(NULL);
}

#if MIMICLAW_AUDIO_MP3_AVAILABLE
static int mp3_file_init(void *user_data)
{
    FRESULT fr;
    char path[AUDIO_PATH_MAX];

    (void)user_data;

    if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) != pdTRUE) {
        return -1;
    }
    snprintf(path, sizeof(path), "%s", s_audio.current_path);
    xSemaphoreGive(s_audio.lock);

    fr = f_open(&s_audio.mp3_file, path, FA_READ);
    if (fr != FR_OK) {
        ESP_LOGE(TAG, "open mp3 failed: %s ret=%d", path, (int)fr);
        if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
            s_audio.mp3_file_open = false;
            s_audio.last_err = ESP_FAIL;
            xSemaphoreGive(s_audio.lock);
        }
        return -1;
    }

    if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_audio.mp3_file_open = true;
        xSemaphoreGive(s_audio.lock);
    }
    return 0;
}

static int mp3_file_read(void *user_data, uint8_t *buf, uint32_t len)
{
    UINT br = 0;
    (void)user_data;

    if (!buf || len == 0) {
        return 0;
    }

    if (!s_audio.mp3_file_open) {
        return 0;
    }

    if (f_read(&s_audio.mp3_file, buf, (UINT)len, &br) != FR_OK) {
        return 0;
    }

    return (int)br;
}

static void mp3_file_close(void *user_data)
{
    (void)user_data;

    if (s_audio.mp3_file_open) {
        (void)f_close(&s_audio.mp3_file);
    }

    if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_audio.mp3_file_open = false;
        s_audio.stop_req = false;
        if (s_audio.current_format == AUDIO_FMT_MP3) {
            s_audio.current_format = AUDIO_FMT_NONE;
            s_audio.current_path[0] = '\0';
        }
        xSemaphoreGive(s_audio.lock);
    }
}
#endif

esp_err_t mimiclaw_audio_init(void)
{
    esp_err_t ret;

    ret = ensure_lock();
    if (ret != ESP_OK) {
        return ret;
    }

    if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    if (s_audio.initialized) {
        xSemaphoreGive(s_audio.lock);
        return ESP_OK;
    }

    s_audio.current_format = AUDIO_FMT_NONE;
    s_audio.current_path[0] = '\0';
    s_audio.last_err = ESP_OK;
    s_audio.stop_req = false;
    s_audio.wav_task = NULL;
    s_audio.mp3_file_open = false;

    xSemaphoreGive(s_audio.lock);

    ret = ensure_fatfs_ready();
    if (ret != ESP_OK) {
        return ret;
    }

    ret = ensure_wave_hw_ready();
    if (ret != ESP_OK) {
        return ret;
    }

    if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_audio.initialized = true;
        xSemaphoreGive(s_audio.lock);
    }

    ESP_LOGI(TAG, "audio initialized");
    return ESP_OK;
}

bool mimiclaw_audio_is_playing(void)
{
    bool running = false;

    if (s_audio.wav_task != NULL) {
        running = true;
    }
#if MIMICLAW_AUDIO_MP3_AVAILABLE
    if (mp3_player_is_running()) {
        running = true;
    }
#endif
    return running;
}

esp_err_t mimiclaw_audio_stop(char *output, size_t output_size)
{
    TaskHandle_t wav_task = NULL;

    if (s_audio.lock && xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_audio.stop_req = true;
        wav_task = s_audio.wav_task;
        xSemaphoreGive(s_audio.lock);
    }

#if MIMICLAW_AUDIO_HW_AVAILABLE
    wave_player_stop();
#endif

#if MIMICLAW_AUDIO_MP3_AVAILABLE
    (void)mp3_player_stop();
#endif

    while (wav_task != NULL) {
        vTaskDelay(pdMS_TO_TICKS(20));
        if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
            wav_task = s_audio.wav_task;
            xSemaphoreGive(s_audio.lock);
        } else {
            break;
        }
    }

    if (s_audio.lock && xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
        s_audio.stop_req = false;
        s_audio.current_format = AUDIO_FMT_NONE;
        s_audio.current_path[0] = '\0';
        s_audio.last_err = ESP_OK;
        xSemaphoreGive(s_audio.lock);
    }

    if (output && output_size > 0) {
        snprintf(output, output_size, "audio stopped");
    }

    return ESP_OK;
}

esp_err_t mimiclaw_audio_play_file(const char *path, char *output, size_t output_size)
{
    audio_format_t fmt;
    char *wav_path = NULL;
    BaseType_t task_ok;

    if (!path) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!is_sd_path(path)) {
        if (output && output_size > 0) {
            snprintf(output, output_size, "only /sd/* is supported");
        }
        return ESP_ERR_INVALID_ARG;
    }

    if (ends_with_ci(path, ".wav")) {
        fmt = AUDIO_FMT_WAV;
    } else if (ends_with_ci(path, ".mp3")) {
        fmt = AUDIO_FMT_MP3;
    } else {
        if (output && output_size > 0) {
            snprintf(output, output_size, "unsupported extension, use .wav or .mp3");
        }
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (mimiclaw_audio_init() != ESP_OK) {
        if (output && output_size > 0) {
            snprintf(output, output_size, "audio init failed");
        }
        return ESP_FAIL;
    }

    (void)mimiclaw_audio_stop(NULL, 0);

    if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    snprintf(s_audio.current_path, sizeof(s_audio.current_path), "%s", path);
    s_audio.current_format = fmt;
    s_audio.last_err = ESP_OK;
    s_audio.stop_req = false;
    xSemaphoreGive(s_audio.lock);

    if (fmt == AUDIO_FMT_WAV) {
        wav_path = strdup(path);
        if (!wav_path) {
            return ESP_ERR_NO_MEM;
        }

        task_ok = xTaskCreate(wav_play_task, "mimi_wav", 3072, wav_path, 11, &s_audio.wav_task);
        if (task_ok != pdPASS) {
            free(wav_path);
            if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
                s_audio.current_format = AUDIO_FMT_NONE;
                s_audio.current_path[0] = '\0';
                s_audio.last_err = ESP_FAIL;
                xSemaphoreGive(s_audio.lock);
            }
            if (output && output_size > 0) {
                snprintf(output, output_size, "create wav task failed");
            }
            return ESP_FAIL;
        }
    } else {
#if MIMICLAW_AUDIO_MP3_AVAILABLE
        s_audio.mp3_cfg.ops.init = mp3_file_init;
        s_audio.mp3_cfg.ops.read = mp3_file_read;
        s_audio.mp3_cfg.ops.close = mp3_file_close;
        s_audio.mp3_cfg.user_data = NULL;

        if (mp3_player_start(&s_audio.mp3_cfg) != 0) {
            if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
                s_audio.current_format = AUDIO_FMT_NONE;
                s_audio.current_path[0] = '\0';
                s_audio.last_err = ESP_FAIL;
                xSemaphoreGive(s_audio.lock);
            }
            if (output && output_size > 0) {
                snprintf(output, output_size, "start mp3 player failed");
            }
            return ESP_FAIL;
        }
#else
        if (xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
            s_audio.current_format = AUDIO_FMT_NONE;
            s_audio.current_path[0] = '\0';
            s_audio.last_err = ESP_ERR_NOT_SUPPORTED;
            xSemaphoreGive(s_audio.lock);
        }
        if (output && output_size > 0) {
            snprintf(output, output_size, "mp3 not enabled");
        }
        return ESP_ERR_NOT_SUPPORTED;
#endif
    }

    if (output && output_size > 0) {
        snprintf(output, output_size, "audio start ok: %s", path);
    }
    return ESP_OK;
}

esp_err_t mimiclaw_audio_status(char *output, size_t output_size)
{
    audio_format_t fmt = AUDIO_FMT_NONE;
    char path[AUDIO_PATH_MAX] = { 0 };
    esp_err_t last_err = ESP_OK;
    bool running;

    if (!output || output_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_audio.lock && xSemaphoreTake(s_audio.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
        fmt = s_audio.current_format;
        snprintf(path, sizeof(path), "%s", s_audio.current_path);
        last_err = s_audio.last_err;
        xSemaphoreGive(s_audio.lock);
    }

    running = mimiclaw_audio_is_playing();

    snprintf(output, output_size,
             "state=%s format=%s path=%s last_err=%s(%d)",
             running ? "playing" : "idle",
             fmt_to_str(fmt),
             path[0] ? path : "(none)",
             esp_err_to_name(last_err),
             last_err);
    return ESP_OK;
}
