#include "common.h"

#include "bflb_sec_trng.h"

#if defined(MBEDTLS_ENTROPY_C)

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    uintptr_t flag;
    flag = bflb_irq_save();
    bflb_trng_readlen( output, len );
    //memset(output, 0, len); /* test for ecp */
    *olen = len;
    bflb_irq_restore(flag);
    return( 0 );
}
#endif

#endif /* MBEDTLS_ENTROPY_C */
