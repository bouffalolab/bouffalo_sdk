if((${CHIP} STREQUAL "bl702"))
    set(TARGET_REQUIRED_LIBS xz usb_stack)
    set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
    set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/usb)
elseif ((${CHIP} STREQUAL "bl602"))
    set(TARGET_REQUIRED_LIBS xz)
endif()



list(APPEND TARGET_REQUIRED_SRCS blsp_common.c blsp_media_boot.c )
list(APPEND TARGET_REQUIRED_SRCS blsp_boot_parser.c blsp_boot_decompress.c blsp_port.c )
list(APPEND TARGET_REQUIRED_SRCS bflb_eflash_loader_uart.c  ) #bflb_eflash_loader_gpio.c
list(APPEND TARGET_REQUIRED_SRCS bflb_eflash_loader_cmds.c )

list(APPEND TARGET_REQUIRED_SRCS bflb_eflash_loader_interface.c )
if((${CHIP} STREQUAL "bl702"))
    list(APPEND TARGET_REQUIRED_SRCS bflb_eflash_loader_usb.c )
    list(APPEND TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/usb/usb_dc.c ${BSP_COMMON_DIR}/usb/uart_interface.c )
endif()
SET(LINKER_SCRIPT ${BOOT2_LINKER_SCRIPT})  


set(mains main.c)
generate_bin()



