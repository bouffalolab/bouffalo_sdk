sdk_generate_library()

sdk_library_add_sources(startup/start.S)
sdk_library_add_sources(startup/start_load.c)
sdk_library_add_sources(startup/system_bl602.c)
sdk_library_add_sources(startup/interrupt.c)

if(CONFIG_ROMAPI)
sdk_library_add_sources(src/bl602_romapi.c)
sdk_add_compile_definitions(-DBFLB_USE_ROM_DRIVER)
endif()

sdk_library_add_sources(src/bl602_aon.c)
sdk_library_add_sources(src/bl602_common.c)
sdk_library_add_sources(src/bl602_clock.c)
sdk_library_add_sources(src/bl602_ef_cfg.c)
sdk_library_add_sources(src/bl602_glb.c)
sdk_library_add_sources(src/bl602_hbn.c)
sdk_library_add_sources(src/bl602_l1c.c)
sdk_library_add_sources(src/bl602_pds.c)
sdk_library_add_sources(src/bl602_tzc_sec.c)

sdk_library_add_sources(src/bl602_sflash_ext.c)
sdk_library_add_sources(src/bl602_xip_sflash_ext.c)
sdk_library_add_sources(src/bl602_sf_cfg_ext.c)

sdk_library_add_sources(port/bl602_clock.c)
sdk_library_add_sources(src/bl602_pm.c)

sdk_add_include_directories(
include
include/hardware
)

SET(MCPU "riscv-e24")
SET(MARCH "rv32imafc")
SET(MABI "ilp32f")

sdk_add_compile_definitions(-DARCH_RISCV -DBFLB_USE_HAL_DRIVER)
sdk_add_compile_options(-march=${MARCH} -mabi=${MABI})
sdk_add_link_options(-march=${MARCH} -mabi=${MABI})

sdk_add_compile_definitions(-DCONFIG_IRQ_NUM=80)