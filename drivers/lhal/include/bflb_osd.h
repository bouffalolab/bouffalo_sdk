#ifndef _BFLB_OSD_H
#define _BFLB_OSD_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup OSD
  * @{
  */

/** @defgroup OSD_BLEND_FORMAT osd blend color format definition
  * @{
  */
#define OSD_BLEND_FORMAT_ARGB8888          0
#define OSD_BLEND_FORMAT_AYUV8888          1
#define OSD_BLEND_FORMAT_ARGB4444          2
#define OSD_BLEND_FORMAT_AYUV4444          3
#define OSD_BLEND_FORMAT_ARGB1555          4
#define OSD_BLEND_FORMAT_AYUV1555          5
#define OSD_BLEND_FORMAT_RGB565            6
#define OSD_BLEND_FORMAT_YUV655            7
#define OSD_BLEND_FORMAT_A8_GLOBAL_RGB     8
#define OSD_BLEND_FORMAT_A8_GLOBAL_YUV     9
#define OSD_BLEND_FORMAT_ARGB_PALETTE_8BIT 10
#define OSD_BLEND_FORMAT_AYUV_PALETTE_8BIT 11
#define OSD_BLEND_FORMAT_ARGB_PALETTE_4BIT 12
#define OSD_BLEND_FORMAT_AYUV_PALETTE_4BIT 13
#define OSD_BLEND_FORMAT_ARGB_PALETTE_2BIT 14
#define OSD_BLEND_FORMAT_AYUV_PALETTE_2BIT 15
#define OSD_BLEND_FORMAT_ARGB_PALETTE_1BIT 16
#define OSD_BLEND_FORMAT_AYUV_PALETTE_1BIT 17
/**
  * @}
  */

/** @defgroup OSD_DRAW_LAYER osd draw layer definition
  * @{
  */
#define OSD_DRAW_LAYER_0                   0
#define OSD_DRAW_LAYER_1                   1
#define OSD_DRAW_LAYER_2                   2
#define OSD_DRAW_LAYER_3                   3
#define OSD_DRAW_LAYER_4                   4
#define OSD_DRAW_LAYER_5                   5
#define OSD_DRAW_LAYER_6                   6
#define OSD_DRAW_LAYER_7                   7
#define OSD_DRAW_LAYER_8                   8
#define OSD_DRAW_LAYER_9                   9
#define OSD_DRAW_LAYER_10                  10
#define OSD_DRAW_LAYER_11                  11
#define OSD_DRAW_LAYER_12                  12
#define OSD_DRAW_LAYER_13                  13
#define OSD_DRAW_LAYER_14                  14
#define OSD_DRAW_LAYER_15                  15
/**
  * @}
  */

/** @defgroup OSD_DRAW_YUV_COLOR osd draw yuv color definition
  * @{
  */
#define OSD_DRAW_YUV_COLOR_BLACK           (0x008080) /* (0, 128, 128) */
#define OSD_DRAW_YUV_COLOR_WHITE           (0xFF8080) /* (255, 128, 128) */
#define OSD_DRAW_YUV_COLOR_RED             (0x4C54FF) /* (76, 84, 255) */
#define OSD_DRAW_YUV_COLOR_GREEN           (0x952B15) /* (149, 43, 21) */
#define OSD_DRAW_YUV_COLOR_BLUE            (0x1DFF6B) /* (29, 255, 107) */

#define OSD_DRAW_YUV_COLOR_SILVER          (0xC08080) /* (192, 128, 128) */
#define OSD_DRAW_YUV_COLOR_GRAY            (0x808080) /* (128, 128, 128) */
#define OSD_DRAW_YUV_COLOR_DIMGRAY         (0x698080) /* (105, 128, 128) */
#define OSD_DRAW_YUV_COLOR_SKYBLUE         (0xBC9A5A) /* (188, 154, 90) */
#define OSD_DRAW_YUV_COLOR_INDIGO          (0x25B49B) /* (37, 180, 155) */
#define OSD_DRAW_YUV_COLOR_NAVY            (0x0EC075) /* (14, 192, 117) */
#define OSD_DRAW_YUV_COLOR_MAGENTA         (0x69D4EA) /* (105, 212, 234) */
#define OSD_DRAW_YUV_COLOR_PURPLE          (0x34AAB5) /* (52, 170, 181) */
#define OSD_DRAW_YUV_COLOR_CYAN            (0xB2AB00) /* (178, 171, 0) */
#define OSD_DRAW_YUV_COLOR_TEAL            (0x599540) /* (89, 149, 64) */
#define OSD_DRAW_YUV_COLOR_PINK            (0xD47A9E) /* (212, 122, 158) */
#define OSD_DRAW_YUV_COLOR_DEEPPINK        (0x6898EB) /* (104, 152, 235) */
#define OSD_DRAW_YUV_COLOR_YELLOW          (0xE20095) /* (226, 0, 149) */
#define OSD_DRAW_YUV_COLOR_GREENYELLOW     (0xCE2567) /* (206, 37, 103) */
#define OSD_DRAW_YUV_COLOR_GOLD            (0xCA0DA5) /* (202, 13, 165) */
#define OSD_DRAW_YUV_COLOR_CHOCOLATE       (0x7F48BA) /* (127, 72, 186) */
#define OSD_DRAW_YUV_COLOR_ORANGE          (0xAD1EBA) /* (173, 30, 186) */
/**
  * @}
  */

/** @defgroup OSD_CMD osd feature control cmd definition
  * @{
  */
#define OSD_CMD_BLEND_SET_1_BIT_ALPHA      (0x01)
#define OSD_CMD_BLEND_SET_PRELOAD_TIME     (0x02)
/**
  * @}
  */

/**
 * @brief OSD coordinate structure
 *
 * @param start_x OSD start of coordinate x
 * @param start_y OSD start of coordinate y
 * @param end_x   OSD end of coordinate x, not include
 * @param end_y   OSD end of coordinate y, not include
 */
struct bflb_osd_coordinate_s {
    uint16_t start_x;
    uint16_t start_y;
    uint16_t end_x;
    uint16_t end_y;
};

/**
 * @brief OSD blend configuration structure
 *
 * @param blend_format      OSD blend color format, use @ref OSD_BLEND_FORMAT
 * @param order_a           OSD blend order of A data, range: 0~3, 0 means LSB, 3 means MSB
 * @param order_rv          OSD blend order of R/V data, range: 0~3, 0 means LSB, 3 means MSB
 * @param order_gy          OSD blend order of G/Y data, range: 0~3, 0 means LSB, 3 means MSB
 * @param order_bu          OSD blend order of B/U data, range: 0~3, 0 means LSB, 3 means MSB
 * @param coor              OSD blend coordinate structure
 * @param layer_buffer_addr OSD blend start address of layer buffer
 */
struct bflb_osd_blend_config_s {
    uint8_t blend_format;
    uint8_t order_a;
    uint8_t order_rv;
    uint8_t order_gy;
    uint8_t order_bu;
    struct bflb_osd_coordinate_s coor;
    uint32_t layer_buffer_addr;
};

/**
 * @brief OSD blend replace color value or palette index structure
 *
 * @param replace_between OSD blend replace range, true: when (min_value <= value <= max_value), the value will be replaced with target_value
 *                        false: when (value < min_value || value > max_value), the value will be replaced with target_value
 * @param target_value    OSD blend target value of color or palette index
 * @param min_value       OSD blend min value of color or palette index
 * @param max_value       OSD blend max value of color or palette index
 */
struct bflb_osd_blend_replace_s {
    bool replace_between;
    uint8_t target_value;
    uint8_t min_value;
    uint8_t max_value;
};

/**
 * @brief OSD draw rectangle configuration structure
 *
 * @param is_solid         OSD draw rectangle type, enable: solid rectangle, disable: hollow rectangle
 * @param border_thickness OSD draw rectangle border line thickness, should be even
 * @param color_y          OSD draw rectangle color Y
 * @param color_u          OSD draw rectangle color U
 * @param color_v          OSD draw rectangle color V
 * @param coor             OSD draw rectangle coordinate structure
 */
struct bflb_osd_draw_config_s {
    bool is_solid;
    uint8_t border_thickness;
    uint8_t color_y;
    uint8_t color_u;
    uint8_t color_v;
    struct bflb_osd_coordinate_s coor;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize osd blend.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save osd blend config
 */
void bflb_osd_blend_init(struct bflb_device_s *dev, struct bflb_osd_blend_config_s *config);

/**
 * @brief Enable osd blend.
 *
 * @param [in] dev device handle
 */
void bflb_osd_blend_enable(struct bflb_device_s *dev);

/**
 * @brief Disable osd blend.
 *
 * @param [in] dev device handle
 */
void bflb_osd_blend_disable(struct bflb_device_s *dev);

/**
 * @brief Set osd blend coordinate.
 *
 * @param [in] dev device handle
 * @param [in] coor pointer to osd coordinate
 */
void bflb_osd_blend_set_coordinate(struct bflb_device_s *dev, struct bflb_osd_coordinate_s *coor);

/**
 * @brief Set osd blend global alpha.
 *
 * @param [in] dev device handle
 * @param [in] enable enable or disable global alpha
 * @param [in] value value of global alpha
 */
void bflb_osd_blend_set_global_a(struct bflb_device_s *dev, bool enable, uint8_t value);

/**
 * @brief Set osd blend global RGB or YUV.
 *
 * @param [in] dev device handle
 * @param [in] enable enable or disable global alpha
 * @param [in] rv value of global R or V
 * @param [in] gy value of global G or Y
 * @param [in] bu value of global B or U
 */
void bflb_osd_blend_set_global_rgb_yuv(struct bflb_device_s *dev, bool enable, uint8_t rv, uint8_t gy, uint8_t bu);

/**
 * @brief Set osd blend palette index and color.
 *
 * @param [in] dev device handle
 * @param [in] index index of color
 * @param [in] value value of color
 */
void bflb_osd_blend_set_palette_color(struct bflb_device_s *dev, uint8_t index, uint32_t value);

/**
 * @brief Replace osd blend palette index with target index.
 *
 * @param [in] dev device handle
 * @param [in] enable enable or disable replace function
 * @param [in] replace pointer to save osd blend palette index replace config
 */
void bflb_osd_blend_replace_palette_index(struct bflb_device_s *dev, bool enable, struct bflb_osd_blend_replace_s *replace);

/**
 * @brief Replace osd blend color value with target value.
 *
 * @param [in] dev device handle
 * @param [in] enable enable or disable replace function
 * @param [in] replace_a pointer to save osd blend value of alpha replace config
 * @param [in] replace_rv pointer to save osd blend value of R/V replace config
 * @param [in] replace_gy pointer to save osd blend value of G/Y replace config
 * @param [in] replace_bu pointer to save osd blend value of B/U replace config
 * 
 */
void bflb_osd_blend_replace_color_value(struct bflb_device_s *dev, bool enable, struct bflb_osd_blend_replace_s *replace_a,
                                        struct bflb_osd_blend_replace_s *replace_rv, struct bflb_osd_blend_replace_s *replace_gy, struct bflb_osd_blend_replace_s *replace_bu);

/**
 * @brief Initialize osd draw rectangle.
 *
 * @param [in] dev device handle
 * @param [in] layer draw layer, use @ref OSD_DRAW_LAYER
 * @param [in] config pointer to save osd draw config
 */
void bflb_osd_draw_init(struct bflb_device_s *dev, uint8_t layer, struct bflb_osd_draw_config_s *config);

/**
 * @brief Enable osd draw rectangle.
 *
 * @param [in] dev device handle
 * @param [in] layer draw layer, use @ref OSD_DRAW_LAYER
 */
void bflb_osd_draw_enable(struct bflb_device_s *dev, uint8_t layer);

/**
 * @brief Disable osd draw rectangle.
 *
 * @param [in] dev device handle
 * @param [in] layer draw layer, use @ref OSD_DRAW_LAYER
 */
void bflb_osd_draw_disable(struct bflb_device_s *dev, uint8_t layer);

/**
 * @brief Set osd draw rectangle coordinate.
 *
 * @param [in] dev device handle
 * @param [in] layer draw layer, use @ref OSD_DRAW_LAYER
 * @param [in] coor pointer to osd coordinate
 */
void bflb_osd_draw_set_coordinate(struct bflb_device_s *dev, uint8_t layer, struct bflb_osd_coordinate_s *coor);

/**
 * @brief Enable or disable osd interrupt.
 *
 * @param [in] dev device handle
 * @param [in] mask true means disable, false means enable
 */
void bflb_osd_int_mask(struct bflb_device_s *dev, bool mask);

/**
 * @brief Get osd interrupt status.
 *
 * @param [in] dev device handle
 * @return interrupt status
 */
bool bflb_osd_get_intstatus(struct bflb_device_s *dev);

/**
 * @brief Clear osd interrupt status.
 *
 * @param [in] dev device handle
 */
void bflb_osd_int_clear(struct bflb_device_s *dev);

/**
 * @brief Control osd feature.
 *
 * @param [in] dev device handle
 * @param [in] cmd feature command, use @ref OSD_CMD
 * @param [in] arg user data
 * @return A negated errno value on failure.
 */
int bflb_osd_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

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
