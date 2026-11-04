/* Copyright (c) 2026 Bouffalo Lab. SPDX-License-Identifier: Apache-2.0 */
/* Include production to exercise parser/transport error paths without adding
 * test switches or exporting socket internals in the SDK API. */
#include <sys/socket.h>
#include <errno.h>
static int force_send_error;
static ssize_t test_send(int fd, const void *data, size_t size, int flags)
{
    if (force_send_error) { errno = force_send_error; return -1; }
    return send(fd, data, size, flags);
}
#define send test_send
#include "../src/rtsp_server.c"
#undef send
#include <assert.h>

static uint32_t rng(void *ctx) { (void)ctx; return 0x12345678; }
static int empty(void *ctx, uint8_t *dst, size_t cap, size_t *n, uint32_t *ts)
{ (void)ctx; (void)dst; (void)cap; (void)n; (void)ts; return 0; }
static uint32_t clock_now(void *ctx, unsigned track, uint32_t now)
{ (void)ctx; return now * (track ? 8U : 90U) + 123U; }
static uint32_t get32(const uint8_t *p)
{ return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
typedef struct { uint8_t *data; size_t size; } fixture_t;
static int ready;
static int video_ready(void *ctx) { (void)ctx; return ready; }
static int fixture(void *ctx, uint8_t *dst, size_t cap, size_t *n, uint32_t *ts)
{
    fixture_t *f = ctx; assert(f->size <= cap);
    memcpy(dst, f->data, f->size); *n = f->size; *ts = 9000U * 90U; return 1;
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    FILE *f = fopen(argv[1], "rb"); assert(f);
    uint8_t *data = malloc(1000000); assert(data);
    size_t size = fread(data, 1, 1000000, f); fclose(f);
    rtsp_server_jpeg_t j;
    assert(!rtsp_server_jpeg_parse(data, size, &j));
    uint8_t *scan = malloc(j.scan_size); assert(scan);
    size_t offset = 0;
    while (offset < j.scan_size) {
        uint8_t packet[1190]; memset(packet, 0xa5, sizeof(packet)); size_t n = 0;
        size_t bytes = rtsp_server_jpeg_payload(&j, offset, packet, 1188, &n);
        assert(bytes > 8 && bytes <= 1188 && n && packet[1188] == 0xa5);
        assert(((size_t)packet[1] << 16 | (size_t)packet[2] << 8 | packet[3]) == offset);
        size_t hdr = offset ? 8 : 140;
        assert(bytes == hdr + n && n <= j.scan_size - offset);
        memcpy(scan + offset, packet + hdr, n); offset += n;
    }
    assert(!memcmp(scan, j.scan, j.scan_size));
    assert(!rtsp_server_jpeg_payload(&j, j.scan_size, scan, j.scan_size, &offset));
    /* Every truncated header/scan must fail without reading outside input. */
    for (size_t n = 0; n < size - 1; n++) assert(rtsp_server_jpeg_parse(data, n, &j) == -1);
    unsigned state = 7;
    for (unsigned run = 0; run < 3000; run++) {
        uint8_t malformed[512];
        for (size_t k = 0; k < sizeof(malformed); k++) {
            state = state * 1664525U + 1013904223U; malformed[k] = (uint8_t)(state >> 24);
        }
        (void)rtsp_server_jpeg_parse(malformed, sizeof(malformed), &j);
    }
    rtsp_server_config_t cfg = {.port = 0, .path = "/live", .max_jpeg_size = 200000,
        .read_video = empty, .random32 = rng};
    rtsp_server_t *s = NULL; assert(!rtsp_server_open(&cfg, sizeof(cfg), &s));
    /* Precise channels, bounded integer parsing and rejected mixed/overflow
     * transport parameters; exercise the original char-width sscanf hazard. */
    char ok[] = "RTP/AVP/TCP;unicast;interleaved=254-255";
    assert(setup_track(s, 0, ok) == 200);
    assert(s->tracks[0].channel[0] == 254 && s->tracks[0].channel[1] == 255);
    char bad[] = "RTP/AVP/TCP;unicast;interleaved=256-257";
    assert(setup_track(s, 1, bad) == 461);
    char overlap[] = "RTP/AVP/TCP;unicast;interleaved=255-0";
    assert(setup_track(s, 1, overlap) == 461);
    unsigned n; assert(parse_number("42949672960", 0x7fffffffU, &n));
    assert(parse_number("+12", 100, &n) && parse_number("12abc", 100, &n));
    s->now = UINT32_MAX - 2U; s->last_poll = s->now; s->have_clock = 1;
    assert(!rtsp_server_poll(s, 5)); assert(s->elapsed_ms == 8);
    rtsp_server_close(s);
    /* Listener stop/start frees its port and all instance allocations. */
    for (unsigned i = 0; i < 20; i++) { assert(!rtsp_server_open(&cfg, sizeof(cfg), &s)); rtsp_server_close(s); }
    assert(!rtsp_server_open(&cfg, sizeof(cfg), &s));
    rtsp_server_config_t conflict = cfg; conflict.port = s->stats.port;
    rtsp_server_t *failed = (void *)1;
    assert(rtsp_server_open(&conflict, sizeof(conflict), &failed) == -1 && !failed);
    int pair[2]; assert(!socketpair(AF_UNIX, SOCK_STREAM, 0, pair));
    assert(!nonblocking(pair[0]) && !nonblocking(pair[1]));
    int small = 1024;
    assert(!setsockopt(pair[0], SOL_SOCKET, SO_SNDBUF, &small, sizeof(small)));
    uint8_t padding[512] = {0}; size_t prefilled = 0;
    int sent;
    while ((sent = (int)send(pair[0], padding, sizeof(padding), 0)) > 0) prefilled += (size_t)sent;
    assert(prefilled && retryable());
    s->client = pair[0]; s->now = 10;
    char body[3500]; memset(body, 'x', sizeof(body) - 1); body[sizeof(body) - 1] = 0;
    response(s, 200, 42, NULL, body);
    size_t expected_size = s->output_size;
    uint8_t expected[INPUT_SIZE]; memcpy(expected, s->output, expected_size);
    force_send_error = ENOMEM;
    assert(flush_output(s) == 0 && s->client == pair[0] && !s->output_offset);
    assert(s->stats.last_send_error == ENOMEM && !s->stats.send_failures);
    force_send_error = 0;
    assert(flush_output(s) == 0 && !s->output_offset && s->output_size == expected_size);
    uint8_t received[16384]; size_t total = 0;
    for (unsigned i = 0; i < 1000 && total < prefilled + expected_size; i++) {
        s->now = 20 + i;
        int got = (int)recv(pair[1], received + total, 256, 0);
        if (got > 0) total += (size_t)got;
        else assert(got < 0 && retryable());
        assert(flush_output(s) >= 0);
    }
    assert(total == prefilled + expected_size && !s->output_size);
    assert(s->progress_ms >= 20);
    assert(!memcmp(received + prefilled, expected, expected_size));
    /* Congested media can expire at packet boundaries, including a final
     * JPEG fragment (scan_size is already zero); control cannot be dropped. */
    s->tracks[0].tcp = 1; s->now = 300; s->frame_ms = 10;
    packet_begin(s, 0, 100, 0); s->jpeg.scan_size = 0;
    assert(flush_output(s) == 1 && !s->output_size && s->stats.video_dropped == 1);
    assert(s->stats.media_expired == 1 && !s->stats.video_frames);
    packet_begin(s, 0, 100, 0); s->output_offset = 2;
    force_send_error = EAGAIN;
    assert(!flush_output(s) && s->output_offset == 2 && s->output_size == 104);
    assert(s->stats.media_expired == 1); /* Partial TCP interleaving stays intact. */
    s->tracks[0].tcp = 0; s->output_offset = 0;
    assert(flush_output(s) == 1 && s->stats.media_expired == 2);
    force_send_error = 0;
    s->tracks[0].tcp = 1; s->tracks[0].setup = 1; s->tracks[0].packets = 1;
    s->now = 1000; start_clock(&s->tracks[0], 90000, 1000);
    start_clock(&s->tracks[0], 130000, 1400); /* Queue jitter must not re-anchor. */
    s->now = 2000; assert(sender_report(s, 0));
    assert(get32(s->output + 4 + 16) == 180000);
    s->output_size = 0; s->config.rtp_clock = clock_now;
    s->now = 8000; assert(sender_report(s, 0));
    assert(get32(s->output + 4 + 16) == 720123);
    s->output_size = 0;
    /* Limit a large JPEG burst, rather than limiting camera frames/sec. */
    fixture_t source = {data, size};
    s->config.read_video = fixture; s->config.ctx = &source;
    s->config.video_packets_per_poll = 1;
    s->playing = 1; s->tracks[0].report_ms = 9000;
    uint16_t seq_before = s->tracks[0].sequence;
    assert(!rtsp_server_poll(s, 9000));
    assert(s->tracks[0].sequence == (uint16_t)(seq_before + 1U));
    size_t pending_offset = s->frame_offset;
    s->config.video_ready = video_ready; ready = 0;
    assert(!video_packet(s) && s->frame_offset == pending_offset);
    assert(s->jpeg.scan_size); /* Brief pressure preserves sent fragments. */
    ready = 1;
    assert(video_packet(s) && s->frame_offset > pending_offset);
    s->output_size = 0;
    ready = 0; s->now += FRAME_TIMEOUT_MS + 1U;
    uint32_t dropped = s->stats.video_dropped;
    assert(!video_packet(s) && !s->jpeg.scan_size);
    assert(s->stats.video_dropped == dropped + 1U);
    ready = 1;
    s->playing = 0; s->config.read_video = empty;
    /* A pending TCP write may not be dropped/replaced with another packet.
     * Timeout closes the client but retains the listening server. */
    s->output_size = 100; s->output_offset = 1;
    s->write_ms = 10; s->activity_ms = 10;
    s->activity_ms = 9000; s->write_ms = 10; s->progress_ms = 9000;
    force_send_error = EAGAIN;
    assert(!rtsp_server_poll(s, 13000) && s->client == pair[0]);
    assert(!rtsp_server_poll(s, 14001) && s->client == -1 && s->listener >= 0);
    force_send_error = 0;
    close(pair[1]);
    char udp[] = "RTP/AVP;unicast;client_port=5000-5001";
    s->config.video_dscp = 34;
    assert(setup_track(s, 0, udp) == 200);
    int tos = 0; socklen_t tos_size = sizeof(tos);
    assert(!getsockopt(s->tracks[0].fd[0], IPPROTO_IP, IP_TOS, &tos, &tos_size));
    assert(tos == (34 << 2));
    /* The same bounded packet budget applies to UDP. */
    assert(!socketpair(AF_UNIX, SOCK_STREAM, 0, pair));
    assert(!nonblocking(pair[0]));
    s->client = pair[0]; s->playing = 1; s->activity_ms = 15000;
    s->peer.sin_family = AF_INET; s->peer.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    s->config.read_video = fixture; s->config.ctx = &source;
    s->config.video_packets_per_poll = 1;
    seq_before = s->tracks[0].sequence;
    assert(!rtsp_server_poll(s, 15000));
    assert(s->tracks[0].sequence == (uint16_t)(seq_before + 1U));
    close(pair[1]);
    int udp_fd = s->tracks[0].fd[0], listener_fd = s->listener;
    rtsp_server_close(s);
    assert(fcntl(udp_fd, F_GETFD) == -1 && errno == EBADF);
    assert(fcntl(listener_fd, F_GETFD) == -1 && errno == EBADF);
    free(scan); free(data);
    puts("RTSP: JPEG tails, truncation/fuzz, transports, tick wrap, partial writes, timeouts and lifecycle passed");
}
