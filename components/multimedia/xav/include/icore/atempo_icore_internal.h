/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ATEMPO_ICORE_INTERNAL_H__
#define __ATEMPO_ICORE_INTERNAL_H__

#include "icore/icore_msg.h"
#include "atempo/atempo_cls.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define ATEMPO_ICORE_AP_IDX          (0)
#define ATEMPO_ICORE_CP_IDX          (1)
#else
#define ATEMPO_ICORE_AP_IDX          (1)
#define ATEMPO_ICORE_CP_IDX          (2)
#endif

#define IPC_CMD_ATEMPO_ICORE         (62)
#define ATEMPO_ICORE_IPC_SERIVCE_ID  (0x17)

enum {
    ICORE_CMD_ATEMPO_INVALID,
    ICORE_CMD_ATEMPO_NEW,
    ICORE_CMD_ATEMPO_WRITE,
    ICORE_CMD_ATEMPO_READ,
    ICORE_CMD_ATEMPO_SET_SPEED,
    ICORE_CMD_ATEMPO_FLUSH,
    ICORE_CMD_ATEMPO_FREE,
};

typedef struct {
    uint32_t          rate;
    void              *atempo;      // resp: atempox_t
} atempo_icore_new_t;

typedef struct {
    void              *atempo;
    int16_t           *in;
    size_t            nb_samples;
} atempo_icore_write_t;

typedef struct {
    void              *atempo;
    int16_t           *out;
    size_t            nb_samples;
} atempo_icore_read_t;

typedef struct {
    void              *atempo;
    float             speed;
} atempo_icore_set_speed_t;

typedef struct {
    void      *atempo;
} atempo_icore_free_t;

typedef atempo_icore_free_t atempo_icore_flush_t;

__END_DECLS__

#endif /* __ATEMPO_ICORE_INTERNAL_H__ */

