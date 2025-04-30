
#include "eth_phy.h"
#include "ephy_general.h"
#include "ephy_lan8720.h"

#define DBG_TAG "LAN8720"
#include "log.h"

eth_phy_drv_t ephy_lan8720_drv = {
    .phy_id = EPHY_LAN8720_ID,
    .phy_id_mask = EPHY_LAN8720_MASK,
    .phy_name = "LAN8720",

    .phy_init = ephy_lan8720_init,
    .phy_ctrl = ephy_lan8720_ctrl,
};

int ephy_lan8720_init(eth_phy_ctrl_t *phy_ctrl, eth_phy_init_cfg_t *cfg)
{
    uint16_t regval;

    /* close all capable */
    eth_phy_mdio_read(phy_ctrl->phy_addr, LAN8720_SPECIAL_MODE_OFFSET, &regval);
    regval &= ~LAN8720_MODE_MASK;
    eth_phy_mdio_write(phy_ctrl->phy_addr, LAN8720_SPECIAL_MODE_OFFSET, regval);

    ephy_general_init(phy_ctrl, cfg);

    return 0;
}

int ephy_lan8720_ctrl(eth_phy_ctrl_t *phy_ctrl, uint32_t cmd, uint32_t arg)
{
    int ret = 0;
    uint16_t regval;

    /* check cmd type */
    uint32_t cmd_type = (cmd & EPHY_CMD_TYPE_MASK) >> EPHY_CMD_TYPE_SHIFT;
    if (cmd_type == EPHY_CMD_TYPE_GENERAL) {
        /* general cmd */
        ret = ephy_general_ctrl(phy_ctrl, cmd, arg);
        return ret;
    } else if (cmd_type != EPHY_CMD_TYPE_LAN8720) {
        /* cmd error */
        LOG_E("cmd type error: 0x%08X\r\n", cmd);
        return -1;
    }

    switch (cmd) {
        case EPHY_CMD_LAN8720_SET_FAR_LOOPBACK:
            eth_phy_mdio_read(phy_ctrl->phy_addr, LAN8720_MODE_CTRL_STA_OFFSET, &regval);
            if (arg) {
                regval |= LAN8720_FARLOOPBACK_BIT;
            } else {
                regval &= ~LAN8720_FARLOOPBACK_BIT;
            }
            eth_phy_mdio_write(phy_ctrl->phy_addr, LAN8720_MODE_CTRL_STA_OFFSET, regval);
            break;

        default:
            LOG_E("not support cmd: 0x%08X\r\n", cmd);
            ret = -1;
    }

    return ret;
}
