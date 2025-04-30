#ifndef __BL_CAM_PIC_H__
#define __BL_CAM_PIC_H__
#include <stdint.h>
void bl_cam_frame_init(void);
int bl_cam_frame_fifo_get(uint32_t *c_frames, uint8_t **c_ptr1, uint32_t *c_len1, uint8_t **c_ptr2, uint32_t *c_len2, int pic_mode);
int bl_cam_frame_get_info(uint8_t *quality, uint16_t *width, uint16_t *height);
int bl_cam_frame_config(uint8_t quality, uint16_t width, uint16_t height);
int bl_cam_frame_pop(void);
int bl_cam_frame_reset(void);
#endif
