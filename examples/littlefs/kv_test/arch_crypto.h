#ifndef _ARCH_CRYPTO_H_
#define _ARCH_CRYPTO_H_

#include <stdint.h>

void arch_md5(uint8_t *input, int len, uint8_t hash[16]);

#endif /* _ARCH_CRYPTO_H_ */
