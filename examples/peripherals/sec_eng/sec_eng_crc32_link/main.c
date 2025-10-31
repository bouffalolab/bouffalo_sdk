#include "bflb_mtimer.h"
#include "bflb_sec_sha.h"
#include "bflb_l1c.h"
#include "board.h"

#define TEST_LEN (4)
#define CRC_LEN  (4)

/* high->low:00 01 02 03,  Calculated according to 0x00010203*/
ATTR_NOCACHE_RAM_SECTION static uint8_t crc32_test_buf[3][8] = {
    { 0x00, 0x01, 0x02, 0x03 },
    { 0x00, 0x01, 0x02, 0x03 },
    /* two block test */
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 },
};

static uint8_t crc32_test_sum[3][CRC_LEN] = {
    { 0x13, 0x86, 0xb9, 0x8b },
    { 0x2a, 0xc9, 0x6d, 0x6b },
    { 0x9f, 0x68, 0xaa, 0x88 }
};

static uint8_t crc32_test_buf_len[3] = { 4, 4, 8 };

uint8_t result[3][CRC_LEN] = { { 0 }, { 0 }, { 0 } };

ATTR_NOCACHE_RAM_SECTION static struct bflb_crc_link_s link_cfg1 = {
    .sha_mode = 0,              /* CRC32 do not care */
    .sha_hash_sel = 0,          /* New hash */
    .sha_int_clear = 0,         /* Not clear interrupt */
    .sha_int_set = 0,           /* Not set interrupt */
    .sha_mode_ext = 3,          /* CRC32 */
    .sha_msg_len = 1,           /* Number of 512-bit block */
    .sha_src_addr = 0x50020000, /* Message source address */
    .dout_inv = 1,              /* dout inv of CRC */
    .dout_ref = 1,              /* din ref of CRC */
    .din_ref = 1,               /* dout ref of CRC */
    .poly = 0x04C11DB7,         /* poly of CRC */
    .iv_and_hash = 0xffffffff   /* CRC hash and also used as iv in 1st block */
};

ATTR_NOCACHE_RAM_SECTION static struct bflb_crc_link_s link_cfg2 = {
    .sha_mode = 0,              /* CRC32 do not care */
    .sha_hash_sel = 0,          /* New hash */
    .sha_int_clear = 0,         /* Not clear interrupt */
    .sha_int_set = 0,           /* Not set interrupt */
    .sha_mode_ext = 3,          /* CRC32 */
    .sha_msg_len = 1,           /* Number of 512-bit block */
    .sha_src_addr = 0x50020000, /* Message source address */
    .dout_inv = 0,              /* dout inv of CRC */
    .dout_ref = 0,              /* din ref of CRC */
    .din_ref = 0,               /* dout ref of CRC */
    .poly = 0x04C11DB7,         /* poly of CRC */
    .iv_and_hash = 0xffffffff   /* CRC hash and also used as iv in 1st block */
};

ATTR_NOCACHE_RAM_SECTION static struct bflb_crc_link_s link_cfg3 = {
    .sha_mode = 0,              /* CRC32 do not care */
    .sha_hash_sel = 0,          /* New hash */
    .sha_int_clear = 0,         /* Not clear interrupt */
    .sha_int_set = 0,           /* Not set interrupt */
    .sha_mode_ext = 3,          /* CRC32 */
    .sha_msg_len = 1,           /* Number of 512-bit block */
    .sha_src_addr = 0x50020000, /* Message source address */
    .dout_inv = 1,              /* dout inv of CRC */
    .dout_ref = 1,              /* din ref of CRC */
    .din_ref = 1,               /* dout ref of CRC */
    .poly = 0x04C11DB7,         /* poly of CRC */
    .iv_and_hash = 0xffffffff   /* CRC hash and also used as iv in 1st block */
};

ATTR_NOCACHE_RAM_SECTION static struct bflb_crc_link_s link_cfg4 = {
    .sha_mode = 0,              /* CRC32 do not care */
    .sha_hash_sel = 0,          /* New hash */
    .sha_int_clear = 0,         /* Not clear interrupt */
    .sha_int_set = 0,           /* Not set interrupt */
    .sha_mode_ext = 3,          /* CRC32 */
    .sha_msg_len = 1,           /* Number of 512-bit block */
    .sha_src_addr = 0x50020000, /* Message source address */
    .dout_inv = 1,              /* dout inv of CRC */
    .dout_ref = 1,              /* din ref of CRC */
    .din_ref = 1,               /* dout ref of CRC */
    .poly = 0x04C11DB7,         /* poly of CRC */
    .iv_and_hash = 0xffffffff   /* CRC hash and also used as iv in 1st block */
};

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

    printf("Sec Eng Case: crc32 run in link mode\r\n");

    struct bflb_device_s *sha;

    sha = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);

    bflb_group0_request_sha_access(sha);

    bflb_sha_link_init(sha);
    /* single block test msglen=1, shaHashSel=0 */
    bflb_sha_crc32_link_work(sha, (uint32_t)&link_cfg1, crc32_test_buf[0], crc32_test_buf_len[0], result[0]);
    bflb_data_compare(crc32_test_sum[0], result[0], CRC_LEN);
    printf("single block test msglen=1, shaHashSel=0 link_cfg1 success.\r\n");

    /* single block test msglen=1, shaHashSel=0 */
    bflb_sha_crc32_link_work(sha, (uint32_t)&link_cfg2, crc32_test_buf[1], crc32_test_buf_len[1], result[1]);
    bflb_data_compare(crc32_test_sum[1], result[1], CRC_LEN);
    printf("single block test msglen=1, shaHashSel=0 link_cfg2 success.\r\n");

    /* 2 blocks test, msglen=2, shaHashSel=0, calculation 2 blocks at a time */
    bflb_sha_crc32_link_work(sha, (uint32_t)&link_cfg3, crc32_test_buf[2], crc32_test_buf_len[2], result[2]);
    bflb_data_compare(crc32_test_sum[2], result[2], CRC_LEN);
    printf("2 blocks test, msglen=2, shaHashSel=0, calculation 2 blocks at a time success.\r\n");

    /* 2 blocks test, msglen=1, Start with the second block, shaHashSel=1, calculation 1 block at a time */
    bflb_sha_crc32_link_work(sha, (uint32_t)&link_cfg4, crc32_test_buf[2], 4, result[2]);
    bflb_data_compare(crc32_test_sum[0], result[2], CRC_LEN);
    printf("2 blocks test, msglen=1, Start with the second block, shaHashSel=1, calculation 1 block at a time success.\r\n");

    link_cfg4.sha_hash_sel = 1;
    bflb_sha_crc32_link_work(sha, (uint32_t)&link_cfg4, crc32_test_buf[2] + 4, 4, result[2]);
    bflb_data_compare(crc32_test_sum[2], result[2], CRC_LEN);
    printf("link_cfg4.sha_hash_sel = 1 success.\r\n");

    bflb_sha_link_deinit(sha);
    bflb_group0_release_sha_access(sha);
    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}