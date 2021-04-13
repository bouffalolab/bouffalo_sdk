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

#include "ff.h"
#include "wav_play_form_sd_card.h"

FATFS Fs_1; 
audio_dev_t   Audio_Dev;

void fatfs_sd_driver_register(void);

int main(void)
{
    FRESULT res;

    bflb_platform_init(0);

    fatfs_sd_driver_register();
    sd_wav_play_register(&Audio_Dev);

    res = f_mount(&Fs_1,"1:",1);
    if(res==FR_OK)
    {
        if(Audio_Dev.audio_init(&Audio_Dev,"1:wav_test/test9_44100_stereo_s32(24bit).wav") == 0)
        {
            MSG("Audio Init success\r\n");
        }else{
            MSG("Audio Init error\r\n");
        }
    }
    else
    {
        MSG("fatfs open ERROR\r\n");
    }

    /* start/stop test */
    Audio_Dev.audio_control(&Audio_Dev,AUDIO_CMD_START,NULL);
    bflb_platform_delay_ms(10000);
    Audio_Dev.audio_control(&Audio_Dev,AUDIO_CMD_STOP,NULL);
    bflb_platform_delay_ms(5000);
    Audio_Dev.audio_control(&Audio_Dev,AUDIO_CMD_START,NULL);
    bflb_platform_delay_ms(5000);
    
    /* volume test */
    for(uint32_t vol=0; vol<80; vol++)
    {
        Audio_Dev.audio_control(&Audio_Dev,AUDIO_CMD_VOLUME,(void *)vol);
        bflb_platform_delay_ms(100);
    }
    for(uint32_t vol=80; vol>0; vol--)
    {
        Audio_Dev.audio_control(&Audio_Dev,AUDIO_CMD_VOLUME,(void *)vol);
        bflb_platform_delay_ms(100);
    }

    Audio_Dev.audio_control(&Audio_Dev,AUDIO_CMD_VOLUME,(void *)30);

    BL_CASE_SUCCESS;

}




