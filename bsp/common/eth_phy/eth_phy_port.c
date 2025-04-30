
#include "bflb_emac.h"
#include "bflb_mtimer.h"

#include "eth_phy_port.h"

static struct bflb_device_s *emac_dev = NULL;

void eth_phy_delay_ms(uint32_t ms)
{
    bflb_mtimer_delay_ms(ms);
}

int eth_phy_mdio_read(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
    if (!emac_dev) {
        emac_dev = bflb_device_get_by_name("emac0");
        if (!emac_dev) {
            return -1;
        }
    }

    return bflb_emac_md_read(emac_dev, phy_addr, reg_addr, data);
}

int eth_phy_mdio_write(uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
    if (!emac_dev) {
        emac_dev = bflb_device_get_by_name("emac0");
        if (!emac_dev) {
            return -1;
        }
    }

    return bflb_emac_md_write(emac_dev, phy_addr, reg_addr, data);
}
