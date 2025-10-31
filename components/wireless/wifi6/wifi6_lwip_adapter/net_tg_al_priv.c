 /**
 ****************************************************************************************
 *
 * @file fhost_tg.c
 *
 * @brief Definition of TG task for Fully Hosted firmware.
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup FHOST_TG
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <string.h>
#include "fhost.h"

#include "net_tg_al_priv.h"
#include "net_tg_al.h"

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// Table of TG streams
struct fhost_tg_stream g_streams[FHOST_TG_MAX_TRAFFIC_STREAMS];

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/// main function of TG send task
static RTOS_TASK_FCT(tg_send_main)
{
    struct fhost_tg_stream *stream = env;
    // Wait for semaphore from fhost_tg_start to ensure the flag active is at 1
    rtos_semaphore_wait(stream->tg_semaphore, -1);

    switch (stream->prof.prof_id)
    {
        case FHOST_PROF_IPTV :
        case FHOST_PROF_FILE_TX :
        case FHOST_PROF_MCAST :
            net_tg_send(stream);
            break;
        case FHOST_PROF_TRANSC :
            net_tg_send_transc(stream);
            break;
        default :
            TRACE_APP(ERR, "[TG] Invalid profile for TX");
            break;
    }
    // Signal the semaphore to fhost_tg_stop to ensure that we have finished sending
    rtos_semaphore_signal(stream->tg_semaphore, false);

    rtos_task_delete(NULL);
}

/**
 ****************************************************************************************
 * @brief Search existing TG stream profile by stream id
 *
 * @param[in] stream_id    ID of TG stream
 *
 * @return pointer to TG stream, NULL if error
 ****************************************************************************************
 **/
static struct fhost_tg_stream *fhost_tg_find_stream_profile(uint32_t stream_id)
{
    if (stream_id < FHOST_TG_MAX_TRAFFIC_STREAMS)
    {
        return &g_streams[stream_id];
    }
    else
    {
        return NULL;
    }
}

int fhost_tg_config(uint32_t stream_id, uint32_t prof, int direction, uint32_t rip,
                    uint32_t remote_port, uint32_t lip, uint32_t local_port, uint32_t rate,
                    uint32_t pksize, uint32_t duration, uint16_t tos)
{
    struct fhost_tg_stream *stream = fhost_tg_find_stream_profile(stream_id);
    if (stream == NULL)
    {
        fhost_printf("Invalid stream id");
        return 1;
    }

    if (stream->active)
    {
        fhost_printf("Stream %d is already active", stream_id);
        return 1;
    }

    if ((direction != FHOST_TG_DIRECT_SEND) && (direction != FHOST_TG_DIRECT_RECV))
    {
        fhost_printf("Invalid direction");
        return 1;
    }

    if (prof >= FHOST_PROF_LAST)
    {
        fhost_printf("Invalid profile");
        return 1;
    }

    memset(stream, 0, sizeof(struct fhost_tg_stream));

    stream->id = stream_id;
    stream->prof.prof_id = prof;
    stream->prof.direction = direction;
    stream->prof.remote_ip = rip;
    stream->prof.remote_port = remote_port;
    stream->prof.local_ip = lip;
    stream->prof.local_port = local_port;
    stream->prof.rate = rate;
    stream->prof.pksize = pksize;
    stream->prof.duration = duration;
    stream->prof.tos = tos;

    return 0;
}

int fhost_tg_start(uint32_t stream_id)
{
    struct fhost_tg_stream *stream = fhost_tg_find_stream_profile(stream_id);
    if (stream == NULL)
    {
        fhost_printf("Invalid stream id");
        return 1;
    }

    if (stream->active)
    {
        fhost_printf("Stream %d is already active", stream_id);
        return 1;
    }

    memset(&(stream->stats), 0, sizeof(struct fhost_tg_stats));

    if (stream->prof.direction == FHOST_TG_DIRECT_SEND)
    {
        if (rtos_semaphore_create(&stream->tg_semaphore, 1, 0))
            return 1;

        if (rtos_semaphore_create(&stream->net_tg_semaphore, 1, 0))
        {
            rtos_semaphore_delete(stream->tg_semaphore);
            return 1;
        }
        if (rtos_mutex_create(&stream->tg_mutex))
        {
            rtos_semaphore_delete(stream->tg_semaphore);
            rtos_semaphore_delete(stream->net_tg_semaphore);
            return 1;
        }
        if (rtos_task_create(tg_send_main, "TG_SEND", TG_SEND_TASK,
                            FHOST_TG_SEND_STACK_SIZE, stream, fhost_tg_priority,
                            &stream->tg_handle))
        {
            rtos_semaphore_delete(stream->tg_semaphore);
            rtos_semaphore_delete(stream->net_tg_semaphore);
            rtos_mutex_delete(stream->tg_mutex);
            return 1;
        }
        stream->active = true;
        stream->credits = FHOST_TG_SEND_BUF_CNT;
        // Signal the semaphore to tg_send_main to ensure the flag active is at 1
        rtos_semaphore_signal(stream->tg_semaphore, false);
        return 0;
    }
    else if (stream->prof.direction == FHOST_TG_DIRECT_RECV)
    {
        switch (stream->prof.prof_id)
        {
            case FHOST_PROF_IPTV :
            case FHOST_PROF_FILE_TX :
            case FHOST_PROF_MCAST :
                net_tg_recv_start(stream);
                break;
            case FHOST_PROF_TRANSC :
                net_tg_recv_transc_start(stream);
                break;
            default :
                fhost_printf("Invalid profile for RX");
                return 1;
        }
        stream->active = true;
        return 0;
    }
    else
    {
        fhost_printf("Invalid stream direction");
        return 1;
    }
}

struct fhost_tg_stats *fhost_tg_stop(uint32_t stream_id)
{
    struct fhost_tg_stream *stream = fhost_tg_find_stream_profile(stream_id);
    if (stream == NULL)
    {
         fhost_printf("Invalid stream id");
         return NULL;
    }

    if (!stream->active)
    {
        fhost_printf("Stream %d is not active", stream_id);
        return NULL;
    }

    stream->active = false;
    if (stream->prof.direction == FHOST_TG_DIRECT_SEND)
    {
        // Wait for the TG task to finish its processing
        rtos_semaphore_wait(stream->tg_semaphore, -1);
        rtos_semaphore_delete(stream->tg_semaphore);
        rtos_mutex_delete(stream->tg_mutex);
        rtos_semaphore_delete(stream->net_tg_semaphore);
    }
    else if (stream->prof.direction == FHOST_TG_DIRECT_RECV)
    {
        net_tg_recv_stop(stream);
    }
    return &(stream->stats);
}

/// @}
