sdk_generate_library()

file(GLOB_RECURSE SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/bflb_pec_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bflb_pec_i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bflb_pec_ir.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bflb_pec_pwm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bflb_pec_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bflb_pec_uart_rx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bflb_pec_uart_tx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bflb_pec_dpi.c
)
sdk_library_add_sources(${SOURCES})

sdk_add_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/)
