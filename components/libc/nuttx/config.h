#ifndef __INCLUDE_NUTTX_CONFIG_H
#define __INCLUDE_NUTTX_CONFIG_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FAR
#define CODE
#define set_errno(e)

/* Defined in lib_skipspace.c */

void lib_skipspace(FAR const char **pptr);

/* Defined in lib_isbasedigit.c */

bool lib_isbasedigit(int ch, int base, FAR int *value);

/* Defined in lib_checkbase.c */

int lib_checkbase(int base, FAR const char **pptr);

#if defined(CPU_D0)
#define CONFIG_HAVE_LONG_LONG
#else
#define CONFIG_ARCH_RV32IM
#endif

// #  define CONFIG_HAVE_FLOAT 1
// #  define CONFIG_HAVE_DOUBLE 1
// #  define CONFIG_HAVE_LONG_DOUBLE 1

#define lib_malloc(s)      malloc(s)
#define lib_zalloc(s)      zalloc(s)
#define lib_realloc(p, s)  realloc(p, s)
#define lib_memalign(p, s) memalign(p, s)
#define lib_free(p)        free(p)

#include "nuttx_limits.h"

#endif