#include "config.h"
#include "webrtc_peer.h"
#include "webrtc_peer_audio.h"
#include <stdlib.h>
#include <string.h>

struct webrtc_peer_audio {
    webrtc_peer_audio_config_t config;
    uint32_t ssrc, timestamp, newest_end;
    uint32_t capacity, head, history_capacity, history_write, history_count;
    uint32_t lost_run, period, phase, recover_left;
    uint32_t target_ms, pending_timestamp;
    uint16_t last_sequence, pending_sequence;
    /* recover_from holds the last emitted sample as a scalar ramp anchor,
     * not a buffered waveform tail. Zero produces a fade-in from silence. */
    int16_t last_output, recover_from;
    uint8_t have_base, primed, started, pending_restart;
    int16_t *samples, *history;
    uint8_t *valid;
#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
    webrtc_peer_audio_stats_t stats;
#endif
};

#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
#define COUNT(s, field, n) ((s)->stats.field += (uint32_t)(n))
#else
#define COUNT(s, field, n) ((void)0)
#endif

static int64_t difference(uint32_t a, uint32_t b)
{
    uint32_t delta = a - b;
    return delta <= INT32_MAX ? (int64_t)delta : (int64_t)delta - INT64_C(4294967296);
}

static uint32_t span(const webrtc_peer_audio_t *s)
{
    int64_t n = difference(s->newest_end, s->timestamp);
    return n > 0 ? (uint32_t)n : 0;
}

static int present(const webrtc_peer_audio_t *s, uint32_t index)
{
    return (s->valid[index >> 3] >> (index & 7U)) & 1U;
}

void webrtc_peer_audio_reset(webrtc_peer_audio_t *s)
{
    if (!s) return;
    s->head = s->history_write = s->history_count = 0;
    s->lost_run = s->recover_left = s->period = s->phase = 0;
    s->have_base = s->primed = s->started = s->pending_restart = 0;
    s->target_ms = s->config.min_delay_ms;
    s->last_output = s->recover_from = 0;
    memset(s->valid, 0, (s->capacity + 7U) / 8U);
#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
    uint32_t bytes = s->stats.allocated_bytes;
    memset(&s->stats, 0, sizeof(s->stats));
    s->stats.allocated_bytes = bytes;
#endif
}

int webrtc_peer_audio_open(const webrtc_peer_audio_config_t *c, webrtc_peer_audio_t **out)
{
    if (!out) return WEBRTC_PEER_ERR_INVALID_ARG;
    *out = NULL;
    if (!c || (c->sample_rate != 8000U && c->sample_rate != 16000U) ||
        c->min_delay_ms < 20U || c->max_delay_ms > 1000U ||
        c->max_delay_ms < c->min_delay_ms || c->min_delay_ms % 20U || c->max_delay_ms % 20U)
        return WEBRTC_PEER_ERR_INVALID_ARG;
    uint32_t capacity = c->sample_rate * c->max_delay_ms / 1000U;
    uint32_t history = 1;
    while (history < c->sample_rate * 40U / 1000U) history *= 2U;
    size_t bytes = sizeof(webrtc_peer_audio_t) + (capacity + history) * sizeof(int16_t) +
                   (capacity + 7U) / 8U;
    webrtc_peer_audio_t *s = calloc(1, bytes);
    if (!s) return WEBRTC_PEER_ERR_NO_MEMORY;
    s->config = *c;
    s->capacity = capacity;
    s->history_capacity = history;
    s->samples = (int16_t *)(s + 1);
    s->history = s->samples + capacity;
    s->valid = (uint8_t *)(s->history + history);
#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
    s->stats.allocated_bytes = (uint32_t)bytes;
#endif
    webrtc_peer_audio_reset(s);
    *out = s;
    return WEBRTC_PEER_OK;
}

void webrtc_peer_audio_close(webrtc_peer_audio_t *s) { free(s); }

static void rebase(webrtc_peer_audio_t *s, uint32_t ssrc, uint16_t seq, uint32_t ts)
{
    if (s->have_base) COUNT(s, resyncs, 1);
    if (s->have_base && s->ssrc != ssrc) {
        s->history_count = s->history_write = 0;
        s->lost_run = s->recover_left = s->period = 0;
        s->last_output = 0;
    }
    memset(s->valid, 0, (s->capacity + 7U) / 8U);
    s->head = 0;
    s->ssrc = ssrc;
    s->timestamp = s->newest_end = ts;
    s->last_sequence = seq;
    s->have_base = 1;
    s->primed = s->pending_restart = 0;
}

int webrtc_peer_audio_push(webrtc_peer_audio_t *s, uint32_t ssrc, uint16_t seq,
                           uint32_t ts, const int16_t *pcm, size_t count)
{
    if (!s || !pcm || !count || count > s->capacity) return WEBRTC_PEER_ERR_INVALID_ARG;
    if (!s->have_base || ssrc != s->ssrc) rebase(s, ssrc, seq, ts);
    int64_t offset = difference(ts, s->timestamp);
    if (offset < -(int64_t)s->capacity) {
        /* A lone late packet cannot rewind playout. Confirm a same-SSRC clock
         * restart using two packets with consecutive sequence and time. */
        uint16_t forward = (uint16_t)(seq - s->last_sequence);
        if (!forward || (forward >= 32768U &&
            (uint16_t)(s->last_sequence - seq) < 3000U)) {
            COUNT(s, late_samples, count);
            return WEBRTC_PEER_OK;
        }
        int64_t dt = difference(ts, s->pending_timestamp);
        if (s->pending_restart && seq == (uint16_t)(s->pending_sequence + 1U) &&
            dt > 0 && dt <= (int64_t)s->config.sample_rate) {
            rebase(s, ssrc, seq, ts);
            offset = 0;
        } else {
            s->pending_restart = 1;
            s->pending_sequence = seq;
            s->pending_timestamp = ts;
            COUNT(s, late_samples, count);
            return WEBRTC_PEER_OK;
        }
    } else {
        s->pending_restart = 0;
    }
    if (offset < 0 && !s->started && offset >= -(int64_t)s->capacity &&
        (int64_t)span(s) - offset <= s->capacity) {
        uint32_t prepend = (uint32_t)-offset;
        s->head = (s->head + s->capacity - prepend) % s->capacity;
        s->timestamp = ts;
        offset = 0;
    }
    if (offset + (int64_t)count <= 0) {
        COUNT(s, late_samples, count);
        return WEBRTC_PEER_OK;
    }
    if (offset < 0) {
        uint32_t old = (uint32_t)-offset;
        COUNT(s, late_samples, old);
        pcm += old;
        count -= old;
        ts += old;
        offset = 0;
    }
    if ((uint64_t)offset + count > s->capacity) {
        uint64_t excess = (uint64_t)offset + count - s->capacity;
        if (excess >= s->capacity) {
            /* A large discontinuity expires the old timeline. */
            rebase(s, ssrc, seq, ts);
            offset = 0;
        } else {
            /* A catch-up burst may exceed the right edge by a small amount.
             * Shed only the oldest prefix, preserving the rest of the queued
             * speech. A whole-buffer reset here amplifies short bursts. */
            for (uint32_t i = 0; i < (uint32_t)excess; ++i) {
                if (present(s, s->head)) COUNT(s, window_discard_samples, 1);
                s->valid[s->head >> 3] &= (uint8_t)~(1U << (s->head & 7U));
                if (++s->head == s->capacity) s->head = 0;
            }
            s->timestamp += (uint32_t)excess;
            offset -= (int64_t)excess;
            s->recover_left = s->config.sample_rate * 5U / 1000U;
            s->recover_from = s->last_output;
        }
    }
    uint32_t index = (s->head + (uint32_t)offset) % s->capacity;
    for (size_t i = 0; i < count; ++i) {
        if (present(s, index)) COUNT(s, duplicate_samples, 1);
        else {
            s->samples[index] = pcm[i];
            s->valid[index >> 3] |= (uint8_t)(1U << (index & 7U));
        }
        if (++index == s->capacity) index = 0;
    }
    uint32_t end = ts + (uint32_t)count;
    if (difference(end, s->newest_end) > 0) s->newest_end = end;
    if ((uint16_t)(seq - s->last_sequence) < 32768U) s->last_sequence = seq;
    return WEBRTC_PEER_OK;
}

static int16_t history_sample(const webrtc_peer_audio_t *s, uint32_t back)
{
    return s->history[(s->history_write - back) & (s->history_capacity - 1U)];
}

static void find_period(webrtc_peer_audio_t *s)
{
    uint32_t frame = s->config.sample_rate / 50U;
    uint32_t low = s->config.sample_rate / 400U;
    uint32_t high = s->config.sample_rate / 66U;
    s->period = 0;
    s->phase = 0;
    if (s->history_count < frame + low) return;
    if (high > s->history_count - frame) high = s->history_count - frame;
    uint32_t best = UINT32_MAX;
    for (uint32_t lag = low; lag <= high; ++lag) {
        uint32_t score = 0;
        for (uint32_t i = 1; i <= frame; ++i) {
            int32_t d = (int32_t)history_sample(s, i) - history_sample(s, i + lag);
            score += (uint32_t)(d < 0 ? -d : d);
        }
        if (score < best) { best = score; s->period = lag; }
    }
}

static int16_t conceal(webrtc_peer_audio_t *s)
{
    if (!s->lost_run) find_period(s);
    uint32_t fade = s->config.sample_rate * 60U / 1000U;
    int32_t out = 0;
    if (s->period && s->lost_run < fade) {
        int32_t sample = history_sample(s, s->period - s->phase);
        out = sample * (int32_t)(fade - s->lost_run) / (int32_t)fade;
        if (++s->phase == s->period) s->phase = 0;
    }
    if (s->lost_run < UINT32_MAX) s->lost_run++;
    return (int16_t)out;
}

int webrtc_peer_audio_pull(webrtc_peer_audio_t *s, int16_t *pcm, size_t capacity)
{
    if (!s || !pcm || capacity < s->config.sample_rate / 50U)
        return WEBRTC_PEER_ERR_INVALID_ARG;
    uint32_t frame = s->config.sample_rate / 50U;
    if (!s->have_base) return 0;
    if (!s->primed && span(s) >= s->config.sample_rate * s->target_ms / 1000U)
        s->primed = 1;
    if (!s->primed) {
        if (!s->started) return 0;
        for (uint32_t i = 0; i < frame; ++i) pcm[i] = s->last_output = conceal(s);
        COUNT(s, concealed_samples, frame);
        COUNT(s, rebuffer_samples, frame);
        return (int)frame;
    }
    s->started = 1;
    for (uint32_t i = 0; i < frame; ++i) {
        int16_t value;
        if (present(s, s->head)) {
            int16_t real = s->samples[s->head];
            s->valid[s->head >> 3] &= (uint8_t)~(1U << (s->head & 7U));
            if (s->lost_run) {
                s->recover_left = s->config.sample_rate * 5U / 1000U;
                s->recover_from = s->last_output;
                s->lost_run = 0;
            }
            value = real;
            if (s->recover_left) {
                uint32_t total = s->config.sample_rate * 5U / 1000U;
                value = (int16_t)(((int32_t)s->recover_from * (int32_t)s->recover_left +
                       (int32_t)real * (int32_t)(total - s->recover_left)) / (int32_t)total);
                s->recover_left--;
            }
            s->history[s->history_write++ & (s->history_capacity - 1U)] = real;
            if (s->history_count < s->history_capacity) s->history_count++;
            COUNT(s, real_samples, 1);
        } else {
            s->recover_left = 0;
            value = conceal(s);
            COUNT(s, concealed_samples, 1);
        }
        pcm[i] = s->last_output = value;
        if (++s->head == s->capacity) s->head = 0;
        s->timestamp++;
    }
    if (s->lost_run >= frame * 3U && !span(s)) {
        /* Re-prime after prolonged starvation; one-frame increases bound the
         * adaptation and never change the hardware sample rate. */
        if (s->target_ms < s->config.max_delay_ms) s->target_ms += 20U;
        s->primed = 0;
    }
    return (int)frame;
}

int webrtc_peer_audio_get_stats(const webrtc_peer_audio_t *s,
                                webrtc_peer_audio_stats_t *out, size_t size)
{
#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
    if (!s || !out || size < sizeof(*out)) return WEBRTC_PEER_ERR_INVALID_ARG;
    *out = s->stats;
    out->buffered_samples = span(s);
    out->target_delay_ms = s->target_ms;
    return WEBRTC_PEER_OK;
#else
    (void)s; (void)out; (void)size;
    return WEBRTC_PEER_ERR_UNSUPPORTED;
#endif
}
