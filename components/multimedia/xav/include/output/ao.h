/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AO_H__
#define __AO_H__

#include "avutil/common.h"
#include "avutil/av_typedef.h"
#include "output/ao_cls.h"
#include "output/ao_ioctl.h"

__BEGIN_DECLS__

#define AO_OPS_MAX            (6)

typedef struct ao_conf {
    char                      *name;         ///< ao name
    uint32_t                  period_ms;     ///< period cache size(ms) for audio out. 0 means use default
    uint32_t                  period_num;    ///< number of period_ms. total cache size for ao is (period_num * period_ms * (rate / 1000) * 2 * (16/8)). 0 means use default
    uint8_t                   eq_segments;   ///< equalizer segments number. 0 means don't need eq
    uint8_t                   *aef_conf;     ///< config data for aef
    size_t                    aef_conf_size; ///< size of the config data for aef
    uint32_t                  resample_rate; ///< none zereo means need to resample
    uint8_t                   vol_en;        ///< soft vol scale enable
    uint8_t                   vol_index;     ///< soft vol scale index (0~255)
    uint8_t                   atempo_play_en;///< atempo play enable
    float                     speed;         ///< atempo play speed.suggest: 0.5 ~ 2.0;

    int32_t                   db_min;        ///< min db for digital volume curve
    int32_t                   db_max;        ///< max db for digital volume curve
} ao_conf_t;

/**
 * @brief  regist ao ops
 * @param  [in] ops
 * @return 0/-1
 */
int ao_ops_register(const struct ao_ops *ops);

/**
 * @brief  init the ao config param
 * @param  [in] ao_cnf
 * @return 0/-1
 */
int ao_conf_init(ao_conf_t *ao_cnf);

/**
 * @brief  open a audio out
 * @param  [in] sf     : audio sample format
 * @param  [in] ao_cnf
 * @return NULL on error
 */
ao_cls_t* ao_open(sf_t sf, const ao_conf_t *ao_cnf);

/**
 * @brief  control the ao
 * @param  [in] o
 * @param  [in] cmd : command, see AO_CMD_XX
 * @param  [in] arg
 * @param  [in/out] arg_size
 * @return
 */
int ao_control(ao_cls_t *o, int cmd, void *arg, size_t *arg_size);

/**
 * @brief  start audio out
 * @param  [in] o
 * @return 0/-1
 */
int ao_start(ao_cls_t *o);

/**
 * @brief  stop audio out
 * @param  [in] o
 * @return 0/-1
 */
int ao_stop(ao_cls_t *o);

/**
 * @brief  write data to audio out
 * @param  [in] o
 * @param  [in] buf
 * @param  [in] count
 * @return -1 when err
 */
int ao_write(ao_cls_t *o, const uint8_t *buf, size_t count);

/**
 * @brief  drain out the tail pcm
 * @param  [in] o
 * @return 0/-1
 */
int ao_drain(ao_cls_t *o);

/**
 * @brief  close/destroy a audio out
 * @param  [in] o
 * @return 0/-1
 */
int ao_close(ao_cls_t *o);

__END_DECLS__

#endif /* __AO_H__ */

