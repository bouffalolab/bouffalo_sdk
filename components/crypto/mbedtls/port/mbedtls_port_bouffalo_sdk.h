#ifndef MBEDTLS_PORT_PLATFORM_BOUFFALO_SDK
#define MBEDTLS_PORT_PLATFORM_BOUFFALO_SDK

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mem.h"

#define MBEDTLS_PLATFORM_STD_FREE kfree
#define MBEDTLS_PLATFORM_STD_CALLOC kcalloc

#define MBEDTLS_PLATFORM_FPRINTF_MACRO fprintf
#define MBEDTLS_PLATFORM_PRINTF_MACRO printf
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO snprintf
#define MBEDTLS_PLATFORM_EXIT_MACRO exit

#endif /* end of include guard: MBEDTLS_PORT_PLATFORM_BOUFFALO_SDK */
