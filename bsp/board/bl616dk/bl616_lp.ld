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
__EM_SIZE = DEFINED(btble_controller_init) ? 32K : 0K;
__LPFW_SIZE = 30K;

__RFTLV_SIZE_OFFSET = 1K;
__RFTLV_SIZE_HOLE = 2K;
__RFTLV_HEAD1_H = (0x46524C42); /* BLRF */
__RFTLV_HEAD1_L = (0x41524150); /* PAPA */

VerOffset = 0xc00;

MEMORY
{
    fw_header_memory  (rx)  : ORIGIN = 0xA0000000 - 0x1000, LENGTH = 4K
    xip_memory        (rx)  : ORIGIN = 0xA0000000,  LENGTH = 4M
    ram_psram         (wxa) : ORIGIN = 0xA8000000,  LENGTH = 4M
    ram_nocache       (!rx) : ORIGIN = 0x22FC0400,  LENGTH = 320K - 1K  /* reserved 1k for security */
    ram_memory        (!rx) : ORIGIN = 0x62FC0400,  LENGTH = 320K - 1K  /* reserved 1k for security */
    ram_wifi_nocache  (wxa) : ORIGIN = 0x23010000,  LENGTH = 160K - __EM_SIZE
    hbn_memory        (rx)  : ORIGIN = 0x20010000,  LENGTH = 512
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
		KEEP (*(.blverinf))
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
        *(EXCLUDE_FILE( *bl616_glb*.o* \
                        *bl616_pds*.o* \
                        *bl616_common*.o* \
                        *bl616_sf_cfg*.o* \
                        *bl616_sf_ctrl*.o* \
                        *bl616_sflash*.o* \
                        *bl616_xip_sflash*.o* \
                        *bl616_ef_ctrl*.o* \
                        *bl616_ef_cfg*.o* \
                        *bl616_clock*.o* \
                        *_clz*.o* \
                        *bl616_romapi_patch*.o* ).rodata*)

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
        LONG(ORIGIN(ram_wifi_nocache));
        LONG(LENGTH(ram_wifi_nocache));
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

        *bl616_glb*.o*(.rodata*)
        *bl616_pds*.o*(.rodata*)
        *bl616_common*.o*(.rodata*)
        *bl616_sf_cfg*.o*(.rodata*)
        *bl616_sf_ctrl*.o*(.rodata*)
        *bl616_sflash*.o*(.rodata*)
        *bl616_xip_sflash*.o*(.rodata*)
        *bl616_ef_ctrl*.o*(.rodata*)
        *bl616_ef_cfg*.o*(.rodata*)
        *bl616_clock*.o*(.rodata*)
        *_clz*.o*(.rodata*)
        *bl616_romapi_patch*.o*(.rodata*)

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
        __bflog_tags_start__ = .;
        *(.bflog_tags_array)
        . = ALIGN(4);
        __bflog_tags_end__ = .;
    } > ram_memory AT > xip_memory

    .hbn_ram_region :
    {
        __hbn_load_addr = LOADADDR(.hbn_ram_region);
        __hbn_ram_start__ = ADDR(.hbn_ram_region);
        __hbn_ram_end__ = ADDR(.hbn_ram_region) + SIZEOF(.hbn_ram_region);

        . = ALIGN(32);
        KEEP(*(.hbn_code.*))
        KEEP(*(.hbn_data.*))
        . = ALIGN(32);

    } > hbn_memory  AT > xip_memory

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

    .lpfw : 
    {
        /* For DMA Burst copy lpfw binary from flash to ram */
        . = ALIGN(32);
        LONG(0x0);        /* LPFW SHA256 */
        . = ALIGN(32);
        LONG(0x5746504C); /* LPFW */
        . = ALIGN(32);

        __lpfw_load_addr = .;
    } > xip_memory

    ASSERT(((ABSOLUTE(__lpfw_load_addr) & 0x1F) == 0), "LPFW not aligned 32!")
    PROVIDE(__binary_length = __lpfw_load_addr - ORIGIN(xip_memory));

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
        PROVIDE( __lp_mon_start = ADDR(.lp_mon) );
        PROVIDE( __lp_mon_end = ADDR(.lp_mon) + SIZEOF(.lp_mon) );
        *(.lp_mon_ctx)

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

    .wram.nocache_noinit_lpfw_share (NOLOAD) :
    {
        __lpfw_share_start = ADDR(.wram.nocache_noinit_lpfw_share);
        __lpfw_share_end = ADDR(.wram.nocache_noinit_lpfw_share) + __LPFW_SIZE;

        _sshram = . ;

        *(.lpfw_share_nocache_noinit*)

        __lpfw_share_used = .;

        . = (__lpfw_share_used < __lpfw_share_end) ? __lpfw_share_end : .;

    } > ram_wifi_nocache
 
    .wram.nocache_noinit (NOLOAD) :
    {
        PROVIDE( __wifi_bss_start = ADDR(.wram.nocache_noinit) );
        PROVIDE( __wifi_bss_end = ADDR(.wram.nocache_noinit) + SIZEOF(.wram.nocache_noinit) );

        *(SHAREDRAMIPC)
        *(SHAREDRAM)
        _eshram = .;
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
    } > ram_wifi_nocache

    PROVIDE( _heap_wifi_start = . );
    PROVIDE( _heap_wifi_size = ORIGIN(ram_wifi_nocache) + LENGTH(ram_wifi_nocache) - _heap_wifi_start );
    PROVIDE(__LD_CONFIG_EM_SEL = __EM_SIZE);
}

