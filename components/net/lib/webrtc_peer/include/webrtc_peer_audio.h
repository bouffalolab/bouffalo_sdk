#ifndef WEBRTC_PEER_AUDIO_H
#define WEBRTC_PEER_AUDIO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct webrtc_peer_audio webrtc_peer_audio_t;
typedef struct {
    uint32_t sample_rate;       /* Mono PCM, 8000 or 16000 Hz. */
    uint16_t min_delay_ms;      /* Multiples of 20 ms. */
    uint16_t max_delay_ms;      /* Bounds the timestamp window, <= 1000 ms. */
} webrtc_peer_audio_config_t;

/* One owner serializes these calls. Creation allocates one bounded block;
 * there are no allocations, locks, clock reads or logs in push/pull.
 * RTP timestamps must be in PCM sample_rate units, with one tick per sample.
 * This does not decode a codec, negotiate FEC, or replace clock-drift resampling. */
int webrtc_peer_audio_open(const webrtc_peer_audio_config_t *config,
                           webrtc_peer_audio_t **audio);
void webrtc_peer_audio_close(webrtc_peer_audio_t *audio);
void webrtc_peer_audio_reset(webrtc_peer_audio_t *audio);
int webrtc_peer_audio_push(webrtc_peer_audio_t *audio, uint32_t ssrc,
                           uint16_t sequence, uint32_t timestamp,
                           const int16_t *pcm, size_t samples);
/* Pull exactly 20 ms, driven by hardware consumption. Returns a negative error
 * for invalid arguments. Before playback first starts, or after reset(),
 * returns zero without writing pcm until enough audio has accumulated.
 * Once started, valid pulls return sample_rate/50 samples even while re-priming
 * after starvation or a timeline change, until reset(). Re-priming outputs PLC
 * without advancing the queued media timeline so the hardware stays supplied.
 * Missing samples use pitch repetition with a 60 ms fade, followed by silence.
 * A 5 ms recovery blend suppresses discontinuities. This is lightweight PLC,
 * not a bit-exact implementation of ITU G.711 Appendix I or WebRTC NetEQ. */
int webrtc_peer_audio_pull(webrtc_peer_audio_t *audio, int16_t *pcm, size_t capacity);

/* Public regardless of consumer macros. Without library diagnostics,
 * get_stats returns ERR_UNSUPPORTED without inspecting arguments or writing. */
typedef struct {
    uint32_t real_samples, concealed_samples, rebuffer_samples;
    uint32_t late_samples, duplicate_samples, resyncs, window_discard_samples;
    uint32_t buffered_samples, target_delay_ms, allocated_bytes;
} webrtc_peer_audio_stats_t;
int webrtc_peer_audio_get_stats(const webrtc_peer_audio_t *audio,
                                webrtc_peer_audio_stats_t *stats, size_t size);

#ifdef __cplusplus
}
#endif
#endif
