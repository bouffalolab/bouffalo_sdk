/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-14 22:06:35
 * @LastEditTime: 2020-10-17 14:17:24
 * @Description: the code belongs to jiejie, please keep the author information
 * and source code according to the license.
 */
#ifndef _PLATFORM_MEMORY_H_
#define _PLATFORM_MEMORY_H_
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

#ifdef __cplusplus
extern "C" {
#endif

void* platform_memory_alloc(size_t size);
void* platform_memory_calloc(size_t num, size_t size);
void platform_memory_free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif
