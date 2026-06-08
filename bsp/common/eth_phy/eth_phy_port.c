
#include "bflb_emac.h"
#include "bflb_mtimer.h"
#include "bflb_name.h"

#include "eth_phy_port.h"

const char *bsp_emac_get_device_name(uint8_t port)
{
#if defined(BL618DG)
    if (port == 0) {
        return BFLB_NAME_EMAC_V2_0;
    } else if (port == 1) {
        return BFLB_NAME_EMAC_V2_1;
    }
#else
    if (port == 0) {
        return BFLB_NAME_EMAC0;
    }
#endif

    return NULL;
}

struct bflb_device_s *bsp_emac_get_device(uint8_t port)
{
    const char *device_name = bsp_emac_get_device_name(port);

    if (!device_name) {
        return NULL;
    }

    return bflb_device_get_by_name(device_name);
}

void eth_phy_delay_ms(uint32_t ms)
{
    bflb_mtimer_delay_ms(ms);
}

int eth_phy_mdio_read(struct bflb_device_s *mac_mdio_dev, uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
    if (!mac_mdio_dev) {
        return -1;
    }

    return bflb_emac_md_read(mac_mdio_dev, phy_addr, reg_addr, data);
}

int eth_phy_mdio_write(struct bflb_device_s *mac_mdio_dev, uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
    if (!mac_mdio_dev) {
        return -1;
    }

    return bflb_emac_md_write(mac_mdio_dev, phy_addr, reg_addr, data);
}
