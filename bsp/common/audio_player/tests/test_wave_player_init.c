/* Compile the production wave_player_init() with fault-injected RTOS allocation
 * and hardware stubs. No capture task is scheduled by this host test. */
#include "wave_player.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned UBaseType_t;
typedef void *SemaphoreHandle_t;
typedef void *QueueHandle_t;
typedef void *TaskHandle_t;
#define configMAX_PRIORITIES 16
#define pdPASS 1
#define WAVE_PLAYER_BUILTIN_CODEC_AVAILABLE 0

struct bflb_device_s { int unused; };
typedef struct { int unused; } Ring_Buffer_Type;
typedef struct { int unused; } cap_msg_t;

static struct {
    struct bflb_device_s *i2s_dev, *i2c_dev, *audac_dev, *auadc_dev, *dma_ch_tx, *dma_ch_rx;
    audio_codec_dev_t play_codec, cap_codec;
    wave_player_backend_t playback_backend, capture_backend;
    uint32_t src_hz, out_hz, cap_hz;
    uint8_t out_channels, cap_channels, tx_slot_count, tx_slot_word_bytes;
    SemaphoreHandle_t sem_tx_dma_done, sem_wave_rb_space;
    Ring_Buffer_Type wave_rb;
    uint8_t wave_src_buf[64];
#if WAVE_PLAYER_ENABLE_REC
    wave_player_capture_layout_t capture_layout;
    SemaphoreHandle_t sem_rx_dma_done;
    QueueHandle_t cap_queue;
    TaskHandle_t cap_task;
#endif
} g_audio;
static UBaseType_t playback_task_priority = 3;
static struct bflb_device_s device;
static unsigned allocation_calls, fail_at, hardware_calls, live_objects;

static void *allocate_object(void)
{
    if (++allocation_calls == fail_at) return NULL;
    void *object = malloc(1);
    assert(object != NULL);
    ++live_objects;
    return object;
}
static void delete_object(void *object)
{
    assert(object != NULL && live_objects > 0);
    free(object);
    --live_objects;
}
#define xSemaphoreCreateBinary() allocate_object()
#define vSemaphoreDelete(object) delete_object(object)
#if WAVE_PLAYER_ENABLE_REC
static QueueHandle_t xQueueCreate(UBaseType_t count, size_t size)
{
    assert(count == WAVE_PLAYER_CAP_DMA_BLOCKS * 2 && size == sizeof(cap_msg_t));
    return allocate_object();
}
#define vQueueDelete(object) delete_object(object)
static void audio_cap_proc(void *arg) { (void)arg; }
static int xTaskCreate(void (*entry)(void *), char *name, unsigned depth, void *arg,
                       UBaseType_t priority, TaskHandle_t *task)
{
    assert(entry == audio_cap_proc && strcmp(name, "audio_cap") == 0);
    assert(depth == 2048 && arg == NULL && priority == 7);
    void *object = allocate_object();
    if (object == NULL) return 0;
    *task = object;
    return pdPASS;
}
#endif
static wave_player_backend_t wave_player_resolve_backend(wave_player_backend_t requested,
    wave_player_backend_t configured, bool has_i2s, bool has_builtin)
{
    (void)configured;
    (void)has_builtin;
    return has_i2s ? requested : WAVE_PLAYER_BACKEND_AUTO;
}
static struct bflb_device_s *bflb_device_get_by_name(const char *name)
{
    assert(name != NULL);
    ++hardware_calls;
    return &device;
}
static void board_i2s_codec_gpio_init(void) { ++hardware_calls; }
static void bflb_i2c_init(struct bflb_device_s *dev, unsigned hz)
{
    assert(dev == &device && hz == 200000);
}
audio_codec_type_t audio_codec_scan(struct bflb_device_s *dev, audio_codec_dev_t *codec)
{
    codec->i2c = dev;
    codec->type = AUDIO_CODEC_TYPE_ES8388;
    codec->name = "mock ES8388";
    return codec->type;
}
int audio_codec_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    assert(dev->type == AUDIO_CODEC_TYPE_ES8388 && cfg->enable_adc && cfg->enable_dac);
    assert(cfg->mic_input_mode == AUDIO_CODEC_MIC_INPUT_DIFF);
    return 0;
}
int audio_codec_set_mute(audio_codec_dev_t *dev, bool mute) { (void)dev; (void)mute; return 0; }
int audio_codec_set_volume(audio_codec_dev_t *dev, int volume) { (void)dev; (void)volume; return 0; }
static int wave_player_apply_rec_mic_pga(uint8_t pga) { (void)pga; return 0; }
static uint8_t wave_player_default_playback_channels(void) { return WAVE_PLAYER_OUT_CHANNELS; }
static uint8_t wave_player_default_capture_channels(void) { return 2; }
static void rb_lock(void) { }
static void rb_unlock(void) { }
static void Ring_Buffer_Init(Ring_Buffer_Type *rb, uint8_t *data, size_t bytes,
                             void (*lock)(void), void (*unlock)(void))
{
    (void)rb; (void)data; (void)bytes; (void)lock; (void)unlock;
}
static void wave_player_diag_reset(void) { }

/* Generated from wave_player.c by the runner, rather than copying the function. */
#include "wave_player_init_under_test.h"

static void assert_no_resources(void)
{
    if (live_objects != 0) fprintf(stderr, "init failure leaked %u RTOS objects\n", live_objects);
    assert(live_objects == 0);
    assert(g_audio.sem_tx_dma_done == NULL && g_audio.sem_wave_rb_space == NULL);
#if WAVE_PLAYER_ENABLE_REC
    assert(g_audio.sem_rx_dma_done == NULL && g_audio.cap_queue == NULL && g_audio.cap_task == NULL);
#endif
}
static void release_success(void)
{
#if WAVE_PLAYER_ENABLE_REC
    delete_object(g_audio.cap_task);
    delete_object(g_audio.cap_queue);
    delete_object(g_audio.sem_rx_dma_done);
#endif
    delete_object(g_audio.sem_wave_rb_space);
    delete_object(g_audio.sem_tx_dma_done);
    memset(&g_audio, 0, sizeof(g_audio));
    assert_no_resources();
}
int main(void)
{
    wave_player_hw_cfg_t cfg = {.i2s_dev_name = "i2s0", .i2c_dev_name = "i2c0",
        .dma_tx_dev_name = "tx", .dma_rx_dev_name = "rx",
        .playback_backend = WAVE_PLAYER_BACKEND_I2S_CODEC,
        .capture_backend = WAVE_PLAYER_BACKEND_I2S_CODEC,
        .playback_task_priority = 7, .capture_task_priority = 7};
    assert(wave_player_init(NULL) < 0);
    cfg.playback_task_priority = configMAX_PRIORITIES;
    assert(wave_player_init(&cfg) < 0);
    cfg.playback_task_priority = 7;
    assert(allocation_calls == 0 && hardware_calls == 0);
#if WAVE_PLAYER_ENABLE_REC
    cfg.capture_layout = (wave_player_capture_layout_t){ .mic_mask = 2, .ref_mask = 2 };
    assert(wave_player_init(&cfg) < 0);
    cfg.capture_layout = (wave_player_capture_layout_t){ .mic_mask = 4 };
    assert(wave_player_init(&cfg) < 0);
    assert(allocation_calls == 0);
    cfg.capture_layout = (wave_player_capture_layout_t){ .mic_mask = 2, .ref_mask = 1 };
#endif
    enum { OBJECTS = WAVE_PLAYER_ENABLE_REC ? 5 : 2 };
    for (unsigned failure = 1; failure <= OBJECTS; ++failure) {
        for (unsigned attempt = 0; attempt < 2; ++attempt) {
            allocation_calls = 0;
            fail_at = failure;
            assert(wave_player_init(&cfg) < 0);
            assert_no_resources();
        }
        allocation_calls = 0;
        fail_at = 0;
        assert(wave_player_init(&cfg) == 0);
        assert(live_objects == OBJECTS && allocation_calls == OBJECTS);
        unsigned hardware_before = hardware_calls;
        unsigned char snapshot[sizeof(g_audio)];
        memcpy(snapshot, &g_audio, sizeof(g_audio));
        cfg.playback_task_priority = 6;
        assert(wave_player_init(&cfg) < 0);
        assert(hardware_calls == hardware_before && allocation_calls == OBJECTS);
        assert(memcmp(snapshot, &g_audio, sizeof(g_audio)) == 0 && playback_task_priority == 7);
        cfg.playback_task_priority = 7;
        release_success();
    }
    printf("Player init: %u allocation failure sites, repeated failure/retry and duplicate init passed (REC=%u)\n",
           OBJECTS, WAVE_PLAYER_ENABLE_REC);
    return 0;
}
