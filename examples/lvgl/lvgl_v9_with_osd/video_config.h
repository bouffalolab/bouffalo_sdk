/**
 * @file video_config.h
 * @brief Customer-editable knobs for the video-playback example, gathered here.
 *
 * Edit only this file to change:
 *   - background video resolution  VIDEO_WIDTH / VIDEO_HEIGHT
 *   - video frame-rate cap         VIDEO_TARGET_FPS
 *   - SD-card frame directory      FRAME_DIR
 *   - frames per chunk             FRAMES_PER_CHUNK
 *
 * SD-card frame layout: <FRAME_DIR>/CCC/pNNNN.jpg
 *   CCC  = chunk index (000,001,...), each chunk holds FRAMES_PER_CHUNK frames
 *   NNNN = frame index (0000,0001,...) */
#ifndef _VIDEO_CONFIG_H_
#define _VIDEO_CONFIG_H_

#include "lcd.h" /* LCD_INTERFACE_TYPE / LCD_INTERFACE_DPI (panel interface type) */

/* SW GPIO clock. */
#if defined(LCD_DPI_STANDARD)
#define DPI_PIXEL_CLOCK_USE_SW_GPIO 1
#else
#define DPI_PIXEL_CLOCK_USE_SW_GPIO 0
#endif

/* ---- Video frame-rate cap ------------------------------------------------ */
#ifndef VIDEO_TARGET_FPS
#define VIDEO_TARGET_FPS   100U
#endif

/* ---- SD-card frame directory --------------------------------------------- */
#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI)
/*  dpi:(704x384 800x480) (960x540 1024x600) */
#define FRAME_DIR        "/sd/1024x600"
#define VIDEO_WIDTH    1024
#define VIDEO_HEIGHT   600

#else
/*  dsi:(384x768 480x960) (384x768 480x854) (576x1024 720x1280)
 *      (96x528 src:176(video:172)x640) */
#define FRAME_DIR        "/sd/258x960"
#define VIDEO_WIDTH    258
#define VIDEO_HEIGHT   960

#endif

/* Number of frames per chunk sub-directory (CCC). */
#define FRAMES_PER_CHUNK 100

#endif /* _VIDEO_CONFIG_H_ */
