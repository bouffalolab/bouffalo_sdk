cmake_minimum_required(VERSION 3.19.3)

get_target_property(C_CPP_PROPERTIES_INCLUDE sdk_intf_lib INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(C_CPP_PROPERTIES_DEFINES sdk_intf_lib INTERFACE_COMPILE_DEFINITIONS)
list(SORT C_CPP_PROPERTIES_INCLUDE)
list(SORT C_CPP_PROPERTIES_DEFINES)

foreach(item ${C_CPP_PROPERTIES_INCLUDE})
    string(APPEND C_CPP_PROPERTIES_INCLUDE_IN "\n                \"${item}\",")
endforeach()

foreach(item ${C_CPP_PROPERTIES_DEFINES})
    string(REGEX REPLACE "([^\"])([\"])" "\\1\\\\\"" nitem ${item})
    string(APPEND C_CPP_PROPERTIES_DEFINES_IN "\n                \"${nitem}\",")
endforeach()

get_filename_component(SYS_INCLUDE_PATH ${CMAKE_C_COMPILER} DIRECTORY)
string(APPEND C_CPP_PROPERTIES_INCLUDE_IN "\n                \"${SYS_INCLUDE_PATH}/../riscv64-unknown-elf/include\",")

get_filename_component(BL_SDK_CMAKE_DIR ${BL_SDK_BASE}/cmake/extension.cmake ABSOLUTE)
get_filename_component(BL_SDK_CMAKE_DIR ${BL_SDK_CMAKE_DIR} DIRECTORY)

if(VSCODE_DIR)
    set(VSCODE_DIR_ ${VSCODE_DIR}/.vscode)
else()
    set(VSCODE_DIR_ ${BL_SDK_CMAKE_DIR}/../.vscode)
endif()

# may cause fails when multi-threading ci build
# configure_file(${BL_SDK_BASE}/tools/vscode/c_cpp_properties.json ${VSCODE_DIR_}/c_cpp_properties.json)
