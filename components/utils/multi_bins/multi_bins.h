#ifndef MULTI_BINS_H
#define MULTI_BINS_H

#include <stdint.h>

typedef struct {
    char name[8];
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t vma_start;
    uint32_t vma_end;
} __attribute__((packed)) multi_bin_desc_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Return the first VMA address after all populated ranges. Returns 0 or -1. */
int multi_bins_get_free_vma_start(uint32_t *start);
/* Return the first free VMA below an exclusive partition limit. */
int multi_bins_get_free_vma_start_below(uint32_t *start, uint32_t limit);

/* Copy a descriptor by name. Returns 0 or -1. */
int multi_bins_get_desc(const char *name, multi_bin_desc_t *out);
/* Return the address used to translate file offsets into runtime source addresses. */
const uint8_t *multi_bins_get_image_base(void);

/* Return VMA addresses when populated, otherwise the mapped LMA addresses. */
const uint8_t *multi_bins_get_start(const char *name);
const uint8_t *multi_bins_get_end(const char *name);

#ifdef __cplusplus
}
#endif

#endif
