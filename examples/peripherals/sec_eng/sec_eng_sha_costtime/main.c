#include "bflb_mtimer.h"
#include "bflb_sec_sha.h"
#include "board.h"

ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(32))) uint8_t sha_input_buf[16 * 1024]; /* input addr must be align 32 */

#define SHA_COSTTIME_MAX_BLOCK 1024U

uint8_t sha_output_buf[128];

ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha1_ctx_s ctx_sha1;
ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha256_ctx_s ctx_sha256;
ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha512_ctx_s ctx_sha512;

static void emit_done_marker(const char *marker)
{
    for (uint32_t i = 0; i < 5; i++) {
        printf("%s\r\n", marker);
        bflb_mtimer_delay_ms(20);
    }
}

int main(void)
{
    uint64_t start_time;

    board_init();

    struct bflb_device_s *sha;

    sha = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);

    bflb_group0_request_sha_access(sha);

    for (uint32_t i = 0; i < 16 * 1024; i++) {
        sha_input_buf[i] = i & 0xff;
    }

    bflb_sha_init(sha, SHA_MODE_SHA1);

    for (uint32_t i = 1; i <= SHA_COSTTIME_MAX_BLOCK; i <<= 1) {
        start_time = bflb_mtimer_get_time_us();
        bflb_sha1_start(sha, &ctx_sha1);
        bflb_sha1_update(sha, &ctx_sha1, sha_input_buf, 16 * i);
        bflb_sha1_finish(sha, &ctx_sha1, sha_output_buf);
        printf("sha1 block:%d cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
    }

    printf("sha1 success\r\n");

    bflb_sha_init(sha, SHA_MODE_SHA224);
    for (uint32_t i = 1; i <= SHA_COSTTIME_MAX_BLOCK; i <<= 1) {
        start_time = bflb_mtimer_get_time_us();
        bflb_sha256_start(sha, &ctx_sha256);
        bflb_sha256_update(sha, &ctx_sha256, sha_input_buf, 16 * i);
        bflb_sha256_finish(sha, &ctx_sha256, sha_output_buf);
        printf("sha224 block:%d cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
    }
    printf("sha224 success\r\n");

    bflb_sha_init(sha, SHA_MODE_SHA256);
    for (uint32_t i = 1; i <= SHA_COSTTIME_MAX_BLOCK; i <<= 1) {
        start_time = bflb_mtimer_get_time_us();
        bflb_sha256_start(sha, &ctx_sha256);
        bflb_sha256_update(sha, &ctx_sha256, sha_input_buf, 16 * i);
        bflb_sha256_finish(sha, &ctx_sha256, sha_output_buf);
        printf("sha256 block:%d cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
    }
    printf("sha256 success\r\n");

#if !defined(BL702L)
    bflb_sha_init(sha, SHA_MODE_SHA384);
    for (uint32_t i = 1; i <= SHA_COSTTIME_MAX_BLOCK; i <<= 1) {
        start_time = bflb_mtimer_get_time_us();
        bflb_sha512_start(sha, &ctx_sha512);
        bflb_sha512_update(sha, &ctx_sha512, sha_input_buf, 16 * i);
        bflb_sha512_finish(sha, &ctx_sha512, sha_output_buf);
        printf("sha384 block:%d cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
    }
    printf("sha384 success\r\n");

#if !defined(BL602) && !defined(BL702)
    bflb_sha_init(sha, SHA_MODE_SHA512);
    for (uint32_t i = 1; i <= SHA_COSTTIME_MAX_BLOCK; i <<= 1) {
        start_time = bflb_mtimer_get_time_us();
        bflb_sha512_start(sha, &ctx_sha512);
        bflb_sha512_update(sha, &ctx_sha512, sha_input_buf, 16 * i);
        bflb_sha512_finish(sha, &ctx_sha512, sha_output_buf);
        printf("sha512 block:%d cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
    }
    printf("sha512 success\r\n");
#endif
#endif

    printf("sha success\r\n");
    emit_done_marker("sec_eng_sha_costtime done");
    bflb_group0_release_sha_access(sha);
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
