include(${CMAKE_CURRENT_SOURCE_DIR}/ble_common.cmake)

add_subdirectory(btblecontroller)
if(NOT CONFIG_BLE_HOST_DISABLE)
    add_subdirectory(blestack)
	if (CONFIG_BT_MESH)
        add_subdirectory(blemesh)
        if (CONFIG_BT_MESH_MODEL)
            add_subdirectory(blemesh_model)
        endif()
    endif()
endif()

# sdk_add_compile_definitions(${CFLAGS})
