/* configure the CPU type */
OUTPUT_ARCH( "riscv" )

/* configure the entry point */
ENTRY(__start)

StackSize    = 0x1000; /*  4KB */
HeapMinSize  = 0x1000; /*  4KB */
psram_heap_min_size = 0x1000;

__RFTLV_SIZE_OFFSET = 1K;
__RFTLV_SIZE_HOLE = 2K;
__RFTLV_HEAD1_H = (0x46524C42); /* */
__RFTLV_HEAD1_L = (0x41524150); /* PAPA */

/* Link stage feature control */
PROVIDE( __macsw_conf_bl_wifi_ps_enable = 0 );
PROVIDE( __macsw_conf_plat_dma_enable = 1 );

VerOffset = 0xc00;

MEMORY
{
    fw_header_memory  (rx)  : ORIGIN = 0x80000000 - 0x1000, LENGTH = 4K
    xip_memory  (rx)  : ORIGIN = 0x80000000, LENGTH = 4M
    ram_psram  (wxa)   : ORIGIN = 0x88000000, LENGTH = 4M
    ram_nocache (!rx) : ORIGIN = 0x21000400, LENGTH = 320K - 1K /* reserved 1k for security */
    ram_memory  (!rx) : ORIGIN = 0x61000400, LENGTH = 320K - 1K /* reserved 1k for security */
    ram_wifi  (wxa)   : ORIGIN = 0x21050000, LENGTH = 320K
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
        LONG(0x42464c42);
        LONG(0x46524556);
        KEEP (*(.qcc74xverinf))
        LONG(0x42464c42);
        LONG(0x46524556);
        KEEP (*(.verinfo))
    } > xip_memory

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

        INCLUDE run_in_ram_function.ld.in

        *libfreertos.a:*.o*(.text*)
        *libfreertos.a:*.o*(.rodata.CSWTCH*)
        *libfreertos.a:*.o*(.rodata.ullMachineTimerCompareRegisterBase)
        *libfreertos.a:*.o*(.rodata.uxTimerIncrementsForOneTick)
        *libfreertos.a:*.o*(.rodata.uxTimerIncrementsForOneTick)
        *libfreertos.a:*.o*(.rodata.xISRStackTop)

        /* *libbl616_phyrf.a:*.o*(.text*) */
        /* *libbl616_phyrf.a:*.o*(.rodata*) */

        *wifi6_lwip_adapter.a:net_tg_al.c.o*(.text*)
        *lwip.a:udp.c.o*(.text*)

        *libc.a:lib_a-memset.c.o*(.text)
        *libc.a:lib_a-memset.c.o*(.rodata)
        *libc.a:lib_a-memcpy.c.o*(.text)
        *libc.a:lib_a-memcpy.c.o*(.rodata)
        *libc.a:lib_a-memcpy-asm.c.o*(.text)
        *libc.a:lib_a-memcpy-asm.c.o*(.rodata)
        *libc.a:lib_vikmemcpy.c.o*(.text*)
        *libc.a:lib_vikmemcpy.c.o*(.rodata*)
        *libc.a:lib_memset.c.o*(.text*)
        *libc.a:lib_memset.c.o*(.rodata*)
        *libgcc.a:_clzsi2.o*(.text)
        *libgcc.a:_clzsi2.o*(.rodata)
        *libgcc.a:_clz.o*(.text)
        *libgcc.a:_clz.o*(.rodata)

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
        *bl616d_ef_cfg*.o*(.rodata*)
        *bl616d_clock*.o*(.rodata*)
        *_clz*.o*(.rodata*)
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
        __qcc74xlog_tags_start__ = .;
        *(.qcc74xlog_tags_array)
        . = ALIGN(4);
        __qcc74xlog_tags_end__ = .;
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

    .stack (NOLOAD):
    {
        __StackLimit = .;
        . = ALIGN(16);
        . = . + StackSize;
        __StackTop = .;
        __freertos_irq_stack_top = .;
    } > ram_memory

    .high_stack (NOLOAD):
    {
      . = ALIGN(16);
      . = . + StackSize;
      __freertos_high_irq_stack_top = .;
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
      PROVIDE( _heap_wifi_start = . );
    } > ram_wifi

    __text_load_addr = (__psram_load_addr + SIZEOF (.psram_data_region));

    .text __text_load_addr : AT(__text_load_addr)
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
        *(EXCLUDE_FILE( *bl616d_glb*.o* \
                        *bl616d_pds*.o* \
                        *bl616d_common*.o* \
                        *bl616d_sf_cfg*.o* \
                        *bl616d_sf_ctrl*.o* \
                        *bl616d_sflash*.o* \
                        *bl616d_xip_sflash*.o* \
                        *bl616d_ef_ctrl*.o* \
                        *bl616d_ef_cfg*.o* \
                        *bl616d_clock*.o* \
                        *_clz*.o* \
                        *bl616d_romapi_patch*.o* ).rodata*)

        *(.srodata)
        *(.srodata.*)

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

        /* _dump_sections for qcc74x_coredump.c*/
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
        LONG(0xffffffff);
        LONG(0xffffffff);
        /* _dump_sections for qcc74x_coredump.c*/

        . = ALIGN(4);
        __text_code_end__ = .;
    } > xip_memory

    .build_id :
    {
        . = ALIGN(4);
        PROVIDE( elf_build_id = ABSOLUTE(.) + 16);
        *(.note.gnu.build-id)
    } > xip_memory

    __etext_final = (__text_load_addr + SIZEOF (.text));
    ASSERT(__etext_final <= ORIGIN(xip_memory) + LENGTH(xip_memory), "code memory overflow")

    PROVIDE( _heap_wifi_size = ORIGIN(ram_wifi) + LENGTH(ram_wifi) - _heap_wifi_start );
}
