/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __EQ_ICORE_INTERNAL_H__
#define __EQ_ICORE_INTERNAL_H__

#include "icore/icore_msg.h"
#include "aef/eq_cls.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define EQ_ICORE_AP_IDX          (0)
#define EQ_ICORE_CP_IDX          (1)
#else
#define EQ_ICORE_AP_IDX          (1)
#define EQ_ICORE_CP_IDX          (2)
#endif

#define IPC_CMD_EQ_ICORE         (57)
#define EQ_ICORE_IPC_SERIVCE_ID  (0x12)

enum {
    ICORE_CMD_EQ_INVALID,
    ICORE_CMD_EQ_NEW,
    ICORE_CMD_EQ_SET_PARAM,
    ICORE_CMD_EQ_SET_ENABLE,
    ICORE_CMD_EQ_PROCESS,
    ICORE_CMD_EQ_FREE,
};

typedef struct {
    uint32_t          rate;
    uint8_t           eq_segments;
    void              *eq;      // resp: eqx_t
} eq_icore_new_t;

typedef struct {
    void              *eq;      // eqx_t
    uint8_t           segid;
    eqfp_t            param;
} eq_icore_setpa_t;

typedef struct {
    void              *eq;
    uint8_t           enable;
} eq_icore_seten_t;

typedef struct {
    void              *eq;
    int16_t           *in;
    int16_t           *out;
    size_t            nb_samples;
} eq_icore_process_t;

typedef struct {
    void      *eq;
} eq_icore_free_t;

__END_DECLS__

#endif /* __EQ_ICORE_INTERNAL_H__ */

