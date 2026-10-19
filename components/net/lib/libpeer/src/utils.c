#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/md.h"

struct UtilsRandom {
  mbedtls_ctr_drbg_context drbg;
  mbedtls_entropy_context entropy;
};

UtilsRandom* utils_random_create(void) {
  static const unsigned char personalization[] = "libpeer-owner-rng";
  UtilsRandom* random = malloc(sizeof(*random));
  if (!random) return NULL;
  mbedtls_ctr_drbg_init(&random->drbg);
  mbedtls_entropy_init(&random->entropy);
  if (mbedtls_ctr_drbg_seed(&random->drbg, mbedtls_entropy_func, &random->entropy,
                            personalization, sizeof(personalization) - 1) != 0) {
    utils_random_destroy(random);
    return NULL;
  }
  /* Keep entropy alive for the DRBG's automatic reseeding callback. */
  return random;
}

void utils_random_destroy(UtilsRandom* random) {
  if (!random) return;
  mbedtls_ctr_drbg_free(&random->drbg);
  mbedtls_entropy_free(&random->entropy);
  free(random);
}

int utils_random_get_bytes(UtilsRandom* random, unsigned char* output, size_t length) {
  if (!random || !output || !length) return -1;
  return mbedtls_ctr_drbg_random(&random->drbg, output, length);
}

int utils_random_get_string(UtilsRandom* random, char* output, int length) {
  static const char alphabet[] =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  unsigned char bytes[64];
  int written = 0;
  if (!output || length < 0) return -1;
  output[0] = '\0';
  if (!random) return -1;
  while (written < length) {
    size_t count = (size_t)(length - written);
    if (count > sizeof(bytes)) count = sizeof(bytes);
    if (utils_random_get_bytes(random, bytes, count) != 0) {
      memset(output, 0, (size_t)written + 1);
      return -1;
    }
    for (size_t i = 0; i < count; ++i) {
      if (bytes[i] < 248) output[written++] = alphabet[bytes[i] % 62];
    }
  }
  output[length] = '\0';
  return 0;
}

/* Legacy one-shot helpers for callers without an owner context. ICE reuses
 * Agent.random instead, including credentials and periodic transactions. */
int utils_random_bytes(unsigned char* output, size_t length) {
  UtilsRandom* random = utils_random_create();
  int ret = utils_random_get_bytes(random, output, length);
  utils_random_destroy(random);
  return ret;
}

int utils_random_string(char* output, int length) {
  UtilsRandom* random = utils_random_create();
  int ret = utils_random_get_string(random, output, length);
  utils_random_destroy(random);
  return ret;
}

void utils_get_hmac_sha1(const char* input, size_t input_len, const char* key, size_t key_len, unsigned char* output) {
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA1;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
  mbedtls_md_hmac_starts(&ctx, (const unsigned char*)key, key_len);
  mbedtls_md_hmac_update(&ctx, (const unsigned char*)input, input_len);
  mbedtls_md_hmac_finish(&ctx, output);
  mbedtls_md_free(&ctx);
}

void utils_get_md5(const char* input, size_t input_len, unsigned char* output) {
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_MD5;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char*)input, input_len);
  mbedtls_md_finish(&ctx, output);
  mbedtls_md_free(&ctx);
}
