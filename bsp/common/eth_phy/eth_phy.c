

#include "eth_phy_port.h"

#include "eth_phy.h"
#include "ephy_general.h"
#include "ephy_lan8720.h"

#define DBG_TAG "EPHY"
#include "log.h"

static eth_phy_drv_t *phy_drv_table[] = {
    &ephy_lan8720_drv,
};

static int eth_phy_drv_match(eth_phy_ctrl_t *phy_ctrl)
{
    eth_phy_drv_t *phy_drv;

    for (uint32_t drv_num = 0; drv_num < sizeof(phy_drv_table) / sizeof(phy_drv_table[0]); drv_num++) {
        phy_drv = phy_drv_table[drv_num];

        if ((phy_ctrl->phy_id & ~phy_drv->phy_id_mask) == (phy_drv->phy_id & ~phy_drv->phy_id_mask)) {
            phy_ctrl->phy_drv = phy_drv;
            LOG_I("drv_match success, phy_name: %s\r\n", phy_ctrl->phy_drv->phy_name);
            return 0;
        }
    }

    phy_ctrl->phy_drv = &ephy_general_drv;
    LOG_W("drv_match falied, use %s driver\r\n", ephy_general_drv.phy_name);

    return -1;
}

int eth_phy_scan(eth_phy_ctrl_t *phy_ctrl, uint8_t start_addr, uint8_t end_addr)
{
    uint32_t phy_id;
    uint16_t phy_id_1, phy_id_2;
    uint8_t phy_addr;

    start_addr = (start_addr < EPHY_ADDR_MAX) ? start_addr : EPHY_ADDR_MAX;
    end_addr = (end_addr < EPHY_ADDR_MAX) ? end_addr : EPHY_ADDR_MAX;

    for (phy_addr = start_addr; phy_addr <= end_addr; phy_addr++) {
        eth_phy_mdio_read(phy_addr, EPHY_ID1_OFFSET, &phy_id_1);
        eth_phy_mdio_read(phy_addr, EPHY_ID2_OFFSET, &phy_id_2);

        phy_id = ((uint32_t)phy_id_1 << 16) | phy_id_2;

        if (phy_id != 0 && phy_id != 0xffffffff) {
            LOG_I("eth phy scan success, phy_addr: %d, phy_id: 0x%08X\r\n", phy_addr, phy_id);
            break;
        }
    }

    if (phy_addr > end_addr) {
        LOG_E("eth phy scan success failed\r\n");
        return -1;
    }

    phy_ctrl->phy_addr = phy_addr;
    phy_ctrl->phy_id = phy_id;
    phy_ctrl->phy_drv = NULL;
    /* try to match drv */
    eth_phy_drv_match(phy_ctrl);

    return 0;
}

int eth_phy_init(eth_phy_ctrl_t *phy_ctrl, eth_phy_init_cfg_t *cfg)
{
    /* no phy_drv, try to match */
    if (phy_ctrl->phy_drv == NULL) {
        LOG_I("no phy_drv, try to match\r\n");
        if (eth_phy_drv_match(phy_ctrl) < 0) {
            LOG_E("eth phy drv_match failed\r\n");
            return -1;
        } else {
            LOG_I("eth phy drv_match success, phy_name: %s\r\n", phy_ctrl->phy_drv->phy_name);
        }
    }

    if (phy_ctrl->phy_drv->phy_init) {
        return phy_ctrl->phy_drv->phy_init(phy_ctrl, cfg);
    } else {
        return -1;
    }
}

int eth_phy_ctrl(eth_phy_ctrl_t *phy_ctrl, uint32_t cmd, uint32_t arg)
{
    if (phy_ctrl->phy_drv == NULL) {
        LOG_E("no phy_drv\r\n");
        return -1;
    }

    if (phy_ctrl->phy_drv->phy_ctrl) {
        return phy_ctrl->phy_drv->phy_ctrl(phy_ctrl, cmd, arg);
    } else {
        LOG_E("no phy_ctrl\r\n");
        return -1;
    }
}
