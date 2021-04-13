/**
 * @file partition.h
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
#ifndef __PARTITION_H__
#define __PARTITION_H__

#include "stdint.h"
#include "string.h"

/** @addtogroup  BFLB_Common_Driver
 *  @{
 */

/** @addtogroup  PARTITION
 *  @{
 */

/** @defgroup  PARTITION_Public_Types
 *  @{
 */

/**
 *  @brief Partition table error type definition
 */
typedef enum {
    PT_ERROR_SUCCESS,                       /*!< Partition table error type:success */
    PT_ERROR_TABLE_NOT_VALID,               /*!< Partition table error type:table not found */
    PT_ERROR_ENTRY_NOT_FOUND,               /*!< Partition table error type:entry not found */
    PT_ERROR_ENTRY_UPDATE_FAIL,             /*!< Partition table error type:entry update fail */
    PT_ERROR_CRC32,                         /*!< Partition table error type:crc32 error */
    PT_ERROR_PARAMETER,                     /*!< Partition table error type:input parameter error */
    PT_ERROR_FALSH_READ,                    /*!< Partition table error type:flash read error */
    PT_ERROR_FALSH_WRITE,                   /*!< Partition table error type:flash write error */
    PT_ERROR_FALSH_ERASE,                   /*!< Partition table error type:flash erase error */
}PtTable_Error_Type;

/**
 *  @brief Partition id type definition
 */
typedef enum {
    PT_TABLE_ID_0,                          /*!< Partition table ID 0 */
    PT_TABLE_ID_1,                          /*!< Partition table ID 1 */
    PT_TABLE_ID_INVALID,                    /*!< Partition table ID invalid */
}PtTable_ID_Type;

/**
 *  @brief Partition id type definition
 */
typedef enum {
    PT_ENTRY_FW_CPU0,                       /*!< Partition entry type:CPU0 firmware */
    PT_ENTRY_FW_CPU1,                       /*!< Partition entry type:CPU1 firmware */
    PT_ENTRY_MAX=16,                        /*!< Partition entry type:Max */
}PtTable_Entry_Type;

/**
 *  @brief Partition table config definition
 */
typedef struct {
    uint32_t magicCode;                     /*!< Partition table magic code */
    uint16_t version;                       /*!< Partition table verdion */
    uint16_t entryCnt;                      /*!< Partition table entry count */
    uint32_t age;                           /*!< Partition table age */
    uint32_t crc32;                         /*!< Partition table CRC32 value */
}PtTable_Config;

/**
 *  @brief Partition table entry config definition
 */
typedef struct {
    uint8_t type;                           /*!< Partition entry type */
    uint8_t device;                         /*!< Partition entry device */
    uint8_t activeIndex;                    /*!< Partition entry active index */
    uint8_t name[9];                        /*!< Partition entry name */
    uint32_t Address[2];                    /*!< Partition entry start address */
    uint32_t maxLen[2];                     /*!< Partition entry max length */
    uint32_t len;                           /*!< Partition entry length */
    uint32_t age;                           /*!< Partition entry age */
}PtTable_Entry_Config;

/**
 *  @brief Partition table stuff config definition
 */
typedef struct {
    PtTable_Config ptTable;                 /*!< Partition table */
    PtTable_Entry_Config ptEntries[PT_ENTRY_MAX];    /*!< Partition entries */
    uint32_t crc32;                         /*!< Partition entries crc32 */
}PtTable_Stuff_Config;

/*@} end of group PARTITION_Public_Types */

/** @defgroup  PARTITION_Public_Constants
 *  @{
 */

/** @defgroup  PTTABLE_ERROR_TYPE
 *  @{
 */
#define IS_PTTABLE_ERROR_TYPE(type)                      (((type) == PT_ERROR_SUCCESS) || \
                                                          ((type) == PT_ERROR_TABLE_NOT_VALID) || \
                                                          ((type) == PT_ERROR_ENTRY_NOT_FOUND) || \
                                                          ((type) == PT_ERROR_ENTRY_UPDATE_FAIL) || \
                                                          ((type) == PT_ERROR_CRC32) || \
                                                          ((type) == PT_ERROR_PARAMETER) || \
                                                          ((type) == PT_ERROR_FALSH_READ) || \
                                                          ((type) == PT_ERROR_FALSH_WRITE) || \
                                                          ((type) == PT_ERROR_FALSH_ERASE))

/** @defgroup  PTTABLE_ID_TYPE
 *  @{
 */
#define IS_PTTABLE_ID_TYPE(type)                         (((type) == PT_TABLE_ID_0) || \
                                                          ((type) == PT_TABLE_ID_1) || \
                                                          ((type) == PT_TABLE_ID_INVALID))

/** @defgroup  PTTABLE_ENTRY_TYPE
 *  @{
 */
#define IS_PTTABLE_ENTRY_TYPE(type)                      (((type) == PT_ENTRY_FW_CPU0) || \
                                                          ((type) == PT_ENTRY_FW_CPU1) || \
                                                          ((type) == PT_ENTRY_MAX))

/*@} end of group PARTITION_Public_Constants */

/** @defgroup  PARTITION_Public_Macros
 *  @{
 */
#define BFLB_PT_TABLE0_ADDRESS                               0xE000
#define BFLB_PT_TABLE1_ADDRESS                               0xF000
#define BFLB_PT_MAGIC_CODE                                   0x54504642
typedef int (*pPtTable_Flash_Erase)(uint32_t startaddr,uint32_t endaddr);
typedef int (*pPtTable_Flash_Write)(uint32_t addr,uint8_t *data, uint32_t len);
typedef int (*pPtTable_Flash_Read) (uint32_t addr,uint8_t *data, uint32_t len);

/*@} end of group PARTITION_Public_Macros */

/** @defgroup  PARTITION_Public_Functions
 *  @{
 */
void PtTable_Set_Flash_Operation(pPtTable_Flash_Erase erase,pPtTable_Flash_Write write,pPtTable_Flash_Read read);
PtTable_ID_Type PtTable_Get_Active_Partition_Need_Lock(PtTable_Stuff_Config ptStuff[2]);
PtTable_Error_Type PtTable_Get_Active_Entries_By_ID(PtTable_Stuff_Config *ptStuff,
                                                    PtTable_Entry_Type type,
PtTable_Entry_Config *ptEntry);
PtTable_Error_Type PtTable_Get_Active_Entries_By_Name(PtTable_Stuff_Config *ptStuff,
                                                      uint8_t *name,
PtTable_Entry_Config *ptEntry);
PtTable_Error_Type PtTable_Update_Entry(PtTable_ID_Type targetTableID,
PtTable_Stuff_Config *ptStuff,
                                        PtTable_Entry_Config *ptEntry);
PtTable_Error_Type PtTable_Create(PtTable_ID_Type ptID);

/*@} end of group PARTITION_Public_Functions */

/*@} end of group PARTITION */

/*@} end of group BFLB_Common_Driver */

#endif /* __PARTITION_H__ */
