#include "bflb_mtimer.h"
#include "bflb_sec_aes.h"
#include "board.h"

const uint8_t aes_256bit_key[32] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

uint8_t aes_iv[16] = { 0 };

ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_input_buf[16 * 1024];
ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_enc_buf[16 * 1024];
ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_dec_buf[16 * 1024];

void mbedtls_check(uint32_t j)
{
    for (uint32_t i = 0; i < 16 * j; i++) {
        if (aes_dec_buf[i] != aes_input_buf[i]) {
            printf("mbedtls test fail\r\n");
            while (1) {
            }
        }
    }
}

int main(void)
{
    uint64_t start_time;

    board_init();

    struct bflb_device_s *aes;

    aes = bflb_device_get_by_name("aes");

    bflb_group0_request_aes_access(aes);

    for (uint32_t i = 0; i < 16 * 1024; i++) {
        aes_input_buf[i] = i & 0xff;
    }

    bflb_aes_init(aes);

    bflb_mtimer_delay_ms(500);

    for (uint32_t i = 1; i <= 1024; i++) {
        printf("test aes ecb 128 \r\n");
        bflb_aes_set_mode(aes, AES_MODE_ECB);
        bflb_aes_setkey(aes, aes_256bit_key, 128);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_encrypt(aes, aes_input_buf, aes_iv, aes_enc_buf, 16 * i);
        printf("aes ecb 128 block:%d enc cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_decrypt(aes, aes_enc_buf, aes_iv, aes_dec_buf, 16 * i);
        printf("aes ecb 128 block:%d dec cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        mbedtls_check(i);
#if !defined(BL702L)
        printf("test aes ecb 256 \r\n");
        bflb_aes_set_mode(aes, AES_MODE_ECB);
        bflb_aes_setkey(aes, aes_256bit_key, 256);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_encrypt(aes, aes_input_buf, aes_iv, aes_enc_buf, 16 * i);
        printf("aes ecb 256 block:%d enc cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_decrypt(aes, aes_enc_buf, aes_iv, aes_dec_buf, 16 * i);
        printf("aes ecb 256 block:%d dec cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        mbedtls_check(i);
#endif
    }

    for (uint32_t i = 1; i <= 1024; i++) {
        printf("test aes cbc 128 \r\n");
        bflb_aes_set_mode(aes, AES_MODE_CBC);
        bflb_aes_setkey(aes, aes_256bit_key, 128);
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_encrypt(aes, aes_input_buf, aes_iv, aes_enc_buf, 16 * i);
        printf("aes cbc 128 block:%d enc cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_decrypt(aes, aes_enc_buf, aes_iv, aes_dec_buf, 16 * i);
        printf("aes cbc 128 block:%d dec cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        mbedtls_check(i);

#if !defined(BL702L)
        printf("test aes cbc 256 \r\n");
        memset(aes_iv, 0, 16);
        bflb_aes_set_mode(aes, AES_MODE_CBC);
        bflb_aes_setkey(aes, aes_256bit_key, 256);
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_encrypt(aes, aes_input_buf, aes_iv, aes_enc_buf, 16 * i);
        printf("aes cbc 256 block:%d enc cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_decrypt(aes, aes_enc_buf, aes_iv, aes_dec_buf, 16 * i);
        printf("aes cbc 256 block:%d dec cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        mbedtls_check(i);
#endif
    }

    for (uint32_t i = 1; i <= 1024; i++) {
        printf("test aes ctr 128 \r\n");
        bflb_aes_set_mode(aes, AES_MODE_CTR);
        bflb_aes_setkey(aes, aes_256bit_key, 128);
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_encrypt(aes, aes_input_buf, aes_iv, aes_enc_buf, 16 * i);
        printf("aes ctr 128 block:%d enc cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_decrypt(aes, aes_enc_buf, aes_iv, aes_dec_buf, 16 * i);
        printf("aes ctr 128 block:%d dec cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        mbedtls_check(i);

#if !defined(BL702L)
        printf("test aes ctr 256 \r\n");
        bflb_aes_set_mode(aes, AES_MODE_CTR);
        bflb_aes_setkey(aes, aes_256bit_key, 256);
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_encrypt(aes, aes_input_buf, aes_iv, aes_enc_buf, 16 * i);
        printf("aes ctr 256 block:%d enc cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_decrypt(aes, aes_enc_buf, aes_iv, aes_dec_buf, 16 * i);
        printf("aes ctr 256 block:%d dec cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        mbedtls_check(i);
#endif
    }

    for (uint32_t i = 1; i <= 1024; i++) {
        printf("test aes xts 128 \r\n");
        bflb_aes_set_mode(aes, AES_MODE_XTS);
        bflb_aes_setkey(aes, aes_256bit_key, 128);
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_encrypt(aes, aes_input_buf, aes_iv, aes_enc_buf, 16 * i);
        printf("aes xts 128 block:%d enc cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_decrypt(aes, aes_enc_buf, aes_iv, aes_dec_buf, 16 * i);
        printf("aes xts 128 block:%d dec cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        mbedtls_check(i);

#if !defined(BL702L)
        printf("test aes xts 256 \r\n");
        bflb_aes_set_mode(aes, AES_MODE_XTS);
        bflb_aes_setkey(aes, aes_256bit_key, 256);
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_encrypt(aes, aes_input_buf, aes_iv, aes_enc_buf, 16 * i);
        printf("aes xts 256 block:%d enc cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        memset(aes_iv, 0, 16);
        start_time = bflb_mtimer_get_time_us();
        bflb_aes_decrypt(aes, aes_enc_buf, aes_iv, aes_dec_buf, 16 * i);
        printf("aes xts 256 block:%d dec cost time:%d us\r\n", i, (uint32_t)(bflb_mtimer_get_time_us() - start_time));
        mbedtls_check(i);
#endif
    }

    printf("aes success\r\n");
    bflb_group0_release_aes_access(aes);
    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}