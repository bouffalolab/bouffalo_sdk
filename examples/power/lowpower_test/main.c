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
#include "hal_power.h"
#include "hal_clock.h"
#include "shell.h"
/**
 * @brief test low power wfi function
 *
 * @param argc
 * @param argv
 */
void wfi(int argc, char *argv[])
{
    MSG("Enter Wfi Mode: System Frequency = %dMHz\r\n", system_clock_get(SYSTEM_CLOCK_ROOT_CLOCK));
    bflb_platform_delay_ms(10);
    //lp_set_clock_gate(ENABLE,CLOCK_AHB_ALL);
    //lp_power_off_dll();
    lp_enter_wfi();
}

/**
 * @brief test low power pds function
 *
 * @param argc
 * @param argv
 */
void pds(int argc, char *argv[])
{
    int sleepTim = 0;

    if (argc == 2) {
        sleepTim = atoi(argv[1]);
    } else {
        sleepTim = 0;
        MSG("Enter pds mode . RTC wake up is disable,if want to use ,please enter "
            "pds tim"
            " . \r\n");
    }

    MSG("Enter PDS Mode: System Frequency = %dMHz, wake up time is %d * 1/32k\r\n", system_clock_get(SYSTEM_CLOCK_ROOT_CLOCK), sleepTim);
    bflb_platform_delay_ms(1000);

    lp_set_clock_gate(ENABLE, CLOCK_AHB_ALL);
    lp_set_all_gpio_hz();
    lp_enter_pds(sleepTim, NULL, NULL);
}

/**
 * @brief test low power hbn function
 *
 * @param argc
 * @param argv
 */
void hbn(int argc, char *argv[])
{
    int sleepTim = 0;

    if (argc == 2) {
        sleepTim = atoi(argv[1]);
    } else {
        sleepTim = 0;
    }

    MSG("Enter HBN Mode: System Frequency = %dMHz, wake up time is %d * 1/32k\r\n", system_clock_get(SYSTEM_CLOCK_ROOT_CLOCK), sleepTim);

    bflb_platform_delay_ms(1000);

    lp_set_clock_gate(ENABLE, CLOCK_AHB_ALL);
    lp_set_all_gpio_hz();
    lp_enter_hbn(sleepTim);
}

void shell_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    uint8_t data;
    if (state == UART_EVENT_RX_FIFO) {
        data = *(uint8_t *)args;
        shell_handler(data);
    }
}

int main(void)
{
    bflb_platform_init(0);
    shell_init();
    struct device *uart = device_find("debug_log");
    if (uart) {
        device_set_callback(uart, shell_irq_callback);
        device_control(uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT));
    }

    MSG("Power Test Start:\r\n");
    MSG("Input Command Example:\r\n");
    MSG("wfi \r\n");
    MSG("pds wakeTim\r\n");
    MSG("hbn wakeTim\r\n\r\n");

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}

SHELL_CMD_EXPORT(wfi, enter wfi mode)
SHELL_CMD_EXPORT(pds, enter pds)
SHELL_CMD_EXPORT(hbn, enter hbn)