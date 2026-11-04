#ifndef MBEDTLS_GCM_ALT_CONTEXT_H
#define MBEDTLS_GCM_ALT_CONTEXT_H

#include "mbedtls/private_access.h"

/* The backend owns a separately compiled upstream software GCM context. */
typedef struct mbedtls_gcm_context {
    void *MBEDTLS_PRIVATE(backend);
} mbedtls_gcm_context;

#endif
