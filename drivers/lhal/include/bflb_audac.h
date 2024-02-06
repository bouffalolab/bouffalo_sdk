#ifndef _BFLB_AUDAC_H
#define _BFLB_AUDAC_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup AUDAC
  * @{
  */

/**
 *  @defgroup AUDAC_SAMPLING_RATE audac sampling rate definition
 * @{
 */
#define AUDAC_SAMPLING_RATE_8K                      0
#define AUDAC_SAMPLING_RATE_16K                     1
#define AUDAC_SAMPLING_RATE_22P05K                  5
#define AUDAC_SAMPLING_RATE_24K                     3
#define AUDAC_SAMPLING_RATE_32K                     2
#define AUDAC_SAMPLING_RATE_44P1K                   6
#define AUDAC_SAMPLING_RATE_48K                     4
/**
  * @}
  */

/**
 *  @defgroup AUDAC_OUTPUT_MODE audac output mode definition
 * @{
 */
#define AUDAC_OUTPUT_MODE_PWM                       0
#define AUDAC_OUTPUT_MODE_GPDAC_CH_A                1
#define AUDAC_OUTPUT_MODE_GPDAC_CH_B                2
#define AUDAC_OUTPUT_MODE_GPDAC_CH_A_B              3
/**
  * @}
  */

/**
 *  @defgroup AUDAC_SOURCE_CHANNEL audac channel mode definition
 * @{
 */
#define AUDAC_SOURCE_CHANNEL_SINGLE                 0x01
#define AUDAC_SOURCE_CHANNEL_DUAL                   0x03
/**
  * @}
  */

/**
 *  @defgroup AUDAC_MIXER_MODE audac mixer mode definition
 *  this parameter is valid only in AUDAC_SOURCE_CHANNEL_DUAL mode
 * @{
 */
#define AUDAC_MIXER_MODE_ONLY_L                     0
#define AUDAC_MIXER_MODE_ONLY_R                     1
#define AUDAC_MIXER_MODE_SUM                        2
#define AUDAC_MIXER_MODE_AVERAGE                    3
/**
  * @}
  */

/**
 *  @defgroup AUDAC_DATA_FORMAT audac data format definition
 * @{
 */
#define AUDAC_DATA_FORMAT_16BIT                     3
#define AUDAC_DATA_FORMAT_20BIT                     2
#define AUDAC_DATA_FORMAT_24BIT                     1
#define AUDAC_DATA_FORMAT_32BIT                     0
/**
  * @}
  */

/**
 *  @defgroup AUDAC_RAMP_RATE audac volume ramp rate definition
 * @{
 */
#define AUDAC_RAMP_RATE_FS_2                        0
#define AUDAC_RAMP_RATE_FS_4                        1
#define AUDAC_RAMP_RATE_FS_8                        2
#define AUDAC_RAMP_RATE_FS_16                       3
#define AUDAC_RAMP_RATE_FS_32                       4
#define AUDAC_RAMP_RATE_FS_64                       5
#define AUDAC_RAMP_RATE_FS_128                      6
#define AUDAC_RAMP_RATE_FS_256                      7
#define AUDAC_RAMP_RATE_FS_512                      8
#define AUDAC_RAMP_RATE_FS_1024                     9
#define AUDAC_RAMP_RATE_FS_2048                     10
/**
  * @}
  */

/**
 *  @defgroup AUDAC_VOLUME_UPDATE_MODE audac volume update mode definition
 * @{
 */
#define AUDAC_VOLUME_UPDATE_MODE_FORCE              0
#define AUDAC_VOLUME_UPDATE_MODE_RAMP               1
#define AUDAC_VOLUME_UPDATE_MODE_RAMP_ZERO_CROSSING 2
/**
  * @}
  */

/** @defgroup AUDAC_INTSTS audac interrupt status definition
 * @{
  * @{
  */
#define AUDAC_INTSTS_VOLUME_RAMP                    (1 << 0)
#define AUDAC_INTSTS_FIFO_OVER                      (1 << 1)
#define AUDAC_INTSTS_FIFO_UNDER                     (1 << 2)
#define AUDAC_INTSTS_FIFO_AVAILABLE                 (1 << 3)

/** @defgroup AUDAC_CMD audac feature control cmd definition
  * @{
  */
#define AUDAC_CMD_PLAY_START                        (0x01)
#define AUDAC_CMD_PLAY_STOP                         (0x02)
#define AUDAC_CMD_SET_MUTE                          (0x03)
#define AUDAC_CMD_SET_VOLUME_VAL                    (0x04)
#define AUDAC_CMD_CLEAR_TX_FIFO                     (0x05)
#define AUDAC_CMD_GET_TX_FIFO_CNT                   (0x06)

/**
  * @}
  */

// clang-format off
#define IS_AUDAC_SAMPLING_RATE(type)   (((type) == AUDAC_SAMPLING_RATE_8K) || \
                                  ((type) == AUDAC_SAMPLING_RATE_16K) || \
                                  ((type) == AUDAC_SAMPLING_RATE_22P05K) || \
                                  ((type) == AUDAC_SAMPLING_RATE_24K) || \
                                  ((type) == AUDAC_SAMPLING_RATE_32K) || \
                                  ((type) == AUDAC_SAMPLING_RATE_44P1K) || \
                                  ((type) == AUDAC_SAMPLING_RATE_48K))

#define IS_AUDAC_OUTPUT_MODE(type)  (((type) == AUDAC_OUTPUT_MODE_PWM) || \
                                  ((type) == AUDAC_OUTPUT_MODE_GPDAC_CH_A) || \
                                  ((type) == AUDAC_OUTPUT_MODE_GPDAC_CH_B) || \
                                  ((type) == AUDAC_OUTPUT_MODE_GPDAC_CH_A_B))

#define IS_AUDAC_SOURCE_CHANNEL(type)  (((type) == AUDAC_SOURCE_CHANNEL_SINGLE) || \
                                  ((type) == AUDAC_SOURCE_CHANNEL_DUAL))

#define IS_AUDAC_MIXER_MODE(type)  (((type) == AUDAC_MIXER_MODE_ONLY_L) || \
                                  ((type) == AUDAC_MIXER_MODE_ONLY_R) || \
                                  ((type) == AUDAC_MIXER_MODE_SUM) || \
                                  ((type) == AUDAC_MIXER_MODE_AVERAGE))

#define IS_AUDAC_DATA_FORMAT(type)  (((type) == AUDAC_DATA_FORMAT_16BIT) || \
                                  ((type) == AUDAC_DATA_FORMAT_20BIT) || \
                                  ((type) == AUDAC_DATA_FORMAT_24BIT) || \
                                  ((type) == AUDAC_DATA_FORMAT_32BIT))

#define IS_AUDAC_RAMP_RATE(type)  ((type) <= AUDAC_RAMP_RATE_FS_2048)

#define IS_AUDAC_VOLUME_UPDATE_MODE(type)  (((type) == AUDAC_VOLUME_UPDATE_MODE_FORCE) || \
                                  ((type) == AUDAC_VOLUME_UPDATE_MODE_RAMP) || \
                                  ((type) == AUDAC_VOLUME_UPDATE_MODE_RAMP_ZERO_CROSSING))

#define IS_AUDAC_THRESHOLD(type)      ((type) <= 7)

// clang-format on

/**
 * @brief AUDAC initialization configuration structure
 *
 * @param sampling_rate         AUDAC sampling rate, use @ref AUDAC_SAMPLING_RATE
 * @param output_mode           AUDAC mode, use @ref AUDAC_OUTPUT_MODE
 * @param source_channels_num   AUDAC source channels num, use @ref AUDAC_SOURCE_CHANNEL
 * @param mixer_mode            AUDAC mixer mode, valid only in AUDAC_SOURCE_CHANNEL_DUAL mode, use @ref AUDAC_MIXER_MODE
 * @param data_format           AUDAC audac data format, use @ref AUDAC_DATA_FORMAT
 * @param fifo_threshold        AUDAC tx fifo threshold, 0 ~ 7
 * @param dma_enable            AUDAC dma mode enable, use true or false
 */
struct bflb_audac_init_config_s {
    uint8_t sampling_rate;
    uint8_t output_mode;
    uint8_t source_channels_num;
    uint8_t mixer_mode;
    uint8_t data_format;
    uint8_t fifo_threshold;
};

/**
 * @brief AUDAC volume configuration structure
 *
 * @param mute_ramp_en              AUDAC mute mode en, use true or false
 * @param mute_up_ramp_rate         AUDAC mute up ramp rate, valid when mute_ramp_en is true, use @ref AUDAC_RAMP_RATE
 * @param mute_down_ramp_rate       AUDAC mute down ramp rate, valid when mute_ramp_en is true, use @ref AUDAC_RAMP_RATE
 * @param volume_update_mode        AUDAC volume update mode, use @ref AUDAC_VOLUME_UPDATE_MODE
 * @param volume_ramp_rate          AUDAC volume ramp rate, valid when volume_update_mode is not AUDAC_VOLUME_UPDATE_MODE_FORCE, use @ref AUDAC_RAMP_RATE
 * @param volume_zero_cross_timeout AUDAC volume update zero cross timeout period, valid only in AUDAC_VOLUME_UPDATE_MODE_RAMP_ZERO_CROSSING mode
 */
struct bflb_audac_volume_config_s {
    bool mute_ramp_en;
    uint8_t mute_up_ramp_rate;
    uint8_t mute_down_ramp_rate;
    uint8_t volume_update_mode;
    uint8_t volume_ramp_rate;
    uint8_t volume_zero_cross_timeout;
};

#ifdef __cplusplus
extern "C" {
#endif

int bflb_audac_init(struct bflb_device_s *dev, const struct bflb_audac_init_config_s *config);

int bflb_audac_volume_init(struct bflb_device_s *dev, const struct bflb_audac_volume_config_s *vol_cfg);

int bflb_audac_link_rxdma(struct bflb_device_s *dev, bool enable);

int bflb_audac_int_mask(struct bflb_device_s *dev, uint32_t mask);

int bflb_audac_get_intstatus(struct bflb_device_s *dev);

int bflb_audac_int_clear(struct bflb_device_s *dev, uint32_t int_clear);

int bflb_audac_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

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