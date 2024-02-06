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
#ifndef __PHY_8720_H__
#define __PHY_8720_H__

#include <string.h>
#include "ethernet_phy.h"

/* LAN8720 PHY Address*/
#define EMAC_PHY_ADDRESS          0x00U
#define PHY_8720_LINK_TO               ((uint32_t)0x00000FFFU)
#define PHY_8720_AUTONEGO_COMPLETED_TO ((uint32_t)0x00000FFFU)
/* Section 3: Common PHY Registers */
#define PHY_8720_BCR                     ((uint16_t)0x00U) /*!< Transceiver Basic Control Register   */
#define PHY_8720_BSR                     ((uint16_t)0x01U) /*!< Transceiver Basic Status Register    */
#define PHY_8720_BSR_100BASETXFULL       (1 << 14)
#define PHY_8720_BSR_100BASETXHALF       (1 << 13)
#define PHY_8720_BSR_10BASETXFULL        (1 << 12)
#define PHY_8720_BSR_10BASETXHALF        (1 << 11)
#define PHY_8720_RESET                   ((uint16_t)0x8000U) /*!< PHY Reset */
#define PHY_8720_LOOPBACK                ((uint16_t)0x4000U) /*!< Select loop-back mode */
#define PHY_8720_FULLDUPLEX_100M         ((uint16_t)0x2100U) /*!< Set the full-duplex mode at 100 Mb/s */
#define PHY_8720_HALFDUPLEX_100M         ((uint16_t)0x2000U) /*!< Set the half-duplex mode at 100 Mb/s */
#define PHY_8720_FULLDUPLEX_10M          ((uint16_t)0x0100U) /*!< Set the full-duplex mode at 10 Mb/s  */
#define PHY_8720_HALFDUPLEX_10M          ((uint16_t)0x0000U) /*!< Set the half-duplex mode at 10 Mb/s  */
#define PHY_8720_AUTONEGOTIATION         ((uint16_t)0x1000U) /*!< Enable auto-negotiation function     */
#define PHY_8720_RESTART_AUTONEGOTIATION ((uint16_t)0x0200U) /*!< Restart auto-negotiation function    */
#define PHY_8720_POWERDOWN               ((uint16_t)0x0800U) /*!< Select the power down mode           */
#define PHY_8720_ISOLATE                 ((uint16_t)0x0400U) /*!< Isolate PHY from MII                 */
#define PHY_8720_AUTONEGO_COMPLETE       ((uint16_t)0x0020U) /*!< Auto-Negotiation process completed   */
#define PHY_8720_LINKED_STATUS           ((uint16_t)0x0004U) /*!< Valid link established               */
#define PHY_8720_JABBER_DETECTION        ((uint16_t)0x0002U) /*!< Jabber condition detected            */

#define PHY_8720_PHYID1                  ((uint16_t)0x02U) /*!< PHY ID 1    */
#define PHY_8720_PHYID2                  ((uint16_t)0x03U) /*!< PHY ID 2    */
#define PHY_8720_ADVERTISE               ((uint16_t)0x04U) /*!< Auto-negotiation advertisement       */
#define PHY_8720_ADVERTISE_100BASETXFULL (1 << 8)
#define PHY_8720_ADVERTISE_100BASETXHALF (1 << 7)
#define PHY_8720_ADVERTISE_10BASETXFULL  (1 << 6)
#define PHY_8720_ADVERTISE_10BASETXHALF  (1 << 5)
#define PHY_8720_ADVERTISE_8023          (1 << 0)
#define PHY_8720_LPA                     ((uint16_t)0x05U) /*!< Auto-negotiation link partner base page ability    */
#define PHY_8720_EXPANSION               ((uint16_t)0x06U) /*!< Auto-negotiation expansion           */

// Special Modes Register
#define PHY_8720_SMR                     ((uint16_t)0x12U) /*!< Special Modes Register                */
#define PHY_8720_SMR_MODE_OFFSET         (5)               /*!< MODE 0x000 10H; 0x001 10F; 0x010 100H; 0x011 100F; 0x100 100HA; 0x101 100HAR; 0x110 PDM; 0x111 all capable Auto-negotiation */
#define PHY_8720_SMR_MODE_MASK           (0x7 << PHY_8720_SMR_MODE_OFFSET)
#define PHY_8720_SMR_MODE_10H_NA         (0x0 << PHY_8720_SMR_MODE_OFFSET)
#define PHY_8720_SMR_MODE_10F_NA         (0x1 << PHY_8720_SMR_MODE_OFFSET)
#define PHY_8720_SMR_MODE_100H_NA        (0x2 << PHY_8720_SMR_MODE_OFFSET)
#define PHY_8720_SMR_MODE_100F_NA        (0x3 << PHY_8720_SMR_MODE_OFFSET)
#define PHY_8720_SMR_MODE_100H_AO        (0x4 << PHY_8720_SMR_MODE_OFFSET)
#define PHY_8720_SMR_MODE_100HR_AO       (0x5 << PHY_8720_SMR_MODE_OFFSET)
#define PHY_8720_SMR_MODE_PDM            (0x6 << PHY_8720_SMR_MODE_OFFSET)
#define PHY_8720_SMR_MODE_ALL_CAPABLE    (0x7 << PHY_8720_SMR_MODE_OFFSET)
#define PHY_8720_SMR_PHYAD_OFFSET        (0x0)

/* Section 4: Extended PHY Registers */
#define PHY_8720_CTRL_STATUS                         ((uint16_t)17U)   /*!< PHY MODE CONTROL/STATUS REGISTER Offset     */
#define PHY_8720_CTRL_STATUS_EDPWRDOWN               (1 << 13)         /*!< EDPWRDOWN */
#define PHY_8720_CTRL_STATUS_ENERGYON                (1 << 1)          /*!< ENERGYON */
#define PHY_8720_SPECIAL_MODES                       ((uint16_t)18U)   /*!< PHY SPECIAL MODES REGISTER Offset     */
#define PHY_8720_SPECIAL_MODES_MODE                  (7 << 5)          /*!< Transceiver mode of operation */
#define PHY_8720_SPECIAL_MODES_MODE_PWRDOWN          (6 << 5)          /*!< All capable. Auto-negotiation enabled */
#define PHY_8720_SPECIAL_MODES_MODE_ALL              (7 << 5)          /*!< All capable. Auto-negotiation enabled */
#define PHY_8720_SPECIAL_MODES_PHYAD                 (0x1F << 0)       /*!< PHY Address */
#define PHY_8720_SR                                  ((uint16_t)0x1FU) /*!< PHY special control/ status register Offset     */
#define PHY_8720_SR_SPEED_OFFSET                     (2)
#define PHY_8720_SR_SPEED_MASK                       (0x7 << PHY_8720_SR_SPEED_OFFSET)
#define PHY_8720_SR_SPEED_10BASETXHALF               (0x1 << PHY_8720_SR_SPEED_OFFSET)
#define PHY_8720_SR_SPEED_10BASETXFULL               (0x5 << PHY_8720_SR_SPEED_OFFSET)
#define PHY_8720_SR_SPEED_100BASETXHALF              (0x2 << PHY_8720_SR_SPEED_OFFSET)
#define PHY_8720_SR_SPEED_100BASETXFULL              (0x6 << PHY_8720_SR_SPEED_OFFSET)
#define PHY_8720_SR_SPEED_MODE_COMPARE(status, mode) (!!(mode == (status & PHY_8720_SR_SPEED_MASK)))
#define PHY_8720_SPEED_STATUS                        ((uint16_t)0x0004U) /*!< PHY Speed mask                                  */
#define PHY_8720_DUPLEX_STATUS                       ((uint16_t)0x0010U) /*!< PHY Duplex mask                                 */
#define PHY_8720_ISFR                                ((uint16_t)0x1DU)   /*!< PHY Interrupt Source Flag register Offset       */
#define PHY_8720_ISFR_INT4                           ((uint16_t)0x0010U) /*!< PHY Link down inturrupt                         */

int phy_8720_init(struct bflb_device_s *emac, struct bflb_emac_phy_cfg_s *cfg);
emac_phy_status_t phy_8720_status_get(void);

#endif /* __PHY_8720_H__ */