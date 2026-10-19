sdk_add_subdirectory_ifdef(CONFIG_FATFS fatfs)
sdk_add_subdirectory_ifdef(CONFIG_ROMFS romfs)

if(CONFIG_EASYFLASH4 OR CONFIG_LITTLEFS)
  if(CONFIG_EASYFLASH4)
    message(WARNING "Easyflash is deprecated. Use littlefs/CONFIG_LITTLEFS for storage access.")
  endif()
  add_subdirectory(littlefs)
endif()
