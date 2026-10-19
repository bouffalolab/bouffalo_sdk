#include <stddef.h>
#include <string.h>
#include "multi_bins.h"

#define MULTI_BIN_DESC_COUNT 16
#define MULTI_BIN_TABLE_OFFSET 512U

extern const uint8_t __multi_bins__[] __attribute__((weak));

static const uint8_t *multi_bins_find(const char *name)
{
    const uint8_t *desc;
    char padded_name[8] = { 0 };
    size_t name_len;

    if (name == NULL || __multi_bins__ == NULL) {
        return NULL;
    }
    name_len = strlen(name);
    if (name_len == 0 || name_len > sizeof(padded_name)) {
        return NULL;
    }
    memcpy(padded_name, name, name_len);

    for (int i = 0; i < MULTI_BIN_DESC_COUNT; i++) {
        desc = __multi_bins__ + i * sizeof(multi_bin_desc_t);
        if (memcmp(desc, "\xff\xff\xff\xff\xff\xff\xff\xff", sizeof(padded_name)) == 0) {
            break;
        }
        if (memcmp(desc, padded_name, sizeof(padded_name)) == 0) {
            return desc;
        }
    }

    return NULL;
}

int multi_bins_get_desc(const char *name, multi_bin_desc_t *out)
{
    const uint8_t *desc;

    if (out == NULL) {
        return -1;
    }
    desc = multi_bins_find(name);
    if (desc == NULL) {
        return -1;
    }
    memcpy(out, desc, sizeof(*out));
    return 0;
}

const uint8_t *multi_bins_get_image_base(void)
{
    multi_bin_desc_t self;

    if (multi_bins_get_desc("SELF", &self) < 0 || self.end_addr <= self.start_addr) {
        return NULL;
    }
#ifdef CONFIG_PSRAM_XIP
    if (self.vma_end <= self.vma_start || self.vma_start < self.start_addr) {
        return NULL;
    }
    return (const uint8_t *)(uintptr_t)(self.vma_start - self.start_addr);
#else
    return __multi_bins__ - MULTI_BIN_TABLE_OFFSET - self.start_addr;
#endif
}

static const uint8_t *multi_bins_get_addr(const char *name, int end)
{
    multi_bin_desc_t desc;

    if (multi_bins_get_desc(name, &desc) < 0 || desc.end_addr <= desc.start_addr) {
        return NULL;
    }

#ifdef CONFIG_PSRAM_XIP
    if (desc.vma_end > desc.vma_start) {
        return (const uint8_t *)(uintptr_t)(end ? desc.vma_end : desc.vma_start);
    }
    return NULL;
#else
    const uint8_t *image_base;

    image_base = multi_bins_get_image_base();
    if (image_base == NULL) {
        return NULL;
    }
    return image_base + (end ? desc.end_addr : desc.start_addr);
#endif
}

const uint8_t *multi_bins_get_start(const char *name)
{
    return multi_bins_get_addr(name, 0);
}

const uint8_t *multi_bins_get_end(const char *name)
{
    return multi_bins_get_addr(name, 1);
}

int multi_bins_get_free_vma_start_below(uint32_t *start, uint32_t limit)
{
    const multi_bin_desc_t *desc;
    const multi_bin_desc_t *self_desc;
    uint32_t max_end = 0;

    if (start == NULL) {
        return -1;
    }
    *start = 0;
    self_desc = (const multi_bin_desc_t *)__multi_bins__;
    if (self_desc == NULL || memcmp(self_desc->name, "SELF\0\0\0", 8) != 0) {
        return -1;
    }

    for (int i = 0; i < MULTI_BIN_DESC_COUNT; i++) {
        desc = (const multi_bin_desc_t *)(__multi_bins__ + i * sizeof(*desc));
        if (memcmp(desc->name, "\xff\xff\xff\xff\xff\xff\xff\xff", sizeof(desc->name)) == 0) {
            if (max_end == 0) {
                return -1;
            }
            *start = max_end;
            return 0;
        }
        if (desc->vma_end > desc->vma_start && desc->vma_end <= limit &&
            desc->vma_end > max_end) {
            max_end = desc->vma_end;
        }
    }

    return -1;
}

int multi_bins_get_free_vma_start(uint32_t *start)
{
    return multi_bins_get_free_vma_start_below(start, UINT32_MAX);
}
