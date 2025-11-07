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
PROVIDE(__boot2_pass_param_addr = 0x42049C00);/* 0x42030000+103*1024 */

__EM_SIZE = DEFINED(ble_controller_init) ? 8K : 0K;

MEMORY
{
    fw_header_memory  (rx)  : ORIGIN = 0x23000000 - 0x2000, LENGTH = 8K
	xip_memory  (rx)  : 	ORIGIN = 0x23000000, LENGTH = 3K + 256
 	xip_memory1 (rx)  : 	ORIGIN = 0x23000d00, LENGTH = 44K + 768
    itcm_memory (rx)  : 	ORIGIN = 0x2201c000, LENGTH = 40K
    dtcm_memory (rx)  : 	ORIGIN = 0x42026000, LENGTH = 4K
    ram_memory  (!rx) :   	ORIGIN = 0x42027000, LENGTH = 132K
}

SECTIONS
{
    PROVIDE(__metal_chicken_bit = 0);

    .fw_header :
    {
        KEEP(*(.fw_header))
    } > fw_header_memory

    .text :
    {
        . = ALIGN(4);
        __text_code_start__ = .;

        KEEP (*(SORT_NONE(.init)))
        KEEP (*(SORT_NONE(.vector)))

        KEEP (*(.text.metal.init.enter))

        *(.text.SystemInit*)
        *(.text.start_load*)
        *(.text.system_bor_init*)
        *(.text.HBN_Enable_BOR_IRQ*)
        *(.text.HBN_Set_BOR_Cfg*)
        *(.text.HBN_Disable_BOR_IRQ*)

		*start_load*.o*(.text)

        . = ALIGN(4);
        __text_code_end__ = .;
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
    . = ALIGN(4);
    __itcm_load_addr = ORIGIN(xip_memory1);

    .itcm_region : AT (__itcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_code_start__ = .;
        *(.text*)
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)
        *(.tcm_code.*)
        *(.tcm_const.*)
        *(.sclock_rlt_code.*)
        *(.sclock_rlt_const.*)
        *bl602_romapi*.o*(.text)
        *bl602_romapi*.o*(.text.*)
        *bl602_romapi*.o*(.rodata)
        *bl602_romapi*.o*(.rodata.*)
        *bl602_romapi*.o*(.srodata)
        *bl602_romapi*.o*(.srodata.*)
        *bl602_uart*.o* (.text*)
        *hal_uart*.o* (.text*)

        . = ALIGN(4);
        __tcm_code_end__ = .;

    } > itcm_memory

    __dtcm_load_addr = __itcm_load_addr + SIZEOF(.itcm_region);

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

    PROVIDE( __wifi_bss_start = 0 );
    PROVIDE( __wifi_bss_end = 0 );

    /* EM size */
    PROVIDE( __LD_CONFIG_EM_SEL = __EM_SIZE );

}

