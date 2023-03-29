/**
 * @file rtos_def.h
 * @brief
 *
 * Copyright (c) 2023 Bouffalolab team
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

#ifndef RTOS_DEF_H_
#define RTOS_DEF_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "mem.h"

#include <stdbool.h>
/*
 * DEFINITIONS
 ****************************************************************************************
 */
/// RTOS task handle
typedef TaskHandle_t        rtos_task_handle;

/// RTOS priority
typedef UBaseType_t         rtos_prio;

/// RTOS task function
typedef TaskFunction_t      rtos_task_fct;

/// RTOS queue
typedef QueueHandle_t       rtos_queue;

/// RTOS semaphore
typedef SemaphoreHandle_t   rtos_semaphore;

/// RTOS mutex
typedef SemaphoreHandle_t   rtos_mutex;

/*
 * MACROS
 ****************************************************************************************
 */
/// Macro building the prototype of a RTOS task function
#define RTOS_TASK_FCT(name)        portTASK_FUNCTION(name, env)

/// Macro building a task priority as an offset of the IDLE task priority
#define RTOS_TASK_PRIORITY(prio)  (tskIDLE_PRIORITY + (prio))

/// Macro defining a null RTOS task handle
#define RTOS_TASK_NULL             NULL

#endif // RTOS_DEF_H_
