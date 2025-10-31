/**
  ******************************************************************************
  * @file    bflb_sp_common.h
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
#ifndef __BFLB_SP_COMMON_H__
#define __BFLB_SP_COMMON_H__

#include "stdint.h"

#if defined(BL702L)
#define BFLB_BOOT2_XZ_MALLOC_BUF_SIZE 40 * 1024
#else
#define BFLB_BOOT2_XZ_MALLOC_BUF_SIZE 80 * 1024
#endif

void bflb_sp_dump_data(void *datain, int len);
void bflb_sp_boot2_jump_entry(void);
int32_t bflb_sp_mediaboot_pre_jump(void);
uint8_t bflb_sp_boot2_get_feature_flag(void);

extern uint32_t g_anti_rollback_flag[3];
extern uint32_t g_anti_ef_en,g_anti_ef_app_ver;
extern uint8_t g_malloc_buf[BFLB_BOOT2_XZ_MALLOC_BUF_SIZE];

#endif /* __BFLB_SP_COMMON_H__ */
