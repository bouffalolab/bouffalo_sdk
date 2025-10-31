#include "bflb_mtimer.h"
#include "bflb_sec_gmac.h"
#include "bflb_l1c.h"
#include "board.h"

#define TEXT_LEN 48

ATTR_NOCACHE_RAM_SECTION static uint8_t gmac_test_pt[TEXT_LEN] = {
    0x78, 0xfe, 0xb2, 0x71, 0xb9, 0xc2, 0x28, 0xf3,
    0x92, 0xa3, 0xb6, 0x60, 0xce, 0xda, 0x88, 0x03,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};

ATTR_NOCACHE_RAM_SECTION static uint8_t gmac_test_result[TEXT_LEN] = {
    0xb7, 0xde, 0x53, 0x53, 0x68, 0xb0, 0x85, 0x2c,
    0x88, 0x62, 0x70, 0x91, 0x46, 0xc7, 0x2e, 0x5e,
    0x85, 0xf8, 0xb0, 0xb6, 0xe5, 0x7a, 0x45, 0xc3,
    0xdc, 0x23, 0x92, 0xd6, 0x1a, 0xbb, 0x8c, 0xf3,
    0x56, 0xa9, 0xbc, 0x95, 0x1e, 0xa6, 0x4f, 0xf7,
    0xa4, 0x37, 0xbc, 0xe5, 0x33, 0x9a, 0x3c, 0x86
};

ATTR_NOCACHE_RAM_SECTION static struct bflb_sec_gmac_link_s link_cfg1 = {
    .gmac_int_clear = 0,     /* Not clear interrupt */
    .gmac_int_set = 0,       /* Not set interrupt */
    .gmac_msg_len = 1,       /* Number of 128-bit block */
    .gmac_src_addr = 0,      /* Message source address */
    .gmac_key0 = 0xca342b2e, /* GMAC key */
    .gmac_key1 = 0x884cfa59, /* GMAC key */
    .gmac_key2 = 0xef8a2c3b, /* GMAC key */
    .gmac_key3 = 0x66e94bd4, /* GMAC key */
    { 0 }                    /* Result of GMAC */
};

ATTR_NOCACHE_RAM_SECTION static struct bflb_sec_gmac_link_s link_cfg2 = {
    .gmac_int_clear = 0,     /* Not clear interrupt */
    .gmac_int_set = 0,       /* Not set interrupt */
    .gmac_msg_len = 1,       /* Number of 128-bit block */
    .gmac_src_addr = 0,      /* Message source address */
    .gmac_key0 = 0xca342b2e, /* GMAC key */
    .gmac_key1 = 0x884cfa59, /* GMAC key */
    .gmac_key2 = 0xef8a2c3b, /* GMAC key */
    .gmac_key3 = 0x66e94bd4, /* GMAC key */
    { 0 }                    /* Result of GMAC */
};

ATTR_NOCACHE_RAM_SECTION struct bflb_sec_gmac_link_s link_cfg3 = {
    .gmac_int_clear = 0,     /* Not clear interrupt */
    .gmac_int_set = 0,       /* Not set interrupt */
    .gmac_msg_len = 1,       /* Number of 128-bit block */
    .gmac_src_addr = 0,      /* Message source address */
    .gmac_key0 = 0xca342b2e, /* GMAC key */
    .gmac_key1 = 0x884cfa59, /* GMAC key */
    .gmac_key2 = 0xef8a2c3b, /* GMAC key */
    .gmac_key3 = 0x66e94bd4, /* GMAC key */
    { 0 }                    /* Result of GMAC */
};

static uint32_t
bflb_data_compare(const uint8_t *expected, uint8_t *input, uint32_t len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        if (input[i] != expected[i]) {
            printf("Compare fail at %d,input %02x, but expect %02x\r\n", i, input[i], expected[i]);
            printf("Expected values:\r\n");
            for (int j = 0; j < len; j++) {
                printf("%02x ", expected[j]);
            }
            printf("\r\nInput values:\r\n");
            for (int j = 0; j < len; j++) {
                printf("%02x ", input[j]);
            }
            printf("\r\n");
            while (1) {
            }
        }
    }

    return 0;
}

int main(void)
{
    uint8_t result[16] = { 0 };
    board_init();

    printf("Sec Eng Case: GMAC run in link mode\r\n");

    struct bflb_device_s *gmac;
    gmac = bflb_device_get_by_name(BFLB_NAME_SEC_GMAC);

    bflb_group0_request_gmac_access(gmac);

    bflb_sec_gmac_le_enable(gmac);
    /* Enable gmac link mode */
    bflb_sec_gmac_link_enable(gmac, 1);

    /* Deal with first link address, the first 16 bytes encrypt */
    bflb_sec_gmac_link_work(gmac, (uint32_t)&link_cfg1, gmac_test_pt, 16, result);
    bflb_data_compare(gmac_test_result, result, 16);
    printf("Deal with the first link address link_cfg1, the 16 bytes encrypt success\r\n\r\n");

    /* Deal with second link address,encrypt 32 bytes */
    bflb_sec_gmac_link_work(gmac, (uint32_t)&link_cfg2, gmac_test_pt, 32, result);
    bflb_data_compare(gmac_test_result + 16, result, 16);
    printf("Deal with the second link address link_cfg2, the 32 bytes success\r\n\r\n");

    /* Deal with third link address,encrypt 48 bytes */
    bflb_sec_gmac_link_work(gmac, (uint32_t)&link_cfg3, gmac_test_pt, TEXT_LEN, result);
    bflb_data_compare(gmac_test_result + 32, result, 16);
    printf("Deal with the third link address link_cfg3, the 48 bytes success\r\n\r\n");

    /* Deal with first link address,encrypt the remaining 32 bytes */
    bflb_sec_gmac_link_work(gmac, (uint32_t)&link_cfg1, gmac_test_pt + 16, TEXT_LEN - 16, result);
    bflb_data_compare(gmac_test_result + 32, result, 16);
    printf("Deal with the first link address link_cfg1, the remaining 32 bytes success\r\n\r\n");

    /* Deal with second link address,encrypt the remaining 16 bytes */
    bflb_sec_gmac_link_work(gmac, (uint32_t)&link_cfg2, gmac_test_pt + 32, TEXT_LEN - 32, result);
    bflb_data_compare(gmac_test_result + 32, result, 16);
    printf("Deal with the second link address link_cfg2, the remaining 16 bytes success\r\n\r\n");

    bflb_sec_gmac_link_enable(gmac, 0);

    bflb_group0_release_gmac_access(gmac);
    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}
