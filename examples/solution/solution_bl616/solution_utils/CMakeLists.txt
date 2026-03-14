sdk_generate_library()

# SD add fatfs_conf_user.h
if(CONFIG_FATFS)
    sdk_add_include_directories(sd_card)
endif()

# queue
# ##################################################################################################

# Frame_buff Queue
if(CONFIG_SOLUTION_QUEUE_YUYV)
    sdk_add_include_directories(queue)
    sdk_library_add_sources(queue/frame_queue_ctrl.c)
    sdk_library_add_sources(queue/frame_queue.c)
endif()

# MJPEG Queue
if(CONFIG_SOLUTION_QUEUE_MJPEG)
    sdk_add_include_directories(queue)
    sdk_library_add_sources(queue/frame_queue_ctrl.c)
    sdk_library_add_sources(queue/frame_queue.c)
endif()

# Auadc Queue
if(CONFIG_SOLUTION_QUEUE_AUDIO_IN)
    sdk_add_include_directories(queue)
    sdk_library_add_sources(queue/frame_queue_ctrl.c)
    sdk_library_add_sources(queue/frame_queue.c)
endif()

# Audac Queue
if(CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
    sdk_add_include_directories(queue)
    sdk_library_add_sources(queue/frame_queue_ctrl.c)
    sdk_library_add_sources(queue/frame_queue.c)
endif()

# audio
# ##################################################################################################

# Audio Auadc
if(CONFIG_SOLUTION_FUNC_AUDIO_AUADC)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_IN) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_IN))
        message(FATAL_ERROR "audio_adc need audio_in_queue")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/auadc.c)
endif()

# Audio Audac
if(CONFIG_SOLUTION_FUNC_AUDIO_AUDAC)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_OUT) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT))
        message(FATAL_ERROR "audio_dac need audio_out_queue")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/audac.c)
endif()

# Audio I2S Input
if(CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_IN) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_IN))
        message(FATAL_ERROR "i2s_input need audio_in_queue")
    endif()

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/external_codec_i2s.c)
endif()

# Audio I2S Output
if(CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_OUT) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT))
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
    sdk_library_add_sources(usbh_cb/usbh_uac_port.c)
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
    sdk_library_add_sources(usbh_cb/usbh_uac_port.c)
endif()

# Audio Lookback
if(CONFIG_SOLUTION_FUNC_AUDIO_LOOPBACK)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_IN) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_IN))
        message(FATAL_ERROR "audio_loopback need audio_in_queue")
    endif()

    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_OUT) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT))
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
if(CONFIG_SOLUTION_FUNC_DBI_DISP)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_YUYV) OR (NOT CONFIG_SOLUTION_QUEUE_YUYV))
        message(FATAL_ERROR "dbi_display need yuyv_queue")
    endif()

    sdk_add_include_directories(dbi_disp)
    sdk_library_add_sources(dbi_disp/dbi_disp.c)
endif()

# JPEG Compression
if(CONFIG_SOLUTION_FUNC_MJPEG)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "mjpeg_compress need mjpeg_queue")
    endif()

    sdk_add_include_directories(mjpeg)
    sdk_library_add_sources(mjpeg/mjpeg.c mjpeg/jpeg_head.c)
endif()

# DVP Sensor
if(CONFIG_SOLUTION_FUNC_DVP)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_YUYV) OR (NOT CONFIG_SOLUTION_QUEUE_YUYV))
        message(FATAL_ERROR "dvp need yuyv_queue")
    endif()

    sdk_add_include_directories(dvp)
    sdk_library_add_sources(dvp/dvp.c)
endif()

# DVP jpeg line buffer mode
if(CONFIG_SOLUTION_FUNC_DVP_JPEG)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "dvp_jpeg need jpeg_queue")
    endif()

    sdk_add_include_directories(mjpeg)
    sdk_library_add_sources(mjpeg/dvp_mjpeg.c mjpeg/jpeg_head.c)
endif()

# USB Video Class (yuyv mode)
if(CONFIG_SOLUTION_FUNC_UVC_YUYV)
    if((NOT DEFINED CONFIG_CHERRYUSB_HOST) OR (NOT CONFIG_CHERRYUSB_HOST))
        message(FATAL_ERROR "UVC(yuyv) need cherry_usb host")
    endif()

    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_YUYV) OR (NOT CONFIG_SOLUTION_QUEUE_YUYV))
        message(FATAL_ERROR "UVC(yuyv) need yuyv_queue")
    endif()

    if(NOT DEFINED FLAG_UVC_UAC_ADDED)
        set(FLAG_UVC_UAC_ADDED 1)
        add_subdirectory(usbh_uvc_uac)
    endif()
    sdk_library_add_sources(usbh_cb/usbh_uvc_port.c)
endif()

# USB Video Class (mjpeg mode)
if(CONFIG_SOLUTION_FUNC_UVC_JPEG)
    if((NOT DEFINED CONFIG_CHERRYUSB_HOST) OR (NOT CONFIG_CHERRYUSB_HOST))
        message(FATAL_ERROR "UVC(jpeg) need cherry_usb host")
    endif()

    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "UVC(jpeg) need mjpeg_queue")
    endif()

    if(NOT DEFINED FLAG_UVC_UAC_ADDED)
        set(FLAG_UVC_UAC_ADDED 1)
        add_subdirectory(usbh_uvc_uac)
    endif()
    sdk_library_add_sources(usbh_cb/usbh_uvc_port.c)
endif()

# other
# ##################################################################################################

# Save Video Jpeg to SDcard .avi
if(CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
    if((NOT DEFINED CONFIG_FATFS) OR (NOT CONFIG_FATFS))
        message(FATAL_ERROR "sdcard need fatfs")
    endif()
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "avi vids need mjpeg_queue")
    endif()

    sdk_add_include_directories(sd_card)
    sdk_library_add_sources(sd_card/avi_jpeg_sd.c)
    sdk_library_add_sources(sd_card/jpeg_sd.c)
endif()

# Save Audio PCM to SDcard .avi
if(CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS)
    if((NOT DEFINED CONFIG_FATFS) OR (NOT CONFIG_FATFS))
        message(FATAL_ERROR "sdcard need fatfs")
    endif()
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_OUT) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT))
        message(FATAL_ERROR "avi auds need audio_out_queue")
    endif()

    sdk_add_include_directories(sd_card)
    sdk_library_add_sources(sd_card/avi_jpeg_sd.c)
    sdk_library_add_sources(sd_card/jpeg_sd.c)
endif()

# Hibooster New Implementation
if(CONFIG_SOLUTION_FUNC_HIBOOSTER_NEW)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "hibooster_new need mjpeg_queue")
    endif()

    add_subdirectory(hibooster_new)

    sdk_add_include_directories(hibooster_new)
    sdk_library_add_sources(hibooster_new/hb_start.c)
endif()

# Hibooster (image_transmission)
if(CONFIG_SOLUTION_FUNC_HIBOOSTER)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "UVC(jpeg) need mjpeg_queue")
    endif()

    add_subdirectory(hibooster)
endif()

# rtsp (image_transmission)
if(CONFIG_SOLUTION_FUNC_RTSP)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "RTSP(jpeg) need mjpeg_queue")
    endif()

    sdk_library_add_sources(rtsp/bl_cam_rtsp.c)
    sdk_add_include_directories(rtsp/include .)
endif()

# Conflict Check
# ##################################################################################################

# Conflict: dvp uvc
if((DEFINED CONFIG_SOLUTION_FUNC_DVP) AND (DEFINED CONFIG_SOLUTION_FUNC_UVC_YUYV))
    if(CONFIG_SOLUTION_FUNC_DVP AND CONFIG_SOLUTION_FUNC_UVC_YUYV)
        message(FATAL_ERROR "Frame_buff queue Conflict: dvp - uvc_yuyv")
    endif()
endif()

# Conflict: dvp_jpeg
if(CONFIG_SOLUTION_FUNC_DVP_JPEG)
    if(CONFIG_SOLUTION_FUNC_MJPEG)
        message(FATAL_ERROR "Conflict: dvp_jpeg - jpeg")
    endif()

    if(CONFIG_SOLUTION_FUNC_DVP)
        message(FATAL_ERROR "Conflict: dvp_jpeg - dvp")
    endif()

    if(CONFIG_SOLUTION_FUNC_UVC_JPEG)
        message(FATAL_ERROR "Jpeg_buff queue Conflict: dvp_jpeg - uvc_jpeg")
    endif()

endif()

# Conflict: auadc uac
if((DEFINED CONFIG_SOLUTION_FUNC_UAC_IN) AND (DEFINED CONFIG_SOLUTION_FUNC_AUDIO_AUADC))
    if(CONFIG_SOLUTION_FUNC_UAC_IN AND CONFIG_SOLUTION_FUNC_AUDIO_AUADC)
        message(FATAL_ERROR "Auadc_buff queue Conflict: auadc - uac(in)")
    endif()
endif()
