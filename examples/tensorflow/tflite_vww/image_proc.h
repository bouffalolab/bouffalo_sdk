
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bsp_sf_psram.h"
#ifdef __cplusplus
extern "C" {
#endif

int8_t *image_proc(uint8_t *picture, int frame_size, int CAMERA_RESOLUTION_X, int CAMERA_RESOLUTION_Y, int *piture_lcd);
// all of your legacy C code here

#ifdef __cplusplus
}
#endif