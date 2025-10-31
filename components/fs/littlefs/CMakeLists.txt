sdk_generate_library()
sdk_library_add_sources(littlefs/lfs_util.c)
sdk_library_add_sources(littlefs/lfs.c)
sdk_add_include_directories(littlefs)

# sdk_library_add_sources(littlefs/bd/lfs_filebd.c.c)
# sdk_library_add_sources(littlefs/bd/lfs_rambd.c.c)
# sdk_library_add_sources(littlefs/bd/lfs_testbd.c.c)
# sdk_add_include_directories(littlefs/bd)
sdk_library_add_sources(port/lfs_xip_flash.c)
sdk_add_include_directories(port)

if(CONFIG_FREERTOS)
    sdk_add_compile_definitions(-DLFS_THREADSAFE)
endif()

sdk_library_add_sources(easyflash_port/lfs_easyflash.c)
sdk_add_include_directories(easyflash_port)
