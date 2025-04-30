sdk_generate_library()

string(TOUPPER "${CHIP}" upper_case_chip)
string(TOLOWER "${CHIP}" lower_case_chip)

sdk_add_private_compile_definitions(-D${upper_case_chip})
sdk_add_private_compile_definitions(-DCHIP_HDR=<${lower_case_chip}.h>)
sdk_add_private_compile_definitions(-DCHIP_GLB_HDR=<${lower_case_chip}_glb.h>)
sdk_add_private_compile_definitions(-DCHIP_SYS_HDR=<${lower_case_chip}_sys.h>)
sdk_add_private_compile_definitions(-DCONFIG_BL_SDK)

if ("${upper_case_chip}" STREQUAL "BL616")
  sdk_add_private_compile_definitions(-DMAC_ADDRESS_MAX_NUM=3)
else ()
  sdk_add_private_compile_definitions(-DMAC_ADDRESS_MAX_NUM=2)
endif()

sdk_add_include_directories(./include)

if (CONFIG_LITTLEFS)
  sdk_library_add_sources(./ot_settings_littlefs.c)
elseif (CONFIG_EASYFLASH4)
  sdk_library_add_sources(./ot_settings_easyflash.c)
  message(WARNING "Easyflash is deprecated. Better to use littlefs/CONFIG_LITTLEFS for faster storage access.")
else ()
  message(FATAL_ERROR "Please select one of CONFIG_LITTLEFS and CONFIG_EASYFLASH4 for OpenThread setting storage.")
endif()

if (NOT CONFIG_MATTER)
  sdk_library_add_sources(./ot_freertos.c)
endif()

sdk_library_add_sources(./ot_sys_bflb.c)
sdk_library_add_sources(./ot_alarm_${lower_case_chip}.c)
sdk_library_add_sources(./ot_radio.c)
sdk_library_add_sources(./ot_radio_bflb.c)

if (CONFIG_OTBR)
  sdk_add_private_include_directories(.)
  sdk_library_add_sources(./otbr_lwip.c)
endif()

if (NOT DEFINED OT_NCP)
  sdk_library_add_sources(./ot_logging.c)
endif()

if ("${OT_SERIAL}" STREQUAL "SYS_CLI")
  sdk_library_add_sources(./ot_shell.c)
elseif ("${OT_SERIAL}" STREQUAL "UART") 
  sdk_library_add_sources(./ot_uart_bflb.c)
  if ($(OT_SERIAL_BUFFER_SIZE)) 
    sdk_add_private_compile_definitions(-DOT_SERIAL_BUFFER_SIZE=${OT_SERIAL_BUFFER_SIZE})
  endif()
  if ($(OT_SERIAL_LINE_SIZE)) 
    sdk_add_private_compile_definitions(-DOT_SERIAL_LINE_SIZE=${OT_SERIAL_LINE_SIZE})
  endif()
else ()
  message (FATAL_ERROR "openthread_port: ${OT_SERIAL} is not available for OT_SERIAL.")
endif()

# disable undef warning
# sdk_add_compile_options(-Wno-undef)
