#ifndef _BFLB_SEC_SHA_H
#define _BFLB_SEC_SHA_H

#include "bflb_core.h"

#define SHA_MODE_SHA1   0
#define SHA_MODE_SHA224 1
#define SHA_MODE_SHA256 2
#define SHA_MODE_SHA384 3
#define SHA_MODE_SHA512 4

struct bflb_sha1_ctx_s {
    uint32_t total[2];                                    /*!< Number of bytes processed */
    __attribute__((aligned(32))) uint8_t sha_buf[64];     /*!< Data not processed but in this temp buffer */
    __attribute__((aligned(32))) uint8_t sha_padding[64]; /*!< Padding data */
    uint8_t sha_feed;                                     /*!< Sha has feed data */
};

struct bflb_sha256_ctx_s {
    uint32_t total[2];                                    /*!< Number of bytes processed */
    __attribute__((aligned(32))) uint8_t sha_buf[64];     /*!< Data not processed but in this temp buffer */
    __attribute__((aligned(32))) uint8_t sha_padding[64]; /*!< Padding data */
    uint8_t sha_feed;                                     /*!< Sha has feed data */
};

struct bflb_sha512_ctx_s {
    uint64_t total[2];                                     /*!< Number of bytes processed */
    __attribute__((aligned(32))) uint8_t sha_buf[128];     /*!< Data not processed but in this temp buffer */
    __attribute__((aligned(32))) uint8_t sha_padding[128]; /*!< Padding data */
    uint8_t sha_feed;                                      /*!< Sha has feed data */
};

#ifdef __cplusplus
extern "C" {
#endif

void bflb_sha_init(struct bflb_device_s *dev, uint8_t mode);
void bflb_sha1_start(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx);
void bflb_sha256_start(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx);
void bflb_sha512_start(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx);
int bflb_sha1_update(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx, const uint8_t *input, uint32_t len);
int bflb_sha256_update(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx, const uint8_t *input, uint32_t len);
int bflb_sha512_update(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx, const uint8_t *input, uint32_t len);
void bflb_sha1_finish(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx, uint8_t *output);
void bflb_sha256_finish(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx, uint8_t *output);
void bflb_sha512_finish(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx, uint8_t *output);

#ifdef __cplusplus
}
#endif

#endif