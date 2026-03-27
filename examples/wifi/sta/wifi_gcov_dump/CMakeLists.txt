cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

# Link with gcov runtime to support macsw library coverage
sdk_add_link_options(--coverage)

# Wrap stdio functions to redirect to FatFS for GCOV data writing
sdk_add_link_options(-Wl,--wrap=fopen)
sdk_add_link_options(-Wl,--wrap=fclose)
sdk_add_link_options(-Wl,--wrap=fwrite)
sdk_add_link_options(-Wl,--wrap=fflush)
sdk_add_link_options(-Wl,--wrap=__gcov_lock)
sdk_add_link_options(-Wl,--wrap=__gcov_unlock)

sdk_add_compile_options(
-Wno-implicit-function-declaration
-Wno-incompatible-pointer-types
-Wno-int-conversion
-Wno-return-mismatch
)


if(CONFIG_WIFI_GATEWAY)
sdk_add_compile_definitions(-DFHOST_BUFFER_COPY=1)
sdk_add_compile_definitions(-DFHOST_RX_BUF_CNT=48)
sdk_add_compile_definitions(-DFHOST_RX_BUF_SECTION=\".bss\")
sdk_add_compile_definitions(-DCONFIG_WIFI_GATEWAY)
sdk_add_compile_definitions(-DCFG_ENABLE_AP_PS)
#sdk_add_compile_definitions(-DLWIP_HEAP_SIZE=\(32*1024\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_TCP=\(30*60*1000\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_TCP_DISCON=\(2*20*1000\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_UDP=\(620*1000\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_ICMP=\(5*2*1000\))
endif()

target_sources(app PRIVATE
	           fatfs_show.c gcov_flush.c gcov_stubs.c)

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(wifi_gcov_dump)
