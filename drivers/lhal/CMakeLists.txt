sdk_generate_library()

sdk_library_add_sources(src/bflb_common.c)

sdk_library_add_sources(src/bflb_adc.c)
sdk_library_add_sources(src/bflb_dac.c)
sdk_library_add_sources(src/bflb_dma.c)
if((NOT ("${CHIP}" STREQUAL "bl602")))
sdk_library_add_sources(src/bflb_emac.c)
endif()
sdk_library_add_sources(src/bflb_gpio.c)
sdk_library_add_sources(src/bflb_i2c.c)
sdk_library_add_sources(src/bflb_ir.c)
sdk_library_add_sources(src/bflb_uart.c)
sdk_library_add_sources(src/bflb_spi.c)
sdk_library_add_sources(src/bflb_rtc.c)
sdk_library_add_sources(src/bflb_sec_aes.c)
sdk_library_add_sources(src/bflb_sec_sha.c)
sdk_library_add_sources(src/bflb_sec_trng.c)
# sdk_library_add_sources(src/bflb_sec_pka.c)
sdk_library_add_sources(src/bflb_timer.c)
sdk_library_add_sources(src/bflb_wdg.c)

# sdk_library_add_sources(src/bflb_clock.c)

if(("${CHIP}" STREQUAL "bl702") OR ("${CHIP}" STREQUAL "bl602"))
sdk_library_add_sources(src/bflb_pwm_v1.c)
elseif("${CHIP}" STREQUAL "bl702l")
sdk_library_add_sources(src/bflb_pwm_v1.c)
sdk_library_add_sources(src/bflb_pwm_v2.c)
else()
sdk_library_add_sources(src/bflb_pwm_v2.c)
endif()

if(CONFIG_CHERRYUSB)
if("${CHIP}" STREQUAL "bl702")
sdk_library_add_sources(src/bflb_usb_v1.c)
elseif(("${CHIP}" STREQUAL "bl602") OR ("${CHIP}" STREQUAL "bl702l"))
# no usb
else()
sdk_library_add_sources(src/bflb_usb_v2.c)
endif()
endif()

# optional
sdk_library_add_sources(src/bflb_irq.c)
sdk_library_add_sources(src/bflb_mtimer.c)

sdk_add_include_directories(include)
sdk_add_include_directories(include/arch)
sdk_add_include_directories(include/arch/risc-v/t-head)
sdk_add_include_directories(include/arch/risc-v/t-head/Core/Include)
sdk_add_include_directories(config/${CHIP})

if((NOT ("${CHIP}" STREQUAL "bl702")) AND (NOT ("${CHIP}" STREQUAL "bl602")) AND (NOT ("${CHIP}" STREQUAL "bl702l")))
sdk_library_add_sources(include/arch/risc-v/t-head/rv_hart.c)
sdk_library_add_sources(include/arch/risc-v/t-head/rv_pmp.c)
endif()

sdk_library_add_sources(config/${CHIP}/device_table.c)

string(TOUPPER ${CHIP} CHIPNAME)
sdk_add_compile_definitions(-D${CHIPNAME})

if(CPU_ID)
string(TOUPPER ${CPU_ID} CPU_ID_NAME)
sdk_add_compile_definitions(-DCPU_${CPU_ID_NAME})
endif()