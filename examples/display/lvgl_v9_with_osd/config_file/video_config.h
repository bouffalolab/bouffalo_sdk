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

/* The actual symbols in display/default/Kconfig are DISPLAY_DEFAULT_DSI,
 * DISPLAY_DEFAULT_RGB_CMD and the VIDEO_RGB_* module choices; there are no
 * DISPLAY_DEFAULT_VIDEO_DPI_* / VIDEO_INTERFACE_* symbols in this framework.
 * Panel selection and pixel clock remain main/app_display's responsibility. */
#if defined(CONFIG_DISPLAY_DEFAULT_DSI) && CONFIG_DISPLAY_DEFAULT_DSI
#define VIDEO_INTERFACE_DPI 0
#elif (defined(CONFIG_DISPLAY_DEFAULT_RGB_CMD) && CONFIG_DISPLAY_DEFAULT_RGB_CMD) || \
	(defined(CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_800X480) && CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_800X480) || \
	(defined(CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_1024X600) && CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_1024X600)
#define VIDEO_INTERFACE_DPI 1
#else
#error "Select a DSI or RGB video panel in the display framework"
#endif

/* SW GPIO clock compatibility knob. New display clock/pin setup belongs to main;
 * the video pipeline never toggles panel GPIOs or programs display registers. */
#ifndef DPI_PIXEL_CLOCK_USE_SW_GPIO
#define DPI_PIXEL_CLOCK_USE_SW_GPIO 0
#endif

/* ---- Video frame-rate cap ------------------------------------------------ */
#ifndef VIDEO_TARGET_FPS
#define VIDEO_TARGET_FPS   100U
#endif

/* ---- SD-card frame directory --------------------------------------------- */
#if VIDEO_INTERFACE_DPI
/* dpi: (704x384 on 800x480), (960x540 on 1024x600), or full-screen.
 * Override VIDEO_WIDTH/HEIGHT and FRAME_DIR together for centered sub-panel video. */
#ifndef FRAME_DIR
#define FRAME_DIR        "/sd/1024x600"
#endif
#ifndef VIDEO_WIDTH
#define VIDEO_WIDTH     1024U
#endif
#ifndef VIDEO_HEIGHT
#define VIDEO_HEIGHT    600U
#endif
#else
/* dsi: (384x768 on 480x960/480x854), (576x1024 on 720x1280),
 *      (96x528 on 172x640), or full-screen. The default EK79007 is 1024x600;
 *      the old 258x960 default belongs to AXS15231E and cannot fit EK79007. */
#ifndef FRAME_DIR
#define FRAME_DIR        "/sd/1024x600"
#endif
#ifndef VIDEO_WIDTH
#define VIDEO_WIDTH     1024U
#endif
#ifndef VIDEO_HEIGHT
#define VIDEO_HEIGHT    600U
#endif
#endif

/* Number of frames per chunk sub-directory (CCC). */
#ifndef FRAMES_PER_CHUNK
#define FRAMES_PER_CHUNK 100U
#endif
#if FRAMES_PER_CHUNK == 0
#error "FRAMES_PER_CHUNK must be positive"
#endif

#endif /* _VIDEO_CONFIG_H_ */