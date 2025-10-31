/**
 ****************************************************************************************
 *
 * @file net_iperf_al.h
 *
 * @brief Declaration of the networking stack abstraction layer used for iperf.
 * The functions declared here shall be implemented in the networking stack and call the
 * corresponding functions.
 *
 * Copyright (C) RivieraWaves 2017-2021
 *
 ****************************************************************************************
 */

#ifndef NET_IPERF_AL_H_
#define NET_IPERF_AL_H_

/*
 * DEFINITIONS
 ****************************************************************************************
 */
// Forward declarations of iperf stream
struct fhost_iperf_stream;

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/*
 * FUNCTIONS
 ****************************************************************************************
 */
#define net_al_if_t struct netif*
#define inet_buf_tx_t struct pbuf

/// Interface types
typedef enum
{
    /// STA interface
    MGMR_VIF_STA = 0,
    /// AP interface
    MGMR_VIF_AP
};

//#define fhost_to_net_if(x) 

#define FHOST_IPERF_STACK_SIZE (4096)
#define FHOST_IPERF_PRIORITY   (28)
#define fhost_print(x, fmt, ...) printf(fmt, ##__VA_ARGS__)
#define ASSERT_ERR
#define co_ntohl    ntohl
#define co_htonl    htonl

/**
 ****************************************************************************************
 * @brief Open UDP connection as client and connect to UDP server.
 * Initialize UDP transmission and send UDP datagram to server. After sending a burst of
 * UDP frames, wait in order to meet bandwidth constraints.
 * The last UDP datagram needs to have a negative packet id to indicate the server that
 * transmission is over.
 *
 * @param[in] iperf_stream_ptr  Pointer to iperf stream
 * @return  0 if successful, -1 otherwise.
 ****************************************************************************************
 **/
int net_iperf_udp_client_run(struct fhost_iperf_stream* iperf_stream_ptr);

/**
 ****************************************************************************************
 * @brief Open UDP connection as server and listen to UDP port. Set packet reception
 * callback function to handle UDP packets. When a packet with ID < 0 is
 * received, the server report is sent to the client and statistics are printed.
 *
 * @param[in] iperf_stream_ptr  Pointer to iperf stream
 * @return  0 if successful, -1 otherwise.
 ****************************************************************************************
 **/
int net_iperf_udp_server_run(struct fhost_iperf_stream* iperf_stream_ptr);

/**
 ****************************************************************************************
 * @brief Open TCP connection as server and listen to TCP port. Wait for TCP traffic
 * to end before returning
 *
 * @param[in] iperf_stream_ptr  Pointer to iperf stream
 * @return  0 if successful, -1 otherwise.
 ****************************************************************************************
 **/
int net_iperf_tcp_server_run(struct fhost_iperf_stream* iperf_stream_ptr);

/**
 ****************************************************************************************
 * @brief Open UDP connection as client and connect to UDP server.
 * Initialize UDP transmission and send UDP datagram to server. After sending a burst of
 * UDP frames, wait in order to meet bandwidth constraints.
 * The last UDP datagram needs to have a negative packet id to indicate the server that
 * transmission is over.
 *
 * @param[in] stream  Pointer to iperf stream
 * @return  0 if successful, -1 otherwise.
 ****************************************************************************************
 **/
int net_iperf_tcp_client_run(struct fhost_iperf_stream* stream);

/**
 ****************************************************************************************
 * @brief TCP closing function. Remove TCP callbacks, print final stats,
 * close TCP protocol control block and wake-up iperf task.
 *
 * @param[in] stream  Pointer to iperf stream
 ****************************************************************************************
 **/
void net_iperf_tcp_close(struct fhost_iperf_stream *stream);
#endif //NET_IPERF_AL_H_
