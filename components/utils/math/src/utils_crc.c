#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <utils_crc.h>

extern const uint32_t crc32Tab[256];

void utils_crc32_stream_init(struct crc32_stream_ctx *ctx)
{
    ctx->crc = 0xffffffff;
}

void utils_crc32_stream_feed(struct crc32_stream_ctx *ctx, uint8_t data)
{
    ctx->crc = crc32Tab[(ctx->crc ^ data) & 0xFF] ^ (ctx->crc >> 8);
}

void utils_crc32_stream_feed_block(struct crc32_stream_ctx *ctx, const uint8_t *data, uint32_t len)
{
    while (len--)
        ctx->crc = crc32Tab[(ctx->crc ^ *data++) & 0xFF] ^ (ctx->crc >> 8);
}

uint32_t utils_crc32_stream_results(struct crc32_stream_ctx *ctx)
{
    return ctx->crc ^ 0xffffffff;
}

#define POLY 0x8408
uint16_t utils_crc16_ccitt(const void *dataIn, uint32_t len)
{
    const uint8_t *data_p = (const uint8_t *)dataIn;
    uint8_t i;
    uint8_t data;
    uint16_t crc;

    crc = 0xFFFF;

    if (len == 0)
        return (~crc);

    do {
        for (i = 0, data = (unsigned int)0xff & *data_p++;
            i < 8;
            i++, data >>= 1) {
        if ((crc & 0x0001) ^ (data & 0x0001))
            crc = (crc >> 1) ^ POLY;
        else
            crc >>= 1;
        }
    } while (--len);

    crc = ~crc;

    data = crc;

    return (crc);
}
