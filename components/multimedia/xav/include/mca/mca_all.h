/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MCA_ALL_H__
#define __MCA_ALL_H__

#include "avutil/common.h"
#include "avutil/av_config.h"

__BEGIN_DECLS__

#define REGISTER_MCAXER(X, x)                                          \
    {                                                                  \
        extern int mcax_register_##x();                                \
        if (CONFIG_MCAXER_##X)                                         \
            mcax_register_##x();                                       \
    }


/**
 * @brief  regist mca for local
 * @return 0/-1
 */
int mcax_register_local();

/**
 * @brief  regist mca for ipc
 * @return 0/-1
 */
int mcax_register_ipc();

/**
 * @brief  regist mca
 * @return 0/-1
 */
static inline int mcax_register()
{
#if defined(CONFIG_MCAXER_LOCAL)
    REGISTER_MCAXER(LOCAL, local);
#endif
#if defined(CONFIG_MCAXER_IPC)
    REGISTER_MCAXER(IPC, ipc);
#endif
    return 0;
}

__END_DECLS__

#endif /* __MCA_ALL_H__ */

