/* SPDX-License-Identifier: Apache-2.0 */
/* Exercise the production session owner with controlled RTOS/DMA failures. */
#define _POSIX_C_SOURCE 200809L
#include "audio_pcm.h"
#include "wave_player_pcm.h"
#include <math.h>
#include <FreeRTOS.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define portMAX_DELAY                      UINT32_MAX
#define WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH 640U
#define WAVE_PLAYER_TX_DONE_NOTIFY_BIT     1U
#define DMA_ADDR_I2S_TDR                   0x100U
#define DMA_ADDR_AUDAC_TDR                 0x200U
#define eSetBits                           0
#define taskENTER_CRITICAL()               test_enter_critical()
#define taskEXIT_CRITICAL()                test_exit_critical()
static void test_enter_critical(void);
static void test_exit_critical(void);
typedef pthread_mutex_t *SemaphoreHandle_t;
static pthread_mutex_t critical = PTHREAD_MUTEX_INITIALIZER;
static _Thread_local unsigned critical_depth;
static unsigned config_calls;
static unsigned init_calls;
static unsigned transport_stops;
static unsigned tx_starts;
static bool fail_init;
static bool fail_config;
static bool fail_start;
static bool fail_reload;
static bool tx_registers_reloaded;
static bool tx_peripheral_paused;
static bool tx_dma_restarted;
static unsigned tx_pauses;
static unsigned tx_resumes;
static bool hold_worker;
static int volume_error;
static int mute_error;
static unsigned volume_calls;
static unsigned mute_calls;
static uint8_t current_playback_volume;
static uint8_t current_record_volume;
static float current_record_gain_db;
static uint8_t record_volume_slots[AUDIO_CODEC_MAX_SLOTS];
static float record_gain_slots[AUDIO_CODEC_MAX_SLOTS];
static unsigned record_control_calls;
static int record_control_error;
static _Thread_local TaskHandle_t current_task;
static _Thread_local bool schedule_capture_reference;
static _Thread_local bool schedule_handoff_wait;
static bool handoff_waiting;
static pthread_mutex_t handoff_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t handoff_condition = PTHREAD_COND_INITIALIZER;
static void (*capture_reservation_hook)(void);
static uint8_t hardware_tokens[4];
struct bflb_dma_channel_lli_transfer_s {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t nbytes;
};
static struct {
    unsigned size;
    unsigned used;
} fake_ring;
static struct {
    SemaphoreHandle_t sem_tx_dma_done;
    SemaphoreHandle_t sem_wave_rb_space;
    void *dma_ch_tx;
    void *dma_ch_rx;
    uint32_t tx_llipool[8];
    int16_t pcm_ping[WAVE_PLAYER_OUT_SAMPLES * 2];
    int16_t pcm_pong[WAVE_PLAYER_OUT_SAMPLES * 2];
    wave_player_backend_t playback_backend;
    uint32_t tx_block_bytes;
    uint32_t tx_lli_count;
    uint32_t out_hz;
    uint32_t cap_hz;
    uint32_t logical_tx_block_bytes;
    uint32_t cap_isr_seq;
    uint32_t cap_epoch;
    uint32_t tx_dma_completions;
    uint32_t tx_period_samples;
    uint64_t tx_total_samples;
    uint8_t src_channels;
    uint8_t tx_slot_count, tx_slot_word_bytes;
    bool io_started;
    bool rec_enable;
    bool rec_stop;
    bool stop;
    bool paused;
    TaskHandle_t wave_task;
    TaskHandle_t cap_task;
    wave_player_capture_layout_t capture_layout;
    wave_player_rec_cb_t rec_cb;
    void *rec_cb_user_data;
    audio_codec_dev_t play_codec;
    audio_codec_dev_t cap_codec;
    int wave_rb;
} g_audio;
#include "mock_codec_channel_caps.h"
static wave_player_playback_frame_cb_t playback_frame_cb;
static void *playback_frame_user_data;
static int audio_pcm_lock(void);
static void audio_pcm_unlock(void);

uint64_t bflb_mtimer_get_time_us(void)
{
    struct timespec now;
    assert(clock_gettime(CLOCK_MONOTONIC, &now) == 0);
    return (uint64_t)now.tv_sec * 1000000U + (uint64_t)now.tv_nsec / 1000U;
}

static SemaphoreHandle_t xSemaphoreCreateMutex(void)
{
    SemaphoreHandle_t mutex = malloc(sizeof(*mutex));
    assert(mutex != NULL);
    assert(pthread_mutex_init(mutex, NULL) == 0);
    return mutex;
}

static void vSemaphoreDelete(SemaphoreHandle_t mutex)
{
    assert(pthread_mutex_destroy(mutex) == 0);
    free(mutex);
}

static int xSemaphoreTake(SemaphoreHandle_t mutex, uint32_t ticks)
{
    if (ticks == portMAX_DELAY) {
        return pthread_mutex_lock(mutex) == 0 ? pdTRUE : 0;
    }
    uint64_t started = bflb_mtimer_get_time_us();
    do {
        if (pthread_mutex_trylock(mutex) == 0) {
            return pdTRUE;
        }
    } while (bflb_mtimer_get_time_us() - started < (uint64_t)ticks * 1000U);
    return 0;
}

static int xSemaphoreGive(SemaphoreHandle_t mutex)
{
    int result = pthread_mutex_unlock(mutex) == 0 ? pdTRUE : 0;
    return result;
}

static TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
    return current_task;
}

static int xTaskNotify(TaskHandle_t task, uint32_t bits, int action)
{
    assert(task == g_audio.wave_task && bits == WAVE_PLAYER_TX_DONE_NOTIFY_BIT && action == eSetBits);
    return pdTRUE;
}

static void audio_io_stop(void)
{
    ++transport_stops;
    g_audio.io_started = false;
    ++g_audio.cap_epoch;
    tx_peripheral_paused = true;
    tx_dma_restarted = false;
}

static void audio_io_pause(void)
{
    assert(g_audio.io_started);
    tx_peripheral_paused = true;
    tx_dma_restarted = false;
    ++tx_pauses;
}

static void audio_io_resume(void)
{
    assert(g_audio.io_started);
    assert(tx_peripheral_paused && tx_dma_restarted && tx_registers_reloaded);
    tx_peripheral_paused = false;
    ++tx_resumes;
}

static void vTaskDelay(uint32_t ticks)
{
    if (schedule_handoff_wait) {
        assert(pthread_mutex_lock(&handoff_lock) == 0);
        handoff_waiting = true;
        assert(pthread_cond_broadcast(&handoff_condition) == 0);
        assert(pthread_mutex_unlock(&handoff_lock) == 0);
    }
    if (!hold_worker && g_audio.wave_task != NULL && g_audio.stop) {
        audio_pcm_player_cleanup();
        g_audio.wave_task = NULL;
        g_audio.stop = false;
    }
    struct timespec delay = { .tv_nsec = (long)ticks * 1000000L };
    assert(nanosleep(&delay, NULL) == 0);
}

static uint8_t wave_player_default_playback_channels(void)
{
    return 2;
}

static uint8_t wave_player_default_capture_channels(void)
{
    return 4;
}

int wave_player_init(const wave_player_hw_cfg_t *config)
{
    ++init_calls;
    if (fail_init) {
        return -EIO;
    }
    g_audio.sem_tx_dma_done = (void *)&hardware_tokens[0];
    g_audio.dma_ch_tx = &hardware_tokens[1];
    g_audio.dma_ch_rx = &hardware_tokens[2];
    g_audio.cap_task = &hardware_tokens[3];
    g_audio.capture_layout = config->capture_layout;
    g_audio.play_codec.drv = g_audio.cap_codec.drv = &mock_codec_driver;
    g_audio.tx_slot_count = 2;
    g_audio.tx_slot_word_bytes = sizeof(int16_t);
    current_record_volume = 100;
    current_record_gain_db = 6.0f;
    memset(record_volume_slots, 100, sizeof(record_volume_slots));
    for (unsigned slot = 0; slot < AUDIO_CODEC_MAX_SLOTS; ++slot) {
        record_gain_slots[slot] = 6.0f;
    }

    const audio_controls_config_t controls = {
        .playback = &g_audio.play_codec, .capture = &g_audio.cap_codec,
        .speaker_mask = 3, .mic_mask = (uint16_t)config->capture_layout.mic_mask,
        .tx_slots = 2, .tx_slot_bytes = sizeof(int16_t),
    };
    return audio_controls_init(&controls);
}

static int wave_player_config_internal(uint32_t rate, uint32_t tx_bytes, uint32_t rx_bytes, uint8_t channels)
{
    (void)rx_bytes;
    ++config_calls;
    assert(audio_pcm_capture_prepared());
    if (fail_config) {
        return -EINVAL;
    }
    g_audio.out_hz = rate;
    g_audio.cap_hz = rate;
    g_audio.src_channels = channels;
    g_audio.playback_backend = WAVE_PLAYER_BACKEND_I2S_CODEC;
    g_audio.tx_block_bytes = tx_bytes;
    g_audio.logical_tx_block_bytes = tx_bytes;
    g_audio.tx_period_samples = tx_bytes / (wave_player_default_playback_channels() * sizeof(int16_t));
    g_audio.io_started = true;
    tx_peripheral_paused = false;
    tx_dma_restarted = true;
    g_audio.cap_isr_seq = 0;
    g_audio.tx_total_samples = 0;
    ++g_audio.cap_epoch;
    return 0;
}

static void bflb_dma_channel_stop(void *device)
{
    assert(device == g_audio.dma_ch_tx);
    assert(tx_peripheral_paused);
    /* A stop retains partially consumed hardware SRC/CONTROL. Only reload
     * resets those registers; changing LLI alone cannot make a safe prime. */
    tx_registers_reloaded = false;
}

static int bflb_dma_channel_lli_reload(void *device, void *pool, uint32_t capacity,
                                       struct bflb_dma_channel_lli_transfer_s *transfers, uint32_t count)
{
    assert(device == g_audio.dma_ch_tx && pool == g_audio.tx_llipool && capacity >= count && count == 2);
    assert(transfers[0].src_addr == (uint32_t)(uintptr_t)g_audio.pcm_ping);
    assert(transfers[1].src_addr == (uint32_t)(uintptr_t)g_audio.pcm_pong);
    assert(transfers[0].dst_addr == DMA_ADDR_I2S_TDR && transfers[1].dst_addr == DMA_ADDR_I2S_TDR);
    assert(transfers[0].nbytes == g_audio.tx_block_bytes && transfers[1].nbytes == g_audio.tx_block_bytes);
    assert(tx_peripheral_paused);
    if (fail_reload) {
        return -1;
    }
    tx_registers_reloaded = true;
    return 2;
}

static void bflb_dma_channel_tcint_clear(void *device)
{
    assert(device == g_audio.dma_ch_tx);
}

static void bflb_dma_channel_lli_link_head(void *device, void *pool, uint32_t count)
{
    assert(device == g_audio.dma_ch_tx && pool == g_audio.tx_llipool && count == g_audio.tx_lli_count);
}

static void bflb_dma_channel_start(void *device)
{
    assert(device == g_audio.dma_ch_tx);
    assert(tx_registers_reloaded && tx_peripheral_paused);
    tx_dma_restarted = true;
    ++tx_starts;
}

static int wave_player_start_internal(void)
{
    if (capture_reservation_hook != NULL) {
        void (*hook)(void) = capture_reservation_hook;
        capture_reservation_hook = NULL;
        hook();
    }
    if (fail_start) {
        audio_pcm_player_cleanup();
        return -1;
    }
    g_audio.stop = false;
    g_audio.wave_task = &fake_ring;
    return 0;
}

static uint32_t Ring_Buffer_Get_Empty_Length(const int *ring)
{
    assert(ring == &g_audio.wave_rb);
    return fake_ring.size - fake_ring.used;
}

static int wave_player_feed_internal(uint8_t *data, uint32_t bytes)
{
    assert(data != NULL && bytes <= fake_ring.size - fake_ring.used);
    fake_ring.used += bytes;
    return (int)bytes;
}

uint32_t wave_player_get_buffered_bytes(void)
{
    return fake_ring.used;
}

static uint32_t wave_player_trim_buffer_internal(uint32_t max_bytes)
{
    unsigned trimmed = fake_ring.used > max_bytes ? fake_ring.used - max_bytes : 0;
    fake_ring.used -= trimmed;
    return trimmed;
}

static void wave_player_flush_internal(void)
{
    fake_ring.used = 0;
}

int audio_codec_set_speaker_channel_mute(audio_codec_dev_t *codec, uint16_t mask, bool mute)
{
    assert(codec == &g_audio.play_codec && mask != 0 && (mask & ~3U) == 0U);
    mute_calls++;
    if (capture_reservation_hook != NULL) {
        void (*hook)(void) = capture_reservation_hook;
        capture_reservation_hook = NULL;
        hook();
    }
    if (mute_error == 0) {
        (void)mute;
    }
    return mute_error;
}

int audio_codec_set_speaker_channel_volume(audio_codec_dev_t *codec, uint16_t mask, int volume)
{
    assert(codec == &g_audio.play_codec && mask != 0 && (mask & ~3U) == 0U && volume >= 0 && volume <= 100);
    volume_calls++;
    if (capture_reservation_hook != NULL) {
        void (*hook)(void) = capture_reservation_hook;
        capture_reservation_hook = NULL;
        hook();
    }
    if (volume_error == 0) {
        current_playback_volume = volume;
    }
    return volume_error;
}

int audio_codec_set_mic_channel_volume(audio_codec_dev_t *codec, uint16_t mask, int volume)
{
    assert(codec == &g_audio.cap_codec && mask != 0 && (mask & ~12U) == 0U);
    assert(volume >= 0 && volume <= 100);
    ++record_control_calls;
    if (record_control_error != 0) {
        return record_control_error;
    }
    current_record_volume = volume;
    for (unsigned c = 0; c < AUDIO_CODEC_MAX_SLOTS; ++c) {
        if (mask & (1U << c)) {
            record_volume_slots[c] = volume;
        }
    }
    return 0;
}

int audio_codec_set_mic_channel_gain_db(audio_codec_dev_t *codec, uint16_t mask, float gain_db)
{
    int step = audio_codec_gain_step_index(mock_mic_gain_steps,
                                         sizeof(mock_mic_gain_steps) / sizeof(mock_mic_gain_steps[0]), gain_db);
    assert(step >= 0);
    assert(codec == &g_audio.cap_codec && mask != 0 && (mask & ~12U) == 0U);
    assert(gain_db >= -12.0f && gain_db <= 24.0f);
    ++record_control_calls;
    if (record_control_error != 0) {
        return record_control_error;
    }
    current_record_gain_db = mock_mic_gain_steps[step];
    for (unsigned c = 0; c < AUDIO_CODEC_MAX_SLOTS; ++c) {
        if (mask & (1U << c)) {
            record_gain_slots[c] = mock_mic_gain_steps[step];
        }
    }
    return 0;
}

int audio_codec_set_mic_channel_mute(audio_codec_dev_t *codec, uint16_t mask, bool mute)
{
    assert(codec == &g_audio.cap_codec && mask != 0 && (mask & ~12U) == 0U);
    (void)mute;
    ++record_control_calls;
    return record_control_error;
}

#include "wave_player_controls_under_test.h"

/* Exercise the real engine boundary and session module together. The engine
 * primitives above provide controlled RTOS, codec and DMA failures. */
#include "wave_player_pcm_engine_under_test.h"
#include "../audio_pcm.c"

static char cli_message[160];

static int capture_cli_message(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    int result = vsnprintf(cli_message, sizeof(cli_message), format, arguments);
    va_end(arguments);
    assert(result >= 0 && (size_t)result < sizeof(cli_message));
    return result;
}

#define printf capture_cli_message
#include "wave_player_cli_controls_under_test.h"
#undef printf

static uint8_t speaker_volume(void)
{
    uint8_t volume = 0;
    assert(audio_pcm_get_speaker_volume(&volume) == 0);
    return volume;
}

static void test_cli_uninitialized(void)
{
    char *mic_on[] = { "mic_mute", "1" };
    assert(audio_pcm_set_mic_mute(true) == -ENODEV);
    assert(cmd_mic_mute(2, mic_on) == -ENODEV && audio_pcm_get_mic_mute_mask() == 0);
}

static void test_record_mute_controls(const audio_pcm_config_t *config)
{
    audio_codec_channel_state_t state;
    assert(audio_pcm_set_mic_channel_mute(4, true) == 0);
    assert(audio_pcm_get_mic_mute_mask() == 4 && !audio_pcm_get_mic_mute());
    assert(audio_pcm_get_mic_channel_state(2, &state) == 0 && state.muted);
    assert(audio_pcm_get_mic_channel_state(3, &state) == 0 && !state.muted);
    assert(audio_pcm_set_mic_mute(true) == 0 && audio_pcm_get_mic_mute_mask() == 12);
    for (unsigned attempt = 0; attempt < 2; ++attempt) {
        audio_pcm_config_t capture = *config;
        capture.sample_rate_hz = attempt ? 48000 : 16000;
        audio_pcm_handle_t session = NULL;
        assert(audio_pcm_open(AUDIO_PCM_CAPTURE, &capture, &session) == 0);
        assert(audio_pcm_get_mic_mute() && audio_pcm_get_mic_mute_mask() == 12);
        assert(audio_pcm_close(session, 20) == 0);
    }
    assert(audio_pcm_set_mic_channel_mute(4, false) == 0);
    assert(audio_pcm_get_mic_mute_mask() == 8 && !audio_pcm_get_mic_mute());
    assert(audio_pcm_set_mic_mute(false) == 0 && audio_pcm_get_mic_mute_mask() == 0);
}

static void test_mic_slot_controls(const audio_pcm_config_t *config)
{
    audio_pcm_handle_t session = NULL;
    audio_codec_channel_state_t state;
    audio_codec_channel_caps_t caps;
    assert(audio_pcm_set_mic_channel_volume(4, 32) == 0);
    assert(audio_pcm_set_mic_channel_gain_db(8, 10.5f) == 0);
    assert(audio_pcm_get_mic_channel_state(2, &state) == 0 && state.volume_percent == 32);
    assert(audio_pcm_get_mic_channel_state(3, &state) == 0 && state.gain_db == 12.0f);
    assert(audio_pcm_get_mic_channel_caps(2, &caps) == 0);
    assert(caps.independent_controls & AUDIO_CODEC_CONTROL_GAIN_DB);
    unsigned before = record_control_calls;
    const uint16_t invalid[] = { 0, 1, 5, 16, UINT16_MAX };
    for (unsigned i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        assert(audio_pcm_set_mic_channel_volume(invalid[i], 44) == -EINVAL);
        assert(audio_pcm_set_mic_channel_gain_db(invalid[i], 6.0f) == -EINVAL);
        assert(audio_pcm_set_mic_channel_mute(invalid[i], true) == -EINVAL);
    }
    assert(audio_pcm_set_mic_gain_db(NAN) == -EINVAL);
    assert(audio_pcm_set_mic_gain_db(INFINITY) == -EINVAL);
    assert(audio_pcm_set_mic_gain_db(25.0f) == -ERANGE);
    assert(record_control_calls == before);
    record_control_error = -2;
    assert(audio_pcm_set_mic_channel_volume(4, 44) == -ENOTSUP);
    assert(audio_pcm_get_mic_channel_state(2, &state) == 0 && state.volume_percent == 32);
    assert(state.valid & AUDIO_CODEC_CONTROL_VOLUME);
    record_control_error = -1;
    assert(audio_pcm_set_mic_channel_gain_db(8, 6.0f) == -EIO);
    assert(audio_pcm_get_mic_channel_state(3, &state) == 0 && state.gain_db == 12.0f);
    assert(!(state.valid & AUDIO_CODEC_CONTROL_GAIN_DB));
    record_control_error = 0;
    assert(audio_pcm_set_mic_channel_gain_db(8, 12.0f) == 0);
    for (unsigned attempt = 0; attempt < 2; ++attempt) {
        audio_pcm_config_t capture = *config;
        capture.sample_rate_hz = attempt ? 48000 : 16000;
        assert(audio_pcm_open(AUDIO_PCM_CAPTURE, &capture, &session) == 0);
        assert(record_volume_slots[2] == 32 && record_volume_slots[3] == 100);
        assert(record_gain_slots[2] == 6.0f && record_gain_slots[3] == 12.0f);
        assert(audio_pcm_close(session, 20) == 0);
    }
    assert(audio_pcm_set_mic_volume(72) == 0);
    assert(audio_pcm_set_mic_gain_db(6.0f) == 0);
}

static void test_cli_presets(const audio_pcm_config_t *config)
{
    char *volume[] = { "speaker_vol", "20" };
    char *new_volume[] = { "speaker_vol", "90" };
    char *mute_on[] = { "speaker_mute", "1" };
    char *mute_off[] = { "speaker_mute", "0" };
    audio_pcm_handle_t session = NULL;
    assert(cmd_speaker_vol(2, volume) == 0 && speaker_volume() == 20);
    assert(cmd_speaker_mute(2, mute_on) == 0);
    g_audio.io_started = true;
    assert(cmd_speaker_vol(2, new_volume) == 0 && speaker_volume() == 90);
    assert(cmd_speaker_mute(2, mute_off) == 0);
    g_audio.io_started = false;
    /* Preset controls survive opening the first session. */
    assert(audio_pcm_open(AUDIO_PCM_CAPTURE, config, &session) == 0);
    assert(speaker_volume() == 90 && current_playback_volume == 90 && !audio_pcm_get_speaker_mute());
    assert(audio_pcm_close(session, 20) == 0);
}

static void test_cli_pcm_owned(void)
{
    char *volume[] = { "speaker_vol", "20" };
    char *mute_on[] = { "speaker_mute", "1" };
    assert(audio_pcm_owned());
    assert(cmd_speaker_vol(2, volume) == 0 && speaker_volume() == 20);
    volume[1] = "90";
    assert(cmd_speaker_vol(2, volume) == 0 && speaker_volume() == 90);
    assert(cmd_speaker_mute(2, mute_on) == 0 && audio_pcm_get_speaker_mute());
    unsigned before = volume_calls;
    volume_error = -1;
    volume[1] = "20";
    assert(cmd_speaker_vol(2, volume) == -EIO && volume_calls == before + 1);
    uint8_t uncertain = 77;
    assert(audio_pcm_get_speaker_volume(&uncertain) == -ENOTSUP && uncertain == 77);
    volume_error = 0;
    volume[1] = "90";
    assert(cmd_speaker_vol(2, volume) == 0 && speaker_volume() == 90);
    char *mic_volume[] = { "mic_vol", "83" };
    char *mic_gain[] = { "mic_gain_db", "-6.0" };
    assert(cmd_mic_vol(2, mic_volume) == 0 && record_volume_slots[2] == 83 && record_volume_slots[3] == 83);
    assert(cmd_mic_gain_db(2, mic_gain) == 0 && record_gain_slots[2] == -6.0f && record_gain_slots[3] == -6.0f);
    char *selected[] = { "mic_vol", "34", "4" };
    assert(cmd_mic_vol(3, selected) == 0 && record_volume_slots[2] == 34 && record_volume_slots[3] == 83);
    selected[1] = "10.5";
    selected[2] = "8";
    assert(cmd_mic_gain_db(3, selected) == 0 && record_gain_slots[2] == -6.0f && record_gain_slots[3] == 12.0f);
    char *query[] = { "audio_channel", "mic", "3" };
    unsigned before_query = volume_calls + record_control_calls;
    assert(cmd_audio_channel(3, query) == 0 && strstr(cli_message, "role=mic slot=3") != NULL);
    assert(strstr(cli_message, "volume=83") != NULL && strstr(cli_message, "gain_db=12.00") != NULL);
    assert(volume_calls + record_control_calls == before_query);
    char *partial_mute[] = { "mic_mute", "1", "4" };
    assert(cmd_mic_mute(3, partial_mute) == 0 && audio_pcm_get_mic_mute_mask() == 4);
    assert(!audio_pcm_get_mic_mute());
    partial_mute[1] = "0";
    assert(cmd_mic_mute(3, partial_mute) == 0 && audio_pcm_get_mic_mute_mask() == 0);
    const char *invalid_gain[] = { "nan", "inf", "-inf", "6tail", "" };
    for (unsigned i = 0; i < sizeof(invalid_gain) / sizeof(invalid_gain[0]); ++i) {
        mic_gain[1] = (char *)invalid_gain[i];
        assert(cmd_mic_gain_db(2, mic_gain) == -EINVAL);
    }
    selected[1] = "50";
    selected[2] = "0";
    before = record_control_calls;
    assert(cmd_mic_vol(3, selected) == -EINVAL);
    selected[2] = "1";
    assert(cmd_mic_vol(3, selected) == -EINVAL && record_control_calls == before);
}

static audio_pcm_frame_info_t received;
static audio_pcm_frame_info_t reference;
static unsigned received_calls;
static unsigned reference_calls;
static pthread_mutex_t callback_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t callback_cond = PTHREAD_COND_INITIALIZER;
static bool block_callback;
static bool callback_entered;
static bool callback_release;
static int close_from_callback_result;
static bool pause_reference;
static bool reference_paused;
static bool reference_release;
static bool handoff_done;
static bool handoff_result;
static bool reference_self_open;
static uint32_t paused_reference_sequence;

static void test_enter_critical(void)
{
    if (critical_depth++ == 0U) {
        assert(pthread_mutex_lock(&critical) == 0);
    }
}

static void test_exit_critical(void)
{
    assert(critical_depth != 0U);
    if (--critical_depth != 0U) {
        return;
    }
    bool pause = schedule_capture_reference && pause_reference &&
                 g_pcm.playback_sequence == paused_reference_sequence &&
                 g_pcm.sessions[AUDIO_PCM_CAPTURE].playback_callback_users != 0U;
    if (pause) {
        pause_reference = false;
    }
    assert(pthread_mutex_unlock(&critical) == 0);
    if (pause) {
        assert(pthread_mutex_lock(&handoff_lock) == 0);
        reference_paused = true;
        assert(pthread_cond_broadcast(&handoff_condition) == 0);
        while (!reference_release) {
            assert(pthread_cond_wait(&handoff_condition, &handoff_lock) == 0);
        }
        assert(pthread_mutex_unlock(&handoff_lock) == 0);
    }
}

static void on_capture(const int16_t *samples, const audio_pcm_frame_info_t *info, void *context)
{
    assert(samples != NULL && context == &received);
    received = *info;
    ++received_calls;
    if (block_callback) {
        assert(pthread_mutex_lock(&callback_lock) == 0);
        callback_entered = true;
        assert(pthread_cond_broadcast(&callback_cond) == 0);
        while (!callback_release) {
            assert(pthread_cond_wait(&callback_cond, &callback_lock) == 0);
        }
        assert(pthread_mutex_unlock(&callback_lock) == 0);
    }
}

static void on_reference(const int16_t *samples, const audio_pcm_frame_info_t *info, void *context)
{
    assert(samples != NULL && context == &reference);
    if (reference_self_open) {
        audio_pcm_handle_t session = NULL;
        const audio_pcm_config_t config = {
            .sample_rate_hz = 16000, .period_samples = 160, .channels = 1, .bits_per_sample = 16
        };
        assert(audio_pcm_open(AUDIO_PCM_PLAYBACK, &config, &session) == -EDEADLK && session == NULL);
        reference_self_open = false;
    }
    if (reference_calls != 0U && reference.epoch == info->epoch) {
        assert(info->first_sample_index >= reference.first_sample_index + reference.samples_per_channel);
        assert(info->sequence == reference.sequence + 1U);
    }
    reference = *info;
    ++reference_calls;
}

static void on_self_close(const int16_t *samples, const audio_pcm_frame_info_t *info, void *context)
{
    (void)samples;
    (void)info;
    close_from_callback_result = audio_pcm_close(context, 0);
}

static wave_player_rec_frame_t frame;
static int16_t pcm[4 * WAVE_PLAYER_CAP_MAX_SAMPLES_PER_CH];

static void *capture_worker(void *context)
{
    schedule_capture_reference = context != NULL;
    current_task = g_audio.cap_task;
    audio_pcm_capture_dispatch(&frame, NULL);
    return NULL;
}

static void *reference_handoff_worker(void *context)
{
    (void)context;
    current_task = g_audio.wave_task;
    schedule_handoff_wait = true;
    handoff_result = audio_pcm_player_wait_reference();
    if (handoff_result) {
        audio_pcm_playback_dispatch(pcm, 160, 16000, 1, NULL);
        audio_pcm_player_started();
    } else {
        audio_pcm_player_failed();
        audio_pcm_player_cleanup();
        taskENTER_CRITICAL();
        g_audio.wave_task = NULL;
        g_audio.stop = false;
        taskEXIT_CRITICAL();
    }
    assert(pthread_mutex_lock(&handoff_lock) == 0);
    handoff_done = true;
    assert(pthread_cond_broadcast(&handoff_condition) == 0);
    assert(pthread_mutex_unlock(&handoff_lock) == 0);
    return NULL;
}

static void test_reference_handoff(bool cancel, bool fail)
{
    audio_pcm_handle_t input = NULL;
    audio_pcm_handle_t output = NULL;
    const audio_pcm_config_t capture = {
        .sample_rate_hz = 16000, .period_samples = 160, .channels = 4, .bits_per_sample = 16
    };
    const audio_pcm_config_t playback = {
        .sample_rate_hz = 16000, .period_samples = 160, .channels = 1, .bits_per_sample = 16
    };
    assert(audio_pcm_open(AUDIO_PCM_CAPTURE, &capture, &input) == 0);
    assert(audio_pcm_set_playback_callback(input, on_reference, &reference) == 0);
    frame = (wave_player_rec_frame_t){
        .samples = pcm,
        .sample_rate_hz = 16000,
        .samples_per_channel = 640,
        .channels = 4,
        .layout = g_audio.capture_layout,
        .sequence = 1,
        .epoch = g_audio.cap_epoch,
    };
    reference_calls = 0;
    reference_self_open = true;
    pause_reference = true;
    reference_paused = reference_release = handoff_waiting = handoff_done = false;
    paused_reference_sequence = 1;
    pthread_t capture_thread;
    assert(pthread_create(&capture_thread, NULL, capture_worker, &frame) == 0);
    assert(pthread_mutex_lock(&handoff_lock) == 0);
    while (!reference_paused) {
        assert(pthread_cond_wait(&handoff_condition, &handoff_lock) == 0);
    }
    assert(pthread_mutex_unlock(&handoff_lock) == 0);
    assert(reference_calls == 0 && g_pcm.sessions[AUDIO_PCM_CAPTURE].playback_callback_users == 1);
    assert(audio_pcm_set_playback_callback(input, NULL, NULL) == -EBUSY);
    fail_start = fail;
    assert(audio_pcm_open(AUDIO_PCM_PLAYBACK, &playback, &output) == (fail ? -EIO : 0));
    fail_start = false;
    pthread_t writer_thread;
    if (!fail) {
        assert(pthread_create(&writer_thread, NULL, reference_handoff_worker, NULL) == 0);
        assert(pthread_mutex_lock(&handoff_lock) == 0);
        while (!handoff_waiting && !handoff_done) {
            assert(pthread_cond_wait(&handoff_condition, &handoff_lock) == 0);
        }
        assert(handoff_waiting && !handoff_done && reference_calls == 0);
        assert(pthread_mutex_unlock(&handoff_lock) == 0);
        if (cancel) {
            hold_worker = true;
            assert(audio_pcm_close(output, 100) == 0);
            assert(pthread_join(writer_thread, NULL) == 0 && !handoff_result);
            hold_worker = false;
            assert(g_audio.rec_enable && g_pcm.sessions[AUDIO_PCM_CAPTURE].playback_callback_users == 1);
        }
    } else {
        assert(output == NULL && !g_pcm.sessions[AUDIO_PCM_PLAYBACK].used);
    }
    assert(pthread_mutex_lock(&handoff_lock) == 0);
    reference_release = true;
    assert(pthread_cond_broadcast(&handoff_condition) == 0);
    assert(pthread_mutex_unlock(&handoff_lock) == 0);
    assert(pthread_join(capture_thread, NULL) == 0);
    if (!fail && !cancel) {
        assert(pthread_join(writer_thread, NULL) == 0 && handoff_result);
        assert(reference_calls == 2 && reference.sequence == 2 && reference.first_sample_index == 640);
        assert(audio_pcm_close(output, 20) == 0);
    } else {
        assert(reference_calls == 1 && reference.sequence == 1);
    }
    assert(audio_pcm_close(input, 20) == 0);
}

static void capture_during_writer_reservation(void)
{
    assert(g_audio.wave_task == NULL && g_pcm.sessions[AUDIO_PCM_PLAYBACK].used);
    audio_pcm_capture_dispatch(&frame, NULL);
    assert(reference_calls == 0);
}

static void test_reference_reservation(void)
{
    audio_pcm_handle_t input = NULL;
    audio_pcm_handle_t output = NULL;
    audio_pcm_config_t config = { .sample_rate_hz = 16000, .period_samples = 160, .channels = 4, .bits_per_sample = 16 };
    assert(audio_pcm_open(AUDIO_PCM_CAPTURE, &config, &input) == 0);
    assert(audio_pcm_set_playback_callback(input, on_reference, &reference) == 0);
    frame = (wave_player_rec_frame_t){
        .samples = pcm,
        .sample_rate_hz = 16000,
        .samples_per_channel = 640,
        .channels = 4,
        .layout = g_audio.capture_layout,
        .sequence = 1,
        .epoch = g_audio.cap_epoch,
    };
    reference_calls = 0;
    reference_self_open = false;
    capture_reservation_hook = capture_during_writer_reservation;
    config.channels = 1;
    assert(audio_pcm_open(AUDIO_PCM_PLAYBACK, &config, &output) == 0);
    assert(capture_reservation_hook == NULL && reference_calls == 0);
    audio_pcm_player_started();
    assert(audio_pcm_close(output, 20) == 0);
    assert(audio_pcm_close(input, 20) == 0);
}

int main(void)
{
    current_task = &current_task;
    test_cli_uninitialized();
    fake_ring.size = 11;
    const wave_player_hw_cfg_t hardware = {
        .i2s_dev_name = "i2s0",
        .capture_layout = { .mic_mask = 12, .ref_mask = 1 },
    };
    audio_pcm_handle_t input = NULL;
    audio_pcm_handle_t output = NULL;
    audio_pcm_config_t cap = { .sample_rate_hz = 16000, .period_samples = 160, .channels = 4, .bits_per_sample = 16 };
    audio_pcm_config_t play = { .sample_rate_hz = 16000, .period_samples = 160, .channels = 1, .bits_per_sample = 16 };
    assert(audio_pcm_open(AUDIO_PCM_CAPTURE, &cap, &input) == -ENODEV);
    fail_init = true;
    assert(audio_pcm_init(&hardware) == -EIO);
    fail_init = false;
    assert(audio_pcm_init(&hardware) == 0);
    assert(audio_pcm_init(&hardware) == -EALREADY);
    unsigned initialized = init_calls;
    unsigned char initialized_state[sizeof(g_audio)];
    memcpy(initialized_state, &g_audio, sizeof(g_audio));
    char same_name[] = "i2s0";
    wave_player_hw_cfg_t copied_hardware = hardware;
    copied_hardware.i2s_dev_name = same_name;
    assert(audio_pcm_init(&copied_hardware) == -EALREADY);
    same_name[0] = 'x';
    assert(audio_pcm_init(&copied_hardware) == -EALREADY);
    assert(audio_pcm_init(&hardware) == -EALREADY);
    assert(init_calls == initialized && memcmp(initialized_state, &g_audio, sizeof(g_audio)) == 0);
    const audio_controls_config_t narrow_controls = {
        .playback = &g_audio.play_codec, .capture = &g_audio.cap_codec,
        .speaker_mask = 1, .mic_mask = 12, .tx_slots = 2, .tx_slot_bytes = sizeof(int16_t),
    };
    assert(audio_controls_init(&narrow_controls) == 0);
    assert(wave_player_set_speaker_mute(true) == 0 && wave_player_get_speaker_mute());
    wave_player_pcm_engine_state_t narrow_state;
    wave_player_pcm_engine_snapshot(&narrow_state);
    assert(!narrow_state.muted);
    audio_controls_config_t full_controls = narrow_controls;
    full_controls.speaker_mask = 3;
    assert(audio_controls_init(&full_controls) == 0);
    test_mic_slot_controls(&cap);
    test_record_mute_controls(&cap);
    test_cli_presets(&cap);
    assert(audio_pcm_set_speaker_volume(37) == 0);
    assert(audio_pcm_set_speaker_mute(true) == 0);
    assert(audio_pcm_set_mic_volume(72) == 0);
    assert(audio_pcm_set_mic_gain_db(6.0f) == 0);
    wave_player_hw_cfg_t other_hardware = hardware;
    other_hardware.dma_tx_dev_name = "dma_other";
    assert(audio_pcm_init(&other_hardware) == -EALREADY);
    assert(init_calls == initialized);
    audio_pcm_caps_t caps;
    assert(audio_pcm_get_caps(&caps) == 0 && caps.capture_channels == 4 && caps.bits_per_sample == 16);
    assert(caps.mic_mask == 12 && caps.speaker_mask == 3);
    audio_pcm_caps_t initialized_caps = caps;
    void *capture_dma = g_audio.dma_ch_rx;
    g_audio.dma_ch_rx = NULL;
    assert(audio_pcm_get_caps(&caps) == 0 && memcmp(&caps, &initialized_caps, sizeof(caps)) == 0);
    g_audio.dma_ch_rx = capture_dma;
    cap.bits_per_sample = 24;
    assert(audio_pcm_open(AUDIO_PCM_CAPTURE, &cap, &input) == -ENOTSUP);
    cap.bits_per_sample = 16;
    fail_config = true;
    assert(audio_pcm_open(AUDIO_PCM_CAPTURE, &cap, &input) == -EINVAL && input == NULL);
    assert(!audio_pcm_owned());
    fail_config = false;
    assert(audio_pcm_open(AUDIO_PCM_CAPTURE, &cap, &input) == 0);
    assert(speaker_volume() == 37 && audio_pcm_get_speaker_mute() && current_record_volume == 72 && current_record_gain_db == 6.0f);
    assert(!g_audio.rec_stop && g_audio.rec_enable);
    test_cli_pcm_owned();
    /* Session controls use the engine while capture is active. */
    assert(audio_pcm_set_speaker_volume(38) == 0 && speaker_volume() == 38);
    assert(audio_pcm_set_mic_volume(73) == 0 && current_record_volume == 73);
    assert(audio_pcm_set_mic_gain_db(12.0f) == 0 && current_record_gain_db == 12.0f);
    unsigned configured = config_calls;
    unsigned stops = transport_stops;
    assert(audio_pcm_set_capture_callback(input, on_capture, &received) == 0);
    assert(audio_pcm_set_playback_callback(input, on_reference, &reference) == 0);
    frame = (wave_player_rec_frame_t){
        .samples = pcm,
        .samples_per_channel = 640,
        .sample_rate_hz = 16000,
        .channels = 4,
        .layout = hardware.capture_layout,
        .muted_mic_mask = 12,
        .sequence = 1,
        .epoch = g_audio.cap_epoch,
        .first_sample_index = 0,
    };
    audio_pcm_capture_dispatch(&frame, NULL);
    assert(received_calls == 1 && received.first_sample_index == 0 && received.layout.mic_mask == 12);
    assert(received.muted_mic_mask == 12 && reference.muted_mic_mask == 0);
    frame.muted_mic_mask = 0;
    assert(reference_calls == 1 && reference.channels == 1 && reference.layout.ref_mask == 1);
    frame.sequence = 4;
    frame.first_sample_index = 1920;
    audio_pcm_capture_dispatch(&frame, NULL);
    assert(received_calls == 2 && received.pts_us == 120000 && received.muted_mic_mask == 0);
    assert(received.flags == AUDIO_PCM_FRAME_DISCONTINUITY);
    uint32_t first_epoch = received.epoch;
    play.sample_rate_hz = 48000;
    assert(audio_pcm_open(AUDIO_PCM_PLAYBACK, &play, &output) == -EBUSY);
    assert(config_calls == configured && g_audio.rec_enable);
    play.sample_rate_hz = 16000;
    fail_start = true;
    assert(audio_pcm_open(AUDIO_PCM_PLAYBACK, &play, &output) == -EIO && output == NULL);
    assert(config_calls == configured && transport_stops == stops && g_audio.rec_enable);
    fail_start = false;
    assert(audio_pcm_open(AUDIO_PCM_PLAYBACK, &play, &output) == 0);
    assert(config_calls == configured && g_audio.rec_enable && g_audio.src_channels == 1);
    g_audio.tx_total_samples = 2560;
    audio_pcm_playback_dispatch(pcm, 160, 16000, 1, NULL);
    assert(reference.first_sample_index == 2560 && reference.flags == AUDIO_PCM_FRAME_DISCONTINUITY);
    audio_pcm_playback_dispatch(pcm, 160, 16000, 1, NULL);
    assert(reference.first_sample_index == 2720 && reference.flags == 0);
    audio_pcm_player_started();
    g_audio.tx_total_samples = 2720;
    audio_pcm_playback_dispatch(pcm, 160, 16000, 1, NULL);
    assert(reference.first_sample_index == 2880 && reference.flags == 0);
    g_audio.tx_total_samples = 3360;
    audio_pcm_playback_dispatch(pcm, 160, 16000, 1, NULL);
    assert(reference.first_sample_index == 3520 && reference.flags == AUDIO_PCM_FRAME_DISCONTINUITY);
    assert(audio_pcm_lock() == 0);
    assert(audio_pcm_write(output, pcm, 12) == -EAGAIN);
    audio_pcm_unlock();
    assert(audio_pcm_write(output, pcm, 12) == 10 && fake_ring.used == 10);
    assert(audio_pcm_write(output, pcm, 3) == -EINVAL);
    assert(audio_pcm_flush(output) == 0);
    g_audio.tx_total_samples = 3840;
    audio_pcm_playback_dispatch(pcm, 160, 16000, 1, NULL);
    assert(reference.first_sample_index == 4000 && reference.epoch == first_epoch);
    assert(reference.flags == AUDIO_PCM_FRAME_DISCONTINUITY);
    int16_t muted[] = { 1, 2, 3 };
    assert(audio_pcm_set_speaker_mute(true) == 0);
    audio_pcm_prepare_playback(muted, sizeof(muted));
    assert(muted[0] == 0 && muted[2] == 0);
    assert(audio_pcm_set_speaker_mute(false) == 0);
    hold_worker = true;
    assert(audio_pcm_close(output, 2) == -ETIMEDOUT);
    assert(audio_pcm_write(output, pcm, 2) == -ECANCELED);
    assert(g_audio.rec_enable && transport_stops == stops);
    unsigned references_while_closing = reference_calls;
    frame.sequence = 5;
    frame.first_sample_index = 2560;
    audio_pcm_capture_dispatch(&frame, NULL);
    assert(reference_calls == references_while_closing);
    hold_worker = false;
    assert(audio_pcm_close(output, 20) == 0);
    assert(transport_stops == stops && g_audio.rec_enable && audio_pcm_owned());
    frame.sequence = 6;
    frame.first_sample_index = 3200;
    audio_pcm_capture_dispatch(&frame, NULL);
    assert(reference_calls == references_while_closing);
    assert(reference.first_sample_index == 4000 && g_pcm.playback_next_sample == 4160);
    frame.sequence = 7;
    frame.first_sample_index = 3840;
    audio_pcm_capture_dispatch(&frame, NULL);
    assert(reference_calls == references_while_closing + 1);
    assert(reference.first_sample_index == 4160 && reference.samples_per_channel == 320);
    assert(reference.pts_us == 260000 && reference.epoch == first_epoch);
    assert(reference.flags == AUDIO_PCM_FRAME_DISCONTINUITY);
    frame.sequence = 8;
    frame.first_sample_index = 4480;
    audio_pcm_capture_dispatch(&frame, NULL);
    assert(reference.first_sample_index == 4480 && reference.samples_per_channel == 640);
    assert(reference.flags == 0);
    assert(audio_pcm_open(AUDIO_PCM_PLAYBACK, &play, &output) == 0);
    fail_reload = true;
    audio_pcm_player_started();
    assert(g_pcm.player_start_result == -EIO && g_audio.stop && !tx_registers_reloaded);
    assert(audio_pcm_write(output, pcm, 2) == -EIO);
    fail_reload = false;
    assert(audio_pcm_close(output, 20) == 0 && g_audio.rec_enable);
    play.channels = 2;
    assert(audio_pcm_open(AUDIO_PCM_PLAYBACK, &play, &output) == 0);
    assert(g_audio.src_channels == 2 && config_calls == configured);
    audio_pcm_player_started();
    audio_pcm_playback_dispatch(pcm, 160, 16000, 2, NULL);
    assert(reference.first_sample_index == 5120 && reference.samples_per_channel == 160);
    assert(reference.flags == AUDIO_PCM_FRAME_DISCONTINUITY);
    g_audio.tx_total_samples = 5840;
    audio_pcm_playback_dispatch(pcm, 160, 16000, 2, NULL);
    assert(reference.first_sample_index == 6000 && reference.flags == AUDIO_PCM_FRAME_DISCONTINUITY);
    audio_pcm_playback_dispatch(pcm, 160, 16000, 2, NULL);
    assert(reference.first_sample_index == 6160 && reference.flags == 0);
    assert(audio_pcm_write(output, pcm, 12) == 8 && fake_ring.used == 8);
    assert(audio_pcm_close(input, 20) == 0);
    assert(!g_audio.rec_enable && g_audio.io_started && g_audio.wave_task != NULL);
    assert(audio_pcm_close(output, 20) == 0 && !g_audio.io_started && !audio_pcm_owned());
    assert(audio_pcm_open(AUDIO_PCM_CAPTURE, &cap, &input) == 0);
    assert(audio_pcm_set_capture_callback(input, on_capture, &received) == 0);
    frame.sequence = 1;
    frame.first_sample_index = 0;
    frame.epoch = g_audio.cap_epoch;
    block_callback = true;
    pthread_t worker;
    assert(pthread_create(&worker, NULL, capture_worker, NULL) == 0);
    assert(pthread_mutex_lock(&callback_lock) == 0);
    while (!callback_entered) {
        assert(pthread_cond_wait(&callback_cond, &callback_lock) == 0);
    }
    assert(pthread_mutex_unlock(&callback_lock) == 0);
    assert(audio_pcm_set_capture_callback(input, NULL, NULL) == -EBUSY);
    assert(audio_pcm_close(input, 0) == -ETIMEDOUT);
    assert(pthread_mutex_lock(&callback_lock) == 0);
    callback_release = true;
    assert(pthread_cond_broadcast(&callback_cond) == 0);
    assert(pthread_mutex_unlock(&callback_lock) == 0);
    assert(pthread_join(worker, NULL) == 0);
    assert(audio_pcm_close(input, 20) == 0);
    unsigned calls_after_close = received_calls;
    audio_pcm_capture_dispatch(&frame, NULL);
    assert(received_calls == calls_after_close);
    assert(audio_pcm_open(AUDIO_PCM_CAPTURE, &cap, &input) == 0);
    assert(audio_pcm_set_capture_callback(input, on_self_close, input) == 0);
    current_task = g_audio.cap_task;
    audio_pcm_capture_dispatch(&frame, NULL);
    assert(close_from_callback_result == 0); /* The prior epoch is rejected. */
    frame.epoch = g_audio.cap_epoch;
    audio_pcm_capture_dispatch(&frame, NULL);
    assert(close_from_callback_result == -EDEADLK);
    current_task = &current_task;
    assert(audio_pcm_lock() == 0);
    assert(audio_pcm_close(input, 0) == -ETIMEDOUT);
    assert(audio_pcm_abort(input) == 0 && !g_audio.rec_enable);
    audio_pcm_unlock();
    assert(audio_pcm_close(input, 20) == 0);
    test_reference_handoff(false, false);
    test_reference_handoff(true, false);
    test_reference_handoff(false, true);
    test_reference_reservation();
    audio_pcm_player_failed();
    assert(tx_starts >= 3);
    assert(tx_pauses >= tx_resumes && tx_resumes == tx_starts);
    vSemaphoreDelete(g_pcm.lock);
    puts("PCM exclusive ownership, one-time init, exact format, DMA metadata, backpressure, abort and close barriers passed");
    return 0;
}
