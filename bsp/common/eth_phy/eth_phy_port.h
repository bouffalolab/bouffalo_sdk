
#ifndef __ETH_PHY_PORT__
#define __ETH_PHY_PORT__

#include "stdint.h"
#include "stdbool.h"

struct bflb_device_s;
struct eth_phy_ctrl_s;

/* Each PHY must use an independent MDIO controller; task context only. Bare-metal
 * callers must serialize externally. Raw MDIO writes must not run concurrently
 * with management API configuration of the same PHY.
 * Lock order: port management lock -> MDIO lock; never the reverse. */
#ifndef CONFIG_EMAC_MDIO_LOCK
#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
#define CONFIG_EMAC_MDIO_LOCK 1
#else
#define CONFIG_EMAC_MDIO_LOCK 0
#endif
#endif

#if CONFIG_EMAC_MDIO_LOCK
#include "FreeRTOS.h"
#include "semphr.h"

typedef struct {
	SemaphoreHandle_t handle;
	StaticSemaphore_t storage;
	bool initialized;
} eth_phy_mdio_lock_t;
#endif

void eth_phy_delay_ms(uint32_t ms);

const char *bsp_emac_get_device_name(uint8_t port);
struct bflb_device_s *bsp_emac_get_device(uint8_t port);

int eth_phy_mdio_read(struct eth_phy_ctrl_s *phy_ctrl, uint8_t phy_addr, uint8_t reg_addr, uint16_t *data);
int eth_phy_mdio_write(struct eth_phy_ctrl_s *phy_ctrl, uint8_t phy_addr, uint8_t reg_addr, uint16_t data);

#endif
