/****************************************************************************************
* @file bl702_flash_lp.ld
*
* @brief This file is the map file (gnuarm or armgcc).
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

__EM_SIZE = 8K;
__CACHE_SIZE = 16K;

MEMORY
{
    fw_header_memory  (rx)  : ORIGIN = 0x23000000 - 0x1000, LENGTH = 4K
    xip_memory        (rx)  : ORIGIN = 0x23000000, LENGTH = 1024K
    ocram_ret_1       (rx)  : ORIGIN = 0x42020000, LENGTH = (27K + 512)       /* 0x42020000 ~ 0x42026E00, size 27.5K */
    ocram_ret_2       (rx)  : ORIGIN = 0x4202A000, LENGTH = (16K - 12)        /* 0x4202A000 ~ 0x4202E000, (except 0x4202DFF4), size 16K */
    hbn_memory        (rx)  : ORIGIN = 0x40010000, LENGTH = 4K                /* hbnram, size 4K */
    ocram_notret_1    (rx)  : ORIGIN = 0x42026E00, LENGTH = (13K - 512)       /* 0x42026E00 ~ 0x4202A000, size 12.5K */
    dtcm_notret_2     (rx)  : ORIGIN = 0x42014000, LENGTH = 48K               /* dtcm, size 48K */
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
        KEEP (*(SORT_NONE(.init)))
        KEEP (*(SORT_NONE(.vector)))

    } > xip_memory

    .text :
    {
        __text_code_start__ = ADDR(.text);
        __text_code_end__ = ADDR(.text) + SIZEOF(.text);

        *(.text)
        *(.text.*)

        /* section information for shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;

        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;

        *(.rodata*)
        *(.srodata)
        *(.srodata.*)

        KEEP(*(.bleromro))

        /* _dump_sections for bflb_coredump.c*/
        . = ALIGN(4);
        PROVIDE( _dump_sections = ABSOLUTE(.));
        LONG(ORIGIN(ocram_ret_1));
        LONG(LENGTH(ocram_ret_1));
        LONG(ORIGIN(ocram_ret_2));
        LONG(LENGTH(ocram_ret_2));
        LONG(ORIGIN(hbn_memory));
        LONG(LENGTH(hbn_memory));
        LONG(ORIGIN(ocram_notret_1));
        LONG(LENGTH(ocram_notret_1));
        LONG(ORIGIN(dtcm_notret_2));
        LONG(LENGTH(dtcm_notret_2));
        LONG(0xffffffff);
        LONG(0xffffffff);
        /* _dump_sections for bl_coredump.c*/
    } > xip_memory

    .build_id :
    {
        . = ALIGN(4);
        elf_build_id = ABSOLUTE(.) + 16;
        *(.note.gnu.build-id)
    } > xip_memory

    .hbn_ram_region : ALIGN(4)
    {
        __hbn_load_addr = LOADADDR(.hbn_ram_region);
        __hbn_ram_start__ = ADDR(.hbn_ram_region);
        __hbn_ram_end__ = ADDR(.hbn_ram_region) + SIZEOF(.hbn_ram_region);

        /* put bl702_rf_private.o in hbnram to avoid ocram conflict during rf calibration */
        *libphyrf.a:bl702_rf_private.c.o*(.retention .retention_noinit)
        *libphyrf.a:bl702_rf_private.c.o*(.sdata.* .sbss.* .bss.* COMMON)

        *(.hbn_code.*)
        *(.hbn_data.*)
    } > hbn_memory AT > xip_memory

    ASSERT(SIZEOF(.hbn_ram_region) <= 0x400, ".hbn_ram_region overlapped with IOT2LP_PARA(refer to bl_lp.h)")

    RAM_DATA : ALIGN(4)
    {
        __ram_load_addr = LOADADDR(RAM_DATA);
        __ram_data_start__ = ADDR(RAM_DATA);
        __ram_data_end__ = ADDR(RAM_DATA) + SIZEOF(RAM_DATA);

        /* tcm code that should be retentive after pds wakeup */
        *libstd.a:bl702_romapi.c.o*(.tcm_code.* .sclock_rlt_code.*)
        *libstd.a:bl702_aon.c.o*(.tcm_code.* .sclock_rlt_code.*)
        *libstd.a:bl702_glb.c.o*(.tcm_code.* .sclock_rlt_code.*)
        *libstd.a:bl702_hbn.c.o*(.tcm_code.* .sclock_rlt_code.*)
        *libstd.a:bl702_pm.c.o*(.tcm_code.* .sclock_rlt_code.*)
        *libsys.a:bl702_lp_debug.c.o*(.tcm_code.* .sclock_rlt_code.*)
        *libsys.a:bl702_lp.c.o*(.tcm_code.* .sclock_rlt_code.*)
        *libsys.a:bl702_sys.c.o*(.tcm_code.* .sclock_rlt_code.*)
        *libsys.a:bl702_xip_recovery.c.o*(.tcm_code.* .sclock_rlt_code.*)
        *liblhal.a:bflb_sf_ctrl.c.o*(.tcm_code.* .sclock_rlt_code.*)
        *liblhal.a:bflb_sflash.c.o*(.tcm_code.* .sclock_rlt_code.*)

        . = ALIGN(4);
        PROVIDE( __global_pointer$ = . + 0x800 );

        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)
        *(.sdata2)
        *(.sdata2.*)

        *(.nocache_ram)
        
        . = ALIGN(8);
        *(._k_queue.static.*)
        *(._k_sem.static.*)
        *(._k_mutex.static.*)
        _bt_gatt_service_static_list_start = .;
        KEEP(*(SORT_BY_NAME("._bt_gatt_service_static.static.*")))
        _bt_gatt_service_static_list_end = .;
        _bt_l2cap_fixed_chan_list_start = .;
        KEEP(*(SORT_BY_NAME("._bt_l2cap_fixed_chan.static.*")))
        _bt_l2cap_fixed_chan_list_end = .;

        . = ALIGN(4);
        __bflb_log_tags_start__ = .;
        *(.bflb_log_tags_array)
        . = ALIGN(4);
        __bflb_log_tags_end__ = .;
    } > ocram_ret_2 AT > xip_memory

    .bss (NOLOAD) :
    {
        __bss_start__ = ADDR(.bss);
        __bss_end__ = ADDR(.bss) + SIZEOF(.bss);

        *(.bss*)
        *(.sbss*)
        *(COMMON)
    } > ocram_ret_2

    .noinit_data (NOLOAD) :
    {
        __noinit_data_start__ = ADDR(.noinit_data);
        __noinit_data_end__ = ADDR(.noinit_data) + SIZEOF(.noinit_data);

        *(.noinit_data*)
        *(.nocache_noinit_ram)

        KEEP(*(.bleromrw))
    } > ocram_ret_2

    .itcm_region : ALIGN(4)
    {
        __itcm_load_addr = LOADADDR(.itcm_region);
        __tcm_code_start__ = ADDR(.itcm_region);
        __tcm_code_end__ = ADDR(.itcm_region) + SIZEOF(.itcm_region);

        *(.tcm_code.*)
        *(.tcm_const.*)
        *(.sclock_rlt_code.*)
        *(.sclock_rlt_const.*)
    } > dtcm_notret_2 AT > xip_memory

    .dtcm_region : ALIGN(4)
    {
        __dtcm_load_addr = LOADADDR(.dtcm_region);
        __tcm_data_start__ = ADDR(.dtcm_region);
        __tcm_data_end__ = ADDR(.dtcm_region) + SIZEOF(.dtcm_region);

        *(.tcm_data)
        /* *finger_print.o(.data*) */
    } > dtcm_notret_2 AT > xip_memory

    .stack_dummy (NOLOAD) : ALIGN(128)  /* ALIGN(128) required by FreeRTOS */
    {
        . = . + StackSize;
        __StackTop = .;
    } > ocram_notret_1

    PROVIDE( __freertos_irq_stack_top = __StackTop );
    PROVIDE( _sp_main = __StackTop );
    PROVIDE( _sp_base = __StackTop - StackSize );

    __HeapBase = ORIGIN(ocram_ret_1);
    __HeapLimit = ORIGIN(ocram_ret_1) + LENGTH(ocram_ret_1);
    ASSERT(__HeapLimit - __HeapBase >= HeapMinSize, "heap size is too short.")

    /* EM size */
    PROVIDE( __LD_CONFIG_EM_SEL = __EM_SIZE );
}

