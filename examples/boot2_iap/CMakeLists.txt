list(APPEND ADD_INCLUDE 
"${CMAKE_CURRENT_SOURCE_DIR}"
)

set(TARGET_REQUIRED_LIBS xz)

list(APPEND TARGET_REQUIRED_SRCS blsp_common.c blsp_media_boot.c )
list(APPEND TARGET_REQUIRED_SRCS blsp_boot_parser.c blsp_boot_decompress.c blsp_port.c )
list(APPEND TARGET_REQUIRED_SRCS bflb_eflash_loader_uart.c  ) #bflb_eflash_loader_gpio.c
list(APPEND TARGET_REQUIRED_SRCS bflb_eflash_loader_cmds.c )
SET(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/blsp_boot2_iap_flash.ld)
set(mains blsp_boot2_iap.c)
generate_bin()



