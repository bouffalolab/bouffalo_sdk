#include <string.h>
#include "bflb_l1c.h"
#include "bflb_sec_sha.h"
#include "board.h"
#include "bl616cl_psram.h"

#define PSRAM_TEST_SIZE    (256 * 1024)
#define PSRAM_TEST_MAGIC   (0x50535241)
#define PSRAM_TEST_VERSION (0x00020000)
#define PSRAM_SHA256_SIZE  (32)
#define PSRAM_PATTERN_BASE (0x13572468)

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t prepare_count;
    uint32_t verify_count;
    uint32_t pattern_seed;
    uint32_t data_len;
    uint8_t sha256[PSRAM_SHA256_SIZE];
} psram_test_header_t;

ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t psram_test_buffer[PSRAM_TEST_SIZE];
static ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha256_ctx_s psram_sha256_ctx;

static inline psram_test_header_t *psram_get_header(void)
{
    return (psram_test_header_t *)psram_test_buffer;
}

static int psram_header_is_valid(const psram_test_header_t *hdr, uint32_t data_len)
{
    return (hdr->magic == PSRAM_TEST_MAGIC) &&
           (hdr->version == PSRAM_TEST_VERSION) &&
           (hdr->data_len == data_len);
}

static uint32_t psram_next_pattern_seed(const psram_test_header_t *hdr)
{
    return PSRAM_PATTERN_BASE + hdr->prepare_count;
}

static uint8_t psram_test_pattern_byte(uint32_t index, uint32_t seed)
{
    uint32_t value = seed;

    value ^= index * 131U;
    value += (index >> 3) * 17U;
    value ^= (index >> 11);

    return (uint8_t)(value & 0xFF);
}

static void psram_dump_sha256(const char *tag, const uint8_t *sha256)
{
    printf("[PSRAM] %s", tag);
    for (uint32_t i = 0; i < PSRAM_SHA256_SIZE; i++) {
        printf("%02X", sha256[i]);
    }
    printf("\r\n");
}

static int psram_calc_sha256(const uint8_t *data, uint32_t len, uint8_t *sha256)
{
    struct bflb_device_s *sha = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);

    if (sha == NULL) {
        printf("[PSRAM] FAILED: sec_sha device not found\r\n");
        return -1;
    }

    bflb_group0_request_sha_access(sha);
    bflb_sha_init(sha, SHA_MODE_SHA256);
    bflb_sha256_start(sha, &psram_sha256_ctx);
    if (bflb_sha256_update(sha, &psram_sha256_ctx, data, len) != 0) {
        printf("[PSRAM] FAILED: sha256 update error\r\n");
        return -2;
    }
    bflb_sha256_finish(sha, &psram_sha256_ctx, sha256);

    return 0;
}

void psram_test_prepare(void)
{
    psram_test_header_t *hdr = psram_get_header();
    uint8_t *data = psram_test_buffer + sizeof(psram_test_header_t);
    uint32_t data_len = PSRAM_TEST_SIZE - sizeof(psram_test_header_t);
    uint32_t pattern_seed;
    int ret;

    if (!psram_header_is_valid(hdr, data_len)) {
        memset(hdr, 0, sizeof(*hdr));
    }

    pattern_seed = psram_next_pattern_seed(hdr);

    for (uint32_t i = 0; i < data_len; i++) {
        data[i] = psram_test_pattern_byte(i, pattern_seed);
    }

    bflb_l1c_dcache_clean_range((void *)data, data_len);

    ret = psram_calc_sha256(data, data_len, hdr->sha256);
    if (ret != 0) {
        printf("[PSRAM] FAILED: prepare SHA256 (ret=%d)\r\n", ret);
        return;
    }

    hdr->magic = PSRAM_TEST_MAGIC;
    hdr->version = PSRAM_TEST_VERSION;
    hdr->prepare_count++;
    hdr->pattern_seed = pattern_seed;
    hdr->data_len = data_len;

    printf("[PSRAM] Prepared %u bytes, seed=0x%08X, prepare_count=%u\r\n",
           data_len,
           pattern_seed,
           hdr->prepare_count);
    psram_dump_sha256("SHA256 after prepare: ", hdr->sha256);
}

int psram_test_verify(void)
{
    psram_test_header_t *hdr = psram_get_header();
    uint8_t calc_sha256[PSRAM_SHA256_SIZE];
    uint8_t *data = psram_test_buffer + sizeof(psram_test_header_t);
    uint32_t data_len = PSRAM_TEST_SIZE - sizeof(psram_test_header_t);
    int ret = 0;

    if (hdr->magic != PSRAM_TEST_MAGIC) {
        printf("[PSRAM] FAILED: Magic mismatch! Expected 0x%08X, got 0x%08X\r\n", PSRAM_TEST_MAGIC, hdr->magic);
        ret = -1;
    }

    if (hdr->version != PSRAM_TEST_VERSION) {
        printf("[PSRAM] FAILED: Version mismatch! Expected 0x%08X, got 0x%08X\r\n", PSRAM_TEST_VERSION, hdr->version);
        ret = -2;
    }

    if (hdr->data_len != data_len) {
        printf("[PSRAM] FAILED: Data length mismatch! Expected %u, got %u\r\n", data_len, hdr->data_len);
        ret = -3;
    }

    if (ret == 0) {
        bflb_l1c_dcache_invalidate_range((void *)data, hdr->data_len);

        ret = psram_calc_sha256(data, hdr->data_len, calc_sha256);
        if (ret != 0) {
            printf("[PSRAM] FAILED: calculate SHA256 during verify (ret=%d)\r\n", ret);
            return ret;
        }

        psram_dump_sha256("SHA256 during verify: ", calc_sha256);

        if (memcmp(hdr->sha256, calc_sha256, PSRAM_SHA256_SIZE) != 0) {
            printf("[PSRAM] FAILED: SHA256 mismatch during verify\r\n");
            psram_dump_sha256("Expected SHA256: ", hdr->sha256);
            printf("[PSRAM] First 16 bytes: ");
            for (uint32_t i = 0; i < 16 && i < hdr->data_len; i++) {
                printf("%02X ", data[i]);
            }
            printf("\r\n");
            ret = -4;
        }
    }

    if (ret == 0) {
        printf("[PSRAM] PASSED: %u bytes verified, seed=0x%08X, verify_count=%u\r\n",
               hdr->data_len,
               hdr->pattern_seed,
               hdr->verify_count + 1);
        hdr->verify_count++;
    } else {
        printf("[PSRAM] FAILED: Content corrupted during verify (ret=%d)\r\n", ret);
    }

    return ret;
}
