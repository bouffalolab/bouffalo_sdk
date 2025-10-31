#ifndef __INCLUDE_NUTTX_CONFIG_H
#define __INCLUDE_NUTTX_CONFIG_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mem.h" 

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

#define lib_malloc(s)      kmalloc(s, MM_KERNEL)
#define lib_zalloc(s)      kmalloc(s, MM_KERNEL|MM_ZERO)
#define lib_realloc(p, s)  krealloc(p, s)
#define lib_memalign(p, s) kmemalign(p, s)
#define lib_free(p)        kfree(p)

#include "nuttx_limits.h"

#endif