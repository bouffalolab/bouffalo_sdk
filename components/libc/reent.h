#if defined(__PICOLIBC__)
#include <reent_compat.h>
#else
#include_next <reent.h>
#endif
