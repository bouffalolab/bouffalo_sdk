cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})


# Define
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
sdk_add_compile_definitions(-DCFG_CPU_E907)
sdk_add_compile_definitions(-DCFG_BL616)
sdk_add_compile_definitions(-DCPU_M0)
sdk_add_compile_definitions(-DCFG_RTOS)
sdk_add_compile_definitions(-DCFG_FHOST)
sdk_add_compile_definitions(-DCFG_UMAC)
sdk_add_compile_definitions(-DCFG_STATS)
sdk_add_compile_definitions(-DCFG_DBG=0)
sdk_add_compile_definitions(-DCFG_BCN)
sdk_add_compile_definitions(-DCFG_AGG)
sdk_add_compile_definitions(-DCFG_AMSDU_4K)
sdk_add_compile_definitions(-DCFG_VIF_MAX=2)
sdk_add_compile_definitions(-DCFG_STA_MAX=4)
sdk_add_compile_definitions(-DCFG_SPC=16)
sdk_add_compile_definitions(-DCFG_TXDESC0=0)
sdk_add_compile_definitions(-DCFG_TXDESC1=32)
sdk_add_compile_definitions(-DCFG_TXDESC2=1)
sdk_add_compile_definitions(-DCFG_TXDESC3=1)
#now txdesc4 isn't used beacause beacon change to VI
sdk_add_compile_definitions(-DCFG_TXDESC4=4)
sdk_add_compile_definitions(-DCFG_MAC_VER_V21)
sdk_add_compile_definitions(-DCFG_MDM_VER_V30)
sdk_add_compile_definitions(-DCFG_IPC_VER_V11)
sdk_add_compile_definitions(-DCFG_PLF_VER_V30)
# sdk_add_compile_definitions(-DCFG_PS)
sdk_add_compile_definitions(-DCFG_MU_CNT=1)
# sdk_add_compile_definitions(-DCFG_UAPSD)
sdk_add_compile_definitions(-DCFG_KEYCFG)
sdk_add_compile_definitions(-DCFG_P2P=0)
sdk_add_compile_definitions(-DCFG_P2P_GO)
sdk_add_compile_definitions(-DCFG_BWLEN)
sdk_add_compile_definitions(-DCFG_HE)
# sdk_add_compile_definitions(-DCFG_TWT=8)
sdk_add_compile_definitions(-DCFG_BARX=2)
sdk_add_compile_definitions(-DCFG_BATX=1)
sdk_add_compile_definitions(-DCFG_REORD_BUF=12)
#sdk_add_compile_definitions(-DCFG_DBGDUMPKEY)
#sdk_add_compile_definitions(-DCFG_TRACE)
sdk_add_compile_definitions(-DCFG_UF)
sdk_add_compile_definitions(-DCFG_MFP)
sdk_add_compile_definitions(-DCFG_FTM_INIT)
sdk_add_compile_definitions(-DCFG_FTM_RSP)
sdk_add_compile_definitions(-DCFG_HSU=0)
sdk_add_compile_definitions(-DCONFIG_RWNX_LWIP)
sdk_add_compile_definitions(-DCFG_CRYPTO)
sdk_add_compile_definitions(-DCONFIG_BL6_MBEDTLS)
sdk_add_compile_definitions(-DCFG_FHOST_MONITOR)
# sdk_add_compile_definitions(-DCFG_IPERF)
#sdk_add_compile_definitions(-DCFG_PING)
# sdk_add_compile_definitions(-DHIGH_PERFORMANCE_PBUF)
sdk_add_compile_definitions(-DCFG_SMARTCONFIG)
sdk_add_compile_definitions(-DCONFIG_NO_STDOUT_DEBUG)
#sdk_add_compile_definitions(-DCONFIG_WPA_LOG_LEVEL=MSG_EXCESSIVE  #MSG_EXCESSIVE, MSG_MSGDUMP, MSG_DEBUG, MSG_INFO, MSG_WARNING, MSG_ERROR)
sdk_add_compile_definitions(-DBL616_IOTSDK)
sdk_add_compile_definitions(-DWL_BB_TPC=1)
sdk_add_compile_definitions(-DWL_BB_PD=1)
#sdk_add_compile_definitions(-DWIFI_DEMO_TEST)
#sdk_add_compile_definitions(-DCONFIG_WPA_BL_LOG_PRINT)
#sdk_add_compile_definitions(-DBL616_DHCP_DEBUG)

target_sources(app PRIVATE
                    app/audio_file.c
                    app/codec_mic.c
                    app/codec_speaker.c
                    app/codec_speaker_sin.c
                    app/codec_template.c
                    app/codec_loop.c
                    multimedia_port.c
                   )

sdk_add_include_directories(. app)

sdk_set_main_file(main.c)

project(codec)
