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
VerOffset = 0xc00;
Boot2Size = 0xC000;
PROVIDE(__boot2_pass_param_addr = 0x4202DC00);

MEMORY
{
    fw_header_memory  (rx)  : ORIGIN = 0x23000000 - 0x2000, LENGTH = 8K
    xip_memory  (rx)  : 	ORIGIN = 0x23000000, LENGTH = 3K + 256
 	xip_memory1 (rx)  : 	ORIGIN = 0x23000d00, LENGTH = 44K + 768
    itcm_memory (rx)  : 	ORIGIN = 0x22014000, LENGTH = 12K
    dtcm_memory (rx)  : 	ORIGIN = 0x42017000, LENGTH = 4K
    ram_memory  (!rx) : 	ORIGIN = 0x42018000, LENGTH = 87K
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

        /* section information for usb desc */
        . = ALIGN(4);
        _usb_desc_start = .;
        KEEP(*(usb_desc))
        . = ALIGN(4);
        _usb_desc_end = .;
    } > xip_memory

	.ver 0 : AT(ORIGIN(xip_memory) + VerOffset)
    {

		LONG(0x42464c42); /* BLFB */
		LONG(0x46524556); /* VERF */
		KEEP (*(.bflb_verinf))
		LONG(0x42464c42); /* BLFB */
		LONG(0x46524556); /* VERF */
		KEEP (*(.verinfo))
    }
    .text :
    {
        . = ALIGN(4);
        __text_code_start__ = .;

        KEEP (*(SORT_NONE(.init)))
        KEEP (*(SORT_NONE(.vector)))

        KEEP (*(.text.metal.init.enter))

        /* section information for usb desc */
        . = ALIGN(4);
        _usb_desc_start = .;
        KEEP(*(usb_desc))
        . = ALIGN(4);
        _usb_desc_end = .;

        *(EXCLUDE_FILE
            ( *bl702_uart*.o*
        	  *bflb_uart*.o*
        	  *bl702_sec_eng*.o*
        	  *bl702_glb*.o*
        	  *hal_sec_hash*.o*
        	  *hal_mtimer*.o*
        	  *hal_boot2*.o*
        	  *bflb_eflash_loader_interface*.o*
        	  *bflb_eflash_loader_uart*.o*
        	  *bflb_platform*.o*
        	  *xz_port*.o*
        	  *drv_device*.o*
        	  *bl702_clock*.o*
        	  *_udivdi3*.o*
        	  *HBN_Set_UART_CLK_Sel*
        	  *bflb_sec_sha*.o*
        	  *lib_a-memset*.o*) .text*)

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
        *bl702_romapi*.o*(.text)
        *bl702_romapi*.o*(.text.*)
        *bl702_romapi*.o*(.rodata)
        *bl702_romapi*.o*(.rodata.*)
        *bl702_romapi*.o*(.srodata)
        *bl702_romapi*.o*(.srodata.*)
        *bl702_uart*.o* (.text*)
        *bflb_uart*.o* (.text*)
        *bl702_sec_eng*.o* (.text*)
        *bl702_glb*.o* (.text*)
        *hal_sec_hash*.o* (.text*)
        *hal_mtimer*.o* (.text*)
        *hal_boot2*.o* (.text*)
        *bflb_eflash_loader_interface*.o* (.text*)
        *bflb_eflash_loader_uart*.o* (.text*)
        *bflb_platform*.o* (.text*)
        *xz_port*.o* (.text*)
        *drv_device*.o* (.text*)
        *bl702_clock*.o* (.text*)
        *udivdi3*(.rodata*)
        *udivdi3*(.text*)
        *memset*(.text*)
        *bflb_sec_sha*.o* (.text*)
        . = ALIGN(4);
        __tcm_code_end__ = .;

    } > itcm_memory

    __hbn_load_addr = __itcm_load_addr + SIZEOF(.itcm_region);

    .hbn_ram_region : AT (__hbn_load_addr)
    {
        __hbn_ram_start__ = ADDR(.hbn_ram_region);
        __hbn_ram_end__ = ADDR(.hbn_ram_region) + SIZEOF(.hbn_ram_region);

        /* put bl702_rf_private.o in hbnram to avoid ocram conflict during rf calibration */
        *libphyrf.a:bl702_rf_private.c.o*(.retention .retention_noinit)
        *libphyrf.a:bl702_rf_private.c.o*(.sdata.* .sbss.* .bss.* COMMON)

        *(.hbn_code.*)
        *(.hbn_data.*)
    } > hbn_memory

    ASSERT(SIZEOF(.hbn_ram_region) <= 0x400, ".hbn_ram_region overlapped with IOT2LP_PARA(refer to bl_lp.h)")


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

    /* check flash size must less 48K */
    __etext_final__ = (__ram_load_addr + SIZEOF(.ram_data_region));
    ASSERT(__etext_final__ <= ORIGIN(xip_memory) + Boot2Size, "xip memory overflowed")

    .nocache_ram_region (NOLOAD) :
    {
        . = ALIGN(4);
        *(.nocache_ram)
        *(.nocache_noinit_ram)
        *(.noncacheable)
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

