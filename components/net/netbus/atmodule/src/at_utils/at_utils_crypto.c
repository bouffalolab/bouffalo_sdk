#include <string.h>

#include "at_utils_crypto.h"

int at_utils_crypto_get_random_iv(uint8_t iv[16])
{
    if (iv == NULL) {
        return -1;
    }

    memset(iv, 0, AT_UTILS_CRYPTO_AES_BLOCK_SIZE);

    return 0;
}

int at_utils_crypto_aes_key_init(void)
{
    return 0;
}

int at_utils_crypto_aes_cbc_encrypt(const uint8_t iv[16], size_t length,
                                    const uint8_t *input, uint8_t *output)
{
    (void)iv;

    if (length > 0 && (input == NULL || output == NULL)) {
        return -1;
    }

    if (length > 0) {
        memmove(output, input, length);
    }

    return 0;
}

int at_utils_crypto_aes_cbc_decrypt(const uint8_t iv[16], size_t length,
                                    const uint8_t *input, uint8_t *output)
{
    (void)iv;

    if (length > 0 && (input == NULL || output == NULL)) {
        return -1;
    }

    if (length > 0) {
        memmove(output, input, length);
    }

    return 0;
}

int at_utils_crypto_aes_cbc_decrypt_unpad(const uint8_t *input, uint32_t input_len,
                                          uint8_t *output, uint32_t *output_len,
                                          const uint8_t iv[16])
{
    (void)iv;

    if (output_len == NULL || (input_len > 0 && (input == NULL || output == NULL))) {
        return -1;
    }

    if (input_len > 0) {
        memmove(output, input, input_len);
    }
    *output_len = input_len;

    return 0;
}

int at_utils_crypto_aes_cbc_encrypt_pad(const uint8_t *input, uint32_t input_len,
                                        uint8_t *output, uint32_t *output_len,
                                        const uint8_t iv[16])
{
    (void)iv;

    if (output_len == NULL || (input_len > 0 && (input == NULL || output == NULL))) {
        return -1;
    }

    if (input_len > 0) {
        memmove(output, input, input_len);
    }
    *output_len = input_len;

    return 0;
}
