#ifndef _BFLB_I2C_H
#define _BFLB_I2C_H

#include "bflb_core.h"

/* Bit definitions for the flags field in struct bflb_i2c_msg_s
 *
 * START/STOP Rules:
 *
 * 1. The lower half I2C driver will always issue the START condition at the
 *    beginning of a message unless I2C_M_NOSTART flag is set in the
 *    message.
 *
 * 2. The lower half I2C driver will always issue the STOP condition at the
 *    end of the messages unless:
 *
 *    a. The I2C_M_NOSTOP flag is set in the message, OR
 *    b. The following message has the I2C_M_NOSTART flag set (meaning
 *       that following message is simply a continuation of the transfer).
 *
 * A proper I2C repeated start would then have I2C_M_NOSTOP set on msg[n]
 * and I2C_M_NOSTART *not* set on msg[n+1].  See the following table:
 *
 *   msg[n].flags  msg[n+1].flags Behavior
 *   ------------ --------------- -----------------------------------------
 *   0            0                Two normal, separate messages with STOP
 *                                 on msg[n] then START on msg[n+1]
 *   0*           I2C_M_NOSTART    Continuation of the same transfer (must
 *                                 be the same direction).  See NOTE below.
 *   NO_STOP      0                No STOP on msg[n]; repeated START on
 *                                 msg[n+1].
 *
 * * NOTE: NO_STOP is implied in this case and may or not be explicitly
 *   included in the msg[n] flags
 */

#define I2C_M_READ    0x0001 /* Read data, from slave to master */
#define I2C_M_TEN     0x0002 /* Ten bit address */
#define I2C_M_NOSTOP  0x0040 /* Message should not end with a STOP */
#define I2C_M_NOSTART 0x0080 /* Message should not begin with a START */

/* I2c bus speed */

#define I2C_SPEED_STANDARD  100000  /* Standard speed (100Khz) */
#define I2C_SPEED_FAST      400000  /* Fast speed     (400Khz) */
#define I2C_SPEED_FAST_PLUS 1000000 /* Fast+ speed    (  1Mhz) */
#define I2C_SPEED_HIGH      3400000 /* High speed     (3.4Mhz) */

/**
 * @brief I2C message structure
 *
 * @param addr          Slave address (7- or 10-bit)
 * @param flags         See I2C_M_* definitions
 * @param buffer        Buffer to be transferred
 * @param length        Length of the buffer in bytes, should be less than 256.
 */
struct bflb_i2c_msg_s {
    uint16_t addr;
    uint16_t flags;
    uint8_t *buffer;
    uint16_t length;
};

#ifdef __cplusplus
extern "C" {
#endif

void bflb_i2c_init(struct bflb_device_s *dev, uint32_t frequency);
void bflb_i2c_deinit(struct bflb_device_s *dev);
void bflb_i2c_link_txdma(struct bflb_device_s *dev, bool enable);
void bflb_i2c_link_rxdma(struct bflb_device_s *dev, bool enable);
int bflb_i2c_transfer(struct bflb_device_s *dev, struct bflb_i2c_msg_s *msgs, int count);

void bflb_i2c_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

#ifdef __cplusplus
}
#endif

#endif