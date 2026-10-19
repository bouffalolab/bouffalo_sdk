
/**
 ****************************************************************************************
 *
 * @file ipc_compat.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */


#ifndef _IPC_H_
#define _IPC_H_

#include "bflb_os_private.h"

#if 1
#define __WARN()        bflb_os_printf("%s:%d\r\n", __func__, __LINE__)
#else
#define __WARN()        
#endif

#define WARN_ON(condition) ({                       \
    int __ret_warn_on = !!(condition);              \
    if (__ret_warn_on)                              \
        __WARN();                                   \
    __ret_warn_on;                                  \
    })

#define WARN_ON_ONCE(condition) ({          \
    static bool __warned;                   \
    int __ret_warn_once = !!(condition);    \
                                            \
    if (__ret_warn_once)                    \
        if (WARN_ON(!__warned))             \
            __warned = true;                \
    __ret_warn_once;                        \
    })

#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~__round_mask(x, y))

#if 1
#define ASSERT_ERR(condition)   assert(condition)
#else
#define ASSERT_ERR(condition) RT_ASSERT(condition)
#endif

#endif /* _IPC_H_ */
