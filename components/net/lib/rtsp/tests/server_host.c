/* Copyright (c) 2026 Bouffalo Lab. SPDX-License-Identifier: Apache-2.0 */
#define _POSIX_C_SOURCE 200809L
#include "rtsp_server.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/random.h>

static volatile sig_atomic_t running = 1;
static uint8_t jpeg[1024 * 1024];
static size_t jpeg_size;
static uint32_t now, video_at, audio_at, samples;
static uint32_t random32(void *ctx)
{
    (void)ctx; uint32_t n;
    if (getrandom(&n, sizeof(n), 0) != sizeof(n)) exit(2);
    return n;
}
static int video(void *ctx, uint8_t *dst, size_t cap, size_t *n, uint32_t *ts)
{
    (void)ctx;
    if ((uint32_t)(now - video_at) < 33U) return 0;
    video_at = now;
    if (cap < jpeg_size) return -1;
    memcpy(dst, jpeg, jpeg_size); *n = jpeg_size; *ts = now * 90U; return 1;
}
static int audio(void *ctx, uint8_t *dst, size_t cap, size_t *n, uint32_t *ts)
{
    (void)ctx;
    if ((uint32_t)(now - audio_at) < 20U) return 0;
    audio_at = now;
    if (cap < 160) return -1;
    memset(dst, 0xd5, 160); *n = 160; *ts = samples; samples += 160; return 1;
}
static void stop(int sig) { (void)sig; running = 0; }
int main(int argc, char **argv)
{
    if (argc < 2) return 2;
    FILE *f = fopen(argv[1], "rb"); if (!f) return 2;
    jpeg_size = fread(jpeg, 1, sizeof(jpeg), f); fclose(f);
    rtsp_server_config_t cfg = {.port = argc > 2 ? (uint16_t)atoi(argv[2]) : 0,
        .path = "/live", .max_jpeg_size = sizeof(jpeg), .read_video = video,
        .read_audio = audio, .random32 = random32};
    rtsp_server_t *server;
    if (rtsp_server_open(&cfg, sizeof(cfg), &server)) return 1;
    rtsp_server_stats_t stats;
    rtsp_server_get_stats(server, &stats, sizeof(stats));
    printf("PORT %u\n", stats.port); fflush(stdout);
    signal(SIGINT, stop); signal(SIGTERM, stop);
    while (running) {
        struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
        now = (uint32_t)((uint64_t)ts.tv_sec * 1000 + (unsigned long)ts.tv_nsec / 1000000);
        if (rtsp_server_poll(server, now)) break;
        struct timespec wait = {.tv_nsec = 1000000}; nanosleep(&wait, NULL);
    }
    rtsp_server_get_stats(server, &stats, sizeof(stats));
    printf("frames=%lu audio=%lu invalid=%lu blocked=%lu connections=%lu disconnects=%lu\n",
           (unsigned long)stats.video_frames, (unsigned long)stats.audio_frames,
           (unsigned long)stats.video_invalid, (unsigned long)stats.send_would_block,
           (unsigned long)stats.connections, (unsigned long)stats.disconnects);
    rtsp_server_close(server); return 0;
}
