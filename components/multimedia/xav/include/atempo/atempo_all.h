/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ATEMPO_ALL_H__
#define __ATEMPO_ALL_H__

#include "avutil/common.h"
#include "avutil/av_config.h"

__BEGIN_DECLS__

#define REGISTER_ATEMPOER(X, x)                                          \
    {                                                                    \
        extern int atempo_register_##x();                                \
        if (CONFIG_ATEMPOER_##X)                                         \
            atempo_register_##x();                                       \
    }

/**
 * @brief  regist audio atempoer for sonic
 * @return 0/-1
 */
int atempo_register_sonic();

/**
 * @brief  regist audio atempoer for ipc
 * @return 0/-1
 */
int atempo_register_ipc();

/**
 * @brief  regist audio atempoer
 * @return 0/-1
 */
int atempo_register();

__END_DECLS__

#endif /* __ATEMPO_ALL_H__ */

