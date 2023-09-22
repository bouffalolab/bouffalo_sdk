/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AD_ICORE_H__
#define __AD_ICORE_H__

#include "avutil/avframe.h"
#include "avutil/avpacket.h"
#include "icore/ad_icore_typedef.h"

__BEGIN_DECLS__

typedef struct ad_icore ad_icore_t;

/**
 * @brief  init the icore audio decoder
 * @return 0/-1
 */
int ad_icore_init();

/**
* @brief  init the ad_icore config param
* @param  [in] ad_cnf
* @return 0/-1
*/
int ad_icore_conf_init(adi_conf_t *adi_cnf);

/**
 * @brief  open/create one audio remote-ipc decoder
 * @param  [in] adi_cnf
 * @return NULL on error
 */
ad_icore_t* ad_icore_open(avcodec_id_t id, const adi_conf_t *adi_cnf);

/**
* @brief  get the audio sample format
* @param  [in] hdl
* @param  [out] sf
* @return 0/-1
*/
int ad_icore_get_sf(ad_icore_t *hdl, sf_t *sf);

/**
 * @brief  decode one audio frame
 * @param  [in] hdl
 * @param  [out] frame : frame of pcm
 * @param  [out] got_frame : whether decode one frame
 * @param  [in]  pkt : data of encoded audio
 * @return -1 when err happens, otherwise the number of bytes consumed from the
 *         input avpacket is returned
 */
int ad_icore_decode(ad_icore_t *hdl, avframe_t *frame, int *got_frame, const avpacket_t *pkt);

/**
 * @brief  reset the ad_icore
 * @param  [in] hdl
 * @return 0/-1
 */
int ad_icore_reset(ad_icore_t *hdl);

/**
 * @brief  close/destroy audio remote-ipc decoder
 * @param  [in] hdl
 * @return 0/-1
 */
int ad_icore_close(ad_icore_t *hdl);

__END_DECLS__

#endif /* __AD_ICORE_H__ */

