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

StackSize    = 0x0400; /*  1KB */
HeapMinSize  = 0x1000; /*  4KB */

MEMORY
{
    /* fw_header_memory  (rx)  : ORIGIN = 0x00010000, LENGTH = 256+16 */
    code_memory  (rx)  : ORIGIN = 0x00010000, LENGTH = 54K
    ram_memory  (!rx) : ORIGIN = 0x0001D800, LENGTH = 10K
}

SECTIONS
{
    /* .fw_header :
    {
        KEEP(*(.fw_header))
        LONG(ORIGIN(code_memory))
        LONG(0x48474553)
    } > fw_header_memory */

    .init :
    {
        KEEP (*(SORT_NONE(.init)))
        KEEP (*(SORT_NONE(.vector)))

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

        /* section information for sysinit */
        . = ALIGN(4);
        __sysinit_array_start = .;
        KEEP(*(SORT_BY_NAME(.sysinit.*)))
        __sysinit_array_stop = .;

        /* section information for components's version */
        . = ALIGN(4);
        _version_info_section_start = .;
        KEEP(*(.version.*))
        _version_info_section_end = .;

        /*put .rodata**/
        *(.tcm_code.*)
        *(.tcm_const.*)
        *(.sclock_rlt_code.*)
        *(.sclock_rlt_const.*)

        *(.rodata*)

        *(.srodata)
        *(.srodata.*)

        /* _dump_sections for bflb_coredump.c*/
        . = ALIGN(4);
        PROVIDE( _dump_sections = ABSOLUTE(.));
        LONG(ORIGIN(ram_memory));
        LONG(LENGTH(ram_memory));
        LONG(0x28010000); /* bt memory */
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
        *(.nocache_noinit_ram)
        *(.noncacheable)

        . = ALIGN(4);
        __nocache_ram_data_end__ = .;
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
        /* *finger_print.o(.data*) */

        . = ALIGN(4);
        __tcm_data_end__ = .;
    } > ram_memory

    /* Data section */
    RAM_DATA :
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

    /* Picolibc thread-local storage. The boot TLS block is consumed by
     * bflb_picolibc_init_boot_tls() before main(); FreeRTOS allocates a TLS
     * block from each task stack afterwards. */
    __tls_load_addr = LOADADDR(RAM_DATA) + SIZEOF(RAM_DATA);

    ram_tdata : AT (__tls_load_addr)
    {
        *(.tdata .tdata.* .gnu.linkonce.td.*)
    } > ram_memory

    ram_tbss (NOLOAD) :
    {
        *(.tbss .tbss.* .gnu.linkonce.tb.*)
        *(.tcommon)
        __tls_end = .;
    } > ram_memory

    PROVIDE(__tdata_start = ADDR(ram_tdata));
    PROVIDE(__tdata_source = LOADADDR(ram_tdata));
    PROVIDE(__tdata_size = SIZEOF(ram_tdata));
    PROVIDE(__tbss_start = ADDR(ram_tbss));
    PROVIDE(__tbss_offset = ADDR(ram_tbss) - ADDR(ram_tdata));
    PROVIDE(__tbss_size = SIZEOF(ram_tbss));
    PROVIDE(__tls_base = ADDR(ram_tdata));
    PROVIDE(__tls_align = MAX(MAX(ALIGNOF(ram_tdata), ALIGNOF(ram_tbss)), 4));
    PROVIDE(__tls_size = __tls_end - __tls_base);

    ram_tbss_space (NOLOAD) :
    {
        . = ADDR(ram_tbss);
        . = . + SIZEOF(ram_tbss);
    } > ram_memory

    ram_tls_boot (NOLOAD) :
    {
        . = ALIGN(__tls_align);
        PROVIDE(__boot_tls_start = .);
        . = . + __tls_size;
    } > ram_memory


    __ram_load_addr = LOADADDR(RAM_DATA);
    __etext_final = (__dtcm_load_addr + SIZEOF(.dtcm_region));
    ASSERT(__etext_final <= ORIGIN(code_memory) + LENGTH(code_memory), "code memory overflow")

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
        . = ALIGN(4);
        __HeapBase = .;

        KEEP(*(.heap*))

        . = ALIGN(4);
        __HeapLimit = .;
    } > ram_memory

    __HeapLimit = ORIGIN(ram_memory) + LENGTH(ram_memory);
    ASSERT(__HeapLimit - __HeapBase >= HeapMinSize, "heap region overflow")

}

