/**
  ******************************************************************************
  * @file    bflb_sp_port.h
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
#ifndef __BFLB_SP_PORT_H__
#define __BFLB_SP_PORT_H__

#include "stdint.h"
#include "bflb_flash.h"
#include "bflb_sec_sha.h"
#include "bflb_port_boot2.h"

#define MFG_START_REQUEST_OFFSET HAL_BOOT2_MFG_START_REQUEST_OFFSET
#define BFLB_SP_BOOT2_XIP_BASE      HAL_BOOT2_FLASH_XIP_BASE
#define BFLB_SP_BOOT2_ROLLBACK
#define BFLB_SP_BOOT2_SUPPORT_DECOMPRESS          HAL_BOOT2_SUPPORT_DECOMPRESS
#define BFLB_SP_BOOT2_SUPPORT_USB_IAP             HAL_BOOT2_SUPPORT_USB_IAP
#define BFLB_SP_BOOT2_SUPPORT_EFLASH_LOADER_RAM   HAL_BOOT2_SUPPORT_EFLASH_LOADER_RAM
#define BFLB_SP_BOOT2_SUPPORT_EFLASH_LOADER_FLASH HAL_BOOT2_SUPPORT_EFLASH_LOADER_FLASH
#define BFLB_SP_BOOT2_SUPPORT_SIGN_ENCRYPT        HAL_BOOT2_SUPPORT_SIGN_ENCRYPT
#define BFLB_SP_BOOT2_TCM_BASE                    0

#define BOOT2_MODE_RELEASE 0x01
#define BOOT2_MODE_DEBUG   0x02
#ifdef CONFIG_DEBUG
#define BFLB_SP_BOOT2_MODE BOOT2_MODE_DEBUG
#else
#define BFLB_SP_BOOT2_MODE BOOT2_MODE_RELEASE
#endif
#define BFLB_SP_BOOT2_CPU_GROUP_MAX     HAL_BOOT2_CPU_GROUP_MAX
#define BFLB_SP_BOOT2_CPU_MAX           HAL_BOOT2_CPU_MAX
#define BFLB_SP_BOOT2_RAM_IMG_COUNT_MAX HAL_BOOT2_RAM_IMG_COUNT_MAX

#define BFLB_BOOT2_LOG_OUT_BUF_SIZE       (2 * 1024)

void bflb_boot2_release_dump_log(void);

uint32_t bflb_sp_boot2_get_cpu_count(void);
uint8_t bflb_sp_read_power_save_mode(void);
void bflb_sp_boot2_pass_parameter(void *data, uint32_t len);
void bflb_sp_boot2_releae_other_cpu(void);
void bflb_sp_boot2_start_timer(void);
void bflb_sp_boot2_show_timer(void);
void bflb_sp_boot2_init_sec_eng_pka(void);
uint8_t bflb_sp_get_anti_rollback_flag(uint32_t *value);
uint8_t bflb_sp_set_anti_rollback_flag(uint32_t value);
uint8_t bflb_sp_clean_anti_rollback_flag();

void boot2_wdt_init(void);
void boot2_wdt_set_value(uint16_t wdt_timeout);
void boot2_wdt_feed(void);
void boot2_wdt_disable(void);

void bflb_sp_boot2_printf(const char *format, ...);

extern struct bflb_device_s *boot2_wdg;
extern struct bflb_device_s *sha;
extern struct bflb_device_s *uartx;
extern struct bflb_sha256_ctx_s ctx_sha256;
extern struct bflb_device_s *console;
extern void bflb_uart_set_console(struct bflb_device_s *dev);
extern const bflb_verinf_t boot2_ver;

#define BOOT2_MSG(a, ...)            bflb_sp_boot2_printf(a, ##__VA_ARGS__)
#define BOOT2_MSG_WAR(a, ...)        bflb_sp_boot2_printf(a, ##__VA_ARGS__)
#define BOOT2_MSG_DBG(a, ...)        bflb_sp_boot2_printf(a, ##__VA_ARGS__)
#define BOOT2_MSG_ERR(a, ...)        bflb_sp_boot2_printf(a, ##__VA_ARGS__)
#define HBN_STATUS_ANTIROLLBACK_FLAG 0x42524E41

#endif /* __BFLB_SP_PORT_H__ */
