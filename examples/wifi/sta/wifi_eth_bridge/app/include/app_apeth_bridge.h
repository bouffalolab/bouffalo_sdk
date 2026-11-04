#ifndef __APP_APETH_BRIDGE_H__
#define __APP_APETH_BRIDGE_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

struct netif;

int app_apeth_bridge_init(void);
int app_emac_init(void);
struct netif *app_emac_netif(void);
bool app_emac_physical_link_up(void);
int app_wds_bridge_init(void);
void app_wds_bridge_wifi_ready(void);
void app_wds_bridge_poll(void);
void app_wds_bridge_radio_up(bool ap);
void app_wds_bridge_radio_down(bool ap);

#endif
