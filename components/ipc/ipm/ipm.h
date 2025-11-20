
#ifndef __DRIVERS_IPM_H_
#define __DRIVERS_IPM_H_

/**
 * @brief IPM Interface
 * @defgroup ipm_interface IPM Interface
 * @since 1.0
 * @version 1.0.0
 * @ingroup io_interfaces
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdint.h>
#include <bflb_ipc.h>

typedef struct
{
    struct bflb_device_s *ipc;
    void *data;
} ipm_device_t;

/**
 * @typedef ipm_callback_t
 * @brief Callback API for incoming IPM messages
 *
 * These callbacks execute in interrupt context. Therefore, use only
 * interrupt-safe APIS. Registration of callbacks is done via
 * @a ipm_register_callback
 *
 * @param ipmdev Driver instance
 * @param user_data Pointer to some private data provided at registration
 *        time.
 * @param id Message type identifier.
 * @param data Message data pointer. The correct amount of data to read out
 *        must be inferred using the message id/upper level protocol.
 */
typedef void (*ipm_callback_t)(ipm_device_t *ipmdev, void *user_data,
                   uint32_t id, volatile void *data);

/**
 * @brief Try to send a message over the IPM device.
 *
 * A message is considered consumed once the remote interrupt handler
 * finishes. If there is deferred processing on the remote side,
 * or if outgoing messages must be queued and wait on an
 * event/semaphore, a high-level driver can implement that.
 *
 * There are constraints on how much data can be sent or the maximum value
 * of id. Use the @a ipm_max_data_size_get and @a ipm_max_id_val_get routines
 * to determine them.
 *
 * The @a size parameter is used only on the sending side to determine
 * the amount of data to put in the message registers. It is not passed along
 * to the receiving side. The upper-level protocol dictates the amount of
 * data read back.
 *
 * @param ipmdev Driver instance
 * @param wait If nonzero, busy-wait for remote to consume the message. The
 *           message is considered consumed once the remote interrupt handler
 *           finishes. If there is deferred processing on the remote side,
 *           or you would like to queue outgoing messages and wait on an
 *           event/semaphore, you can implement that in a high-level driver
 * @param id Message identifier. Values are constrained by
 *        @a ipm_max_data_size_get since many boards only allow for a
 *        subset of bits in a 32-bit register to store the ID.
 * @param data Pointer to the data sent in the message.
 * @param size Size of the data.
 *
 * @retval -EBUSY    If the remote hasn't yet read the last data sent.
 * @retval -EMSGSIZE If the supplied data size is unsupported by the driver.
 * @retval -EINVAL   If there was a bad parameter, such as: too-large id value.
 *                   or the device isn't an outbound IPM channel.
 * @retval 0         On success.
 */
int ipm_send(ipm_device_t *ipmdev, int wait, uint32_t id,
               const void *data, int size);


/**
 * @brief Register a callback function for incoming messages.
 *
 * @param ipmdev Driver instance pointer.
 * @param cb Callback function to execute on incoming message interrupts.
 * @param user_data Application-specific data pointer which will be passed
 *        to the callback function when executed.
 */
void ipm_register_callback(ipm_device_t *ipmdev,
                     ipm_callback_t cb, void *user_data);

/**
 * @brief Return the maximum number of bytes possible in an outbound message.
 *
 * IPM implementations vary on the amount of data that can be sent in a
 * single message since the data payload is typically stored in registers.
 *
 * @param ipmdev Driver instance pointer.
 *
 * @return Maximum possible size of a message in bytes.
 */
int ipm_max_data_size_get(ipm_device_t *ipmdev);

/**
 * @brief Return the maximum id value possible in an outbound message.
 *
 * Many IPM implementations store the message's ID in a register with
 * some bits reserved for other uses.
 *
 * @param ipmdev Driver instance pointer.
 *
 * @return Maximum possible value of a message ID.
 */
uint32_t ipm_max_id_val_get(ipm_device_t *ipmdev);

/**
 * @brief Enable interrupts and callbacks for inbound channels.
 *
 * @param ipmdev Driver instance pointer.
 * @param enable Set to 0 to disable and to nonzero to enable.
 *
 * @retval 0       On success.
 * @retval -EINVAL If it isn't an inbound channel.
 */
int ipm_set_enabled(ipm_device_t *ipmdev, int enable);

/**
 * @brief Signal asynchronous command completion
 *
 * Some IPM backends have an ability to deliver a command
 * asynchronously.  The callback will be invoked in interrupt context,
 * but the message (including the provided data pointer) will stay
 * "active" and unacknowledged until later code (presumably in thread
 * mode) calls ipm_complete().
 *
 * This function is, obviously, a noop on drivers without async
 * support.
 *
 * @param ipmdev Driver instance pointer.
 */
void ipm_complete(ipm_device_t *ipmdev);



#ifdef __cplusplus
}
#endif

/**
 * @}
 */



#endif /* __DRIVERS_IPM_H_ */
