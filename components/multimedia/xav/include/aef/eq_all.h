/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __EQ_ALL_H__
#define __EQ_ALL_H__

#include "avutil/common.h"

__BEGIN_DECLS__

#define REGISTER_EQXER(X, x)                                          \
    {                                                                 \
        extern int eqx_register_##x();                                \
        if (CONFIG_EQXER_##X)                                         \
            eqx_register_##x();                                       \
    }

/**
 * @brief  regist equalizer for silan
 * @return 0/-1
 */
int eqx_register_silan();

/**
 * @brief  regist equalizer for ipc
 * @return 0/-1
 */
int eqx_register_ipc();

/**
 * @brief  regist audio equalizer
 * @return 0/-1
 */
static inline int eqx_register()
{
#if defined(CONFIG_EQXER_SILAN)
    REGISTER_EQXER(SILAN, silan);
#endif
#if defined(CONFIG_EQXER_IPC)
    REGISTER_EQXER(IPC, ipc);
#endif
    return 0;
}

__END_DECLS__

#endif /* __EQ_ALL_H__ */

