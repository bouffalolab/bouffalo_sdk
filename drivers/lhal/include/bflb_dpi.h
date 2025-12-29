#ifndef _BFLB_DPI_H
#define _BFLB_DPI_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup DPI
  * @{
  */

/** @defgroup DPI_INPUT dpi input selection definition
  * @{
  */
#define DPI_INPUT_SEL_TEST_PATTERN_WITHOUT_OSD 0 /* only support test pattern data format input */
#define DPI_INPUT_SEL_TEST_PATTERN_WITH_OSD    1 /* only support test pattern data format input */
#define DPI_INPUT_SEL_FRAMEBUFFER_WITHOUT_OSD  2 /* only support rgb data format input, not support yuv data format */
#define DPI_INPUT_SEL_FRAMEBUFFER_WITH_OSD     3 /* support rgb and yuv data format input */

/**
  * @}
  */

/** @defgroup DPI_INTERFACE dpi interface color coding definition
  * @{
  */
#define DPI_INTERFACE_24_PIN                   0 /* D0 ~ D23 */
#define DPI_INTERFACE_18_PIN_MODE_1            1 /* D0 ~ D17 */
#define DPI_INTERFACE_18_PIN_MODE_2            2 /* D0 ~ D5, D8 ~ D13, D16 ~ D21 */
#define DPI_INTERFACE_16_PIN_MODE_1            3 /* D0 ~ D15 */
#define DPI_INTERFACE_16_PIN_MODE_2            4 /* D0 ~ D4, D8 ~ D13, D16 ~ D20 */
#define DPI_INTERFACE_16_PIN_MODE_3            5 /* D1 ~ D5, D8 ~ D13, D17 ~ D21 */
/**
  * @}
  */

/** @defgroup DPI_TEST dpi test pattern color definition
  * @{
  */
#define DPI_TEST_PATTERN_NULL                  0
#define DPI_TEST_PATTERN_BLACK                 1
#define DPI_TEST_PATTERN_RED                   2
#define DPI_TEST_PATTERN_GREE                  3
#define DPI_TEST_PATTERN_YELLOW                4
/**
  * @}
  */

/** @defgroup DPI_DATA dpi data format definition
  * @{
  */
#define DPI_DATA_FORMAT_YUYV                   0 /* [31:24]=V, [23:16]=Y, [15:8]=U, [7:0]=Y */
#define DPI_DATA_FORMAT_RGB888                 1 /* [31:24]=B, [23:16]=R, [15:8]=G, [7:0]=B */
#define DPI_DATA_FORMAT_RGB565                 2 /* [31:27]=R, [26:21]=G, [20:16]=B, [15:11]=R, [10:5]=G, [4:0]=B */
#define DPI_DATA_FORMAT_NRGB8888               3 /* [31:24]=N, [23:16]=R, [15:8]=G, [7:0]=B */
#define DPI_DATA_FORMAT_Y_UV_PLANAR            4 /* Y and UV components are stored separately in two memory blocks */
#define DPI_DATA_FORMAT_UYVY                   5 /* [31:24]=Y, [23:16]=V, [15:8]=Y, [7:0]=U */
#define DPI_DATA_FORMAT_BGR888                 6 /* [31:24]=R, [23:16]=B, [15:8]=G, [7:0]=R */
#define DPI_DATA_FORMAT_BGR565                 7 /* [31:27]=B, [26:21]=G, [20:16]=R, [15:11]=B, [10:5]=G, [4:0]=R */
#define DPI_DATA_FORMAT_NBGR8888               8 /* [31:24]=N, [23:16]=B, [15:8]=G, [7:0]=R */
#define DPI_DATA_FORMAT_TEST_PATTERN           10
/**
  * @}
  */

/** @defgroup DPI_YUV420 dpi yuv420 input mode uv valid line definition
  * @{
  */
#define DPI_YUV420_UV_VALID_LINE_EVEN          0 /* 0/2/4/6/8... lines are valid */
#define DPI_YUV420_UV_VALID_LINE_ODD           1 /* 1/3/5/7/9... lines are valid */
/**
  * @}
  */

/** @defgroup DPI_BURST dpi burst length definition
  * @{
  */
#define DPI_BURST_INCR1                        0
#define DPI_BURST_INCR4                        1
#define DPI_BURST_INCR8                        2
#define DPI_BURST_INCR16                       3
#define DPI_BURST_INCR32                       5
#define DPI_BURST_INCR64                       6
/**
  * @}
  */

/** @defgroup DPI_CMD dpi feature control cmd definition
  * @{
  */
#define DPI_CMD_SET_YUV420_UV_VALID_LINE       (0x01)
#define DPI_CMD_SET_BURST                      (0x02)
/**
  * @}
  */

/**
 * @brief DPI configuration structure
 *
 * @param width               DPI active Width
 * @param height              DPI active height
 * @param hsw                 DPI horizontal synchronization width
 * @param hbp                 DPI horizontal back porch
 * @param hfp                 DPI horizontal front porch
 * @param vsw                 DPI vertical synchronization width
 * @param vbp                 DPI vertical back porch
 * @param vfp                 DPI vertical front porch
 * @param interface           DPI interface color coding, use @ref DPI_INTERFACE
 * @param input_sel           DPI input selection, use @ref DPI_INPUT
 * @param test_pattern        DPI test pattern color, use @ref DPI_TEST
 * @param data_format         DPI data format, use @ref DPI_DATA
 * @param framebuffer_addr    DPI start address of framebuffer, also start address of Y framebuffer for DPI_DATA_FORMAT_Y_UV_PLANAR
 * @param uv_framebuffer_addr DPI start address of UV framebuffer, only for DPI_DATA_FORMAT_Y_UV_PLANAR
 */
struct bflb_dpi_config_s {
    uint16_t width;
    uint16_t height;
    uint8_t hsw;
    uint16_t hbp;
    uint8_t hfp;
    uint8_t vsw;
    uint16_t vbp;
    uint8_t vfp;
    uint8_t interface;
    uint8_t input_sel;
    uint8_t test_pattern;
    uint8_t data_format;
    uint32_t framebuffer_addr;
    uint32_t uv_framebuffer_addr;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize dpi.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save dpi config
 */
void bflb_dpi_init(struct bflb_device_s *dev, const struct bflb_dpi_config_s *config);

/**
 * @brief Enable dpi.
 *
 * @param [in] dev device handle
 */
void bflb_dpi_enable(struct bflb_device_s *dev);

/**
 * @brief Disable dpi.
 *
 * @param [in] dev device handle
 */
void bflb_dpi_disable(struct bflb_device_s *dev);

/**
 * @brief Switch framebuffer.
 *
 * @param [in] dev device handle
 * @param [in] addr start address of framebuffer to switch
 */
void bflb_dpi_framebuffer_switch(struct bflb_device_s *dev, uint32_t addr);

/**
 * @brief Switch Y and UV framebuffer in DPI_DATA_FORMAT_Y_UV_PLANAR mode.
 *
 * @param [in] dev device handle
 * @param [in] y_addr start address of Y framebuffer to switch
 * @param [in] uv_addr start address of UV framebuffer to switch
 */
void bflb_dpi_framebuffer_planar_switch(struct bflb_device_s *dev, uint32_t y_addr, uint32_t uv_addr);

/**
 * @brief Get start address of the framebuffer which is using.
 *
 * @param [in] dev device handle
 * @return Start address of the framebuffer.
 */
uint32_t bflb_dpi_get_framebuffer_using(struct bflb_device_s *dev);

/**
 * @brief Set dpi test pattern custom color.
 *
 * @param [in] dev device handle
 * @param [in] max max value of color
 * @param [in] value start value of color
 * @param [in] step step of color
 */
void bflb_dpi_set_test_pattern_custom(struct bflb_device_s *dev, uint16_t max, uint16_t value, uint8_t step);

/**
 * @brief Control dpi feature.
 *
 * @param [in] dev device handle
 * @param [in] cmd feature command, use @ref DPI_CMD
 * @param [in] arg user data
 * @return A negated errno value on failure.
 */
int bflb_dpi_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

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
