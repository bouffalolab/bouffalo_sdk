#include <errno.h>

#if defined(BFLB_USE_PICOLIBC)
#undef errno
extern __thread int errno;
#endif

#ifndef CONFIG_FREERTOS
int *__errno(void)
{
#if defined(BFLB_USE_PICOLIBC)
    return &errno;
#else
    static int __errno_value = 0;
    return &__errno_value;
#endif
}
#endif
