/****************************************************************************************
* @file map.txt
*
* @brief This file is the map file (gnuarm or armgcc).
*
* Copyright (C) BouffaloLab 2018
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

StackSize = 0x1000; /*  4KB */
VerOffset = 0xC00;
Boot2Size = 0xC000;
PROVIDE(__boot2_pass_param_addr = 0x42033C00);

MEMORY
{
    fw_header_memory  (rx)  : ORIGIN = 0x23000000 - 0x1000, LENGTH = 4K
    xip_memory  (rx)  : 	ORIGIN = 0x23000000, LENGTH = 3K
    ver_memory  (rx)  : 	ORIGIN = 0x23000000 + VerOffset, LENGTH = 256
    xip_memory1 (rx)  : 	ORIGIN = 0x23000d00, LENGTH = 44K + 768
    itcm_memory (rx)  : 	ORIGIN = 0x22020000, LENGTH = 16K
    dtcm_memory (rx)  : 	ORIGIN = 0x42024000, LENGTH = 4K
    ram_memory  (!rx) : 	ORIGIN = 0x42025000, LENGTH = 59K
    hbn_memory  (!rx) : 	ORIGIN = 0x40010000, LENGTH = 4K
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

        KEEP (*(.text.metal.init.enter))
        KEEP (*(SORT_NONE(.init)))

        *(.text.hal*)
        *(.text.GLB*)
        *(.text.bflb_sp_do_ram_image_boot*)
        *(.text.start_load)
        *(.text.bflb_sp_boot2_show_timer)

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
        . = ALIGN(4);
        __text_code_start__ = .;

        KEEP (*(SORT_NONE(.init)))
        KEEP (*(SORT_NONE(.vector)))

        KEEP (*(.text.metal.init.enter))

        /* *(EXCLUDE_FILE
            ( *bl702l_glb*.o*
              *hal_sec_hash*.o*
              *hal_mtimer*.o*
              *hal_boot2*.o*
              *bl702l_pds*.o*
              *bl702l_common*.o*
              *bl702l_sf_cfg*.o*
              *bl702l_sf_cfg_ext*.o*
              *bl702l_sf_ctrl*.o*
              *bl702l_sflash*.o*
              *bl702l_sflash_ext*.o*
              *bl702l_xip_sflash*.o*
              *bl702l_xip_sflash_ext*.o*
              *bl702l_ef_ctrl*.o*) .text*) */

        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)

        . = ALIGN(4);
        __text_code_end__ = .;
    } > xip_memory1

    __itcm_load_addr = __text_code_end__;

    .itcm_region : AT (__itcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_code_start__ = .;
        *(.tcm_code.*)
        *(.tcm_const.*)
        *(.sclock_rlt_code.*)
        *(.sclock_rlt_const.*)

        /* *bl702l_glb*.o*(.rodata*) */
        /* *bl702l_pds*.o*(.rodata*) */
        /* *bl702l_common*.o*(.rodata*) */
        /* *bl702l_sf_cfg*.o*(.rodata*) */
        /* *bl702l_sf_cfg_ext*.o*(.rodata*) */
        /* *bl702l_sf_ctrl*.o*(.rodata*) */
        /* *bl702l_sflash*.o*(.rodata*) */
        /* *bl702l_sflash_ext*.o*(.rodata*)*/
        /* *bl702l_xip_sflash*.o*(.rodata*)*/
        /* *bl702l_xip_sflash_ext*.o*(.rodata*)*/
        /* *bl702l_ef_ctrl*.o*(.rodata*)       */

        . = ALIGN(4);
        __tcm_code_end__ = .;

    } > itcm_memory

    __hbn_load_addr = __itcm_load_addr + SIZEOF(.itcm_region);

    .hbn_ram_region : AT (__hbn_load_addr)
    {
        . = ALIGN(4);
        __hbn_ram_start__ = .;
        *(.hbn_code.*)
        *(.hbn_data.*)
        *(.retention)
        . = ALIGN(4);
        __hbn_ram_end__ = .;
    } > hbn_memory

    __dtcm_load_addr = __hbn_load_addr + SIZEOF(.hbn_ram_region);

    .dtcm_region : AT (__dtcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_data_start__ = .;

        *(.tcm_data.*)

        . = ALIGN(4);
        __tcm_data_end__ = .;
    } > dtcm_memory

    __StackTop = ORIGIN(dtcm_memory) + LENGTH(dtcm_memory);
    __StackLimit = __StackTop - StackSize;

    /* Check if data + heap + stack exceeds dtcm limit */
    ASSERT(__StackLimit >= __tcm_data_end__, "region dtcm overflowed with stack")

    __system_ram_load_addr = __dtcm_load_addr + SIZEOF(.dtcm_region);

    .system_ram_data_region :
    {
        . = ALIGN(4);
        __system_ram_data_start__ = .;

        *(.system_ram)
        *(.nocache_ram)

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
        *(.sdata)
        *(.sdata.*)
        *(.sdata2)
        *(.sdata2.*)

        . = ALIGN(4);
        __ram_data_end__ = .;
    } > ram_memory

    /* check flash size must less 48K */
    __etext_final__ = (__ram_load_addr + SIZEOF(.ram_data_region));
    ASSERT(__etext_final__ <= ORIGIN(xip_memory) + Boot2Size, "xip memory overflowed")

    .nocache_ram_region (NOLOAD) :
    {
        . = ALIGN(4);
        *(.nocache_ram)
        *(.nocache_noinit_ram)
        . = ALIGN(4);
    } > ram_memory

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

    .noinit_hbn_ram_region (NOLOAD) :
    {
        . = ALIGN(4);
        *(.noinit_hbn_ram_data*)
        . = ALIGN(4);
    } > hbn_memory

}

