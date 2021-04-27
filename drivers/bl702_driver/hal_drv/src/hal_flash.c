/**
 * @file hal_flash.c
 * @brief 
 * 
 * Copyright 2019-2030 Bouffalolab team
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
#include "bl702_xip_sflash.h"
#include "bl702_sf_cfg.h"
#include "bl702_glb.h"
#include "hal_flash.h"

/**
 * @brief erase flash memory addr should be 4k aligned or will erase more than size Byte 
 *        each block have 4K Byte
 * 
 * @param addr flash memory block addr
 * @param size erase bytes number
 * @return int success or not
 */
int flash_erase(uint32_t addr, uint32_t size)
{

    SPI_Flash_Cfg_Type flashCfg;
    uint32_t ret = 0;

    XIP_SFlash_Opt_Enter();
    ret = SF_Cfg_Flash_Identify(1, 1, 0, 0, &flashCfg);
    XIP_SFlash_Opt_Exit();

    if ((ret & BFLB_FLASH_ID_VALID_FLAG) == 0)
    {
        return -FLASH_NOT_DETECT;
    }

    XIP_SFlash_Erase_With_Lock(&flashCfg, SF_CTRL_DO_MODE, addr, size - 1);

    return 0;
}

/**
 * @brief read data form flash
 * 
 * @param addr read flash addr
 * @param data read data pointer
 * @param size read data size
 * @return int 
 */
int flash_read(uint32_t addr, uint8_t *data, uint32_t size)
{
    SPI_Flash_Cfg_Type flashCfg;
    uint32_t ret = 0;

    XIP_SFlash_Opt_Enter();
    ret = SF_Cfg_Flash_Identify(1, 1, 0, 0, &flashCfg);
    XIP_SFlash_Opt_Exit();

    if ((ret & BFLB_FLASH_ID_VALID_FLAG) == 0)
    {
        return -FLASH_NOT_DETECT;
    }

    XIP_SFlash_Read_With_Lock(&flashCfg, SF_CTRL_DO_MODE, addr, data, size);

    return 0;
}

/**
 * @brief write data to flash
 * 
 * @param addr write flash addr
 * @param data write data pointer
 * @param size write data size
 * @return int 
 */
int flash_write(uint32_t addr, uint8_t *data, uint32_t size)
{
    SPI_Flash_Cfg_Type flashCfg;
    uint32_t ret = 0;

    XIP_SFlash_Opt_Enter();
    ret = SF_Cfg_Flash_Identify(1, 1, 0, 0, &flashCfg);
    XIP_SFlash_Opt_Exit();

    if ((ret & BFLB_FLASH_ID_VALID_FLAG) == 0)
    {
        return -FLASH_NOT_DETECT;
    }

    XIP_SFlash_Write_With_Lock(&flashCfg, SF_CTRL_DO_MODE, addr, data, size);

    return 0;
}