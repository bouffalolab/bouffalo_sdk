#ifndef _BFLB_MJPEG_H
#define _BFLB_MJPEG_H

#include "bflb_core.h"

/** @defgroup MJPEG_FORMAT mjpeg format definition
  * @{
  */
#define MJPEG_FORMAT_YUV422_YUYV   0
#define MJPEG_FORMAT_YUV422_YVYU   1
#define MJPEG_FORMAT_YUV422_UYVY   2
#define MJPEG_FORMAT_YUV422_VYUY   3
#define MJPEG_FORMAT_YUV422SP_NV16 4
#define MJPEG_FORMAT_YUV422SP_NV61 5
#define MJPEG_FORMAT_YUV420SP_NV12 6
#define MJPEG_FORMAT_YUV420SP_NV21 7
#define MJPEG_FORMAT_GRAY          8
/**
  * @}
  */

/** @defgroup MJPEG_INTSTS mjpeg interrupt status definition
  * @{
  */
#define MJPEG_INTSTS_ONE_FRAME (1 << 4)
/**
  * @}
  */

/** @defgroup MJPEG_INTCLR mjpeg interrupt clear definition
  * @{
  */
#define MJPEG_INTCLR_ONE_FRAME (1 << 8)
/**
  * @}
  */

#define MJPEG_MAX_FRAME_COUNT 4

/**
 * @brief MJPEG configuration structure
 *
 * @param format            MJPEG format, use @ref MJPEG_FORMAT
 * @param resolution_x      MJPEG width, must be a multiple of 8 or a multiple of 16
 * @param resolution_y      MJPEG higth, must be a multiple of 8 or a multiple of 16
 * @param input_bufaddr0    MJPEG input buffer address 0 for yy , must be align 16
 * @param input_bufaddr1    MJPEG input buffer address 0 for yy , must be align 16
 * @param output_bufaddr    MJPEG output buffer address , must be align 16
 * @param output_bufsize    MJPEG output buffer size, must be larger than resolution_x*resolution_y*2*MJPEG_MAX_FRAME_COUNT
 */
struct bflb_mjpeg_config_s {
    uint8_t format;
    uint16_t resolution_x;
    uint16_t resolution_y;
    uint32_t input_bufaddr0;
    uint32_t input_bufaddr1;
    uint32_t output_bufaddr;
    uint32_t output_bufsize;
};

#ifdef __cplusplus
extern "C" {
#endif

void bflb_mjpeg_init(struct bflb_device_s *dev, const struct bflb_mjpeg_config_s *config);
void bflb_mjpeg_start(struct bflb_device_s *dev);
void bflb_mjpeg_stop(struct bflb_device_s *dev);
void bflb_mjpeg_sw_run(struct bflb_device_s *dev, uint8_t frame_count);
void bflb_mjpeg_tcint_mask(struct bflb_device_s *dev, bool mask);
void bflb_mjpeg_errint_mask(struct bflb_device_s *dev, bool mask);
uint32_t bflb_mjpeg_get_intstatus(struct bflb_device_s *dev);
void bflb_mjpeg_int_clear(struct bflb_device_s *dev, uint32_t int_clear);
uint8_t bflb_mjpeg_get_frame_count(struct bflb_device_s *dev);
void bflb_mjpeg_pop_one_frame(struct bflb_device_s *dev);
uint32_t bflb_mjpeg_get_frame_info(struct bflb_device_s *dev, uint8_t **pic);
void bflb_mjpeg_calculate_quantize_table(uint8_t quality, uint16_t *input_table, uint16_t *output_table);
void bflb_mjpeg_fill_quantize_table(struct bflb_device_s *dev, uint16_t *input_yy, uint16_t *input_uv);
void bflb_mjpeg_fill_jpeg_header_tail(struct bflb_device_s *dev, uint8_t *header, uint32_t header_len);
void bflb_mjpeg_set_yuv420sp_cam_input(struct bflb_device_s *dev, uint8_t yy, uint8_t uv);

#ifdef __cplusplus
}
#endif

#endif