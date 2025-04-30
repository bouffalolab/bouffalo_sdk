include(${CMAKE_CURRENT_SOURCE_DIR}/ble_common.cmake)


if((${CHIP} STREQUAL "bl702"))
add_subdirectory(blecontroller)
else()
add_subdirectory(btblecontroller)
endif()

if(NOT CONFIG_BLE_HOST_DISABLE)
    add_subdirectory(blestack)
	if (CONFIG_BT_MESH)
        add_subdirectory(blemesh)
        sdk_add_subdirectory_ifdef(CONFIG_BT_MESH_MODEL blemesh_model)
    endif()
	sdk_add_subdirectory_ifdef(CONFIG_BT_BREDR btprofile)
        sdk_add_subdirectory_ifdef(CONFIG_AUTO_PTS bleptstest)
endif()

# disable undef warning
sdk_add_compile_options(-Wno-undef)
