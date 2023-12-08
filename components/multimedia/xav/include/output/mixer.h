/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MIXER_H__
#define __MIXER_H__

#include "output/mixer_channel.h"

__BEGIN_DECLS__

#define MIXER_READ_EVENT            (0x01)

typedef struct mixer_struct         mixer_t;

struct mix_buf {
    sfifo_t                         *fifo;          ///< ref pointer
    char                            *pos;           ///< read pos of sfifo for mix
};

struct mixer_struct {
    sf_t                            sf;
    uint8_t                         nb_cnls;        ///< nubmer of mixer-channel
    uint8_t                         nb_mbufs;       ///< nubmer of mix buf
    struct mix_buf                  *mbufs;         ///< array of mix buf
    msp_mutex_t                     lock;
#ifdef __linux__
    pthread_cond_t                  cond;
#else
    msp_event_t                     evt;
#endif
    msp_slist_t                         lists;
};

/**
 * @brief  create one mixer
 * @param  [in] sf
 * @return NULL on error
 */
mixer_t* mixer_new(sf_t sf);

/**
 * @brief  get the number of channels attached
 * @param  [in] mixer
 * @return -1 on error
 */
int mixer_get_nb_cnls(mixer_t *mixer);

/**
 * @brief  config sf before channels attached
 * @param  [in] mixer
 * @param  [in] sf
 * @return 0/-1
 */
int mixer_set_sf(mixer_t *mixer, sf_t sf);

/**
 * @brief  get the sf of mixer
 * @param  [in] mixer
 * @return
 */
sf_t mixer_get_sf(mixer_t *mixer);

/**
 * @brief  read pcm data from the mixer
 * @param  [in] mixer
 * @param  [in] buf
 * @param  [in] size
 * @param  [in] timeout : ms. 0 means no block, -1 means wait-forever
 * @return -1 on error
 */
int mixer_read(mixer_t *mixer, uint8_t *buf, size_t size, uint32_t timeout);

/**
 * @brief  signal channels which had empty&weof
 * @param  [in] mixer
 * @return 0/-1
 */
int mixer_drain_signal(mixer_t *mixer);

/**
 * @brief  whether had empty & weof of the mixer-channels
 * @param  [in] mixer
 * @return 0/1, -1 on error
 */
int mixer_check_empty_weof(mixer_t *mixer);

/**
 * @brief  attach channel to the mixer
 * @param  [in] mixer
 * @param  [in] cnl
 * @return 0/-1
 */
int mixer_attach(mixer_t *mixer, mixer_cnl_t *cnl);

/**
 * @brief  deattach channel from the mixer
 * @param  [in] mixer
 * @param  [in] cnl
 * @return 0/-1
 */
int mixer_dettach(mixer_t *mixer, mixer_cnl_t *cnl);

/**
 * @brief  set channel status
 * @param  [in] mixer
 * @param  [in] cnl
 * @param  [in] status : MIXER_CNL_STATUS_XX
 * @return -1 on error
 */
int mixer_set_cnl_status(mixer_t *mixer, mixer_cnl_t *cnl, int status);

/**
 * @brief  destroy the mixer. need dettach all mixer-channel
 * @param  [in] mixer
 * @return 0/-1
 */
int mixer_free(mixer_t *mixer);

__END_DECLS__

#endif /* __MIXER_H__ */

