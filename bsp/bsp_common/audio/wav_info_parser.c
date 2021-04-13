/**
 * @file wav_info_parser.c
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

#include "wav_info_parser.h"

/* get File pointer from top of file*/
int wav_data_parser(uint8_t buff[1024], wav_information_t *wav_information)
{
    uint32_t offset=0;
    uint32_t chunk_id;

    /* RIFF WAVE Chunk */
    chunk_id = ((chunk_riff_t *)&buff[offset])->chunk_id;
    if(chunk_id == 0x46464952)
    {
        wav_information->chunk_riff_offset = offset;
        wav_information->chunk_riff = *((chunk_riff_t *)&buff[offset]);
        offset += sizeof(chunk_riff_t);
    }else{
        wav_information->chunk_riff_offset = -1;
        return 1;
    }
    /* Format Chunk */
    chunk_id = ((chunk_format_t *)&buff[offset])->chunk_id;
    if(chunk_id == 0x20746D66 && offset<1000) /* fmt */
    {
        wav_information->chunk_format_offset = offset;
        wav_information->chunk_format = *((chunk_format_t *)&buff[offset]);
        offset += ((chunk_format_t*)&buff[offset])->chunk_size + 8;
    }else
    {
        wav_information->chunk_format_offset = -1;
        return 1;
    }
    /* Fact/list Chunk */
    chunk_id = ((chunk_fact_t *)&buff[offset])->chunk_id;
    if((chunk_id == 0X74636166 || chunk_id == 0X5453494C) && offset<1000) /*fact or list*/
    {
        wav_information->chunk_fact_offset = offset;
        wav_information->chunk_fact = *((chunk_fact_t *)&buff[offset]);
        offset += ((chunk_fact_t*)&buff[offset])->chunk_size + 8;
    }else{
        wav_information->chunk_fact_offset = -1;
    }
    /* Data Chunk */
    chunk_id = ((chunk_data_t *)&buff[offset])->chunk_id;
    if(chunk_id == 0X61746164 && offset<1000 )
    {
        wav_information->chunk_data_offset = offset;
        wav_information->chunk_data = *((chunk_data_t *)&buff[offset]);
    }else{
        wav_information->chunk_data_offset = -1;
        return 1;
    }
    return 0;
}