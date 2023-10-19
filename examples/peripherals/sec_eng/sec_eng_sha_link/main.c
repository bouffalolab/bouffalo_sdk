#include "bflb_mtimer.h"
#include "bflb_sec_sha.h"
#include "bflb_l1c.h"
#include "board.h"

const uint8_t sha1_testbuf[2][104 + 1] = {
    { "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" },
    { "zyxwvutsrqponmlkjihgfedcbazyxwvutsrqponmlkjihgfedcbazyxwvutsrqponmlkjihgfedcbazyxwvutsrqponmlkjihgfedcba" }
};

const uint8_t sha1_testsum[2][20] = {
    { 0xd8, 0xcc, 0x4f, 0xd0, 0xa5, 0x7d, 0x0e, 0x0e,
      0x96, 0xcd, 0xb3, 0xe7, 0x41, 0x64, 0xf7, 0x34,
      0xc5, 0x93, 0xed, 0x65 },
    { 0x3a, 0x46, 0x88, 0xd3, 0xc3, 0xf9, 0xfe, 0xe8,
      0x59, 0xcb, 0xdd, 0x52, 0x11, 0x5a, 0x32, 0x56,
      0x4f, 0x2c, 0x43, 0x6d }
};

const uint8_t sha256_testbuf[2][104 + 1] = {
    { "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" },
    { "zyxwvutsrqponmlkjihgfedcbazyxwvutsrqponmlkjihgfedcbazyxwvutsrqponmlkjihgfedcbazyxwvutsrqponmlkjihgfedcba" }
};

const uint8_t sha256_testsum[2][32] = {
    { 0x94, 0x1a, 0xc3, 0x78, 0x68, 0x2e, 0x3d, 0xc6,
      0x62, 0x75, 0xdd, 0x49, 0xd5, 0xfb, 0x09, 0x97,
      0x87, 0x54, 0xec, 0xf4, 0x23, 0x1d, 0x18, 0xd3,
      0x03, 0x26, 0xfa, 0x51, 0x96, 0x26, 0x48, 0xec },
    { 0x0a, 0x9e, 0xfa, 0x03, 0x71, 0xcb, 0xf6, 0x78,
      0xe2, 0xd9, 0x59, 0x90, 0xa0, 0x22, 0x29, 0xe7,
      0x00, 0x78, 0x8d, 0x4a, 0x55, 0xc7, 0x8c, 0x46,
      0xab, 0xac, 0xe1, 0x40, 0x27, 0x6b, 0x7e, 0x5f }
};

const uint8_t sha512_testbuf[2][208 + 1] = {
    { "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" },
    { "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" }
};

const uint8_t sha512_testsum[2][128] = {
    { 0xa8, 0xf0, 0x5c, 0x44, 0x2c, 0x9f, 0xe7, 0xad,
      0xdb, 0x7b, 0xec, 0xc4, 0x13, 0xe3, 0x5b, 0xf4,
      0x81, 0x86, 0x5c, 0x8a, 0xd7, 0x85, 0xb0, 0xd0,
      0x17, 0x60, 0x40, 0x22, 0xca, 0xfd, 0xd1, 0x4e,
      0xb5, 0x82, 0xb8, 0xa0, 0xa6, 0x78, 0x59, 0x72,
      0xc9, 0xf9, 0x9c, 0x31, 0x9c, 0x7b, 0xf0, 0x27,
      0xb6, 0x28, 0x16, 0x7a, 0x0b, 0xad, 0xf6, 0xd5,
      0x84, 0x21, 0xc5, 0x75, 0x9c, 0x85, 0xc3, 0x37 },

    { 0xa8, 0xf0, 0x5c, 0x44, 0x2c, 0x9f, 0xe7, 0xad,
      0xdb, 0x7b, 0xec, 0xc4, 0x13, 0xe3, 0x5b, 0xf4,
      0x81, 0x86, 0x5c, 0x8a, 0xd7, 0x85, 0xb0, 0xd0,
      0x17, 0x60, 0x40, 0x22, 0xca, 0xfd, 0xd1, 0x4e,
      0xb5, 0x82, 0xb8, 0xa0, 0xa6, 0x78, 0x59, 0x72,
      0xc9, 0xf9, 0x9c, 0x31, 0x9c, 0x7b, 0xf0, 0x27,
      0xb6, 0x28, 0x16, 0x7a, 0x0b, 0xad, 0xf6, 0xd5,
      0x84, 0x21, 0xc5, 0x75, 0x9c, 0x85, 0xc3, 0x37 }
};

uint8_t sha_output_buf[128];

ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha1_link_ctx_s ctx_sha1;
ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha256_link_ctx_s ctx_sha256;
ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha512_link_ctx_s ctx_sha512;

static uint32_t bflb_data_compare(const uint8_t *expected, uint8_t *input, uint32_t len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        if (input[i] != expected[i]) {
            printf("Compare fail at %d,input %02x, but expect %02x\r\n", i, input[i], expected[i]);
            while (1) {
            }
        }
    }

    return 0;
}

int main(void)
{
    board_init();

    struct bflb_device_s *sha;

    sha = bflb_device_get_by_name("sha");

    bflb_group0_request_sha_access(sha);

    bflb_sha_link_init(sha);

    bflb_sha1_link_start(sha, &ctx_sha1);
    bflb_l1c_dcache_clean_range((void *)sha1_testbuf[0], 104);
    bflb_sha1_link_update(sha, &ctx_sha1, sha1_testbuf[0], 104);
    bflb_sha1_link_finish(sha, &ctx_sha1, sha_output_buf);
    bflb_data_compare(sha1_testsum[0], sha_output_buf, 20);
    printf("sha1 link success\r\n");

    bflb_sha256_link_start(sha, &ctx_sha256, 0);
    bflb_l1c_dcache_clean_range((void *)sha256_testbuf[0], 104);
    bflb_sha256_link_update(sha, &ctx_sha256, sha256_testbuf[0], 104);
    bflb_sha256_link_finish(sha, &ctx_sha256, sha_output_buf);
    bflb_data_compare(sha256_testsum[0], sha_output_buf, 32);
    printf("sha256 link success\r\n");

#if !defined(BL702L)
    bflb_sha512_link_start(sha, &ctx_sha512, 0);
    bflb_l1c_dcache_clean_range((void *)sha512_testbuf[0], 208);
    bflb_sha512_link_update(sha, &ctx_sha512, sha512_testbuf[0], 208);
    bflb_sha512_link_finish(sha, &ctx_sha512, sha_output_buf);
    bflb_data_compare(sha512_testsum[0], sha_output_buf, 64);
    printf("sha512 link success\r\n");
#endif
    bflb_group0_release_sha_access(sha);
    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}