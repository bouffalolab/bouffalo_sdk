/**
  ******************************************************************************
  * @file    bflb_flash_secreg_port.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
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

#ifndef _BFLB_FLASH_SECREG_PORT_H
#define _BFLB_FLASH_SECREG_PORT_H

#include <stdint.h>
#include <stdbool.h>
#include "bflb_flash.h"

#define BFLB_FLASH_SECREG_API_TYPE_GENERAL 0U
#define BFLB_FLASH_SECREG_API_TYPE_ISSI    1U
#define BFLB_FLASH_SECREG_API_TYPE_MXIC    2U

#if defined(BL602) || defined(BL702)
#define BFLB_SF_CTRL_BUF_BASE ((uint32_t)0x4000B700)
#elif defined(BL702L)
#define BFLB_SF_CTRL_BUF_BASE ((uint32_t)0x4000B600)
#elif defined(BL606P) || defined(BL808) || defined(BL616)
#define BFLB_SF_CTRL_BUF_BASE ((uint32_t)0x2000B600)
#elif defined(BL628)
#define BFLB_SF_CTRL_BUF_BASE ((uint32_t)0x20082600)
#endif

struct flash_secreg_api {
    int (*read)(const spi_flash_cfg_type *flash_cfg, uint32_t address, void *data, uint32_t len);
    int (*write)(const spi_flash_cfg_type *flash_cfg, uint32_t address, const void *data, uint32_t len);
    int (*erase)(const spi_flash_cfg_type *flash_cfg, uint32_t address);
    int (*get_lock)(const spi_flash_cfg_type *flash_cfg, uint8_t lb_count, uint8_t lb_offset, uint8_t *lb);
    int (*set_lock)(const spi_flash_cfg_type *flash_cfg, uint8_t lb_count, uint8_t lb_offset, uint8_t lb, uint8_t lb_write_len);
};

extern struct flash_secreg_api flash_secreg_apis[];

#endif
