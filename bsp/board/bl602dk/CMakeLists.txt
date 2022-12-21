sdk_add_include_directories(.)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board.c)

sdk_set_linker_script(bl602_flash.ld)