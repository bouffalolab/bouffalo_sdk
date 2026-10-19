#ifndef NETHUB_NETDEV_WIFI_H
#define NETHUB_NETDEV_WIFI_H

#include "mr_netdev.h"
#include "nh_types.h"

#include <stdio.h>

#define NETDEV_NETHUB_CNT        (4)
#define NETDEV_NETHUB_FRAME_SIZE (128)

extern mr_netdev_priv_t *g_netdev_wifi_priv;

int netdev_wifi_init(mr_msg_ctrl_priv_t *msg_ctrl);
int netdev_wifi_upld(nethub_frame_t *frame, void *arg);

#endif
