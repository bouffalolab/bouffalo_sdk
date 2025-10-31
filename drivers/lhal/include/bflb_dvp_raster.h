#ifndef _BFLB_DVP_RASTER_H
#define _BFLB_DVP_RASTER_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup DVP_RASTER
  * @{
  */

/** @defgroup DVP_RASTER_GEOMETRIC_TRANSFOR dvp raster geometric transfor mode definition
  * @{
  */
#define DVP_RASTER_TRANSLATE         (0 << 6)
#define DVP_RASTER_FLIP_HORIZONTAL   (1 << 4)
#define DVP_RASTER_FLIP_VERTICAL     (1 << 5)
#define DVP_RASTER_ROTATE_DEGREE_90  (1 << 6)
#define DVP_RASTER_ROTATE_DEGREE_180 (2 << 6)
#define DVP_RASTER_ROTATE_DEGREE_270 (3 << 6)
/**
  * @}
  */

/** @defgroup DVP_RASTER_INT dvp raster interrupt definition
  * @{
  */
#define DVP_RASTER_INT_FRAME_DONE    (1 << 0)
#define DVP_RASTER_INT_RAM_OVERWRITE (1 << 1)
#define DVP_RASTER_INT_ALL           (DVP_RASTER_INT_FRAME_DONE | DVP_RASTER_INT_RAM_OVERWRITE)
/**
  * @}
  */

/** @defgroup DVP_RASTER_CMD dvp raster feature control cmd definition
  * @{
  */
#define DVP_RASTER_CMD_SET_H_BLANK   (0x01)
#define DVP_RASTER_CMD_SET_V_BLANK   (0x02)

/**
  * @}
  */

/**
 * @brief DVP raster configuration structure
 *
 * @param mode          DVP raster mode, use @ref DVP_RASTER_GEOMETRIC_TRANSFOR
 * @param resolution_x  the number of pixels in a line
 * @param resolution_y  the number of lines in a frame
 * @param y_frame_addr  start address of buffer to store y data
 * @param y_frame_cnt   frame count of buffer to store y data
 * @param uv_frame_addr start address of buffer to store uv data
 * @param uv_frame_cnt  frame count of buffer to store uv data
 */
struct bflb_dvp_raster_config_s {
    uint32_t mode;
    uint16_t resolution_x;
    uint16_t resolution_y;
    uint32_t y_frame_addr;
    uint8_t y_frame_cnt;
    uint32_t uv_frame_addr;
    uint8_t uv_frame_cnt;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize dvp raster.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save dvp raster config
 */
void bflb_dvp_raster_init(struct bflb_device_s *dev, struct bflb_dvp_raster_config_s *config);

/**
 * @brief Deinitialize dvp raster.
 *
 * @param [in] dev device handle
 */
void bflb_dvp_raster_deinit(struct bflb_device_s *dev);

/**
 * @brief Enable dvp raster.
 *
 * @param [in] dev device handle
 */
void bflb_dvp_raster_enable(struct bflb_device_s *dev);

/**
 * @brief Disable dvp raster.
 *
 * @param [in] dev device handle
 */
void bflb_dvp_raster_disable(struct bflb_device_s *dev);

/**
 * @brief Enable or disable dvp raster interrupt.
 *
 * @param [in] dev device handle
 * @param [in] int_type type of dvp raster interrupt, use @ref DVP_RASTER_INT
 * @param [in] mask true means disable, false means enable
 */
void bflb_dvp_raster_int_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask);

/**
 * @brief Get dvp raster interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_type type of dvp raster interrupt, use @ref DVP_RASTER_INT
 * @return interrupt status
 */
bool bflb_dvp_raster_get_intstatus(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Clear dvp raster interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_type type of dvp raster interrupt, use @ref DVP_RASTER_INT
 */
void bflb_dvp_raster_int_clear(struct bflb_device_s *dev, uint32_t int_type);

/**
 * @brief Crop vsync.
 *
 * @param [in] dev device handle
 * @param [in] start_line start line of window
 * @param [in] end_line end line of window, not include
 */
void bflb_dvp_raster_crop_vsync(struct bflb_device_s *dev, uint16_t start_line, uint16_t end_line);

/**
 * @brief Crop hsync.
 *
 * @param [in] dev device handle
 * @param [in] start_pixel start pixel of each line
 * @param [in] end_pixel end pixel of each line, not include
 */
void bflb_dvp_raster_crop_hsync(struct bflb_device_s *dev, uint16_t start_pixel, uint16_t end_pixel);

/**
 * @brief Control dvp raster feature.
 *
 * @param [in] dev device handle
 * @param [in] cmd feature command, use @ref DVP_RASTER_CMD
 * @param [in] arg user data
 * @return A negated errno value on failure.
 */
int bflb_dvp_raster_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#endif
