/*
 *   Copyright (c) 2014 - 2019 Oleh Kulykov <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#ifndef __RWS_MEMORY_H__
#define __RWS_MEMORY_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include "rws_common.h"

#if 1
// size > 0 => malloc
void * rws_malloc(const size_t size);

// size > 0 => malloc
void * rws_malloc_zero(const size_t size);
void rws_free(void * mem);
void rws_free_clean(void ** mem);
#else
#define rws_malloc(size) aos_malloc(size)
#define rws_malloc_zero(size) aos_zalloc(size)

#define rws_free(ptr) aos_free((void*)(ptr))
#define rws_free_clean(ptr) aos_freep((char**)(ptr))
#endif

#endif
