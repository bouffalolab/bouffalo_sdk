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

/* configure the entry point */
ENTRY(__start)

StackSize    = 0x1000; /*  4KB */
HeapMinSize  = 0x1000; /*  4KB */
psram_heap_min_size = 0x1000;
nocache_max_size = 20K;
__EM_SIZE = DEFINED(btble_controller_init) ? 32K : 0K;
wifi_ram_max_size = DEFINED(wifi_main) ? (160K -  __EM_SIZE): 32K;

remain_wifi_ram = 160K - wifi_ram_max_size - __EM_SIZE;

__RFTLV_SIZE_OFFSET = 1K;
__RFTLV_SIZE_HOLE = 2K;
__RFTLV_HEAD1_H = (0x46524C42); /* */
__RFTLV_HEAD1_L = (0x41524150); /* PAPA */

MEMORY
{
    fw_header_memory  (rx)  : ORIGIN = 0xA0000000 - 0x1000, LENGTH = 4K
    xip_memory  (rx)  : ORIGIN = 0xA0000000, LENGTH = 4M
    ram_psram  (wxa)   : ORIGIN = 0xA8000000 + __RFTLV_SIZE_OFFSET, LENGTH = 4M - __RFTLV_SIZE_OFFSET
    itcm_memory (rx)  : ORIGIN = 0x62FC0400, LENGTH = 11K /* first 1K is for security */
    dtcm_memory (rx)  : ORIGIN = 0x62FC3000, LENGTH = 4K
    nocache_ram_memory (!rx) : ORIGIN = 0x22FC4000, LENGTH = nocache_max_size
    ram_memory  (!rx) : ORIGIN = 0x62FC4000 + nocache_max_size, LENGTH = 320K - 12K - 4K - nocache_max_size + remain_wifi_ram
    ram_wifi  (wxa)   : ORIGIN = 0x23038000 - wifi_ram_max_size - __EM_SIZE, LENGTH = wifi_ram_max_size
}

SECTIONS
{

    BOOT2_PT_ADDR = 0x63027c00;

    .fw_header :
    {
        KEEP(*(.fw_header))
    } > fw_header_memory

    .init :
    {
        KEEP (*(SORT_NONE(.init)))
        KEEP (*(SORT_NONE(.vector)))
        *system_bl616*.o*(.rodata*)
        *system_bl616*.o*(.text*)
        *start_load*.o*(.rodata*)
        *start_load*.o*(.text*)
    } > xip_memory

    . = ALIGN(4);
    . = ORIGIN(xip_memory) + __RFTLV_SIZE_OFFSET;
    __psram_load_addr = .;
    .rftlv.tool : AT (__psram_load_addr)
    {
        . = ALIGN(4);
        __psram_data_start__ = .;
        PROVIDE( _ld_symbol_rftlv_address = . );
        LONG(__RFTLV_HEAD1_H);
        LONG(__RFTLV_HEAD1_L);
        . = __psram_data_start__ + __RFTLV_SIZE_HOLE;
    } > ram_psram
    . = ORIGIN(xip_memory) + __RFTLV_SIZE_OFFSET + __RFTLV_SIZE_HOLE;

    __rftlv_load_addr = (__psram_load_addr + SIZEOF (.rftlv.tool));

    .psram_data_region : AT (__rftlv_load_addr)
    {
        . = ALIGN(4);
        __text_code_start__ = .;

        *(.text)
        *(.text.*)

        /* section information for shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;

        /* section information for components's version */
        . = ALIGN(4);
        _version_info_section_start = .;
        KEEP(*(.version.*))
        _version_info_section_end = .;

        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;

        /* section information for usb usbh_class_info */
        . = ALIGN(4);
        __usbh_class_info_start__ = .;
        KEEP(*(.usbh_class_info))
        . = ALIGN(4);
        __usbh_class_info_end__ = .;

        /*put .rodata**/
        *(EXCLUDE_FILE( *bl616_glb*.o* \
                        *bl616_pds*.o* \
                        *bl616_common*.o* \
                        *bl616_sf_cfg*.o* \
                        *bl616_sf_ctrl*.o* \
                        *bl616_sflash*.o* \
                        *bl616_xip_sflash*.o* \
                        *bl616_ef_ctrl*.o* \
                        *bl616_romapi_patch*.o* ).rodata*)

        *(.srodata)
        *(.srodata.*)

        /* _dump_sections for bflb_coredump.c*/
        . = ALIGN(4);
        PROVIDE( _dump_sections = ABSOLUTE(.));
        LONG(ORIGIN(itcm_memory));
        LONG(LENGTH(itcm_memory));
        LONG(ORIGIN(dtcm_memory));
        LONG(LENGTH(dtcm_memory));
        LONG(ORIGIN(ram_memory));
        LONG(LENGTH(ram_memory));
        LONG(ORIGIN(nocache_ram_memory));
        LONG(LENGTH(nocache_ram_memory));
        LONG(ORIGIN(ram_wifi));
        LONG(LENGTH(ram_wifi));
        /*
        LONG(ORIGIN(ram_psram));
        LONG(LENGTH(ram_psram));
         */
        LONG(0x28010000); /* bt memory */
        LONG(__EM_SIZE);
        LONG(0xffffffff);
        LONG(0xffffffff);
        /* _dump_sections for bl_coredump.c*/

        . = ALIGN(4);
        __text_code_end__ = .;

        KEEP(*(.psram_data*))

        . = ALIGN(4);
        __psram_data_end__ = .;
    } > ram_psram

    __etext_final = (__rftlv_load_addr + SIZEOF (.psram_data_region));
    ASSERT(__etext_final <= ORIGIN(xip_memory) + LENGTH(xip_memory), "code memory overflow")

    __itcm_load_addr = (__rftlv_load_addr + SIZEOF (.psram_data_region));

    .itcm_region : AT (__itcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_code_start__ = .;
        *(.tcm_code.*)
        *(.tcm_const.*)
        *(.sclock_rlt_code.*)
        *(.sclock_rlt_const.*)

        *bl616_glb*.o*(.rodata*)
        *bl616_pds*.o*(.rodata*)
        *bl616_common*.o*(.rodata*)
        *bl616_sf_cfg*.o*(.rodata*)
        *bl616_sf_ctrl*.o*(.rodata*)
        *bl616_sflash*.o*(.rodata*)
        *bl616_xip_sflash*.o*(.rodata*)
        *bl616_ef_ctrl*.o*(.rodata*)
        *bl616_romapi_patch*.o*(.rodata*)

        . = ALIGN(4);
        __tcm_code_end__ = .;
    } > itcm_memory

    __dtcm_load_addr = __itcm_load_addr + SIZEOF(.itcm_region);

    .dtcm_region : AT (__dtcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_data_start__ = .;

        *(.tcm_data)
        /* *finger_print.o(.data*) */

        . = ALIGN(4);
        __tcm_data_end__ = .;
    } > dtcm_memory

    /*************************************************************************/
    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (NOLOAD):
    {
        . = ALIGN(0x4);
        . = . + StackSize;
        . = ALIGN(0x4);
    } > dtcm_memory

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(dtcm_memory) + LENGTH(dtcm_memory);
    PROVIDE( __freertos_irq_stack_top = __StackTop);
    PROVIDE( _sp_main = __StackTop);
    PROVIDE( _sp_base = __StackTop - StackSize);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __tcm_data_end__, "region RAM overflowed with stack")

    /*************************************************************************/

    __nocache_ram_load_addr = __dtcm_load_addr + SIZEOF(.dtcm_region);

    .nocache_ram_region  : AT (__nocache_ram_load_addr)
    {
        . = ALIGN(4);
        __nocache_ram_data_start__ = .;

        *(.nocache_ram)

        . = ALIGN(4);
        __nocache_ram_data_end__ = .;
    } > nocache_ram_memory

    __ram_load_addr = __nocache_ram_load_addr + SIZEOF(.nocache_ram_region);

    /* Data section */
    RAM_DATA : AT (__ram_load_addr)
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
        __bflog_tags_start__ = .;
        *(.bflog_tags_array)
        . = ALIGN(4);
        __bflog_tags_end__ = .;
        __ram_data_end__ = .;
    } > ram_memory


    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;

        *(.bss*)
        *(.sbss*)
        *(COMMON)

        . = ALIGN(4);
        __bss_end__ = .;
    } > ram_memory

    .noinit_data (NOLOAD) :
    {
        . = ALIGN(4);
        __noinit_data_start__ = .;

        *(.noinit_data*)

        . = ALIGN(4);
        __noinit_data_end__ = .;
    } > ram_memory

    .nocache_noinit_ram_region (NOLOAD) :
    {
        . = ALIGN(4);
        __nocache_noinit_ram_data_start__ = .;

        *(.nocache_noinit_ram)
        *(.noncacheable)

        . = ALIGN(4);
        __nocache_noinit_ram_data_end__ = .;
    } > nocache_ram_memory

    .heap (NOLOAD):
    {
        . = ALIGN(4);
        __HeapBase = .;

        KEEP(*(.heap*))

        . = ALIGN(4);
        __HeapLimit = .;
    } > ram_memory

    __HeapLimit = ORIGIN(ram_memory) + LENGTH(ram_memory);
    ASSERT(__HeapLimit - __HeapBase >= HeapMinSize, "heap region overflow")

    .psram_noinit_data (NOLOAD):
    {
        . = ALIGN(4);
        __psram_noinit_data_start__ = .;

        KEEP(*(.psram_noinit*))
        KEEP(*(.img_buf*))

        . = ALIGN(4);
        __psram_noinit_data_end__ = .;
    } > ram_psram

    .psram_heap (NOLOAD):
    {
        . = ALIGN(4);
        __psram_heap_base = .;
        KEEP(*(.psram_heap*))
        . = ALIGN(4);
        __psram_heap_end = .;
    } > ram_psram

    __psram_limit = ORIGIN(ram_psram) + LENGTH(ram_psram);
    ASSERT(__psram_limit - __psram_heap_base >= psram_heap_min_size, "psram heap region overflow")

    .wifibss (NOLOAD) :
    {
      PROVIDE( __wifi_bss_start = ADDR(.wifibss) );
      PROVIDE( __wifi_bss_end = ADDR(.wifibss) + SIZEOF(.wifibss) );
      _sshram = . ;
      *(SHAREDRAMIPC)
      *(SHAREDRAM)
      _eshram = . ;
      *ipc_shared.o(COMMON)
      *sdu_shared.o(COMMON)
      *hal_desc.o(COMMON)
      *txl_buffer_shared.o(COMMON)
      *txl_frame_shared.o(COMMON)
      *scan_shared.o(COMMON)
      *scanu_shared.o(COMMON)
      *mfp_bip.o(COMMON)
      *me_mic.o(COMMON)
      *(.wifi_ram*)
      . = ALIGN(16);
    } > ram_wifi

    PROVIDE( _heap_wifi_start = . );
    PROVIDE( _heap_wifi_size = ORIGIN(ram_wifi) + LENGTH(ram_wifi) - _heap_wifi_start );
    PROVIDE(__LD_CONFIG_EM_SEL = __EM_SIZE);
    ASSERT((wifi_ram_max_size + __EM_SIZE) <= 160K,"wifi ram overflow")
}

