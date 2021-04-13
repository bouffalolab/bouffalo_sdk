/**
 * @file wav_info_parser.h
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

#ifndef __WAV_INFO_PARSER__
#define __WAV_INFO_PARSER__

#include "bl_common.h"

/* RIFF (RIFF WAVE Chunk) */
typedef  struct __attribute__((packed))
{
    uint32_t  chunk_id;     
    uint32_t chunk_size;     
    uint32_t format;        
}chunk_riff_t ;

/* fmt (Format Chunk)*/
typedef struct __attribute__((packed))
{
    uint32_t chunk_id;       
    uint32_t chunk_size ;    
    uint16_t audio_format;    
    uint16_t num_of_channels;  
    uint32_t sample_rate;    
    uint32_t byte_rate;     
    uint16_t block_align;     
    uint16_t bits_per_sample; 
}chunk_format_t;

//fact (Fact Chunk)
typedef struct  __attribute__((packed))
{
    uint32_t chunk_id;     
    uint32_t chunk_size ;    
    uint32_t data_fact_size;   
}chunk_fact_t;

//data (Data Chunk)
typedef struct __attribute__((packed))
{
    uint32_t chunk_id;       
    uint32_t chunk_size ;   
}chunk_data_t;

//
typedef enum
{
    CHUNK_RIFF,
    CHUNK_FORMAT,
    CHUNK_FACT,
    CHUNK_DATA,
}mav_chunk_t;

//.wav information
typedef struct 
{
    int            chunk_riff_offset;  
    chunk_riff_t   chunk_riff;
    int            chunk_format_offset;
    chunk_format_t chunk_format;
    int            chunk_fact_offset;
    chunk_fact_t   chunk_fact;
    int            chunk_data_offset;
    chunk_data_t   chunk_data;
}wav_information_t;

int wav_data_parser(uint8_t buff[1024], wav_information_t *wav_information);

#endif