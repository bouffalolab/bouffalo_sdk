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
#ifndef __EPHY_GENERAL_H__
#define __EPHY_GENERAL_H__

/* The following PHY registers are the IEEE802.3 standard definition, same register and bit field may
    have different names in various PHYs, but the feature they represent should be same or very similar. */

/* Register offsets *********************************************************/

/* IEEE802.3 standard PHY registers. */
#define EPHY_BASIC_CONTROL_OFFSET                 (0)  /* Basic Control Register */
#define EPHY_BASIC_STATUS_OFFSET                  (1)  /* Basic Status Register */
#define EPHY_ID1_OFFSET                           (2)  /* PHY Identifier 1 */
#define EPHY_ID2_OFFSET                           (3)  /* PHY Identifier 2 */
#define EPHY_AUTONEG_ADVERTISE_OFFSET             (4)  /* Auto-Negotiation Advertisement Register */
#define EPHY_AUTONEG_LINKPARTNER_OFFSET           (5)  /* Auto-Negotiation Link Partner Ability Register */
#define EPHY_AUTONEG_EXPANSION_OFFSET             (6)  /* Auto-Negotiation Expansion Register */
#define EPHY_1000BASET_CONTROL_OFFSET             (9)  /* 1000BASE-T control register. */
#define EPHY_MMD_ACCESS_CONTROL_OFFSET            (13) /* MMD access control register. */
#define EPHY_MMD_ACCESS_DATA_OFFSET               (14) /* MMD access data register. */

/* Register Bitfield definitions *****************************************************/

/* 0 : Basic Control Register */
#define EPHY_CSR_SOFT_RESET_BIT                   (1 << 15) /* software reset */
#define EPHY_CSR_LOOPBACK_BIT                     (1 << 14) /* loopback mode */
#define EPHY_CSR_SPEED_SEL_LSB_BIT                (1 << 13) /* Speed Select (LSB) */
#define EPHY_CSR_AUTO_NEGOTIATION_EN_BIT          (1 << 12) /* Enable auto-negotiate */
#define EPHY_CSR_POWER_DOWN_BIT                   (1 << 11) /* General power down mode */
#define EPHY_CSR_ISOLATE_BIT                      (1 << 10) /* electrical isolation of PHY from the RMII */
#define EPHY_CSR_RESTART_NEGOTIATION_BIT          (1 << 9)  /* restart auto-negotiate */
#define EPHY_CSR_DUPLEX_MODE_BIT                  (1 << 8)  /* half/full duplex */
#define EPHY_CSR_COLLISION_TEST_BIT               (1 << 7)  /* enable COL signal test */
#define EPHY_CSR_SPEED_SEL_MSB_BIT                (1 << 6)  /* enable COL signal test */

/* 1 : Basic Status Register */
#define EPHY_BSR_100BASE_T4_ABLE_BIT              (1 << 15) /* 100BASE-T4 ability */
#define EPHY_BSR_100BASE_TX_FULL_DUPLEX_ABLE_BIT  (1 << 14) /* 100BASE-TX Full Duplex ability */
#define EPHY_BSR_100BASE_TX_HALF_DUPLEX_ABLE_BIT  (1 << 13) /* 100BASE-TX Half Duplex ability */
#define EPHY_BSR_10BASE_TX_FULL_DUPLEX_ABLE_BIT   (1 << 12) /* 10BASE-TX Full Duplex ability */
#define EPHY_BSR_10BASE_TX_HALF_DUPLEX_ABLE_BIT   (1 << 11) /* 10BASE-TX Half Duplex ability */
#define EPHY_BSR_100BASE_T2_FULL_DUPLEX_ABLE_BIT  (1 << 10) /* 100BASE-T2 Full Duplex ability */
#define EPHY_BSR_100BASE_T2_HALF_DUPLEX_ABLE_BIT  (1 << 9)  /* 100BASE-T2 Half Duplex ability */
#define EPHY_BSR_AUTO_NEGOTIATE_COMPLETE_BIT      (1 << 5)  /* auto-negotiate process completed */
#define EPHY_BSR_REMOTE_FAULT_BIT                 (1 << 4)  /* remote fault condition detected */
#define EPHY_BSR_AUTO_NEGOTIATE_ABLE_BIT          (1 << 3)  /* Auto-Negotiate Ability */
#define EPHY_BSR_LINK_STATUS_BIT                  (1 << 2)  /* Link Status */
#define EPHY_BSR_BABBER_DETECT_BIT                (1 << 1)  /* Jabber Detect */
#define EPHY_BSR_EXT_CAPA_BIT                     (1 << 0)  /* supports extended capabilities registers */

/* 2 : PHY Identifier 1 */
#define EPHY_ID1_SHIFT                            (0)
#define EPHY_ID1_MASK                             (0xffff < EPHY_ID1_SHIFT)

/* 3 : PHY Identifier 2 */
#define EPHY_ID2_SHIFT                            (0)
#define EPHY_ID2_MASK                             (0xffff < EPHY_ID2_SHIFT)

/* 4: Auto-Negotiation Advertisement Register */
#define EPHY_REMOTE_FAULT_BIT                     (1 << 13) /* Remote Fault */
#define EPHY_PAUSE_FULL_DUPLEX_ASYNC_BIT          (1 << 11) /* PAUSE for full duplex links */
#define EPHY_PAUSE_FULL_DUPLEX_BIT                (1 << 10) /* Async PAUSE for full duplex links */
#define EPHY_100BASE_T4_ABLE_BIT                  (1 << 9)  /* 100BASE-T4 ability */
#define EPHY_100BASE_TX_FULL_DUPLEX_ABLE_BIT      (1 << 8)  /* 100BASE-TX Full Duplex ability */
#define EPHY_100BASE_TX_ABLE_BIT                  (1 << 7)  /* 100BASE-TX ability */
#define EPHY_10BASE_TX_FULL_DUPLEX_ABLE_BIT       (1 << 6)  /* 10BASE-TX Full Duplex ability */
#define EPHY_10BASE_TX_ABLE_BIT                   (1 << 5)  /* 10BASE-TX ability */

/* 5: Auto-Negotiation Link Partner Ability Register */
#define EPHY_NEXT_PAGE_ABLE_BIT                   (1 << 15) /* next page ability */
#define EPHY_ACKNOWLEDGE_BIT                      (1 << 14) /*  link code word received from partner */
#define EPHY_REMOTE_FAULT_BIT                     (1 << 13) /* Remote Fault */
#define EPHY_PAUSE_OPT_BIT                        (1 << 10) /* Pause Operation */
#define EPHY_100BASE_T4_ABLE_BIT                  (1 << 9)  /* 100BASE-T4 ability */
#define EPHY_100BASE_TX_FULL_DUPLEX_ABLE_BIT      (1 << 8)  /* 100BASE-TX Full Duplex ability */
#define EPHY_100BASE_TX_ABLE_BIT                  (1 << 7)  /* 100BASE-TX ability */
#define EPHY_10BASE_TX_FULL_DUPLEX_ABLE_BIT       (1 << 6)  /* 10BASE-TX Full Duplex ability */
#define EPHY_10BASE_TX_ABLE_BIT                   (1 << 5)  /* 10BASE-TX ability */

/* 6: Auto-Negotiation Expansion Register */
#define EPHY_EXP_PARALLEL_DETECT_FAULT_BIT        (1 << 4) /* fault detected by parallel detection logic */
#define EPHY_EXP_LINK_PARTNER_NEXT_PAGE_ABLE_BIT  (1 << 3) /* link partner has next page ability */
#define EPHY_EXP_NEXT_PAGE_ABLE_BIT               (1 << 2) /* local device has next page ability */
#define EPHY_EXP_PAGE_RECEIVED_BIT                (1 << 1) /* new page received */
#define EPHY_EXP_LINK_PARTNER_AUTO_NEGOT_ABLE_BIT (1 << 0) /* link partner has auto-negotiation ability */

/* Register def end *********************************************************/

#define EPHY_GENERAL_ID                           (0x00000000)
#define EPHY_GENERAL_MASK                         (0xffffffff)

/* phy general cmd def */
#define EPHY_CMD_TYPE_GENERAL_MASK                (EPHY_CMD_TYPE_GENERAL << EPHY_CMD_TYPE_SHIFT)
#define EPHY_CMD_GET_LINK_STA                     (1 | EPHY_CMD_TYPE_GENERAL_MASK)
#define EPHY_CMD_GET_SPEED_MODE                   (2 | EPHY_CMD_TYPE_GENERAL_MASK)
#define EPHY_CMD_SET_RESTART_NEGOTIATE            (3 | EPHY_CMD_TYPE_GENERAL_MASK)
#define EPHY_CMD_SET_LOOPBACK_MODE                (4 | EPHY_CMD_TYPE_GENERAL_MASK)
#define EPHY_CMD_SET_POWER_DOWN                   (5 | EPHY_CMD_TYPE_GENERAL_MASK)

extern eth_phy_drv_t ephy_general_drv;

int ephy_general_init(eth_phy_ctrl_t *phy_ctrl, eth_phy_init_cfg_t *cfg);
int ephy_general_ctrl(eth_phy_ctrl_t *phy_ctrl, uint32_t cmd, uint32_t arg);

#endif /* __PHY_8720_H__ */