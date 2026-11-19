/* SPDX-License-Identifier: Apache-2.0 */
/* Logical PCM sessions sharing the wave-player hardware engine. */
#include "audio_pcm.h"
#include "wave_player_pcm.h"
#include "bflb_mtimer.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#if WAVE_PLAYER_ENABLE_REC
#define AUDIO_PCM_CAPTURE_SAMPLES_MAX ((WAVE_PLAYER_REC_MAX_FS_HZ * WAVE_PLAYER_REC_FRAME_MS) / 1000U)
#endif

struct audio_pcm_session {
    audio_pcm_direction_t direction;
    audio_pcm_config_t config;
    audio_pcm_frame_cb_t capture_callback;
    audio_pcm_frame_cb_t playback_callback;
    void *capture_context;
    void *playback_context;
    uint32_t generation;
    uint32_t callback_users;
    uint32_t capture_callback_users;
    uint32_t playback_callback_users;
    uint32_t io_users;
    uint32_t open_capture_sequence;
    uint32_t capture_epoch;
    uint32_t previous_capture_sequence;
    uint64_t capture_sample_index;
    bool used;
    bool closing;
    bool aborted;
    bool capture_seen;
};

static struct {
    SemaphoreHandle_t lock;
    audio_pcm_caps_t caps;
    struct audio_pcm_session sessions[2];
    uint32_t generation;
    uint32_t sample_rate_hz;
    uint32_t period_samples;
    bool initialized;
    bool claimed;
    bool prepare_capture;
    volatile int player_start_result;
    uint64_t playback_next_sample;
    uint32_t playback_sequence;
    bool playback_reference_valid;
    bool playback_reference_silent;
    bool playback_reference_gap;
    uint32_t engine_epoch;
    bool tx_resume_pending;
} g_pcm;

bool audio_pcm_owned(void)
{
    bool owned;
    taskENTER_CRITICAL();
    owned = g_pcm.claimed;
    taskEXIT_CRITICAL();
    return owned;
}

#if WAVE_PLAYER_ENABLE_REC
static void audio_pcm_capture_dispatch(const wave_player_rec_frame_t *frame, void *context);
#endif
static void audio_pcm_playback_dispatch(const int16_t *samples, uint32_t count, uint32_t rate, uint8_t channels,
                                        void *context);

/* The PCM device shares the existing DMA engine. Application-specific queues
 * and read deadlines belong to its consumer, not to this hardware owner. */
static int audio_pcm_lock(void)
{
    if (g_pcm.lock == NULL) {
        return -ENODEV;
    }
    return xSemaphoreTake(g_pcm.lock, portMAX_DELAY) == pdTRUE ? 0 : -EIO;
}

static int audio_pcm_lock_until(uint64_t started, uint32_t timeout_ms)
{
    if (g_pcm.lock == NULL) {
        return -ENODEV;
    }
    uint64_t elapsed = (bflb_mtimer_get_time_us() - started) / 1000U;
    uint32_t remaining = elapsed < timeout_ms ? timeout_ms - (uint32_t)elapsed : 0;
    TickType_t ticks = pdMS_TO_TICKS(remaining);
    if (remaining != 0U && ticks == 0U) {
        ticks = 1;
    }
    return xSemaphoreTake(g_pcm.lock, ticks) == pdTRUE ? 0 : -ETIMEDOUT;
}

static int audio_pcm_try_lock(void)
{
    if (g_pcm.lock == NULL) {
        return -ENODEV;
    }
    return xSemaphoreTake(g_pcm.lock, 0) == pdTRUE ? 0 : -EAGAIN;
}

static void audio_pcm_unlock(void)
{
    (void)xSemaphoreGive(g_pcm.lock);
}

static bool audio_pcm_session_valid(audio_pcm_handle_t session)
{
    return session == &g_pcm.sessions[AUDIO_PCM_PLAYBACK] || session == &g_pcm.sessions[AUDIO_PCM_CAPTURE];
}

static bool audio_pcm_session_live(audio_pcm_handle_t session)
{
    return audio_pcm_session_valid(session) && session->used && !session->closing && !session->aborted;
}

bool audio_pcm_keep_capture(void)
{
    bool keep;
    taskENTER_CRITICAL();
    keep = g_pcm.claimed && audio_pcm_session_live(&g_pcm.sessions[AUDIO_PCM_CAPTURE]);
    taskEXIT_CRITICAL();
    return keep;
}

bool audio_pcm_capture_prepared(void)
{
    return g_pcm.prepare_capture;
}

void audio_pcm_player_cleanup(void)
{
    if (!audio_pcm_keep_capture()) {
        wave_player_pcm_engine_stop_io();
    }
}

bool audio_pcm_player_wait_reference(void)
{
    if (!audio_pcm_owned()) {
        return true;
    }
    for (;;) {
        taskENTER_CRITICAL();
        uint32_t users = g_pcm.sessions[AUDIO_PCM_CAPTURE].playback_callback_users;
        wave_player_pcm_engine_state_t state;
        wave_player_pcm_engine_snapshot(&state);
        bool stopped = state.stopped;
        taskEXIT_CRITICAL();
        if (stopped || users == 0U) {
            return !stopped;
        }
        /* Writer reservation blocks new silence. Let the capture task finish
         * its admitted reference before this task publishes either prime. */
        vTaskDelay(1);
    }
}

void audio_pcm_player_started(void)
{
    if (audio_pcm_owned()) {
        if (g_pcm.tx_resume_pending) {
            if (wave_player_pcm_engine_resume_tx() != 0) {
                g_pcm.player_start_result = -EIO;
                return;
            }
            g_pcm.tx_resume_pending = false;
        }
        g_pcm.player_start_result = 0;
    }
}

void audio_pcm_player_failed(void)
{
    wave_player_pcm_engine_state_t state;
    wave_player_pcm_engine_snapshot(&state);
    if (audio_pcm_owned() && !state.stopped) {
        g_pcm.player_start_result = -EIO;
    }
}

void audio_pcm_prepare_playback(int16_t *samples, size_t bytes)
{
    bool silence;
    taskENTER_CRITICAL();
    wave_player_pcm_engine_state_t state;
    wave_player_pcm_engine_snapshot(&state);
    silence = g_pcm.claimed && (!audio_pcm_session_live(&g_pcm.sessions[AUDIO_PCM_PLAYBACK]) || state.muted);
    taskEXIT_CRITICAL();
    if (silence) {
        memset(samples, 0, bytes);
    }
}

int audio_pcm_init(const wave_player_hw_cfg_t *hardware)
{
    if (hardware == NULL) {
        return -EINVAL;
    }
    if (g_pcm.lock == NULL) {
        SemaphoreHandle_t candidate = xSemaphoreCreateMutex();
        if (candidate == NULL) {
            return -ENOMEM;
        }
        taskENTER_CRITICAL();
        if (g_pcm.lock == NULL) {
            g_pcm.lock = candidate;
            candidate = NULL;
        }
        taskEXIT_CRITICAL();
        if (candidate != NULL) {
            vSemaphoreDelete(candidate);
        }
    }
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    if (g_pcm.initialized) {
        result = -EALREADY;
    } else {
        result = wave_player_init(hardware) == 0 ? 0 : -EIO;
        if (!result) {
            wave_player_pcm_engine_get_caps(&g_pcm.caps);
            g_pcm.initialized = true;
        }
    }
    audio_pcm_unlock();
    return result;
}

int audio_pcm_get_caps(audio_pcm_caps_t *caps)
{
    if (caps == NULL) {
        return -EINVAL;
    }
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    if (!g_pcm.initialized) {
        result = -ENODEV;
    } else {
        *caps = g_pcm.caps;
    }
    audio_pcm_unlock();
    return result;
}

static bool audio_pcm_rate_supported(uint32_t rate)
{
    switch (rate) {
        case 8000:
        case 11025:
        case 12000:
        case 16000:
        case 22050:
        case 24000:
        case 32000:
        case 44100:
        case 48000:
            return true;
        default:
            return false;
    }
}

static int audio_pcm_validate_config(audio_pcm_direction_t direction, const audio_pcm_config_t *config)
{
    if (config == NULL || (direction != AUDIO_PCM_CAPTURE && direction != AUDIO_PCM_PLAYBACK)) {
        return -EINVAL;
    }
    if (config->bits_per_sample != 16 || !audio_pcm_rate_supported(config->sample_rate_hz) ||
        config->period_samples > WAVE_PLAYER_OUT_SAMPLES) {
        return -ENOTSUP;
    }
#if WAVE_PLAYER_FIX_SAMPLE_RATE > 0
    if (config->sample_rate_hz != WAVE_PLAYER_FIX_SAMPLE_RATE) {
        return -ENOTSUP;
    }
#endif
    if (direction == AUDIO_PCM_PLAYBACK) {
        return config->channels >= 1 && config->channels <= 2 ? 0 : -ENOTSUP;
    }
#if WAVE_PLAYER_ENABLE_REC
    return g_pcm.caps.capture_supported && config->channels == g_pcm.caps.capture_channels ? 0 : -ENOTSUP;
#else
    return -ENOTSUP;
#endif
}

int audio_pcm_open(audio_pcm_direction_t direction, const audio_pcm_config_t *config, audio_pcm_handle_t *out)
{
    if (out == NULL) {
        return -EINVAL;
    }
    *out = NULL;
    wave_player_pcm_engine_state_t state;
    wave_player_pcm_engine_snapshot(&state);
    if (state.called_from_audio_task) {
        return -EDEADLK;
    }
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    result = g_pcm.initialized ? audio_pcm_validate_config(direction, config) : -ENODEV;
    if (result) {
        audio_pcm_unlock();
        return result;
    }
    struct audio_pcm_session *session = &g_pcm.sessions[direction];
    bool first = !g_pcm.claimed;
    if (session->used || g_pcm.sessions[0].closing || g_pcm.sessions[1].closing) {
        result = -EBUSY;
    } else if (!first && (g_pcm.sample_rate_hz != config->sample_rate_hz ||
                          (config->period_samples && config->period_samples != g_pcm.period_samples))) {
        result = -EBUSY;
    }
    if (result) {
        audio_pcm_unlock();
        return result;
    }
    if (first) {
        uint32_t period = config->period_samples ? config->period_samples : WAVE_PLAYER_OUT_SAMPLES;
        uint8_t source_channels = direction == AUDIO_PCM_PLAYBACK ? config->channels : 1U;
        taskENTER_CRITICAL();
        g_pcm.claimed = true;
#if WAVE_PLAYER_ENABLE_REC
        g_pcm.prepare_capture = g_pcm.caps.capture_supported;
#endif
        wave_player_pcm_engine_attach(
#if WAVE_PLAYER_ENABLE_REC
            audio_pcm_capture_dispatch,
#endif
            audio_pcm_playback_dispatch, direction == AUDIO_PCM_CAPTURE);
        taskEXIT_CRITICAL();
        result = wave_player_pcm_engine_configure(config->sample_rate_hz, period, source_channels);
        g_pcm.prepare_capture = false;
        if (result) {
            wave_player_pcm_engine_stop_io();
            taskENTER_CRITICAL();
            g_pcm.claimed = false;
            wave_player_pcm_engine_detach();
            taskEXIT_CRITICAL();
            audio_pcm_unlock();
            return result;
        }
        g_pcm.sample_rate_hz = config->sample_rate_hz;
        g_pcm.period_samples = period;
        g_pcm.playback_next_sample = 0;
        g_pcm.playback_sequence = 0;
        g_pcm.playback_reference_valid = false;
        g_pcm.playback_reference_gap = false;
        ++g_pcm.engine_epoch;
    }
    taskENTER_CRITICAL();
    memset(session, 0, sizeof(*session));
    session->used = true;
    session->direction = direction;
    session->config = *config;
    session->generation = ++g_pcm.generation;
    wave_player_pcm_engine_snapshot(&state);
#if WAVE_PLAYER_ENABLE_REC
    session->open_capture_sequence = state.capture_sequence;
    session->capture_epoch = state.capture_epoch;
    if (direction == AUDIO_PCM_CAPTURE) {
        wave_player_pcm_engine_capture_enable(true);
    }
#endif
    taskEXIT_CRITICAL();
    if (direction == AUDIO_PCM_PLAYBACK) {
        /* Capture-only operation retains a silent TX DMA clock without a
         * software player. Selecting the source channels here does not touch
         * RX DMA, its queue, or its sample timeline. */
        g_pcm.player_start_result = 1;
        /* Reserve the writer before starting its task. Capture admission
         * already sees the live playback session and cannot add silence. */
        g_pcm.tx_resume_pending = true;
        result = wave_player_pcm_engine_start_playback(config->channels);
        if (result) {
            taskENTER_CRITICAL();
            session->aborted = true;
            session->closing = true;
            taskEXIT_CRITICAL();
            /* Task creation failed synchronously; the existing capture
             * transport remains owned and the silent TX clock can resume. */
            if (audio_pcm_keep_capture() && g_pcm.tx_resume_pending) {
                audio_pcm_player_started();
            }
            taskENTER_CRITICAL();
            session->used = false;
            session->closing = false;
            if (!g_pcm.sessions[AUDIO_PCM_CAPTURE].used) {
                g_pcm.claimed = false;
                wave_player_pcm_engine_detach();
            }
            taskEXIT_CRITICAL();
            audio_pcm_unlock();
            return -EIO;
        }
    }
    *out = session;
    audio_pcm_unlock();
    return 0;
}

static int audio_pcm_set_callback(audio_pcm_handle_t session, audio_pcm_frame_cb_t callback, void *context,
                                  bool capture)
{
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    taskENTER_CRITICAL();
    if (!audio_pcm_session_live(session) || (capture && session->direction != AUDIO_PCM_CAPTURE)) {
        result = -EINVAL;
    } else if (capture ? session->capture_callback_users != 0 : session->playback_callback_users != 0) {
        result = -EBUSY;
    } else if (capture) {
        session->capture_callback = callback;
        session->capture_context = context;
    } else {
        session->playback_callback = callback;
        session->playback_context = context;
    }
    taskEXIT_CRITICAL();
    audio_pcm_unlock();
    return result;
}

int audio_pcm_set_capture_callback(audio_pcm_handle_t session, audio_pcm_frame_cb_t callback, void *context)
{
    return audio_pcm_set_callback(session, callback, context, true);
}

int audio_pcm_set_playback_callback(audio_pcm_handle_t session, audio_pcm_frame_cb_t callback, void *context)
{
    return audio_pcm_set_callback(session, callback, context, false);
}

int audio_pcm_write(audio_pcm_handle_t session, const void *samples, size_t bytes)
{
    if (samples == NULL || bytes > INT_MAX) {
        return -EINVAL;
    }
    int result = audio_pcm_try_lock();
    if (result) {
        return result;
    }
    wave_player_pcm_engine_state_t state;
    wave_player_pcm_engine_snapshot(&state);
    if (!audio_pcm_session_valid(session) || !session->used || session->direction != AUDIO_PCM_PLAYBACK) {
        result = -EINVAL;
    } else if (session->aborted || session->closing) {
        result = -ECANCELED;
    } else if (bytes % (session->config.channels * sizeof(int16_t))) {
        result = -EINVAL;
    } else if (g_pcm.player_start_result < 0 || !state.playback_running) {
        result = -EIO;
    } else {
        uint32_t frame_bytes = session->config.channels * sizeof(int16_t);
        result = wave_player_pcm_engine_write(samples, bytes, frame_bytes);
    }
    audio_pcm_unlock();
    return result;
}

int audio_pcm_abort(audio_pcm_handle_t session)
{
    int result = 0;
    bool playback = false;
    taskENTER_CRITICAL();
    if (!audio_pcm_session_valid(session) || !session->used) {
        result = -EINVAL;
    } else {
        session->aborted = true;
        playback = session->direction == AUDIO_PCM_PLAYBACK;
        if (playback) {
            /* Keep the ring owner alive until the lock-free flush finishes. */
            ++session->io_users;
        }
#if WAVE_PLAYER_ENABLE_REC
        if (session->direction == AUDIO_PCM_CAPTURE) {
            wave_player_pcm_engine_capture_enable(false);
        }
#endif
    }
    taskEXIT_CRITICAL();
    if (playback) {
        wave_player_pcm_engine_flush();
        taskENTER_CRITICAL();
        --session->io_users;
        taskEXIT_CRITICAL();
    }
    return result;
}

int audio_pcm_close(audio_pcm_handle_t session, uint32_t timeout_ms)
{
    uint64_t started = bflb_mtimer_get_time_us();
    int result = audio_pcm_lock_until(started, timeout_ms);
    if (result) {
        return result;
    }
    if (!audio_pcm_session_valid(session) || !session->used) {
        audio_pcm_unlock();
        return -EINVAL;
    }
    wave_player_pcm_engine_state_t state;
    wave_player_pcm_engine_snapshot(&state);
    if (state.called_from_audio_task) {
        audio_pcm_unlock();
        return -EDEADLK;
    }
    uint32_t generation = session->generation;
    taskENTER_CRITICAL();
    session->closing = true;
    session->aborted = true;
#if WAVE_PLAYER_ENABLE_REC
    if (session->direction == AUDIO_PCM_CAPTURE) {
        wave_player_pcm_engine_capture_enable(false);
    }
#endif
    taskEXIT_CRITICAL();
    audio_pcm_unlock();
    for (;;) {
        uint32_t users;
        taskENTER_CRITICAL();
        users = session->callback_users + session->io_users;
        taskEXIT_CRITICAL();
        if (!users) {
            break;
        }
        if (bflb_mtimer_get_time_us() - started >= (uint64_t)timeout_ms * 1000U) {
            return -ETIMEDOUT;
        }
        vTaskDelay(1);
    }
    result = audio_pcm_lock_until(started, timeout_ms);
    if (result) {
        return result;
    }
    if (!session->used || session->generation != generation) {
        audio_pcm_unlock();
        return -EINVAL;
    }
    if (session->direction == AUDIO_PCM_PLAYBACK) {
        wave_player_pcm_engine_flush();
        uint64_t elapsed = (bflb_mtimer_get_time_us() - started) / 1000U;
        uint32_t remaining = elapsed < timeout_ms ? timeout_ms - (uint32_t)elapsed : 0;
        result = wave_player_pcm_engine_stop(remaining);
    } else if (!g_pcm.sessions[AUDIO_PCM_PLAYBACK].used) {
        uint64_t elapsed = (bflb_mtimer_get_time_us() - started) / 1000U;
        uint32_t remaining = elapsed < timeout_ms ? timeout_ms - (uint32_t)elapsed : 0;
        result = wave_player_pcm_engine_stop(remaining);
    }
    if (!result) {
        taskENTER_CRITICAL();
        session->capture_callback = NULL;
        session->playback_callback = NULL;
        session->capture_context = NULL;
        session->playback_context = NULL;
        session->used = false;
        session->closing = false;
        if (!g_pcm.sessions[0].used && !g_pcm.sessions[1].used) {
            g_pcm.claimed = false;
            wave_player_pcm_engine_detach();
        }
        taskEXIT_CRITICAL();
    }
    audio_pcm_unlock();
    return result;
}

static bool audio_pcm_callback_enter(audio_pcm_handle_t session, bool capture, audio_pcm_frame_cb_t *callback,
                                     void **context)
{
    bool acquired = false;
    taskENTER_CRITICAL();
    if (audio_pcm_session_live(session)) {
        *callback = capture ? session->capture_callback : session->playback_callback;
        *context = capture ? session->capture_context : session->playback_context;
        if (*callback != NULL) {
            ++session->callback_users;
            if (capture) {
                ++session->capture_callback_users;
            } else {
                ++session->playback_callback_users;
            }
            acquired = true;
        }
    }
    taskEXIT_CRITICAL();
    return acquired;
}

static void audio_pcm_callback_leave(audio_pcm_handle_t session, bool callback, bool capture)
{
    taskENTER_CRITICAL();
    --session->callback_users;
    if (callback) {
        if (capture) {
            --session->capture_callback_users;
        } else {
            --session->playback_callback_users;
        }
    }
    taskEXIT_CRITICAL();
}

static bool audio_pcm_reference_update(audio_pcm_frame_info_t *info, bool silent)
{
    taskENTER_CRITICAL();
    if (!silent) {
        wave_player_pcm_engine_state_t state;
        wave_player_pcm_engine_snapshot(&state);
        uint64_t scheduled = state.tx_total_samples;
        if (!g_pcm.tx_resume_pending) {
            /* A running player's inactive refill follows the current DMA
             * block, including after coalesced completion notifications. */
            scheduled += state.tx_period_samples;
        }
        info->first_sample_index = g_pcm.playback_next_sample;
        if (info->first_sample_index < scheduled) {
            info->first_sample_index = scheduled;
            info->flags |= AUDIO_PCM_FRAME_DISCONTINUITY;
        }
    }
    if (info->first_sample_index < g_pcm.playback_next_sample) {
        uint64_t overlap = g_pcm.playback_next_sample - info->first_sample_index;
        if (overlap >= info->samples_per_channel) {
            /* A queued capture descriptor can predate the final player
             * reference. Discard it instead of moving the clock backward. */
            g_pcm.playback_reference_gap = true;
            taskEXIT_CRITICAL();
            return false;
        }
        info->samples_per_channel -= (uint32_t)overlap;
        info->first_sample_index = g_pcm.playback_next_sample;
        info->flags |= AUDIO_PCM_FRAME_DISCONTINUITY;
    }
    if (info->first_sample_index > g_pcm.playback_next_sample || g_pcm.playback_reference_gap ||
        (g_pcm.playback_reference_valid && g_pcm.playback_reference_silent != silent)) {
        info->flags |= AUDIO_PCM_FRAME_DISCONTINUITY;
    }
    info->sequence = ++g_pcm.playback_sequence;
    g_pcm.playback_next_sample = info->first_sample_index + info->samples_per_channel;
    g_pcm.playback_reference_valid = true;
    g_pcm.playback_reference_silent = silent;
    g_pcm.playback_reference_gap = false;
    taskEXIT_CRITICAL();
    info->pts_us = info->first_sample_index * 1000000U / info->sample_rate_hz;
    return true;
}

#if WAVE_PLAYER_ENABLE_REC
static const int16_t audio_pcm_silence[AUDIO_PCM_CAPTURE_SAMPLES_MAX];

static void audio_pcm_capture_dispatch(const wave_player_rec_frame_t *frame, void *context)
{
    (void)context;
    struct audio_pcm_session *session = &g_pcm.sessions[AUDIO_PCM_CAPTURE];
    audio_pcm_frame_cb_t callback = NULL;
    void *callback_context = NULL;
    if (frame == NULL || frame->sample_rate_hz == 0) {
        return;
    }
    /* Admit a frame even without a data callback so that a reference-only
     * subscriber and later callbacks observe the same capture sample clock. */
    taskENTER_CRITICAL();
    uint32_t previous_sequence =
        session->capture_seen ? session->previous_capture_sequence : session->open_capture_sequence;
    uint32_t sequence_delta = frame->sequence - previous_sequence;
    bool live = audio_pcm_session_live(session) && frame->epoch == session->capture_epoch &&
                sequence_delta <= INT32_MAX && sequence_delta != 0;
    if (live) {
        ++session->callback_users;
    }
    taskEXIT_CRITICAL();
    if (!live) {
        return;
    }
    uint32_t flags = sequence_delta != 1U ? AUDIO_PCM_FRAME_DISCONTINUITY : 0;
    session->capture_sample_index = frame->first_sample_index;
    audio_pcm_frame_info_t info = {
        .sample_rate_hz = frame->sample_rate_hz,
        .samples_per_channel = frame->samples_per_channel,
        .sequence = frame->sequence,
        .epoch = g_pcm.engine_epoch,
        .flags = flags,
        .first_sample_index = session->capture_sample_index,
        .pts_us = session->capture_sample_index * 1000000U / frame->sample_rate_hz,
        .channels = frame->channels,
        .bits_per_sample = 16,
        .layout = frame->layout,
        .muted_mic_mask = frame->muted_mic_mask,
    };
    session->previous_capture_sequence = frame->sequence;
    session->capture_seen = true;
    taskENTER_CRITICAL();
    callback = session->capture_callback;
    callback_context = session->capture_context;
    if (callback != NULL) {
        ++session->capture_callback_users;
    }
    taskEXIT_CRITICAL();
    if (callback != NULL) {
        callback(frame->samples, &info, callback_context);
        taskENTER_CRITICAL();
        --session->capture_callback_users;
        taskEXIT_CRITICAL();
    }
    bool silent_reference;
    taskENTER_CRITICAL();
    /* A closing/aborted player still emits scheduled silence until its task
     * exits. Synthesize reference only after that producer is gone. */
    wave_player_pcm_engine_state_t state;
    wave_player_pcm_engine_snapshot(&state);
    silent_reference = !state.playback_running && !g_pcm.sessions[AUDIO_PCM_PLAYBACK].used;
    callback = session->playback_callback;
    callback_context = session->playback_context;
    if (silent_reference && callback != NULL) {
        ++session->playback_callback_users;
    }
    taskEXIT_CRITICAL();
    if (silent_reference && callback != NULL && frame->samples_per_channel <= AUDIO_PCM_CAPTURE_SAMPLES_MAX) {
        info.channels = 1;
        info.muted_mic_mask = 0;
        info.layout.mic_mask = 0;
        info.layout.ref_mask = 1;
        /* Silence can be shortened safely when the capture descriptor partly
         * overlaps a reference already emitted by the stopped player. */
        if (audio_pcm_reference_update(&info, true)) {
            callback(audio_pcm_silence, &info, callback_context);
        }
    }
    if (silent_reference && callback != NULL) {
        taskENTER_CRITICAL();
        --session->playback_callback_users;
        taskEXIT_CRITICAL();
    }
    audio_pcm_callback_leave(session, false, false);
}
#endif

static void audio_pcm_playback_dispatch(const int16_t *samples, uint32_t count, uint32_t rate, uint8_t channels,
                                        void *context)
{
    (void)context;
    audio_pcm_frame_info_t info = {
        .sample_rate_hz = rate,
        .samples_per_channel = count,
        .epoch = g_pcm.engine_epoch,
        .channels = channels,
        .bits_per_sample = 16,
        .layout = { .mic_mask = 0, .ref_mask = (1U << channels) - 1U },
    };
    if (!audio_pcm_reference_update(&info, false)) {
        return;
    }
    for (unsigned i = 0; i < 2; ++i) {
        struct audio_pcm_session *session = &g_pcm.sessions[i];
        audio_pcm_frame_cb_t callback = NULL;
        void *callback_context = NULL;
        if (!audio_pcm_callback_enter(session, false, &callback, &callback_context)) {
            continue;
        }
        callback(samples, &info, callback_context);
        audio_pcm_callback_leave(session, true, false);
    }
}

uint32_t audio_pcm_buffered_bytes(audio_pcm_handle_t session)
{
    uint32_t bytes = 0;
    if (!audio_pcm_lock()) {
        if (audio_pcm_session_live(session) && session->direction == AUDIO_PCM_PLAYBACK) {
            bytes = wave_player_get_buffered_bytes();
        }
        audio_pcm_unlock();
    }
    return bytes;
}

uint32_t audio_pcm_trim_buffer(audio_pcm_handle_t session, uint32_t max_bytes)
{
    uint32_t discarded = 0;
    if (!audio_pcm_lock()) {
        if (audio_pcm_session_live(session) && session->direction == AUDIO_PCM_PLAYBACK) {
            discarded = wave_player_pcm_engine_trim(max_bytes);
        }
        audio_pcm_unlock();
    }
    return discarded;
}

int audio_pcm_flush(audio_pcm_handle_t session)
{
    int result = audio_pcm_lock();
    if (result) {
        return result;
    }
    if (!audio_pcm_session_valid(session) || !session->used || session->direction != AUDIO_PCM_PLAYBACK) {
        result = -EINVAL;
    } else {
        wave_player_pcm_engine_flush();
    }
    audio_pcm_unlock();
    return result;
}

int audio_pcm_set_mic_channel_volume(uint16_t mask, uint8_t volume)
{
    int result = audio_pcm_lock();
    if (result != 0) {
        return result;
    }
    result = g_pcm.initialized ? wave_player_set_mic_channel_volume(mask, volume) : -ENODEV;
    audio_pcm_unlock();
    return result;
}

int audio_pcm_set_mic_volume(uint8_t volume)
{
    return audio_pcm_set_mic_channel_volume(g_pcm.caps.mic_mask, volume);
}

int audio_pcm_set_mic_channel_gain_db(uint16_t mask, float gain_db)
{
    int result = audio_pcm_lock();
    if (result != 0) {
        return result;
    }
    result = g_pcm.initialized ? wave_player_set_mic_channel_gain_db(mask, gain_db) : -ENODEV;
    audio_pcm_unlock();
    return result;
}

int audio_pcm_set_mic_gain_db(float gain_db)
{
    return audio_pcm_set_mic_channel_gain_db(g_pcm.caps.mic_mask, gain_db);
}

int audio_pcm_set_mic_channel_mute(uint16_t mask, bool mute)
{
    int result = audio_pcm_lock();
    if (result != 0) {
        return result;
    }
    result = g_pcm.initialized ? wave_player_set_mic_channel_mute(mask, mute) : -ENODEV;
    audio_pcm_unlock();
    return result;
}

int audio_pcm_set_mic_mute(bool mute)
{
    return audio_pcm_set_mic_channel_mute(g_pcm.caps.mic_mask, mute);
}

int audio_pcm_get_mic_channel_caps(uint8_t slot, audio_codec_channel_caps_t *caps)
{
    int result = audio_pcm_lock();
    if (result != 0) {
        return result;
    }
    result = g_pcm.initialized ? wave_player_get_mic_channel_caps(slot, caps) : -ENODEV;
    audio_pcm_unlock();
    return result;
}

int audio_pcm_get_mic_channel_state(uint8_t slot, audio_codec_channel_state_t *state)
{
    int result = audio_pcm_lock();
    if (result != 0) {
        return result;
    }
    result = g_pcm.initialized ? wave_player_get_mic_channel_state(slot, state) : -ENODEV;
    audio_pcm_unlock();
    return result;
}

bool audio_pcm_get_mic_mute(void)
{
    return wave_player_get_mic_mute();
}

int audio_pcm_set_speaker_channel_volume(uint16_t mask, uint8_t volume)
{
    int result = audio_pcm_lock();
    if (result != 0) {
        return result;
    }
    result = g_pcm.initialized ? wave_player_set_speaker_channel_volume(mask, volume) : -ENODEV;
    audio_pcm_unlock();
    return result;
}

int audio_pcm_set_speaker_volume(uint8_t volume)
{
    return audio_pcm_set_speaker_channel_volume(g_pcm.caps.speaker_mask, volume);
}

int audio_pcm_set_speaker_channel_mute(uint16_t mask, bool mute)
{
    int result = audio_pcm_lock();
    if (result != 0) {
        return result;
    }
    result = g_pcm.initialized ? wave_player_set_speaker_channel_mute(mask, mute) : -ENODEV;
    audio_pcm_unlock();
    return result;
}

int audio_pcm_set_speaker_mute(bool mute)
{
    return audio_pcm_set_speaker_channel_mute(g_pcm.caps.speaker_mask, mute);
}

int audio_pcm_get_speaker_channel_caps(uint8_t slot, audio_codec_channel_caps_t *caps)
{
    int result = audio_pcm_lock();
    if (result != 0) {
        return result;
    }
    result = g_pcm.initialized ? wave_player_get_speaker_channel_caps(slot, caps) : -ENODEV;
    audio_pcm_unlock();
    return result;
}

int audio_pcm_get_speaker_channel_state(uint8_t slot, audio_codec_channel_state_t *state)
{
    int result = audio_pcm_lock();
    if (result != 0) {
        return result;
    }
    result = g_pcm.initialized ? wave_player_get_speaker_channel_state(slot, state) : -ENODEV;
    audio_pcm_unlock();
    return result;
}

bool audio_pcm_get_speaker_mute(void)
{
    return wave_player_get_speaker_mute();
}

uint16_t audio_pcm_get_mic_mute_mask(void)
{
    return wave_player_get_mic_mute_mask();
}

int audio_pcm_get_speaker_volume(uint8_t *volume)
{
    int result = audio_pcm_lock();
    if (result != 0) {
        return result;
    }
    result = g_pcm.initialized ? wave_player_get_speaker_volume(volume) : -ENODEV;
    audio_pcm_unlock();
    return result;
}
