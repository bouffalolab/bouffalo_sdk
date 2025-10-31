#include <stddef.h>
#include <stdint.h>
#include <coredump.h>

struct binfmt_header {
    uint8_t magic[4];
    uint32_t version;
    uint32_t segment_count;
};

struct binfmt_seg_desc {
    uint64_t addr;
    uint64_t length;
};

void core_bin_start_hook(uint32_t *lma, size_t len)
{
    int seg_cnt = 0;
    struct binfmt_header header = {.magic = {0xff, 0xff, 0xff, 0xff},           \
                                   .version = 0x1,                              \
                                   .segment_count = 0xffffffff};
    struct binfmt_seg_desc seg_desc;

    for (int i = 0; (&_dump_sections + i)->addr != 0xffffffff; i++) {
        if ((&_dump_sections + i)->addr == 0)
            break;
        if ((&_dump_sections + i)->len == 0)
            continue;
        seg_desc.addr = (uint64_t)(&_dump_sections + i)->addr;
        seg_desc.length = (uint64_t)(&_dump_sections + i)->len;
        coredump_xip_flash_write(*lma + sizeof(header) + seg_cnt*sizeof(seg_desc), (uint8_t *)&seg_desc, sizeof(seg_desc));
        seg_cnt++;
    }
    header.segment_count = seg_cnt;
    if(!coredump_xip_flash_write(*lma, (uint8_t *)&header, sizeof(header)))
        *lma += sizeof(header) + seg_cnt * sizeof(seg_desc);
}

void core_bin_sections_hook(uint32_t *lma, uint8_t *lma_xip, size_t len)
{
    if(!coredump_xip_flash_write(*lma, lma_xip, len))
        *lma += len;
}

void core_bin_end_hook(uint32_t header_lma)
{
    uint8_t magic[4] = {'B', 'K', 'C', 'D'};

    coredump_xip_flash_write(header_lma, magic, sizeof(magic));
}

