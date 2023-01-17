sdk_generate_library()
sdk_library_add_sources(diskio.c)
sdk_library_add_sources(ff.c)
sdk_library_add_sources(ffsystem.c)
sdk_library_add_sources(ffunicode.c)
sdk_library_add_sources(diskio.c)
sdk_add_include_directories(.)
sdk_add_compile_definitions(-DCONFIG_FATFS)

if(CONFIG_FF_FS_REENTRANT)
    sdk_add_compile_definitions(-DFF_FS_REENTRANT=${CONFIG_FF_FS_REENTRANT})
endif()

# fatfs use sdcard over sdio
if(CONFIG_FATFS_SDH_SDCARD)
sdk_add_compile_definitions(-DCONFIG_FATFS_SDH_SDCARD)
sdk_library_add_sources(port/fatfs_sdh_sdcard.c)
endif()