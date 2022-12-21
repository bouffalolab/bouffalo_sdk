#include "bflb_flash.h"
#include "bflb_sec_aes.h"
#include "board.h"

uint8_t aes_ctr_128bit_key[16] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

uint8_t aes_ctr_128bit_iv[16] = {
    0x00, 0x6C, 0xB6, 0xDB, 0xC0, 0x54, 0x3B, 0x59,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define AES_DATA_LEN 4096

ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_ctr_pt_buffer[AES_DATA_LEN];
ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_ctr_ct_buffer[AES_DATA_LEN];
ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_ctr_tmp_buffer[AES_DATA_LEN];
ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_ctr_tmp_buffer2[AES_DATA_LEN];

static void bflb_data_compare(const uint8_t *expected, uint8_t *input, uint32_t len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        if (input[i] != expected[i]) {
            printf("Compare fail at %d,input %02x, but expect %02x\r\n", i, input[i], expected[i]);
            while (1) {
            }
        }
    }
}

/**
 *  This demo tests two decryption ways.
 *  One is decrypted by aes module, another is decrypted by flash aes module.
 */

int main(void)
{
    board_init();

    struct bflb_device_s *aes;

    for (uint32_t i = 0; i < AES_DATA_LEN; i++) {
        aes_ctr_pt_buffer[i] = i & 0xff;
    }

    aes = bflb_device_get_by_name("aes");

    bflb_group0_request_aes_access(aes);

    bflb_aes_init(aes);
    bflb_aes_set_mode(aes, AES_MODE_CTR);
    bflb_aes_setkey(aes, aes_ctr_128bit_key, 128);
    bflb_aes_encrypt(aes, aes_ctr_pt_buffer, aes_ctr_128bit_iv, aes_ctr_ct_buffer, AES_DATA_LEN);
    bflb_aes_decrypt(aes, aes_ctr_ct_buffer, aes_ctr_128bit_iv, aes_ctr_tmp_buffer, AES_DATA_LEN);
    bflb_data_compare(aes_ctr_pt_buffer, aes_ctr_tmp_buffer, AES_DATA_LEN);

    printf("aes ctr128 encrypt & decrypt success\r\n");

    bflb_flash_aes_disable();

    /* erase 0x00010000 4k flash */
    bflb_flash_erase(0x00010000, 4096);

    memset(aes_ctr_tmp_buffer, 0, AES_DATA_LEN);
    /* write 0x00010000 flash data */
    bflb_flash_write(0x00010000, aes_ctr_ct_buffer, AES_DATA_LEN);
    bflb_flash_read(0x00010000, aes_ctr_tmp_buffer, AES_DATA_LEN);
    bflb_data_compare(aes_ctr_ct_buffer, aes_ctr_tmp_buffer, AES_DATA_LEN);
    printf("flash write & read success\r\n");

    struct bflb_flash_aes_config_s config;
    config.region = 0;
    config.region_enable = 1;
    config.lock_enable = 0;
    config.key = aes_ctr_128bit_key;
    config.keybits = FLASH_AES_KEY_128BITS;
    config.iv = aes_ctr_128bit_iv;
    config.start_addr = 64 * 1024; /* 0x00010000 */
    config.end_addr = 68 * 1024;   /* 0x00011000 */

    bflb_flash_aes_init(&config);
    bflb_flash_aes_enable();

    /* read 0x00010000 flash data */
    bflb_data_compare(aes_ctr_pt_buffer, (uint8_t *)(FLASH_XIP_BASE - bflb_flash_get_image_offset() + 0x00010000), AES_DATA_LEN);
    printf("flash decrypt with flash aes ctr128 success\r\n");

    bflb_flash_read(0x00010000, aes_ctr_tmp_buffer, AES_DATA_LEN);
    bflb_aes_decrypt(aes, &aes_ctr_tmp_buffer[0], aes_ctr_128bit_iv, &aes_ctr_tmp_buffer2[0], 1024);  /* set new iv */
    bflb_aes_decrypt(aes, &aes_ctr_tmp_buffer[1024], NULL, &aes_ctr_tmp_buffer2[1024], 1024);         /* use last iv */
    bflb_aes_decrypt(aes, &aes_ctr_tmp_buffer[1024 * 2], NULL, &aes_ctr_tmp_buffer2[1024 * 2], 1024); /* use last iv */
    bflb_aes_decrypt(aes, &aes_ctr_tmp_buffer[1024 * 3], NULL, &aes_ctr_tmp_buffer2[1024 * 3], 1024); /* use last iv */
    bflb_data_compare(aes_ctr_pt_buffer, aes_ctr_tmp_buffer2, AES_DATA_LEN);
    printf("flash decrypt with sec eng aes ctr128 success\r\n");

    printf("flash test success\r\n");
    while (1) {
    }
}