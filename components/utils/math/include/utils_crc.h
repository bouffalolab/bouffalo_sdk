#ifndef __UTILS_CRC_H__
#define __UTILS_CRC_H__

#include <stdint.h>

struct crc32_stream_ctx {
    uint32_t crc;
};

void utils_crc32_stream_init(struct crc32_stream_ctx *ctx);
void utils_crc32_stream_feed(struct crc32_stream_ctx *ctx, uint8_t data);
void utils_crc32_stream_feed_block(struct crc32_stream_ctx *ctx, const uint8_t *data, uint32_t len);
uint32_t utils_crc32_stream_results(struct crc32_stream_ctx *ctx);

#endif
