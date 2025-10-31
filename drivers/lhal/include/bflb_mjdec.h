#ifndef _BFLB_MJDEC_H
#define _BFLB_MJDEC_H

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
#define MJDEC_FORMAT_YUV422SP_NV16 (4)
#define MJDEC_FORMAT_YUV422SP_NV61 (5)
#define MJDEC_FORMAT_YUV420SP_NV12 (6)
#define MJDEC_FORMAT_YUV420SP_NV21 (7)
#define MJDEC_FORMAT_GRAY          (8)
/**
  * @}
  */

/** @defgroup MJDEC_ERROR definition
  * @{
  */
#define MJDEC_OK                  (0)
#define MJDEC_ERR_POINTER_NULL    (1)
#define MJDEC_ERR_HEADER_SOI      (2)
#define MJDEC_ERR_HEADER_MARKER   (3)
#define MJDEC_ERR_DQT_TYPE        (4)
#define MJDEC_ERR_DQT_LENGTH      (5)
#define MJDEC_ERR_HEADER_TIMEOUT  (6)
#define MJDEC_ERR_DHT_TRIG        (20)
#define MJDEC_ERR_DHT_TYPE        (21)
#define MJDEC_ERR_DHT_COUNT       (22)
#define MJDEC_ERR_DHT_YY_DC_CNT   (23)
#define MJDEC_ERR_DHT_YY_DC_VAL   (24)
#define MJDEC_ERR_DHT_YY_DC_BYTES (25)
#define MJDEC_ERR_DHT_YY_AC_BYTES (26)
#define MJDEC_ERR_DHT_UV_DC_CNT   (27)
#define MJDEC_ERR_DHT_UV_DC_VAL   (28)
#define MJDEC_ERR_DHT_UV_DC_BYTES (29)
#define MJDEC_ERR_DHT_UV_AC_BYTES (30)
/**
  * @}
  */

/** @defgroup MJDEC_INTSTS mjdec interrupt status definition
  * @{
  */
#define MJDEC_INTSTS_ONE_FRAME     (1 << 4)
/**
  * @}
  */

/** @defgroup MJDEC_INTCLR mjdec interrupt clear definition
  * @{
  */
#define MJDEC_INTCLR_ONE_FRAME     (1 << 0)
/**
  * @}
  */

/**
  * @}
  */

// clang-format off
#define IS_MJDEC_FORMAT(type)   ((type) >= MJDEC_FORMAT_YUV422SP_NV16) && ((type) <= MJDEC_FORMAT_GRAY)

#define IS_MJDEC_RESOLUTION(type)   (!((type) % 8))

#if defined(BL808)
#define IS_MJDEC_QUALITY(type)   ((((type) > 0) && ((type) <= 75)) || ((type) == 100))
#endif

#define IS_MJDEC_ADDR(type)   (!((type) % 8))

// clang-format on

/**
 * @brief MJDEC configuration structure
 *
 * @param format            MJDEC format, use @ref MJDEC_FORMAT
 * @param swap_enable       MJDEC output is pingpang buffer(swap_enable = 1) or consecutive growth(swap_enable = 0)
 * @param resolution_x      MJDEC width, must be a multiple of 8
 * @param resolution_y      MJDEC higth, must be a multiple of 8
 * @param head_size         MJDEC stream head bytes
 * @param output_bufaddr0   MJDEC output buffer 0 address which is for YY component, must be align 8
 * @param output_bufaddr1   MJDEC output buffer 1 address which is for UV component, must be align 8
 */
struct bflb_mjdec_config_s {
    uint8_t format;
    uint8_t swap_enable;
    uint16_t resolution_x;
    uint16_t resolution_y;
    uint16_t head_size;
    uint32_t output_bufaddr0;
    uint32_t output_bufaddr1;
};

#if defined(BL616D)
/**
 * @brief MJDEC DHT configuration structure
 *
 * @param yy_dc_cnt   MJDEC YY DC component bit count pointer
 * @param yy_dc_val   MJDEC YY DC component value pointer
 * @param yy_ac_cnt   MJDEC YY AC component bit count pointer
 * @param yy_ac_val   MJDEC YY AC component value pointer
 * @param uv_dc_cnt   MJDEC UV DC component bit count pointer
 * @param uv_dc_val   MJDEC UV DC component value pointer
 * @param uv_ac_cnt   MJDEC UV AC component bit count pointer
 * @param uv_ac_val   MJDEC UV AC component value pointer
 * @param yy_dc_bytes MJDEC YY DC component value bytes
 * @param yy_ac_bytes MJDEC YY AC component value bytes
 * @param uv_dc_bytes MJDEC UV DC component value bytes
 * @param uv_ac_bytes MJDEC UV AC component value bytes
 */
struct bflb_mjdec_dht_s {
    uint8_t *yy_dc_cnt;
    uint8_t *yy_dc_val;
    uint8_t *yy_ac_cnt;
    uint8_t *yy_ac_val;
    uint8_t *uv_dc_cnt;
    uint8_t *uv_dc_val;
    uint8_t *uv_ac_cnt;
    uint8_t *uv_ac_val;
    uint16_t yy_dc_bytes;
    uint16_t yy_ac_bytes; 
    uint16_t uv_dc_bytes;
    uint16_t uv_ac_bytes;
};

#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BL616D)
/**
 * @brief Set DQT from JPEG specification 
 *
 * @param [in] dev device handle
 * @param [in] yy pointer of YY component DQT
 * @param [in] uv pointer of UV component DQT
 */
void bflb_mjdec_set_dqt_from_spec(struct bflb_device_s *dev, uint8_t (*yy)[8], uint8_t (*uv)[8]);

/**
 * @brief Set DQT from ZIGZAG
 *
 * @param [in] dev device handle
 * @param [in] yy pointer of YY component DQT ordered by zig_zag
 * @param [in] uv pointer of UV component DQT ordered by zig_zag
 */
void bflb_mjdec_set_dqt_from_zigzag(struct bflb_device_s *dev, uint8_t *yy, uint8_t *uv);

/**
 * @brief Set DQT from JPEG header
 *
 * @param [in] dev device handle
 * @param [in] header pointer of JPEG header
 * @return error status
 */
uint8_t bflb_mjdec_set_dqt_from_header(struct bflb_device_s *dev, uint8_t *header);

/**
 * @brief Set DHT from JPEG specification 
 *
 * @param [in] dev device handle
 * @param [in] pointer of DHT
 * @return error status
 */
uint8_t bflb_mjdec_set_dht_from_spec(struct bflb_device_s *dev, struct bflb_mjdec_dht_s *dht);

/**
 * @brief Set DHT from JPEG header
 *
 * @param [in] dev device handle
 * @param [in] header pointer of JPEG header
 * @return error status
 */
uint8_t bflb_mjdec_set_dht_from_header(struct bflb_device_s *dev, uint8_t *header);
#endif

/**
 * @brief Set DQT from quality value
 *
 * @param [in] dev device handle
 * @param [in] quality JPEG quality
 */
void bflb_mjdec_set_dqt_from_quality(struct bflb_device_s *dev, uint8_t quality);

/**
 * @brief Initialize mjdec.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save mjdec config
 */
void bflb_mjdec_init(struct bflb_device_s *dev, const struct bflb_mjdec_config_s *config);

/**
 * @brief Start mjdec dec-compression
 *
 * @param [in] dev device handle
 */
void bflb_mjdec_start(struct bflb_device_s *dev);

/**
 * @brief Stop mjdec dec-compression
 *
 * @param [in] dev device handle
 */
void bflb_mjdec_stop(struct bflb_device_s *dev);

/**
 * @brief Push one frame of jpeg to mjdec for dec-compression
 *
 * @param [in] dev device handle
 * @param [in] frame jpeg frame start address, must be align 8 bytes
 */
void bflb_mjdec_push_jpeg(struct bflb_device_s *dev, void *frame);

/**
 * @brief Enable or disable mjdec one frame dec-compression completion interrupt.
 *
 * @param [in] dev device handle
 * @param [in] mask true means disable, false means enable
 */
void bflb_mjdec_tcint_mask(struct bflb_device_s *dev, bool mask);

/**
 * @brief Get mjdec interrupt status.
 *
 * @param [in] dev device handle
 * @return interrupt status
 */
uint32_t bflb_mjdec_get_intstatus(struct bflb_device_s *dev);

/**
 * @brief Clear mjdec interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] int_clear clear value
 */
void bflb_mjdec_int_clear(struct bflb_device_s *dev, uint32_t int_clear);

/**
 * @brief Get number of frame count that has dec-compressed.
 *
 * @param [in] dev device handle
 * @return dec-compressed frame count
 */
uint8_t bflb_mjdec_get_frame_count(struct bflb_device_s *dev);

/**
 * @brief Drop one frame that has dec-compressed.
 *
 * @param [in] dev device handle
 */
void bflb_mjdec_pop_one_frame(struct bflb_device_s *dev);

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