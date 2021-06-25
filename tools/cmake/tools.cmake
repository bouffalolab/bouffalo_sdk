function(generate_library)
    get_filename_component(library_name ${CMAKE_CURRENT_LIST_DIR} NAME)
    message(STATUS "[register library component: ${library_name}], path:${CMAKE_CURRENT_LIST_DIR}")

    add_library(${library_name})

    # Add src to lib
    if(ADD_SRCS)
        target_sources(${library_name} PRIVATE ${ADD_SRCS})
    endif()

    # Add global config include
    if(ADD_INCLUDE)
    foreach(include_dir ${ADD_INCLUDE})
        get_filename_component(abs_dir ${include_dir} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
        if(NOT IS_DIRECTORY ${abs_dir})
            message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE}: ${include_dir} not found!")
        endif()
        target_include_directories(${library_name} PUBLIC ${abs_dir})
    endforeach()
    endif()

    # Add private include
    if(ADD_PRIVATE_INCLUDE)
    foreach(include_dir ${ADD_PRIVATE_INCLUDE})
        get_filename_component(abs_dir ${include_dir} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
        if(NOT IS_DIRECTORY ${abs_dir})
            message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE}: ${include_dir} not found!")
        endif()
        target_include_directories(${library_name} PRIVATE ${abs_dir})
    endforeach()
    endif()

    # Add definitions public
    if(ADD_DEFINITIONS)
    target_compile_options(${library_name} PUBLIC ${ADD_DEFINITIONS})
    endif()

    # Add definitions private
    if(ADD_PRIVATE_DEFINITIONS)
        target_compile_options(${library_name} PRIVATE ${ADD_DEFINITIONS_PRIVATE})
    endif()

    # Add requirements
    if(ADD_REQUIREMENTS)
        foreach(lib ${ADD_REQUIREMENTS})
            if(TARGET ${lib})
            add_dependencies(${library_name} ${lib})
            target_link_libraries(${library_name} ${lib})
            else()
            message(FATAL_ERROR "${lib} is not a target")
            endif()
        endforeach()
    endif()

    # Add static lib
    if(ADD_STATIC_LIB)
        foreach(lib ${ADD_STATIC_LIB})
            if(NOT EXISTS "${lib}")
                message(FATAL_ERROR "Can not find ${lib}")
            endif()
            get_filename_component(static_lib_relative_dir ${lib} DIRECTORY)
            get_filename_component(static_lib_name ${lib} NAME)
            target_link_directories(${library_name} PUBLIC ${static_lib_relative_dir})
            target_link_libraries(${library_name} ${static_lib_name})
        endforeach()
    endif()

    # Add dynamic lib
    if(ADD_DYNAMIC_LIB)
        foreach(lib ${ADD_DYNAMIC_LIB})
            if(NOT EXISTS "${lib}")
                message(FATAL_ERROR "Can not find ${lib}")
            endif()
            get_filename_component(dynamic_lib_relative_dir ${lib} DIRECTORY)
            get_filename_component(dynamic_lib_name ${lib} NAME)
            target_link_directories(${library_name} PUBLIC ${dynamic_lib_relative_dir})
            target_link_libraries(${library_name} ${dynamic_lib_name})
        endforeach()
    endif()
endfunction()

function(generate_bin)

    get_filename_component(current_relative_dir_name ${CMAKE_CURRENT_LIST_DIR} NAME)
    string(REGEX REPLACE "(.*)/${current_relative_dir_name}$" "\\1" above_absolute_dir ${CMAKE_CURRENT_LIST_DIR})
    get_filename_component(above_relative_dir_name ${above_absolute_dir} NAME)

    # Add common options
    add_compile_options(${GLOBAL_C_FLAGS})
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-std=c99>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-nostdlib>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)

    check_all_library()

    foreach(mainfile IN LISTS mains)
    # Get file name without directory
    get_filename_component(mainname ${mainfile} NAME_WE)

	if(DEFINED OUTPUT)
        set(OUTPUT_DIR ${OUTPUT})
        set(target_name firmware)
    else()
        if(${above_relative_dir_name} STREQUAL "examples")
            set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/out/${APP_DIR}/${current_relative_dir_name})
            set(target_name ${current_relative_dir_name}_${mainname})
        else()
            set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/out/${APP_DIR}/${above_relative_dir_name}/${current_relative_dir_name})
            set(target_name ${current_relative_dir_name}_${mainname})
        endif()

    endif()

	file(MAKE_DIRECTORY ${OUTPUT_DIR})
	set(HEX_FILE ${OUTPUT_DIR}/${target_name}.hex)
    set(BIN_FILE ${OUTPUT_DIR}/${target_name}.bin)
    set(MAP_FILE ${OUTPUT_DIR}/${target_name}.map)
    set(ASM_FILE ${OUTPUT_DIR}/${target_name}.asm)

    if(TARGET_REQUIRED_SRCS)
        foreach(src ${TARGET_REQUIRED_SRCS})
            if((NOT EXISTS ${src}) AND (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${src}))
            message(FATAL_ERROR "${src} not exist,maybe you should autocomplete your path\r\n")
            endif()
            list(APPEND SRCS ${src})
        endforeach()

    endif()

    list(APPEND SRCS ${CMAKE_SOURCE_DIR}/bsp/bsp_common/platform/bflb_platform.c)
    list(APPEND SRCS ${CMAKE_SOURCE_DIR}/bsp/bsp_common/platform/syscalls.c)
    list(APPEND SRCS ${CMAKE_SOURCE_DIR}/bsp/board/${BOARD}/board.c)

    add_executable(${target_name}.elf ${mainfile} ${SRCS})
    target_link_options(${target_name}.elf PRIVATE ${GLOBAL_LD_FLAGS})
    set_target_properties(${target_name}.elf PROPERTIES LINK_FLAGS "-T${LINKER_SCRIPT} -Wl,-Map=${MAP_FILE}")
    set_target_properties(${target_name}.elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

    set_target_properties(${target_name}.elf PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

    # Add private include
    foreach(include_dir ${TARGET_REQUIRED_PRIVATE_INCLUDE})
        get_filename_component(abs_dir ${include_dir} ABSOLUTE BASE_DIR ${CMAKE_SOURCE_DIR})
        if(NOT IS_DIRECTORY ${abs_dir})
            message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE}: ${include_dir} not found!")
        endif()
        target_include_directories(${target_name}.elf PRIVATE ${abs_dir})
    endforeach()

    # Add definitions private
    if(TARGET_REQUIRED_PRIVATE_OPTIONS)
    target_compile_options(${target_name}.elf PRIVATE ${TARGET_REQUIRED_PRIVATE_OPTIONS})
    endif()

    add_dependencies(${target_name}.elf ${CHIP}_driver)
    # Add libs
    target_link_libraries(${target_name}.elf ${CHIP}_driver c)

    if(TARGET_REQUIRED_LIBS)
        target_link_libraries(${target_name}.elf ${TARGET_REQUIRED_LIBS})
    endif()

    target_link_libraries(${target_name}.elf m)

    add_custom_command(TARGET ${target_name}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${target_name}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_OBJDUMP} -d -S $<TARGET_FILE:${target_name}.elf> >${ASM_FILE}
        # COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${mainname}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE} ${CMAKE_SOURCE_DIR}/tools/bflb_flash_tool/img/project.bin
        COMMENT "Generate ${BIN_FILE}\r\nCopy ${BIN_FILE} into download path")
    endforeach()
endfunction(generate_bin)

function(search_application component_path)

if(DEFINED APP)

    file(GLOB_RECURSE cmakelists_files ${component_path}/CMakeLists.txt)
    set(app_find_ok 0)

    foreach(cmakelists_file ${cmakelists_files})
    get_filename_component(app_relative_dir ${cmakelists_file} DIRECTORY)
    get_filename_component(app_name ${app_relative_dir} NAME)
        if(${APP} STREQUAL "all")
            message(STATUS "[run app:${app_name}], path:${app_relative_dir}")
            add_subdirectory(${app_relative_dir})
            set(app_find_ok 1)
        elseif(${app_name} MATCHES "^${APP}")
            message(STATUS "[run app:${app_name}], path:${app_relative_dir}")
            add_subdirectory(${app_relative_dir})
            set(app_find_ok 1)
        endif()
    endforeach()
    if(NOT app_find_ok)
    message(FATAL_ERROR "can not find app:${APP}")
    endif()
else()
add_subdirectory($ENV{PROJECT_DIR}/src src)
endif()

endfunction()

function(check_add_library target_name directory)
    if(NOT TARGET ${target_name})
    add_subdirectory(${directory} ${CMAKE_SOURCE_DIR}/build/libraries/${target_name})
    endif()
endfunction()

function(check_all_library)
    check_add_library(common ${CMAKE_SOURCE_DIR}/common)
    check_add_library(fatfs ${CMAKE_SOURCE_DIR}/components/fatfs)
    check_add_library(usb_stack ${CMAKE_SOURCE_DIR}/components/usb_stack)

    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/drivers/${CHIP}_driver)
    check_add_library(${CHIP}_driver ${CMAKE_SOURCE_DIR}/drivers/${CHIP}_driver)
    endif()

    if(${SUPPORT_SHELL} STREQUAL "y")
    check_add_library(shell ${CMAKE_SOURCE_DIR}/components/shell)
    endif()
    if(${SUPPORT_FREERTOS} STREQUAL "y")
    check_add_library(freertos ${CMAKE_SOURCE_DIR}/components/freertos)
    endif()

    if(${SUPPORT_CRYPTO} STREQUAL "sw" OR ${SUPPORT_CRYPTO} STREQUAL "hw")
    check_add_library(bflb_port ${CMAKE_SOURCE_DIR}/components/mbedtls/bflb_port)
    endif()

    if(${SUPPORT_LVGL} STREQUAL "y")
    check_add_library(lvgl ${CMAKE_SOURCE_DIR}/components/lvgl)
    endif()

    if(${SUPPORT_XZ} STREQUAL "y")
    check_add_library(xz ${CMAKE_SOURCE_DIR}/components/xz)
    endif()

    if(${SUPPORT_BLE} STREQUAL "y")
    if(${SUPPORT_FREERTOS} STREQUAL "n")
    message(FATAL_ERROR "ble need freertos,so you should set SUPPORT_FREERTOS=y")
    endif()
    check_add_library(ble ${CMAKE_SOURCE_DIR}/components/ble)
    endif()

    if(${SUPPORT_LWIP} STREQUAL "y")
    if(${SUPPORT_FREERTOS} STREQUAL "n")
    message(FATAL_ERROR "lwip need freertos,so you should set SUPPORT_FREERTOS=y")
    endif()
    check_add_library(lwip ${CMAKE_SOURCE_DIR}/components/lwip)
    endif()

    if(${SUPPORT_TFLITE} STREQUAL "y")
    check_add_library(xz ${CMAKE_SOURCE_DIR}/components/tflite)
    endif()

endfunction(check_all_library)
