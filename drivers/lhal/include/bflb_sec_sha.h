#ifndef _BFLB_SEC_SHA_H
#define _BFLB_SEC_SHA_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup SEC_SHA
  * @{
  */

#define SHA_MODE_SHA256     0
#define SHA_MODE_SHA224     1
#define SHA_MODE_SHA1       2
#define SHA_MODE_SHA512     4
#define SHA_MODE_SHA384     5
#define SHA_MODE_SHA512T224 6
#define SHA_MODE_SHA512T256 7
#define SHA_MODE_CRC16      9
#define SHA_MODE_CRC32      10

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

struct bflb_sha_link_s {
    uint32_t                 : 2;  /*!< [1:0]Reserved */
    uint32_t sha_mode        : 3;  /*!< [4:2]Sha-256/sha-224/sha-1/sha-1/sha-512/sha-384/sha-512T224/sha-512T256 */
    uint32_t                 : 1;  /*!< [5]Reserved */
    uint32_t sha_newhash_dis : 1;  /*!< [6]New hash or accumulate last hash */
    uint32_t                 : 2;  /*!< [8:7]Reserved */
    uint32_t sha_intclr      : 1;  /*!< [9]Clear interrupt */
    uint32_t sha_intset      : 1;  /*!< [10]Set interrupt */
    uint32_t                 : 1;  /*!< [11]Reserved */
    uint32_t sha_mode_ext    : 2;  /*!< [13:12]Extention,0:sha 1:md5 2:crc16 3:crc32 */
    uint32_t                 : 2;  /*!< [15:14]Reserved */
    uint32_t sha_msglen      : 16; /*!< [31:16]Number of 512-bit block */
    uint32_t sha_srcaddr;          /*!< Message source address */
    uint32_t result[16];           /*!< Result of SHA */
};

struct bflb_sha1_link_ctx_s {
    uint32_t total[2];                                    /*!< Number of bytes processed */
    __attribute__((aligned(32))) uint8_t sha_buf[64];     /*!< Data not processed but in this temp buffer */
    __attribute__((aligned(32))) uint8_t sha_padding[64]; /*!< Padding data */
    uint32_t link_addr;                                   /*!< Link configure address */
    struct bflb_sha_link_s link_cfg;
};

struct bflb_sha256_link_ctx_s {
    uint32_t total[2];                                    /*!< Number of bytes processed */
    __attribute__((aligned(32))) uint8_t sha_buf[64];     /*!< Data not processed but in this temp buffer */
    __attribute__((aligned(32))) uint8_t sha_padding[64]; /*!< Padding data */
    uint32_t link_addr;                                   /*!< Link configure address */
    struct bflb_sha_link_s link_cfg;
};

struct bflb_sha512_link_ctx_s {
    uint64_t total[2];                                     /*!< Number of bytes processed */
    __attribute__((aligned(32))) uint8_t sha_buf[128];     /*!< Data not processed but in this temp buffer */
    __attribute__((aligned(32))) uint8_t sha_padding[128]; /*!< Padding data */
    uint32_t link_addr;                                    /*!< Link configure address */
    struct bflb_sha_link_s link_cfg;
};

struct bflb_crc_link_s {
    uint32_t               : 2;  /*!< [1:0]Reserved */
    uint32_t sha_mode      : 3;  /*!< [4:2]Sha-256/sha-224/sha-1/sha-1/sha-512/sha-384/sha-512T224/sha-512T256 */
    uint32_t               : 1;  /*!< [5]Reserved */
    uint32_t sha_hash_sel  : 1;  /*!< [6]New hash or accumulate last hash */
    uint32_t               : 2;  /*!< [8:7]Reserved */
    uint32_t sha_int_clear : 1;  /*!< [9]Clear interrupt */
    uint32_t sha_int_set   : 1;  /*!< [10]Set interrupt */
    uint32_t               : 1;  /*!< [11]Reserved */
    uint32_t sha_mode_ext  : 2;  /*!< [13:12]Extention,0:sha 1:md5 2:crc16 3:crc32 */
    uint32_t               : 2;  /*!< [15:14]Reserved */
    uint32_t sha_msg_len   : 16; /*!< [31:16]Number of 512-bit block */
    uint32_t sha_src_addr;       /*!< Message source address */
    uint32_t poly;               /*!< poly of CRC */
    uint32_t dout_inv : 1;       /*!< [0]dout inv of CRC */
    uint32_t dout_ref : 1;       /*!< [1]dout ref of CRC */
    uint32_t din_ref  : 1;       /*!< [2]din ref of CRC */
    uint32_t          : 29;      /*!< [31:3]Reserved */
    uint32_t iv_and_hash;        /*!< CRC hash and also used as iv in 1st block */

} __attribute__((aligned(4)));

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] mode
 */
void bflb_sha_init(struct bflb_device_s *dev, uint8_t mode);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 */
void bflb_sha1_start(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 */
void bflb_sha256_start(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 */
void bflb_sha512_start(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] input
 * @param [in] len
 * @return int
 */
int bflb_sha1_update(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx, const uint8_t *input, uint32_t len);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] input
 * @param [in] len
 * @return int
 */
int bflb_sha256_update(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx, const uint8_t *input, uint32_t len);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] input
 * @param [in] len
 * @return int
 */
int bflb_sha512_update(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx, const uint8_t *input, uint64_t len);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] output
 */
void bflb_sha1_finish(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx, uint8_t *output);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] output
 */
void bflb_sha256_finish(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx, uint8_t *output);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] output
 */
void bflb_sha512_finish(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx, uint8_t *output);

/**
 * @brief
 *
 * @param [in] dev
 */
void bflb_sha_link_init(struct bflb_device_s *dev);

/**
 * @brief
 *
 * @param [in] dev
 */
void bflb_sha_link_deinit(struct bflb_device_s *dev);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] link
 */
void bflb_sha1_link_start(struct bflb_device_s *dev, struct bflb_sha1_link_ctx_s *ctx);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] link
 */
void bflb_sha256_link_start(struct bflb_device_s *dev, struct bflb_sha256_link_ctx_s *ctx, int is224);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] link
 */
void bflb_sha512_link_start(struct bflb_device_s *dev, struct bflb_sha512_link_ctx_s *ctx, int is384);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] input
 * @param [in] len
 * @return int
 */
int bflb_sha1_link_update(struct bflb_device_s *dev,
                          struct bflb_sha1_link_ctx_s *ctx,
                          const uint8_t *input,
                          uint32_t len);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] input
 * @param [in] len
 * @return int
 */
int bflb_sha256_link_update(struct bflb_device_s *dev,
                            struct bflb_sha256_link_ctx_s *ctx,
                            const uint8_t *input,
                            uint32_t len);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] input
 * @param [in] len
 * @return int
 */
int bflb_sha512_link_update(struct bflb_device_s *dev,
                            struct bflb_sha512_link_ctx_s *ctx,
                            const uint8_t *input,
                            uint64_t len);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] output
 */
void bflb_sha1_link_finish(struct bflb_device_s *dev,
                           struct bflb_sha1_link_ctx_s *ctx,
                           uint8_t *output);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] output
 */
void bflb_sha256_link_finish(struct bflb_device_s *dev,
                             struct bflb_sha256_link_ctx_s *ctx,
                             uint8_t *output);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] ctx
 * @param [in] output
 */
void bflb_sha512_link_finish(struct bflb_device_s *dev,
                             struct bflb_sha512_link_ctx_s *ctx,
                             uint8_t *output);

/**
 * @brief
 * @return 0 for success, -1 for failure
 * @param [in] dev
 */
int bflb_group0_request_sha_access(struct bflb_device_s *dev);

/**
 * @brief
 * @return 0 for success, -1 for failure
 * @param [in] dev
 */
int bflb_group1_request_sha_access(struct bflb_device_s *dev);

/**
 * @brief
 *
 * @param [in] dev
 */
void bflb_group0_release_sha_access(struct bflb_device_s *dev);

/**
 * @brief
 *
 * @param [in] dev
 */
void bflb_group1_release_sha_access(struct bflb_device_s *dev);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] addr
 * @param [in] in
 * @param [in] len
 * @param [in] out
 */
int bflb_sha_crc32_link_work(struct bflb_device_s *dev, uint32_t addr, const uint8_t *in, uint32_t len, uint8_t *out);

/**
 * @brief
 *
 * @param [in] dev
 * @param [in] input, 32 bytes aligned
 * @param [in] output
 * @param [in] nblock
 */
int bflb_sha1_once_padded(struct bflb_device_s *dev, const uint8_t *input, uint8_t *output, uint32_t nblock);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#endif