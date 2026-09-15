if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    set(TOOL_SUFFIX ".exe")
    set(CMAKE ${BL_SDK_BASE}/tools/cmake/bin/cmake.exe)
elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
    set(TOOL_SUFFIX "-ubuntu")
    set(CMAKE cmake)
elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    set(TOOL_SUFFIX "-macos")
    set(CMAKE cmake)
endif()

set(PROJECT_SDK_VERSION ${CONFIG_PROJECT_SDK_VERSION})
if(NOT DEFINED PROJECT_SDK_VERSION OR PROJECT_SDK_VERSION STREQUAL "")
    if(EXISTS "${BL_SDK_BASE}/VERSION")
        file(READ "${BL_SDK_BASE}/VERSION" VERSION_FILE_CONTENT)

        string(REGEX MATCH "PROJECT_SDK_VERSION[ ]+\"([^\"]+)\"" _ ${VERSION_FILE_CONTENT})
        set(PROJECT_SDK_VERSION "${CMAKE_MATCH_1}")
    else()
        message(STATUS "No valid SDK VERSION file found, fallback to 1.1.1")
        set(PROJECT_SDK_VERSION "1.1.1")
    endif()
endif()

if(EXISTS "${BL_SDK_BASE}/tools/bflb_tools")
    set(FULL_FLASH_TOOL ON)
    set(BL_FW_POST_PROC ${BL_SDK_BASE}/tools/bflb_tools/bflb_fw_post_proc/bflb_fw_post_proc${TOOL_SUFFIX})
    set(BL_FW_POST_PROC_CONFIG --chipname=${CHIP} --imgfile=${BIN_FILE} --appkeys=shared)

    message(STATUS "Use full firmware tool: ${BL_FW_POST_PROC}")
else()
    set(FULL_FLASH_TOOL OFF)
    set(BFLB_IMAGE_TOOL python3 ${BL_SDK_BASE}/tools/bflb_flash/bflb_whole_bin.py)
    set(BFLB_IMAGE_DIR ${CMAKE_CURRENT_BINARY_DIR}/images)

    message(STATUS "Use simple firmware tool: ${BL_SDK_BASE}/tools/bflb_flash/bflb_whole_bin.py")
endif()

set(BFLB_BOOT2_FILE ${CONFIG_BOARD_BOOT2FILE})
set(BFLB_PT_FILE ${CONFIG_BOARD_PTFILE})
set(BFLB_DTS_FILE ${CONFIG_BOARD_DTSFILE})
set(BFLB_MFG_FILE)

if(CONFIG_OVERRIDE_BOOT2FILE)
    set(BFLB_BOOT2_FILE ${SDK_DEMO_PATH}/${CONFIG_OVERRIDE_BOOT2FILE})
endif()

if(CONFIG_OVERRIDE_PTFILE)
    set(BFLB_PT_FILE ${SDK_DEMO_PATH}/${CONFIG_OVERRIDE_PTFILE})
endif()

if(CONFIG_OVERRIDE_DTSFILE)
    set(BFLB_DTS_FILE ${SDK_DEMO_PATH}/${CONFIG_OVERRIDE_DTSFILE})
endif()

if(CONFIG_OVERRIDE_MFGFILE)
    set(BFLB_MFG_FILE ${SDK_DEMO_PATH}/${CONFIG_OVERRIDE_MFGFILE})
endif()

if(FULL_FLASH_TOOL)
    set(FLASH_PROG_CFG_GENERATOR
        ${BL_SDK_BASE}/tools/bflb_tools/flash_prog_cfg.py)
    set(FLASH_PROG_CFG ${CMAKE_CURRENT_BINARY_DIR}/flash_prog_cfg.ini)
    set(FLASH_PROG_CFG_CMD
        python3 "${FLASH_PROG_CFG_GENERATOR}"
        --partition-table "${BFLB_PT_FILE}"
        --output "${FLASH_PROG_CFG}"
        --application "${BIN_FILE}")

    get_property(FLASH_ERASE GLOBAL PROPERTY SDK_FLASH_ERASE)
    if("${FLASH_ERASE}" STREQUAL "")
        set(FLASH_ERASE 1)
    endif()
    list(APPEND FLASH_PROG_CFG_CMD --erase "${FLASH_ERASE}")

    if(CONFIG_APP_BUILD_TYPE_STD_APP)
        list(APPEND FLASH_PROG_CFG_CMD
            --standard
            --boot2 "${CMAKE_CURRENT_BINARY_DIR}/build_out/boot2_*.bin"
            --partition-bin "${CMAKE_CURRENT_BINARY_DIR}/build_out/partition.bin")
    elseif(NOT CONFIG_APP_BUILD_TYPE_DIRECT_BOOT_APP)
        message(FATAL_ERROR "Unsupported application build type")
    endif()

    get_property(FLASH_IMAGES GLOBAL PROPERTY SDK_FLASH_PARTITION_IMAGES)
    while(FLASH_IMAGES)
        list(POP_FRONT FLASH_IMAGES FLASH_IMAGE_PARTITION FLASH_IMAGE_FILE)
        list(APPEND FLASH_PROG_CFG_CMD --image
            "${FLASH_IMAGE_PARTITION}" "${FLASH_IMAGE_FILE}")
    endwhile()

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        "${FLASH_PROG_CFG_GENERATOR}" "${BFLB_PT_FILE}")
    execute_process(
        COMMAND ${FLASH_PROG_CFG_CMD}
        RESULT_VARIABLE FLASH_PROG_CFG_RESULT
        ERROR_VARIABLE FLASH_PROG_CFG_ERROR)
    if(NOT FLASH_PROG_CFG_RESULT EQUAL 0)
        message(FATAL_ERROR
            "Failed to generate ${FLASH_PROG_CFG}:\n${FLASH_PROG_CFG_ERROR}")
    endif()
endif()

if(FULL_FLASH_TOOL)
    list(APPEND BL_FW_POST_PROC_CONFIG
        --boot2file=${BFLB_BOOT2_FILE}
        --ptfile=${BFLB_PT_FILE}
        --dtsfile=${BFLB_DTS_FILE})
    if(BFLB_MFG_FILE)
        list(APPEND BL_FW_POST_PROC_CONFIG --mfgfile=${BFLB_MFG_FILE})
    endif()
endif()

if(FULL_FLASH_TOOL AND CONFIG_OTA_HEADER_USE_SDK_VERSION)
    set(OTA_HEADER_VERSION_PREFIX "EVENT_V")
    if(DEFINED CONFIG_OTA_VERSION_PREFIX)
        set(OTA_HEADER_VERSION_PREFIX "${CONFIG_OTA_VERSION_PREFIX}")
    endif()

    set(OTA_HEADER_SW_VERSION "${OTA_HEADER_VERSION_PREFIX}${PROJECT_SDK_VERSION}")
    string(HEX "${OTA_HEADER_SW_VERSION}" OTA_HEADER_SW_VERSION_HEX)
    string(LENGTH "${OTA_HEADER_SW_VERSION_HEX}" OTA_HEADER_SW_VERSION_HEX_LEN)
    math(EXPR OTA_HEADER_VERSION_LEN "${OTA_HEADER_SW_VERSION_HEX_LEN} / 2")
    if(OTA_HEADER_VERSION_LEN GREATER 15)
        message(FATAL_ERROR "${OTA_HEADER_SW_VERSION} exceeds OTA header software-version payload limit (max 15 bytes)")
    endif()
    list(APPEND BL_FW_POST_PROC_CONFIG --ota_ver=${PROJECT_SDK_VERSION})
    list(APPEND BL_FW_POST_PROC_CONFIG --ota_ver_prefix=${OTA_HEADER_VERSION_PREFIX})
endif()

if(FULL_FLASH_TOOL)
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
endif()

# POST_PROC combine cmd
set(combine_cmds)
if(CONFIG_SKIP_COMBINE)
    list(APPEND combine_cmds
        COMMAND ${CMAKE} -E echo "[fw_post_proc] skipped: raw binary output requested")
elseif(FULL_FLASH_TOOL)
    list(APPEND combine_cmds
        COMMAND ${BL_FW_POST_PROC} ${BL_FW_POST_PROC_CONFIG})
elseif(CONFIG_APP_BUILD_TYPE_STD_APP)
    list(APPEND combine_cmds
        COMMAND ${CMAKE} -E remove_directory ${BFLB_IMAGE_DIR}
        COMMAND ${BFLB_IMAGE_TOOL} image
            --app ${BIN_FILE}
            --boot2 ${BFLB_BOOT2_FILE}
            --pt ${BFLB_PT_FILE}
            --dts ${BFLB_DTS_FILE}
            --output ${BFLB_IMAGE_DIR})
else(CONFIG_APP_BUILD_TYPE_DIRECT_BOOT_APP)
    list(APPEND combine_cmds
        COMMAND ${CMAKE} -E remove_directory ${BFLB_IMAGE_DIR}
        COMMAND ${CMAKE} -E make_directory ${BFLB_IMAGE_DIR}
        COMMAND ${CMAKE} -E copy ${BIN_FILE} ${BFLB_IMAGE_DIR}/application@0x0.bin)
endif()
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

if(CONFIG_THIRDCORE_LP_IMAGE)
    list(APPEND post_build_cmds
        COMMAND ${CMAKE} -E echo "[dualcore] append CONFIG_THIRDCORE_LP_IMAGE"
	COMMAND python3 ${BL_SDK_BASE}/tools/byai/multi_bins.py
	        ${BIN_FILE}
                --append LP
                ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_THIRDCORE_LP_IMAGE}
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

if(CONFIG_SHELL AND CONFIG_SHELL_AUTOLIST_FILE)
    list(APPEND post_build_cmds
        COMMAND ${CMAKE} -E echo "[shell_auto] auto execute shell commands ${CONFIG_SHELL_AUTOLIST_FILE} after boot"
        COMMAND python3 ${BL_SDK_BASE}/tools/byai/multi_bins.py
            ${BIN_FILE}
            --append AUTOLIST
            ${CONFIG_SHELL_AUTOLIST_FILE}
            --output ${BIN_FILE}
            --align 0x100
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
