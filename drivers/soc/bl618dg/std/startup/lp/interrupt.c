/**
 * @file interrupt.c
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
#include "bflb_core.h"
#include <csi_core.h>
#include "irq_ctx.h"

typedef void (*pFunc)(void);

struct bflb_irq_info_s g_irqvector[CONFIG_IRQ_NUM];

extern void default_trap_handler(void);
extern void default_interrupt_handler(void);
#ifdef CONFIG_IRQ_USE_VECTOR
const pFunc __Vectors[] __attribute__((section(".vector"), aligned(64))) = {
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*  3: M-mode Soft IRQ   */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*  7: M-mode Timer IRQ  */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*         */
    default_interrupt_handler, /*         */

    default_interrupt_handler, //DMA2_INT0_IRQHandler_Wrapper,              /* 16 +  0 */
    default_interrupt_handler, //DMA2_INT1_IRQHandler_Wrapper,              /* 16 +  1 */
    default_interrupt_handler, //DMA2_INT2_IRQHandler_Wrapper,              /* 16 +  2 */
    default_interrupt_handler, //DMA2_INT3_IRQHandler_Wrapper,              /* 16 +  3 */
    default_interrupt_handler, //                                           /* 16 +  4 */
    default_interrupt_handler, //                                           /* 16 +  5 */
    default_interrupt_handler, //                                           /* 16 +  6 */
    default_interrupt_handler, //                                           /* 16 +  7 */
    default_interrupt_handler, //PWM1_IRQHandler_Wrapper,                   /* 16 +  8 */
    default_interrupt_handler, //I2C2_IRQHandler_Wrapper,                   /* 16 +  9 */
    default_interrupt_handler, //Timer1_Channel0_IRQHandler_Wrapper,        /* 16 + 10 */
    default_interrupt_handler, //Timer1_Channel1_IRQHandler_Wrapper,        /* 16 + 11 */
    default_interrupt_handler, //Timer1_Channel2_IRQHandler_Wrapper,        /* 16 + 12 */
    default_interrupt_handler, //WDG1_IRQHandler_Wrapper,                   /* 16 + 13 */
    default_interrupt_handler, //UART3_IRQHandler_Wrapper,                  /* 16 + 14 */
    default_interrupt_handler, //SPI1_IRQHandler_Wrapper,                   /* 16 + 15 */
    default_interrupt_handler, //WIFI_TO_CPU_PART2_IRQHandler_Wrapper,      /* 16 + 16 */
    default_interrupt_handler, //RF_TOP_INT0,                               /* 16 + 17 */
    default_interrupt_handler, //RF_TOP_INT1,                               /* 16 + 18 */
    default_interrupt_handler, //WIFI_TBTT_Sleep_IRQHandler_Wrapper,        /* 16 + 19 */
    default_interrupt_handler, //WIFI_TBTT_Wakeup_IRQHandler_Wrapper,       /* 16 + 20 */
    default_interrupt_handler, //WIFI_IPC_IRQHandler_Wrapper,               /* 16 + 21 */
    default_interrupt_handler, //IPC0_CH0_IRQHandler_Wrapper,               /* 16 + 22 */
    default_interrupt_handler, //IPC0_CH1_IRQHandler_Wrapper,               /* 16 + 23 */
    default_interrupt_handler, //IPC1_CH0_IRQHandler_Wrapper,               /* 16 + 24 */
    default_interrupt_handler, //IPC1_CH1_IRQHandler_Wrapper,               /* 16 + 25 */
};
#endif

void exception_entry(uintptr_t *regs)
{
    unsigned long cause;
    unsigned long epc;
    unsigned long tval;

    printf("exception_entry\r\n");

    cause = READ_CSR(CSR_MCAUSE);
    printf("mcause=%08x\r\n", (int)cause);
    epc = READ_CSR(CSR_MEPC);
    printf("mepc:%08x\r\n", (int)epc);
    tval = READ_CSR(CSR_MTVAL);
    printf("mtval:%08x\r\n", (int)tval);

    cause = (cause & 0x3ff);

    const char *mcause_str[] = {
        "Instruction address misaligned",
        "Instruction access fault",
        "Illegal instruction",
        "Breakpoint",
        "Load address misaligned",
        "Load access fault",
        "Store/AMO address misaligned",
        "Store/AMO access fault",
        "Environment call from U-mode",
        "Environment call from S-mode",
        "RSVD",
        "Environment call from M-mode",
        "Instruction page fault",
        "Load page fault",
        "RSVD",
        "Store/AMO page fault"
    };

    printf("%s\r\n", mcause_str[cause & 0xf]);
    // clang-format off
#ifdef CONFIG_TRAP_DUMP_ALL_REGS
    printf("ra = 0x%08x  ", regs[REG_RA]); printf("sp = 0x%08x  ", regs[REG_SP]); printf("gp = 0x%08x  ", regs[REG_GP]);
    printf("tp = 0x%08x  ", regs[REG_TP]); printf("\n\r");

    printf("t0 = 0x%08x  ", regs[REG_T0]); printf("t1 = 0x%08x  ", regs[REG_T1]); printf("t2 = 0x%08x  ", regs[REG_T2]);
    printf("t3 = 0x%08x  ", regs[REG_T3]); printf("\n\r");
    printf("t4 = 0x%08x  ", regs[REG_T4]); printf("t5 = 0x%08x  ", regs[REG_T5]); printf("t6 = 0x%08x  ", regs[REG_T6]);
    printf("\n\r");

    printf("a0 = 0x%08x  ", regs[REG_A0]); printf("a1 = 0x%08x  ", regs[REG_A1]); printf("a2 = 0x%08x  ", regs[REG_A2]);
    printf("a3 = 0x%08x  ", regs[REG_A3]); printf("\n\r");
    printf("a4 = 0x%08x  ", regs[REG_A4]); printf("a5 = 0x%08x  ", regs[REG_A5]); printf("a6 = 0x%08x  ", regs[REG_A6]);
    printf("a7 = 0x%08x  ", regs[REG_A7]); printf("\n\r");

    printf("s0 = 0x%08x  ", regs[REG_S0]); printf("s1 = 0x%08x  ", regs[REG_S1]); printf("s2 = 0x%08x  ", regs[REG_S2]);
    printf("s3 = 0x%08x  ", regs[REG_S3]); printf("\n\r");
    printf("s4 = 0x%08x  ", regs[REG_S4]); printf("s5 = 0x%08x  ", regs[REG_S5]); printf("s6 = 0x%08x  ", regs[REG_S6]);
    printf("s7 = 0x%08x  ", regs[REG_S7]); printf("\n\r");
    printf("s8 = 0x%08x  ", regs[REG_S8]); printf("s9 = 0x%08x  ", regs[REG_S9]); printf("s10 = 0x%08x ", regs[REG_S10]);
    printf("s11 = 0x%08x  ", regs[REG_S11]); printf("\n\r");
#endif
    // clang-format on
    if ((cause == 8) || (cause == 11)) {
        epc += 4;
        WRITE_CSR(CSR_MEPC, epc);
    } else {
        while (1) {
        }
    }
}

void interrupt_entry(void)
{
    irq_callback handler;
    void *arg;
    volatile uint32_t mcause = 0UL;
    uint32_t irq_num;

    mcause = READ_CSR(CSR_MCAUSE);
    irq_num = mcause & 0x3FF;

    if (irq_num < CONFIG_IRQ_NUM) {
        handler = g_irqvector[irq_num].handler;
        arg = g_irqvector[irq_num].arg;
        if (handler) {
            handler(irq_num, arg);
        } else {
        }
    } else {
    }
}
