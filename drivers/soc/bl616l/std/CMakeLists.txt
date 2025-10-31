sdk_generate_library()

sdk_library_add_sources(startup/start.S)
sdk_library_add_sources(startup/vector.S)
sdk_library_add_sources(startup/riscv_fpu.S)
sdk_library_add_sources(startup/start_load.c)
sdk_library_add_sources(startup/system_bl616l.c)
sdk_library_add_sources(startup/interrupt.c)

# set CONFIG_ROMAPI n before chip tapeout
unset(CONFIG_ROMAPI CACHE)
set(CONFIG_ROMAPI
    n
    CACHE INTERNAL "Override CONFIG_ROMAPI for bl616l"
)

if(CONFIG_ROMAPI)
    sdk_library_add_sources(src/bl616l_romapi_e907.c)
    sdk_library_add_sources(src/bl616l_romapi_patch.c)
    sdk_add_compile_definitions(-DBFLB_USE_ROM_DRIVER)
else()
    sdk_library_add_sources(src/bl616l_aon.c)
    sdk_library_add_sources(src/bl616l_clock.c)
    sdk_library_add_sources(src/bl616l_common.c)
    sdk_library_add_sources(src/bl616l_ef_cfg.c)
    sdk_library_add_sources(src/bl616l_glb.c)
    sdk_library_add_sources(src/bl616l_hbn.c)
    sdk_library_add_sources(src/bl616l_pds.c)
endif()
sdk_library_add_sources(src/bl616l_sec_dbg.c)
sdk_library_add_sources(src/bl616l_tzc_sec.c)
sdk_library_add_sources(src/bl616l_psram.c)
sdk_library_add_sources(src/bl616l_pm.c)

sdk_library_add_sources(port/bl616l_clock.c)
sdk_library_add_sources(port/bl616l_reset.c)

if(CONFIG_RF)
    sdk_library_add_sources(src/bl616l_mfg_efuse.c)
    sdk_library_add_sources(src/bl616l_mfg_flash.c)
    sdk_library_add_sources(src/bl616l_mfg_media.c)
endif()

if(CONFIG_STD_BOOTROM_ROMDRIVER)
    sdk_library_add_sources(src/bl616l_romdriver_e907.c)
endif()

sdk_add_include_directories(include include/hardware)

set(MCPU "e907")
set(MARCH "rv32imafc_xtheade")
set(MABI "ilp32f")

sdk_add_compile_definitions(-DARCH_RISCV -DBFLB_USE_HAL_DRIVER)
sdk_add_compile_options(-march=${MARCH} -mabi=${MABI} -mtune=${MCPU})
sdk_add_link_options(-march=${MARCH} -mabi=${MABI} -mtune=${MCPU})
sdk_add_private_compile_definitions_ifdef(CONFIG_USB_DISABLED -DCONFIG_USB_DISABLED)

sdk_add_compile_definitions(-DCONFIG_IRQ_NUM=83)
