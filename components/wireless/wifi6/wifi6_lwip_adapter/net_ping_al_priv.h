/**
 ****************************************************************************************
 *
 * @file fhost_ping.h
 *
 * @brief Definitions of the fully hosted ping task.
 *
 *
 ****************************************************************************************
 */
#ifndef _FHOST_PING_H_
#define _FHOST_PING_H_

/**
 ****************************************************************************************
 * @defgroup FHOST_PING FHOST_PING
 * @ingroup FHOST_IPC
 * @brief Ping task for fully hosted firmware
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "net_tg_al.h"
#include "net_tg_al_priv.h"
#include "rtos_al.h"

/*
 * DEFINITIONS
 ****************************************************************************************
 */
/// Arguments for @ref fhost_ping_start
struct fhost_ping_task_args
{
    ///Destination IP address
    uint32_t rip;
    ///Ping rate (pkts / s)
    uint32_t rate;
    ///Packet size
    uint32_t pksize;
    ///Ping duration
    uint32_t duration;
    ///Type of service
    uint16_t tos;
    ///Boolean for running ping in background
    bool background;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// Table of ping streams
extern struct fhost_ping_stream p_streams[FHOST_PING_MAX_STREAMS];

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Search ping stream by stream id
 * It will return the ping stream associated to the provided ping stream id.
 *
 * @param[in] stream_id  Ping stream id
 *
 * @return Pointer to ping stream if found, NULL otherwise
 ****************************************************************************************
 **/
struct fhost_ping_stream *fhost_ping_find_stream_profile(uint32_t stream_id);

/**
 ****************************************************************************************
 * @brief Start ping command with certain configuration options
 * This function creates the RTOS task dedicated to the ping send processing and return
 * the stream id of ping stream launched.
 *
 * @param[in] args  Pointer to arguments for fhost_ping_start (@ref fhost_ping_task_args)
 *
 * @return RTOS task handle of ping task on success, NULL otherwise
 ****************************************************************************************
 */
rtos_task_handle fhost_ping_start(void *args);

/**
 ****************************************************************************************
 * @brief Stop ping process launched
 * This function stops the ping process according to ping stream
 *
 * @param[in] ping_stream Pointer to the ping stream to stop
 ****************************************************************************************
 */
void fhost_ping_stop(struct fhost_ping_stream *ping_stream);


/**
 ****************************************************************************************
 * @brief Ping sigkill handler. Closes connection and ping stream.
 *
 * @param[in] ping_handle      Ping task handle
 *
 * @return FHOST_IPC_SUCCESS if successful, FHOST_IPC_ERROR otherwise
 ****************************************************************************************
 **/
int fhost_ping_sigkill_handler(rtos_task_handle ping_handle);

/// @}

#endif // _FHOST_PING_H_
