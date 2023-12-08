/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MIXER_CHANNEL_H__
#define __MIXER_CHANNEL_H__

#include "avutil/common.h"
#include "avutil/av_typedef.h"
#include "avutil/straight_fifo.h"
#ifdef __linux__
#include "xutils/list.h"
#endif

__BEGIN_DECLS__

#define MIXER_FIFO_SIZE_DEFAULT     (8*1024)

typedef struct mixer_struct         mixer_t;
typedef struct mixer_channel        mixer_cnl_t;

enum {
    MIXER_CNL_STATUS_UNKNOWN,
    MIXER_CNL_STATUS_RUNING,
    MIXER_CNL_STATUS_PAUSED,
};

struct mixer_channel {
    sf_t                            sf;
    sfifo_t                         *fifo;         ///< for mixer
    msp_slist_t                         node;
    mixer_t                         *mixer;
    uint8_t                         status;        ///< MIXER_CNL_STATUS_XXX
    uint8_t                         weof;           ///< write is finish
    msp_sem_t                       sem;
    msp_mutex_t                     lock;
};

/**
 * @brief  create one mixer-channel
 * @param  [in] sf
 * @param  [in] fsize : fifo size. 8 align
 * @return NULL on error
 */
mixer_cnl_t* mixer_cnl_new(sf_t sf, size_t fsize);

/**
 * @brief  write pcm data to the channel
 * @param  [in] cnl
 * @param  [in] buf
 * @param  [in] size
 * @param  [in] timeout : ms. 0 means no block, -1 means wait-forever
 * @return -1 on error
 */
int mixer_cnl_write(mixer_cnl_t *cnl, uint8_t *buf, size_t size, uint32_t timeout);

/**
 * @brief  get pcm size in the channel, not mix yet
 * @param  [in] cnl
 * @return -1 on error
 */
int mixer_cnl_get_size(mixer_cnl_t *cnl);

/**
* @brief  set write eof of the cnl
* @param  [in] cnl
* @param  [in] eof
* @return -1 on error
*/
int mixer_cnl_set_weof(mixer_cnl_t *cnl, uint8_t eof);

/**
 * @brief  get write eof of the cnl
 * @param  [in] cnl
 * @return 0/1, -1 on error
 */
int mixer_cnl_get_weof(mixer_cnl_t *cnl);

/**
 * @brief  wait drain-out of the channel
 * @param  [in] cnl
 * @return
 */
void mixer_cnl_drain_wait(mixer_cnl_t *cnl);

/**
 * @brief  send signal for drain-out already of the channel
 * @param  [in] cnl
 * @return
 */
void mixer_cnl_drain_signal(mixer_cnl_t *cnl);

/**
 * @brief  destroy the channel. need dettach from the mixer before destroy
 * @param  [in] cnl
 * @return 0/-1
 */
int mixer_cnl_free(mixer_cnl_t *cnl);

__END_DECLS__

#endif /* __MIXER_CHANNEL_H__ */

