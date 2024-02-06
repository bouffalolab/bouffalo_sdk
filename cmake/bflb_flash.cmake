if(MINGW OR CYGWIN OR WIN32)
set(TOOL_SUFFIX ".exe")
elseif(UNIX)
set(TOOL_SUFFIX "-ubuntu")
elseif(APPLE)
set(TOOL_SUFFIX "-macos")
endif()

set(BL_FW_POST_PROC ${BL_SDK_BASE}/tools/bflb_tools/bflb_fw_post_proc/bflb_fw_post_proc${TOOL_SUFFIX})

set(BL_FW_POST_PROC_CONFIG --chipname=${CHIP} --imgfile=${BIN_FILE})

if(BOARD_DIR)
list(APPEND BL_FW_POST_PROC_CONFIG --brdcfgdir=${BOARD_DIR}/${BOARD}/config)
else()
list(APPEND BL_FW_POST_PROC_CONFIG --brdcfgdir=${BL_SDK_BASE}/bsp/board/${BOARD}/config)
endif()

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

add_custom_target(combine
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${BL_FW_POST_PROC} ${BL_FW_POST_PROC_CONFIG})