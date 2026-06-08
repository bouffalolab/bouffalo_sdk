cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

if(CONFIG_GCOV_DUMP)
    # Link with gcov runtime to support macsw library coverage
    sdk_add_link_options(--coverage)

    # Wrap stdio functions to redirect to LittleFS for GCOV data writing
    sdk_add_link_options(-Wl,--wrap=fopen)
    sdk_add_link_options(-Wl,--wrap=fclose)
    sdk_add_link_options(-Wl,--wrap=fwrite)
    sdk_add_link_options(-Wl,--wrap=fflush)
    sdk_add_link_options(-Wl,--wrap=__gcov_lock)
    sdk_add_link_options(-Wl,--wrap=__gcov_unlock)

    target_sources(app PRIVATE
	           lfs_show.c lfs_gcov_flush.c lfs_gcov_stubs.c gcov_test_cmds.c)

    target_include_directories(app PRIVATE
        ${BL_SDK_BASE}/components/wireless/macsw/include
        ${BL_SDK_BASE}/components/wireless/macsw/inc
        ${BL_SDK_BASE}/components/wireless/macsw
        ${BL_SDK_BASE}/components/wireless/macsw/plf/refip/src/arch/risc-v
        ${BL_SDK_BASE}/components/wireless/macsw/plf/refip/src/arch/risc-v/ll/thead_e907
        ${BL_SDK_BASE}/components/wireless/macsw/plf/refip/src/driver/csidma)
endif()

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(macsw_test)
