/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef MSP_TYPES_H
#define MSP_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#ifdef CONFIG_CHIP_BL606P_E907
#include <fcntl.h>
#endif 

#ifdef __cplusplus
extern "C" {
#endif

#define MSP_EXPORT(...)

#ifdef __cplusplus
}
#endif

#if !defined(__BEGIN_DECLS__) || !defined(__END_DECLS__)

#if defined(__cplusplus)
#  define __BEGIN_DECLS__ extern "C" {
#  define __END_DECLS__   }
#else
#  define __BEGIN_DECLS__
#  define __END_DECLS__
#endif

#endif


#endif /* MSP_TYPES_H */
