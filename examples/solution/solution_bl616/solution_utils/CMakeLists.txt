sdk_generate_library()

sdk_add_include_directories(.)
sdk_library_add_sources(solution.c)

# queue
# ##################################################################################################

sdk_add_include_directories(queue)
sdk_library_add_sources(queue/frame_queue_ctrl.c)
sdk_library_add_sources(queue/frame_queue.c)

# audio
# ##################################################################################################

# Audio Auadc
if(CONFIG_SOLUTION_FUNC_AUDIO_AUADC)
    if(NOT CONFIG_SOLUTION_QUEUE_AUDIO_IN)
        message(FATAL_ERROR "audio_adc need audio_in_queue")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/auadc.c)
endif()

# Audio Audac
if(CONFIG_SOLUTION_FUNC_AUDIO_AUDAC)
    if(NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
        message(FATAL_ERROR "audio_dac need audio_out_queue")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/audac.c)
endif()

# Audio I2S Input
if(CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN)
    if(NOT CONFIG_SOLUTION_QUEUE_AUDIO_IN)
        message(FATAL_ERROR "i2s_input need audio_in_queue")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/external_codec_i2s.c)
endif()

# Audio I2S Output
if(CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT)
    if(NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
        message(FATAL_ERROR "i2s_output need audio_out_queue")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/external_codec_i2s.c)
endif()

# USB UAC Input
if(CONFIG_SOLUTION_FUNC_UAC_IN)
    if((NOT CONFIG_CHERRYUSB_HOST) OR (NOT CONFIG_CHERRYUSB_HOST_AUDIO))
        message(FATAL_ERROR "UAC(input) need cherry_usb host audio")
    endif()

    if(NOT CONFIG_SOLUTION_QUEUE_AUDIO_IN)
        message(FATAL_ERROR "UAC(input) need audio_in_queue")
    endif()

    if(NOT DEFINED FLAG_UVC_UAC_ADDED)
        set(FLAG_UVC_UAC_ADDED 1)
        add_subdirectory(usbh_uvc_uac)
    endif()
    sdk_library_add_sources(usbh_uvc_uac/port_cb/usbh_uac_port.c)
endif()

# USB UAC Output
if(CONFIG_SOLUTION_FUNC_UAC_OUT)
    if((NOT CONFIG_CHERRYUSB_HOST) OR (NOT CONFIG_CHERRYUSB_HOST_AUDIO))
        message(FATAL_ERROR "UAC(output) need cherry_usb host audio")
    endif()

    if(NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
        message(FATAL_ERROR "UAC(output) need audio_out_queue")
    endif()

    if(NOT DEFINED FLAG_UVC_UAC_ADDED)
        set(FLAG_UVC_UAC_ADDED 1)
        add_subdirectory(usbh_uvc_uac)
    endif()
    sdk_library_add_sources(usbh_uvc_uac/port_cb/usbh_uac_port.c)
endif()

# Audio Lookback
if(CONFIG_SOLUTION_FUNC_AUDIO_LOOPBACK)
    if(NOT CONFIG_SOLUTION_QUEUE_AUDIO_IN)
        message(FATAL_ERROR "audio_loopback need audio_in_queue")
    endif()

    if(NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
        message(FATAL_ERROR "audio_loopback need audio_out_queue")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/audio_loopback.c)
endif()

# es8388 codec init
if(CONFIG_SOLUTION_FUNC_AUDIO_ES8388)
    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/codec_es8388_cfg.c)
endif()

# video
# ##################################################################################################

# DBI Display
if(CONFIG_SOLUTION_FUNC_DISP_DBI)
    if((NOT CONFIG_SOLUTION_QUEUE_IMG_RAW_OUT) AND (NOT CONFIG_SOLUTION_QUEUE_IMG_RAW_IN))
        message(FATAL_ERROR "dbi_display need img_raw_queue_out or img_raw_queue_in")
    endif()

    sdk_add_include_directories(display/display_mcu)
    sdk_library_add_sources(display/display_mcu/dbi_disp.c)
    sdk_library_add_sources(display/display_mcu/display_flush.c)
endif()

# RGB Display
if(CONFIG_SOLUTION_FUNC_DISP_RGB)
    if((NOT CONFIG_SOLUTION_QUEUE_IMG_RAW_OUT) AND (NOT CONFIG_SOLUTION_QUEUE_IMG_RAW_IN))
        message(FATAL_ERROR "rgb_display need img_raw_queue_out or img_raw_queue_in")
    endif()

    sdk_add_include_directories(display/display_rgb)
    sdk_library_add_sources(display/display_rgb/rgb_disp.c)
    sdk_library_add_sources(display/display_rgb/rgb_draw.c)
endif()

# DVP Sensor
if(CONFIG_SOLUTION_FUNC_DVP)
    if(NOT CONFIG_SOLUTION_QUEUE_IMG_RAW_OUT)
        message(FATAL_ERROR "dvp need img_raw_queue_out")
    endif()

    sdk_add_include_directories(dvp)
    sdk_library_add_sources(dvp/dvp.c)
endif()

# JPEG Encoder (mjpeg mode)
if(CONFIG_SOLUTION_FUNC_MJPEG_ENC)
    if((NOT CONFIG_SOLUTION_QUEUE_MJPEG_OUT) AND (NOT CONFIG_SOLUTION_QUEUE_IMG_RAW_OUT))
        message(FATAL_ERROR "mjpeg_enc need mjpeg_queue_out or img_raw_queue_out")
    endif()

    sdk_add_include_directories(mjpeg_enc)
    sdk_library_add_sources(mjpeg_enc/mjpeg_enc.c mjpeg_enc/jpeg_head.c)
endif()

# JPEG Decoder (mjpeg mode)
if(CONFIG_SOLUTION_FUNC_MJPEG_DEC)
    if((NOT CONFIG_SOLUTION_QUEUE_MJPEG_IN) OR (NOT CONFIG_SOLUTION_QUEUE_IMG_RAW_IN))
        message(FATAL_ERROR "mjpeg_dec need mjpeg_queue_in and img_raw_queue_in")
    endif()

    sdk_add_include_directories(mjpeg_dec)
    sdk_library_add_sources(mjpeg_dec/mjpeg_dec.c)
endif()

# DVP JPEG Encoder line buffer mode
if(CONFIG_SOLUTION_FUNC_DVP_JPEG_ENC)
    if(NOT CONFIG_SOLUTION_QUEUE_MJPEG_OUT)
        message(FATAL_ERROR "dvp_jpeg need mjpeg_queue_out")
    endif()

    sdk_add_include_directories(mjpeg_enc)
    sdk_library_add_sources(mjpeg_enc/dvp_mjpeg_enc.c mjpeg_enc/jpeg_head.c)
endif()

# USB Video Class (yuyv mode)
if(CONFIG_SOLUTION_FUNC_UVC_YUYV)
    if(NOT CONFIG_SOLUTION_QUEUE_IMG_RAW_OUT)
        message(FATAL_ERROR "UVC(yuyv) need img_raw_queue_out")
    endif()

    if(NOT DEFINED FLAG_UVC_UAC_ADDED)
        set(FLAG_UVC_UAC_ADDED 1)
        add_subdirectory(usbh_uvc_uac)
    endif()
    sdk_library_add_sources(usbh_uvc_uac/port_cb/usbh_uvc_port.c)
endif()

# USB Video Class (mjpeg mode)
if(CONFIG_SOLUTION_FUNC_UVC_JPEG)
    if(NOT CONFIG_SOLUTION_QUEUE_MJPEG_OUT)
        message(FATAL_ERROR "UVC(jpeg) need mjpeg_queue_out")
    endif()

    if(NOT DEFINED FLAG_UVC_UAC_ADDED)
        set(FLAG_UVC_UAC_ADDED 1)
        add_subdirectory(usbh_uvc_uac)
    endif()
    sdk_library_add_sources(usbh_uvc_uac/port_cb/usbh_uvc_port.c)
endif()

# other
# ##################################################################################################

# Save Video Jpeg to SDcard .avi
if(CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
    if(NOT CONFIG_FATFS)
        message(FATAL_ERROR "sdcard need fatfs")
    endif()
    if((NOT CONFIG_SOLUTION_QUEUE_MJPEG_OUT) AND (NOT CONFIG_SOLUTION_QUEUE_MJPEG_IN))
        message(FATAL_ERROR "avi vids need mjpeg_queue_out or mjpeg_queue_in")
    endif()

    sdk_add_include_directories(sd_card)
    sdk_library_add_sources(sd_card/avi_jpeg_sd.c)
    sdk_library_add_sources(sd_card/jpeg_sd.c)
endif()

# Save Audio PCM to SDcard .avi
if(CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS)
    if(NOT CONFIG_FATFS)
        message(FATAL_ERROR "sdcard need fatfs")
    endif()
    if(NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
        message(FATAL_ERROR "avi auds need audio_out_queue")
    endif()

    sdk_add_include_directories(sd_card)
    sdk_library_add_sources(sd_card/avi_jpeg_sd.c)
    sdk_library_add_sources(sd_card/jpeg_sd.c)
endif()

# Hibooster (image send or receive)
if(CONFIG_SOLUTION_FUNC_HIBOOSTER_TX OR CONFIG_SOLUTION_FUNC_HIBOOSTER_RX OR CONFIG_HB_SEND_TEST OR CONFIG_HB_RECV_TEST)
    add_subdirectory(hibooster)

    if(CONFIG_SOLUTION_FUNC_HIBOOSTER_TX)
        if(NOT CONFIG_SOLUTION_QUEUE_MJPEG_OUT)
            message(FATAL_ERROR "hibooster_tx need mjpeg_queue_out")
        endif()
        sdk_add_include_directories(hibooster/solution_tx)
        sdk_library_add_sources(hibooster/solution_tx/hb_start_tx.c)
    endif()

    if(CONFIG_SOLUTION_FUNC_HIBOOSTER_RX)
        if(NOT CONFIG_SOLUTION_QUEUE_MJPEG_IN)
           message(FATAL_ERROR "hibooster_rx need mjpeg_queue_in")
        endif()
        sdk_add_include_directories(hibooster/solution_rx)
        sdk_library_add_sources(hibooster/solution_rx/hb_start_rx.c)
    endif()
endif()

# rtsp (image_transmission)
if(CONFIG_SOLUTION_FUNC_RTSP)
    if(NOT CONFIG_SOLUTION_QUEUE_MJPEG_OUT)
        message(FATAL_ERROR "RTSP(jpeg) need mjpeg_queue_out")
    endif()

    sdk_library_add_sources(rtsp/bl_cam_rtsp.c)
    sdk_add_include_directories(rtsp/include .)
endif()

# Conflict Check
# ##################################################################################################

# Conflict: dbi_display - rgb_display
if(CONFIG_SOLUTION_FUNC_DISP_DBI AND CONFIG_SOLUTION_FUNC_DISP_RGB)
    message(FATAL_ERROR "Display Conflict: dbi_display - rgb_display")
endif()

# Conflict: dvp - uvc_yuyv
if(CONFIG_SOLUTION_FUNC_DVP AND CONFIG_SOLUTION_FUNC_UVC_YUYV)
    message(FATAL_ERROR "Frame_buff queue Conflict: dvp - uvc_yuyv")
endif()

# Conflict: dvp_jpeg - jpeg_enc - uvc_jpeg 
if(CONFIG_SOLUTION_FUNC_DVP_JPEG_ENC AND CONFIG_SOLUTION_FUNC_MJPEG_ENC)
    message(FATAL_ERROR "Jpeg_buff queue Conflict: dvp_jpeg - mjpeg_enc")
elseif(CONFIG_SOLUTION_FUNC_DVP_JPEG_ENC AND CONFIG_SOLUTION_FUNC_UVC_JPEG)
    message(FATAL_ERROR "Jpeg_buff queue Conflict: dvp_jpeg - uvc_jpeg")
elseif(CONFIG_SOLUTION_FUNC_MJPEG_ENC AND CONFIG_SOLUTION_FUNC_UVC_JPEG)
    message(FATAL_ERROR "Jpeg_buff queue Conflict: mjpeg_enc - uvc_jpeg")
endif()

# Conflict: auadc uac
if((DEFINED CONFIG_SOLUTION_FUNC_UAC_IN) AND (DEFINED CONFIG_SOLUTION_FUNC_AUDIO_AUADC))
    if(CONFIG_SOLUTION_FUNC_UAC_IN AND CONFIG_SOLUTION_FUNC_AUDIO_AUADC)
        message(FATAL_ERROR "Auadc_buff queue Conflict: auadc - uac(in)")
    endif()
endif()
