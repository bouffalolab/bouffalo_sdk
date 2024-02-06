#ifndef USBH_UVC_UAC_H
#define USBH_UVC_UAC_H

#include "usbh_core.h"
#include "usbh_video.h"
#include "usbh_audio.h"

#include "bflb_dma.h"

#define VIDEO_ISO_INTERVAL (2)
#define VIDEO_ISO_PACKETS  (8 * VIDEO_ISO_INTERVAL)
#define VIDEO_EP_MPS       3072

extern volatile uint16_t g_video_width;
extern volatile uint16_t g_video_height;
extern volatile uint16_t g_video_format;
extern volatile uint16_t g_video_altsetting;

extern volatile uint32_t g_uvc_fps;

extern struct usbh_videostreaming g_uvcsteam;
extern volatile void *g_uvc_frame_buff;

extern void usbh_video_one_frame_callback_mjpeg(struct usbh_videostreaming *stream);
extern void usbh_video_one_frame_callback_yuyv(struct usbh_videostreaming *stream);
struct bflb_device_s *usbh_video_get_dma_ch(void);
void usbh_video_fps_record(void);
void usbh_video_test(void);
extern void usbh_video_run_callback(void);
extern void usbh_video_stop_callback(void);
extern void usbh_video_set_config(uint16_t width,
                                  uint16_t height,
                                  uint8_t format,
                                  uint8_t altsetting);

#define AUDIO_SAMPLING_FREQ 16000
#define AUDIO_ISO_PACKETS   4
#define AUDIO_EP_MPS        512

extern volatile void *g_uac_frame_buff;

extern void usbh_audio_one_frame_callback(uint32_t data_size);
struct bflb_device_s *usbh_audio_get_dma_ch(void);
extern void usbh_audio_test(void);
extern void usbh_audio_run_callback(void);
extern void usbh_audio_stop_callback(void);

#endif