/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
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
#include "hal_sec_ecdsa.h"

/** @addtogroup  BFLB_Peripheral_Case
 *  @{
 */

/** @addtogroup  SEC_ECDH
 *  @{
 */

/** @defgroup  SEC_ECDH_Private_Macros
 *  @{
 */

/*@} end of group SEC_ECDH_Private_Macros */

/** @defgroup  SEC_ECDH_Private_Types
 *  @{
 */

/*@} end of group SEC_ECDH_Private_Types */

/** @defgroup  SEC_ECDH_Private_Variables
 *  @{
 */

/*@} end of group SEC_ECDH_Private_Variables */

/** @defgroup  SEC_ECDH_Global_Variables
 *  @{
 */

/*@} end of group SEC_ECDH_Global_Variables */

/** @defgroup  SEC_ECDH_Private_Fun_Declaration
 *  @{
 */

/*@} end of group SEC_ECDH_Private_Fun_Declaration */

/** @defgroup  SEC_ECDH_Private_Functions
 *  @{
 */

int main(void)
{
    /* We use following two case to show how ECDH works*/
    //==========================
    //Alice's secret key:   1672493697407246559549867004310108600574958080780180188771300643695147736525
    //Alice's public key:   (8186931993451804108690395451499266320135437983074928011404785122795116228120L, 42583342814291813019381020879433259376088942101048764740128506039642196631017L)
    //Bob's secret key: 80820894835688796582966945354468122770925276582929501754975915934762303146292
    //Bob's public key: (110419239604359817300303235482162221248952850592986847629047641306847745620659L, 19394459839852358891515335116028197103691369259799083720249515230794728385441L)
    //==========================
    //Alice's shared key:   (89478371667134321617252675049711550685663874249668539239111908843552332917927L, 817906898059826893312335918167859928021104862259509866199871605894193053448L)
    //Bob's shared key: (89478371667134321617252675049711550685663874249668539239111908843552332917927L, 817906898059826893312335918167859928021104862259509866199871605894193053448L)
    //==========================
    //The shared value is the x-value:  89478371667134321617252675049711550685663874249668539239111908843552332917927

    uint8_t ecdh_private_key1[32] = { 0x03, 0xb2, 0x99, 0xf, 0x18, 0x16, 0x38, 0x78, 0x88, 0xb1, 0x9, 0xfd, 0xd4, 0x33, 0x94, 0x1a,
                                      0x1f, 0x7e, 0x7c, 0x8b, 0xf7, 0xf2, 0x34, 0x15, 0xb0, 0xc1, 0x21, 0x61, 0xc0, 0xe9, 0xdd, 0xcd };
    uint8_t ecdh_public_keyx1[32] = { 0x12, 0x19, 0xa3, 0xa9, 0xd, 0xbf, 0xfa, 0xf3, 0xd5, 0x1b, 0x8c, 0xa0, 0xa6, 0xec, 0xd9, 0xa8,
                                      0xca, 0xf0, 0x6, 0xaf, 0x3b, 0x52, 0x4b, 0x9f, 0x93, 0x96, 0xcb, 0xa, 0xe6, 0xa6, 0xee, 0x18 };
    uint8_t ecdh_public_keyy1[32] = { 0x5e, 0x25, 0x51, 0x62, 0xd6, 0x5, 0x89, 0xe3, 0x65, 0x3d, 0xde, 0xf1, 0x59, 0xcb, 0xee, 0x70,
                                      0x56, 0x40, 0x71, 0x55, 0x9e, 0xec, 0xe0, 0x4, 0x81, 0xa4, 0xfc, 0x7a, 0xf, 0xe5, 0x5d, 0xe9 };
    uint8_t ecdh_shared_keyx1[32] = { 0xc5, 0xd2, 0xf6, 0xb1, 0x69, 0x8c, 0x4f, 0xff, 0x1b, 0x4a, 0x9d, 0x4, 0x4d, 0x7a, 0x3d, 0xbf,
                                      0xa5, 0xed, 0xfc, 0xd6, 0xc9, 0x12, 0xe7, 0x67, 0x32, 0x1e, 0x3f, 0x31, 0x4b, 0xf1, 0x2c, 0xa7 };

    uint8_t ecdh_private_key2[32] = { 0xb2, 0xaf, 0x1, 0x62, 0x49, 0x36, 0x12, 0xee, 0x36, 0xfb, 0x72, 0x20, 0xc9, 0xcc, 0xba, 0xe7,
                                      0x86, 0x5, 0x11, 0x84, 0x72, 0xed, 0x1c, 0xa4, 0xf8, 0x2c, 0x3a, 0xb, 0xf6, 0x9e, 0xc9, 0x34 };
    uint8_t ecdh_public_keyx2[32] = { 0xf4, 0x1f, 0x13, 0x2a, 0xf2, 0x7b, 0x67, 0x22, 0xcb, 0x6a, 0x73, 0xb5, 0x21, 0x17, 0x32, 0x32,
                                      0x7d, 0x8c, 0x42, 0x71, 0x3, 0x9d, 0x54, 0xc, 0x68, 0xdc, 0xac, 0x8b, 0x1d, 0x63, 0x3a, 0xb3 };
    uint8_t ecdh_public_keyy2[32] = { 0x2a, 0xe0, 0xe0, 0x11, 0x45, 0xbc, 0x5e, 0x1b, 0x30, 0x4, 0x1b, 0xc3, 0x45, 0x3b, 0xb9, 0xb6,
                                      0x77, 0x64, 0x6b, 0x7e, 0x96, 0x1a, 0xad, 0x22, 0xd2, 0xe4, 0x9f, 0x29, 0x8e, 0x64, 0xab, 0xa1 };
    uint8_t ecdh_shared_keyx2[32] = { 0xc5, 0xd2, 0xf6, 0xb1, 0x69, 0x8c, 0x4f, 0xff, 0x1b, 0x4a, 0x9d, 0x4, 0x4d, 0x7a, 0x3d, 0xbf,
                                      0xa5, 0xed, 0xfc, 0xd6, 0xc9, 0x12, 0xe7, 0x67, 0x32, 0x1e, 0x3f, 0x31, 0x4b, 0xf1, 0x2c, 0xa7 };

    /* Another case */
    uint8_t ecdh_private_key[32] = { 0xba, 0x25, 0xf6, 0xc1, 0xa7, 0x82, 0x9a, 0x18, 0x51, 0x08, 0x0c, 0xa8, 0x79, 0x50, 0xb9, 0xd5,
                                     0x17, 0x7a, 0x8a, 0x65, 0x72, 0xd3, 0x12, 0x87, 0x07, 0x04, 0x9c, 0x74, 0x95, 0x3e, 0xf6, 0x11 };
    uint8_t ecdh_public_keyx[32] = { 0xbe, 0xbb, 0x36, 0x3c, 0xfd, 0x53, 0x8d, 0x94, 0x86, 0x7c, 0xf5, 0xbb, 0x35, 0xd8, 0xb6, 0x7b,
                                     0x74, 0x8c, 0x7a, 0xcf, 0x05, 0xa4, 0x5c, 0x70, 0x79, 0xb4, 0x78, 0x50, 0x8e, 0x87, 0xea, 0x4c };
    uint8_t ecdh_public_keyy[32] = { 0x84, 0x3c, 0x5b, 0x9c, 0x9c, 0x6b, 0x6c, 0xca, 0xe0, 0xfe, 0x1b, 0x2b, 0xdc, 0xce, 0xb0, 0x24,
                                     0x3b, 0x4c, 0xc3, 0x58, 0x4c, 0x59, 0x28, 0x4c, 0xda, 0x26, 0x8d, 0x67, 0x9e, 0x8c, 0x29, 0xd4 };
    uint8_t ecdh_shared_keyx[32] = { 0x58, 0xa7, 0x3b, 0x0f, 0x15, 0x58, 0x4e, 0x3a, 0xed, 0x9e, 0x01, 0x3c, 0xbc, 0xdd, 0x9f, 0x56,
                                     0x16, 0xac, 0x5f, 0xe4, 0x8f, 0x2f, 0xad, 0x38, 0x98, 0x50, 0x43, 0x5e, 0x4e, 0xed, 0x74, 0x53 };
    uint32_t ecdh_keyx[8];
    uint32_t ecdh_keyy[8];

    int32_t i;
    uint8_t *p;
    sec_ecdh_handle_t ecdh_handle;

    bflb_platform_init(2000000);
    MSG("ECDH Case\r\n");

    sec_ecdh_init(&ecdh_handle, ECP_SECP256R1);

    MSG("Bob calculate encrypt key\r\n");
    bflb_platform_clear_time();
    sec_ecdh_get_encrypt_key(&ecdh_handle, (uint32_t *)ecdh_public_keyx1, (uint32_t *)ecdh_public_keyy1, (uint32_t *)ecdh_private_key2, ecdh_keyx, ecdh_keyy);
    MSG("Get encrypt key time=%dms\r\n", (unsigned int)bflb_platform_get_time_ms());

    p = (uint8_t *)ecdh_keyx;

    for (i = 0; i < 32; i++) {
        if (ecdh_shared_keyx2[i] != p[i]) {
            MSG("ECDH Compare fail\r\n");
            BL_CASE_FAIL;
        }
    }

    MSG("Alice calculate encrypt key\r\n");
    bflb_platform_clear_time();
    sec_ecdh_get_encrypt_key(&ecdh_handle, (uint32_t *)ecdh_public_keyx2, (uint32_t *)ecdh_public_keyy2, (uint32_t *)ecdh_private_key1, ecdh_keyx, ecdh_keyy);
    MSG("Get encrypt key time=%dms\r\n", (unsigned int)bflb_platform_get_time_ms());

    p = (uint8_t *)ecdh_keyx;

    for (i = 0; i < 32; i++) {
        if (ecdh_shared_keyx1[i] != p[i]) {
            MSG("ECDH Compare fail\r\n");
            BL_CASE_FAIL;
        }
    }

    MSG("Another case\r\n");
    MSG("Get Public key\r\n");
    bflb_platform_clear_time();
    sec_ecdh_get_public_key(&ecdh_handle, (uint32_t *)ecdh_private_key, ecdh_keyx, ecdh_keyy);
    MSG("Get public key time=%dms\r\n", (unsigned int)bflb_platform_get_time_ms());

    p = (uint8_t *)ecdh_keyx;

    for (i = 0; i < 32; i++) {
        if (ecdh_public_keyx[i] != p[i]) {
            MSG("ECDH Compare fail\r\n");
            BL_CASE_FAIL;
        }
    }

    p = (uint8_t *)ecdh_keyy;

    for (i = 0; i < 32; i++) {
        if (ecdh_public_keyy[i] != p[i]) {
            MSG("ECDH Compare fail\r\n");
            BL_CASE_FAIL;
        }
    }

    MSG("Get shared key\r\n");
    bflb_platform_clear_time();
    sec_ecdh_get_encrypt_key(&ecdh_handle, ecdh_keyx, ecdh_keyy, (uint32_t *)ecdh_private_key, ecdh_keyx, ecdh_keyy);
    MSG("Get share key time=%dms\r\n", (unsigned int)bflb_platform_get_time_ms());

    p = (uint8_t *)ecdh_keyx;

    for (i = 0; i < 32; i++) {
        if (ecdh_shared_keyx[i] != p[i]) {
            MSG("ECDH Compare fail\r\n");
            BL_CASE_FAIL;
        }
    }

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}

/*@} end of group SEC_ECDH_Private_Functions */

/*@} end of group SEC_ECDH */

/*@} end of group BFLB_Peripheral_Case */
