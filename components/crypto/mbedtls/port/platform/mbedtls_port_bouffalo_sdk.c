#include "mbedtls_port_bouffalo_sdk.h"

#include <string.h>
#include <stdlib.h>

#ifdef CONFIG_FREERTOS
#include <FreeRTOS.h>

void *mbedtls_port_calloc( size_t nmemb, size_t size )
{
    const size_t sz = nmemb * size;
    void *m = malloc( sz );
    if ( m )
        memset( m, 0, sz );
    return( m );
}

void mbedtls_port_free( void * ptr ) {
    free( ptr );
}
#else
void *mbedtls_port_calloc( size_t nmemb, size_t size )
{
    while(1);
}

void mbedtls_port_free( void * ptr ) {
    while(1);
}
#endif
