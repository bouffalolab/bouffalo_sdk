/**
 ****************************************************************************************
 *
 * @file net_al.h
 *
 * @brief Declaration of the networking stack abstraction layer.
 * The functions declared here shall be implemented in the networking stack and call the
 * corresponding functions.
 *
 *
 ****************************************************************************************
 */

#ifndef NET_AL_H_
#define NET_AL_H_

/**
 ****************************************************************************************
 * @defgroup FHOST_NET_AL FHOST_NET_AL
 * @ingroup FHOST_AL
 * @brief Network stack interface
 *
 * The Fully Hosted firmware requires an implementation of the functions described here.
 *
 * A file net_def.h must also be provided and include definition of the following types:
 * - @b net_al_if_t: Network stack internal representation of a interface. FHOST module simply
 *                keep a reference to such structure it is never dereferenced and as such
 *                doesn't need to be fully defined.
 * - @b net_al_rx_t: Network stack header for a RX buffer (i.e. doesn't contains data,
 *                    just a pointer to it). It is allocated (but not initialized) by the
 *                    FHOST module and must then be fully defined.
 * - @b net_al_tx_t: Network stack header for a TX buffer (i.e. doesn't contains data,
 *                    just a pointer to it). It is never dereferenced (@ref net_buf_tx_info
 *                    is used to read it) and as such doesn't need to be fully defined in
 *                    net_def.h
 *
 * <b>TX path: </b>\n
 * The network stack should push buffer to the FHOST firmware using the function
 * @ref fhost_tx_start. The buffers pushed should contains a IEEE 802.3 (aka ethernet)
 * MAC header. The FHOST module will take care to replace it by a IEEE 802.11 MAC header.\n
 * Each buffer allocated for TX shall be allocated with at least @ref NET_AL_TX_HEADROOM
 * bytes available in the buffer headroom. \n
 * The payload of a TX buffer may be split on several segments (up to 3) but the first
 * segment must at least contains the full IEEE 802.3 header and the headroom.\n
 * Each data segment allocated for TX shall also be allocated in SHARED memory. Indeed
 * The FHOST module uses @ref net_buf_tx_info to retrieve data segment addresses and length
 * and passes this information 'directly' to the MACHW for transmission.
 *
 * <b>RX path: </b>\n
 * When a buffer is received by the FHOST module, the IEEE 802.11 MAC header is replaced
 * by a IEEE 802.3 MAC header and then the buffer is pushed to the Network stack using
 * @ref net_if_input.
 *
 * <b>IP related functions: </b>\n
 * There are a few functions that are only needed if @ref fhost_set_vif_ip or
 * @ref fhost_get_vif_ip function of fhost_api will be used by the integration layer.
 * If this is not the case then there is no need to implemment them and the macro
 * @p NET_AL_NO_IP should be defined in net_def.h.
 *
 * The optional functions are:
 * - net_if_set_default
 * - net_if_set_ip
 * - net_if_get_ip
 * - net_dhcp_start
 * - net_dhcp_stop
 * - net_dhcp_release
 * - net_dhcp_address_obtained
 * - net_set_dns
 * - net_get_dns
 *
 * @{
 ****************************************************************************************
 */
#include <stdint.h>
#include <stdbool.h>
#include "net_def.h"

// abstract layer
typedef void *  net_al_if_t;
typedef void *  net_al_rx_t;
typedef void *  net_al_tx_t;

/// Minimum headroom to include in all TX buffer
#define NET_AL_TX_HEADROOM 384

/// Prototype for a function to free a network buffer */
typedef void (*net_buf_free_fn)(void *net_buf);

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Call the checksum computation function of the TCP/IP stack
 *
 * @param[in] dataptr Pointer to the data buffer on which the checksum is computed
 * @param[in] len Length of the data buffer
 *
 * @return The computed checksum
 ****************************************************************************************
 */
uint16_t net_ip_chksum(const void *dataptr, int len);

/**
 ****************************************************************************************
 * @brief Add a network interface
 *
 * This function must initialize the provided net_al_if_t structure.
 * The private VIF structure must be the one returned by @ref net_if_vif_info
 *
 * @param[in] net_if    Pointer to the net_if structure to add
 * @param[in] mac_addr  MAC address of the interface
 * @param[in] ipaddr    IPv4 address of the interface (NULL if not available)
 * @param[in] netmask   Net mask of the interface (NULL if not available)
 * @param[in] gw        Gateway address of the interface (NULL if not available)
 * @param[in] vif_priv  Pointer to the VIF private structure
 *
 * @return 0 on success and != 0 if error occurred
 ****************************************************************************************
 */
int net_if_add(net_al_if_t *net_if, const uint8_t *mac_addr, const uint32_t *ipaddr,
               const uint32_t *netmask, const uint32_t *gw, void *vif_priv);

/**
 ****************************************************************************************
 * @brief Get network interface MAC address
 *
 * @param[in] net_if Pointer to the net_if structure
 *
 * @return Pointer to interface MAC address
 ****************************************************************************************
 */
const uint8_t *net_if_get_mac_addr(net_al_if_t net_if);

/**
 ****************************************************************************************
 * @brief Get pointer to network interface from its name
 *
 * @param[in] name Name of the interface
 *
 * @return pointer to the net_if structure and NULL if such interface doesn't exist.
 ****************************************************************************************
 */
net_al_if_t net_if_find_from_name(const char *name);

/**
 ****************************************************************************************
 * @brief Get name of network interface
 *
 * Copy the name on the interface (including a terminating a null byte) in the given
 * buffer. If buffer is not big enough then the interface name is truncated and no
 * null byte is written in the buffer.
 *
 * @param[in] net_if  Pointer to the net_if structure
 * @param[in] buf     Buffer to write the interface name
 * @param[in] len     Length of the buffer.
 *
 * @return  < 0 if error occurred, otherwise the number of characters (excluding the
 * terminating null byte) needed to write the interface name. If return value is greater
 * or equal to @p len, it means that the interface name has been truncated
 ****************************************************************************************
 */
int net_if_get_name(net_al_if_t net_if, char *buf, int len);

/**
 ****************************************************************************************
 * @brief Indicate that the network interface is now up (i.e. able to do traffic)
 *
 * @param[in] net_if Pointer to the net_if structure
 ****************************************************************************************
 */
void net_if_up_cb(net_al_if_t net_if);

/**
 ****************************************************************************************
 * @brief Indicate that the network interface is now down
 *
 * @param[in] net_if Pointer to the net_if structure
 ****************************************************************************************
 */
void net_if_down_cb(net_al_if_t net_if);

/**
 ****************************************************************************************
 * @brief Call the networking stack input function.
 * This function is supposed to link the payload data and length to the RX buffer
 * structure passed as parameter. The free_fn function shall be called when the networking
 * stack is not using the buffer anymore.
 ****************************************************************************************
 */
void net_al_input(net_al_rx_t net_buf, void *payload,
                  net_al_if_t net_if,
                  uint16_t length,
                  uint8_t offset,
                  uint8_t skip_after_eth_hdr,
		  net_buf_free_fn free_fn);

/**
 ****************************************************************************************
 * @brief Get the pointer to the VIF private structure attached to a net interface.
 *
 * @param[in] net_if Pointer to the net_if structure
 *
 * @return The pointer to the VIF private structure attached to the net interface when it
 * has been initialized (@ref net_if_add)
 ****************************************************************************************
 */
void *net_if_vif_info(net_al_if_t net_if);

 /**
  ****************************************************************************************
  * @brief Allocate a buffer for TX.
  *
  * This function is used to transmit buffer that do not originate from the Network Stack.
  * (e.g. a management frame sent by wpa_supplicant)
  * This function allocates a buffer for transmission. The buffer must still reserve
  * @ref NET_AL_TX_HEADROOM headroom space like for regular TX buffers.
  *
  * @param[in] length   Size, in bytes, of the payload
  *
  * @return The pointer to the allocated TX buffer and NULL if allocation failed
  ****************************************************************************************
  */
net_al_tx_t net_buf_tx_alloc(uint32_t length);
net_al_tx_t net_buf_tx_alloc_fill(const uint8_t *frame, uint32_t length);

/**
 ****************************************************************************************
 * some tx, rx buffer handle hook
 ****************************************************************************************
 */
struct net_al_tx_req {
    net_al_tx_t net_buf;
    void *cfm_cb;
    void *cfm_cb_arg;
    void *buf_rx;
    net_al_if_t net_if;
    int type;
    int no_cck;
};
int net_al_tx_req(struct net_al_tx_req req);
void net_al_rx_resend(bool forward,
                      void *buf,
                      void *payload,
                      int length,
                      int offset,
                      int mac_hdr_len);

/**
 ****************************************************************************************
 * some hook from fhost
 ****************************************************************************************
 */
typedef void (* release_buf_cb)(net_al_tx_t net_buf, uint32_t tx_status,
                                void *cfm_cb, void *cfm_cb_arg,
                                void *buf_rx);
void net_al_tx_cfm();
void net_al_tx_do_sta_del(uint8_t sta_id, release_buf_cb release_buf);
void net_al_tx_init();

/**
 ****************************************************************************************
 * @brief Allocate a buffer for TX with a reference to a payload.
 *
 * This function allocates a buffer for transmission. It only allocates memory for the
 * TX buffer structure. The pointer to the payload is set to NULL and should be updated
 *
 * @param[in] length   Size, in bytes, of the payload
 *
 * @return The pointer to the allocated TX buffer and NULL if allocation failed
 ****************************************************************************************
 */
net_al_tx_t net_buf_tx_alloc_ref(uint32_t length);

/**
 ****************************************************************************************
 * @brief Provides information on a TX buffer.
 *
 * This function is used by the FHOST module before queuing the buffer for transmission.
 * This function must returns information (pointer and length) on all data segments that
 * compose the buffer. Each buffer must at least have one data segment.
 * It must also return a pointer to the headroom (of size @ref NET_AL_TX_HEADROOM)
 * which must be reserved for each TX buffer on their first data segment.
 *
 * @param[in]     buf       Pointer to the TX buffer structure
 * @param[in]     tot_len   Total size in bytes on the buffer (includes size of all data
 *                          segment)
 * @param[in,out] seg_cnt   Contains the maximum number of data segment supported (i.e.
 *                          the size of @p seg_addr and @p seg_len parameter) and must be
 *                          updated with the actual number of segment in this buffer.
 * @param[out]    seg_addr  Table to retrieve the address of each segment.
 * @param[out]    seg_len   Table to retrieve the length, in bytes, of each segment.
 * @param[out]    headroom_len Headroom len.
 *
 * @return The pointer to the headroom reserved at the beginning of the first data segment
 ****************************************************************************************
 */
void *net_buf_tx_info(net_al_tx_t *net_buf, uint16_t *tot_len, int *seg_cnt,
                      uint32_t seg_addr[], uint16_t seg_len[], uint32_t *headroom_len);

/**
 ****************************************************************************************
 * @brief Returns true if payloads of TX buffer are all located in SHARERAM.
 ****************************************************************************************
 */
bool net_buf_tx_all_shram(net_al_tx_t net_buf);
/**
 ****************************************************************************************
 * @brief Free a TX buffer that was involved in a transmission.
 *
 * @param[in] buf Pointer to the TX buffer structure
 ****************************************************************************************
 */
void net_buf_tx_free(net_al_tx_t buf);

/**
 ****************************************************************************************
 * @brief Initialize the networking stack
 *
 * Once the initialization is complete (i.e. the associated RTOS has started) the
 * function @ref fhost_task_ready must be called with @p IP_TASK as parameter.
 *
 * @return 0 on success and != 0 if packet is not accepted
 ****************************************************************************************
 */
int net_init(void);

/**
 ****************************************************************************************
 * @brief Send a L2 (aka ethernet) packet
 *
 * Send data on the link layer (L2). If destination address is not NULL, Ethernet header
 * will be added (using ethertype parameter) and MAC address of the sending interface is
 * used as source address. If destination address is NULL, it means that ethernet header
 * is already present and frame should be send as is.
 * The data buffer will be copied by this function, and must then be freed by the caller.
 *
 * The primary purpose of this function is to allow the supplicant sending EAPOL frames.
 * As these frames are often followed by addition/deletion of crypto keys, that
 * can cause encryption to be enabled/disabled in the MAC, it is required to ensure that
 * the packet transmission is completed before proceeding to the key setting.
 * This function shall therefore be blocking until the frame has been transmitted by the
 * MAC.
 *
 * @param[in] net_if    Pointer to the net_if structure.
 * @param[in] data      Data buffer to send.
 * @param[in] data_len  Buffer size, in bytes.
 * @param[in] ethertype Ethernet type to set in the ethernet header. (in host endianess)
 * @param[in] dst_addr  Ethernet address of the destination. If NULL then it means that
 *                      ethernet header is already present in the frame (and in this case
 *                      ethertype should be ignored)
 * @param[out] ack      Optional to get transmission status. If not NULL, the value
 *                      pointed is set to true if peer acknowledged the transmission and
 *                      false in all other cases.
 *
 * @return 0 on success and != 0 if packet hasn't been sent
 ****************************************************************************************
 */
int net_l2_send(net_al_if_t net_if, const uint8_t *data, int data_len, uint16_t ethertype,
                const uint8_t *dst_addr, bool *ack);

/**
 ****************************************************************************************
 * @brief Create a L2 (aka ethernet) socket for specific packet
 *
 * Create a L2 socket that will receive specified frames: a given ethertype on a given
 * interface.
 * It is expected to fail if a L2 socket for the same ethertype/interface couple already
 * exists.
 *
 * @note As L2 sockets are not specified in POSIX standard, the implementation of such
 * function may be impossible in some network stack.
 *
 * @param[in] net_if    Pointer to the net_if structure.
 * @param[in] ethertype Ethernet type to filter. (in host endianess)
 *
 * @return <0 if error occurred and the socket descriptor otherwise.
 ****************************************************************************************
 */
int net_l2_socket_create(net_al_if_t net_if, uint16_t ethertype);

/**
 ****************************************************************************************
 * @brief Delete a L2 (aka ethernet) socket
 *
 * @param[in] sock Socket descriptor returned by @ref net_l2_socket_create
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 **/
int net_l2_socket_delete(void);

/**
 ****************************************************************************************
 * @brief Concatenate 2 Tx buffers
 *
 * @param[in] buf Pointer to the TX buffer structure 1
 * @param[in] buf Pointer to the TX buffer structure 2
 ****************************************************************************************
 */

void net_buf_tx_cat(net_al_tx_t net_buf_tx_1, net_al_tx_t net_buf_tx_2);

#endif // NET_AL_H_
/**
 * @}
 */
