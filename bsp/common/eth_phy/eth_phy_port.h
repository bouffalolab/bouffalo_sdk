
#ifndef __ETH_PHY_PORT__
#define __ETH_PHY_PORT__

#include "stdint.h"

void eth_phy_delay_ms(uint32_t ms);

int eth_phy_mdio_read(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data);
int eth_phy_mdio_write(uint8_t phy_addr, uint8_t reg_addr, uint16_t data);

#endif
