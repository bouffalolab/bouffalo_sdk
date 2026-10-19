/**
 ****************************************************************************************
 *
 * @file fhost_tg.h
 *
 * @brief Definitions of the fully hosted TG (traffic generator) task.
 *
 *
 ****************************************************************************************
 */

#ifndef _FHOST_TG_H_
#define _FHOST_TG_H_

/**
 ****************************************************************************************
 * @defgroup FHOST_TG FHOST_TG
 * @ingroup FHOST_IPC
 * @brief TG task for fully hosted firmware
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "fhost_ipc.h"
#include "lwip/tcpip.h"
#include "rtos_al.h"
#include "net_tg_al.h"

#include "assert.h"
#undef TRACE_APP
#define TRACE_APP(err, ...) {printf(__VA_ARGS__); assert(0);}

/*
 * DEFINITIONS
 ****************************************************************************************
 */
/// Number of traffic stream
#define FHOST_TG_MAX_TRAFFIC_STREAMS 4
/// Number of TG send buffers (credits)
#define FHOST_TG_SEND_BUF_CNT        8

/// Traffic sending direction
#define FHOST_TG_DIRECT_SEND                1
/// Traffic receiving direction
#define FHOST_TG_DIRECT_RECV                2

/// TG profile ID
enum profile_id
{
    /// Profile Types
    /// Large file transfer between endpoints
    FHOST_PROF_FILE_TX = 1,
    /// A group/multicast traffic Real Audio stream
    FHOST_PROF_MCAST,
    /// IPTV stream
    FHOST_PROF_IPTV,
    /// A series of client / server transactions
    FHOST_PROF_TRANSC,
    /// NOT USED
    FHOST_PROF_START_SYNC,
    /// NOT USED
    FHOST_PROF_CALI_RTD = FHOST_PROF_START_SYNC,
    /// NOT USED
    FHOST_PROF_UAPSD,
    /// NOT USED
    FHOST_PROF_LAST
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// TG profile
struct fhost_tg_profile
{
     /// Profile ID
    enum profile_id prof_id;
    /// Traffic direction
    int direction;
    /// Remote IP address
    uint32_t remote_ip;
    /// Remote port
    uint32_t remote_port;
    /// Local IP address
    uint32_t local_ip;
    /// Local port
    uint32_t local_port;
    /// Traffic rate (pkts /s)
    uint32_t rate;
    /// Packet size (bytes)
    uint32_t pksize;
    /// Traffic duration (seconds)
    uint32_t duration;
    /// TOS value to be put in the IP header
    uint16_t tos;
};

/// TG statistics
struct fhost_tg_stats
{
    /// Number of TX frames sent
    uint32_t tx_frames;
    /// Number of RX frames received
    uint32_t rx_frames;
    /// Counter for send command
    uint32_t counter_send;
    /// Counter for recv command
    uint32_t counter_recv;
    /// Number of TX buffer bytes
    uint32_t tx_bytes;
    /// Number of RX buffer bytes
    uint32_t rx_bytes;
    /// Traffic throughput
    uint32_t throughput;
    /// Number of expected frames
    uint32_t expected;
    /// Number of lost frames
    uint32_t lost;
    /// Number of unordered frames
    uint32_t unordered;
    /// Round trip time delay of transaction
    uint32_t rt_time;
};

/// TG stream
struct fhost_tg_stream
{
    /// Stream ID
    uint32_t id;
    /// State of TG stream (true for active, false for inactive)
    bool active;
    /// TG semaphore used to synchronize the TG thread and the FHOST IPC one
    rtos_semaphore tg_semaphore;
    /// NET_TG Semaphore used to synchronize the process in the net_tg_al layer
    rtos_semaphore net_tg_semaphore;
    /// Handle of TG send task (not used yet)
    rtos_task_handle tg_handle;
    /// TG mutex used when we modify the credits for sending process
    rtos_mutex tg_mutex;
    /// Param pointer for NET TG AL
    void *arg;
    /// Credits for TG sending buffer
    int8_t credits;
    /// Timestamp used for transaction
    struct tg_time transc_timestamp;
    /// TG profile
    struct fhost_tg_profile prof;
    /// TG statistics
    struct fhost_tg_stats stats;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// Table of TG streams
extern struct fhost_tg_stream g_streams[FHOST_TG_MAX_TRAFFIC_STREAMS];

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Start TG stream configuration
 * This function configures the TG stream with certain characteristics.
 *
 * @param[in] stream_id    TG stream ID
 * @param[in] prof         TG profile ID
 * @param[in] direction    Direction of stream (DIRECT_SEND, DIRECT_RECV)
 * @param[in] rip          Remote IP address
 * @param[in] remote_port  Remote port
 * @param[in] lip          Local IP address
 * @param[in] local_port   Local port
 * @param[in] rate         Traffic rate (pkts / s)
 * @param[in] pksize       Packet size
 * @param[in] duration     Traffic duration
 * @param[in] tos          Type of service
 *
 * @return 0 on success and !=0 otherwise
 ****************************************************************************************
 */
int fhost_tg_config(uint32_t stream_id, uint32_t prof, int direction, uint32_t rip,
                    uint32_t remote_port, uint32_t lip, uint32_t local_port, uint32_t rate,
                    uint32_t pksize, uint32_t duration, uint16_t tos);

/**
 ****************************************************************************************
 * @brief Start TG traffic for sending or receiving process according to the direction
 * of TG stream
 * If it's sending direction :
 * This function creates the RTOS task dedicated to the TG send processing.
 * We can send several TG streams at the same time.
 * If it's receiving direction :
 * This function starts the receiving server dedicated to the TG recv processing.
 * We can also receive several TG streams at the same time.
 *
 * @param[in] stream_id    TG stream ID
 *
 * @return 0 on success and !=0 otherwise
 ****************************************************************************************
 */
int fhost_tg_start(uint32_t stream_id);

/**
 ****************************************************************************************
 * @brief Start TG stop task
 * This function stops the TG sending or receiving process according to TG stream ID
 * and return the statistics
 *
 * @param[in] stream_id    TG stream ID
 *
 * @return pointer to the statistics structure of stream, NULL if error
 ****************************************************************************************
 */
struct fhost_tg_stats *fhost_tg_stop(uint32_t stream_id);


/*
 * DEFINITIONS
 ****************************************************************************************
 */
/// Maximum number of ping streams
#define FHOST_PING_MAX_STREAMS 3
/// Profile ID for ping command
#define FHOST_PROF_PING 7

/// Ping profile
struct fhost_ping_profile
{
    /// IP address of destination
    uint32_t dst_ip;
    /// Ping rate (pkts /s)
    uint32_t rate;
    /// Packet size
    uint32_t pksize;
    /// Ping duration
    uint32_t duration;
    /// TOS value to be put in the IP header
    uint16_t tos;
};

/// Ping statistics
struct fhost_ping_stats
{
    /// Number of TX frames sent
    uint32_t tx_frames;
    /// Number of RX frames received
    uint32_t rx_frames;
    /// Number of TX buffer bytes
    uint32_t tx_bytes;
    /// Number of RX buffer bytes
    uint32_t rx_bytes;
    /// Round trip delay (in us) of ping command
    uint32_t rt_time;
    /// Epoch time of the first ping request sent
    struct tg_time first_msg;
    /// Epoch time of the last ping reply received or last request sent
    /// (whichever occurs last)
    struct tg_time last_msg;
    /// Duration (in us) between first request and last reply
    uint32_t time;
};

/// Ping configuration structure
struct fhost_ping_stream
{
    /// Ping thread ID
    uint32_t id;
    /// State of ping thread (true for active, false for inactive)
    bool active;
    /// Param pointer for the net_tg_al layer
    void *arg;
    /// Ping sequence number
    uint16_t ping_seq_num;
    /// Ping latest reply received
    uint16_t ping_reply_num;
    /// Ping semaphore used to synchronize the ping thread and the FHOST IPC one
    rtos_semaphore ping_semaphore;
    /// Handle of ping send task (not used yet)
    rtos_task_handle ping_handle;
    /// Ping timestamp
    struct tg_time ping_timestamp;
    /// Ping profile
    struct fhost_ping_profile prof;
    /// Ping statistics
    struct fhost_ping_stats stats;
    /// Background task
    bool background;
};

/// @}

#endif // _FHOST_TG_H_

