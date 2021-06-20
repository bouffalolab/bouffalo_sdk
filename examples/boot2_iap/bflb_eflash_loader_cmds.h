/**
  ******************************************************************************
  * @file    blsp_eflash_loader_cmds.h
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __BFLB_EFLASH_LOADER_CMDS_H__
#define __BFLB_EFLASH_LOADER_CMDS_H__

#include "stdint.h"

#define BFLB_EFLASH_LOADER_CMD_GET_BOOTINFO    0x0010
#define BFLB_EFLASH_LOADER_CMD_LOAD_BOOTHEADER 0x0011
#define BFLB_EFLASH_LOADER_CMD_LOAD_PUBLICKEY  0x0012
#define BFLB_EFLASH_LOADER_CMD_LOAD_PUBLICKEY2 0x0013
#define BFLB_EFLASH_LOADER_CMD_LOAD_SIGNATURE  0x0014
#define BFLB_EFLASH_LOADER_CMD_LOAD_SIGNATURE2 0x0015
#define BFLB_EFLASH_LOADER_CMD_LOAD_AESIV      0x0016
#define BFLB_EFLASH_LOADER_CMD_LOAD_SEGHEADER  0x0017
#define BFLB_EFLASH_LOADER_CMD_LOAD_SEGDATA    0x0018
#define BFLB_EFLASH_LOADER_CMD_CHECK_IMG       0x0019
#define BFLB_EFLASH_LOADER_CMD_RUN             0x001A

#define BFLB_EFLASH_LOADER_CMD_CHANGE_RATE 0x0020
#define BFLB_EFLASH_LOADER_CMD_RESET       0x0021

#define BFLB_EFLASH_LOADER_CMD_FLASH_ERASE       0x0030
#define BFLB_EFLASH_LOADER_CMD_FLASH_WRITE       0x0031
#define BFLB_EFLASH_LOADER_CMD_FLASH_READ        0x0032
#define BFLB_EFLASH_LOADER_CMD_FLASH_BOOT        0x0033
#define BFLB_EFLASH_LOADER_CMD_FLASH_WRITE_CHECK 0x003A
#define BFLB_EFLASH_LOADER_CMD_FLASH_SET_PARA    0x003B
#define BFLB_EFLASH_LOADER_CMD_FLASH_CHIPERASE   0x003C
#define BFLB_EFLASH_LOADER_CMD_FLASH_READSHA     0x003D
#define BFLB_EFLASH_LOADER_CMD_FLASH_XIP_READSHA 0x003E

#define BFLB_EFLASH_LOADER_CMD_FLASH_XIP_READ      0x0034
#define BFLB_EFLASH_LOADER_CMD_FLASH_SBUS_XIP_READ 0x0035

#define BFLB_EFLASH_LOADER_CMD_FLASH_READ_JEDECID     0x0036
#define BFLB_EFLASH_LOADER_CMD_FLASH_READ_STATUS_REG  0x0037
#define BFLB_EFLASH_LOADER_CMD_FLASH_WRITE_STATUS_REG 0x0038

#define BFLB_EFLASH_LOADER_CMD_EFUSE_WRITE         0x0040
#define BFLB_EFLASH_LOADER_CMD_EFUSE_READ          0x0041
#define BFLB_EFLASH_LOADER_CMD_EFUSE_READ_MAC_ADDR 0x0042

#define BFLB_EFLASH_LOADER_CMD_MEM_WRITE 0x0050
#define BFLB_EFLASH_LOADER_CMD_MEM_READ  0x0051

#define BFLB_EFLASH_LOADER_CMD_LOG_READ 0x0071

#define BFLB_EFLASH_LOADER_CMD_XIP_READ_START  0x0060
#define BFLB_EFLASH_LOADER_CMD_XIP_READ_FINISH 0x0061

#define BFLB_EFLASH_LOADER_CMD_GET_ECDH_PK     0x0090
#define BFLB_EFLASH_LOADER_CMD_ECDH_CHANLLENGE 0x0091

/* public key hash type and length */
#define BFLB_EFLASH_LOADER_PK_HASH_TYPE BFLB_HASH_TYPE_SHA256
#define BFLB_EFLASH_LOADER_PK_HASH_SIZE 256 / 8
#define BFLB_EFLASH_LOADER_HASH_TYPE    BFLB_HASH_TYPE_SHA256
#define BFLB_EFLASH_LOADER_AES_TYPE     BFLB_CRYPT_TYPE_AES_CBC
#define BFLB_EFLASH_LOADER_HASH_SIZE    256 / 8

void bflb_eflash_loader_cmd_init(void);
void bflb_eflash_loader_cmd_disable(uint8_t cmdid);
void bflb_eflash_loader_cmd_enable(uint8_t cmdid);
int32_t bflb_eflash_loader_cmd_process(uint8_t cmdid, uint8_t *data, uint16_t len);

typedef int32_t (*pfun_cmd_process)(uint16_t cmd, uint8_t *data, uint16_t len);

struct eflash_loader_cmd_cfg_t {
    uint8_t cmd;
    uint8_t enabled;
    pfun_cmd_process cmd_process;
};

#endif
