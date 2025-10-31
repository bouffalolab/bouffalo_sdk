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
#define DPI_INPUT_SEL_TEST_PATTERN_WITHOUT_OSD 0
#define DPI_INPUT_SEL_TEST_PATTERN_WITH_OSD    1
#define DPI_INPUT_SEL_FRAMEBUFFER_WITHOUT_OSD  2
#define DPI_INPUT_SEL_FRAMEUFFER_WITH_OSD      3

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
#define DPI_DATA_FORMAT_UYVY                   4 /* [31:24]=Y, [23:16]=V, [15:8]=Y, [7:0]=U */
#define DPI_DATA_FORMAT_BGR888                 5 /* [31:24]=R, [23:16]=B, [15:8]=G, [7:0]=R */
#define DPI_DATA_FORMAT_BGR565                 6 /* [31:27]=B, [26:21]=G, [20:16]=R, [15:11]=B, [10:5]=G, [4:0]=R */
#define DPI_DATA_FORMAT_NBGR8888               7 /* [31:24]=N, [23:16]=B, [15:8]=G, [7:0]=R */
#define DPI_DATA_FORMAT_TEST_PATTERN           8
/**
  * @}
  */

/** @defgroup DPI_CMD dpi feature control cmd definition
  * @{
  */

/**
  * @}
  */

/**
 * @brief DPI configuration structure
 *
 * @param width              DPI active Width
 * @param height             DPI active height
 * @param hsw                DPI horizontal synchronization width
 * @param hbp                DPI horizontal back porch
 * @param hfp                DPI horizontal front porch
 * @param vsw                DPI vertical synchronization width
 * @param vbp                DPI vertical back porch
 * @param vfp                DPI vertical front porch
 * @param interface          DPI interface color coding, use @ref DPI_INTERFACE
 * @param input_sel          DPI input selection, use @ref DPI_INPUT
 * @param test_pattern       DPI test pattern color, use @ref DPI_TEST
 * @param data_format        DPI data format, use @ref DPI_DATA
 * @param framebuffer_addr   DPI start address of framebuffer
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
