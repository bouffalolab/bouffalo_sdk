/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AEF_ICORE_INTERNAL_H__
#define __AEF_ICORE_INTERNAL_H__

#include "icore/icore_msg.h"
#include "aef/aef_cls.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define AEF_ICORE_AP_IDX          (0)
#define AEF_ICORE_CP_IDX          (1)
#else
#define AEF_ICORE_AP_IDX          (1)
#define AEF_ICORE_CP_IDX          (2)
#endif

#define IPC_CMD_AEF_ICORE         (59)
#define AEF_ICORE_IPC_SERIVCE_ID  (0x14)

enum {
    ICORE_CMD_AEF_INVALID,
    ICORE_CMD_AEF_NEW,
    ICORE_CMD_AEF_PROCESS,
    ICORE_CMD_AEF_FREE,
};

typedef struct {
    uint32_t          rate;
    uint8_t           *conf;
    size_t            conf_size;
    size_t            nsamples_max;
    void              *aef;      // resp: aefx_t
} aef_icore_new_t;

typedef struct {
    void              *aef;
    int16_t           *in;
    int16_t           *out;
    size_t            nb_samples;
} aef_icore_process_t;

typedef struct {
    void      *aef;
} aef_icore_free_t;

__END_DECLS__

#endif /* __AEF_ICORE_INTERNAL_H__ */

