/* Copyright (c) 2026 Bouffalo Lab. SPDX-License-Identifier: Apache-2.0 */
#ifndef RTSP_SERVER_H
#define RTSP_SERVER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtsp_server rtsp_server_t;

/* Callbacks are nonblocking and run only from poll(). Copy one frame into
 * dst; return 1 for a frame, 0 if none, -1 to discard an invalid source frame.
 * Timestamp units are 90 kHz for JPEG and 8 kHz for mono PCMA. Advance audio
 * timestamps for captured samples even when an upstream queue drops them.
 * JPEG must be baseline 4:2:2/4:2:0, standard Huffman tables, no restart markers.
 */
typedef int (*rtsp_server_read_frame_t)(void *ctx, uint8_t *dst, size_t capacity,
                                      size_t *size, uint32_t *timestamp);
typedef struct {
    uint16_t port;             /* Zero chooses an ephemeral port (host tests). */
    const char *path;          /* Absolute path, e.g. /live; copied at open. */
    size_t max_jpeg_size;
    rtsp_server_read_frame_t read_video;
    rtsp_server_read_frame_t read_audio; /* NULL disables PCMA. */
    uint32_t (*random32)(void *ctx);     /* Required; SSRC/session randomizer. */
    void *ctx;
    /* Optional current sampling clock at now_ms (the same monotonic clock
     * passed to poll), track 0 = JPEG/90000, 1 = PCMA/8000. This must describe
     * capture time, independent of queued packets and socket write timing.
     * Without it, SR extrapolates from the first packet of each track. */
    uint32_t (*rtp_clock)(void *ctx, unsigned track, uint32_t now_ms);
    /* Video packet burst per poll, 0 = 8, maximum 32. Audio/control retain
     * priority. Bounds Wi-Fi bursts without imposing a camera frame rate. */
    unsigned video_packets_per_poll;
    /* Optional port backpressure: 0 pauses video packet preparation until
     * ready or the 200 ms frame deadline, 1 permits it. Audio/control continue.
     * Use to reserve a shared network
     * heap for incoming ACKs and audio before send() exhausts that heap. */
    int (*video_ready)(void *ctx);
    /* Optional DSCP values 0..63, zero keeps best effort. UDP tracks are
     * marked individually; a shared TCP flow uses the video marking when
     * video is configured. These are hints to the network, not guarantees. */
    uint8_t video_dscp, audio_dscp;
} rtsp_server_config_t;

typedef struct {
    uint32_t connections, disconnects, rejected_clients;
    uint32_t video_frames, video_dropped, video_invalid;
    uint32_t audio_frames, audio_dropped;
    uint32_t rtp_packets, rtp_bytes, rtcp_reports, receiver_reports;
    uint32_t send_would_block, malformed_requests;
    uint32_t send_failures;
    int32_t last_send_error; /* Platform errno; also records retryable pressure. */
    uint32_t media_expired, max_packet_wait_ms, max_video_send_ms;
    uint32_t client_connected, playing, audio_playing;
    uint16_t port;
} rtsp_server_stats_t;

/* One bounded client in this API version. Extra clients are closed without
 * displacing the owner. Allocates once at open; poll does not allocate.
 * All calls for an instance must be serialized by its owner, including CLI
 * start/stop/stats. Socket operations are nonblocking. poll should run every
 * 1..5 ms, with an unsigned monotonic millisecond clock (wrap is supported).
 * Returns 0 on success, -1 on invalid configuration/listener failure. A client
 * failure closes only that client and leaves the listener usable.
 */
int rtsp_server_open(const rtsp_server_config_t *config, size_t config_size,
                      rtsp_server_t **server);
int rtsp_server_poll(rtsp_server_t *server, uint32_t now_ms);
int rtsp_server_get_stats(const rtsp_server_t *server, rtsp_server_stats_t *stats,
                          size_t stats_size);
void rtsp_server_close(rtsp_server_t *server);

#ifdef __cplusplus
}
#endif
#endif
