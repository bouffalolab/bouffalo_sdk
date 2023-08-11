sdk_add_include_directories(.)

if(CONFIG_NEWLIB_STANDARD)
    sdk_add_compile_definitions(-DCONFIG_NEWLIB_STANDARD=1)
    get_target_property(SDK_INTF_LIB_LINK_OPTIONS sdk_intf_lib INTERFACE_LINK_OPTIONS)
    list(REMOVE_ITEM SDK_INTF_LIB_LINK_OPTIONS --specs=nano.specs)
    set_target_properties(sdk_intf_lib PROPERTIES INTERFACE_LINK_OPTIONS "")

    foreach(item ${SDK_INTF_LIB_LINK_OPTIONS})
        sdk_add_link_options(${item})
    endforeach()
endif()

sdk_library_add_sources(port_memory.c)

# newlib syscalls and ports
if(CONFIG_NEWLIB)
    sdk_add_compile_definitions(-DCONFIG_NEWLIB=1)

    sdk_library_add_sources(syscalls.c)
    sdk_library_add_sources(port_tty.c)
    sdk_library_add_sources(port_time.c)
    sdk_library_add_sources(port_init_fini.c)

    if(CONFIG_NEWLIB_FATFS)
        sdk_add_compile_definitions(-DCONFIG_NEWLIB_FATFS=1)
        sdk_library_add_sources(port_file_fatfs.c)
    else()
        sdk_library_add_sources(port_file_nosys.c)
    endif()

    if(NOT CONFIG_NEWLIB_STANDARD)
        # newlib nano printf config
        if(NOT ((CONFIG_VSNPRINTF_FLOAT EQUAL 0) AND (CONFIG_VSNPRINTF_FLOAT_EX EQUAL 0)))
            sdk_add_link_options(-u_printf_float)
        endif()
    endif()
else()
    sdk_library_add_sources(syscalls_nosys.c)
endif()

if(NOT CONFIG_NEWLIB_STANDARD)
    # newlib nano scanf config
    if(CONFIG_SCANF_FLOAT OR CONFIG_SCANF_FLOAT_EX)
        sdk_add_link_options(-u_scanf_float)
    endif()
endif()