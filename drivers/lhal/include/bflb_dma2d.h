#ifndef _BFLB_DMA2D_H
#define _BFLB_DMA2D_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup DMA2D
  * @{
  */

/** @defgroup DMA2D_DATA_WIDTH dma2d data width definition
  * @{
  */
#define DMA2D_DATA_WIDTH_8BIT           0
#define DMA2D_DATA_WIDTH_16BIT          1
#define DMA2D_DATA_WIDTH_32BIT          2
#define DMA2D_DATA_WIDTH_64BIT          3
/**
  * @}
  */

/** @defgroup DMA2D_BURST_COUNT dma2d burst increment count definition
  * @{
  */
#define DMA2D_BURST_INCR1               0
#define DMA2D_BURST_INCR4               1
#define DMA2D_BURST_INCR8               2
#define DMA2D_BURST_INCR16              3
/**
  * @}
  */

/** @defgroup DMA2D_IMAGE_GEOMETRIC_TRANSFOR dma2d image geometric transfor mode definition
  * @{
  */
#define DMA2D_IMAGE_TRANSLATE           0
#define DMA2D_IMAGE_FLIP_HORIZONTAL     1
#define DMA2D_IMAGE_FLIP_VERTICAL       2
#define DMA2D_IMAGE_ROTATE_DEGREE_90    3
#define DMA2D_IMAGE_ROTATE_DEGREE_180   4
#define DMA2D_IMAGE_ROTATE_DEGREE_270   5
#define DMA2D_IMAGE_FILL                6
/**
  * @}
  */

/** @defgroup DMA2D_COLOR_KEY_MODE dma2d color key mode definition
  * @}
  */
#define DMA2D_COLOR_KEY_MODE_8BIT       0
#define DMA2D_COLOR_KEY_MODE_16BIT      1
#define DMA2D_COLOR_KEY_MODE_24BIT      2
#define DMA2D_COLOR_KEY_MODE_32BIT      3
/**
  * @}
  */

/** @defgroup DMA2D_COLOR_KEY_MASK dma2d color key mask definition
  * @}
  */
#define DMA2D_COLOR_KEY_MASK_0X000000FF (1 << 0)
#define DMA2D_COLOR_KEY_MASK_0X0000FF00 (1 << 1)
#define DMA2D_COLOR_KEY_MASK_0X00FF0000 (1 << 2)
#define DMA2D_COLOR_KEY_MASK_0XFF000000 (1 << 3)
/**
  * @}
  */

/** @defgroup DMA2D_CMD dma2d feature control cmd definition
  * @{
  */

/**
  * @}
  */

// clang-format off
#define IS_DMA2D_DATA_WIDTH(type) (((type) == DMA2D_DATA_WIDTH_8BIT) || \
                                   ((type) == DMA2D_DATA_WIDTH_16BIT) || \
                                   ((type) == DMA2D_DATA_WIDTH_32BIT) || \
                                   ((type) == DMA2D_DATA_WIDTH_64BIT))

#define IS_DMA2D_BURST_COUNT(type) (((type) == DMA2D_BURST_INCR1) || \
                                    ((type) == DMA2D_BURST_INCR4) || \
                                    ((type) == DMA2D_BURST_INCR8) || \
                                    ((type) == DMA2D_BURST_INCR16))

#define IS_DMA2D_IMAGE_GEOMETRIC_TRANSFOR(type) (((type) == DMA2D_IMAGE_TRANSLATE) || \
                                                 ((type) == DMA2D_IMAGE_FLIP_HORIZONTAL) || \
                                                 ((type) == DMA2D_IMAGE_FLIP_VERTICAL) || \
                                                 ((type) == DMA2D_IMAGE_ROTATE_DEGREE_90) || \
                                                 ((type) == DMA2D_IMAGE_ROTATE_DEGREE_180) || \
                                                 ((type) == DMA2D_IMAGE_ROTATE_DEGREE_270) || \
                                                 ((type) == DMA2D_IMAGE_FILL))

#define IS_DMA2D_COLOR_KEY_MODE(type) (((type) == DMA2D_COLOR_KEY_MODE_8BIT) || \
                                       ((type) == DMA2D_COLOR_KEY_MODE_16BIT) || \
                                       ((type) == DMA2D_COLOR_KEY_MODE_24BIT) || \
                                       ((type) == DMA2D_COLOR_KEY_MODE_32BIT))

#define IS_DMA2D_COLOR_KEY_MASK(type) (((type) == DMA2D_COLOR_KEY_MASK_0X000000FF) || \
                                       ((type) == DMA2D_COLOR_KEY_MASK_0X0000FF00) || \
                                       ((type) == DMA2D_COLOR_KEY_MASK_0X00FF0000) || \
                                       ((type) == DMA2D_COLOR_KEY_MASK_0XFF000000))

// clang-format on

/**
 * @brief DMA2D channel control union
 *
 * @param transfer_size       DMA2D source transfer size, only effective in normal DMA mode
 * @param src_burst           DMA2D source burst size, use @ref DMA2D_BURST_COUNT
 * @param dst_burst           DMA2D destination burst size, use @ref DMA2D_BURST_COUNT
 * @param src_data_width      DMA2D source data width, use @ref DMA2D_DATA_WIDTH
 * @param dst_data_width      DMA2D destination data width, use @ref DMA2D_DATA_WIDTH
 * @param src_incr            DMA2D source increment, only effective in normal DMA mode
 * @param dst_incr            DMA2D destination increment, only effective in normal DMA mode
 * @param int_enable          DMA2D channel interrupt enable
 */
union bflb_dma2d_channel_control_s {
    struct
    {
        uint32_t transfer_size  : 12; /* [11: 0],        r/w,        0x0 */
        uint32_t src_burst      : 2;  /* [13:12],        r/w,        0x1 */
        uint32_t                : 1;  /* [   14],       rsvd,        0x0 */
        uint32_t dst_burst      : 2;  /* [16:15],        r/w,        0x1 */
        uint32_t                : 1;  /* [   17],       rsvd,        0x0 */
        uint32_t src_data_width : 2;  /* [19:18],        r/w,        0x2 */
        uint32_t                : 1;  /* [   20],       rsvd,        0x0 */
        uint32_t dst_data_width : 2;  /* [22:21],        r/w,        0x2 */
        uint32_t                : 3;  /* [25:23],       rsvd,        0x0 */
        uint32_t src_incr       : 1;  /* [   26],        r/w,        0x1 */
        uint32_t dst_incr       : 1;  /* [   27],        r/w,        0x1 */
        uint32_t                : 3;  /* [30:28],       rsvd,        0x0 */
        uint32_t int_enable     : 1;  /* [   31],        r/w,        0x0 */
    } bits;
    uint32_t word;
};

/**
 * @brief DMA2D channel loop and step structure
 *
 * @param src_cnt_x           DMA2D source x count
 * @param src_cnt_y           DMA2D source y count
 * @param src_incr_x          DMA2D source x increment
 * @param src_incr_y          DMA2D source y increment
 * @param dst_cnt_x           DMA2D destination x count
 * @param dst_incr_x          DMA2D destination x increment
 * @param dst_incr_y          DMA2D destination y increment
 */
struct bflb_dma2d_channel_loop_step_s {
    uint16_t src_cnt_x;
    uint16_t src_cnt_y;
    int32_t src_incr_x;
    int32_t src_incr_y;
    uint16_t dst_cnt_x;
    uint16_t reserved;
    int32_t dst_incr_x;
    int32_t dst_incr_y;
};

/**
 * @brief DMA2D channel configuration structure
 *
 * @param src_addr            DMA2D source address
 * @param dst_addr            DMA2D destination address
 * @param next_lli_addr       DMA2D next lli address, bit 0 is enable bit
 * @param control             DMA2D channel control config
 * @param loop_step           DMA2D channel loop and step config
 */
struct bflb_dma2d_channel_config_s {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t next_lli_addr;
    union bflb_dma2d_channel_control_s control;
    struct bflb_dma2d_channel_loop_step_s loop_step;
};

/**
 * @brief DMA2D image structure
 *
 * @param transfer_data_width DMA2D transfer data width, use @ref DMA2D_DATA_WIDTH
 * @param pixel_data_width    DMA2D pixel data width, unit: byte
 * @param src_image_addr      DMA2D source image starting address
 * @param src_image_width     DMA2D source image width, unit: pixel
 * @param src_x_start         DMA2D source starting point x coordinate
 * @param src_y_start         DMA2D source starting point y coordinate
 * @param src_x_end           DMA2D source ending point x coordinate
 * @param src_y_end           DMA2D source ending point y coordinate
 * @param dst_image_addr      DMA2D destination image starting address
 * @param dst_image_width     DMA2D destination image width, unit: pixel
 * @param dst_x_start         DMA2D destination starting point x coordinate
 * @param dst_y_start         DMA2D destination starting point y coordinate
 */
struct bflb_dma2d_image_s {
    uint8_t transfer_data_width;
    uint8_t pixel_data_width;
    uint32_t src_image_addr;
    uint32_t src_image_width;
    uint32_t src_x_start;
    uint32_t src_y_start;
    uint32_t src_x_end;
    uint32_t src_y_end;
    uint32_t dst_image_addr;
    uint32_t dst_image_width;
    uint32_t dst_x_start;
    uint32_t dst_y_start;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize dma2d channel.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save dma2d channel configuration
 */
void bflb_dma2d_channel_init(struct bflb_device_s *dev, struct bflb_dma2d_channel_config_s *config);

/**
 * @brief Deinitialize dma2d channel.
 *
 * @param [in] dev device handle
 */
void bflb_dma2d_channel_deinit(struct bflb_device_s *dev);

/**
 * @brief Start dma2d channel transfer.
 *
 * @param [in] dev device handle
 */
void bflb_dma2d_channel_start(struct bflb_device_s *dev);

/**
 * @brief Stop dma2d channel transfer.
 *
 * @param [in] dev device handle
 */
void bflb_dma2d_channel_stop(struct bflb_device_s *dev);

/**
 * @brief Check if dma2d channel is in busy.
 *
 * @param [in] dev device handle
 * @return true means dma2d channel is enable, otherwise means disable.
 */
bool bflb_dma2d_channel_isbusy(struct bflb_device_s *dev);

/**
 * @brief Register dma2d channel transmission completion interrupt callback.
 *
 * @param [in] dev device handle
 * @param [in] callback interrupt callback
 * @param [in] arg user data
 */
void bflb_dma2d_channel_irq_attach(struct bflb_device_s *dev, void (*callback)(void *arg), void *arg);

/**
 * @brief Unregister dma2d channel transmission completion interrupt callback.
 *
 * @param [in] dev device handle
 */
void bflb_dma2d_channel_irq_detach(struct bflb_device_s *dev);

/**
 * @brief Enable or disable dma2d channel transmission completion interrupt.
 *
 * @param [in] dev device handle
 * @param [in] mask true means disable, false means enable
 */
void bflb_dma2d_channel_tcint_mask(struct bflb_device_s *dev, bool mask);

/**
 * @brief Check if dma2d channel transfers completely.
 *
 * @param [in] dev device handle
 * @return true means yes, false means no.
 */
bool bflb_dma2d_channel_get_tcint_status(struct bflb_device_s *dev);

/**
 * @brief Clear dma2d channel transmission completion interrupt status.
 *
 * @param [in] dev device handle
 */
void bflb_dma2d_channel_tcint_clear(struct bflb_device_s *dev);

/**
 * @brief Calculate dma2d configuration according to image geometric transfor mode.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save dma2d channel configuration
 * @param [in] image pointer to dma2d image information
 * @param [in] mode image geometric transfor mode, use @ref DMA2D_IMAGE_GEOMETRIC_TRANSFOR
 */
void bflb_dma2d_image_geometric_transfor_calculate(struct bflb_device_s *dev, struct bflb_dma2d_channel_config_s *config,
                                                   struct bflb_dma2d_image_s *image, uint32_t mode);

/**
 * @brief Enable dma2d channel color key function.
 *
 * @param [in] dev device handle
 */
void bflb_dma2d_channel_enable_color_key(struct bflb_device_s *dev);

/**
 * @brief Disable dma2d channel color key function.
 *
 * @param [in] dev device handle
 */
void bflb_dma2d_channel_disable_color_key(struct bflb_device_s *dev);

/**
 * @brief Set dma2d channel color key mode and value.
 * @param [in] dev device handle
 * @param [in] mode color key mode, use @ref DMA2D_COLOR_KEY_MODE
 * @param [in] value color key value
 * @param [in] mask color key mask, use @ref DMA2D_COLOR_KEY_MASK
 */
void bflb_dma2d_channel_set_color_key(struct bflb_device_s *dev, uint8_t mode, uint32_t value, uint8_t mask);

/**
 * @brief Control dma2d feature.
 * @param [in] dev device handle
 * @param [in] cmd feature command, use @ref DMA2D_CMD
 * @param [in] arg user data
 * @return A negated errno value on failure.
 */
int bflb_dma2d_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

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
