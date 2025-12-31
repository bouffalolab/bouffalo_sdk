#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ring_buffer.h"

/*
 * Host ringbuffer stress test (SPSC): 1 writer thread + 1 reader thread.
 * - Writer generates framed messages and writes in random chunk sizes.
 * - Reader reads in random chunk sizes and validates sequence + payload.
 *
 * Frame format (little-endian):
 *   magic[2] = {0xA5,0x5A}
 *   payload_len u16
 *   seq u32
 *   payload[payload_len] where payload[i] == (uint8_t)((seq + i) & 0xFF)
 */

#define RB_SIZE            (4096u)
#define MAX_PAYLOAD        (512u)
#define MAX_RW_CHUNK       (512u)
#define MAGIC0             (0xA5u)
#define MAGIC1             (0x5Au)
#define HEADER_SIZE        (2u + 2u + 4u)

typedef struct {
    Ring_Buffer_Type rb;
    uint8_t storage[RB_SIZE];

    volatile bool producer_done;
    volatile bool failed;

    uint64_t produced_msgs;
    uint64_t produced_bytes;
    uint64_t consumed_msgs;
    uint64_t consumed_bytes;

    uint32_t seed_producer;
    uint32_t seed_consumer;

    unsigned duration_sec;
    bool use_lock;
} test_ctx_t;

static pthread_mutex_t g_rb_mu = PTHREAD_MUTEX_INITIALIZER;

static void rb_lock_cb(void)
{
    (void)pthread_mutex_lock(&g_rb_mu);
}

static void rb_unlock_cb(void)
{
    (void)pthread_mutex_unlock(&g_rb_mu);
}

static uint32_t rnd_u32(uint32_t *seed)
{
    /* Deterministic LCG for reproducibility across libc variants */
    *seed = (*seed * 1664525u) + 1013904223u;
    return *seed;
}

static uint32_t rnd_range(uint32_t *seed, uint32_t lo, uint32_t hi_inclusive)
{
    uint32_t r = rnd_u32(seed);
    uint32_t span = (hi_inclusive - lo) + 1u;
    return lo + (r % span);
}

static void logf_threadsafe(const char *tag, const char *fmt, ...)
{
    static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
    va_list ap;

    pthread_mutex_lock(&mu);
    fprintf(stderr, "[%s] ", tag);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    pthread_mutex_unlock(&mu);
}

static void write_u16_le(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
}

static void write_u32_le(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
    p[2] = (uint8_t)((v >> 16) & 0xFFu);
    p[3] = (uint8_t)((v >> 24) & 0xFFu);
}

static uint16_t read_u16_le(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t read_u32_le(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

typedef struct {
    /* Parser state */
    uint8_t buf[HEADER_SIZE + MAX_PAYLOAD];
    uint32_t have;

    bool in_frame;
    uint16_t payload_len;
    uint32_t seq;
    uint32_t payload_read;

    uint32_t expected_seq;
    bool expected_seq_init;
} parser_t;

static void parser_init(parser_t *ps)
{
    memset(ps, 0, sizeof(*ps));
}

static bool parser_feed(test_ctx_t *ctx, parser_t *ps, const uint8_t *data, uint32_t len)
{
    /* Returns false on validation failure */
    for (uint32_t i = 0; i < len; i++) {
        uint8_t b = data[i];

        if (!ps->in_frame) {
            /* Find MAGIC0 MAGIC1 */
            if (ps->have == 0) {
                if (b == MAGIC0) {
                    ps->buf[ps->have++] = b;
                }
                continue;
            }
            if (ps->have == 1) {
                if (b == MAGIC1) {
                    ps->buf[ps->have++] = b;
                } else {
                    ps->have = 0;
                    if (b == MAGIC0) {
                        ps->buf[ps->have++] = b;
                    }
                }
                continue;
            }

            /* Collect remaining header bytes: len(2) + seq(4) */
            ps->buf[ps->have++] = b;
            if (ps->have == HEADER_SIZE) {
                ps->payload_len = read_u16_le(&ps->buf[2]);
                ps->seq = read_u32_le(&ps->buf[4]);

                if (ps->payload_len > MAX_PAYLOAD) {
                    logf_threadsafe("FAIL", "payload_len too large: %u", ps->payload_len);
                    return false;
                }

                if (!ps->expected_seq_init) {
                    ps->expected_seq = ps->seq;
                    ps->expected_seq_init = true;
                }

                if (ps->seq != ps->expected_seq) {
                    logf_threadsafe("FAIL", "seq mismatch: got=%" PRIu32 " expected=%" PRIu32, ps->seq, ps->expected_seq);
                    return false;
                }

                ps->payload_read = 0;
                if (ps->payload_len == 0) {
                    /* Completed one message with empty payload */
                    ps->expected_seq++;
                    ps->in_frame = false;
                    ps->have = 0;
                    ctx->consumed_msgs++;
                } else {
                    ps->in_frame = true;
                }
            }
            continue;
        }

        /* In frame: validate payload */
        uint8_t expected = (uint8_t)((ps->seq + ps->payload_read) & 0xFFu);
        if (b != expected) {
            logf_threadsafe("FAIL", "payload byte mismatch: seq=%" PRIu32 " off=%" PRIu32 " got=0x%02x exp=0x%02x",
                            ps->seq, ps->payload_read, b, expected);
            return false;
        }
        ps->payload_read++;

        if (ps->payload_read == ps->payload_len) {
            /* Completed one message */
            ps->expected_seq++;
            ps->in_frame = false;
            ps->have = 0;
            ctx->consumed_msgs++;
        }
    }

    return true;
}

typedef struct {
    uint8_t frame[HEADER_SIZE + MAX_PAYLOAD];
    uint32_t total;
    uint32_t sent;
    uint32_t payload_len;
    uint32_t seq;
} out_msg_t;

static void build_msg(out_msg_t *m, uint32_t seq, uint32_t payload_len)
{
    m->seq = seq;
    m->payload_len = payload_len;
    m->frame[0] = MAGIC0;
    m->frame[1] = MAGIC1;
    write_u16_le(&m->frame[2], (uint16_t)payload_len);
    write_u32_le(&m->frame[4], seq);
    for (uint32_t i = 0; i < payload_len; i++) {
        m->frame[HEADER_SIZE + i] = (uint8_t)((seq + i) & 0xFFu);
    }
    m->total = HEADER_SIZE + payload_len;
    m->sent = 0;
}

static void *producer_thread(void *arg)
{
    test_ctx_t *ctx = (test_ctx_t *)arg;

    uint64_t start_ms;
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        start_ms = (uint64_t)ts.tv_sec * 1000u + (uint64_t)ts.tv_nsec / 1000000u;
    }

    uint32_t seq = 0;
    out_msg_t cur;
    bool have_msg = false;

    uint64_t end_ms = start_ms + (uint64_t)ctx->duration_sec * 1000u;

    while (!ctx->failed) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        uint64_t now_ms = (uint64_t)ts.tv_sec * 1000u + (uint64_t)ts.tv_nsec / 1000000u;
        /* Stop only between frames to keep stream well-formed */
        if ((now_ms >= end_ms) && !have_msg) {
            break;
        }

        if (!have_msg) {
            uint32_t payload = rnd_range(&ctx->seed_producer, 0u, MAX_PAYLOAD);
            build_msg(&cur, seq, payload);
            seq++;
            have_msg = true;
            ctx->produced_msgs++;
        }

        uint32_t remaining = cur.total - cur.sent;
        uint32_t want = rnd_range(&ctx->seed_producer, 1u, (remaining < MAX_RW_CHUNK ? remaining : MAX_RW_CHUNK));

        uint32_t wrote = Ring_Buffer_Write(&ctx->rb, &cur.frame[cur.sent], want);
        if (wrote == 0) {
            /* Backpressure */
            usleep(50);
            continue;
        }

        cur.sent += wrote;
        ctx->produced_bytes += wrote;

        if (cur.sent == cur.total) {
            have_msg = false;
        }

        /* Encourage interleaving */
        if ((rnd_u32(&ctx->seed_producer) & 0x3Fu) == 0) {
            usleep(10);
        }
    }

    ctx->producer_done = true;
    return NULL;
}

static void *consumer_thread(void *arg)
{
    test_ctx_t *ctx = (test_ctx_t *)arg;
    parser_t ps;
    parser_init(&ps);

    uint8_t tmp[MAX_RW_CHUNK];

    while (!ctx->failed) {
        uint32_t want = rnd_range(&ctx->seed_consumer, 1u, MAX_RW_CHUNK);
        uint32_t got = Ring_Buffer_Read(&ctx->rb, tmp, want);
        if (got == 0) {
            if (ctx->producer_done && Ring_Buffer_Get_Length(&ctx->rb) == 0) {
                break;
            }
            usleep(50);
            continue;
        }

        ctx->consumed_bytes += got;
        if (!parser_feed(ctx, &ps, tmp, got)) {
            ctx->failed = true;
            break;
        }

        if ((rnd_u32(&ctx->seed_consumer) & 0x7Fu) == 0) {
            usleep(10);
        }
    }

    /* If we stopped cleanly, ensure no partial header/frame remains */
    if (!ctx->failed) {
        if (ps.in_frame || ps.have != 0) {
            logf_threadsafe("FAIL", "stopped with partial frame: in_frame=%d have=%u payload_read=%u payload_len=%u",
                            ps.in_frame ? 1 : 0, ps.have, ps.payload_read, ps.payload_len);
            ctx->failed = true;
        }
    }

    return NULL;
}

static void usage(const char *argv0)
{
    fprintf(stderr,
            "Usage: %s [--seconds N] [--seed S] [--no-lock]\n"
            "  --seconds N   duration in seconds (default 3)\n"
            "  --seed S      base seed (default time-based)\n",
            argv0);
}

int main(int argc, char **argv)
{
    test_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));

    ctx.duration_sec = 3;
    ctx.use_lock = true;
    uint32_t base_seed = (uint32_t)time(NULL);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--seconds") == 0) {
            if (i + 1 >= argc) {
                usage(argv[0]);
                return 2;
            }
            ctx.duration_sec = (unsigned)strtoul(argv[++i], NULL, 0);
        } else if (strcmp(argv[i], "--seed") == 0) {
            if (i + 1 >= argc) {
                usage(argv[0]);
                return 2;
            }
            base_seed = (uint32_t)strtoul(argv[++i], NULL, 0);
        } else if (strcmp(argv[i], "--no-lock") == 0) {
            ctx.use_lock = false;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            usage(argv[0]);
            return 2;
        }
    }

    ctx.seed_producer = base_seed ^ 0x12345678u;
    ctx.seed_consumer = base_seed ^ 0x87654321u;

    if (ctx.use_lock) {
        Ring_Buffer_Init(&ctx.rb, ctx.storage, RB_SIZE, rb_lock_cb, rb_unlock_cb);
    } else {
        Ring_Buffer_Init(&ctx.rb, ctx.storage, RB_SIZE, NULL, NULL);
        fprintf(stderr, "NOTE: --no-lock runs with data races on host; failures may be compiler/UB artifacts.\n");
    }

    pthread_t prod, cons;
    if (pthread_create(&prod, NULL, producer_thread, &ctx) != 0) {
        perror("pthread_create(producer)");
        return 1;
    }
    if (pthread_create(&cons, NULL, consumer_thread, &ctx) != 0) {
        perror("pthread_create(consumer)");
        return 1;
    }

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    uint32_t len = Ring_Buffer_Get_Length(&ctx.rb);
    uint32_t empty = Ring_Buffer_Get_Empty_Length(&ctx.rb);
    Ring_Buffer_Status_Type st = Ring_Buffer_Get_Status(&ctx.rb);

    fprintf(stderr, "produced_msgs=%" PRIu64 " produced_bytes=%" PRIu64 "\n", ctx.produced_msgs, ctx.produced_bytes);
    fprintf(stderr, "consumed_msgs=%" PRIu64 " consumed_bytes=%" PRIu64 "\n", ctx.consumed_msgs, ctx.consumed_bytes);
    fprintf(stderr, "final_len=%u final_empty=%u status=%d\n", len, empty, (int)st);

    if (ctx.failed) {
        fprintf(stderr, "RESULT: FAIL\n");
        return 1;
    }

    fprintf(stderr, "RESULT: PASS\n");
    return 0;
}
