#include "arch_crypto.h"
#include "arch_dbg.h"

#include <mbedtls/aes.h>
#include <mbedtls/md5.h>
#include <stdint.h>

void arch_md5(uint8_t *input, int len, uint8_t hash[16])
{
  mbedtls_md5_context context;

  mbedtls_md5_init(&context);
  mbedtls_md5_starts(&context);
  mbedtls_md5_update(&context, input, len);
  mbedtls_md5_finish(&context, hash);
}
