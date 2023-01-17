#include "common.h"

#include <bflb_sec_trng.h>

#if defined(MBEDTLS_ENTROPY_C)

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    bflb_trng_readlen( output, len );
    *olen = len;

    return( 0 );
}
#endif

#endif /* MBEDTLS_ENTROPY_C */
