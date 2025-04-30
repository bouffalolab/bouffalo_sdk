#ifndef __COREDUMP_H__
#define __COREDUMP_H__

struct dump_section {
    uint32_t addr;
    uint32_t len;
};
extern __attribute__((weak)) struct dump_section _dump_sections;

void coredump_print(uintptr_t addr, uintptr_t lma_addr, size_t len, const char *desc);
void bl_coredump_run();

void core_partition_init(uint32_t flash_addr, size_t flash_size);
int coredump_xip_flash_write(uint32_t lma, uint8_t *lma_xip, size_t len);
void core_bin_start_hook(uint32_t *lma, size_t len);
void core_bin_sections_hook(uint32_t *lma, uint8_t *lma_xip, size_t len);
void core_bin_end_hook(uint32_t header_lma);

#endif /* __COREDUMP_H__ */
