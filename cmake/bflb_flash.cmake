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
if(CONFIG_POST_BUILDS_CONCAT_WITH_LP_FW)
    list(APPEND post_build_cmds
        COMMAND ${CMAKE} -E echo "[lp_fw] concate with lp fw bin"
        COMMAND ${BL_SDK_BASE}/tools/lpfw/patch_lpfw${TOOL_SUFFIX} ${BIN_FILE} ${BL_SDK_BASE}/tools/lpfw/bin/${CHIP}_lp_fw.bin)
endif()

if(CONFIG_POST_BUILDS_GENERATE_ROMFS)
    list(APPEND post_build_cmds
        COMMAND ${CMAKE} -E echo "[romfs] generate romfs.bin using romfs directory"
        COMMAND ${BL_SDK_BASE}/tools/genromfs/genromfs${TOOL_SUFFIX} -d romfs/ -f ./${BUILD_DIR}/build_out/romfs.bin)
endif()

if(CONFIG_DUALCORE_NP_IMAGE)
    list(APPEND post_build_cmds
        COMMAND ${CMAKE} -E echo "[dualcore] append CONFIG_DUALCORE_NP_IMAGE"
	COMMAND python3 ${BL_SDK_BASE}/tools/byai/multi_bins.py
	        ${BIN_FILE}
                --append NP
                ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_DUALCORE_NP_IMAGE}
		--output ${BIN_FILE}
                --align 0x400
        )
endif()

if(CONFIG_BACKTRACE)
    list(APPEND post_build_cmds
        COMMAND ${CMAKE} -E echo "[dwarfcfi] generating and embedding DWARF CFI table"
        # Step 1: Generate DWARFCFI bin from ELF
	#COMMAND ${CMAKE} -E echo "  [1/2] Generating DWARFCFI bin from ELF..."
        COMMAND python3 ${BL_SDK_BASE}/tools/byai/unwind_6byte_table.py
            ${CMAKE_CURRENT_BINARY_DIR}/build_out/dwarfcfi.bin
            ${ELF_FILE}
        # Step 2: Append DWARFCFI to firmware using multi_bins.py
	#COMMAND ${CMAKE} -E echo "  [2/2] Appending DWARFCFI to firmware..."
        COMMAND python3 ${BL_SDK_BASE}/tools/byai/multi_bins.py
            ${BIN_FILE}
            --append DWARFCFI
            ${CMAKE_CURRENT_BINARY_DIR}/build_out/dwarfcfi.bin
            --output ${BIN_FILE}
            --align 0x1000
        #COMMAND ${CMAKE} -E echo "  [done] DWARFCFI embedded successfully"
        )
endif()

if(CONFIG_X509_CERTIFICATE_BUNDLE)
    list(APPEND post_build_cmds
        COMMAND ${CMAKE} -E echo "[certs] generating and embedding x509 certificate bundle"
        # Step 1: Generate certificate bundle from PEM files (filtered)
        COMMAND python3 ${BL_SDK_BASE}/components/crypto/mbedtls/bl_crt_bundle/gen_crt_bundle.py
            --input ${BL_SDK_BASE}/components/crypto/mbedtls/bl_crt_bundle/cacrt_all.pem
            --filter ${BL_SDK_BASE}/components/crypto/mbedtls/bl_crt_bundle/cmn_crt_authorities.csv
            --output ${CMAKE_CURRENT_BINARY_DIR}/build_out/x509_crt_bundle
            --quiet
        # Step 2: Append certificate bundle to firmware using multi_bins.py
        COMMAND python3 ${BL_SDK_BASE}/tools/byai/multi_bins.py
            ${BIN_FILE}
            --append CERTS
            ${CMAKE_CURRENT_BINARY_DIR}/build_out/x509_crt_bundle
            --output ${BIN_FILE}
            --align 0x1000
        )
endif()

if(CONFIG_POST_BUILDS_GENERATE_LITTLEFS)
    list(APPEND post_build_cmds
        COMMAND ${CMAKE} -E echo "[littlefs] generate littlefs.bin using littlefs directory, size=${CONFIG_POST_BUILDS_LITTLEFS_SIZE}"
        COMMAND ${BL_SDK_BASE}/tools/genlfs/mklfs${TOOL_SUFFIX} -c lfs -b 4096 -p 256 -r 256 -s ${CONFIG_POST_BUILDS_LITTLEFS_SIZE} -i ./${BUILD_DIR}/build_out/littlefs.bin)
endif()

if(VSCODE_PARSE_EN AND CMAKE_EXPORT_COMPILE_COMMANDS)
    list(APPEND post_build_cmds
        COMMAND ${CMAKE} -E echo "[clangd] update compile_commands.json to .vscode"
        COMMAND ${CMAKE} -E remove_directory ${BL_SDK_BASE}/.vscode/.cache
        COMMAND ${CMAKE} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${BL_SDK_BASE}/.vscode/compile_commands.json)
endif()

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
