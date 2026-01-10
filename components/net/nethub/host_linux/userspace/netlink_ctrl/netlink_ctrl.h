/**
 * @file netlink_ctrl.h
 * @brief Netlink Userspace Control Library Header
 */

#ifndef __NETLINK_CTRL_H__
#define __NETLINK_CTRL_H__

#include <stdint.h>
#include <stddef.h>
#include "netlink_protocol.h"

/* ==================== Constant Definitions ==================== */
#define NETLINK_PROTO          (31)
#define NETLINK_GROUP          (1)
#define NETLINK_USER_DATA_MTU  (1024)
#define MAX_MSGLEN             (NETLINK_USER_DATA_MTU+NETLINK_DATA_HDR_LEN)

/* Netlink handle */
typedef void *netlink_handle_t;

/* Receive callback function type (per data type) */
typedef void (*netlink_recv_callback_t)(const void *data, size_t len);

/**
 * @brief Initialize Netlink connection
 * @param[out] handle Returned handle
 * @retval 0 Success
 * @retval <0 Error code
 */
int netlink_init(netlink_handle_t *handle);

/**
 * @brief Deinitialize Netlink connection
 * @param[in] handle Handle
 * @retval 0 Success
 * @retval <0 Error code
 */
int netlink_deinit(netlink_handle_t handle);

/**
 * @brief Send data to device with type
 * @param[in] handle Handle
 * @param[in] data_type Data type (see netlink_data_type enum in netlink_protocol.h)
 * @param[in] data Data buffer
 * @param[in] len Data length
 * @retval >=0 Sent bytes
 * @retval <0 Error code
 */
int netlink_send(netlink_handle_t handle, uint8_t data_type, const void *data, size_t len);

/**
 * @brief Register receive callback function for specific data type
 * @param[in] handle Handle
 * @param[in] data_type Data type (see netlink_data_type enum in netlink_protocol.h)
 * @param[in] callback Receive callback function
 * @retval 0 Success
 * @retval <0 Error code
 */
int netlink_register_callback(netlink_handle_t handle, uint8_t data_type, netlink_recv_callback_t callback);

/**
 * @brief Get socket file descriptor (for select/poll)
 * @param[in] handle Handle
 * @retval >=0 Socket fd
 * @retval <1 Error
 */
int netlink_get_fd(netlink_handle_t handle);

#endif /* __NETLINK_CTRL_H__ */
