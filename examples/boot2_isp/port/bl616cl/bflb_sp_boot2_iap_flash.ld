/****************************************************************************************
* @file flash.ld
*
* @brief This file is the link script file (gnuarm or armgcc).
*
* Copyright (C) BouffaloLab 2021
*
****************************************************************************************
*/

/* configure the CPU type */
OUTPUT_ARCH( "riscv" )
/* link with the standard c library */
INPUT(-lc)
/* link with the standard GCC library */
INPUT(-lgcc)
/* configure the entry point */
ENTRY(__start)

StackSize    = 0x1000; /*  4KB */

VerOffset = 0xC00;
Boot2Size = 0xC000;
/* Not pass para to app since it will get itself */
PROVIDE(__boot2_pass_param_addr = 0);
MEMORY
{
    fw_header_memory  (rx)  : ORIGIN = 0x80000000 - 0x2000, LENGTH = 8K
    xip_memory  (rx)  : ORIGIN = 0x80000000, LENGTH = 3K
    ver_memory     (rx)  : ORIGIN = 0x80000000 + VerOffset, LENGTH = 256
    ram_psram  (wxa)   : ORIGIN = 0x88000000, LENGTH = 0K
    xip_memory1    (rx)  : ORIGIN = 0x80000d00, LENGTH = 44K + 768
    ram_memory     (!rx) : ORIGIN = 0x60FC0000, LENGTH = 224K
    nocache_memory (!rx) : ORIGIN = 0x20FC0000, LENGTH = 224K
}

SECTIONS
{

    PROVIDE(__metal_chicken_bit = 0);

    .fw_header :
    {
        KEEP(*(.fw_header))
    } > fw_header_memory

    .init :
    {
        __text_code_start__ = .;
        KEEP (*(SORT_NONE(.init)))
        KEEP (*(SORT_NONE(.vector)))
        *(.text.hal*)
    } > xip_memory

    .ver :
    {
		LONG(0x42464c42); /* BLFB */
		LONG(0x46524556); /* VERF */
		KEEP (*(.bflb_verinf))
		LONG(0x42464c42); /* BLFB */
		LONG(0x46524556); /* VERF */
		KEEP (*(.verinfo))
    } > ver_memory

    .text :
    {
        . = ALIGN(4); __mem_copy_sections = ABSOLUTE(.);
        LONG(__tcm_code_start__);   LONG(__tcm_code_end__);   LONG(__itcm_load_addr);
        LONG(__tcm_data_start__);   LONG(__tcm_data_end__);   LONG(__dtcm_load_addr);
        LONG(__ram_data_start__);   LONG(__ram_data_end__);   LONG(__ram_load_addr);
        LONG(__psram_data_start__); LONG(__psram_data_end__); LONG(__psram_load_addr);
        LONG(0xffffffff);           LONG(0xffffffff);         LONG(0xffffffff);

        . = ALIGN(4); __mem_setz_sections = ABSOLUTE(.);
        LONG(__bss_start__);        LONG(__bss_end__);
        LONG(__wifi_bss_start);     LONG(__wifi_bss_end);
        LONG(0xffffffff);           LONG(0xffffffff);

        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)

        . = ALIGN(4);
        __text_code_end__ = .;
    } > xip_memory1

    .build_id :
    {
        . = ALIGN(4);
        PROVIDE( elf_build_id = ABSOLUTE(.) + 16);
        *(.note.gnu.build-id)
        . = ALIGN(4);
        __build_id_end__ = .;
    } > xip_memory1

    __nocache_ram_load_addr = __build_id_end__;
    .nocache_ram_region : AT (__nocache_ram_load_addr)
    {
        . = ALIGN(4);
        __nocache_ram_data_start__ = .;
        __nocache_noinit_ram_data_start__ = .;
        *(.nocache_ram)
        . = ALIGN(4);
        __nocache_ram_data_end__ = .;
    } > nocache_memory

    .nocache_noinit_ram_region (NOLOAD) :
    {
        . = ALIGN(4);
        *(.nocache_noinit_ram)
        *(.noncacheable)
        . = ALIGN(4);
        __nocache_noinit_ram_data_end__ = .;
    } > nocache_memory

    /* Skip the nocache region in ram_memory */
    .ram_skip_nocache_region (NOLOAD) :
    {
        . += (__nocache_noinit_ram_data_end__ - ORIGIN(nocache_memory));
        . = ALIGN(32);
    } > ram_memory

    __itcm_load_addr = __nocache_ram_load_addr + SIZEOF(.nocache_ram_region);
    .itcm_region : AT (__itcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_code_start__ = .;
        *(.tcm_code.*)
        *(.tcm_const.*)
        *(.sclock_rlt_code.*)
        *(.sclock_rlt_const.*)

        . = ALIGN(4);
        __tcm_code_end__ = .;
    } > ram_memory

    __dtcm_load_addr = __itcm_load_addr + SIZEOF(.itcm_region);

    .dtcm_region : AT (__dtcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_data_start__ = .;

        *(.tcm_data)

        . = ALIGN(4);
        __tcm_data_end__ = .;
    } > ram_memory

    __system_ram_load_addr = __dtcm_load_addr + SIZEOF(.dtcm_region);

    .system_ram_data_region :
    {
        . = ALIGN(4);
        __system_ram_data_start__ = .;
        *(.system_ram)
        . = ALIGN(4);
        __system_ram_data_end__ = .;
    } > ram_memory

    __ram_load_addr = __system_ram_load_addr + SIZEOF(.system_ram_data_region);

    /* Data section */
    .ram_data_region : AT (__ram_load_addr)
    {
        . = ALIGN(4);
        __ram_data_start__ = .;

        PROVIDE( __global_pointer$ = . + 0x800 );

        *(.data)
        *(.data.*)

        . = ALIGN(4);
        __ram_data_end__ = .;
    } > ram_memory

    __psram_load_addr = (__ram_load_addr + SIZEOF(.ram_data_region));

    .psram_data_region : AT (__psram_load_addr)
    {
        . = ALIGN(4);
        __psram_data_start__ = .;

        /* Not used */
        /* KEEP(*(.psram_data*)) */

        . = ALIGN(4);
        __psram_data_end__ = .;
    } > ram_psram

    /* check flash size must less 48K */
    __etext_final__ = (__psram_load_addr + SIZEOF(.psram_data_region));
    ASSERT(__etext_final__ <= ORIGIN(xip_memory) + Boot2Size, "xip memory overflowed")

    .system_ram_noinit_data_region (NOLOAD) :
    {
        . = ALIGN(4);
        *(.system_ram_noinit)
        . = ALIGN(4);
    } > ram_memory

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(.sbss*)
        . = ALIGN(4);
        __bss_end__ = .;
    } > ram_memory

    .noinit_data (NOLOAD) :
    {
        . = ALIGN(4);
        *(.noinit_data*)
        . = ALIGN(4);
    } > ram_memory

    .stack (NOLOAD) :
    {
        . = ALIGN(16);
        __StackLimit = .;
        . = . + StackSize;
        __StackTop = .;
    } > ram_memory

    /* Check if data + heap + stack exceeds ram limit */
    ASSERT(__StackTop <= ORIGIN(ram_memory) + LENGTH(ram_memory), "region ram_memory overflowed with stack")

    PROVIDE( __wifi_bss_start = 0 );
    PROVIDE( __wifi_bss_end = 0 );
    PROVIDE(__LD_CONFIG_EM_SEL = 0K);
}

