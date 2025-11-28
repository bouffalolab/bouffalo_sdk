/*
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "riscv_encoding.h"

    .section .init
    .globl __start
    .type __start, @function

__start:
    /* Disable Global Interrupt */
    csrc CSR_MSTATUS, MSTATUS_MIE

    /* Initialize GP and TP and jump table base when zcmt enabled */
    .option push
    .option norelax
    la gp, __global_pointer$

    .option pop

#ifndef CPU_NP
    /* jump to np */
    lui a0, 0xF0000
    lw t0, 0(a0)
    lui t1, 0xF9010
    addi t1, t1, -0x700
    xor t2, t0, t1
    beq t2, x0, cpu_np_start
#endif

    .weak __StackTop
    la      sp, __StackTop
    csrw    mscratch, sp

    /*
     * Set the the NMI base mnvec to share
     * with mtvec by setting CSR_MMISC_CTL
     * bit 9 NMI_CAUSE_FFF to 1
     */
    li t0, MMISC_CTL_NMI_CAUSE_FFF
    csrs CSR_MMISC_CTL, t0

    /*
     * Set Exception Entry MTVEC to default_trap_handler
     * Due to settings above, Exception and NMI
     * will share common entry.
     */
    la t0, default_trap_handler
    csrw CSR_MTVEC, t0

    /* Set the interrupt processing mode to ECLIC mode */
    li t0, 0x3f
    csrc CSR_MTVEC, t0
    csrs CSR_MTVEC, 0x3

#ifdef CONFIG_IRQ_USE_VECTOR
    /*
     * Intialize ECLIC vector interrupt
     * base address mtvt to __Vectors
     */
    la t0, __Vectors
    csrw CSR_MTVT, t0
#endif

#if defined(__riscv_vector)
    /* Enable Vector, and set state to initial */
    li t0, MSTATUS_VS
    csrc mstatus, t0
    li t0, MSTATUS_VS_INITIAL
    csrs mstatus, t0
#endif

    /* Enable mcycle and minstret counter */
    csrci CSR_MCOUNTINHIBIT, 0x5

    .size __start, . - __start

.globl _start_premain
.type _start_premain, @function
_start_premain:

    call SystemInit

    /* start load code to itcm like. */
    call     start_load

    call     System_Post_Init

    .size _start_premain, . - _start_premain

    /* BPU cold bringup need time, so enable BPU before enter to main */
    li t0, MMISC_CTL_BPU
    csrs CSR_MMISC_CTL, t0

    /* argc = argv = 0 */
    li a0, 0
    li a1, 0

    call main

__exit:
    j      __exit


#ifndef CPU_NP

cpu_np_start:    
    /* mtvec: for all exceptions and non-vector mode IRQs */
#ifdef CONFIG_IRQ_USE_VECTOR
    /* mtvt: for all vector mode IRQs */
#endif
    /* sp: */
    .weak __StackTop_NP
    la      sp, __StackTop_NP
    csrw    mscratch, sp

    call SystemInit_NP
    
    call np_board_main

    lui	a5,0x20000
    li	a4,-1
    sw	a4,1352(a5) # 20000548 <remain_wifi_ram+0x1ffe0548>
cpu_np_loop:
    j      cpu_np_loop
#endif

__exit_np:
    j      __exit_np
