/**
 * @file phy_8201f.h
 * @brief
 *
 * Copyright (c) 2023 Bouffalolab team
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
#ifndef __PHY_8201F_H__
#define __PHY_8201F_H__

#include <string.h>
#include "ethernet_phy.h"

/* RPC8201F PHY Address*/
#define EMAC_PHY_ADDRESS                        0x00U
#define PHY_LINK_TO                             ((uint32_t)0x00000FFFU)
#define PHY_AUTONEGO_COMPLETED_TO               ((uint32_t)0x00000FFFU)
/* PHY Registers */
#define PHY_BCR                                 ((uint16_t)0x00U)   /*!< Transceiver Basic Control Register; default: 0x1140 */
#define PHY_RESET                               ((uint16_t)0x8000U) /*!< PHY Reset */
#define PHY_LOOPBACK                            ((uint16_t)0x4000U) /*!< Select loop-back mode */
#define PHY_FULLDUPLEX_1000M                    ((uint16_t)0x0140U) /*!< Set the full-duplex mode at 1000 Mb/s */
#define PHY_HALFDUPLEX_1000M                    ((uint16_t)0x0040U) /*!< Set the half-duplex mode at 1000 Mb/s */
#define PHY_FULLDUPLEX_100M                     ((uint16_t)0x2100U) /*!< Set the full-duplex mode at 100 Mb/s  */
#define PHY_HALFDUPLEX_100M                     ((uint16_t)0x2000U) /*!< Set the half-duplex mode at 100 Mb/s  */
#define PHY_FULLDUPLEX_10M                      ((uint16_t)0x0100U) /*!< Set the full-duplex mode at 10 Mb/s   */
#define PHY_HALFDUPLEX_10M                      ((uint16_t)0x0000U) /*!< Set the half-duplex mode at 10 Mb/s   */
#define PHY_AUTONEGOTIATION                     ((uint16_t)0x1000U) /*!< Enable auto-negotiation function      */
#define PHY_RESTART_AUTONEGOTIATION             ((uint16_t)0x0200U) /*!< Restart auto-negotiation function     */
#define PHY_POWERDOWN                           ((uint16_t)0x0800U) /*!< Select the power down mode            */
#define PHY_ISOLATE                             ((uint16_t)0x0400U) /*!< Isolate PHY from MII                  */

#define PHY_BSR                                 ((uint16_t)0x01U)   /*!< Transceiver Basic Status Register     */
#define PHY_BSR_100BASETXFULL                   (1 << 14)           /*!< 100Base full-duplex                   */
#define PHY_BSR_100BASETXHALF                   (1 << 13)           /*!< 100Base half-duplex                   */
#define PHY_BSR_10BASETXFULL                    (1 << 12)           /*!< 10Base full-duplex                    */
#define PHY_BSR_10BASETXHALF                    (1 << 11)           /*!< 10Base half-duplex                    */
#define PHY_BSR_REMOTE_FAULT                    (1 << 4)            /*!< remote fault condition detected       */
#define PHY_AUTONEGO_COMPLETE                   ((uint16_t)0x0020U) /*!< Auto-Negotiation process completed    */
#define PHY_LINKED_STATUS                       ((uint16_t)0x0004U) /*!< Valid link established   link is up   */
#define PHY_JABBER_DETECTION                    ((uint16_t)0x0002U) /*!< Jabber condition detected             */

#define PHY_PHYID1                              ((uint16_t)0x02U) /*!< PHY ID 1  default 0x0000                */
#define PHY_PHYID2                              ((uint16_t)0x03U) /*!< PHY ID 2  default 0x0128                */
#define PHY_ADVERTISE                           ((uint16_t)0x04U) /*!< Auto-negotiation advertisement          */
#define PHY_ADVERTISE_100BASETXFULL             (1 << 8)          /*!< 100Base-TX full-duplex                  */
#define PHY_ADVERTISE_100BASETXHALF             (1 << 7)          /*!< 100Base-TX half-duplex                  */
#define PHY_ADVERTISE_10BASETXFULL              (1 << 6)          /*!< 10Base-Te full-duplex                   */
#define PHY_ADVERTISE_10BASETXHALF              (1 << 5)          /*!< 10Base-Te half-duplex                   */
#define PHY_ADVERTISE_8023                      (1 << 0)          /*!< IEEE 802.3 mode                         */

#define PHY_LPA                                 ((uint16_t)0x05U) /*!< Auto-negotiation link partner ability   */
#define PHY_EXPANSION                           ((uint16_t)0x06U) /*!< Auto-negotiation expansion           */
/* Section 4: Extended PHY Registers */
//#define PHY_CTRL_STATUS                         ((uint16_t)17U)   /*!< PHY MODE CONTROL/STATUS REGISTER Offset     */
//#define PHY_CTRL_STATUS_EDPWRDOWN               (1 << 13)         /*!< EDPWRDOWN */
//#define PHY_CTRL_STATUS_ENERGYON                (1 << 1)          /*!< ENERGYON */
//#define PHY_SPECIAL_MODES                       ((uint16_t)18U)   /*!< PHY SPECIAL MODES REGISTER Offset     */
//#define PHY_SPECIAL_MODES_MODE                  (7 << 5)          /*!< Transceiver mode of operation */
//#define PHY_SPECIAL_MODES_MODE_PWRDOWN          (6 << 5)          /*!< All capable. Auto-negotiation enabled */
//#define PHY_SPECIAL_MODES_MODE_ALL              (7 << 5)          /*!< All capable. Auto-negotiation enabled */
//#define PHY_SPECIAL_MODES_PHYAD                 (0x1F << 0)       /*!< PHY Address */

// #define PHY_SCR                                 ((uint16_t)0x10U) /*!< PHY specific control register Offset    */
#define PHY_SR                                  ((uint16_t)0x11U) /*!< PHY specific status register Offset     */
#define PHY_SR_SPEED_OFFSET                     (13)
#define PHY_SR_SPEED_MASK                       (0x7 << PHY_SR_SPEED_OFFSET)
#define PHY_SR_SPEED_10BASETXHALF               (0x0 << PHY_SR_SPEED_OFFSET) /* 10Mbps Half-duplex  */
#define PHY_SR_SPEED_10BASETXFULL               (0x1 << PHY_SR_SPEED_OFFSET) /* 10Mbps Full-duplex  */
#define PHY_SR_SPEED_100BASETXHALF              (0x2 << PHY_SR_SPEED_OFFSET) /* 100Mbps Half-duplex */
#define PHY_SR_SPEED_100BASETXFULL              (0x3 << PHY_SR_SPEED_OFFSET) /* 100Mbps Full-duplex */
#define PHY_SR_SPEED_MODE_COMPARE(status, mode) (!!(mode == (status & PHY_SR_SPEED_MASK)))
//#define PHY_SPEED_STATUS                        ((uint16_t)0x0004U) /*!< PHY Speed mask                                  */
//#define PHY_DUPLEX_STATUS                       ((uint16_t)0x0010U) /*!< PHY Duplex mask                                 */
//#define PHY_ISFR                                ((uint16_t)0x1DU)   /*!< PHY Interrupt Source Flag register Offset       */
//#define PHY_ISFR_INT4                           ((uint16_t)0x0010U) /*!< PHY Link down inturrupt                         */

int phy_8201f_init(struct bflb_device_s *emac, struct bflb_emac_phy_cfg_s *cfg);
emac_phy_status_t phy_8201f_status_get(void);

int phy_8201f_auto_negotiate(struct bflb_emac_phy_cfg_s *cfg);
int phy_8201f_link_up(struct bflb_emac_phy_cfg_s *cfg);

#endif /* __PHY_8201F_H__ */