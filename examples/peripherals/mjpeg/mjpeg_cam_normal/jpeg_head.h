#ifndef __JPEG_HEAD_H__
#define __JPEG_HEAD_H__

#include "stdint.h"

#define YUV_MODE_400				1
#define YUV_MODE_420				2
#define YUV_MODE_422				3

extern uint32_t JpegHeadCreate(uint8_t type, uint8_t q, int width, int height, uint8_t *out);

#endif /* __JPEG_HEAD_H__ */