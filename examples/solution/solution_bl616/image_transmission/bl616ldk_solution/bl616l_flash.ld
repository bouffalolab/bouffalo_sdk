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
__EM_SIZE = DEFINED(btble_controller_init) ? 16K : 0K;
wifi_ram_max_size = DEFINED(wifi_main) ? (160K -  __EM_SIZE - 46K): 32K;

remain_wifi_ram = 160K - wifi_ram_max_size - __EM_SIZE;

__RFTLV_SIZE_OFFSET = 1K;
__RFTLV_SIZE_HOLE = 2K;
__RFTLV_HEAD1_H = (0x46524C42); /* BLRF */
__RFTLV_HEAD1_L = (0x41524150); /* PAPA */

VerOffset = 0xc00;

MEMORY
{
    fw_header_memory  (rx)  : ORIGIN = 0x80000000 - 0x1000, LENGTH = 4K
    xip_memory  (rx)  : ORIGIN = 0x80000000, LENGTH = 4M
    ram_psram  (wxa)  : ORIGIN = 0x88000000, LENGTH = 4M
    ram_nocache (!rx) : ORIGIN = 0x20FC0400, LENGTH = 224K - 1K + remain_wifi_ram /* reserved 1k for security */
    ram_memory  (!rx) : ORIGIN = 0x60FC0400, LENGTH = 224K - 1K + remain_wifi_ram /* reserved 1k for security */
    ram_wifi  (wxa)   : ORIGIN = 0x21020000 - wifi_ram_max_size - __EM_SIZE, LENGTH = wifi_ram_max_size /* wram: 0x20ff8000 */
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

    } > xip_memory

    .rftlv.tool :
    {
        . = ORIGIN(xip_memory) + __RFTLV_SIZE_OFFSET;
        PROVIDE( _ld_symbol_rftlv_address = . );
        LONG(__RFTLV_HEAD1_H);
        LONG(__RFTLV_HEAD1_L);
        . = ORIGIN(xip_memory) + __RFTLV_SIZE_OFFSET + __RFTLV_SIZE_HOLE;
    } > xip_memory

    .ver : AT(ORIGIN(xip_memory) + VerOffset)
    {
		LONG(0x42464c42); /* BLFB */
		LONG(0x46524556); /* VERF */
		KEEP (*(.bflb_verinf))
		LONG(0x42464c42); /* BLFB */
		LONG(0x46524556); /* VERF */
		KEEP (*(.verinfo))
    } > xip_memory

    .text :
    {
        . = ALIGN(4);
        __text_code_start__ = .;

        *(EXCLUDE_FILE(*_udivdi3*.o*) .text*)

        /* section information for components's version */
        . = ALIGN(4);
        _version_info_section_start = .;
        KEEP(*(.version.*))
        _version_info_section_end = .;

        /* section information for shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;

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
        *(EXCLUDE_FILE( *bl616l_clock*.o* \
                        *bl616l_common*.o* \
                        *bl616l_ef_cfg*.o* \
                        *bl616l_glb*.o* \
                        *bl616l_pds*.o* \
                        *bl616l_pm*.o* \
                        *bl616l_romapi_patch*.o* ).rodata*)

        *(.srodata*)

        /* global object for c++ */
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(4);
        *(.gcc_except_table.*)

        /* _dump_sections for bflb_coredump.c*/
        . = ALIGN(4);
        PROVIDE( _dump_sections = ABSOLUTE(.));
        LONG(ORIGIN(ram_memory) + (__nocache_noinit_ram_data_end__ - ORIGIN(ram_nocache)));
        LONG(LENGTH(ram_memory) - (__nocache_noinit_ram_data_end__ - ORIGIN(ram_nocache)));
        LONG(ORIGIN(ram_nocache));
        LONG(__nocache_noinit_ram_data_end__ - ORIGIN(ram_nocache));
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
    } > xip_memory

    .build_id :
    {
        . = ALIGN(4);
        elf_build_id = ABSOLUTE(.) + 16;
        *(.note.gnu.build-id)
        . = ALIGN(4);
    } > xip_memory

    .nocache_ram_region  :
    {
        __nocache_ram_load_addr = LOADADDR(.nocache_ram_region);
        __nocache_ram_data_start__ = ADDR(.nocache_ram_region);
        __nocache_ram_data_end__ = ADDR(.nocache_ram_region) + SIZEOF(.nocache_ram_region);

        . = ALIGN(4);

        *(.nocache_ram)

        . = ALIGN(4);
    } > ram_nocache AT > xip_memory

    .nocache_noinit_ram_region (NOLOAD) :
    {
        __nocache_noinit_ram_data_start__ = ADDR(.nocache_noinit_ram_region);
        __nocache_noinit_ram_data_end__ = ADDR(.nocache_noinit_ram_region) + SIZEOF(.nocache_noinit_ram_region);

        . = ALIGN(4);
        *(.nocache_noinit_ram)
        *(.noncacheable)

        . = ALIGN(4);
    } > ram_nocache

    /* Skip the nocache region */
    .ram_skip_nocache_region (NOLOAD) :
    {
        . += (__nocache_noinit_ram_data_end__ - ORIGIN(ram_nocache));
        . = ALIGN(32);
    } > ram_memory

    .itcm_region :
    {
        __itcm_load_addr = LOADADDR(.itcm_region);
        __tcm_code_start__ = ADDR(.itcm_region);
        __tcm_code_end__ = ADDR(.itcm_region) + SIZEOF(.itcm_region);

        . = ALIGN(4);
        *(.tcm_code.*)
        *(.tcm_const.*)
        *(.sclock_rlt_code.*)
        *(.sclock_rlt_const.*)

        *bl616l_clock*.o*(.rodata*)
        *bl616l_common*.o*(.rodata*)
        *bl616l_ef_cfg*.o*(.rodata*)
        *bl616l_glb*.o*(.rodata*)
        *bl616l_pds*.o*(.rodata*)
        *bl616l_pm*.o*(.rodata*)
        *bl616l_romapi_patch*.o*(.rodata*)

        *_udivdi3*.o*(.text*)

        . = ALIGN(4);
    } > ram_memory AT > xip_memory

    .dtcm_region :
    {
        __dtcm_load_addr = LOADADDR(.dtcm_region);
        __tcm_data_start__ = ADDR(.dtcm_region);
        __tcm_data_end__ = ADDR(.dtcm_region) + SIZEOF(.dtcm_region);

        . = ALIGN(4);
        __tcm_data_start__ = .;

        *(.tcm_data)
        /* *finger_print.o(.data*) */

        . = ALIGN(4);
        __tcm_data_end__ = .;
    } > ram_memory AT > xip_memory

    /* Data section */
    RAM_DATA :
    {
        __ram_load_addr = LOADADDR(RAM_DATA);
        __ram_data_start__ = ADDR(RAM_DATA);
        __ram_data_end__ = ADDR(RAM_DATA) + SIZEOF(RAM_DATA);

        . = ALIGN(4);

        PROVIDE( __global_pointer$ = . + 0x800 );

        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)
        *(.sdata2)
        *(.sdata2.*)

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
        _bt_l2cap_br_fixed_chan_list_start = .;
        KEEP(*(SORT_BY_NAME("._bt_l2cap_br_fixed_chan.static.*")))
        _bt_l2cap_br_fixed_chan_list_end = .;

        . = ALIGN(4);
        __bflb_log_tags_start__ = .;
        *(.bflb_log_tags_array)
        . = ALIGN(4);
        __bflb_log_tags_end__ = .;
    } > ram_memory AT > xip_memory

    .psram_data_region :
    {
        __psram_load_addr = LOADADDR(.psram_data_region);
        __psram_data_start__ = ADDR(.psram_data_region);
        __psram_data_end__ = ADDR(.psram_data_region) + SIZEOF(.psram_data_region);

        . = ALIGN(32);

        KEEP(*(.psram_code*))

        . = ALIGN(32);

        KEEP(*(.psram_data*))

        . = ALIGN(32);
    } > ram_psram AT > xip_memory

    __etext_final = (__psram_load_addr + SIZEOF (.psram_data_region));
    __edata_etext_final = ALIGN(__etext_final, 1024);
    ASSERT(__etext_final <= ORIGIN(xip_memory) + LENGTH(xip_memory), "code memory overflow")

    .stack (NOLOAD):
    {
        __StackLimit = .;
        . = ALIGN(16);
        . = . + StackSize;
        __StackTop = .;
        __freertos_irq_stack_top = .;
    } > ram_memory

    PROVIDE( __freertos_irq_stack_top = __StackTop);
    PROVIDE( _sp_main = __StackTop);
    PROVIDE( _sp_base = __StackLimit);

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

    .heap (NOLOAD):
    {
        . = ALIGN(32);
        __HeapBase = .;

        KEEP(*(.heap*))

        . = ALIGN(32);
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
        . = ALIGN(32);
        __psram_heap_base = .;
        KEEP(*(.psram_heap*))
        . = ALIGN(32);
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

