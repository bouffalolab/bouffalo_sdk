/**
 * @file hd_sdio_manage.h
 * @brief Host Driver SDIO management layer header definitions
 * @details This file contains the core SDIO management structures and interfaces
 *          for handling data transfer between host and device through SDIO interface.
 * @author mlwang
 * @date 2025-07-17
 * @version 1.0
 */

#ifndef __HD_SDIO_MANAGE_H__
#define __HD_SDIO_MANAGE_H__

#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>

/* Forward declarations */
struct hd_tty_priv;

/** @defgroup SDIO_MANAGE_MACROS SDIO Management Macros
 * @{
 */
/** Maximum retry count for download operations */
#define SDIO_MANAGE_DNLD_RETRY_CNT_MAX    (100)
/** Maximum wait count for upload SKB operations */
#define SDIO_MANAGE_UPLD_SKB_WAIT_CNT_MAX (100)

/** Download SKB queue depth */
#define SDIO_MANAGE_DNLD_SKB_QUEUE_DEPTH  (128)
/** Upload SKB queue depth */
#define SDIO_MANAGE_UPLD_SKB_QUEUE_DEPTH  (1024)
/** @} */

/** Forward declaration of SDIO management structure */
struct hd_sdio_manage;

/**
 * @struct sdio_trans_desc
 * @brief SDIO data transfer descriptor
 * @details Describes a single SDIO data transfer operation with buffer information
 */
struct sdio_trans_desc {
    int buff_len;  /**< Buffer length allocated */
    int data_len;  /**< Actual data length to transfer */
    uint8_t *buff; /**< Pointer to data buffer */
};

/**
 * @typedef trans_cplt_cb
 * @brief Transfer completion callback function type
 * @param[in] skb Socket buffer that was transferred
 * @param[in] success Transfer success flag (true: success, false: failed/timeout)
 * @param[in] arg User-defined argument passed to callback
 * @retval 0 Success - callback has taken ownership of skb
 * @retval <0 Error - callback rejects skb, caller should free skb
 * @details Resource management contract:
 *          - If callback returns 0: callback takes ownership of skb, caller must not free it
 *          - If callback returns <0: callback rejects skb, caller (SDIO manage) will free it
 *          - Callback should never free skb AND return negative value (double-free risk)
 */
typedef int (*trans_cplt_cb)(struct sk_buff *skb, bool success, void *arg);

/**
 * @typedef card2host_irq_cb
 * @brief Card-to-host interrupt callback function type
 * @param[in] sdio_manage Pointer to SDIO management structure
 */
typedef void (*card2host_irq_cb)(struct hd_sdio_manage *sdio_manage);

/**
 * @struct hd_sdio_card
 * @brief SDIO card abstraction structure
 * @details This structure represents an SDIO card device and provides unified
 *          interface for both SDIO2 and SDIO3 hardware implementations
 */
struct hd_sdio_card {
    /** @name Hardware Information
     * @{
     */
    struct sdio_func *func; /**< SDIO function pointer */
    uint8_t sdio_hw_ver;    /**< SDIO hardware version (2:sdio2, 3:sdio3) */
    bool sdio_boot;         /**< Bootrom mode flag */
    bool app_run;           /**< Application running mode flag */
    bool rd_len_compress;   /**< Read length compression support flag */
    /** @} */

    /** @name Buffer and Queue Configuration
     * @{
     */
    uint16_t dnld_max_size; /**< Slave download buffer size */
    uint16_t upld_max_size; /**< Host upload buffer size */
    uint8_t queue_depth;    /**< Queue depth (must be 2^n) */
    uint8_t queue_mask;     /**< Queue mask (queue_depth - 1) */
    /** @} */

    /** @name Queue Management
     * @{
     */
    uint8_t curr_dnld_in;  /**< Download queue input pointer */
    uint8_t curr_dnld_out; /**< Download queue output pointer */
    uint8_t curr_upld_in;  /**< Upload queue input pointer */
    uint8_t curr_upld_out; /**< Upload queue output pointer */
    /** @} */

    /** @name SDIO2 Specific Fields
     * @{
     */
    uint16_t sdio2_dnld_bitmap;         /**< Download bitmap (SDIO2 only) */
    uint16_t sdio2_upld_bitmap;         /**< Upload bitmap (SDIO2 only) */
    uint32_t sdio2_cmd53_ioport_offset; /**< IO port base offset (SDIO2 only) */
    /** @} */

    /** @name Management and Network Interface
     * @{
     */
    struct hd_sdio_manage *sdio_manage; /**< Associated SDIO management structure */
    struct net_device *netdev;          /**< Associated network device */
    struct hd_tty_priv *tty_msg;        /**< Associated TTY message interface */
    /** @} */

    /** @name Function Pointers - Register Operations
     * @{
     */
    /** Read from function register */
    int (*sdio_read_reg)(struct hd_sdio_card *card, uint32_t reg, uint8_t *data);
    /** Write to function register */
    int (*sdio_write_reg)(struct hd_sdio_card *card, uint32_t reg, uint8_t data);
    /** @} */

    /** @name Function Pointers - Function 0 Register Operations
     * @{
     */
    /** Read from function 0 register */
    int (*sdio_read_fn0_reg)(struct hd_sdio_card *card, uint32_t reg, uint8_t *data);
    /** Write to function 0 register */
    int (*sdio_write_fn0_reg)(struct hd_sdio_card *card, uint32_t reg, uint8_t data);
    /** @} */

    /** @name Function Pointers - Custom Register Operations (SDIO3 only)
     * @{
     */
    /** Read from custom register (SDIO3 only) */
    int (*sdio_read_custom_reg)(struct hd_sdio_card *card, uint16_t reg_offset, void *buff, uint16_t len);
    /** Write to custom register (SDIO3 only) */
    int (*sdio_write_custom_reg)(struct hd_sdio_card *card, uint16_t reg_offset, void *buff, uint16_t len);
    /** @} */

    /** @name Function Pointers - Data Transfer Operations
     * @{
     */
    /** Upload (device to host) data transfer */
    int (*sdio_upld_transf)(struct hd_sdio_card *card, struct sdio_trans_desc *upld_desc, int retry_cnt);
    /** Download (host to device) data transfer */
    int (*sdio_dnld_transf)(struct hd_sdio_card *card, struct sdio_trans_desc *dnld_desc, int retry_cnt);
    /** @} */

    /** @name Function Pointers - Interrupt Handling
     * @{
     */
    /** Register upload interrupt callback */
    int (*sdio_upld_irq_register)(struct hd_sdio_card *card, card2host_irq_cb upld_irq_cb);
    /** Upload interrupt callback function */
    card2host_irq_cb upld_irq_cb;
    /** @} */
};

/**
 * @struct hd_sdio_manage
 * @brief SDIO transfer control and management structure
 * @details Central structure for managing SDIO data transfers through work queues
 *          and socket buffer management
 */
struct hd_sdio_manage {
    /** @name Work Queue Management
     * @{
     */
    struct work_struct sdio_work;            /**< SDIO work structure */
    struct workqueue_struct *sdio_workqueue; /**< SDIO work queue */
    /** @} */

    /** @name Socket Buffer Management
     * @{
     */
    struct sk_buff_head dnld_skb_head; /**< Download socket buffer queue head */
    struct sk_buff_head upld_skb_head; /**< Upload socket buffer queue head */
    /** @} */

    /** @name Synchronization and Control
     * @{
     */
    struct mutex lock;     /**< Mutual exclusion lock */
    atomic_t upld_int_flag; /**< Upload interrupt flag */
    /** @} */

    /** @name Transfer Completion Callbacks
     * @{
     */
    void *dnld_cplt_arg;        /**< Download completion callback argument */
    trans_cplt_cb dnld_cplt_cb; /**< Download completion callback function */
    void *upld_cplt_arg;        /**< Upload completion callback argument */
    trans_cplt_cb upld_cplt_cb; /**< Upload completion callback function */
    struct mutex upld_cplt_cb_mutex; /**< Upload completion callback registration/execution mutex */
    struct mutex dnld_cplt_cb_mutex; /**< Download completion callback registration/execution mutex */
    /** @} */

    /** @name Transfer Buffers
     * @{
     */
    uint8_t *dnld_trans_buf; /**< Download transfer buffer */
    uint8_t *upld_trans_buf; /**< Upload transfer buffer */
    /** @} */

    /** @name Associated Structures
     * @{
     */
    struct hd_sdio_card *sdio_card; /**< Associated SDIO card structure */
    struct hd_msg_ctrl *msg_ctrl;   /**< Associated message control structure */
    /** @} */
};

/** @defgroup SDIO_MANAGE_API SDIO Management API Functions
 * @{
 */

/**
 * @brief Initialize SDIO management structure
 * @param[out] sdio_manage Pointer to store created SDIO management structure
 * @param[in] sdio_card SDIO card structure to associate with
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_sdio_manage_init(struct hd_sdio_manage **sdio_manage, struct hd_sdio_card *sdio_card);

/**
 * @brief Deinitialize SDIO management structure
 * @param[in] sdio_manage SDIO management structure to cleanup
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_sdio_manage_deinit(struct hd_sdio_manage *sdio_manage);

/**
 * @brief Push socket buffer to download queue (asynchronous)
 * @param[in] sdio_manage SDIO management structure
 * @param[in] skb Socket buffer to queue for download
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_sdio_manage_dnld_send(struct hd_sdio_manage *sdio_manage, struct sk_buff *skb);

/**
 * @brief Pop socket buffer from upload queue
 * @param[in] sdio_manage SDIO management structure
 * @param[out] skb Pointer to store retrieved socket buffer
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_sdio_manage_upld_recv(struct hd_sdio_manage *sdio_manage, struct sk_buff **skb);

/**
 * @brief Unified callback registration for both upload and download directions
 * @param[in] sdio_manage SDIO management structure
 * @param[in] upld_recv_cb Upload receive callback function (can be NULL to unregister)
 * @param[in] upld_recv_arg User argument to pass to upload receive callback
 * @param[in] dnld_send_cb Download send completion callback function (can be NULL to unregister)
 * @param[in] dnld_send_arg User argument to pass to download send callback
 * @details This function provides a unified interface for registering both upload and download
 *          completion callbacks simultaneously, simplifying callback management between
 *          msg_ctrl and sdio_manage layers. Pass NULL for callback functions to unregister.
 */
void hd_sdio_manage_cplt_cb_register(struct hd_sdio_manage *sdio_manage, trans_cplt_cb upld_recv_cb, void *upld_recv_arg, trans_cplt_cb dnld_send_cb, void *dnld_send_arg);

/** @} */

#endif