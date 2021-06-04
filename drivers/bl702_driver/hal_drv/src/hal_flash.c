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


static SPI_Flash_Cfg_Type g_boot2_flash_cfg;

/**
 * @brief flash_init
 * 
 * @return int 
 */
int flash_init(void)
{
    L1C_Cache_Flush(0xf);
    SF_Cfg_Get_Flash_Cfg_Need_Lock(0,&g_boot2_flash_cfg);
    g_boot2_flash_cfg.ioMode=g_boot2_flash_cfg.ioMode&0x0f;
    L1C_Cache_Flush(0xf);

    return 0;
}

/**
 * @brief read jedec id
 * 
 * @param data 
 * @return int 
 */
int flash_read_jedec_id(uint8_t *data)
{
    uint32_t jid = 0;
    XIP_SFlash_GetJedecId_Need_Lock(&g_boot2_flash_cfg,g_boot2_flash_cfg.ioMode & 0x0f,(uint8_t *)&jid);
    jid &= 0xFFFFFF;
    BL702_MemCpy(data, (void *)&jid, 4);

    return 0;
}


/**
 * @brief read xip data
 * 
 * @param addr 
 * @param data 
 * @param len 
 * @return BL_Err_Type 
 */
BL_Err_Type flash_read_xip(uint32_t addr,uint8_t *data, uint32_t len)
{
    BL702_MemCpy_Fast(data,
                      (uint8_t *)(BL702_FLASH_XIP_BASE+addr-SF_Ctrl_Get_Flash_Image_Offset()),
                      len);
    return 0;
}


/**
 * @brief write xip data
 * 
 * @param addr 
 * @param data 
 * @param len 
 * @return BL_Err_Type 
 */
BL_Err_Type flash_write_xip(uint32_t addr, uint8_t *data, uint32_t len)
{
    return XIP_SFlash_Write_With_Lock(&g_boot2_flash_cfg,g_boot2_flash_cfg.ioMode & 0x0f,addr,data,len);
}

/**
 * @brief erase xip data
 * 
 * @param startaddr 
 * @param endaddr 
 * @return BL_Err_Type 
 */
BL_Err_Type flash_erase_xip(uint32_t startaddr,uint32_t endaddr)
{
    return XIP_SFlash_Erase_With_Lock(&g_boot2_flash_cfg,g_boot2_flash_cfg.ioMode & 0x0f,startaddr,endaddr-startaddr+1);
}


/**
 * @brief set flash cache
 * 
 * @param cont_read 
 * @param cache_enable 
 * @param cache_way_disable 
 * @param flash_offset 
 * @return BL_Err_Type 
 */
BL_Err_Type ATTR_TCM_SECTION flash_set_cache(uint8_t cont_read,uint8_t cache_enable,uint8_t cache_way_disable, uint32_t flash_offset)
{
    uint32_t tmp[1];
    BL_Err_Type stat;

    /* To make it simple, exit cont read anyway */
    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);

    SFlash_Reset_Continue_Read(&g_boot2_flash_cfg);
    if(g_boot2_flash_cfg.cReadSupport==0){
        cont_read=0;
    }
    
    if(cont_read==1){
        stat=SFlash_Read(&g_boot2_flash_cfg, g_boot2_flash_cfg.ioMode &  0xf, 1, 0x00000000, (uint8_t *)tmp, sizeof(tmp));
        if(SUCCESS!=stat){
            return 0xff;
        }
    }

    /* Set default value */
    L1C_Cache_Enable_Set(0xf);


    if(cache_enable){
        SF_Ctrl_Set_Flash_Image_Offset(flash_offset);
        SFlash_Cache_Read_Enable(&g_boot2_flash_cfg, g_boot2_flash_cfg.ioMode &  0xf, cont_read, cache_way_disable);
    }


    return 0;
}

