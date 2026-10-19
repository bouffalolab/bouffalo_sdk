/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#if defined(CONFIG_AV_DBUS_SUPPORT) && CONFIG_AV_DBUS_SUPPORT
#ifndef __DBUS_UTILS_H__
#define __DBUS_UTILS_H__

#include "avutil/common.h"
#include "media_typedef.h"
#include "avdbus/media_typedef_dbus.h"
#include <dbus/dbus.h>

__BEGIN_DECLS__

/**
 * @brief  get event name by id
 * @param  [in] evt_id
 * @return NULL on error
 */
const char *get_name_by_evtid(aui_player_evtid_t evt_id);

__END_DECLS__

#endif /* __DBUS_UTILS_H__ */

#endif

