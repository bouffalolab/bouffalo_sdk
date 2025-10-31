/**
 * @file phy_8720.h
 * @brief
 *
 * Copyright (c) 2022 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#ifndef __EPHY_LAN8720_H__
#define __EPHY_LAN8720_H__

/* Register offsets *********************************************************/

/* lan-8720 Vendor-specific Register offsets */
#define LAN8720_MODE_CTRL_STA_OFFSET      (17) /* Mode Control/Status Register */
#define LAN8720_SPECIAL_MODE_OFFSET       (18) /* Special Modes */
#define LAN8720_SYMBOL_ERR_CNT_OFFSET     (26) /* Symbol Error Counter Register */
#define LAN8720_CTRL_STA_INDICA_OFFSET    (27) /* Control / Status Indication Register */
#define LAN8720_INIT_SOURCE_OFFSET        (29) /* Interrupt Source Register */
#define LAN8720_INIT_MASK_OFFSET          (30) /* Interrupt Mask Register */
#define LAN8720_PHY_SPEC_CRTL_STA_OFFSET  (31) /* PHY Special Control/Status Register */

/* Register Bitfield definitions *****************************************************/

/* 17 : Mode Control/Status Register */
#define LAN8720_EDPWRDOWN_BIT             (1 << 13) /* Enable the Energy Detect Power-Down mode */
#define LAN8720_FARLOOPBACK_BIT           (1 << 9)  /* Enables far loopback mode */
#define LAN8720_ALTINT_BIT                (1 << 6)  /* Alternate Interrupt Mode */
#define LAN8720_ENERGYON_BIT              (1 << 1)  /* Indicates whether energy is detected */

/* 18 : Special Modes */
#define LAN8720_MODE_SHIFT                (5)
#define LAN8720_MODE_MASK                 (0x7 << LAN8720_MODE_SHIFT) /* Transceiver mode of operation */
#define LAN8720_PHYAD_SHIFT               (0)
#define LAN8720_PHYAD_MASK                (0x1f << LAN8720_PHYAD_SHIFT) /* PHY Address */

/* 26 : Symbol Error Counter Register */
#define LAN8720_SYM_ERR_CNT_SHIFT         (0)
#define LAN8720_SYM_ERR_CNT_MASK          (0xffff << LAN8720_SYM_ERR_CNT_SHIFT)

/* 27 : Control / Status Indication Register */
#define LAN8720_AMDIXCTRL_BIT             (1 << 15) /* HP Auto-MDIX control */
#define LAN8720_CH_SELECT_BIT             (1 << 13) /* Manual channel select */
#define LAN8720_SQEOFF_BIT                (1 << 11) /* Disable the SQE test (Heartbeat) */
#define LAN8720_XPOL_BIT                  (1 << 4)  /* Polarity state of the 10BASE-T */

/* 29 : Interrupt Source Register */
#define LAN8720_INT7_SOURCE_BIT           (1 << 7) /* ENERGYON generated */
#define LAN8720_INT6_SOURCE_BIT           (1 << 6) /* Auto-Negotiation complete */
#define LAN8720_INT5_SOURCE_BIT           (1 << 5) /* Remote Fault Detected */
#define LAN8720_INT4_SOURCE_BIT           (1 << 4) /* Link Down (link status negated) */
#define LAN8720_INT3_SOURCE_BIT           (1 << 3) /* Auto-Negotiation LP Acknowledge */
#define LAN8720_INT2_SOURCE_BIT           (1 << 2) /* Parallel Detection Fault */
#define LAN8720_INT1_SOURCE_BIT           (1 << 1) /* Auto-Negotiation Page Received */

/* 30 : Interrupt Mask Register */
#define LAN8720_INT7_UMASK_BIT            (1 << 7)
#define LAN8720_INT6_UMASK_BIT            (1 << 6)
#define LAN8720_INT5_UMASK_BIT            (1 << 5)
#define LAN8720_INT4_UMASK_BIT            (1 << 4)
#define LAN8720_INT3_UMASK_BIT            (1 << 3)
#define LAN8720_INT2_UMASK_BIT            (1 << 2)
#define LAN8720_INT1_UMASK_BIT            (1 << 1)

/* 31 : PHY Special Control/Status Register */
#define LAN8720_AUTODONE_BIT              (1 << 12) /* Auto-negotiation done indication */
#define LAN8720_SPEED_INDICA_SHIFT        (2)       /* Speed Indication */
#define LAN8720_SPEED_INDICA_MASK         (0x7 << LAN8720_SPEED_INDICA_SHIFT)

/* Register def end *********************************************************/

#define EPHY_LAN8720_ID                   (0x0007c0f0)
#define EPHY_LAN8720_MASK                 (0x0000000f)

/* phy lan8720 cmd def */
#define EPHY_CMD_TYPE_LAN8720_MASK        (EPHY_CMD_TYPE_LAN8720 << EPHY_CMD_TYPE_SHIFT)
#define EPHY_CMD_LAN8720_SET_FAR_LOOPBACK (1 | EPHY_CMD_TYPE_LAN8720_MASK) /* Enables far loopback mode */

extern eth_phy_drv_t ephy_lan8720_drv;

int ephy_lan8720_init(eth_phy_ctrl_t *phy_ctrl, eth_phy_init_cfg_t *cfg);
int ephy_lan8720_ctrl(eth_phy_ctrl_t *phy_ctrl, uint32_t cmd, uint32_t arg);

#endif /* __PHY_8720_H__ */