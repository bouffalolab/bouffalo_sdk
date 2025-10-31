# POST_PROC combine
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Generic")
    set(TOOL_SUFFIX ".exe")
    set(CMAKE ${BL_SDK_BASE}/tools/cmake/bin/cmake.exe)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(TOOL_SUFFIX "-ubuntu")
    set(CMAKE cmake)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    set(TOOL_SUFFIX "-macos")
    set(CMAKE cmake)
endif()

if(EXISTS "${BL_SDK_BASE}/VERSION")
    file(READ "${BL_SDK_BASE}/VERSION" VERSION_FILE_CONTENT)

    string(REGEX MATCH "PROJECT_SDK_VERSION[ ]+\"([^\"]+)\"" _ ${VERSION_FILE_CONTENT})
    set(PROJECT_SDK_VERSION "${CMAKE_MATCH_1}")
else()
    message(WARNING "No Valid version info found for SDK!")
endif()

set(BL_FW_POST_PROC ${BL_SDK_BASE}/tools/bflb_tools/bflb_fw_post_proc/bflb_fw_post_proc${TOOL_SUFFIX})

set(BL_FW_POST_PROC_CONFIG --chipname=${CHIP} --imgfile=${BIN_FILE} --appkeys=shared)

# fw tools board directory select
get_property(FW_TOOL_BOARD_DIR GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_DIR)
if(FW_TOOL_BOARD_DIR)
    message(STATUS "Select FW_TOOL_CUSTOM_BOARD_DIR: ${FW_TOOL_BOARD_DIR}")
elseif(BOARD_DIR)
    set(FW_TOOL_BOARD_DIR ${BOARD_DIR}/${BOARD})
    message(STATUS "Select BOARD_DIR: ${FW_TOOL_BOARD_DIR}")
else()
    set(FW_TOOL_BOARD_DIR ${BL_SDK_BASE}/bsp/board/${BOARD})
    message(STATUS "Select BOARD: ${FW_TOOL_BOARD_DIR}")
endif()
# fw tools board config select
get_property(FW_TOOL_BOARD_CONFIG GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_CONFIG)
if(FW_TOOL_BOARD_CONFIG)
    message(STATUS "Select FW_TOOL_CUSTOM_BOARD_CONFIG: ${FW_TOOL_BOARD_CONFIG}")
else()
    set(FW_TOOL_BOARD_CONFIG config)
    message(STATUS "Select FW_TOOL_BOARD_CONFIG: ${FW_TOOL_BOARD_CONFIG}")
endif()

if(CONFIG_OVERRIDE_PTFILE)
    list(APPEND BL_FW_POST_PROC_CONFIG --ptfile=${SDK_DEMO_PATH}/${CONFIG_OVERRIDE_PTFILE})
endif()

if(CONFIG_OVERRIDE_DTSFILE)
    list(APPEND BL_FW_POST_PROC_CONFIG --dtsfile=${SDK_DEMO_PATH}/${CONFIG_OVERRIDE_DTSFILE})
endif()

if(CONFIG_OVERRIDE_BOOT2FILE)
    list(APPEND BL_FW_POST_PROC_CONFIG --boot2file=${SDK_DEMO_PATH}/${CONFIG_OVERRIDE_BOOT2FILE})
endif()

if(CONFIG_OVERRIDE_MFGFILE)
    list(APPEND BL_FW_POST_PROC_CONFIG --mfgfile=${SDK_DEMO_PATH}/${CONFIG_OVERRIDE_MFGFILE})
endif()

# fw tools --brdcfgdir select
list(APPEND BL_FW_POST_PROC_CONFIG --brdcfgdir=${FW_TOOL_BOARD_DIR}/${FW_TOOL_BOARD_CONFIG})

if(CONFIG_AES_KEY)
    list(APPEND BL_FW_POST_PROC_CONFIG --key=${CONFIG_AES_KEY})
endif()

if(CONFIG_AES_IV)
    list(APPEND BL_FW_POST_PROC_CONFIG --iv=${CONFIG_AES_IV})
endif()

if(CONFIG_PUBLIC_KEY)
    list(APPEND BL_FW_POST_PROC_CONFIG --publickey=${CONFIG_PUBLIC_KEY})
endif()

if(CONFIG_PRIVATE_KEY)
    list(APPEND BL_FW_POST_PROC_CONFIG --privatekey=${CONFIG_PRIVATE_KEY})
endif()

if(CONFIG_FW_POST_PROC_CUSTOM)
    list(APPEND BL_FW_POST_PROC_CONFIG ${CONFIG_FW_POST_PROC_CUSTOM})
endif()

# POST_PROC combine cmd
set(combine_cmds)
list(APPEND combine_cmds COMMAND ${BL_FW_POST_PROC} ${BL_FW_POST_PROC_CONFIG})
add_custom_target(combine WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} ${combine_cmds})


# post_build
set(post_build_cmds)
foreach(item ${CONFIG_POST_BUILDS})
    if("${item}" STREQUAL "CONCAT_WITH_LP_FW")
        list(APPEND post_build_cmds
            COMMAND ${CMAKE} -E echo "[lp_fw] concate with lp fw bin"
            COMMAND ${BL_SDK_BASE}/tools/lpfw/patch_lpfw${TOOL_SUFFIX} ${BIN_FILE} ${BL_SDK_BASE}/tools/lpfw/bin/${CHIP}_lp_fw.bin)

    elseif("${item}" STREQUAL "GENERATE_ROMFS")
        list(APPEND post_build_cmds
            COMMAND ${CMAKE} -E echo "[romfs] generate romfs.bin using romfs directory"
            COMMAND ${BL_SDK_BASE}/tools/genromfs/genromfs${TOOL_SUFFIX} -d romfs/ -f ./${BUILD_DIR}/build_out/romfs.bin)

    elseif("${item}" STREQUAL "GENERATE_LITTLEFS")
        list(APPEND post_build_cmds
            COMMAND ${CMAKE} -E echo "[littlefs] generate littlefs.bin using littlefs directory"
            COMMAND ${BL_SDK_BASE}/tools/genlfs/mklfs${TOOL_SUFFIX} -c lfs -b 4096 -p 256 -r 256 -s 0x71000 -i ./${BUILD_DIR}/build_out/littlefs.bin)
    endif()
endforeach()

add_custom_target(post_build WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} ${post_build_cmds})

# pre_build
set(pre_build_cmds)
if(BUILD_BINS)
    file(GLOB BUILD_BINS
        "${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_DIR}/build_out/*.bin"
        "${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_DIR}/build_out/*.elf"
        "${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_DIR}/build_out/*.xz"
        "${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_DIR}/build_out/*.ota")
    list(APPEND pre_build_cmds
        COMMAND ${CMAKE} -E echo "[clean] clean old binary"
        COMMAND ${CMAKE} -E remove ${BUILD_BINS})
endif()

add_custom_target(pre_build WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} ${pre_build_cmds})
