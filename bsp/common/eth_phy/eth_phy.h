
#ifndef __ETH_PHY_H__
#define __ETH_PHY_H__

#include "stdint.h"
#include "eth_phy_port.h"

/* phy addr limit */
#define EPHY_ADDR_MIN                     (0)
#define EPHY_ADDR_MAX                     (31)

/* link status def */
#define EPHY_LINK_STA_NEGOTIATE           (1)
#define EPHY_LINK_STA_REMOTE_FAULT        (2)
#define EPHY_LINK_STA_DOWN                (3)
#define EPHY_LINK_STA_UP                  (4)

/* speed mode def */
#define EPHY_SPEED_MODE_AUTO_NEGOTIATION  (0)
#define EPHY_SPEED_MODE_10M_HALF_DUPLEX   (1)
#define EPHY_SPEED_MODE_10M_FULL_DUPLEX   (2)
#define EPHY_SPEED_MODE_100M_HALF_DUPLEX  (3)
#define EPHY_SPEED_MODE_100M_FULL_DUPLEX  (4)
#define EPHY_SPEED_MODE_1000M_HALF_DUPLEX (5)
#define EPHY_SPEED_MODE_1000M_FULL_DUPLEX (6)

/* remote/local auto-negotiation ability def */
#define EPHY_ABILITY_10M_T                (1 << 0)
#define EPHY_ABILITY_10M_FULL_DUPLEX      (1 << 1)
#define EPHY_ABILITY_100M_TX              (1 << 2)
#define EPHY_ABILITY_100M_FULL_DUPLEX     (1 << 3)
#define EPHY_ABILITY_100M_T4              (1 << 4)
#define EPHY_ABILITY_SPEED_MASK           (0x0000ffff) /* bit0:16, speed mode */
#define EPHY_ABILITY_PAUSE_FULL_DUPLEX    (1 << 16)
#define EPHY_ABILITY_PAUSE_ASYNC          (1 << 17)

/* cmd type def */
#define EPHY_CMD_TYPE_SHIFT               (16)
#define EPHY_CMD_TYPE_MASK                (0xffff << EPHY_CMD_TYPE_SHIFT)
#define EPHY_CMD_TYPE_GENERAL             (0)
#define EPHY_CMD_TYPE_LAN8720             (1)

/* */
struct eth_phy_drv_s;
struct eth_phy_ctrl_s;
struct eth_phy_init_cfg_s;

/* eth_phy driver */
typedef struct eth_phy_drv_s {
    uint32_t phy_id;
    uint32_t phy_id_mask;

    char *phy_name;

    int (*phy_init)(struct eth_phy_ctrl_s *phy_ctrl, struct eth_phy_init_cfg_s *cfg);
    int (*phy_ctrl)(struct eth_phy_ctrl_s *phy_ctrl, uint32_t cmd, uint32_t arg);
} eth_phy_drv_t;

/* eth_phy ctrl */
typedef struct eth_phy_ctrl_s {
    uint8_t phy_addr;
    uint32_t phy_id;

    eth_phy_drv_t *phy_drv;
} eth_phy_ctrl_t;

/* eth_phy init_cfg */
typedef struct eth_phy_init_cfg_s {
    uint8_t speed_mode;
    uint32_t local_auto_negotiation_ability;
} eth_phy_init_cfg_t;

int eth_phy_scan(struct eth_phy_ctrl_s *phy_ctrl, uint8_t start_addr, uint8_t end_addr);
int eth_phy_init(struct eth_phy_ctrl_s *phy_ctrl, struct eth_phy_init_cfg_s *cfg);
int eth_phy_ctrl(struct eth_phy_ctrl_s *phy_ctrl, uint32_t cmd, uint32_t arg);

#endif
