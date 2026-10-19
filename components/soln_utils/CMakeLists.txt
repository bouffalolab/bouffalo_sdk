# Private source file
set(SOLN_PRIVATE_SOURCES_FILE ${CMAKE_CURRENT_LIST_DIR}/soln_private/sources.cmake)
if(EXISTS ${SOLN_PRIVATE_SOURCES_FILE})
    set(SOLN_PRIVATE_SOURCE_ENABLED ON)
    include(${SOLN_PRIVATE_SOURCES_FILE})
    message(STATUS "[soln_utils] building private modules from source")
else()
    set(SOLN_PRIVATE_SOURCE_ENABLED OFF)
    message(STATUS "[soln_utils] private source unavailable; using prebuilt libraries")
endif()

sdk_generate_library()

sdk_add_include_directories(.)
sdk_library_add_sources(soln_solution.c)

# FBQ
# ##################################################################################################

add_subdirectory(fb_queue)

# Audio
# ##################################################################################################

# Audio Auadc
if(CONFIG_SOLN_AUD_AUADC_EN)
    if(NOT CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_EN)
        message(FATAL_ERROR "AUADC requires SOLN_FBQ_AUD_PCM_LOCAL_EN")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/soln_auadc.c)
endif()

# Audio Audac
if(CONFIG_SOLN_AUD_AUDAC_EN)
    if(NOT CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_EN)
        message(FATAL_ERROR "AUDAC requires SOLN_FBQ_AUD_PCM_REMOTE_EN")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/soln_audac.c)
endif()

# Audio I2S Input/Output
if(CONFIG_SOLN_AUD_I2S_IN_EN OR CONFIG_SOLN_AUD_I2S_OUT_EN)
    if(CONFIG_SOLN_AUD_I2S_IN_EN AND NOT CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_EN)
        message(FATAL_ERROR "I2S input requires SOLN_FBQ_AUD_PCM_LOCAL_EN")
    endif()
    if(CONFIG_SOLN_AUD_I2S_OUT_EN AND NOT CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_EN)
        message(FATAL_ERROR "I2S output requires SOLN_FBQ_AUD_PCM_REMOTE_EN")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/soln_aud_external_codec_i2s.c)
endif()

# USB UAC Input/Output
if(CONFIG_SOLN_AUD_UAC_IN_EN OR CONFIG_SOLN_AUD_UAC_OUT_EN)
    if((NOT CONFIG_CHERRYUSB_HOST) OR (NOT CONFIG_CHERRYUSB_HOST_AUDIO))
        message(FATAL_ERROR "UAC requires CHERRYUSB_HOST and CHERRYUSB_HOST_AUDIO")
    endif()
    if(CONFIG_SOLN_AUD_UAC_IN_EN AND NOT CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_EN)
        message(FATAL_ERROR "UAC input requires SOLN_FBQ_AUD_PCM_LOCAL_EN")
    endif()
    if(CONFIG_SOLN_AUD_UAC_OUT_EN AND NOT CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_EN)
        message(FATAL_ERROR "UAC output requires SOLN_FBQ_AUD_PCM_REMOTE_EN")
    endif()

    if(NOT DEFINED FLAG_UVC_UAC_ADDED)
        set(FLAG_UVC_UAC_ADDED 1)
        add_subdirectory(usbh_uvc_uac)
    endif()
    sdk_library_add_sources(usbh_uvc_uac/soln_port_cb/soln_usbh_uac_port.c)
endif()

# Audio Loopback
if(CONFIG_SOLN_AUD_LOOPBACK_EN)
    if(NOT CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_EN)
        message(FATAL_ERROR "Audio loopback requires SOLN_FBQ_AUD_PCM_LOCAL_EN")
    endif()
    if(NOT CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_EN)
        message(FATAL_ERROR "Audio loopback requires SOLN_FBQ_AUD_PCM_REMOTE_EN")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/soln_aud_loopback.c)
endif()

# Audio ES8388
if(CONFIG_SOLN_AUD_ES8388_EN)
    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/soln_aud_codec_es8388_cfg.c)
endif()

# video
# ##################################################################################################

# DBI Display
if(CONFIG_SOLN_DISP_DBI_EN)
    if(NOT CONFIG_BSP_LCD)
        message(FATAL_ERROR "DBI display requires BSP_LCD")
    endif()
    if((NOT CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN) AND (NOT CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN))
        message(FATAL_ERROR "DBI display requires a local or remote RAW queue")
    endif()

    sdk_add_include_directories(display/display_mcu)
    sdk_library_add_sources(display/display_mcu/soln_dbi_disp.c)
    sdk_library_add_sources(display/display_mcu/soln_display_flush.c)
endif()

# RGB Display
if(CONFIG_SOLN_DISP_RGB_EN)
    if(NOT CONFIG_BSP_LCD)
        message(FATAL_ERROR "RGB display requires BSP_LCD")
    endif()
    if((NOT CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN) AND (NOT CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN))
        message(FATAL_ERROR "RGB display requires a local or remote RAW queue")
    endif()

    sdk_add_include_directories(display/display_rgb)
    sdk_library_add_sources(display/display_rgb/soln_rgb_disp.c)
    sdk_library_add_sources(display/display_rgb/soln_rgb_draw.c)
endif()

# DVP Sensor
if(CONFIG_SOLN_VID_DVP_EN)
    if(NOT CONFIG_BSP_IMAGE_SENSOR)
        message(FATAL_ERROR "DVP capture requires BSP_IMAGE_SENSOR")
    endif()
    if(NOT CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN)
        message(FATAL_ERROR "DVP capture requires SOLN_FBQ_VID_RAW_LOCAL_EN")
    endif()

    sdk_add_include_directories(dvp)
    sdk_library_add_sources(dvp/soln_dvp.c)
endif()

# JPEG Encoder (mjpeg mode)
if(CONFIG_SOLN_VID_JPEG_ENC_EN)
    if((NOT CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN) OR (NOT CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN))
        message(FATAL_ERROR "MJPEG encoder requires both local RAW and local JPEG queues")
    endif()

    sdk_add_include_directories(mjpeg_enc)
    sdk_library_add_sources(mjpeg_enc/soln_mjpeg_enc.c mjpeg_enc/soln_jpeg_head.c)
endif()

# JPEG Decoder (mjpeg mode)
if(CONFIG_SOLN_VID_JPEG_DEC_EN)
    if((NOT CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_EN) OR (NOT CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN))
        message(FATAL_ERROR "MJPEG decoder requires both remote JPEG and remote RAW queues")
    endif()

    sdk_add_include_directories(mjpeg_dec)
    sdk_library_add_sources(mjpeg_dec/soln_mjpeg_dec.c)
endif()

# DVP JPEG Encoder line buffer mode
if(CONFIG_SOLN_VID_DVP_JPEG_ENC_EN)
    if(NOT CONFIG_BSP_IMAGE_SENSOR)
        message(FATAL_ERROR "DVP JPEG encoder requires BSP_IMAGE_SENSOR")
    endif()
    if(NOT CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN)
        message(FATAL_ERROR "DVP JPEG encoder requires SOLN_FBQ_VID_JPEG_LOCAL_EN")
    endif()

    sdk_add_include_directories(mjpeg_enc)
    sdk_library_add_sources(mjpeg_enc/soln_dvp_mjpeg_enc.c mjpeg_enc/soln_jpeg_head.c)
endif()

# USB Video Class (YUYV/JPEG mode)
if(CONFIG_SOLN_VID_UVC_YUYV_EN OR CONFIG_SOLN_VID_UVC_JPEG_EN)
    if((NOT CONFIG_CHERRYUSB_HOST) OR (NOT CONFIG_CHERRYUSB_HOST_VIDEO))
        message(FATAL_ERROR "UVC requires CHERRYUSB_HOST and CHERRYUSB_HOST_VIDEO")
    endif()
    if(CONFIG_SOLN_VID_UVC_YUYV_EN AND NOT CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN)
        message(FATAL_ERROR "UVC YUYV requires SOLN_FBQ_VID_RAW_LOCAL_EN")
    endif()
    if(CONFIG_SOLN_VID_UVC_JPEG_EN AND NOT CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN)
        message(FATAL_ERROR "UVC JPEG requires SOLN_FBQ_VID_JPEG_LOCAL_EN")
    endif()

    if(NOT DEFINED FLAG_UVC_UAC_ADDED)
        set(FLAG_UVC_UAC_ADDED 1)
        add_subdirectory(usbh_uvc_uac)
    endif()
    sdk_library_add_sources(usbh_uvc_uac/soln_port_cb/soln_usbh_uvc_port.c)
endif()

# other
# ##################################################################################################

# Save video/audio to SD card AVI
if(CONFIG_SOLN_SD_AVI_VIDEO_EN OR CONFIG_SOLN_SD_AVI_AUDIO_EN)
    if(NOT CONFIG_FATFS)
        message(FATAL_ERROR "SD AVI requires FATFS")
    endif()
    if((NOT CONFIG_FATFS_SDH_SDCARD) OR (NOT CONFIG_BSP_SDH_SD))
        message(FATAL_ERROR "SD AVI requires FATFS_SDH_SDCARD and BSP_SDH_SD")
    endif()
    if(CONFIG_SOLN_SD_AVI_VIDEO_EN AND (NOT CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN) AND (NOT CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_EN))
        message(FATAL_ERROR "Video AVI requires a local or remote JPEG queue")
    endif()
    if(CONFIG_SOLN_SD_AVI_AUDIO_EN AND NOT CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_EN)
        message(FATAL_ERROR "Audio AVI requires SOLN_FBQ_AUD_PCM_REMOTE_EN")
    endif()

    sdk_add_include_directories(sd_card)
    sdk_library_add_sources(sd_card/soln_avi_jpeg_sd.c)
    sdk_library_add_sources(sd_card/soln_jpeg_sd.c)
endif()

# HiBooster (image send or receive)
if(CONFIG_SOLN_HB_TX_EN OR CONFIG_SOLN_HB_RX_EN OR CONFIG_SOLN_HB_TX_TEST_EN OR CONFIG_SOLN_HB_RX_TEST_EN)
    if(NOT CONFIG_LWIP)
        message(FATAL_ERROR "HiBooster requires LWIP")
    endif()
    add_subdirectory(hibooster)

    if(CONFIG_SOLN_HB_TX_EN)
        if(NOT CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN)
            message(FATAL_ERROR "HiBooster TX requires a local JPEG queue")
        endif()
        sdk_add_include_directories(hibooster/soln_tx)
        sdk_library_add_sources(hibooster/soln_tx/soln_hb_start_tx.c)
    endif()

    if(CONFIG_SOLN_HB_RX_EN)
        if(NOT CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_EN)
            message(FATAL_ERROR "HiBooster RX requires a remote JPEG queue")
        endif()
        sdk_add_include_directories(hibooster/soln_rx)
        sdk_library_add_sources(hibooster/soln_rx/soln_hb_start_rx.c)
    endif()
endif()

# RTSP (image_transmission)
if(CONFIG_SOLN_RTSP_EN)
    if((NOT CONFIG_RTSP) OR (NOT CONFIG_LWIP))
        message(FATAL_ERROR "Soln RTSP requires RTSP and LWIP")
    endif()
    if(NOT CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN)
        message(FATAL_ERROR "Soln RTSP requires SOLN_FBQ_VID_JPEG_LOCAL_EN")
    endif()

    sdk_library_add_sources(rtsp/soln_bl_cam_rtsp.c)
    sdk_add_include_directories(rtsp/include .)
endif()

# Conflict Check
# ##################################################################################################

# Conflict: shared DMA channels
if(CONFIG_SOLN_AUD_AUADC_EN AND CONFIG_SOLN_AUD_I2S_IN_EN)
    message(FATAL_ERROR "Audio input conflict: AUADC and I2S input both use dma0_ch0")
endif()
if(CONFIG_SOLN_AUD_AUDAC_EN AND CONFIG_SOLN_AUD_I2S_OUT_EN)
    message(FATAL_ERROR "Audio output conflict: AUDAC and I2S output both use dma0_ch1")
endif()

# Conflict: DBI display and RGB display
if(CONFIG_SOLN_DISP_DBI_EN AND CONFIG_SOLN_DISP_RGB_EN)
    message(FATAL_ERROR "Display conflict: DBI display and RGB display")
endif()

# Conflict: DVP and UVC YUYV publish to the same local RAW queue
if(CONFIG_SOLN_VID_DVP_EN AND CONFIG_SOLN_VID_UVC_YUYV_EN)
    message(FATAL_ERROR "Local RAW source conflict: DVP and UVC YUYV")
endif()

# Conflict: cam0 is shared by normal DVP and line-buffer DVP JPEG
if(CONFIG_SOLN_VID_DVP_EN AND CONFIG_SOLN_VID_DVP_JPEG_ENC_EN)
    message(FATAL_ERROR "Camera conflict: DVP and DVP JPEG both use cam0")
endif()

# Conflict: one UVC stream instance can select only one format
if(CONFIG_SOLN_VID_UVC_YUYV_EN AND CONFIG_SOLN_VID_UVC_JPEG_EN)
    message(FATAL_ERROR "UVC format conflict: YUYV and JPEG cannot both be enabled")
endif()

# Conflict: local JPEG producers
if(CONFIG_SOLN_VID_DVP_JPEG_ENC_EN AND CONFIG_SOLN_VID_JPEG_ENC_EN)
    message(FATAL_ERROR "Local JPEG source conflict: DVP JPEG and MJPEG encoder")
elseif(CONFIG_SOLN_VID_DVP_JPEG_ENC_EN AND CONFIG_SOLN_VID_UVC_JPEG_EN)
    message(FATAL_ERROR "Local JPEG source conflict: DVP JPEG and UVC JPEG")
elseif(CONFIG_SOLN_VID_JPEG_ENC_EN AND CONFIG_SOLN_VID_UVC_JPEG_EN)
    message(FATAL_ERROR "Local JPEG source conflict: MJPEG encoder and UVC JPEG")
endif()

# Conflict: local PCM capture sources
if(CONFIG_SOLN_AUD_UAC_IN_EN AND CONFIG_SOLN_AUD_AUADC_EN)
    message(FATAL_ERROR "Local PCM source conflict: AUADC and UAC input")
endif()

# Conflict: HiBooster TX and RTSP currently share one local JPEG output subscription
if(CONFIG_SOLN_HB_TX_EN AND CONFIG_SOLN_RTSP_EN)
    message(FATAL_ERROR "Local JPEG output conflict: HiBooster TX and RTSP share the network output ID")
endif()
