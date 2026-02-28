#ifndef _NETDEV_STA_H_
#define _NETDEV_STA_H_

#include "mr_netdev.h"

#include <stdio.h>
#include <nethub.h>
#include <nethub_types.h>

/* NETDEV Channel Configuration. */
#define NETDEV_NETHUB_CNT        (4)   /**< Total frame buffer count */
#define NETDEV_NETHUB_FRAME_SIZE (128) /**< Frame buffer size */

/** NETDEV channel instance */
extern mr_netdev_priv_t *g_netdev_wifi_priv;

int netdev_wifi_init(mr_msg_ctrl_priv_t *msg_ctrl);

int netdev_wifi_upld(nh_skb_t *skb, void *arg);

#endif