/**
  ******************************************************************************
  * @file    bl702_pm.h
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
#ifndef __BL702_PM_H__
#define __BL702_PM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

enum pm_pds_sleep_level {
    PM_PDS_LEVEL_0,
    PM_PDS_LEVEL_1,
    PM_PDS_LEVEL_2,
    PM_PDS_LEVEL_3,
    PM_PDS_LEVEL_4, /*do not recommend to use*/
    PM_PDS_LEVEL_5, /*do not recommend to use*/
    PM_PDS_LEVEL_6, /*do not recommend to use*/
    PM_PDS_LEVEL_7, /*do not recommend to use*/
    PM_PDS_LEVEL_31 = 31,
};

enum pm_hbn_sleep_level {
    PM_HBN_LEVEL_0,
    PM_HBN_LEVEL_1,
    PM_HBN_LEVEL_2,
};

enum pm_event_type {
    PM_HBN_WAKEUP_EVENT_NONE,
    PM_HBN_GPIO9_WAKEUP_EVENT,
    PM_HBN_GPIO10_WAKEUP_EVENT,
    PM_HBN_GPIO11_WAKEUP_EVENT,
    PM_HBN_GPIO12_WAKEUP_EVENT,
    PM_HBN_RTC_WAKEUP_EVENT,
    PM_HBN_BOR_WAKEUP_EVENT,
    PM_HBN_ACOMP0_WAKEUP_EVENT,
    PM_HBN_ACOMP1_WAKEUP_EVENT,
};

void pm_pds_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time);
void pm_hbn_mode_enter(enum pm_hbn_sleep_level hbn_level, uint32_t sleep_time);

void pm_pds_mask_all_wakeup_src(void);
void pm_hbn_mask_all_wakeup_src(void);
void pm_set_pds_wakeup_io(uint8_t pin, uint8_t trig_mode, uint8_t pupd);
void pm_set_hbn_wakeup_io(uint8_t pin, uint8_t trig_mode, uint8_t pupd);
void pm_pds_irq_register(void);
void pm_hbn_out0_irq_register(void);
void pm_hbn_out1_irq_register(void);
void pm_set_wakeup_callback(void (*wakeup_callback)(void));

void pm_bor_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BL702_PM_H__ */