/**
 ****************************************************************************************
 *
 * @file fhost.h
 *
 * @brief Definitions of the fully hosted module.
 *
 *
 ****************************************************************************************
 */

#ifndef _FHOST_H_
#define _FHOST_H_

/**
 ****************************************************************************************
 * @defgroup FHOST FHOST
 * @ingroup MACSW
 * @brief Fully Hosted module implementation.
 * This module creates different RTOS tasks that will be used to handle the operations
 * of the fully-hosted FW
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdarg.h>
#include <stdint.h>
#include <sys/queue.h>

#include "macsw.h"

#define VERSION_FHOST_NUMBER "1.7.10"
#define VERSION_FHOST_MAJOR 1
#define VERSION_FHOST_MINOR 7
#define VERSION_FHOST_PATCH 10

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif
uint32_t dbg_vsnprintf_offset(char *buffer, uint32_t size, uint32_t offset, const char *fmt, va_list args);
#define dbg_vsnprintf(buffer, size, fmt, args) dbg_vsnprintf_offset(buffer, size, 0, fmt, args)

#include "rtos_al.h"
#include "net_al.h"
#include "fhost_api.h"

#define fhost_printf(...) printf(__VA_ARGS__)

#define INVARIANTS

#define INVALID_VIF_IDX 0xFF

/// Number of RX buffers
#if MACSW_AMPDU_RX
#ifndef FHOST_RX_BUF_CNT
#ifndef CFG_ADHOC_ENABLE
#define FHOST_RX_BUF_CNT            (MACSW_MAX_BA_RX * MACSW_AMPDU_RX_BUF_SIZE + 2)
#else
#define FHOST_RX_BUF_CNT            (MACSW_AMPDU_RX_BUF_SIZE + 2)
#endif
#endif
#else
#define FHOST_RX_BUF_CNT             4
#endif

/*
 * DEFINITIONS
 ****************************************************************************************
 */

/// Type of packet to send
enum fhost_tx_buf_type
{
    /// Packet with an ethernet header (from the network stack)
    IEEE802_3,
    /// Packet with a wifi header
    IEEE802_11,
};

/// Type of messages sent using @ref fhost_msg structure
enum fhost_msg_type {
    /**
     * Message received from IPC layer (for test/development only)
     */
    FHOST_MSG_IPC,
    /**
     * Messages received from WIFI stack
     */
    FHOST_MSG_KE_WIFI,
    /**
     * Configuration/control messages received from supplicant application
     */
    FHOST_MSG_CFGMACSW,
};

enum fhost_sta_state {
    FHOST_STA_DISCONNECTED = 0,
    FHOST_STA_SCANNING,
    FHOST_STA_CONNECTING,
    FHOST_STA_4WAY_HANDSHAKE,
    FHOST_STA_GROUP_HANDSHAKE,
    FHOST_STA_CONNECTED,
};

enum fhost_AP_state {
    FHOST_AP_DISABLED = 0,
    FHOST_AP_SCANNING,
    FHOST_AP_ENABLED,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
#if MACSW_MESH_EN
/// struct fhost_mesh_path - Mesh Path information
struct fhost_mesh_path
{
    /// Chained list element
    struct co_list_hdr hdr;
    /// Path index
    uint8_t path_idx;
    /// MAC Address it the path target
    struct mac_addr tgt_mac_addr;
    /// Next Hop STA in the path
    uint8_t nhop_sta;
};
#endif

/// Structure containing the information about the virtual interfaces
struct fhost_vif_tag
{
    /// RTOS network interface structure
    net_al_if_t net_if;
    /// MAC address of the VIF
    struct mac_addr mac_addr;
    /// is scanning
    bool scanning;
    /// XXX if the socket used for IPC
    bool is_cntrl_link;
    /// Pointer to the MAC VIF structure
    void *mac_vif; /* struct vif_info_tag *mac_vif; */
    /// Index of the STA being the AP peer of the device - TODO rework
    uint8_t ap_id;
    /// Parameter to indicate if admission control is mandatory for any access category - TODO rework
    uint8_t acm;
    /// UAPSD queue config for STA interface (bitfield, same format as QoS info)
    uint8_t uapsd_queues;
    /// Isolation Mode - Only used for AP
    bool isolation_mode;
    #if MACSW_MESH_EN
    /// List of mpath
    struct co_list mpath_list;
    /// List of available memory nodes
    struct co_list free_mpath_list;
    /// Mesh Path Information Pool
    struct fhost_mesh_path fhost_mesh_path_pool[MACSW_MESH_PATH_NB];
    /// Whether a frame is being resent on this interface
    bool is_resending;
    #endif
    /// Raw state from wpa
    int raw_state;
    /// State
    int state;
    /// wps state
    bool wps_state;
};

/// Structure used for the inter-task communication
struct fhost_env_tag
{
    /// Table of RTOS network interface structures
    struct fhost_vif_tag vif[CFG_VIF_MAX];
    /// Table linking the MAC VIFs to the FHOST VIFs
    struct fhost_vif_tag *mac2fhost_vif[CFG_VIF_MAX];
};

/// Generate fhost msg ID from a type and an index
#define FHOST_MSG_ID(type, idx) ((type << 12) | (idx & 0xfff))
/// Extract msg Type from msg ID
#define FHOST_MSG_TYPE(id) ((id >> 12) & 0xf)
/// Extract msg Index from msg ID
#define FHOST_MSG_INDEX(id) (id & 0xfff)

/// Generic Message format
struct fhost_msg {
    STAILQ_ENTRY(fhost_msg) entries;
    /// ID of the message. Id is a combination of a type and an index.
    /// To be set using @ref FHOST_MSG_ID macro
    uint16_t id;
    /// Length, in bytes, of the message
    uint16_t len;
    /// Pointer to the message
    void *data;
};

/// Rate Control Information
union fhost_rc_rate_ctrl_info {
    struct {
        /// MCS Index
        uint32_t mcs_idx            : 7;
        /// Bandwidth
        uint32_t bw_tx              : 2;
        /// Guard Interval and Pre Type
        uint32_t gi_and_pre_type_tx : 2;
        /// Format Mod
        uint32_t format_mod_tx      : 3;
        /// DCM
        uint32_t dcm_tx             : 1;
    };
    /// Rate Control info value
    uint32_t value;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// FullHost module environment variable
extern struct fhost_env_tag fhost_env;


void fhost_wakeup(void);

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Get MAC VIF owned by a FHOST VIF.
 *
 * @param[in] fhost_vif_idx Index of the FHOST VIF
 *
 * @return A pointer to the corresponding MAC VIF
 ****************************************************************************************
 */
void *fhost_to_mac_vif(uint8_t fhost_vif_idx);


/**
 ****************************************************************************************
 * @brief Get FHOST VIF owner of a MAC VIF.
 *
 * @param[in] mac_vif_idx Index of the MAC VIF
 *
 * @return A pointer to the corresponding FHOST VIF
 ****************************************************************************************
 */
struct fhost_vif_tag *fhost_from_mac_vif(uint8_t mac_vif_idx);

/**
 ****************************************************************************************
 * @brief Get Network interface associated to a FHOST VIF.
 *
 * @param[in] fhost_vif_idx Index of the FHOST VIF
 *
 * @return A pointer to the corresponding network interface
 ****************************************************************************************
 */
net_al_if_t fhost_to_net_if(uint8_t fhost_vif_idx);

/**
 ****************************************************************************************
 * @brief Get FHOST VIF name.
 *
 * Copy name of a FHOST VIF inside provided buffer including a terminating a null byte.
 * If the buffer is not big enough then interface name is truncated and the null byte
 * is not written in the buffer.
 *
 * @param[in] fhost_vif_idx  Index of the FHOST VIF
 * @param[in] name           Buffer to retrieve interface name
 * @param[in] len            Size, in bytes, of the @p name buffer
 *
 * @return < 0 if error occurred, otherwise the number of characters (excluding the
 * terminating null byte) needed to write the interface name. If return value is greater
 * or equal to @p len, it means that the interface name has been truncated.
 ****************************************************************************************
 */
int fhost_vif_name(int fhost_vif_idx, char *name, int len);

/**
 ****************************************************************************************
 * @brief Get FHOST VIF index from its name.
 *
 * @param[in] name Interface name
 *
 * @return index of the fhost vif and < 0 if there is no interface with the provided name
 ****************************************************************************************
 */
int fhost_vif_idx_from_name(const char *name);

/**
 ****************************************************************************************
 * @brief Configure default queues enabled for U-APSD.
 *
 * This configuration is used when an interface configured as STA connects to an AP that
 * supports U-APSD. This can be called at any time, but the configuration will only be
 * applied for the next connections.
 *
 * @param[in] fhost_vif_idx  Index of the FHOST VIF. (Use -1 to configure all interfaces)
 * @param[in] uapsd_queues   AC bitfield as expected in Qos Info field
 *                           (i.e. Bit0=VO, Bit1=VI, Bit2=BK, bit3=BE)
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int fhost_vif_set_uapsd_queues(int fhost_vif_idx, uint8_t uapsd_queues);

/**
 ****************************************************************************************
 * @brief Initialization of the application.
 *
 * Called during the initialization procedure (i.e. when RTOS scheduler is not yet active).
 * Implementation of this function will depends of the final application and in most
 * cases it will create one of several application tasks and their required communication
 * interface (queue, semaphore, ...)
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int fhost_application_init(void);

/**
 ****************************************************************************************
 * @brief Print a pre-formatted string buffer.
 *
 * Implementation of this function will depend of target/application
 *
 * @param[in] handle Task handle of the RTOS task sending the message
 *                   (can be null to indicate current task)
 * @param[in] buf    Formatted string buffer
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int fhost_print_buf(rtos_task_handle handle, const char *buf);

/**
 ****************************************************************************************
 * @brief Print a message.
 *
 * Implementation of this function will depend of target/application
 *
 * @param[in] handle Task handle of the RTOS task sending the message
 *                   (can be null to indicate current task)
 * @param[in] fmt    Format string
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int fhost_print(rtos_task_handle handle, const char *fmt, ...);

/**
 ****************************************************************************************
 * @brief Compute checksum on a given IP packet
 *
 * Compute IP packet checksum using HSU and fallback to software computation if HSU is
 * unavailable
 *
 * @param[in] dataptr Buffer containing the IP packet
 * @param[in] len     Buffer length (in bytes)
 * @return IP packet checksum
 ****************************************************************************************
 */
uint16_t fhost_ip_chksum(const void *dataptr, int len);

/**
 ****************************************************************************************
 * @brief Connect a STA interface to an AP
 *
 * This is blocking until connection is successful.
 *
 * @param[in] fhost_vif_idx  Fhost VIF index
 * @param[in] cfg            Interface configuration
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int fhost_sta_cfg(int fhost_vif_idx, struct fhost_vif_sta_cfg *cfg);

/**
 ****************************************************************************************
 * @brief Start an Access Point
 *
 * This is blocking until AP is operational.
 *
 * @param[in] fhost_vif_idx  Fhost VIF index
 * @param[in] cfg            AP configuration
 * @param[in] mesh_mode      Enable MESH mode
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int fhost_ap_cfg(int fhost_vif_idx, struct fhost_vif_ap_cfg *cfg, bool mesh_mode);

/**
 ****************************************************************************************
 * @brief Retrieve list of Stations connected to a certain VIF
 *
 * @param[in] fhost_vif         Virtual Interface Information
 * @param[in] table_max_size    Maximum size of the idx_table
 * @param[out] idx_table        Table to fill with the Station indexes
 *
 * @return Size of the idx_table
 ****************************************************************************************
 */
int fhost_get_sta_idx(struct fhost_vif_tag *fhost_vif, uint8_t table_max_size,
                      uint8_t *idx_table);

int fhost_vif_is_up(int fhost_vif_idx);

/// @}

#endif // _FHOST_H_
