#ifndef _BFLB_CANFD_H
#define _BFLB_CANFD_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup CANFD
  * @{
  */

/** @defgroup CANFD_MODE canfd mode definition
  * @{
  */
#define CANFD_MODE_NORMAL            (0)
#define CANFD_MODE_LISTEN_ONLY       (1)
#define CANFD_MODE_INTERNAL_LOOPBACK (2)
#define CANFD_MODE_EXTERNAL_LOOPBACK (3)
/**
  * @}
  */

/** @defgroup CANFD_ID canfd identifier type definition
  * @{
  */
#define CANFD_ID_STANDARD (0)
#define CANFD_ID_EXTENDED (1)
/**
  * @}
  */

/** @defgroup CANFD_FRAME canfd frame type definition
  * @{
  */
#define CANFD_FRAME_DATA   (0)
#define CANFD_FRAME_REMOTE (1)
/**
  * @}
  */

/** @defgroup CANFD_ESI canfd error state indicator definition
  * @{
  */
#define CANFD_ESI_ACTIVE  (0)
#define CANFD_ESI_PASSIVE (1)
/**
  * @}
  */

/** @defgroup CANFD_BRS canfd bit rate switching definition
  * @{
  */
#define CANFD_BRS_OFF (0)
#define CANFD_BRS_ON  (1)
/**
  * @}
  */

/** @defgroup CANFD_FORMAT canfd format type definition
  * @{
  */
#define CANFD_FORMAT_CLASSIC_CAN (0)
#define CANFD_FORMAT_FD_CAN      (1)
/**
  * @}
  */

/** @defgroup CANFD_TX_CMD canfd tx command definition
  * @{
  */
#define CANFD_TX_CMD_STB_ABORT     (0)
#define CANFD_TX_CMD_STB_ALL_START (1)
#define CANFD_TX_CMD_STB_ONE_START (2)
#define CANFD_TX_CMD_PTB_ABORT     (3)
#define CANFD_TX_CMD_PTB_START     (4)
/**
  * @}
  */

/** @defgroup CANFD_STB_STATE canfd secondary transmit buffer state definition
  * @{
  */
#define CANFD_STB_STATE_EMPTY           (0)
#define CANFD_STB_STATE_LESS_EQUAL_HALF (1)
#define CANFD_STB_STATE_MORE_HALF       (2)
#define CANFD_STB_STATE_FULL            (3)
/**
  * @}
  */

/** @defgroup CANFD_RB_STATE canfd receive buffer state definition
  * @{
  */
#define CANFD_RB_STATE_EMPTY                (0)
#define CANFD_RB_STATE_LESS_THRESHOLD       (1)
#define CANFD_RB_STATE_EQUAL_MORE_THRESHOLD (2)
#define CANFD_RB_STATE_FULL                 (3)
/**
  * @}
  */

/** @defgroup CANFD_ERROR_CODE canfd error code definition
  * @{
  */
#define CANFD_ERROR_CODE_NO                 (0)
#define CANFD_ERROR_CODE_BIT                (1)
#define CANFD_ERROR_CODE_FORM               (2)
#define CANFD_ERROR_CODE_STUFF              (3)
#define CANFD_ERROR_CODE_ACKNOWLEDGEMENT    (4)
#define CANFD_ERROR_CODE_CRC                (5)
#define CANFD_ERROR_CODE_OTHER              (6)
/**
  * @}
  */

/** @defgroup CANFD_ACF_ID_TYPE canfd acceptance filter id type definition
  * @{
  */
#define CANFD_ACF_STANDARD_AND_EXTENDED (0)
#define CANFD_ACF_STANDARD_ONLY         (1)
#define CANFD_ACF_EXTENDED_ONLY         (2)
/**
  * @}
  */

/** @defgroup CANFD_INTEN interrupt definition
  * @{
  */
#define CANFD_INTEN_ERROR            (1 << 1)
#define CANFD_INTEN_STB_TC           (1 << 2)
#define CANFD_INTEN_PTB_TC           (1 << 3)
#define CANFD_INTEN_RB_THRESHOLD     (1 << 4)
#define CANFD_INTEN_RB_FULL          (1 << 5)
#define CANFD_INTEN_RB_OVERRUN       (1 << 6)
#define CANFD_INTEN_RB_AVAILABLE     (1 << 7)
#define CANFD_INTEN_BUS_ERROR        (1 << 17)
#define CANFD_INTEN_ARBITRATION_LOST (1 << 19)
#define CANFD_INTEN_ERROR_PASSIVE    (1 << 21)
#define CANFD_INTEN_TT_TIME_TRIGGER  (1 << 28)
#define CANFD_INTEN_TT_WATCH_TRIGGER (1 << 31)
#define CANFD_INTEN_ALL              (0x902A00FE)
/**
  * @}
  */

/** @defgroup CANFD_INTSTS interrupt definition
  * @{
  */
#define CANFD_INTSTS_STB_FULL           (1 << 0)
#define CANFD_INTSTS_TT_TB_FILLED       (1 << 0)
#define CANFD_INTSTS_ABORT              (1 << 8)
#define CANFD_INTSTS_ERROR              (1 << 9)
#define CANFD_INTSTS_STB_TC             (1 << 10)
#define CANFD_INTSTS_TT_TC              (1 << 10)
#define CANFD_INTSTS_PTB_TC             (1 << 11)
#define CANFD_INTSTS_RB_THRESHOLD       (1 << 12)
#define CANFD_INTSTS_RB_FULL            (1 << 13)
#define CANFD_INTSTS_RB_OVERRUN         (1 << 14)
#define CANFD_INTSTS_RB_AVAILABLE       (1 << 15)
#define CANFD_INTSTS_BUS_ERROR          (1 << 16)
#define CANFD_INTSTS_ARBITRATION_LOST   (1 << 18)
#define CANFD_INTSTS_ERROR_PASSIVE      (1 << 20)
#define CANFD_INTSTS_ERROR_PASSIVE_MODE (1 << 22)
#define CANFD_INTSTS_EWL_REACHED        (1 << 23)
#define CANFD_INTSTS_TT_TIME_TRIGGER    (1 << 27)
#define CANFD_INTSTS_TT_TRIGGER_ERROR   (1 << 29)
#define CANFD_INTSTS_TT_WATCH_TRIGGER   (1 << 30)
#define CANFD_INTSTS_ALL                (0x68D5FF01)
/**
  * @}
  */

/** @defgroup CANFD_INTCLR interrupt definition
  * @{
  */
#define CANFD_INTCLR_STB_FULL         (1 << 0)
#define CANFD_INTCLR_TT_TB_FILLED     (1 << 0)
#define CANFD_INTCLR_ABORT            (1 << 8)
#define CANFD_INTCLR_ERROR            (1 << 9)
#define CANFD_INTCLR_STB_TC           (1 << 10)
#define CANFD_INTCLR_TT_TC            (1 << 10)
#define CANFD_INTCLR_PTB_TC           (1 << 11)
#define CANFD_INTCLR_RB_THRESHOLD     (1 << 12)
#define CANFD_INTCLR_RB_FULL          (1 << 13)
#define CANFD_INTCLR_RB_OVERRUN       (1 << 14)
#define CANFD_INTCLR_RB_AVAILABLE     (1 << 15)
#define CANFD_INTCLR_BUS_ERROR        (1 << 16)
#define CANFD_INTCLR_ARBITRATION_LOST (1 << 18)
#define CANFD_INTCLR_ERROR_PASSIVE    (1 << 20)
#define CANFD_INTCLR_TT_TIME_TRIGGER  (1 << 27)
#define CANFD_INTCLR_TT_TRIGGER_ERROR (1 << 29)
#define CANFD_INTCLR_TT_WATCH_TRIGGER (1 << 30)
#define CANFD_INTCLR_ALL              (0x6815FF00)
#define CANFD_INT_EVENT_MASK          (0xFFFFFF)
#define CANFD_INT_TT_MASK             (0xFF << 24)
/**
  * @}
  */

/** @defgroup CANFD_TIME_STAMPING canfd CiA 603 time stamping type definition
  * @{
  */
#define CANFD_TIME_STAMPING_NONE    (0)
#define CANFD_TIME_STAMPING_POS_SOF (1)
#define CANFD_TIME_STAMPING_POS_EOF (2)
/**
  * @}
  */

/** @defgroup CANFD_TT_PRESC canfd ttcan timer prescaler type definition
  * @{
  */
#define CANFD_TT_PRESC_1 (0)
#define CANFD_TT_PRESC_2 (1)
#define CANFD_TT_PRESC_4 (2)
#define CANFD_TT_PRESC_8 (3)
/**
  * @}
  */

/** @defgroup CANFD_CMD canfd feature control cmd definition
  * @{
  */
#define CANFD_CMD_SET_RESET                            (0x01)
#define CANFD_CMD_SET_PTB_SHOT                         (0x02)
#define CANFD_CMD_SET_STB_SHOT                         (0x03)
#define CANFD_CMD_SET_BUS_OFF                          (0x04)
#define CANFD_CMD_SET_TRANSCEIVER_STANDBY_MODE         (0x05)
#define CANFD_CMD_SET_STB_PRIORITY_MODE                (0x06)
#define CANFD_CMD_SET_SELF_ACK_EXTERNAL_LOOPBACK       (0x07)
#define CANFD_CMD_SET_RX_OVERFLOW_DROP_NEW             (0x08)
#define CANFD_CMD_SET_RX_STORE_CORRECT_AND_ERROR_FRAME (0x09)
#define CANFD_CMD_SET_RX_THRESHOLD                     (0x0A)
#define CANFD_CMD_GET_RX_THRESHOLD                     (0x0B)
#define CANFD_CMD_SET_ERROR_WARNING_LIMIT              (0x0C)
#define CANFD_CMD_GET_ERROR_WARNING_LIMIT              (0x0D)
#define CANFD_CMD_GET_VERSION                          (0x0E)
#define CANFD_CMD_SET_TIME_STAMPING                    (0x0F)

// clang-format off
#define IS_CANFD_MODE_TYPE(type) (((type) == CANFD_MODE_NORMAL)            || \
                                  ((type) == CANFD_MODE_LISTEN_ONLY)       || \
                                  ((type) == CANFD_MODE_INTERNAL_LOOPBACK) || \
                                  ((type) == CANFD_MODE_EXTERNAL_LOOPBACK))

#define IS_CANFD_ID_TYPE(type) (((type) == CANFD_ID_STANDARD) || \
                                ((type) == CANFD_ID_EXTENDED))

#define IS_CANFD_FRAME_TYPE(type) (((type) == CANFD_FRAME_DATA) || \
                                   ((type) == CANFD_FRAME_REMOTE))

#define IS_CANFD_ESI_TYPE(type) (((type) == CANFD_ESI_ACTIVE) || \
                                 ((type) == CANFD_ESI_PASSIVE))

#define IS_CANFD_BRS_TYPE(type) (((type) == CANFD_BRS_OFF) || \
                                 ((type) == CANFD_BRS_ON))

#define IS_CANFD_FORMAT_TYPE(type) (((type) == CANFD_FORMAT_CLASSIC_CAN) || \
                                    ((type) == CANFD_FORMAT_FD_CAN))

#define IS_CANFD_TX_CMD_TYPE(type) (((type) == CANFD_TX_CMD_STB_ABORT)     || \
                                    ((type) == CANFD_TX_CMD_STB_ALL_START) || \
                                    ((type) == CANFD_TX_CMD_STB_ONE_START) || \
                                    ((type) == CANFD_TX_CMD_PTB_ABORT)     || \
                                    ((type) == CANFD_TX_CMD_PTB_START))

#define IS_CANFD_STB_STATE_TYPE(type) (((type) == CANFD_STB_STATE_EMPTY)           || \
                                       ((type) == CANFD_STB_STATE_LESS_EQUAL_HALF) || \
                                       ((type) == CANFD_STB_STATE_MORE_HALF)       || \
                                       ((type) == CANFD_STB_STATE_FULL))

#define IS_CANFD_RB_STATE_TYPE(type) (((type) == CANFD_RB_STATE_EMPTY)                 || \
                                       ((type) == CANFD_RB_STATE_LESS_THRESHOLD)       || \
                                       ((type) == CANFD_RB_STATE_EQUAL_MORE_THRESHOLD) || \
                                       ((type) == CANFD_RB_STATE_FULL))

#define IS_CANFD_ERROR_CODE_TYPE(type) (((type) == CANFD_ERROR_CODE_NO)       || \
                                 ((type) == CANFD_ERROR_CODE_BIT)             || \
                                 ((type) == CANFD_ERROR_CODE_FORM)            || \
                                 ((type) == CANFD_ERROR_CODE_STUFF)           || \
                                 ((type) == CANFD_ERROR_CODE_ACKNOWLEDGEMENT) || \
                                 ((type) == CANFD_ERROR_CODE_CRC)             || \
                                 ((type) == CANFD_ERROR_CODE_OTHER))

#define IS_CANFD_ACF_TYPE(type) (((type) == CANFD_ACF_STANDARD_AND_EXTENDED) || \
                                 ((type) == CANFD_ACF_STANDARD_ONLY)         || \
                                 ((type) == CANFD_ACF_EXTENDED_ONLY))

#define IS_CANFD_TIME_STAMPING_TYPE(type) (((type) == CANFD_TIME_STAMPING_NONE)    || \
                                           ((type) == CANFD_TIME_STAMPING_POS_SOF) || \
                                           ((type) == CANFD_TIME_STAMPING_POS_EOF))

#define IS_CANFD_TT_PRESC_TYPE(type) (((type) == CANFD_TT_PRESC_1) || \
                                      ((type) == CANFD_TT_PRESC_2) || \
                                      ((type) == CANFD_TT_PRESC_4) || \
                                      ((type) == CANFD_TT_PRESC_8))

// clang-format on

/**
 * @brief CANFD message header configuration structure
 *
 * @param identifier            CANFD identifier, must be a number between: 0~0x7FF if id_type is CANFD_STANDARD_ID, 0~0x1FFFFFFF if id_type is CANFD_EXTENDED_ID
 * @param id_type               CANFD identifier type of the message, use @ref CANFD_ID
 * @param frame_type            CANFD frame type of the message, use @ref CANFD_FRAME
 * @param data_length           CANFD frame length of the message
 * @param error_state_indicator CANFD error state indicator of the received message, not used when transmit, use @ref CANFD_ESI
 * @param bit_rate_switch       CANFD whether the message with or without bit rate switching, use @ref CANFD_BRS
 * @param fd_format             CANFD whether the message in classic or FD format, use @ref CANFD_FORMAT
 * @param time_stamping_enable  CANFD enable or disable time stamping of the messageRMAT
 * @param time_stamping         CANFD time stamping of the received message, not used when transmit
 */
struct bflb_canfd_message_header_s {
    uint32_t identifier;
    uint8_t id_type;
    uint8_t frame_type;
    uint8_t data_length;
    uint8_t error_state_indicator;
    uint8_t bit_rate_switch;
    uint8_t fd_format;
    uint8_t time_stamping_enable;
    uint64_t time_stamping;
};

/**
 * @brief CANFD timing structure
 *
 * @param prescaler       CANFD prescaler divides the system clock to get the time quanta clock tq_clk
 * @param sync_jump_width CANFD synchronization jump width
 * @param segment_1       CANFD bit timing segment 1
 * @param segment_2       CANFD bit timing segment 2
 */
struct bflb_canfd_timing_s {
    uint8_t prescaler;       /* 0~255(represent 1~256) is available */
    uint8_t segment_1;       /* 0~255(represent 1~256) is available for slow speed, 0~31(represent 1~32) is available for fast speed */
    uint8_t segment_2;       /* 0~127(represent 1~128) is available for slow speed, 0~15(represent 1~16) is available for fast speed */
    uint8_t sync_jump_width; /* 0~127(represent 1~128) is available for slow speed, 0~15(represent 1~16) is available for fast speed */
};

/**
 * @brief CANFD acceptance filter structure
 *
 * @param index   CANFD acceptance filter index
 * @param enable  CANFD enable or disable the acceptance filter
 * @param id_type CANFD accepts standard or extended frame, use @ref CANFD_ACF_ID_TYPE
 * @param resv    CANFD reserved parameter, not used
 * @param mask    CANFD acceptance filter mask value
 * @param code    CANFD acceptance filter code value
 */
struct bflb_canfd_acf_s {
    uint8_t index;
    uint8_t enable;
    uint8_t id_type;
    uint8_t resv;
    uint32_t mask;
    uint32_t code;
};

/**
 * @brief CANFD configuration structure
 *
 * @param timing_slow CANFD timing for slow speed
 * @param timing_fast CANFD timing for fast speed
 * @param acf         CANFD acceptance filter configuration pointer, NULL for skip configuration
 * @param acf_count   CANFD acceptance filter count
 * @param mode        CANFD operation mode, use @ref CANFD_MODE
 * @param bosch_mode  CANFD enable or disable bosch mode, 0 is for ISO 11898-1 mode
 */
struct bflb_canfd_config_s {
    struct bflb_canfd_timing_s timing_slow;
    struct bflb_canfd_timing_s timing_fast;
    struct bflb_canfd_acf_s *acf;
    uint8_t acf_count;
    uint8_t mode;
    uint8_t bosch_mode;
};

/**
 * @brief CANFD error structure
 *
 * @param error_code                CANFD error code, use @ref CANFD_ERROR_CODE
 * @param arbitration_lost_position CANFD arbitration Lost Capture (bit position in the frame where the arbitration has been lost)
 * @param receive_error_count       CANFD number of errors during reception
 * @param transmit_error_count      CANFD number of errors during transmission
 */
struct bflb_canfd_error_s {
    uint8_t error_code;
    uint8_t arbitration_lost_position;
    uint8_t receive_error_count;
    uint8_t transmit_error_count;
};

/**
 * @brief CANFD time tigger configuration structure
 *
 * @param time_stamping      CANFD CiA 603 time stamping type, use @ref CANFD_TIME_STAMPING
 * @param prescaler          CANFD timer prescaler, use @ref CANFD_TT_PRESC
 * @param watch_trigger_time CANFD watch trigger time, 0 is for disable, max is 65535
 */
struct bflb_canfd_tt_config_s {
    uint8_t time_stamping;
    uint8_t prescaler;
    uint16_t watch_trigger_time;
};

/**
 * @brief CANFD time tigger message configuration structure
 *
 * @param time_stamping      CANFD CiA 603 time stamping type, use @ref CANFD_TIME_STAMPING
 * @param prescaler          CANFD timer prescaler, use @ref CANFD_TT_PRESC
 * @param watch_trigger_time CANFD watch trigger time, 0 is for disable, max is 65535
 */
struct bflb_canfd_tt_message_config_s {
    uint8_t slot;
    uint8_t type;
    uint8_t tew;
    uint16_t trig_time;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize canfd.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save canfd config
 * @return A negated errno value on failure.
 */
int bflb_canfd_init(struct bflb_device_s *dev, const struct bflb_canfd_config_s *config);

/**
 * @brief Deinitialize canfd.
 *
 * @param [in] dev device handle
 */
void bflb_canfd_deinit(struct bflb_device_s *dev);

/**
 * @brief Fill primary tx buffer.
 *
 * @param [in] dev device handle
 * @param [in] head pointer of tx frame header
 * @param [in] data pointer of data to be transmitted
 */
void bflb_canfd_write_ptb(struct bflb_device_s *dev, struct bflb_canfd_message_header_s *head, uint8_t *data);

/**
 * @brief Fill secondary tx buffer.
 *
 * @param [in] dev device handle
 * @param [in] head pointer of tx frame header
 * @param [in] data pointer of data to be transmitted
 */
void bflb_canfd_write_stb(struct bflb_device_s *dev, struct bflb_canfd_message_header_s *head, uint8_t *data);

/**
 * @brief Read rx buffer.
 *
 * @param [in] dev device handle
 * @param [in] head pointer of rx frame header
 * @param [in] data pointer of data to store received data
 */
void bflb_canfd_read_rx_buffer(struct bflb_device_s *dev, struct bflb_canfd_message_header_s *head, uint8_t *data);

/**
 * @brief Execute one command for tx.
 *
 * @param [in] dev device handle
 * @param [in] cmd tx command, use @ref CANFD_TX_CMD
 */
void bflb_canfd_exe_tx_cmd(struct bflb_device_s *dev, uint8_t cmd);

/**
 * @brief Check whether primary transmit buffer is busy.
 *
 * @param [in] dev device handle
 * @return true or false.
 */
uint8_t bflb_canfd_ptb_is_busy(struct bflb_device_s *dev);

/**
 * @brief Get canfd second tx buffer state.
 *
 * @param [in] dev device handle
 * @return second tx buffer state, use @ref CANFD_STB_STATE.
 */
uint8_t bflb_canfd_get_tx_buffer_state(struct bflb_device_s *dev);

/**
 * @brief Get canfd rx buffer state.
 *
 * @param [in] dev device handle
 * @return rx buffer state, use @ref CANFD_RB_STATE.
 */
uint8_t bflb_canfd_get_rx_buffer_state(struct bflb_device_s *dev);

/**
 * @brief Set transmit delay compensation.
 *
 * @param [in] dev device handle
 * @param [in] enable enable or disable transmit delay compensation
 * @param [in] offset secondary sample point offset, 0~127 is available
 */
void bflb_canfd_set_tdc(struct bflb_device_s *dev, uint8_t enable, uint8_t offset);

/**
 * @brief Get canfd error state.
 *
 * @param [in] dev device handle
 * @param [in] error pointer of error structure
 */
void bflb_canfd_get_error_state(struct bflb_device_s *dev, struct bflb_canfd_error_s *error);

/**
 * @brief Enable canfd interrupt.
 *
 * @param [in] dev device handle
 * @param [in] inten interrupt type, use @ref CANFD_INTEN
 */
void bflb_canfd_int_enable(struct bflb_device_s *dev, uint32_t inten);

/**
 * @brief Disable canfd interrupt.
 *
 * @param [in] dev device handle
 * @param [in] inten interrupt type, use @ref CANFD_INTEN
 */
void bflb_canfd_int_disable(struct bflb_device_s *dev, uint32_t inten);

/**
 * @brief Get canfd interrupt status.
 *
 * @param [in] dev device handle
 * @return interrupt status, use @ref CANFD_INTSTS
 */
uint32_t bflb_canfd_get_intstatus(struct bflb_device_s *dev);

/**
 * @brief Clear canfd interrupt status.
 *
 * @param [in] dev device handle
 * @param [in] intclr clear value, use @ref CANFD_INTCLR
 */
void bflb_canfd_int_clear(struct bflb_device_s *dev, uint32_t intclr);

/**
 * @brief Get canfd transmit time stamping value.
 *
 * @param [in] dev device handle
 * @return transmit time stamping value
 */
uint64_t bflb_canfd_get_tts(struct bflb_device_s *dev);

/**
 * @brief Initialize time-trigger canfd.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save time-trigger canfd config
 */
void bflb_canfd_tt_init(struct bflb_device_s *dev, const struct bflb_canfd_tt_config_s *config);

/**
 * @brief Initialize time-trigger canfd.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save time-trigger canfd config
 */
void bflb_canfd_tt_set_reference_id(struct bflb_device_s *dev, uint32_t id, uint8_t ide_enable);

void bflb_canfd_tt_write_tb(struct bflb_device_s *dev, struct bflb_canfd_message_header_s *head, uint8_t *data, struct bflb_canfd_tt_message_config_s *tt);

/**
 * @brief Control canfd feature.
 *
 * @param [in] dev device handle
 * @param [in] cmd feature command, use @ref CANFD_CMD
 * @param [in] arg user data
 * @return A negated errno value on failure.
 */
int bflb_canfd_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

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
