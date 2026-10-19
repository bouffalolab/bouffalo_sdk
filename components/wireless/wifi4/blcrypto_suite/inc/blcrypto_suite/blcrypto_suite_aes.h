#ifndef BFLB_CRYPTO_SUITE_AES_H
#define BFLB_CRYPTO_SUITE_AES_H

#include <stdint.h>

struct bflb_crypto_suite_aes;

enum crypto_aes_mode {
    AES_ENC,
    AES_DEC,
};

struct bflb_crypto_suite_aes *bflb_crypto_suite_aes_init();
void bflb_crypto_suite_aes_deinit(struct bflb_crypto_suite_aes **aes);
int bflb_crypto_suite_aes_set_key(struct bflb_crypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *key, int bits);
int bflb_crypto_suite_aes_crypt(struct bflb_crypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *in, uint8_t *out);

#endif /* end of include guard: BFLB_CRYPTO_SUITE_AES_H */
