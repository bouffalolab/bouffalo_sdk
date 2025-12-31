
#include "eth_phy.h"
#include "ephy_general.h"

#define DBG_TAG "EPHY_GENER"
#include "log.h"

eth_phy_drv_t ephy_general_drv = {
    .phy_id = EPHY_GENERAL_ID,
    .phy_id_mask = EPHY_GENERAL_MASK,
    .phy_name = "General",

    .phy_init = ephy_general_init,
    .phy_ctrl = ephy_general_ctrl,
};

/* Check phy ability */
static int ephy_general_ability_check(eth_phy_ctrl_t *phy_ctrl, eth_phy_init_cfg_t *cfg, uint32_t *negotia_ability)
{
    uint16_t regval;
    uint16_t ability;

    eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_STATUS_OFFSET, &regval);

    if (cfg->speed_mode == EPHY_SPEED_MODE_AUTO_NEGOTIATION) {
        /* check auto negotiation */
        if (regval & EPHY_BSR_AUTO_NEGOTIATE_ABLE_BIT) {
            ability = cfg->local_auto_negotiation_ability;
        } else {
            LOG_E("not support auto-negotiation\r\n");
            return -1;
        }

        /* 100M T4 */
        if ((ability & EPHY_ABILITY_100M_T4) && !(regval & EPHY_BSR_100BASE_T4_ABLE_BIT)) {
            ability &= ~EPHY_ABILITY_100M_T4;
            LOG_W("not support 100M-T4\r\n");
        }
        /* 100M full duplex */
        if ((ability & EPHY_ABILITY_100M_FULL_DUPLEX) && !(regval & EPHY_BSR_100BASE_TX_FULL_DUPLEX_ABLE_BIT)) {
            ability &= ~EPHY_ABILITY_100M_FULL_DUPLEX;
            LOG_W("not support 100M-FULL_DUPLEX\r\n");
        }
        /* 100M half duplex */
        if ((ability & EPHY_ABILITY_100M_TX) && !(regval & EPHY_BSR_100BASE_TX_HALF_DUPLEX_ABLE_BIT)) {
            ability &= ~EPHY_ABILITY_100M_TX;
            LOG_W("not support 100M-HALF_DUPLEX\r\n");
        }
        /* 10M full duplex */
        if ((ability & EPHY_ABILITY_10M_FULL_DUPLEX) && !(regval & EPHY_BSR_10BASE_TX_FULL_DUPLEX_ABLE_BIT)) {
            ability &= ~EPHY_ABILITY_10M_FULL_DUPLEX;
            LOG_W("not support 10M-FULL_DUPLEX\r\n");
        }
        /* 10M half duplex */
        if ((ability & EPHY_ABILITY_10M_T) && !(regval & EPHY_BSR_10BASE_TX_HALF_DUPLEX_ABLE_BIT)) {
            ability &= ~EPHY_ABILITY_10M_T;
            LOG_W("not support 10M-HALF_DUPLEX\r\n");
        }

        /* check ability speed */
        if ((ability & EPHY_ABILITY_SPEED_MASK) == 0) {
            LOG_E("not support all speed of auto-negotiation \r\n");
            return -1;
        }

        *negotia_ability = ability;

    } else {
        /* 100M T4 */
        if (cfg->speed_mode == EPHY_ABILITY_100M_T4 && !(regval & EPHY_BSR_100BASE_T4_ABLE_BIT)) {
            LOG_E("not support 100M-T4\r\n");
            return -1;
        }
        /* 100M full duplex */
        if (cfg->speed_mode == EPHY_ABILITY_100M_FULL_DUPLEX && !(regval & EPHY_BSR_100BASE_TX_FULL_DUPLEX_ABLE_BIT)) {
            LOG_E("not support 100M-FULL_DUPLEX\r\n");
            return -1;
        }
        /* 100M half duplex */
        if (cfg->speed_mode == EPHY_ABILITY_100M_TX && !(regval & EPHY_BSR_100BASE_TX_HALF_DUPLEX_ABLE_BIT)) {
            LOG_E("not support 100M-HALF_DUPLEX\r\n");
            return -1;
        }
        /* 10M full duplex */
        if ((cfg->speed_mode == EPHY_ABILITY_10M_FULL_DUPLEX) && !(regval & EPHY_BSR_10BASE_TX_FULL_DUPLEX_ABLE_BIT)) {
            LOG_E("not support 10M-FULL_DUPLEX\r\n");
            return -1;
        }
        /* 10M half duplex */
        if ((cfg->speed_mode == EPHY_ABILITY_10M_T) && !(regval & EPHY_BSR_10BASE_TX_HALF_DUPLEX_ABLE_BIT)) {
            LOG_E("not support 10M-HALF_DUPLEX\r\n");
            return -1;
        }
    }

    return 0;
}

/* phy init */
int ephy_general_init(eth_phy_ctrl_t *phy_ctrl, eth_phy_init_cfg_t *cfg)
{
    uint16_t regval;
    uint32_t negotia_ability = 0;

    /* reset */
    eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, &regval);
    regval |= EPHY_CSR_SOFT_RESET_BIT;
    eth_phy_mdio_write(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, regval);

    /* Per the IEEE 802.3u standard, clause 22 (22.2.4.1.1) the reset process will be completed within 0.5s from the setting of this bit. */
    for (int i = 0; i < 500; i++) {
        eth_phy_delay_ms(1);
        /* read reset bit */
        eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, &regval);
        if (!(regval & EPHY_CSR_SOFT_RESET_BIT)) {
            // LOG_I("reset done: %dms\r\n", i);
            break;
        }
    }

    if (ephy_general_ability_check(phy_ctrl, cfg, &negotia_ability) < 0) {
        return -1;
    }

    if (cfg->speed_mode != EPHY_SPEED_MODE_AUTO_NEGOTIATION) {
        /* disable Auto-Negotiate */
        eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, &regval);
        regval &= ~EPHY_CSR_AUTO_NEGOTIATION_EN_BIT;
        eth_phy_mdio_write(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, regval);

        /* half/full duplex mode */
        if (cfg->speed_mode == EPHY_SPEED_MODE_10M_FULL_DUPLEX || cfg->speed_mode == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
            regval |= EPHY_CSR_DUPLEX_MODE_BIT;
        } else {
            regval &= ~EPHY_CSR_DUPLEX_MODE_BIT;
        }

        /* 10M/100M speed */
        if (cfg->speed_mode == EPHY_SPEED_MODE_100M_HALF_DUPLEX || cfg->speed_mode == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
            regval |= EPHY_CSR_SPEED_SEL_LSB_BIT;
        } else {
            regval &= ~EPHY_CSR_SPEED_SEL_LSB_BIT;
        }

        eth_phy_mdio_write(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, regval);
        return 0;
    }

    /* Auto-Negotiation Advertisement Register */
    eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_AUTONEG_ADVERTISE_OFFSET, &regval);
    /* 10BASE-TX ability */
    if (negotia_ability & EPHY_ABILITY_10M_T) {
        regval |= EPHY_10BASE_TX_ABLE_BIT;
    } else {
        regval &= ~EPHY_10BASE_TX_ABLE_BIT;
    }
    /* 10BASE-TX Full Duplex ability */
    if (negotia_ability & EPHY_ABILITY_10M_FULL_DUPLEX) {
        regval |= EPHY_10BASE_TX_FULL_DUPLEX_ABLE_BIT;
    } else {
        regval &= ~EPHY_10BASE_TX_FULL_DUPLEX_ABLE_BIT;
    }
    /* 100BASE-TX ability */
    if (negotia_ability & EPHY_ABILITY_100M_TX) {
        regval |= EPHY_100BASE_TX_ABLE_BIT;
    } else {
        regval &= ~EPHY_100BASE_TX_ABLE_BIT;
    }
    /* 100BASE-TX Full Duplex ability */
    if (negotia_ability & EPHY_ABILITY_100M_FULL_DUPLEX) {
        regval |= EPHY_100BASE_TX_FULL_DUPLEX_ABLE_BIT;
    } else {
        regval &= ~EPHY_100BASE_TX_FULL_DUPLEX_ABLE_BIT;
    }
    eth_phy_mdio_write(phy_ctrl->phy_addr, EPHY_AUTONEG_ADVERTISE_OFFSET, regval);

    /* enable Auto-Negotiate */
    eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, &regval);
    regval |= EPHY_CSR_AUTO_NEGOTIATION_EN_BIT;
    eth_phy_mdio_write(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, regval);
    /* restart Auto-Negotiate */
    regval |= EPHY_CSR_RESTART_NEGOTIATION_BIT;
    eth_phy_mdio_write(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, regval);

    return 0;
}

/* phy ctrl */
int ephy_general_ctrl(eth_phy_ctrl_t *phy_ctrl, uint32_t cmd, uint32_t arg)
{
    int ret = 0;
    uint16_t regval, regval1, regval2;

    /* check cmd type */
    if ((cmd & EPHY_CMD_TYPE_MASK) >> EPHY_CMD_TYPE_SHIFT != EPHY_CMD_TYPE_GENERAL) {
        LOG_E("cmd type error: 0x%08X\r\n", cmd);
        return -1;
    }

    switch (cmd) {
        case EPHY_CMD_GET_LINK_STA:
            /* check Auto-Negotiate Enable */
            eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, &regval);
            if (regval & EPHY_CSR_AUTO_NEGOTIATION_EN_BIT) {
                /* check Auto-Negotiate Complete */
                eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_STATUS_OFFSET, &regval);
                if (regval & EPHY_BSR_AUTO_NEGOTIATE_COMPLETE_BIT) {
                    /* check link status */
                    if (regval & EPHY_BSR_LINK_STATUS_BIT) {
                        ret = EPHY_LINK_STA_UP;
                    } else {
                        ret = EPHY_LINK_STA_DOWN;
                    }
                } else {
                    /* check Remote Fault */
                    if (regval & EPHY_BSR_REMOTE_FAULT_BIT) {
                        ret = EPHY_LINK_STA_REMOTE_FAULT;
                    } else {
                        ret = EPHY_LINK_STA_NEGOTIATE;
                    }
                }
            } else {
                eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_STATUS_OFFSET, &regval);
                /* check link status */
                if (regval & EPHY_BSR_LINK_STATUS_BIT) {
                    ret = EPHY_LINK_STA_UP;
                } else {
                    ret = EPHY_LINK_STA_DOWN;
                }
            }
            break;

        case EPHY_CMD_GET_SPEED_MODE:
            /* check Auto-Negotiate Enable */
            eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, &regval);
            if (regval & EPHY_CSR_AUTO_NEGOTIATION_EN_BIT) {
                /* check Auto-Negotiate Complete */
                eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_STATUS_OFFSET, &regval);
                if (regval & EPHY_BSR_AUTO_NEGOTIATE_COMPLETE_BIT) {
                    /* get Auto-Negotiation Advertisement Register */
                    eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_AUTONEG_ADVERTISE_OFFSET, &regval1);
                    /* get Auto-Negotiation Link Partner Ability Register  */
                    eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_AUTONEG_LINKPARTNER_OFFSET, &regval2);
                    /* get speed mode */
                    if (regval1 & regval2 & EPHY_100BASE_TX_FULL_DUPLEX_ABLE_BIT) {
                        ret = EPHY_SPEED_MODE_100M_FULL_DUPLEX;
                    } else if (regval1 & regval2 & EPHY_100BASE_TX_ABLE_BIT) {
                        ret = EPHY_SPEED_MODE_100M_HALF_DUPLEX;
                    } else if (regval1 & regval2 & EPHY_10BASE_TX_FULL_DUPLEX_ABLE_BIT) {
                        ret = EPHY_SPEED_MODE_10M_FULL_DUPLEX;
                    } else if (regval1 & regval2 & EPHY_10BASE_TX_ABLE_BIT) {
                        ret = EPHY_SPEED_MODE_10M_HALF_DUPLEX;
                    }
                } else {
                    ret = EPHY_SPEED_MODE_AUTO_NEGOTIATION;
                }
            } else {
                if (regval & EPHY_CSR_DUPLEX_MODE_BIT) {
                    if (regval & EPHY_CSR_SPEED_SEL_LSB_BIT) {
                        ret = EPHY_SPEED_MODE_100M_FULL_DUPLEX;
                    } else {
                        ret = EPHY_SPEED_MODE_10M_FULL_DUPLEX;
                    }
                } else {
                    if (regval & EPHY_CSR_SPEED_SEL_LSB_BIT) {
                        ret = EPHY_SPEED_MODE_100M_HALF_DUPLEX;
                    } else {
                        ret = EPHY_SPEED_MODE_10M_HALF_DUPLEX;
                    }
                }
            }
            break;

        case EPHY_CMD_SET_RESTART_NEGOTIATE:
            /* restart Auto-Negotiate */
            eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, &regval);
            regval |= EPHY_CSR_RESTART_NEGOTIATION_BIT;
            eth_phy_mdio_write(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, regval);
            break;

        case EPHY_CMD_SET_LOOPBACK_MODE:
            /* loopback mode */
            eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, &regval);
            if (arg) {
                regval |= EPHY_CSR_LOOPBACK_BIT;
            } else {
                regval &= ~EPHY_CSR_LOOPBACK_BIT;
            }
            eth_phy_mdio_write(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, regval);
            break;

        case EPHY_CMD_SET_POWER_DOWN:
            /* General power down mode */
            eth_phy_mdio_read(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, &regval);
            if (arg) {
                regval |= EPHY_CSR_POWER_DOWN_BIT;
            } else {
                regval &= ~EPHY_CSR_POWER_DOWN_BIT;
            }
            eth_phy_mdio_write(phy_ctrl->phy_addr, EPHY_BASIC_CONTROL_OFFSET, regval);
            break;

        default:
            LOG_E("not support cmd: 0x%08X\r\n", cmd);
            ret = -1;
            break;
    }

    return ret;
}
