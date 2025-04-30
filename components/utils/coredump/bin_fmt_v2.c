#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <coredump.h>

typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_Phdr;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
} Elf32_Shdr;

static Elf32_Ehdr elf_header;
static uint32_t flash_addr_start;

#define SYMBOL_TABLE "section"
#define SYMBOL_TABLE_SIZE 8

void create_elf_header(Elf32_Ehdr *header, uint32_t section_num, uint32_t section_size)
{
    memset(&elf_header, 0, sizeof(elf_header));
    header->e_ident[0] = 0x7f;
    header->e_ident[1] = 'E';
    header->e_ident[2] = 'L';
    header->e_ident[3] = 'F';
    header->e_ident[4] = '\01';
    header->e_ident[5] = '\01';
    header->e_ident[6] = '\01';
    header->e_type = 2;
    header->e_machine = 0xf3;
    header->e_version = 1;
    header->e_phoff = sizeof(Elf32_Ehdr);
    header->e_phentsize = sizeof(Elf32_Phdr);
    header->e_phnum = section_num;
    header->e_shoff = sizeof(Elf32_Ehdr) + section_num * sizeof(Elf32_Phdr) + SYMBOL_TABLE_SIZE + section_size; // 8 is "section" size.
    header->e_shentsize = sizeof(Elf32_Shdr);
    header->e_shnum = section_num + 2;
    header->e_shstrndx = 1; // fix str table index 1.
}

void create_elf_section(Elf32_Phdr *prog_hdr, Elf32_Shdr *sec_hdr, uint32_t vma, size_t len, size_t offset)
{
    memset(prog_hdr, 0, sizeof(*prog_hdr));
    prog_hdr->p_type = 1;
    prog_hdr->p_offset = elf_header.e_phoff + elf_header.e_phnum * elf_header.e_phentsize + SYMBOL_TABLE_SIZE + offset;
    prog_hdr->p_vaddr = vma;
    prog_hdr->p_paddr = vma;
    prog_hdr->p_filesz = len;
    prog_hdr->p_memsz = len;
    prog_hdr->p_flags = 5;
    prog_hdr->p_align = 0x1;

    memset(sec_hdr, 0, sizeof(*sec_hdr));
    sec_hdr->sh_name = 0;
    sec_hdr->sh_type = 1;
    sec_hdr->sh_flags = 6;
    sec_hdr->sh_addr = vma;
    sec_hdr->sh_offset = elf_header.e_phoff + elf_header.e_phnum * elf_header.e_phentsize + SYMBOL_TABLE_SIZE + offset;
    sec_hdr->sh_size = len;
    sec_hdr->sh_link = 1;
    sec_hdr->sh_addralign = 0x1;
}

void core_bin_start_hook(uint32_t *lma, size_t len)
{
    int seg_cnt = 0;
    uint32_t size = 0;
    flash_addr_start = *lma;

    for (int i = 0; (&_dump_sections + i)->addr != 0xffffffff; i++) {
        if ((&_dump_sections + i)->addr == 0)
            break;
        if ((&_dump_sections + i)->len == 0)
            continue;
        seg_cnt++;
        size += (&_dump_sections + i)->len;
    }

    create_elf_header(&elf_header, seg_cnt, size);

    *lma += elf_header.e_phoff + elf_header.e_phnum * elf_header.e_phentsize + SYMBOL_TABLE_SIZE;
}

void core_bin_sections_hook(uint32_t *lma, uint8_t *lma_xip, size_t len)
{
    Elf32_Shdr sec_hdr;
    Elf32_Phdr prog_hdr;
    static size_t offset_len = 0;
    static size_t seg_num = 0;

    create_elf_section(&prog_hdr, &sec_hdr, (uint32_t)lma_xip, len, offset_len);

    coredump_xip_flash_write(flash_addr_start + elf_header.e_phoff + seg_num * elf_header.e_phentsize, (uint8_t *)&prog_hdr, elf_header.e_phentsize);
    coredump_xip_flash_write(flash_addr_start + elf_header.e_shoff + (seg_num + 2) * elf_header.e_shentsize, (uint8_t *)&sec_hdr, elf_header.e_shentsize);
    offset_len += len;
    seg_num++;

    if(!coredump_xip_flash_write(*lma, lma_xip, len))
        *lma += len;
}

void core_bin_end_hook(uint32_t header_lma)
{
    Elf32_Shdr sec_hdr;

    coredump_xip_flash_write(header_lma, (uint8_t *)&elf_header, sizeof(Elf32_Ehdr));
    
    memset(&sec_hdr, 0, sizeof(sec_hdr));
    // write a NULL section
    coredump_xip_flash_write(header_lma + elf_header.e_shoff, (uint8_t *)&sec_hdr, elf_header.e_shentsize);
    sec_hdr.sh_name = 7;
    sec_hdr.sh_type = 3;
    sec_hdr.sh_offset = elf_header.e_phoff + elf_header.e_phnum * elf_header.e_phentsize;
    sec_hdr.sh_size = SYMBOL_TABLE_SIZE;
    sec_hdr.sh_addralign = 1;
    coredump_xip_flash_write(header_lma + elf_header.e_shoff + 1 * elf_header.e_shentsize, (uint8_t *)&sec_hdr, elf_header.e_shentsize);

    coredump_xip_flash_write(header_lma + elf_header.e_phoff + elf_header.e_phnum * elf_header.e_phentsize, (uint8_t *)SYMBOL_TABLE, SYMBOL_TABLE_SIZE);
}
