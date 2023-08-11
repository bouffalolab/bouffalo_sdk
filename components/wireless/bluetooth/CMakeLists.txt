include(${CMAKE_CURRENT_SOURCE_DIR}/ble_common.cmake)

add_subdirectory(btblecontroller)
if(NOT CONFIG_BLE_HOST_DISABLE)
    add_subdirectory(blestack)
    sdk_add_subdirectory_ifdef(CONFIG_BLSYNC blsync)
	if (CONFIG_BT_MESH)
        add_subdirectory(blemesh)
        sdk_add_subdirectory_ifdef(CONFIG_BT_MESH_MODEL blemesh_model)
    endif()
	sdk_add_subdirectory_ifdef(CONFIG_BT_BREDR btprofile)
endif()

# disable undef warning
sdk_add_compile_options(-Wno-undef)
