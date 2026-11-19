#ifndef CORE_PORTME_H
#define CORE_PORTME_H

#include <stddef.h>
#include <stdint.h>

#define HAS_FLOAT 0
#define HAS_TIME_H 0
#define USE_CLOCK 0
#define HAS_STDIO 0
#define HAS_PRINTF 0
#define ee_printf(...) ((void)0)

#define COMPILER_VERSION "GCC/Clang"
#define COMPILER_FLAGS "-O2"
#define MEM_LOCATION "Static"

typedef signed short ee_s16;
typedef unsigned short ee_u16;
typedef signed int ee_s32;
typedef double ee_f32;
typedef unsigned char ee_u8;
typedef unsigned int ee_u32;
typedef uintptr_t ee_ptr_int;
typedef size_t ee_size_t;
typedef ee_u32 CORE_TICKS;

#define align_mem(x) (void *)(4 + (((ee_ptr_int)(x)-1) & ~3))

#define SEED_METHOD SEED_ARG
#define MEM_METHOD MEM_STATIC
#define MULTITHREAD 1
#define USE_PTHREAD 0
#define USE_FORK 0
#define USE_SOCKET 0
#define MAIN_HAS_NOARGC 1
#define MAIN_HAS_NORETURN 0
#define CORE_DEBUG 0
#define COMPILER_REQUIRES_SORT_RETURN 0

#ifndef PERFORMANCE_RUN
#define PERFORMANCE_RUN 1
#endif
#ifndef VALIDATION_RUN
#define VALIDATION_RUN 0
#endif
#ifndef PROFILE_RUN
#define PROFILE_RUN 0
#endif

typedef struct CORE_PORTABLE_S {
    ee_u8 portable_id;
} core_portable;

#endif
