sdk_generate_library()

sdk_library_add_sources(startup/start.S)
sdk_library_add_sources(startup/start_load.c)
sdk_library_add_sources(startup/system_bl702l.c)
sdk_library_add_sources(startup/interrupt.c)

if(CONFIG_ROMAPI)
sdk_library_add_sources(src/bl702l_romapi.c)
sdk_library_add_sources(src/bl702l_romapi_patch.c)
sdk_add_compile_definitions(-DBFLB_USE_ROM_DRIVER)
else()
sdk_library_add_sources(src/bl702l_aon.c)
sdk_library_add_sources(src/bl702l_clock.c)
sdk_library_add_sources(src/bl702l_common.c)
sdk_library_add_sources(src/bl702l_glb.c)
sdk_library_add_sources(src/bl702l_hbn.c)
sdk_library_add_sources(src/bl702l_l1c.c)
sdk_library_add_sources(src/bl702l_pds.c)
endif()

sdk_library_add_sources(src/bl702l_ef_cfg.c)
sdk_library_add_sources(src/bl702l_kys.c)
sdk_library_add_sources(src/bl702l_pm.c)
sdk_library_add_sources(src/bl702l_tzc_sec.c)

sdk_library_add_sources(port/bl702l_clock.c)

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