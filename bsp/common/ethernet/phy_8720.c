
#include "phy_8720.h"

#define CHECK_PARAM(expr) ((void)0)

struct bflb_device_s *emac_dev = NULL;
static struct bflb_emac_phy_cfg_s *phy_8720_cfg = NULL;

/**
 * @brief phy 8720 reset
 * @return int
 *
 */
int phy_8720_reset(void)
{
    int timeout = 1000;
    uint16_t regval = PHY_RESET;

    /* pull the PHY from power down mode if it is in */
    if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_SPECIAL_MODES, &regval)) {
        printf("emac phy reg read fail 0x%04x\r\n", regval);
        return -1;
    }
    printf("emac phy reg read 0x%04x\r\n", regval);
    if (PHY_SPECIAL_MODES_MODE_PWRDOWN == (regval & PHY_SPECIAL_MODES_MODE)) {
        if (bflb_emac_phy_reg_write(emac_dev, PHY_SPECIAL_MODES, regval | PHY_SPECIAL_MODES_MODE_ALL) != 0) {
            return -1;
        }
    }

    /* disable energy detect powerdown mode for cable detect, this increase the power by 220mW */
    if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_CTRL_STATUS, &regval)) {
        return -1;
    }

    if (bflb_emac_phy_reg_write(emac_dev, PHY_CTRL_STATUS, regval & (~PHY_CTRL_STATUS_EDPWRDOWN)) != 0) {
        return -1;
    }

    /* do sw reset */
    if (bflb_emac_phy_reg_write(emac_dev, PHY_BCR, PHY_RESET) != 0) {
        return -1;
    }

    for (; timeout; timeout--) {
        if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_BCR, &regval)) {
            return -1;
        }

        if (!(regval & PHY_RESET)) {
            return 0;
        }

        bflb_mtimer_delay_ms(1);
    }

    return -1;
}

/**
 * @brief phy 8720 auto negotiate
 * @param cfg phy config
 * @return int
 *
 */
int phy_8720_auto_negotiate(struct bflb_emac_phy_cfg_s *cfg)
{
    uint16_t regval = 0;
    uint16_t phyid1 = 0, phyid2 = 0;
    uint16_t advertise = 0;
    uint16_t lpa = 0;
    uint32_t timeout = 100; //10s,in 100ms

    if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_PHYID1, &phyid1)) {
        printf("read emac phy id 1 error\r\n");
        return -1;
    }

    if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_PHYID2, &phyid2)) {
        printf("read emac phy id 2 error\r\n");
        return -1;
    }
    printf("emac phy id 1 =%08x\r\n", (unsigned int)phyid1);
    printf("emac phy id 2 =%08x\r\n", (unsigned int)phyid2);
    if (cfg->phy_id != (((phyid1 << 16) | phyid2) & 0x000FFFF0)) {
        /* ID error */
        return -1;
    } else {
        cfg->phy_id = (phyid1 << 16) | phyid2;
    }

    if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_BCR, &regval)) {
        return -1;
    }

    regval &= ~PHY_AUTONEGOTIATION;
    regval &= ~(PHY_LOOPBACK | PHY_POWERDOWN);
    regval |= PHY_ISOLATE;

    if (bflb_emac_phy_reg_write(emac_dev, PHY_BCR, regval) != 0) {
        return -1;
    }

    /* set advertisement mode */
    advertise = PHY_ADVERTISE_100BASETXFULL | PHY_ADVERTISE_100BASETXHALF |
                PHY_ADVERTISE_10BASETXFULL | PHY_ADVERTISE_10BASETXHALF |
                PHY_ADVERTISE_8023;

    if (bflb_emac_phy_reg_write(emac_dev, PHY_ADVERTISE, advertise) != 0) {
        return -1;
    }

    bflb_mtimer_delay_ms(16);

    if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_BCR, &regval)) {
        return -1;
    }

    bflb_mtimer_delay_ms(16);
    regval |= (PHY_FULLDUPLEX_100M | PHY_AUTONEGOTIATION);

    if (bflb_emac_phy_reg_write(emac_dev, PHY_BCR, regval) != 0) {
        return -1;
    }

    bflb_mtimer_delay_ms(16);
    regval |= PHY_RESTART_AUTONEGOTIATION;
    regval &= ~PHY_ISOLATE;

    if (bflb_emac_phy_reg_write(emac_dev, PHY_BCR, regval) != 0) {
        return -1;
    }

    bflb_mtimer_delay_ms(100);

    while (1) {
        if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_BSR, &regval)) {
            return -1;
        }

        if (regval & PHY_AUTONEGO_COMPLETE) {
            /* complete */
            break;
        }

        if (!(--timeout)) {
            return -1;
        }

        bflb_mtimer_delay_ms(100);
    }

    bflb_mtimer_delay_ms(100);

    if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_LPA, &lpa)) {
        return -1;
    }

    if (((advertise & lpa) & PHY_ADVERTISE_100BASETXFULL) != 0) {
        /* 100BaseTX and Full Duplex */
        cfg->full_duplex = 1;
        cfg->speed = 100;
        cfg->phy_state = PHY_STATE_READY;
    } else if (((advertise & lpa) & PHY_ADVERTISE_10BASETXFULL) != 0) {
        /* 10BaseT and Full Duplex */
        cfg->full_duplex = 1;
        cfg->speed = 10;
        cfg->phy_state = PHY_STATE_READY;
    } else if (((advertise & lpa) & PHY_ADVERTISE_100BASETXHALF) != 0) {
        /* 100BaseTX and half Duplex */
        cfg->full_duplex = 0;
        cfg->speed = 100;
        cfg->phy_state = PHY_STATE_READY;
    } else if (((advertise & lpa) & PHY_ADVERTISE_10BASETXHALF) != 0) {
        /* 10BaseT and half Duplex */
        cfg->full_duplex = 0;
        cfg->speed = 10;
        cfg->phy_state = PHY_STATE_READY;
    }

    return 0;
}

/**
 * @brief phy 8720 link up
 * @param cfg phy config
 * @return int
 *
 */
int phy_8720_link_up(struct bflb_emac_phy_cfg_s *cfg)
{
    uint16_t phy_bsr = 0;
    uint16_t phy_sr = 0;

    bflb_mtimer_delay_ms(16);

    if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_BSR, &phy_bsr)) {
        return -1;
    }

    bflb_mtimer_delay_ms(16);

    if (!(PHY_LINKED_STATUS & phy_bsr)) {
        return 1; /* error */
    }

    bflb_mtimer_delay_ms(16);

    if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_SR, &phy_sr)) {
        return -1;
    }

    if ((phy_bsr & PHY_BSR_100BASETXFULL) && PHY_SR_SPEED_MODE_COMPARE(phy_sr, PHY_SR_SPEED_100BASETXFULL)) {
        /* 100BaseTX and Full Duplex */
        cfg->full_duplex = 1;
        cfg->speed = 100;
        cfg->phy_state = PHY_STATE_UP;
    } else if ((phy_bsr & PHY_BSR_10BASETXFULL) && PHY_SR_SPEED_MODE_COMPARE(phy_sr, PHY_SR_SPEED_10BASETXFULL)) {
        /* 10BaseT and Full Duplex */
        cfg->full_duplex = 1;
        cfg->speed = 10;
        cfg->phy_state = PHY_STATE_UP;
    } else if ((phy_bsr & PHY_BSR_100BASETXHALF) && PHY_SR_SPEED_MODE_COMPARE(phy_sr, PHY_SR_SPEED_100BASETXHALF)) {
        /* 100BaseTX and half Duplex */
        cfg->full_duplex = 0;
        cfg->speed = 100;
        cfg->phy_state = PHY_STATE_UP;
    } else if ((phy_bsr & PHY_BSR_10BASETXHALF) && PHY_SR_SPEED_MODE_COMPARE(phy_sr, PHY_SR_SPEED_10BASETXHALF)) {
        /* 10BaseT and half Duplex */
        cfg->full_duplex = 0;
        cfg->speed = 10;
        cfg->phy_state = PHY_STATE_UP;
    } else {
        /* 10BaseT and half Duplex */
        cfg->full_duplex = -1;
        cfg->speed = -1;
        cfg->phy_state = PHY_STATE_DOWN;
        return -1;
    }

    return 0;
}

/**
 * @brief  Use energy detector for cable plug in/out detect.
 * @param  cfg: phy config
 * @return int
 *
 */
int phy_8720_poll_cable_status(struct bflb_emac_phy_cfg_s *cfg)
{
    uint16_t phy_regval = 0;

    CHECK_PARAM(NULL != phy_8720_cfg);

    if (0 != bflb_emac_phy_reg_read(emac_dev, PHY_CTRL_STATUS, &phy_regval)) {
        return -1;
    }

    phy_8720_cfg->phy_state = (PHY_CTRL_STATUS_ENERGYON & phy_regval) ? PHY_STATE_UP : PHY_STATE_DOWN;

    return !!(PHY_CTRL_STATUS_ENERGYON & phy_regval);
}

/**
 * @brief  Initialize EMAC PHY 8720 module
 * @param  cfg: phy config
 * @return int
 *
 */
int phy_8720_init(struct bflb_device_s *emac, struct bflb_emac_phy_cfg_s *cfg)
{
    uint16_t phyReg;

    CHECK_PARAM(NULL != cfg);

    phy_8720_cfg = cfg;
    emac_dev = emac;

    printf("emac phy addr:0x%04x\r\n", cfg->phy_address);
    // bflb_emac_phy_set_address(emac_dev, cfg->phy_address);
    bflb_emac_feature_control(emac, EMAC_CMD_SET_PHY_ADDRESS, cfg->phy_address);

    if (0 != phy_8720_reset()) {
        return -1;
    }

    if (cfg->auto_negotiation) {
        /*
        uint32_t cnt=0;
        do{
            if(bflb_emac_phy_reg_read(emac_dev, PHY_BSR, &phyReg) != SUCCESS){
                return -1;
            }
            cnt++;
            if(cnt>PHY_LINK_TO){
                return -1;
            }
        }while((phyReg & PHY_LINKED_STATUS) != PHY_LINKED_STATUS);
        */
        if (0 != phy_8720_auto_negotiate(cfg)) {
            return -1;
        }
    } else {
        if (bflb_emac_phy_reg_read(emac_dev, PHY_BCR, &phyReg) != 0) {
            return -1;
        }

        phyReg &= (~PHY_FULLDUPLEX_100M);

        if (cfg->speed == 10) {
            if (cfg->full_duplex == 1) {
                phyReg |= PHY_FULLDUPLEX_10M;
            } else {
                phyReg |= PHY_HALFDUPLEX_10M;
            }
        } else {
            if (cfg->full_duplex == 1) {
                phyReg |= PHY_FULLDUPLEX_100M;
            } else {
                phyReg |= PHY_HALFDUPLEX_100M;
            }
        }

        if ((bflb_emac_phy_reg_write(emac_dev, PHY_BCR, phyReg)) != 0) {
            return -1;
        }
    }

    // bflb_emac_phy_config_full_duplex(emac_dev, cfg->full_duplex);
    bflb_emac_feature_control(emac, EMAC_CMD_FULL_DUPLEX, cfg->full_duplex);

    return phy_8720_link_up(cfg);
}

/**
 * @brief get phy 8720 module status
 * @return emac_phy_status_t @ref emac_phy_status_t enum
 */
emac_phy_status_t phy_8720_status_get(void)
{
    CHECK_PARAM(NULL != phy_8720_cfg);

    if ((100 == phy_8720_cfg->speed) &&
        (phy_8720_cfg->full_duplex) &&
        (PHY_STATE_UP == phy_8720_cfg->phy_state)) {
        return EMAC_PHY_STAT_100MBITS_FULLDUPLEX;
    } else if (PHY_STATE_UP == phy_8720_cfg->phy_state) {
        return EMAC_PHY_STAT_LINK_UP;
    } else {
        return EMAC_PHY_STAT_LINK_DOWN;
    }
}
