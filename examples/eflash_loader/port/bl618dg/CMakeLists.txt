if(NOT "${CPU_ID}" STREQUAL "ap")
    message(FATAL_ERROR "eflash_loader must run on the BL618DG AP core")
endif()

target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/flashloader_port.c)
sdk_add_link_options(-Wl,-T,${CMAKE_CURRENT_LIST_DIR}/flashloader_memory.ld)
