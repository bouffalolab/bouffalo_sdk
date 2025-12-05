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
__LPCPU_SIZE = 64K;
wifi_ram_max_size = DEFINED(wifi_main) ? (160K -  __EM_SIZE): 32K;

remain_wifi_ram = 160K - wifi_ram_max_size - __EM_SIZE ;

__RFTLV_SIZE_OFFSET = 1K;
__RFTLV_SIZE_HOLE = 2K;
__RFTLV_HEAD1_H = (0x46524C42); /* BLRF */
__RFTLV_HEAD1_L = (0x41524150); /* PAPA */

/* Link stage feature control */
PROVIDE( __macsw_conf_bl_wifi_ps_enable = 0 );
PROVIDE( __macsw_conf_plat_dma_enable = 1 );

VerOffset = 0xc00;

MEMORY
{
    fw_header_memory  (rx)  : ORIGIN = 0x61000000 - (256+16), LENGTH = 256+16
    code_memory  (rx)  : ORIGIN = 0x61000000, LENGTH = 128K
    ram_psram  (wxa)   : ORIGIN = 0xa0000000, LENGTH = 4M
    ram_nocache (!rx) : ORIGIN = 0x21020000, LENGTH = 320K - 1K /* reserved 1k for security */
    ram_memory  (!rx) : ORIGIN = 0x61020000, LENGTH = 320K - 1K /* reserved 1k for security */
    ram_wifi  (wxa)   : ORIGIN = 0x610a0000 - wifi_ram_max_size - __EM_SIZE, LENGTH = wifi_ram_max_size
}

SECTIONS
{

    BOOT2_PT_ADDR = 0x61027c00;

    .fw_header :
    {
        KEEP(*(.fw_header))
        LONG(ORIGIN(code_memory))
        LONG(0x48474553)
    } > fw_header_memory

    .init :
    {
        KEEP (*(SORT_NONE(.init)))
        KEEP (*(SORT_NONE(.vector)))

    } > code_memory

    .rftlv.tool :
    {
        . = ORIGIN(code_memory) + __RFTLV_SIZE_OFFSET;
        PROVIDE( _ld_symbol_rftlv_address = . );
        LONG(__RFTLV_HEAD1_H);
        LONG(__RFTLV_HEAD1_L);
        . = ORIGIN(code_memory) + __RFTLV_SIZE_OFFSET + __RFTLV_SIZE_HOLE;
    } > code_memory

    .ver : AT(ORIGIN(code_memory) + VerOffset)
    {
		LONG(0x42464c42); /* BLFB */
		LONG(0x46524556); /* VERF */
		KEEP (*(.bflb_verinf))
		LONG(0x42464c42); /* BLFB */
		LONG(0x46524556); /* VERF */
		KEEP (*(.verinfo))
    } > code_memory

    .text :
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

        /* section information for components's version */
        . = ALIGN(4);
        _version_info_section_start = .;
        KEEP(*(.version.*))
        _version_info_section_end = .;

        /*put .rodata**/
        *(EXCLUDE_FILE( *bl616d_glb*.o* \
                        *bl616d_pds*.o* \
                        *bl616d_common*.o* \
                        *bl616d_sf_cfg*.o* \
                        *bl616d_sf_ctrl*.o* \
                        *bl616d_sflash*.o* \
                        *bl616d_xip_sflash*.o* \
                        *bl616d_ef_ctrl*.o* \
                        *bl616d_romapi_patch*.o* ).rodata*)

        *(.srodata)
        *(.srodata.*)

        /* _dump_sections for bflb_coredump.c*/
        . = ALIGN(4);
        PROVIDE( _dump_sections = ABSOLUTE(.));
        LONG(ORIGIN(ram_memory));
        LONG(LENGTH(ram_memory));
        LONG(ORIGIN(ram_nocache));
        LONG(LENGTH(ram_nocache));
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
    } > code_memory

    . = ALIGN(4);
    __nocache_ram_load_addr = .;

    .nocache_ram_region  : AT (__nocache_ram_load_addr)
    {
        . = ALIGN(4);
        __nocache_ram_data_start__ = .;

        *(.nocache_ram)

        . = ALIGN(4);
        __nocache_ram_data_end__ = .;
    } > ram_nocache

    .nocache_noinit_ram_region (NOLOAD) :
    {
        . = ALIGN(4);
        __nocache_noinit_ram_data_start__ = .;

        *(.nocache_noinit_ram)
        *(.noncacheable)

        . = ALIGN(4);
        __nocache_noinit_ram_data_end__ = .;
    } > ram_nocache

    /* Skip the nocache region */
    .ram_skip_nocache_region (NOLOAD) :
    {
        . += (__nocache_noinit_ram_data_end__ - ORIGIN(ram_nocache));
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

        *bl616d_glb*.o*(.rodata*)
        *bl616d_pds*.o*(.rodata*)
        *bl616d_common*.o*(.rodata*)
        *bl616d_sf_cfg*.o*(.rodata*)
        *bl616d_sf_ctrl*.o*(.rodata*)
        *bl616d_sflash*.o*(.rodata*)
        *bl616d_xip_sflash*.o*(.rodata*)
        *bl616d_ef_ctrl*.o*(.rodata*)
        *bl616d_romapi_patch*.o*(.rodata*)

        . = ALIGN(4);
        __tcm_code_end__ = .;
    } > ram_memory

    __dtcm_load_addr = __itcm_load_addr + SIZEOF(.itcm_region);

    .dtcm_region : AT (__dtcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_data_start__ = .;

        *(.tcm_data)
        /* *finger_print.o(.data*) */

        . = ALIGN(4);
        __tcm_data_end__ = .;
    } > ram_memory

    __ram_load_addr = __dtcm_load_addr + SIZEOF(.dtcm_region);

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
        __ram_data_end__ = .;
    } > ram_memory

    __psram_load_addr = (__ram_load_addr + SIZEOF (RAM_DATA));

    .psram_data_region : AT (__psram_load_addr)
    {
        . = ALIGN(4);
        __psram_data_start__ = .;

        KEEP(*(.psram_data*))

        . = ALIGN(4);
        __psram_data_end__ = .;
    } > ram_psram

    __etext_final = (__psram_load_addr + SIZEOF (.psram_data_region));
    ASSERT(__etext_final <= ORIGIN(code_memory) + LENGTH(code_memory), "code memory overflow")

    __psram_end_addr = __etext_final;

    .lpcpu : AT (__psram_end_addr)
    {
        /* For DMA Burst copy lpfw binary from flash to ram */
        . = ALIGN(32);
        LONG(0x0);        /* LPFW SHA256 */
        . = ALIGN(32);
        LONG(0x5746504C); /* LPFW */
        . = ALIGN(32);
        PROVIDE( __lpfw_start = . );
        ASSERT(((ABSOLUTE(__lpfw_start) & 0x1F) == 0), "LPFW not aligned 32!");
    } > ram_memory


    __lpcpu_code_end = (__etext_final +  SIZEOF (.lpcpu));
    ASSERT(__lpcpu_code_end <= ORIGIN(code_memory) + LENGTH(code_memory), "code memory overflow")

    .stack (NOLOAD):
    {
        . = . + __LPCPU_SIZE;
        __StackLimit = .;
        . = ALIGN(16);
        . = . + StackSize;
        __StackTop = .;
        __freertos_irq_stack_top = .;

        /* for CPU_NP */
        . = . + StackSize;
        __StackTop_NP = .;
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
