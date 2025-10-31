#ifndef _BFLB_MJDEC_V2_H
#define _BFLB_MJDEC_V2_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup MJDEC
  * @{
  */

/** @defgroup MJDEC_FORMAT mjdec format definition
  * @{
  */
#define MJDEC_FORMAT_GRAY         (0)
#define MJDEC_FORMAT_YUYV         (1)
#define MJDEC_FORMAT_YVYU         (2)
#define MJDEC_FORMAT_UYVY         (3)
#define MJDEC_FORMAT_VYUY         (4)
#define MJDEC_FORMAT_NV16_OR_NV12 (5)
#define MJDEC_FORMAT_NV61_OR_NV21 (6)
#define MJDEC_FORMAT_RGB888       (7)
#define MJDEC_FORMAT_RGB565       (8)
#define MJDEC_FORMAT_NRGB8888     (9)
#define MJDEC_FORMAT_BGR888       (10)
#define MJDEC_FORMAT_BGR565       (11)
#define MJDEC_FORMAT_NBGR8888     (12)
/**
  * @}
  */

/** @defgroup MJDEC_INT mjdec interrupt definition
  * @{
  */
#define MJDEC_INT_ONE_FRAME     (1 << 0)
#define MJDEC_INT_ROW_COMPLETE  (1 << 1)
#define MJDEC_INT_BUS_ERROR     (1 << 2)
#define MJDEC_INT_PARSE_ERROR   (1 << 3)
#define MJDEC_INT_DHT_ERROR     (1 << 4)
#define MJDEC_INT_ERROR         (0x7 << 2)
#define MJDEC_INT_ALL           (0x1F << 0)
/**
  * @}
  */

// clang-format off
#define IS_MJDEC_FORMAT(type) (((type) == MJDEC_FORMAT_GRAY)         || \
                               ((type) == MJDEC_FORMAT_YUYV)         || \
                               ((type) == MJDEC_FORMAT_YVYU)         || \
                               ((type) == MJDEC_FORMAT_UYVY)         || \
                               ((type) == MJDEC_FORMAT_VYUY)         || \
                               ((type) == MJDEC_FORMAT_NV16_OR_NV12) || \
                               ((type) == MJDEC_FORMAT_NV61_OR_NV21) || \
                               ((type) == MJDEC_FORMAT_RGB888)       || \
                               ((type) == MJDEC_FORMAT_RGB565)       || \
                               ((type) == MJDEC_FORMAT_NRGB8888)     || \
                               ((type) == MJDEC_FORMAT_BGR888)       || \
                               ((type) == MJDEC_FORMAT_BGR565)       || \
                               ((type) == MJDEC_FORMAT_NBGR8888))

#define IS_MJDEC_ADDR(type) (!((type) % 8))
// clang-format on

/**
 * @brief MJDEC configuration structure
 *
 * @param format           MJDEC format, use @ref MJDEC_FORMAT
 * @param alpha            MJDEC alpha value for NRGB8888/NBGR8888
 * @param has_header       MJDEC jpeg picture has or not has header
 * @param parse_header     MJDEC decoder parse or not parse header
 * @param input_bufaddr    MJDEC input buffer address, must be align 8
 * @param output_bufaddr0  MJDEC output buffer 0 address which is for YY component, must be align 8
 * @param output_bufaddr1  MJDEC output buffer 1 address which is for UV component, must be align 8
 * @param row_of_interrupt MJDEC row number of MJDEC_INT_ROW_COMPLETE interrupt
 * @param row_of_pause     MJDEC row number for pause decoder
 * @param row_of_addr_loop MJDEC row number for output address loop
 */
struct bflb_mjdec_config_s {
    uint8_t format;
    uint8_t alpha;
    uint8_t has_header;
    uint8_t parse_header;
    uint32_t input_bufaddr;
    uint32_t output_bufaddr0;
    uint32_t output_bufaddr1;
    uint16_t row_of_interrupt;
    uint16_t row_of_pause;
    uint16_t row_of_addr_loop;
};

/**
 * @brief MJDEC parse structure
 *
 * @param resolution_x     MJDEC width resolution
 * @param resolution_y     MJDEC height resolution
 * @param header_length    MJDEC jpeg header length
 * @param samp_yy_horizon  MJDEC componen yy horizon sample coefficient
 * @param samp_yy_vertical MJDEC componen yy vertical sample coefficient
 * @param samp_cb_horizon  MJDEC componen Cb horizon sample coefficient
 * @param samp_cb_vertical MJDEC componen Cb vertical sample coefficient
 * @param samp_cr_horizon  MJDEC componen Cr horizon sample coefficient
 * @param samp_cr_vertical MJDEC componen Cr vertical sample coefficient
 */
struct bflb_mjdec_parse_s {
    uint16_t resolution_x;
    uint16_t resolution_y;
    uint16_t header_length;
    uint8_t samp_yy_horizon;
    uint8_t samp_yy_vertical;
    uint8_t samp_cb_horizon;
    uint8_t samp_cb_vertical;
    uint8_t samp_cr_horizon;
    uint8_t samp_cr_vertical;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize mjdec.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save mjdec config
 */
void bflb_mjdec_init(struct bflb_device_s *dev, const struct bflb_mjdec_config_s *config);

/**
 * @brief Deinitialize mjdec.
 *
 * @param [in] dev device handle
 */
void bflb_mjdec_deinit(struct bflb_device_s *dev);

/**
 * @brief Start mjdec dec-compression
 *
 * @param [in] dev device handle
 */
void bflb_mjdec_start(struct bflb_device_s *dev);

/**
 * @brief Get mjdec header parse result
 *
 * @param [in] dev device handle
 * @param [in] parse pointer to save mjdec header parse result
 */
void bflb_mjdec_get_parse_result(struct bflb_device_s *dev, struct bflb_mjdec_parse_s *parse);

/**
 * @brief Set mjdec row count to pause
 *
 * @param [in] dev device handle
 * @param [in] row row count
 */
void bflb_mjdec_set_row_count_pause(struct bflb_device_s *dev, uint16_t row);

/**
 * @brief Get mjdec row count of pause
 *
 * @param [in] dev device handle
 * @return row count
 */
uint16_t bflb_mjdec_get_row_count_pause(struct bflb_device_s *dev);

/**
 * @brief Enable or disable mjdec interrupt.
 *
 * @param [in] dev device handle
 * @param [in] int_en interrupt type, use @ref MJDEC_INT
 * @param [in] enable true means enable, otherwise disable
 */
void bflb_mjdec_int_enable(struct bflb_device_s *dev, uint32_t int_en, bool enable);

/**
 * @brief Get mjdec interrupt status.
 *
 * @param [in] dev device handle
 * @return interrupt status, use @ref MJDEC_INT
 */
uint32_t bflb_mjdec_get_intstatus(struct bflb_device_s *dev);

/**
 * @brief Clear mjdec interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_clear clear value, use @ref MJDEC_INT
 */
void bflb_mjdec_int_clear(struct bflb_device_s *dev, uint32_t int_clear);

/**
 * @brief Control mjdec feature.
 *
 * @param [in] dev device handle
 * @param [in] cmd feature command
 * @param [in] arg user data
 * @return A negated errno value on failure.
 */
int bflb_mjdec_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

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