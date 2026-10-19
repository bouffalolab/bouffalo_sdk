#ifndef __SDIOWIFI_MGMR_TYPE_H__
#define __SDIOWIFI_MGMR_TYPE_H__

#include <sdiowifi_securec_wrap.h>
#include <stdbool.h>
#include <string.h>
#include <sdiowifi_platform_adapt.h>
#include <lwip/netif.h>
//#include <bl_wifi.h>
#include <wifi_mgmr_ext.h>
//#include <vfs.h>

#include <rnm_server.h>

#include "net_wifi_transceiver.h"
#include <sdiowifi_config.h>
#include <sdiowifi_tx_buf.h>

typedef struct sdiowifi_mgmr {
    bool init;

    rnms_t *rnm;

    net_wifi_trcver_t trcver;

    struct sdiowifi_tx_buf tx_desc;
    sdiowifi_timer_handle_t ping_timer;
} sdiowifi_mgmr_t;

#endif /* __SDIOWIFI_MGMR_H__ */
