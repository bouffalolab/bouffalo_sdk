
#ifndef USBH_UVC_STREAM_H
#define USBH_UVC_STREAM_H

#include "usbh_core.h"
#include "usbh_video.h"

struct uvc_iso_buff_s {
    uint8_t *buff_1;
    uint8_t *buff_2;
    uint32_t buff_size;
    uint16_t iso_interval;
    uint16_t packet_cnt;
    uint16_t ep_mps;
};

extern volatile uint32_t video_complete_count;

/* cb, implemented by user */
void usbh_video_fps_record(void);
void usbh_video_transfer_abort_callback(void);
void usbh_voide_iso_buff_get(struct uvc_iso_buff_s *uvc_iso_buff);

/* dma port, implemented by user */
void usbh_video_dma_init(void);
void usbh_video_dma_lli_fill(uint32_t desc_index, uint32_t src_addr, uint32_t dst_addr, uint32_t nbytes);
void usbh_video_dma_start(void);
void usbh_video_dma_stop(void);
bool usbh_video_dma_isbusy(void);

/* frame buff port, implemented by user */
struct usbh_videoframe *usbh_uvc_frame_alloc(void);
int usbh_uvc_frame_free(struct usbh_videoframe *frame);
int usbh_uvc_frame_send(struct usbh_videoframe *frame);

/* uvc api */
void usbh_video_stream_start(uint16_t width, uint16_t height, uint8_t format);
void usbh_video_stream_stop(void);

#endif