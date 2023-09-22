/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __RES_ICORE_INTERNAL_H__
#define __RES_ICORE_INTERNAL_H__

#include "icore/icore_msg.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define RES_ICORE_AP_IDX          (0)
#define RES_ICORE_CP_IDX          (1)
#else
#define RES_ICORE_AP_IDX          (1)
#define RES_ICORE_CP_IDX          (2)
#endif

#define IPC_CMD_RES_ICORE         (58)
#define RES_ICORE_IPC_SERIVCE_ID  (0x13)

enum {
    ICORE_CMD_RES_INVALID,
    ICORE_CMD_RES_NEW,
    ICORE_CMD_RES_GET_OSAMPLES,
    ICORE_CMD_RES_CONVERT,
    ICORE_CMD_RES_FREE,
};

typedef struct {
    uint32_t          irate;
    uint32_t          orate;
    uint8_t           bits;
    uint8_t           channels;
    void              *r;      // resp: resx_t
} res_icore_new_t;

typedef struct {
    uint32_t          irate;
    uint32_t          orate;
    size_t            nb_isamples;
} res_icore_getosa_t;

typedef struct {
    void              *r;
    void              *in;
    void              *out;
    size_t            nb_isamples;
    size_t            nb_osamples;
} res_icore_convert_t;

typedef struct {
    void      *r;
} res_icore_free_t;

__END_DECLS__

#endif /* __RES_ICORE_INTERNAL_H__ */

