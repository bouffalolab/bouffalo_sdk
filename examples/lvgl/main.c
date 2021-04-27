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
// #include "drv_shell.h"
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "lv_port_disp.h"
#include "bsp_il9341.h"
#include "lvgl.h"
#include "lv_examples.h"


/* lvgl log cb */
void lv_log_print_g_cb(lv_log_level_t level, const char *path, uint32_t line, const char *name, const char *str)
{
    MSG("L:%d,",level);
    MSG("N:%s,",name);
    MSG("D:%s\r\n,",str);
}

int main(void)
{
    bflb_platform_init(0);

    lv_log_register_print_cb(lv_log_print_g_cb);

    lv_init();
    lv_port_disp_init();
    
    lv_demo_benchmark();

    while(1)
    {
        lv_task_handler();
    } 
}
