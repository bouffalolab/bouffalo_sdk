/****************************************************************************************
* @file bl702_flash.ld
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
EXTERN(board_psram_init)

StackSize    = 0x0800; /*  2KB */
HeapMinSize  = 0x1000; /*  4KB */
/* BLE EM is carved out of the top of the SRAM range when the ble
 * controller is linked; otherwise the 8 KiB returns to the heap. */
__EM_SIZE = DEFINED(ble_controller_init) ? 8K : 0K;

MEMORY
{
    fw_header_memory  (rx)  : ORIGIN = 0x23000000 - 0x1000, LENGTH = 4K
    xip_memory  (rx)  : ORIGIN = 0x23000000, LENGTH = 1024K
    /* 0x22014000 and 0x42014000 are the instruction/data aliases of the
     * same 112 KiB OCRAM; EM is carved from the top of the range. */
    itcm_memory (rx)  : ORIGIN = 0x22014000, LENGTH = 112K - __EM_SIZE
    ram_memory  (!rx) : ORIGIN = 0x42014000, LENGTH = 112K - __EM_SIZE
    hbn_memory  (rx)  : ORIGIN = 0x40010000, LENGTH = 0xE00     /* hbn ram 4K used 3.5K*/
    /* BL702 PSRAM is at 0x26000000. */
    psram_memory (wxa) : ORIGIN = 0x26000000, LENGTH = DEFINED(board_psram_init) ? 2M : 0
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

    .rom_header ORIGIN(xip_memory) + 512 :
    {
        __multi_bins__ = ABSOLUTE(.);
        LONG(0x464C4553) LONG(0x00000000) /*SELF*/      LONG(4K)         LONG(4K)
        LONG(0x52415744) LONG(0x49464346) /*DWARFCFI*/  LONG(0x00000000) LONG(0x00000000)
        LONG(0x464D4F52) LONG(0x00000053) /*ROMFS*/     LONG(0x00000000) LONG(0x00000000)
        LONG(0x54524543) LONG(0x00000053) /*CERTS*/     LONG(0x00000000) LONG(0x00000000)
        LONG(0x4F545541) LONG(0x5453494C) /*AUTOLIST*/  LONG(0x00000000) LONG(0x00000000)
        LONG(0xFFFFFFFF) LONG(0xFFFFFFFF)               LONG(0xFFFFFFFF) LONG(0xFFFFFFFF)
    } > xip_memory

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

        /*put .rodata**/
        *(EXCLUDE_FILE( *bl702_glb*.o* \
                        *bl702_pds*.o* \
                        *bl702_common*.o* \
                        *bflb_sf_cfg*.o* \
                        *bl702_sf_cfg_ext*.o* \
                        *bflb_sf_ctrl*.o* \
                        *bflb_sflash*.o* \
                        *bl702_sflash_ext*.o* \
                        *bflb_xip_sflash*.o* \
                        *bl702_xip_sflash_ext*.o* \
                        *bflb_ef_ctrl*.o*) .rodata*)

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

        /* _dump_sections for bflb_coredump.c*/
        . = ALIGN(4);
        PROVIDE( _dump_sections = ABSOLUTE(.));
        LONG(ORIGIN(itcm_memory));
        LONG(SIZEOF(.itcm_region));
        LONG(__ram_start__);
        LONG(ORIGIN(ram_memory) + LENGTH(ram_memory) - __ram_start__);
        LONG(ORIGIN(hbn_memory));
        LONG(LENGTH(hbn_memory));
        LONG(ORIGIN(psram_memory));
        LONG(DEFINED(board_psram_init) ? LENGTH(psram_memory) : 0);
        LONG(0xffffffff);
        LONG(0xffffffff);
        /* _dump_sections for bl_coredump.c*/

        . = ALIGN(4);
        __text_code_end__ = .;
    } > xip_memory

    .sysinit.array :
    {
        . = ALIGN(4);
        __sysinit_array_start = .;
        KEEP(*(SORT_BY_NAME(.sysinit.*)))
        __sysinit_array_stop = .;
    } > xip_memory

    .build_id :
    {
        . = ALIGN(4);
        elf_build_id = ABSOLUTE(.) + 16;
        *(.note.gnu.build-id)
    } > xip_memory

    . = ALIGN(4);
    __itcm_load_addr = .;

    .itcm_region : AT (__itcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_code_start__ = .;

        *(.tcm_code.*)
        *(.tcm_const.*)
        *(.sclock_rlt_code.*)
        *(.sclock_rlt_const.*)

        *bl702_glb*.o*(.rodata*)
        *bl702_pds*.o*(.rodata*)
        *bl702_common*.o*(.rodata*)
        *bflb_sf_cfg*.o*(.rodata*)
        *bl702_sf_cfg_ext*.o*(.rodata*)
        *bflb_sf_ctrl*.o*(.rodata*)
        *bflb_sflash*.o*(.rodata*)
        *bl702_sflash_ext*.o*(.rodata*)
        *bflb_xip_sflash*.o*(.rodata*)
        *bl702_xip_sflash_ext*.o*(.rodata*)
        *bflb_ef_ctrl*.o*(.rodata*)

        . = ALIGN(4);
        __tcm_code_end__ = .;
    } > itcm_memory

    /*
     * ITCM and WRAM are two address aliases of the same physical OCRAM.
     * Start normal RAM immediately after the ITCM image's physical extent so
     * unused ITCM capacity remains available to the system heap. The 16 KiB
     * ILM window (0x22010000/0x42010000) is left untouched.
     */
    __ram_start__ = ORIGIN(ram_memory) + ADDR(.itcm_region) - ORIGIN(itcm_memory) + SIZEOF(.itcm_region);

    __hbn_load_addr = __itcm_load_addr + SIZEOF(.itcm_region);

    .hbn_ram_region : AT (__hbn_load_addr)
    {
        . = ALIGN(4);
        __hbn_ram_start__ = ADDR(.hbn_ram_region);

        /* put bl702_rf_private.o in hbnram to avoid ocram conflict during rf calibration */
        *libphyrf.a:bl702_rf_private.c.o*(.retention)
        *libphyrf.a:bl702_rf_private.c.o*(.sdata.* .sbss.* .bss.* COMMON)

        *(.hbn_code.*)
        *(.hbn_data.*)
        *(.retention)

        . = ALIGN(4);
        __hbn_ram_end__ = .;
    } > hbn_memory

    /* RF calibration state survives PDS/HBN and must not enter TLSF RAM. */
    .hbn_noinit (NOLOAD) :
    {
        . = ALIGN(4);
        *libphyrf.a:bl702_rf_private.c.o*(.retention_noinit)
        *(.retention_noinit)
        . = ALIGN(4);
    } > hbn_memory

    ASSERT(SIZEOF(.hbn_ram_region) + SIZEOF(.hbn_noinit) <= 0x400,
           "HBN retention data overlapped with IOT2LP_PARA(refer to bl_lp.h)")

    __dtcm_load_addr = __hbn_load_addr + SIZEOF(.hbn_ram_region);

    .dtcm_region __ram_start__ : AT (__dtcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_data_start__ = .;

        *(.tcm_data)
        /* *finger_print.o(.data*) */

        . = ALIGN(4);
        __tcm_data_end__ = .;
    } > ram_memory

    /*************************************************************************/
    .stack __tcm_data_end__ (NOLOAD):
    {
        . = ALIGN(16);
        __StackLimit = .;
        . = . + StackSize;
        __StackTop = .;
    } > ram_memory

    PROVIDE( __freertos_irq_stack_top = __StackTop);
    PROVIDE( _sp_main = __StackTop);
    PROVIDE( _sp_base = __StackLimit);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __tcm_data_end__, "region RAM overflowed with stack")
    /*************************************************************************/

    __system_ram_load_addr = __dtcm_load_addr + SIZEOF(.dtcm_region);

    .system_ram_data_region : AT (__system_ram_load_addr)
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
        
        . = ALIGN(4);
        __bflb_log_tags_start__ = .;
        *(.bflb_log_tags_array)
        . = ALIGN(4);
        __bflb_log_tags_end__ = .;
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
        *(.nocache_noinit_ram)
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

    ASSERT((__HeapLimit -  __HeapBase ) >= HeapMinSize, "heap size is too short.")

    PROVIDE(__LD_CONFIG_EM_SEL = __EM_SIZE);

    __psram_heap_base = ORIGIN(psram_memory);
    __psram_limit = ORIGIN(psram_memory) + LENGTH(psram_memory);

}
