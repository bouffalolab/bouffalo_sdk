sdk_generate_library()
sdk_library_add_sources(diskio.c)
sdk_library_add_sources(ff.c)
sdk_library_add_sources(ffsystem.c)
sdk_library_add_sources(ffunicode.c)
sdk_library_add_sources(diskio.c)
sdk_add_include_directories(.)
sdk_add_include_directories(port)
sdk_add_compile_definitions(-DCONFIG_FATFS)

# fatfs use sdcard over sdio
if(CONFIG_FATFS_SDH_SDCARD)
sdk_add_compile_definitions(-DCONFIG_FATFS_SDH_SDCARD)
sdk_library_add_sources(port/fatfs_sdh_sdcard.c)
endif()

# fatfs use usb host
if(CONFIG_FATFS_USBH)
sdk_add_compile_definitions(-DCONFIG_FATFS_USBH)
sdk_library_add_sources(port/fatfs_usbh.c)
endif()