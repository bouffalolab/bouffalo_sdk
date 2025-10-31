#ifndef __LWIP_NETIF_EMAC_H__
#define __LWIP_NETIF_EMAC_H__

#include "lwip/err.h"

#ifndef LWIP_EMAC_PORT_DEBUG
#define LWIP_EMAC_PORT_DEBUG (1)
#endif

/* stack size of rx task */
#ifndef LWIP_EMAC_RX_STACK_SIZE
#define LWIP_EMAC_RX_STACK_SIZE (1024)
#endif

/* rx buff size */
#ifndef LWIP_EMAC_RX_BUFF_SIZE
#define LWIP_EMAC_RX_BUFF_SIZE (1524)//(14 + 4 + 1500 + 4)
#endif
/* rx buff cnt */
#ifndef LWIP_EMAC_RX_BUFF_CNT
#define LWIP_EMAC_RX_BUFF_CNT (6)
#endif

/* tx buff size */
#ifndef LWIP_EMAC_TX_BUFF_SIZE
#define LWIP_EMAC_TX_BUFF_SIZE (1524)//(14 + 4 + 1500 + 4)
#endif
/* tx buff cnt */
#ifndef LWIP_EMAC_TX_BUFF_CNT
#define LWIP_EMAC_TX_BUFF_CNT (4)
#endif

/* emac min frame size  */
#ifndef LWIP_EMAC_FRAME_SIZE_MIN
#define LWIP_EMAC_FRAME_SIZE_MIN (64)//(14 + 46 + 4)
#endif
/* emac max frame size  */
#ifndef LWIP_EMAC_FRAME_SIZE_MAX
#define LWIP_EMAC_FRAME_SIZE_MAX (1524)//(14 + 4 + 1500 + 4)
#endif

/* get tx_buf timeout */
#ifndef LWIP_EMAC_GET_TXBUF_TIMEOUT 
#define LWIP_EMAC_GET_TXBUF_TIMEOUT (10)
#endif

/* emac netif mtu */
#ifndef LWIP_EMAC_NETIF_MTU
#define LWIP_EMAC_NETIF_MTU (1500)
#endif

#if (LWIP_EMAC_RX_BUFF_SIZE < LWIP_EMAC_FRAME_SIZE_MAX)
#error LWIP_EMAC_RX_BUFF_SIZE too small
#endif

/* Exported types ------------------------------------------------------------*/
typedef struct _emac_config_t {
    uint8_t mac_addr[6];
} emac_config_t;

void  eth_emac_hw_config(emac_config_t *cfg);
err_t eth_emac_if_init(struct netif *netif);
void eth_link_state_update(struct netif *netif);
void dhcp_sta_update(struct netif *netif);

#endif
