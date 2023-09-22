/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __XPLAYER_ALL_H__
#define __XPLAYER_ALL_H__

#include "avutil/common.h"
#include "avutil/av_config.h"

__BEGIN_DECLS__

#define REGISTER_PLAYER(X, x)                                          \
    {                                                                    \
        extern int xplayer_register_##x();                                \
        if (CONFIG_PLAYER_##X)                                         \
            xplayer_register_##x();                                       \
    }

/**
 * @brief  regist xplayer for yoc
 * @return 0/-1
 */
int xplayer_register_yoc();

/**
 * @brief  regist xplayer for allwinner
 * @return 0/-1
 */
int xplayer_register_tplayer();

/**
 * @brief  regist xplayer
 * @return 0/-1
 */
static inline int xplayer_register()
{
#if defined(CONFIG_PLAYER_YOC)
    REGISTER_PLAYER(YOC, yoc);
#endif
#if defined(CONFIG_PLAYER_TPLAYER)
    REGISTER_PLAYER(TPLAYER, tplayer);
#endif

    return 0;
}

__END_DECLS__

#endif /* __XPLAYER_ALL_H__ */

