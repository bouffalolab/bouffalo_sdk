#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <utils_base64.h>
#include <utils_crc.h>
#include <utils_hex.h>

#define REVERSE(a) (((a)&0xff) << 24 | ((a)&0xff00) << 8 | ((a)&0xff0000) >> 8 | ((a)&0xff000000) >> 24)

#define COREDUM_CMD_BUF_LEN (128)

#define COREDUMP_VERSION "0.0.1"

#define COREDUMP_BLOCK_START_STR "\r\n------ DATA BEGIN "
#define COREDUMP_BLOCK_END_STR "\r\n------ END "
#define COREDUMP_BLOCK_CLOSE_STR " ------\r\n"

#define COREDUMP_START_STR "\r\n-+-+-+- BFLB COREDUMP v" COREDUMP_VERSION " +-+-+-+\r\n"

#define BASE64_LINE_WRAP  100

/**
 * ------ DATA BEGIN addr@len@desc ------
 * contents ....
 * ------ END crc ------
 */

static void cd_base64_write_block(const uint8_t buf[4], void *opaque) {
    int *line_wrap = (int *)opaque;
    putchar(buf[0]);
    putchar(buf[1]);
    putchar(buf[2]);
    putchar(buf[3]);
    if (++(*line_wrap) > (BASE64_LINE_WRAP >> 2)) {
        puts("\r\n");
        *line_wrap = 0;
    }
}

struct base64_word_ctx {
    /* for output */
    int line_wrap; /* base64 line wrap */

    /* for input */
    uintptr_t addr_base;
    uintptr_t addr_curr;
    uintptr_t addr_end;
    uint8_t buf[4] __attribute__((aligned(4))); /* cache */
    struct crc32_stream_ctx *crc_ctx;
};

static int read_word_cb(uint8_t *data, void *opaque) {
    struct base64_word_ctx *ctx = (struct base64_word_ctx *)opaque;
    uintptr_t base;

    if (ctx->addr_curr < ctx->addr_end) {
        base = (ctx->addr_curr >> 2) << 2;
        if (base != ctx->addr_base) {
            ctx->addr_base = base;
            *(uint32_t *)ctx->buf = *(uint32_t *)base;
        }

        *data = ctx->buf[ctx->addr_curr & 0x3];
        ctx->addr_curr++;

        /* update crc checksum */
        utils_crc32_stream_feed(ctx->crc_ctx, *data);

        return 0;
    } else {
        return 1;
    }
}

static void dump_base64_word(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx) {
    struct base64_word_ctx ctx = {0};

    ctx.addr_base = ((uintptr_t)data >> 2) << 2;
    ctx.addr_curr = (uintptr_t)data;
    ctx.addr_end = ctx.addr_base + len;
    *(uint32_t *)ctx.buf = *(uint32_t *)ctx.addr_base;
    ctx.crc_ctx = crc_ctx;

    utils_base64_encode_stream(read_word_cb, cd_base64_write_block, (void *)&ctx);
}

void coredump_print(uintptr_t addr, uintptr_t lma_addr, size_t len, const char *desc)
{
    static int init = 0;
    if (init == 0) {
        puts(COREDUMP_START_STR);
        init = 1;
    }
    struct crc32_stream_ctx crc_ctx;
    uint32_t crc;

    uintptr_t tmp;
    char tmp_buf[(sizeof(tmp) << 1)] = {0};

    /* Print block header */
    puts(COREDUMP_BLOCK_START_STR);

    /* print address */
    tmp = REVERSE(addr);
    utils_bin2hex(tmp_buf, &tmp, sizeof(uintptr_t));
    for(int i=0; i<(sizeof(tmp) << 1); i++)
        putchar(tmp_buf[i]);

    putchar('@');

    tmp = REVERSE(len);
    utils_bin2hex(tmp_buf, &tmp, sizeof(uint32_t));
    for(int i=0; i<(sizeof(tmp) << 1); i++)
        putchar(tmp_buf[i]);

    if (desc != NULL) {
        putchar('@');
        puts(desc);
    }

    puts(COREDUMP_BLOCK_CLOSE_STR);

    /* Init crc context */
    utils_crc32_stream_init(&crc_ctx);

    /* uart Dump block content */
    dump_base64_word((const void *)lma_addr, (ssize_t)len, &crc_ctx);

    /* Dump end */
    puts(COREDUMP_BLOCK_END_STR);

    crc = utils_crc32_stream_results(&crc_ctx);

    dump_base64_word((const void *)&crc, (ssize_t)sizeof(uint32_t), &crc_ctx);

    puts(COREDUMP_BLOCK_CLOSE_STR);
}
