/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __PLAYER_IOCTL_H__
#define __PLAYER_IOCTL_H__

#include "stream/stream_ioctl.h"
#include "output/ao_ioctl.h"

__BEGIN_DECLS__

enum {
    PLAYER_CMD_UNKNOWN,
    PLAYER_CMD_EQ_SET_PARAM,       ///< if support
    PLAYER_CMD_EQ_ENABLE,          ///< if support
    PLAYER_CMD_SET_CACHE_SIZE,     ///< next play valid
    PLAYER_CMD_SET_RESAMPLE_RATE,  ///< next play valid
    PLAYER_CMD_SET_RCVTO,          ///< next play valid
};

/* equalizer typedef below */
//PLAYER_CMD_EQ_SET_PARAM
typedef oeq_setpa_t peq_setpa_t;

//PLAYER_CMD_EQ_SET_ENABLE
typedef oeq_seten_t peq_seten_t;

__END_DECLS__

#endif /* __PLAYER_IOCTL_H__ */

