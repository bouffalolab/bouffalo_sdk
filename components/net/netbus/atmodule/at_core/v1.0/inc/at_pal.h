/* AT module memory wrappers and macro remaps */
#ifndef AT_MEM_H
#define AT_MEM_H

#include <stdint.h>
#include <stddef.h>

void *at_malloc(size_t size);
void *at_calloc(size_t count, size_t size);
void  at_free(void *ptr);

#endif /* AT_MEM_H */

