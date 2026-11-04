/* Copyright (c) 2026 Bouffalo Lab. SPDX-License-Identifier: Apache-2.0 */
#include "rtsp_server.h"
#include "rtsp_server_jpeg.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#ifdef RTSP_SERVER_POSIX
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#else
#include <lwip/sockets.h>
#include <lwip/inet.h>
#endif
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#define INPUT_SIZE 4096U
#define PACKET_SIZE 1200U
#define FRAME_TIMEOUT_MS 200U
#define WRITE_TIMEOUT_MS 5000U
#define REQUEST_TIMEOUT_MS 5000U
#define IDLE_TIMEOUT_MS 60000U

typedef struct {
    int setup, tcp, have_report, have_timestamp, fd[2];
    uint8_t channel[2];
    uint16_t client_port[2], server_port[2], sequence;
    uint32_t ssrc, packets, octets, timestamp, timestamp_ms, report_ms;
} rtsp_track_t;

struct rtsp_server {
    rtsp_server_config_t config;
    rtsp_server_stats_t stats;
    int listener, client, playing, close_after_response;
    char path[128], session[9];
    struct sockaddr_in peer;
    rtsp_track_t tracks[2];
    char input[INPUT_SIZE + 1];
    size_t input_size, skip_interleaved;
    uint32_t now, last_poll, activity_ms, request_ms, write_ms, progress_ms;
    uint64_t elapsed_ms;
    int have_clock;
    uint8_t output[INPUT_SIZE];
    size_t output_size, output_offset;
    int output_track, output_rtcp;
    uint8_t *frame;
    rtsp_server_jpeg_t jpeg;
    size_t frame_offset;
    uint32_t frame_timestamp, frame_ms;
};

static void put16(uint8_t *p, uint32_t n) { p[0] = (uint8_t)(n >> 8); p[1] = (uint8_t)n; }
static void put32(uint8_t *p, uint32_t n)
{ p[0] = (uint8_t)(n >> 24); p[1] = (uint8_t)(n >> 16); p[2] = (uint8_t)(n >> 8); p[3] = (uint8_t)n; }
static unsigned get16(const uint8_t *p) { return (unsigned)p[0] * 256U + p[1]; }
/* lwIP maps transient pbuf/netbuf exhaustion to ENOMEM, including UDP send.
 * Preserve the exact pending packet until the pool recovers or its deadline. */
static int retryable(void)
{ return errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == ENOBUFS || errno == ENOMEM; }
static int nonblocking(int fd)
{
    int one = 1;
    return ioctl(fd, FIONBIO, &one);
}

static void close_client(rtsp_server_t *s)
{
    for (unsigned i = 0; i < 2; i++) {
        for (unsigned j = 0; j < 2; j++) {
            if (s->tracks[i].fd[j] >= 0) close(s->tracks[i].fd[j]);
        }
        memset(&s->tracks[i], 0, sizeof(s->tracks[i]));
        s->tracks[i].fd[0] = s->tracks[i].fd[1] = -1;
    }
    if (s->client >= 0) { close(s->client); s->stats.disconnects++; }
    s->client = -1; s->playing = 0; s->input_size = s->skip_interleaved = 0;
    s->output_size = s->output_offset = 0; s->jpeg.scan_size = 0;
    s->close_after_response = 0;
}

static int parse_number(const char *p, unsigned limit, unsigned *value)
{
    unsigned n = 0;
    if (*p < '0' || *p > '9') return -1;
    do {
        unsigned digit = (unsigned)(*p++ - '0');
        if (n > (limit - digit) / 10U) return -1;
        n = n * 10U + digit;
    } while (*p >= '0' && *p <= '9');
    while (*p == ' ' || *p == '\t') p++;
    if (*p) return -1;
    *value = n; return 0;
}

static const char *reason(unsigned status)
{
    switch (status) {
    case 200: return "OK";
    case 400: return "Bad Request";
    case 404: return "Not Found";
    case 454: return "Session Not Found";
    case 455: return "Method Not Valid in This State";
    case 461: return "Unsupported Transport";
    case 500: return "Internal Server Error";
    case 501: return "Not Implemented";
    default: return "Bad Request";
    }
}

static void response(rtsp_server_t *s, unsigned status, unsigned cseq,
                     const char *headers, const char *body)
{
    size_t body_size = body ? strlen(body) : 0;
    int n = snprintf((char *)s->output, sizeof(s->output),
                     "RTSP/1.0 %u %s\r\nCSeq: %u\r\nServer: Bouffalo-RTSP/1.0\r\n"
                     "%sContent-Length: %u\r\n\r\n%s", status, reason(status), cseq,
                     headers ? headers : "", (unsigned)body_size, body ? body : "");
    if (n < 0 || (size_t)n >= sizeof(s->output)) { close_client(s); return; }
    s->output_offset = 0; s->output_size = (size_t)n;
    s->output_track = -1; s->output_rtcp = 0; s->write_ms = s->progress_ms = s->now;
}

static void traffic_class(int fd, unsigned dscp)
{
    int tos = (int)(dscp << 2);
    (void)setsockopt(fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
}

static int udp_socket(uint16_t *port, unsigned dscp)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return -1;
    traffic_class(fd, dscp);
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local)); local.sin_family = AF_INET;
    if (nonblocking(fd) || bind(fd, (struct sockaddr *)&local, sizeof(local))) {
        close(fd); return -1;
    }
    socklen_t n = sizeof(local);
    if (getsockname(fd, (struct sockaddr *)&local, &n)) { close(fd); return -1; }
    *port = ntohs(local.sin_port); return fd;
}

static int parse_pair(const char *text, unsigned limit, unsigned *a, unsigned *b)
{
    char first[12], second[12]; int used = 0;
    if (sscanf(text, "%11[0-9]-%11[0-9]%n", first, second, &used) != 2 ||
        text[used] || parse_number(first, limit, a) || parse_number(second, limit, b) || *a == *b) return -1;
    return 0;
}

static int setup_track(rtsp_server_t *s, unsigned index, char *transport)
{
    rtsp_track_t *t = &s->tracks[index];
    char *save = NULL, *token = strtok_r(transport, ";", &save);
    if (!token || t->setup || strchr(token, ',')) return 461;
    int tcp;
    if (!strcasecmp(token, "RTP/AVP/TCP")) tcp = 1;
    else if (!strcasecmp(token, "RTP/AVP") || !strcasecmp(token, "RTP/AVP/UDP")) tcp = 0;
    else return 461;
    unsigned a = 0, b = 0; int pair = 0, unicast = 0;
    while ((token = strtok_r(NULL, ";", &save))) {
        while (*token == ' ' || *token == '\t') token++;
        if (!strcasecmp(token, "unicast")) unicast = 1;
        else if (!strncasecmp(token, "interleaved=", 12) && tcp) {
            if (pair || parse_pair(token + 12, 255, &a, &b)) return 461;
            pair = 1;
        } else if (!strncasecmp(token, "client_port=", 12) && !tcp) {
            if (pair || parse_pair(token + 12, 65535, &a, &b) || !a || !b) return 461;
            pair = 1;
        } else if (strcasecmp(token, "mode=play") && strcasecmp(token, "mode=\"PLAY\"")) return 461;
    }
    if (!pair || !unicast) return 461;
    rtsp_track_t *other = &s->tracks[1U - index];
    if (tcp && other->setup && other->tcp &&
        (a == other->channel[0] || a == other->channel[1] ||
         b == other->channel[0] || b == other->channel[1])) return 461;
    if (!tcp) {
        unsigned dscp = index ? s->config.audio_dscp : s->config.video_dscp;
        t->fd[0] = udp_socket(&t->server_port[0], dscp);
        t->fd[1] = udp_socket(&t->server_port[1], dscp);
        if (t->fd[0] < 0 || t->fd[1] < 0) {
            for (unsigned i = 0; i < 2; i++) { if (t->fd[i] >= 0) close(t->fd[i]); t->fd[i] = -1; }
            return 500;
        }
    }
    t->tcp = tcp; t->channel[0] = (uint8_t)a; t->channel[1] = (uint8_t)b;
    t->client_port[0] = (uint16_t)a; t->client_port[1] = (uint16_t)b;
    t->sequence = (uint16_t)s->config.random32(s->config.ctx);
    t->ssrc = s->config.random32(s->config.ctx);
    t->setup = 1; t->report_ms = s->now;
    if (tcp) traffic_class(s->client, s->tracks[0].setup && s->tracks[0].tcp ?
                           s->config.video_dscp : s->config.audio_dscp);
    return 200;
}

/* Parse a complete bounded request in place. The accumulator handles TCP
 * fragmentation, pipelining and interleaved receiver reports independently. */
static void handle_request(rtsp_server_t *s, char *request, size_t header_size)
{
    char method[24], uri[256], version[16]; int consumed = 0;
    unsigned cseq = 0, have_cseq = 0;
    char *transport = NULL, *session = NULL, *save = NULL;
    char *line = strtok_r(request, "\r\n", &save);
    if (!line || sscanf(line, "%23s %255s %15s%n", method, uri, version, &consumed) != 3 ||
        line[consumed] || strcmp(version, "RTSP/1.0")) goto malformed;
    while ((line = strtok_r(NULL, "\r\n", &save))) {
        if ((size_t)(line - request) >= header_size) break;
        char *colon = strchr(line, ':');
        if (!colon) goto malformed;
        *colon++ = 0; while (*colon == ' ' || *colon == '\t') colon++;
        if (!strcasecmp(line, "CSeq")) {
            if (have_cseq || parse_number(colon, 0x7fffffffU, &cseq)) goto malformed;
            have_cseq = 1;
        } else if (!strcasecmp(line, "Transport")) {
            if (transport) goto malformed;
            transport = colon;
        } else if (!strcasecmp(line, "Session")) {
            if (session) goto malformed;
            session = colon;
            char *end = strpbrk(session, "; \t"); if (end) *end = 0;
        }
    }
    if (!have_cseq) goto malformed;
    if (session && strcmp(session, s->session)) { response(s, 454, cseq, NULL, NULL); return; }
    const char *path = uri;
    if (!strncmp(uri, "rtsp://", 7)) {
        path = strchr(uri + 7, '/');
        if (!path) path = "/";
    }
    unsigned track = 2;
    size_t plen = strlen(s->path);
    if (!strncmp(path, s->path, plen)) {
        if (!strcmp(path + plen, "/track0")) track = 0;
        else if (!strcmp(path + plen, "/track1") && s->config.read_audio) track = 1;
    }
    int aggregate = !strcmp(path, s->path) ||
        (!strncmp(path, s->path, plen) && !strcmp(path + plen, "/"));
    if (!strcmp(method, "OPTIONS")) {
        response(s, 200, cseq, "Public: OPTIONS, DESCRIBE, SETUP, PLAY, PAUSE, GET_PARAMETER, TEARDOWN\r\n", NULL);
    } else if (!aggregate && track == 2) {
        response(s, 404, cseq, NULL, NULL);
    } else if (!strcmp(method, "DESCRIBE")) {
        struct sockaddr_in local; socklen_t len = sizeof(local);
        if (getsockname(s->client, (struct sockaddr *)&local, &len)) { close_client(s); return; }
        char ip[16], headers[256], sdp[1024];
        if (!inet_ntop(AF_INET, &local.sin_addr, ip, sizeof(ip))) { close_client(s); return; }
        snprintf(headers, sizeof(headers), "Content-Type: application/sdp\r\nContent-Base: rtsp://%s:%u%s/\r\n",
                 ip, s->stats.port, s->path);
        snprintf(sdp, sizeof(sdp), "v=0\r\no=- 0 0 IN IP4 %s\r\ns=Bouffalo RTSP\r\n"
                 "c=IN IP4 %s\r\nt=0 0\r\na=control:*\r\n"
                 "m=video 0 RTP/AVP 26\r\na=rtpmap:26 JPEG/90000\r\na=control:track0\r\n%s", ip, ip,
                 s->config.read_audio ? "m=audio 0 RTP/AVP 8\r\na=rtpmap:8 PCMA/8000/1\r\na=ptime:20\r\na=control:track1\r\n" : "");
        response(s, 200, cseq, headers, sdp);
    } else if (!strcmp(method, "SETUP")) {
        if (track == 2 || !transport) { response(s, 461, cseq, NULL, NULL); return; }
        if (s->playing) { response(s, 455, cseq, NULL, NULL); return; }
        if ((s->tracks[0].setup || s->tracks[1].setup) && !session) { response(s, 454, cseq, NULL, NULL); return; }
        unsigned status = (unsigned)setup_track(s, track, transport);
        char headers[320] = "";
        if (status == 200) {
            rtsp_track_t *t = &s->tracks[track];
            if (t->tcp) snprintf(headers, sizeof(headers), "Session: %s;timeout=60\r\n"
                "Transport: RTP/AVP/TCP;unicast;interleaved=%u-%u;ssrc=%08lx\r\n",
                s->session, t->channel[0], t->channel[1], (unsigned long)t->ssrc);
            else snprintf(headers, sizeof(headers), "Session: %s;timeout=60\r\n"
                "Transport: RTP/AVP;unicast;client_port=%u-%u;server_port=%u-%u;ssrc=%08lx\r\n",
                s->session, t->client_port[0], t->client_port[1],
                t->server_port[0], t->server_port[1], (unsigned long)t->ssrc);
        }
        response(s, status, cseq, headers, NULL);
    } else if (!strcmp(method, "PLAY") || !strcmp(method, "PAUSE") ||
               !strcmp(method, "TEARDOWN") || !strcmp(method, "GET_PARAMETER")) {
        if (!session || (!s->tracks[0].setup && !s->tracks[1].setup)) { response(s, 454, cseq, NULL, NULL); return; }
        if (!strcmp(method, "PLAY")) s->playing = 1;
        else if (!strcmp(method, "PAUSE")) { s->playing = 0; s->jpeg.scan_size = 0; }
        else if (!strcmp(method, "TEARDOWN")) { s->playing = 0; s->close_after_response = 1; }
        char headers[64]; snprintf(headers, sizeof(headers), "Session: %s\r\n", s->session);
        response(s, 200, cseq, headers, NULL);
    } else response(s, 501, cseq, NULL, NULL);
    return;
malformed:
    s->stats.malformed_requests++;
    response(s, 400, cseq, NULL, NULL); s->close_after_response = 1;
}

static void receiver_report(rtsp_server_t *s, const uint8_t *data, size_t size)
{
    while (size >= 4) {
        size_t n = ((size_t)get16(data + 2) + 1U) * 4U;
        if ((data[0] >> 6) != 2 || n < 4 || n > size) return;
        if (data[1] == 201 && n >= 8U + 24U * (data[0] & 31U)) s->stats.receiver_reports++;
        data += n; size -= n;
    }
}

static void read_control(rtsp_server_t *s)
{
    if (s->input_size == INPUT_SIZE) return;
    int n = (int)recv(s->client, s->input + s->input_size, INPUT_SIZE - s->input_size, 0);
    if (n > 0) {
        if (!s->input_size && !s->skip_interleaved) s->request_ms = s->now;
        s->input_size += (size_t)n; s->input[s->input_size] = 0; s->activity_ms = s->now;
    } else if (!n || !retryable()) { close_client(s); return; }
    for (unsigned budget = 0; budget < 8 && s->input_size && !s->output_size; budget++) {
        size_t consume = 0;
        if (s->skip_interleaved) {
            consume = s->skip_interleaved < s->input_size ? s->skip_interleaved : s->input_size;
            s->skip_interleaved -= consume;
        } else if ((uint8_t)s->input[0] == '$') {
            if (s->input_size < 4) break;
            size_t length = get16((uint8_t *)s->input + 2);
            if (length + 4 > INPUT_SIZE) {
                s->skip_interleaved = length; consume = 4;
            } else {
                if (length + 4 > s->input_size) break;
                for (unsigned i = 0; i < 2; i++) {
                    if (s->tracks[i].setup && s->tracks[i].tcp &&
                        (uint8_t)s->input[1] == s->tracks[i].channel[1])
                        receiver_report(s, (uint8_t *)s->input + 4, length);
                }
                consume = length + 4;
            }
        } else {
            char *end = strstr(s->input, "\r\n\r\n");
            if (!end) break;
            size_t header = (size_t)(end - s->input) + 4;
            /* Content-Length is parsed before splitting the request, so
             * pipelined messages and bodies are consumed at exact boundaries. */
            unsigned body = 0, have_length = 0;
            char *line = strstr(s->input, "\r\n");
            int bad = memchr(s->input, 0, header) != NULL;
            while (!bad && line && line + 2 < end) {
                line += 2;
                char *next = strstr(line, "\r\n"); if (!next || next > end) { bad = 1; break; }
                if (!strncasecmp(line, "Content-Length:", 15)) {
                    char number[24]; size_t len = (size_t)(next - line) - 15;
                    if (have_length++ || len >= sizeof(number)) { bad = 1; break; }
                    memcpy(number, line + 15, len); number[len] = 0;
                    char *p = number; while (*p == ' ' || *p == '\t') p++;
                    if (parse_number(p, INPUT_SIZE, &body)) bad = 1;
                }
                line = next;
            }
            if (bad || body > INPUT_SIZE - header) {
                s->stats.malformed_requests++; response(s, 400, 0, NULL, NULL);
                s->close_after_response = 1; return;
            }
            consume = header + body;
            if (consume > s->input_size) break;
            /* Terminate before any body; the next pipelined request is left
             * untouched. No dynamic per-request allocation is needed. */
            s->input[header - 2] = 0;
            handle_request(s, s->input, header);
            if (s->client < 0) return;
        }
        memmove(s->input, s->input + consume, s->input_size - consume);
        s->input_size -= consume; s->input[s->input_size] = 0;
        s->request_ms = s->now;
    }
    if (s->input_size == INPUT_SIZE && !s->output_size) {
        s->stats.malformed_requests++; response(s, 400, 0, NULL, NULL); s->close_after_response = 1;
    }
}

static int flush_output(rtsp_server_t *s)
{
    if (!s->output_size) return 1;
    int index = s->output_track, tcp = index < 0 || s->tracks[index].tcp;
    uint32_t wait = (uint32_t)(s->now - s->write_ms);
    if (index >= 0 && wait > s->stats.max_packet_wait_ms) s->stats.max_packet_wait_ms = wait;
    /* A stale media packet must not hold the microphone behind an old JPEG.
     * UDP is atomic. TCP may be discarded only before any interleaved header
     * byte has been written; a partial record must be completed unchanged. */
    if (index >= 0 && !s->output_rtcp && !s->output_offset &&
        (index ? wait > 60U : (uint32_t)(s->now - s->frame_ms) > FRAME_TIMEOUT_MS)) {
        s->stats.media_expired++;
        if (index) s->stats.audio_dropped++;
        else { s->stats.video_dropped++; s->jpeg.scan_size = 0; }
        s->output_size = 0;
        return 1;
    }
    int fd = s->client;
    struct sockaddr_in destination = s->peer;
    if (!tcp) {
        fd = s->tracks[index].fd[s->output_rtcp];
        destination.sin_port = htons(s->tracks[index].client_port[s->output_rtcp]);
    }
    size_t left = s->output_size - s->output_offset;
    int n = tcp ? (int)send(fd, s->output + s->output_offset, left, MSG_NOSIGNAL) :
        (int)sendto(fd, s->output, left, 0, (struct sockaddr *)&destination, sizeof(destination));
    if (n < 0) s->stats.last_send_error = errno;
    if (n < 0 && retryable()) { s->stats.send_would_block++; return 0; }
    if (n <= 0 || (!tcp && (size_t)n != left)) { s->stats.send_failures++; close_client(s); return -1; }
    s->output_offset += (size_t)n;
    s->progress_ms = s->now;
    if (s->output_offset < s->output_size) return 0;
    if (index >= 0 && !s->output_rtcp) {
        size_t size = s->output_size - (tcp ? 4U : 0U);
        s->stats.rtp_packets++; s->stats.rtp_bytes += (uint32_t)size;
        s->tracks[index].packets++; s->tracks[index].octets += (uint32_t)size - 12U;
        if (index == 0 && !s->jpeg.scan_size) {
            s->stats.video_frames++;
            uint32_t age = (uint32_t)(s->now - s->frame_ms);
            if (age > s->stats.max_video_send_ms) s->stats.max_video_send_ms = age;
        }
        if (index == 1) s->stats.audio_frames++;
    } else if (index >= 0) s->stats.rtcp_reports++;
    s->output_size = s->output_offset = 0;
    if (s->close_after_response) { close_client(s); return -1; }
    return 1;
}

static uint8_t *packet_begin(rtsp_server_t *s, unsigned index, size_t size, int rtcp)
{
    rtsp_track_t *t = &s->tracks[index];
    s->output_track = (int)index; s->output_rtcp = rtcp;
    s->output_offset = 0; s->output_size = size + (t->tcp ? 4U : 0U);
    s->write_ms = s->progress_ms = s->now;
    if (t->tcp) {
        s->output[0] = '$'; s->output[1] = t->channel[rtcp]; put16(s->output + 2, (uint32_t)size);
        return s->output + 4;
    }
    return s->output;
}

static void rtp_header(rtsp_track_t *t, uint8_t *p, unsigned type, uint32_t timestamp, int marker)
{
    p[0] = 0x80; p[1] = (uint8_t)(type | (marker ? 0x80U : 0U));
    put16(p + 2, t->sequence++); put32(p + 4, timestamp); put32(p + 8, t->ssrc);
}

static void start_clock(rtsp_track_t *t, uint32_t timestamp, uint32_t now_ms)
{
    /* An SR maps a sampling clock to time, not the head of a send queue.
     * Re-anchoring on every dequeued frame turns queue jitter into clock
     * jumps at the receiver. Keep a stable fallback for simple sources. */
    if (!t->have_timestamp) {
        t->timestamp = timestamp; t->timestamp_ms = now_ms; t->have_timestamp = 1;
    }
}

static int audio_packet(rtsp_server_t *s)
{
    if (!s->tracks[1].setup) return 0;
    uint8_t *p = s->output + (s->tracks[1].tcp ? 4 : 0);
    size_t n = 0; uint32_t timestamp = 0;
    int rc = s->config.read_audio(s->config.ctx, p + 12, 480, &n, &timestamp);
    if (!rc) return 0;
    if (rc < 0 || !n || n > 480) { s->stats.audio_dropped++; return 0; }
    p = packet_begin(s, 1, n + 12, 0);
    rtp_header(&s->tracks[1], p, 8, timestamp, s->tracks[1].packets == 0);
    start_clock(&s->tracks[1], timestamp, s->now);
    return 1;
}

static int video_packet(rtsp_server_t *s)
{
    if (!s->tracks[0].setup) return 0;
    if (s->jpeg.scan_size && (uint32_t)(s->now - s->frame_ms) > FRAME_TIMEOUT_MS) {
        s->jpeg.scan_size = 0; s->stats.video_dropped++;
    }
    if (s->config.video_ready && !s->config.video_ready(s->config.ctx)) {
        /* Brief pressure is not a reason to waste already sent fragments.
         * The frame deadline above still bounds how long we retain a JPEG. */
        return 0;
    }
    if (!s->jpeg.scan_size) {
        size_t n = 0;
        int rc = s->config.read_video(s->config.ctx, s->frame, s->config.max_jpeg_size, &n, &s->frame_timestamp);
        if (!rc) return 0;
        if (rc < 0 || n > s->config.max_jpeg_size || rtsp_server_jpeg_parse(s->frame, n, &s->jpeg)) {
            s->stats.video_invalid++; s->jpeg.scan_size = 0; return 0;
        }
        s->frame_offset = 0; s->frame_ms = s->now;
    }
    uint8_t *p = s->output + (s->tracks[0].tcp ? 4 : 0);
    size_t count = 0;
    size_t n = rtsp_server_jpeg_payload(&s->jpeg, s->frame_offset, p + 12, PACKET_SIZE - 12, &count);
    if (!n) { s->stats.video_invalid++; s->jpeg.scan_size = 0; return 0; }
    s->frame_offset += count;
    int last = s->frame_offset == s->jpeg.scan_size;
    if (last) s->jpeg.scan_size = 0;
    p = packet_begin(s, 0, n + 12, 0);
    rtp_header(&s->tracks[0], p, 26, s->frame_timestamp, last);
    start_clock(&s->tracks[0], s->frame_timestamp, s->frame_ms);
    return 1;
}

static int sender_report(rtsp_server_t *s, unsigned index)
{
    rtsp_track_t *t = &s->tracks[index];
    /* Announce the common clock as soon as each track starts, while the
     * receiver is still priming. Waiting five seconds for the first SR makes
     * its initial A/V clock correction occur during audible playback. */
    if (!t->setup || !t->packets ||
        (t->have_report && (uint32_t)(s->now - t->report_ms) < 5000U)) return 0;
    char cname[32]; int cn = snprintf(cname, sizeof(cname), "rtsp-%s", s->session);
    size_t sdes = (8U + 2U + (size_t)cn + 1U + 3U) & ~(size_t)3;
    uint8_t *p = packet_begin(s, index, 28 + sdes, 1);
    memset(p, 0, 28 + sdes);
    p[0] = 0x80; p[1] = 200; put16(p + 2, 6); put32(p + 4, t->ssrc);
    /* Unsynchronized senders use a relative NTP clock (RFC 3550 section 6.4.1).
     * Both tracks share this origin; no wall-clock configuration is required. */
    put32(p + 8, (uint32_t)(s->elapsed_ms / 1000U));
    put32(p + 12, (uint32_t)((s->elapsed_ms % 1000U) * UINT64_C(4294967296) / 1000U));
    uint32_t timestamp = s->config.rtp_clock ?
        s->config.rtp_clock(s->config.ctx, index, s->now) :
        t->timestamp + (uint32_t)(s->now - t->timestamp_ms) * (index ? 8U : 90U);
    put32(p + 16, timestamp);
    put32(p + 20, t->packets); put32(p + 24, t->octets);
    p[28] = 0x81; p[29] = 202; put16(p + 30, (uint32_t)(sdes / 4 - 1));
    put32(p + 32, t->ssrc); p[36] = 1; p[37] = (uint8_t)cn; memcpy(p + 38, cname, (size_t)cn);
    t->report_ms = s->now; t->have_report = 1; return 1;
}

int rtsp_server_open(const rtsp_server_config_t *config, size_t config_size, rtsp_server_t **out)
{
    if (out) *out = NULL;
    if (!out || !config || config_size < sizeof(*config) || !config->path || config->path[0] != '/' ||
        strlen(config->path) < 2 || strlen(config->path) >= 128 ||
        !config->read_video || !config->random32 || config->max_jpeg_size < 1024 ||
        config->max_jpeg_size > 0xffffffU || config->video_packets_per_poll > 32U ||
        config->video_dscp > 63U || config->audio_dscp > 63U) return -1;
    for (const char *p = config->path; *p; p++)
        if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') ||
              (*p >= '0' && *p <= '9') || *p == '/' || *p == '-' || *p == '_')) return -1;
    rtsp_server_t *s = calloc(1, sizeof(*s));
    if (!s) return -1;
    s->listener = s->client = -1;
    for (unsigned i = 0; i < 2; i++) s->tracks[i].fd[0] = s->tracks[i].fd[1] = -1;
    s->config = *config; strcpy(s->path, config->path);
    s->frame = malloc(config->max_jpeg_size);
    if (!s->frame) { rtsp_server_close(s); return -1; }
    s->listener = socket(AF_INET, SOCK_STREAM, 0);
    if (s->listener < 0) { rtsp_server_close(s); return -1; }
    int one = 1; (void)setsockopt(s->listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    struct sockaddr_in local; memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET; local.sin_port = htons(config->port);
    if (nonblocking(s->listener) || bind(s->listener, (struct sockaddr *)&local, sizeof(local)) ||
        listen(s->listener, 2)) { rtsp_server_close(s); return -1; }
    socklen_t len = sizeof(local);
    if (getsockname(s->listener, (struct sockaddr *)&local, &len)) { rtsp_server_close(s); return -1; }
    s->stats.port = ntohs(local.sin_port);
    *out = s; return 0;
}

int rtsp_server_poll(rtsp_server_t *s, uint32_t now_ms)
{
    if (!s) return -1;
    s->now = now_ms;
    if (s->have_clock) s->elapsed_ms += (uint32_t)(now_ms - s->last_poll);
    s->have_clock = 1; s->last_poll = now_ms;
    struct sockaddr_in peer; socklen_t plen = sizeof(peer);
    int fd = accept(s->listener, (struct sockaddr *)&peer, &plen);
    if (fd >= 0) {
        if (s->client >= 0 || nonblocking(fd)) { s->stats.rejected_clients++; close(fd); }
        else {
            s->client = fd; s->peer = peer; s->activity_ms = s->request_ms = now_ms;
            int one = 1; (void)setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
            snprintf(s->session, sizeof(s->session), "%08lx", (unsigned long)s->config.random32(s->config.ctx));
            s->stats.connections++;
        }
    } else if (!retryable()) return -1;
    if (s->client < 0) return 0;
    if ((uint32_t)(now_ms - s->activity_ms) > IDLE_TIMEOUT_MS ||
        (s->input_size && (uint32_t)(now_ms - s->request_ms) > REQUEST_TIMEOUT_MS) ||
        (s->output_size && (uint32_t)(now_ms - s->progress_ms) > WRITE_TIMEOUT_MS)) {
        close_client(s); return 0;
    }
    if (flush_output(s) < 0) return 0;
    read_control(s);
    if (s->client < 0) return 0;
    for (unsigned i = 0; i < 2; i++) {
        rtsp_track_t *t = &s->tracks[i];
        if (!t->setup || t->tcp) continue;
        uint8_t data[512]; struct sockaddr_in from; socklen_t len = sizeof(from);
        int n = (int)recvfrom(t->fd[1], data, sizeof(data), 0, (struct sockaddr *)&from, &len);
        if (n > 0 && from.sin_addr.s_addr == s->peer.sin_addr.s_addr && ntohs(from.sin_port) == t->client_port[1]) {
            receiver_report(s, data, (size_t)n); s->activity_ms = now_ms;
        }
    }
    unsigned video_packets = 0;
    unsigned video_limit = s->config.video_packets_per_poll ? s->config.video_packets_per_poll : 8U;
    for (unsigned budget = 0; budget < 32 && s->client >= 0; budget++) {
        int rc = flush_output(s);
        if (rc <= 0 || !s->playing) break;
        if (sender_report(s, 0) || sender_report(s, 1) || audio_packet(s)) continue;
        if (video_packets == video_limit || !video_packet(s)) break;
        video_packets++;
    }
    return 0;
}

int rtsp_server_get_stats(const rtsp_server_t *s, rtsp_server_stats_t *stats, size_t size)
{
    if (!s || !stats || size < sizeof(*stats)) return -1;
    *stats = s->stats; stats->client_connected = s->client >= 0; stats->playing = (uint32_t)s->playing;
    stats->audio_playing = (uint32_t)(s->playing && s->tracks[1].setup);
    return 0;
}

void rtsp_server_close(rtsp_server_t *s)
{
    if (!s) return;
    close_client(s);
    if (s->listener >= 0) close(s->listener);
    free(s->frame); free(s);
}
