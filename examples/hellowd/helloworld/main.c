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

#include "hal_uart.h"

int main(void)
{
    bflb_platform_init(0);

    while(1)
    {
    	MSG("hello world!\r\n");
    	bflb_platform_delay_ms(1000);
    } 
}

#ifdef SHELL_SUPPORT
#include "drv_shell.h"
void hellowd()
{
    MSG("hello World\r\n");
}

int func(int argc, char *argv[])
{
    MSG("%dparameter(s)\r\n", argc);
    for (uint8_t i = 1; i < argc; i++)
    {
        MSG("%s\r\n", argv[i]);
    }
    return 0;
}

SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
hwd, hellowd, responds helloworld);
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
echo, func, print a char using parameter case);
#endif
