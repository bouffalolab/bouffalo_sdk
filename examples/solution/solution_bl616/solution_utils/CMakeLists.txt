sdk_generate_library()

# SD add fatfs_conf_user.h
if(CONFIG_FATFS)
    sdk_add_include_directories(sd_card)
endif()

##################################### queue #####################################

# Frame_buff Queue
if(CONFIG_SOLUTION_QUEUE_YUYV)
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_QUEUE_YUYV=${CONFIG_SOLUTION_QUEUE_YUYV})
    
    sdk_add_include_directories(queue)
    sdk_library_add_sources(queue/frame_queue_ctrl.c)
    sdk_library_add_sources(queue/frame_queue.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_QUEUE_YUYV=0)
endif()

# MJPEG Queue
if(CONFIG_SOLUTION_QUEUE_MJPEG)
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_QUEUE_MJPEG=${CONFIG_SOLUTION_QUEUE_MJPEG})
   
    sdk_add_include_directories(queue)
    sdk_library_add_sources(queue/frame_queue_ctrl.c)
    sdk_library_add_sources(queue/frame_queue.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_QUEUE_MJPEG=0)
endif()

# Auadc Queue
if(CONFIG_SOLUTION_QUEUE_AUDIO_IN)
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_QUEUE_AUDIO_IN=${CONFIG_SOLUTION_QUEUE_AUDIO_IN})
   
    sdk_add_include_directories(queue)
    sdk_library_add_sources(queue/frame_queue_ctrl.c)
    sdk_library_add_sources(queue/frame_queue.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_QUEUE_AUDIO_IN=0)
endif()

# Audac Queue
if(CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_QUEUE_AUDIO_OUT=${CONFIG_SOLUTION_QUEUE_AUDIO_OUT})
   
    sdk_add_include_directories(queue)
    sdk_library_add_sources(queue/frame_queue_ctrl.c)
    sdk_library_add_sources(queue/frame_queue.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_QUEUE_AUDIO_OUT=0)
endif()

##################################### audio ####################################

# Audio Auadc
if(CONFIG_SOLUTION_FUNC_AUDIO_AUADC)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_IN) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_IN))
        message(FATAL_ERROR "audio_adc need audio_in_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_AUADC=${CONFIG_SOLUTION_FUNC_AUDIO_AUADC})

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/auadc.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_AUADC=0)
endif()

# Audio Audac
if(CONFIG_SOLUTION_FUNC_AUDIO_AUDAC)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_OUT) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT))
        message(FATAL_ERROR "audio_dac need audio_out_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_AUDAC=${CONFIG_SOLUTION_FUNC_AUDIO_AUDAC})

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/audac.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_AUDAC=0)
endif()

# Audio I2S Input
if(CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_IN) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_IN))
        message(FATAL_ERROR "i2s_input need audio_in_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN=${CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN})

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/external_codec_i2s.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN=0)
endif()

# Audio I2S Output
if(CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_OUT) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT))
        message(FATAL_ERROR "i2s_output need audio_out_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT=${CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT})

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/external_codec_i2s.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT=0)
endif()

# USB UAC Input
if(CONFIG_SOLUTION_FUNC_UAC_IN)
    if((NOT DEFINED CONFIG_CHERRYUSB_HOST) OR (NOT CONFIG_CHERRYUSB_HOST))
        message(FATAL_ERROR "UAC(input) need cherry_usb host")
    endif()

    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_IN) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_IN))
        message(FATAL_ERROR "UAC(input) need audio_in_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_UAC_IN=${CONFIG_SOLUTION_FUNC_UAC_IN})

    if(NOT DEFINED FLAG_UVC_UAC_ADDED)
        set(FLAG_UVC_UAC_ADDED 1)
        # uvc uac protocol stack
        if(CONFIG_SOLUTION_BUILD_FROM_LIB_MODE)
            sdk_add_static_library(usb/libusbh_uvc_uac.a)
        else()
            sdk_add_include_directories(${BL_SDK_BASE}/components/usb/cherryusb/port/ehci)
            add_subdirectory(usbh_uvc_uac usbh_uvc_uac)
        endif()
    endif()

    # callback function
    sdk_add_include_directories(usb)
    sdk_library_add_sources(usb/usbh_uac_callback.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_UAC_IN=0)
endif()

# USB UAC Output
if(CONFIG_SOLUTION_FUNC_UAC_OUT)
    message(FATAL_ERROR "Not suppore uac_output")
endif()

# Audio Lookback
if(CONFIG_SOLUTION_FUNC_AUDIO_LOOPBACK)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_IN) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_IN))
        message(FATAL_ERROR "audio_loopback need audio_in_queue")
    endif()

    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_OUT) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT))
        message(FATAL_ERROR "audio_loopback need audio_out_queue")
    endif()

    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_LOOPBACK=${CONFIG_SOLUTION_FUNC_AUDIO_LOOPBACK})

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/audio_loopback.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_LOOPBACK=0)
endif()

# es8388 codec init
if(CONFIG_SOLUTION_FUNC_AUDIO_ES8388)
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_ES8388=${CONFIG_SOLUTION_FUNC_AUDIO_ES8388})

    sdk_add_include_directories(audio)
    sdk_library_add_sources(audio/codec_es8388_cfg.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_AUDIO_ES8388=0)
endif()

##################################### video ####################################

if(CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH)
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH=${CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH})
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH=640)
endif()

if(CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT)
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT=${CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT})
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT=480)
endif()

# DBI Display
if(CONFIG_SOLUTION_FUNC_DBI_DISP)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_YUYV) OR (NOT CONFIG_SOLUTION_QUEUE_YUYV))
        message(FATAL_ERROR "dbi_display need yuyv_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_DBI_DISP=${CONFIG_SOLUTION_FUNC_DBI_DISP})

    sdk_add_include_directories(dbi_disp)
    sdk_library_add_sources(dbi_disp/dbi_disp.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_DBI_DISP=0)
endif()

# JPEG Compression
if(CONFIG_SOLUTION_FUNC_MJPEG)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "mjpeg_compress need mjpeg_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_MJPEG=${CONFIG_SOLUTION_FUNC_MJPEG})

    sdk_add_include_directories(mjpeg)
    sdk_library_add_sources(mjpeg/mjpeg.c mjpeg/jpeg_head.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_MJPEG=0)
endif()

# DVP Sensor
if(CONFIG_SOLUTION_FUNC_DVP)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_YUYV) OR (NOT CONFIG_SOLUTION_QUEUE_YUYV))
        message(FATAL_ERROR "dvp need yuyv_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_DVP=${CONFIG_SOLUTION_FUNC_DVP})

    sdk_add_include_directories(dvp)
    sdk_library_add_sources(dvp/dvp.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_DVP=0)
endif()

# DVP jpeg line buffer mode
if(CONFIG_SOLUTION_FUNC_DVP_JPEG)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "dvp_jpeg need jpeg_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_DVP_JPEG=${CONFIG_SOLUTION_FUNC_DVP_JPEG})

    sdk_add_include_directories(mjpeg)
    sdk_library_add_sources(mjpeg/dvp_mjpeg.c mjpeg/jpeg_head.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_DVP_JPEG=0)
endif()

# USB Video Class (yuyv mode)
if(CONFIG_SOLUTION_FUNC_UVC_YUYV)
    if((NOT DEFINED CONFIG_CHERRYUSB_HOST) OR (NOT CONFIG_CHERRYUSB_HOST))
        message(FATAL_ERROR "UVC(yuyv) need cherry_usb host")
    endif()

    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_YUYV) OR (NOT CONFIG_SOLUTION_QUEUE_YUYV))
        message(FATAL_ERROR "UVC(yuyv) need yuyv_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_UVC_YUYV=${CONFIG_SOLUTION_FUNC_UVC_YUYV})

    if(NOT DEFINED FLAG_UVC_UAC_ADDED)
        set(FLAG_UVC_UAC_ADDED 1)
        # uvc uvc protocol stack
        if(CONFIG_SOLUTION_BUILD_FROM_LIB_MODE)
            sdk_add_static_library(usb/libusbh_uvc_uac.a)
        else()
            sdk_add_include_directories(${BL_SDK_BASE}/components/usb/cherryusb/port/ehci)
            add_subdirectory(usbh_uvc_uac usbh_uvc_uac)
        endif()
    endif()

    # callback function
    sdk_add_include_directories(usb)
    sdk_library_add_sources(usb/usbh_uvc_callback.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_UVC_YUYV=0)
endif()

# USB Video Class (mjpeg mode)
if(CONFIG_SOLUTION_FUNC_UVC_JPEG)
    if((NOT DEFINED CONFIG_CHERRYUSB_HOST) OR (NOT CONFIG_CHERRYUSB_HOST))
        message(FATAL_ERROR "UVC(jpeg) need cherry_usb host")
    endif()

    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "UVC(jpeg) need mjpeg_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_UVC_JPEG=${CONFIG_SOLUTION_FUNC_UVC_JPEG})

    if(NOT DEFINED FLAG_UVC_UAC_ADDED)
        set(FLAG_UVC_UAC_ADDED 1)
        # uvc uvc protocol stack
        if(CONFIG_SOLUTION_BUILD_FROM_LIB_MODE)
            sdk_add_static_library(usb/libusbh_uvc_uac.a)
        else()
            sdk_add_include_directories(${BL_SDK_BASE}/components/usb/cherryusb/port/ehci)
            add_subdirectory(usbh_uvc_uac usbh_uvc_uac)
        endif()
    endif()

    # callback function
    sdk_add_include_directories(usb)
    sdk_library_add_sources(usb/usbh_uvc_callback.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_UVC_JPEG=0)
endif()

##################################### other ####################################

# Save Video Jpeg to SDcard .avi
if(CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
    if((NOT DEFINED CONFIG_FATFS) OR (NOT CONFIG_FATFS))
        message(FATAL_ERROR "sdcard need fatfs")
    endif()
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "avi vids need mjpeg_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS=${CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS})

    sdk_add_include_directories(sd_card)
    sdk_library_add_sources(sd_card/avi_jpeg_sd.c)
    sdk_library_add_sources(sd_card/jpeg_sd.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS=0)
endif()

# Save Audio PCM to SDcard .avi
if(CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS)
    if((NOT DEFINED CONFIG_FATFS) OR (NOT CONFIG_FATFS))
        message(FATAL_ERROR "sdcard need fatfs")
    endif()
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_AUDIO_OUT) OR (NOT CONFIG_SOLUTION_QUEUE_AUDIO_OUT))
        message(FATAL_ERROR "avi auds need audio_out_queue")
    endif()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS=${CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS})

    sdk_add_include_directories(sd_card)
    sdk_library_add_sources(sd_card/avi_jpeg_sd.c)
    sdk_library_add_sources(sd_card/jpeg_sd.c)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS=0)
endif()

# Hibooster (image_transmission)
if(CONFIG_SOLUTION_FUNC_HIBOOSTER)

    if(CONFIG_HIBOOSTER_SIMU_DATA_TEST)
        sdk_add_compile_definitions(-D SIMU_DATA_TEST)
    elseif((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "UVC(jpeg) need mjpeg_queue")
    endif()

    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_HIBOOSTER=${CONFIG_SOLUTION_FUNC_HIBOOSTER})

    sdk_library_add_sources(hibooster/bl_cam_frame.c)

    if(CONFIG_SOLUTION_BUILD_FROM_LIB_MODE)
        sdk_add_static_library(hibooster/libhibooster.a)
        sdk_add_include_directories(hibooster/include .)
    else()
        add_subdirectory(hibooster)
    endif()
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_HIBOOSTER=0)
endif()
# rtsp (image_transmission)
if(CONFIG_SOLUTION_FUNC_RTSP)
    if((NOT DEFINED CONFIG_SOLUTION_QUEUE_MJPEG) OR (NOT CONFIG_SOLUTION_QUEUE_MJPEG))
        message(FATAL_ERROR "RTSP(jpeg) need mjpeg_queue")
    endif()

    sdk_add_compile_definitions(-DCONFIG_SOLUTION_FUNC_RTSP=${CONFIG_SOLUTION_FUNC_RTSP})

    sdk_library_add_sources(rtsp/bl_cam_rtsp.c)

     sdk_add_include_directories(rtsp/include .)
else()
    sdk_add_compile_definitions(-D CONFIG_SOLUTION_FUNC_RTSP=0)
endif()
##################################### Conflict Check #####################################
## Conflict: dvp uvc
if((DEFINED CONFIG_SOLUTION_FUNC_DVP) AND (DEFINED CONFIG_SOLUTION_FUNC_UVC_YUYV))
    if(CONFIG_SOLUTION_FUNC_DVP AND CONFIG_SOLUTION_FUNC_UVC_YUYV)
        message(FATAL_ERROR "Frame_buff queue Conflict: dvp - uvc_yuyv")
    endif()
endif()

## Conflict: dvp_jpeg
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


## Conflict: auadc uac
if((DEFINED CONFIG_SOLUTION_FUNC_UAC_IN) AND (DEFINED CONFIG_SOLUTION_FUNC_AUDIO_AUADC))
    if(CONFIG_SOLUTION_FUNC_UAC_IN AND CONFIG_SOLUTION_FUNC_AUDIO_AUADC)
        message(FATAL_ERROR "Auadc_buff queue Conflict: auadc - uac(in)")
    endif()
endif()

