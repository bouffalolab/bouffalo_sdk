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
#include "drv_mmheap.h"

uint8_t mmheap_default_pool[4096] __attribute__((section(".system_ram")));

void memheap_test(void)
{
	char *ptr = NULL; 	/* 内存块的指针 */

	for(int i=1;;i++)
	{
		ptr = mmheap_alloc(i*128);
				/* 如果分配成功 */
		if (ptr != NULL)
		{
			memcpy(ptr,"hello123456789123456789123456789",32);
			MSG("ptr :%s\n", ptr);
			MSG("get memory :%d byte\n", i*128);
					/* 释放内存块 */
			mmheap_free(ptr);
			MSG("free memory :%d byte\n", i*128);
			ptr = NULL;
		    bflb_platform_delay_ms(100);
		}
		else
		{
			MSG("try to get %d byte memory failed!\n", i*128);

		    return;
		}
	}

}

int main(void)
{

    bflb_platform_init(0);

    MSG("\r\n memheap test start\r\n");
    memheap_test();
    MSG("\r\n memheap test end\r\n");
    while(1)
    {

    }
}