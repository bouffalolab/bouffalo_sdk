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
#include "hal_gpio.h"

static void gpio11_int_callback(uint32_t pin)
{
    MSG("gpio rising trigger !\r\n");
}
static void gpio12_int_callback(uint32_t pin)
{
    MSG("gpio high level int !\r\n");
}

int main(void)
{
    bflb_platform_init(0);

    gpio_set_mode(GPIO_PIN_11, GPIO_SYNC_RISING_TRIGER_INT_MODE);
    gpio_attach_irq(GPIO_PIN_11, gpio11_int_callback);
    gpio_irq_enable(GPIO_PIN_11, ENABLE);
    gpio_set_mode(GPIO_PIN_12, GPIO_SYNC_HIGH_LEVEL_INT_MODE);
    gpio_attach_irq(GPIO_PIN_12, gpio12_int_callback);
    gpio_irq_enable(GPIO_PIN_12, ENABLE);
    MSG("gpio int test !\r\n");

    while (1) {
        __asm volatile("nop");
    }
}
