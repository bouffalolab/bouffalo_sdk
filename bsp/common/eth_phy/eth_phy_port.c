
#include "bflb_emac.h"
#include "bflb_mtimer.h"
#include "bflb_name.h"

#include "eth_phy.h"

#if defined(CONFIG_EMAC_MDIO_LOCK) && CONFIG_EMAC_MDIO_LOCK
static void eth_phy_mdio_lock(struct eth_phy_ctrl_s *phy_ctrl)
{
    if (!phy_ctrl->mdio_lock.initialized) {
        phy_ctrl->mdio_lock.handle = xSemaphoreCreateMutexStatic(&phy_ctrl->mdio_lock.storage);
        if (phy_ctrl->mdio_lock.handle != NULL) phy_ctrl->mdio_lock.initialized = true;
    }

    xSemaphoreTake(phy_ctrl->mdio_lock.handle, portMAX_DELAY);
}

static void eth_phy_mdio_unlock(struct eth_phy_ctrl_s *phy_ctrl)
{
    xSemaphoreGive(phy_ctrl->mdio_lock.handle);
}
#else
static void eth_phy_mdio_lock(struct eth_phy_ctrl_s *phy_ctrl)
{
    (void)phy_ctrl;
}

static void eth_phy_mdio_unlock(struct eth_phy_ctrl_s *phy_ctrl)
{
    (void)phy_ctrl;
}
#endif

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

int eth_phy_mdio_read(struct eth_phy_ctrl_s *phy_ctrl, uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
    int ret;

    if (!phy_ctrl->mac_mdio_dev) {
        return -1;
    }

    eth_phy_mdio_lock(phy_ctrl);
    ret = bflb_emac_md_read(phy_ctrl->mac_mdio_dev, phy_addr, reg_addr, data);
    eth_phy_mdio_unlock(phy_ctrl);
    return ret;
}

int eth_phy_mdio_write(struct eth_phy_ctrl_s *phy_ctrl, uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
    int ret;

    if (!phy_ctrl->mac_mdio_dev) {
        return -1;
    }

    eth_phy_mdio_lock(phy_ctrl);
    ret = bflb_emac_md_write(phy_ctrl->mac_mdio_dev, phy_addr, reg_addr, data);
    eth_phy_mdio_unlock(phy_ctrl);
    return ret;
}
