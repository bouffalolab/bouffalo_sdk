#ifndef BFLB_TLSF_H
#define BFLB_TLSF_H

#include <stdint.h>
#include <string.h>

int bflb_mmheap_init(void *begin_addr, uint32_t size);
void *bflb_malloc(size_t nbytes);
void bflb_free(void *ptr);
void *bflb_realloc(void *ptr, size_t nbytes);
void *bflb_calloc(size_t count, size_t size);
void *bflb_malloc_align(size_t align, size_t size);

#endif