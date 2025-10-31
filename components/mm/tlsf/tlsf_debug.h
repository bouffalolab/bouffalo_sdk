/****************************************************************************
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
 ****************************************************************************/

#ifndef __QCC74x_MM_TLSF_DEBUG_H
#define __QCC74x_MM_TLSF_DEBUG_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "mem.h"

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Dump only used memory blocks with detailed information
 * @param heap Pointer to memory heap structure
 * @return 0 on success, -1 on error
 */
int tlsf_dump_used_blocks(struct mem_heap_s *heap);

/**
 * @brief Dump only free memory blocks with TLSF-specific information
 * @param heap Pointer to memory heap structure
 * @return 0 on success, -1 on error
 */
int tlsf_dump_free_blocks(struct mem_heap_s *heap);

/**
 * @brief Dump TLSF control structure information
 * @param heap Pointer to memory heap structure
 * @return 0 on success, -1 on error
 */
int tlsf_dump_control_info(struct mem_heap_s *heap);

/**
 * @brief Perform TLSF integrity check and dump results
 * @param heap Pointer to memory heap structure
 * @return 0 on success, -1 on error
 */
int tlsf_check_and_dump(struct mem_heap_s *heap);

/**
 * @brief Comprehensive memory analysis and dump
 * @param heap Pointer to memory heap structure
 * @return 0 on success, -1 on error
 */
int tlsf_comprehensive_dump(struct mem_heap_s *heap);

#ifdef __cplusplus
}
#endif

#endif /* __QCC74x_MM_TLSF_DEBUG_H */
