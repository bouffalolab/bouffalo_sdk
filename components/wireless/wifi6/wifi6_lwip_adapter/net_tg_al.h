/**
 ****************************************************************************************
 *
 * @file net_tg_al.h
 *
 * @brief Declaration of the networking stack abstraction layer used for traffic generator.
 * The functions declared here shall be implemented in the networking stack and call the
 * corresponding functions.
 *
 *
 ****************************************************************************************
 */

#ifndef NET_TG_AL_H_
#define NET_TG_AL_H_

/*
 * DEFINITIONS
 ****************************************************************************************
 */
// Forward declarations
struct fhost_tg_stream;
struct fhost_ping_stream;

/// Time structure
struct tg_time
{
    /// second
    uint32_t sec;
    /// microsecond
    uint32_t usec;
};

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Send UDP packet to a destination IP address without allocating memory for buffer,
 * used for @ref FHOST_PROF_FILE_TX.
 *
 * This function will send datagram to specific IP address and port.
 * The packet to send is divided into two parts :
 *      - TG header : it includes header message, counter, and time stamp
 *      - TG buffer : it contains a pointer which is pointed to a static memory zone
 * These two parts are chained together as a whole packet.
 * The interval time of sending is limited to 20 ms referring to net_tg_sleep_time()
 *
 * @param[in] stream  Pointer to TG Stream
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 **/
int net_tg_send(struct fhost_tg_stream *stream);

/**
 ****************************************************************************************
 * @brief Start receiving UDP packet from specific sender configured in PCB, used for
 * @ref FHOST_PROF_FILE_TX.
 *
 * This function will bind the local address, connect to the remote address and configure
 * the callback function which will be called every time there's datagram from the
 * configured PCB
 *
 * @param[in] stream  Pointer to TG Stream
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 **/
int net_tg_recv_start(struct fhost_tg_stream *stream);

/**
 ****************************************************************************************
 * @brief Stop receiving UDP packet from specific PCB, used for @ref FHOST_PROF_FILE_TX.
 *
 * This function will free the UDP PCB of the receiving callback function so that it will
 * not be called.
 *
 * @param[in] stream  Pointer to TG Stream
 ****************************************************************************************
 **/
void net_tg_recv_stop(struct fhost_tg_stream *stream);

/**
 ****************************************************************************************
 * @brief Start transaction traffic between two DUT, used for @ref FHOST_PROF_TRANSC.
 *
 * @param[in] stream  Pointer to TG Stream
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 **/
int net_tg_send_transc(struct fhost_tg_stream *stream);

/**
 ****************************************************************************************
 * @brief Start receiving UDP packet from specific sender configured in PCB, used for
 * @ref FHOST_PROF_TRANSC.
 *
 * This function will bind the local address, connect to the remote address and configure
 * the callback function which will be called every time there's datagram from the
 * configured PCB.
 *
 * @param[in] stream  Pointer to TG Stream
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 **/
int net_tg_recv_transc_start(struct fhost_tg_stream *stream);

/**
 ****************************************************************************************
 * @brief Ping a remote IP address
 *
 * This function will send a ICMP ECHO packet to the remote IP address wait for the
 * response.
 *
 * @param[in] stream  Pointer to ping Stream
 *
 * @return 0 on success, < 0 if fatal error occurred and the number of transmittion that
 * have been skipped otherwise (e.g. because of lack of memory).
 ****************************************************************************************
 **/
int net_ping_send(struct fhost_ping_stream *stream);

/**
 ****************************************************************************************
 * @brief Calculate the time difference
 *
 * @param[in] t1  TG time structure
 * @param[in] t2  TG time structure
 *
 * @return the time difference t2 - t1 in us (could be negative)
 ****************************************************************************************
 **/
int net_time_diff(struct tg_time *t1, struct tg_time *t2);

#endif // NET_TG_AL_H_
