#ifndef GCM_HW_PREFETCH_H
#define GCM_HW_PREFETCH_H

#include "mbedtls/aes.h"

/* Encrypt one GCM counter block, prefetching following blocks when possible. */
int gcm_hw_prefetch_encrypt_block(mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16]);

#endif
