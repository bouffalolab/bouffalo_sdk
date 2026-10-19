/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "avutil/common.h"
#include "stream/stream.h"
#include "stream/stream_all.h"
#include "avformat/avformat_all.h"
#include "avformat/avformat.h"
#include "avcodec/avcodec_all.h"
#include "output/ao.h"
#include "output/output_all.h"
#include "aef/eq_all.h"
#include "aef/aef_all.h"
#include "atempo/atempo_all.h"
#include "swresample/resample_all.h"
#include "player_typedef.h"
#include "player_ioctl.h"

__BEGIN_DECLS__

typedef struct player_cb player_t;

/**
 * @brief  init player module default
 * attention: can rewrite this function by caller
 * @return 0/-1
 */
int player_init();

/**
 * @brief  init the player config param
 * @param  [in] ply_cnf
 * @return 0/-1
 */
int player_conf_init(ply_conf_t *ply_cnf);

/**
 * @brief  new a player obj
 * @param  [in] ply_cnf
 * @return NULL on error
 */
player_t* player_new(const ply_conf_t *ply_cnf);

/**
 * @brief  control/config the player by command
 * @param  [in] player
 * @param  [in] cmd    : PLAYER_CMD_XXX
 * @param  [in] ...
 * @return
 */
int player_ioctl(player_t *player, int cmd, ...);

/**
 * @brief  player play interface
 * @param  [in] player
 * @param  [in] url        : example: http://ip:port/xx.mp3
 * @param  [in] start_time : begin play time, ms
 * @return 0/-1
 */
int player_play(player_t *player, const char *url, uint64_t start_time);

/**
 * @brief  pause the player
 * @param  [in] player
 * @return 0/-1
 */
int player_pause(player_t *player);

/**
 * @brief  resume the player
 * @param  [in] player
 * @return 0/-1
 */
int player_resume(player_t *player);

/**
 * @brief  stop the player
 * @param  [in] player
 * @return 0/-1
 */
int player_stop(player_t *player);

/**
 * @brief  free/destroy the player obj
 * @param  [in] player
 * @return 0/-1
 */
int player_free(player_t *player);

/**
 * @brief  seek to the time
 * @param  [in] player
 * @param  [in] timestamp : seek time
 * @return 0/-1
 */
int player_seek(player_t *player, uint64_t timestamp);

/**
 * @brief  get current play time
 * @param  [in] player
 * @param  [in/out] ptime
 * @return 0/-1
 */
int player_get_cur_ptime(player_t *player, play_time_t *ptime);

/**
 * @brief  get media info
 * @param  [in] player
 * @param  [in/out] minfo
 * @return 0/-1
 */
int player_get_media_info(player_t *player, media_info_t *minfo);

/**
 * @brief  get soft vol index of the player
 * @param  [in] player
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int player_get_vol(player_t *player, uint8_t *vol);

/**
 * @brief  set soft vol index of the player
 * @param  [in] player
 * @param  [in/out] vol : vol scale index(0~255)
 * @return 0/-1
 */
int player_set_vol(player_t *player, uint8_t vol);

/**
 * @brief  get play speed of the player
 * @param  [in] player
 * @param  [out] speed
 * @return 0/-1
 */
int player_get_speed(player_t *player, float *speed);

/**
 * @brief  set play speed of the player
 * @param  [in] player
 * @param  [in] speed : [PLAY_SPEED_MIN ~ PLAY_SPEED_MAX]
 * @return 0/-1
 */
int player_set_speed(player_t *player, float speed);

/**
 * @brief  get play source url
 * @param  [in] player
 * @return NULL on error
 */
const char* player_get_url(player_t *player);

/**
 * @brief  get play demuxer
 * @param  [in] player
 * @param  [in/out] demuxer
 * @return 0
 */
void player_get_demuxer(player_t *player, demux_cls_t *demuxer);

__END_DECLS__

#endif /* __PLAYER_H__ */

