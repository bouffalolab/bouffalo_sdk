/**
 * @file mr_debugfs.h
 * @brief Debug support and logging macros
 * @details This file contains debug logging macros and DebugFS interface definitions
 *          for kernel driver debugging and monitoring
 */

#ifndef __MR_DEBUGFS_H__
#define __MR_DEBUGFS_H__

/** @defgroup DEBUGFS_MACROS DebugFS Helper Macros
 * @brief Macros for creating DebugFS file operations (currently disabled)
 * @{
 */
#if 0

/**
 * @brief Add a DebugFS file with specified permissions
 * @param name File name
 * @param parent Parent directory
 * @param mode File permissions
 */
#define DEBUGFS_ADD_FILE(name, parent, mode)                                         \
    do {                                                                             \
        if (!debugfs_create_file(#name, mode, parent, card, &mr_dbgfs_##name##_ops)) \
            goto err;                                                                \
    } while (0)

/**
 * @brief Declare a DebugFS read function
 * @param name Function name suffix
 */
#define DEBUGFS_READ_FUNC(name) \
    static ssize_t mr_dbgfs_##name##_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos);

/**
 * @brief Define read-only DebugFS file operations
 * @param name Function name suffix
 */
#define DEBUGFS_READ_FILE_OPS(name)                               \
    DEBUGFS_READ_FUNC(name);                                      \
    static const struct file_operations mr_dbgfs_##name##_ops = { \
        .read = mr_dbgfs_##name##_read,                           \
        .open = simple_open,                                      \
        .llseek = generic_file_llseek,                            \
    };

/**
 * @brief Declare a DebugFS write function
 * @param name Function name suffix
 */
#define DEBUGFS_WRITE_FUNC(name) \
    static ssize_t mr_dbgfs_##name##_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos);

/**
 * @brief Define write-only DebugFS file operations
 * @param name Function name suffix
 */
#define DEBUGFS_WRITE_FILE_OPS(name)                              \
    DEBUGFS_WRITE_FUNC(name);                                     \
    static const struct file_operations mr_dbgfs_##name##_ops = { \
        .write = mr_dbgfs_##name##_write,                         \
        .open = simple_open,                                      \
        .llseek = generic_file_llseek,                            \
    };

/**
 * @brief Unregister DebugFS entries for SDIO card
 * @param card SDIO card structure
 */
void mr_debugfs_unregister(struct mr_sdio_card *card);

/**
 * @brief Register DebugFS entries for SDIO card
 * @param card SDIO card structure
 * @retval 0 Success
 * @retval <0 Error code
 */
int mr_debugfs_register(struct mr_sdio_card *card);

#endif
/** @} */

/** @defgroup SDIO_DEBUG_MACROS SDIO Driver Debug Logging Macros
 * @brief Debug logging macros for SDIO driver subsystem
 * @{
 */

/**
 * @brief SDIO driver debug logging (currently disabled)
 * @param card SDIO card structure
 * @param a Format string
 * @param ... Format arguments
 */
#if (0)
#define SDIO_DRV_DBG(card, a, ...) pr_info("[sdio%d fn%d] " a, card->sdio_hw_ver, card->func->num, ##__VA_ARGS__)
#else
#define SDIO_DRV_DBG(a, ...)
#endif

/**
 * @brief SDIO driver informational logging
 * @param card SDIO card structure
 * @param a Format string
 * @param ... Format arguments
 */
#if (1)
#define SDIO_DRV_INFO(card, a, ...) pr_notice("[sdio%d fn%d] " a, card->sdio_hw_ver, card->func->num, ##__VA_ARGS__)
#else
#define SDIO_DRV_INFO(a, ...)
#endif

/**
 * @brief SDIO driver error logging
 * @param card SDIO card structure
 * @param a Format string
 * @param ... Format arguments
 */
#define SDIO_DRV_ERR(card, a, ...) pr_err("[sdio%d fn%d] " a, card->sdio_hw_ver, card->func->num, ##__VA_ARGS__)

/** @} */

/** @defgroup MSG_CTRL_DEBUG_MACROS Message Control Debug Logging Macros
 * @brief Debug logging macros for message control subsystem
 * @{
 */

/**
 * @brief Message control debug logging (currently disabled)
 * @param msg_ctrl Message control structure
 * @param a Format string
 * @param ... Format arguments
 */
#if (0)
#define MSG_CTRL_DBG(msg_ctrl, a, ...) pr_info("[mr_msg] " a, ##__VA_ARGS__)
#else
#define MSG_CTRL_DBG(a, ...)
#endif

/**
 * @brief Message control informational logging
 * @param msg_ctrl Message control structure
 * @param a Format string
 * @param ... Format arguments
 */
#if (1)
#define MSG_CTRL_INFO(msg_ctrl, a, ...) pr_notice("[mr_msg] " a, ##__VA_ARGS__)
#else
#define MSG_CTRL_INFO(a, ...)
#endif

/**
 * @brief Message control error logging
 * @param msg_ctrl Message control structure
 * @param a Format string
 * @param ... Format arguments
 */
#define MSG_CTRL_ERR(msg_ctrl, a, ...) pr_err("[mr_msg] " a, ##__VA_ARGS__)

/** @} */

/** @defgroup SPEED_TEST_DEBUG_MACROS Speed Test Debug Logging Macros
 * @brief Debug logging macros for speed test subsystem
 * @{
 */

/**
 * @brief Speed test debug logging (currently disabled)
 * @param speed_test Speed test structure
 * @param a Format string
 * @param ... Format arguments
 */
#if (0)
#define SPEED_TEST_DBG(speed_test, a, ...) pr_info("[speed_test %d] " a, speed_test->msg_tag, ##__VA_ARGS__)
#else
#define SPEED_TEST_DBG(a, ...)
#endif

/**
 * @brief Speed test informational logging
 * @param speed_test Speed test structure
 * @param a Format string
 * @param ... Format arguments
 */
#if (1)
#define SPEED_TEST_INFO(speed_test, a, ...) pr_notice("[speed_test %d] " a, speed_test->msg_tag, ##__VA_ARGS__)
#else
#define SPEED_TEST_INFO(a, ...)
#endif

/**
 * @brief Speed test error logging
 * @param speed_test Speed test structure
 * @param a Format string
 * @param ... Format arguments
 */
#define SPEED_TEST_ERR(speed_test, a, ...) pr_err("[speed_test %d] " a, speed_test->msg_tag, ##__VA_ARGS__)

/** @} */

/** @defgroup ETH_NETDEV_DEBUG_MACROS Ethernet Network Device Debug Logging Macros
 * @brief Debug logging macros for ethernet network device subsystem
 * @{
 */

/**
 * @brief Ethernet network device debug logging (currently disabled)
 * @param eth_netdev Ethernet network device structure
 * @param a Format string
 * @param ... Format arguments
 */
#if (0)
#define ETH_NETDEV_DBG(eth_netdev, a, ...) pr_info("[eth_netdev] " a, ##__VA_ARGS__)
#else
#define ETH_NETDEV_DBG(a, ...)
#endif

/**
 * @brief Ethernet network device informational logging
 * @param eth_netdev Ethernet network device structure
 * @param a Format string
 * @param ... Format arguments
 */
#if (1)
#define ETH_NETDEV_INFO(eth_netdev, a, ...) pr_notice("[eth_netdev] " a, ##__VA_ARGS__)
#else
#define ETH_NETDEV_INFO(a, ...)
#endif

/**
 * @brief Ethernet network device error logging
 * @param eth_netdev Ethernet network device structure
 * @param a Format string
 * @param ... Format arguments
 */
#define ETH_NETDEV_ERR(eth_netdev, a, ...) pr_err("[eth_netdev] " a, ##__VA_ARGS__)

/** @} */

/** @defgroup TTY_MSG_DEBUG_MACROS TTY Message Debug Logging Macros
 * @brief Debug logging macros for TTY message subsystem
 * @{
 */

#if (0)
#define TTY_DBG(tty_msg, fmt, args...) \
    pr_info("[%s] " fmt, tty_msg->tty_driver->driver_name, tty_msg->tty_driver->major, ##args)
#else
#define TTY_DBG(tty_msg, fmt, args...)
#endif

#if (1)
#define TTY_INFO(tty_msg, fmt, args...) pr_notice("[%s] " fmt, tty_msg->tty_driver->name, ##args)
#else
#define TTY_INFO(tty_msg, fmt, args...)
#endif

#define TTY_ERR(tty_msg, fmt, args...) pr_err("[%s] " fmt, tty_msg->tty_driver->driver_name, ##args)

/** @} */

/** @defgroup NETLINK_DEBUG_MACROS Netlink Debug Logging Macros
 * @brief Debug logging macros for Netlink subsystem
 * @{
 */

#if (0)
#define NETLINK_DBG(netlink, fmt, args...) pr_info("[mr_netlink] " fmt, ##args)
#else
#define NETLINK_DBG(netlink, fmt, args...)
#endif

#if (1)
#define NETLINK_INFO(netlink, fmt, args...) pr_notice("[mr_netlink] " fmt, ##args)
#else
#define NETLINK_INFO(netlink, fmt, args...)
#endif

#define NETLINK_ERR(netlink, fmt, args...) pr_err("[mr_netlink] " fmt, ##args)

/** @} */

#endif //__MR_DEBUGFS_H__
