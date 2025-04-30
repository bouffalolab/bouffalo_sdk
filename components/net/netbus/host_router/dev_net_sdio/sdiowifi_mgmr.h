#ifndef __SDIOWIFI_MGMR_H__
#define __SDIOWIFI_MGMR_H__

#include <stdbool.h>

//#include <sdiowifi_platform_adapt.h>
#include <lwip/netif.h>

//#include <rnm_server.h>

#include <sdiowifi_config.h>
#include <sdiowifi_utils.h>
//#include <net_wifi_transceiver.h>

#include "trcver_sdio.h"
#include "sdiowifi_tx_buf.h"

#define SDIOWIFI_VERSION 0xAB001234

enum {
    SDIOWIFI_MGMR_MSG_TYPE_CMD = 0x8000,
    SDIOWIFI_MGMR_MSG_TYPE_ETH_FRAME_STA_TO_WIFI_TX = 0x2000,
    SDIOWIFI_MGMR_MSG_TYPE_ETH_FRAME_STA_FROM_WIFI_RX,
    SDIOWIFI_MGMR_MSG_TYPE_ETH_FRAME_AP_TO_WIFI_TX,
    SDIOWIFI_MGMR_MSG_TYPE_ETH_FRAME_AP_FROM_WIFI_RX,
    SDIOWIFI_MGMR_MSG_TYPE_WIFI_FRAME_SNIFFER_TO_WIFI_TX,
    SDIOWIFI_MGMR_MSG_TYPE_WIFI_FRAME_SNIFFER_FROM_WIFI_RX,
};

#define SDIOWIFI_PING_INTERVAL_MS 250

struct sdiowifi_mgmr;

extern struct sdiowifi_mgmr g_sdiowifi_mgmr;

int sdiowifi_mgmr_start(struct sdiowifi_mgmr *env);
/**
 * @brief Reinit sdiowifi.
 *
 * @param init_tx_buf Init tx buffer or not. Should be set to true if
 * called right after wakeup from LP, false otherwise.
 *
 * @return 0 on success, -1 on failure
 */
int sdiowifi_mgmr_reinit(bool init_tx_buf);

/**
 * @brief init sdiowifi tx buf after lp exiting.
 *
 * @param NULL
 *
 * @return 0 on success, -1 on failure
 */
int sdiowifi_tx_buf_init_after_lp(void);

int sdiowifi_update_tx_credit(struct sdiowifi_tx_buf *tb);

/**
 * @brief it prepares before entering into powersaving mode
 *
 * @param NULL
 *
 * @RETURN 0 ON SUCCESS, -1 on failure
 */
int sdiowifi_mgmr_ps_prepare(void);

/**
 * @brief Retrieve the IP address of the SDIO Ethernet.
 *
 * @param NULL
 *
 * @RETURN 0 ON SUCCESS, -1 on failure
 */
int sdio_mgmr_sdioeth_ip4_addr_get(uint32_t *addr, uint32_t *mask, uint32_t *gw, uint32_t *dns);

#endif /* __SDIOWIFI_MGMR_H__ */
