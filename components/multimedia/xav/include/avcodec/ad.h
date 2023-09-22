/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AD_H__
#define __AD_H__

#include "avutil/common.h"
#include "avutil/av_typedef.h"
#include "avcodec/ad_cls.h"

__BEGIN_DECLS__

#define REGISTER_DECODE(X, x)                                         \
    {                                                                 \
        extern struct ad_ops ad_ops_##x;                              \
        if (CONFIG_DECODE_##X)                                        \
            ad_ops_register(&ad_ops_##x);                             \
    }

#define AD_OPS_MAX            (16)

typedef struct ad_conf {
    sf_t                      sf;             ///< if needed
    uint8_t                   *extradata;     ///< extradata or frame header, if needed
    int32_t                   extradata_size;
    uint32_t                  block_align;    ///< size of one frame, if needed
    uint32_t                  bps;            ///< bps, if needed
} ad_conf_t;

/**
 * @brief  regist ad ops
 * @param  [in] ops
 * @return 0/-1
 */
int ad_ops_register(const struct ad_ops *ops);

/**
* @brief  init the ad config param
* @param  [in] ad_cnf
* @return 0/-1
*/
int ad_conf_init(ad_conf_t *ad_cnf);

/**
 * @brief  open/create one audio decoder
 * @param  [in] id
 * @param  [in] ad_cnf
 * @return NULL on err
 */
ad_cls_t* ad_open(avcodec_id_t id, const ad_conf_t *ad_cnf);

/**
 * @brief  decode one audio frame
 * @param  [in] o
 * @param  [out] frame : frame of pcm
 * @param  [out] got_frame : whether decode one frame
 * @param  [in]  pkt : data of encoded audio
 * @return -1 when err happens, otherwise the number of bytes consumed from the
 *         input avpacket is returned
 */
int ad_decode(ad_cls_t *o, avframe_t *frame, int *got_frame, const avpacket_t *pkt);

/**
 * @brief  control a audio decoder
 * @param  [in] o
 * @param  [in] cmd : command
 * @param  [in] arg
 * @param  [in/out] arg_size
 * @return 0/-1
 */
int ad_control(ad_cls_t *o, int cmd, void *arg, size_t *arg_size);

/**
 * @brief  reset the audio decoder
 * @param  [in] o
 * @return 0/-1
 */
int ad_reset(ad_cls_t *o);

/**
 * @brief  close/destroy audio decoder
 * @param  [in] o
 * @return 0/-1
 */
int ad_close(ad_cls_t *o);

__END_DECLS__

#endif /* __AD_H__ */

