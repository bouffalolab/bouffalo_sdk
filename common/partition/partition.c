/**
 * @file partition.c
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
#include "partition.h"
#include "softcrc.h"
#include "misc.h"
/** @addtogroup  BFLB_Common_Driver
 *  @{
 */

/** @addtogroup  PARTITION
 *  @{
 */

/** @defgroup  PARTITION_Private_Macros
 *  @{
 */
#undef MSG_ERR
#define MSG_ERR(...)

/*@} end of group PARTITION_Private_Macros */

/** @defgroup  PARTITION_Private_Types
 *  @{
 */

/*@} end of group PARTITION_Private_Types */

/** @defgroup  PARTITION_Private_Variables
 *  @{
 */
pPtTable_Flash_Erase PtTable_Flash_Erase=NULL;
pPtTable_Flash_Write PtTable_Flash_Write=NULL;
pPtTable_Flash_Read PtTable_Flash_Read=NULL;

/*@} end of group PARTITION_Private_Variables */

/** @defgroup  PARTITION_Global_Variables
 *  @{
 */
extern int main(void);

/*@} end of group PARTITION_Global_Variables */

/** @defgroup  PARTITION_Private_Fun_Declaration
 *  @{
 */

/*@} end of group PARTITION_Private_Fun_Declaration */

/** @defgroup  PARTITION_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief  Judge partition table valid
 *
 * @param  ptStuff: Partition table stuff pointer
 *
 * @return 0 for invalid and 1 for valid
 *
*******************************************************************************/
static uint8_t PtTable_Valid(PtTable_Stuff_Config *ptStuff)
{
    PtTable_Config *ptTable=&ptStuff->ptTable;
    PtTable_Entry_Config *ptEntries=ptStuff->ptEntries;
    uint32_t *pCrc32;
    uint32_t entriesLen=sizeof(PtTable_Entry_Config)*ptTable->entryCnt;

    if(ptTable->magicCode==BFLB_PT_MAGIC_CODE){
        if (ptTable->entryCnt > PT_ENTRY_MAX) {
            MSG_ERR("PT Entry Count Error\r\n");
            return 0;
        }
        if(ptTable->crc32!=
            BFLB_Soft_CRC32((uint8_t*)ptTable,sizeof(PtTable_Config)-4)){
            MSG_ERR("PT CRC Error\r\n");
            return 0;
        }
        pCrc32=(uint32_t *)((uintptr_t)ptEntries+entriesLen);
        if(*pCrc32!=BFLB_Soft_CRC32((uint8_t*)ptEntries,entriesLen)){
            MSG_ERR("PT Entry CRC Error\r\n");
            return 0;
        }
        return 1;
    }
    return 0;
}

/*@} end of group PARTITION_Private_Functions */

/** @defgroup  PARTITION_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief  Register partition flash read write erase fucntion
 *
 * @param  erase: Flash erase function
 * @param  write: Flash write function
 * @param  read: Flash read function
 *
 * @return None
 *
*******************************************************************************/
void PtTable_Set_Flash_Operation(pPtTable_Flash_Erase erase,pPtTable_Flash_Write write,pPtTable_Flash_Read read)
{
    PtTable_Flash_Erase=erase;
    PtTable_Flash_Write=write;
    PtTable_Flash_Read=read;
}

/****************************************************************************//**
 * @brief  Get active partition table whole stuff
 *
 * @param  ptStuff[2]: Partition table stuff pointer
 *
 * @return Active partition table ID
 *
*******************************************************************************/
PtTable_ID_Type PtTable_Get_Active_Partition_Need_Lock(PtTable_Stuff_Config ptStuff[2])
{
    uint32_t ptValid[2]={0,0};
    PtTable_ID_Type activePtID;

    if(ptStuff==NULL){
        return PT_TABLE_ID_INVALID;
    }
    activePtID=PT_TABLE_ID_INVALID;

    PtTable_Flash_Read(BFLB_PT_TABLE0_ADDRESS,(uint8_t *)&ptStuff[0],sizeof(PtTable_Stuff_Config));
    ptValid[0]=PtTable_Valid(&ptStuff[0]);

    PtTable_Flash_Read(BFLB_PT_TABLE1_ADDRESS,(uint8_t *)&ptStuff[1],sizeof(PtTable_Stuff_Config));
    ptValid[1]=PtTable_Valid(&ptStuff[1]);

    if(ptValid[0]==1 && ptValid[1]==1){
        if(ptStuff[0].ptTable.age>=ptStuff[1].ptTable.age){
            activePtID=PT_TABLE_ID_0;
        }else{
            activePtID=PT_TABLE_ID_1;
        }
    }else if(ptValid[0]==1){
        activePtID=PT_TABLE_ID_0;
    }else if(ptValid[1]==1){
        activePtID=PT_TABLE_ID_1;
    }
    return activePtID;
}

/****************************************************************************//**
 * @brief  Get partition entry according to entry ID
 *
 * @param  ptStuff: Partition table stuff pointer
 * @param  type: Type of partition entry
 * @param  ptEntry: Partition entry pointer to store read data
 *
 * @return PT_ERROR_SUCCESS or PT_ERROR_ENTRY_NOT_FOUND or PT_ERROR_PARAMETER
 *
*******************************************************************************/
PtTable_Error_Type PtTable_Get_Active_Entries_By_ID(PtTable_Stuff_Config *ptStuff,
                                            PtTable_Entry_Type type,
                                            PtTable_Entry_Config *ptEntry)
{
    uint32_t i=0;

    if(ptStuff==NULL||ptEntry==NULL){
        return PT_ERROR_PARAMETER;
    }
    for (i=0; i < ptStuff->ptTable.entryCnt; i++) {
        if (ptStuff->ptEntries[i].type == type){
            ARCH_MemCpy_Fast(ptEntry,&ptStuff->ptEntries[i],sizeof(PtTable_Entry_Config));
            return PT_ERROR_SUCCESS;
        }
    }
    return PT_ERROR_ENTRY_NOT_FOUND;
}

/****************************************************************************//**
 * @brief  Get partition entry according to entry name
 *
 * @param  ptStuff: Partition table stuff pointer
 * @param  name: Name of partition entry
 * @param  ptEntry: Partition entry pointer to store read data
 *
 * @return PT_ERROR_SUCCESS or PT_ERROR_ENTRY_NOT_FOUND or PT_ERROR_PARAMETER
 *
*******************************************************************************/
PtTable_Error_Type PtTable_Get_Active_Entries_By_Name(PtTable_Stuff_Config *ptStuff,
                                            uint8_t *name,
                                            PtTable_Entry_Config *ptEntry)
{
    uint32_t i=0;
    uint32_t len=strlen((char *)name);

    if(ptStuff==NULL||ptEntry==NULL){
        return PT_ERROR_PARAMETER;
    }
    for (i=0; i < ptStuff->ptTable.entryCnt; i++) {
        if (strlen((char *)ptStuff->ptEntries[i].name) == len &&
            memcmp((char *)ptStuff->ptEntries[i].name,(char *)name,len) == 0){
            ARCH_MemCpy_Fast(ptEntry,&ptStuff->ptEntries[i],sizeof(PtTable_Entry_Config));
            return PT_ERROR_SUCCESS;
        }
    }
    return PT_ERROR_ENTRY_NOT_FOUND;
}

/****************************************************************************//**
 * @brief  Update partition entry
 *
 * @param  targetTableID: Target partition table to update
 * @param  ptStuff: Partition table stuff pointer
 * @param  ptEntry: Partition entry pointer to update
 *
 * @return Partition update result
 *
*******************************************************************************/
PtTable_Error_Type PtTable_Update_Entry(PtTable_ID_Type targetTableID,
                                        PtTable_Stuff_Config *ptStuff,
                                        PtTable_Entry_Config *ptEntry)
{
    uint32_t i=0;
    int32_t ret;
    uint32_t writeAddr;
    uint32_t entriesLen;
    PtTable_Config *ptTable;
    PtTable_Entry_Config *ptEntries;
    uint32_t *pCrc32;

    if(ptEntry==NULL||ptStuff==NULL){
        return PT_ERROR_PARAMETER;
    }

    ptTable=&ptStuff->ptTable;
    ptEntries=ptStuff->ptEntries;

    if(targetTableID==PT_TABLE_ID_INVALID){
        return PT_ERROR_TABLE_NOT_VALID;
    }

    if(targetTableID==PT_TABLE_ID_0){
        writeAddr=BFLB_PT_TABLE0_ADDRESS;
    }else{
        writeAddr=BFLB_PT_TABLE1_ADDRESS;
    }
    for (i=0; i < ptTable->entryCnt; i++) {
        if (ptEntries[i].type == ptEntry->type){
            ARCH_MemCpy_Fast(&ptEntries[i],ptEntry,sizeof(PtTable_Entry_Config));
            break;
        }
    }
    if(i==ptTable->entryCnt){
        /* Not found this entry ,add new one */
        if(ptTable->entryCnt<PT_ENTRY_MAX){
            ARCH_MemCpy_Fast(&ptEntries[ptTable->entryCnt],ptEntry,sizeof(PtTable_Entry_Config));
            ptTable->entryCnt++;
        }else{
            return PT_ERROR_ENTRY_UPDATE_FAIL;
        }
    }

    /* Prepare write back to flash */
    /* Update age */
    ptTable->age++;
    ptTable->crc32=BFLB_Soft_CRC32((uint8_t*)ptTable,sizeof(PtTable_Config)-4);

    /* Update entries CRC */
    entriesLen=ptTable->entryCnt*sizeof(PtTable_Entry_Config);
    pCrc32=(uint32_t *)((uintptr_t)ptEntries+entriesLen);
    *pCrc32=BFLB_Soft_CRC32((uint8_t *)&ptEntries[0],entriesLen);

    /* Write back to flash */
    /* Erase flash first */
    ret=PtTable_Flash_Erase(writeAddr,writeAddr+sizeof(PtTable_Config)+entriesLen+4-1);
    if(ret!=0){
        MSG_ERR("Flash Erase error\r\n");
        return PT_ERROR_FALSH_WRITE;
    }
    /* Write flash */
    ret=PtTable_Flash_Write(writeAddr,(uint8_t *)ptStuff,sizeof(PtTable_Stuff_Config));
    if(ret!=0){
        MSG_ERR("Flash Write error\r\n");
        return PT_ERROR_FALSH_WRITE;
    }

    return PT_ERROR_SUCCESS;
}

/****************************************************************************//**
 * @brief  Create partition entry
 *
 * @param  ptID: Partition table ID
 *
 * @return Partition create result
 *
*******************************************************************************/
PtTable_Error_Type PtTable_Create(PtTable_ID_Type ptID)
{
    uint32_t writeAddr;
    int32_t ret;
    PtTable_Config ptTable;

    if(ptID==PT_TABLE_ID_INVALID){
        return PT_ERROR_TABLE_NOT_VALID;
    }

    if(ptID==PT_TABLE_ID_0){
        writeAddr=BFLB_PT_TABLE0_ADDRESS;
    }else{
        writeAddr=BFLB_PT_TABLE1_ADDRESS;
    }

    /* Prepare write back to flash */
    ptTable.magicCode=BFLB_PT_MAGIC_CODE;
    ptTable.version=0;
    ptTable.entryCnt=0;
    ptTable.age=0;
    ptTable.crc32=BFLB_Soft_CRC32((uint8_t*)&ptTable,sizeof(PtTable_Config)-4);
    /* Write back to flash */
    ret=PtTable_Flash_Erase(writeAddr,writeAddr+sizeof(PtTable_Config)-1);
    if(ret!=0){
        MSG_ERR("Flash Erase error\r\n");
        return PT_ERROR_FALSH_ERASE;
    }
    ret=PtTable_Flash_Write(writeAddr,(uint8_t *)&ptTable,sizeof(PtTable_Config));
    if(ret!=0){
        MSG_ERR("Flash Write error\r\n");
        return PT_ERROR_FALSH_WRITE;
    }
    return PT_ERROR_SUCCESS;
}

/*@} end of group PARTITION_Public_Functions */

/*@} end of group PARTITION */

/*@} end of group BFLB_Common_Driver */
