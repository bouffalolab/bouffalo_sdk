/*
 * FreeRTOS Kernel V10.6.2
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef PORTCONTEXT_H
#define PORTCONTEXT_H

#ifndef configENABLE_FPU
    #define configENABLE_FPU 1
#endif

#if __riscv_xlen == 64
    #define portWORD_SIZE 8
    #define store_x sd
    #define load_x ld
#elif __riscv_xlen == 32
    #define store_x sw
    #define load_x lw
    #define portWORD_SIZE 4
#else
    #error Assembler did not define __riscv_xlen
#endif

#include "freertos_risc_v_chip_specific_extensions.h"

/* Only the standard core registers are stored by default.  Any additional
 * registers must be saved by the portasmSAVE_ADDITIONAL_REGISTERS and
 * portasmRESTORE_ADDITIONAL_REGISTERS macros - which can be defined in a chip
 * specific version of freertos_risc_v_chip_specific_extensions.h.  See the
 * notes at the top of portASM.S file. */
#ifdef __riscv_32e
    #define portCONTEXT_SIZE               ( 15 * portWORD_SIZE )
    #define portCRITICAL_NESTING_OFFSET    14
#else
    #define portCONTEXT_SIZE               ( 31 * portWORD_SIZE )
    #define portCRITICAL_NESTING_OFFSET    30
#endif

#if ( configENABLE_FPU == 1 )
    /* Bit [14:13] in the mstatus encode the status of FPU state which is one of
     * the following values:
     * 1. Value: 0, Meaning: Off.
     * 2. Value: 1, Meaning: Initial.
     * 3. Value: 2, Meaning: Clean.
     * 4. Value: 3, Meaning: Dirty.
     */
    #define MSTATUS_FS_MASK                 0x6000
    #define MSTATUS_FS_INITIAL              0x2000
    #define MSTATUS_FS_CLEAN                0x4000
    #define MSTATUS_FS_DIRTY                0x6000
    #define MSTATUS_FS_OFFSET               13

    #ifdef __riscv_fdiv
        #if __riscv_flen == 32
            #define load_f                  flw
            #define store_f                 fsw
        #elif __riscv_flen == 64
            #define load_f                  fld
            #define store_f                 fsd
        #else
            #error Assembler did not define __riscv_flen
        #endif

        #define portFPU_REG_SIZE                ( __riscv_flen / 8 )
        #define portFPU_REG_COUNT               33 /* 32 Floating point registers plus one CSR. */
        #define portFPU_REG_OFFSET( regIndex )  ( ( 2 * portWORD_SIZE ) + ( regIndex * portFPU_REG_SIZE ) )
        #define portFPU_CONTEXT_SIZE            ( portFPU_REG_SIZE * portFPU_REG_COUNT )
    #else
        #error configENABLE_FPU must not be set to 1 if the hardware does not have FPU
    #endif
#endif

/*-----------------------------------------------------------*/

.extern pxCurrentTCB
.extern xISRStackTop
.extern xCriticalNesting
.extern pxCriticalNesting

/* stat trap variable */
.extern vCountLastTrapCost
.extern ulLastTrapCount
.extern lLastTrapNum
.extern ulSwitchOutExtra
.extern ulSwitchInExtra
/*-----------------------------------------------------------*/

    .macro portcontexSAVE_FPU_CONTEXT
addi sp, sp, -( portFPU_CONTEXT_SIZE )
/* Store the FPU registers. */
store_f f0,  portFPU_REG_OFFSET( 0  )( sp )
store_f f1,  portFPU_REG_OFFSET( 1  )( sp )
store_f f2,  portFPU_REG_OFFSET( 2  )( sp )
store_f f3,  portFPU_REG_OFFSET( 3  )( sp )
store_f f4,  portFPU_REG_OFFSET( 4  )( sp )
store_f f5,  portFPU_REG_OFFSET( 5  )( sp )
store_f f6,  portFPU_REG_OFFSET( 6  )( sp )
store_f f7,  portFPU_REG_OFFSET( 7  )( sp )
store_f f8,  portFPU_REG_OFFSET( 8  )( sp )
store_f f9,  portFPU_REG_OFFSET( 9  )( sp )
store_f f10, portFPU_REG_OFFSET( 10 )( sp )
store_f f11, portFPU_REG_OFFSET( 11 )( sp )
store_f f12, portFPU_REG_OFFSET( 12 )( sp )
store_f f13, portFPU_REG_OFFSET( 13 )( sp )
store_f f14, portFPU_REG_OFFSET( 14 )( sp )
store_f f15, portFPU_REG_OFFSET( 15 )( sp )
store_f f16, portFPU_REG_OFFSET( 16 )( sp )
store_f f17, portFPU_REG_OFFSET( 17 )( sp )
store_f f18, portFPU_REG_OFFSET( 18 )( sp )
store_f f19, portFPU_REG_OFFSET( 19 )( sp )
store_f f20, portFPU_REG_OFFSET( 20 )( sp )
store_f f21, portFPU_REG_OFFSET( 21 )( sp )
store_f f22, portFPU_REG_OFFSET( 22 )( sp )
store_f f23, portFPU_REG_OFFSET( 23 )( sp )
store_f f24, portFPU_REG_OFFSET( 24 )( sp )
store_f f25, portFPU_REG_OFFSET( 25 )( sp )
store_f f26, portFPU_REG_OFFSET( 26 )( sp )
store_f f27, portFPU_REG_OFFSET( 27 )( sp )
store_f f28, portFPU_REG_OFFSET( 28 )( sp )
store_f f29, portFPU_REG_OFFSET( 29 )( sp )
store_f f30, portFPU_REG_OFFSET( 30 )( sp )
store_f f31, portFPU_REG_OFFSET( 31 )( sp )
csrr t0, fcsr
store_x t0,  portFPU_REG_OFFSET( 32 )( sp )
    .endm
/*-----------------------------------------------------------*/

    .macro portcontextRESTORE_FPU_CONTEXT
/* Restore the FPU registers. */
load_f f0,  portFPU_REG_OFFSET( 0  )( sp )
load_f f1,  portFPU_REG_OFFSET( 1  )( sp )
load_f f2,  portFPU_REG_OFFSET( 2  )( sp )
load_f f3,  portFPU_REG_OFFSET( 3  )( sp )
load_f f4,  portFPU_REG_OFFSET( 4  )( sp )
load_f f5,  portFPU_REG_OFFSET( 5  )( sp )
load_f f6,  portFPU_REG_OFFSET( 6  )( sp )
load_f f7,  portFPU_REG_OFFSET( 7  )( sp )
load_f f8,  portFPU_REG_OFFSET( 8  )( sp )
load_f f9,  portFPU_REG_OFFSET( 9  )( sp )
load_f f10, portFPU_REG_OFFSET( 10 )( sp )
load_f f11, portFPU_REG_OFFSET( 11 )( sp )
load_f f12, portFPU_REG_OFFSET( 12 )( sp )
load_f f13, portFPU_REG_OFFSET( 13 )( sp )
load_f f14, portFPU_REG_OFFSET( 14 )( sp )
load_f f15, portFPU_REG_OFFSET( 15 )( sp )
load_f f16, portFPU_REG_OFFSET( 16 )( sp )
load_f f17, portFPU_REG_OFFSET( 17 )( sp )
load_f f18, portFPU_REG_OFFSET( 18 )( sp )
load_f f19, portFPU_REG_OFFSET( 19 )( sp )
load_f f20, portFPU_REG_OFFSET( 20 )( sp )
load_f f21, portFPU_REG_OFFSET( 21 )( sp )
load_f f22, portFPU_REG_OFFSET( 22 )( sp )
load_f f23, portFPU_REG_OFFSET( 23 )( sp )
load_f f24, portFPU_REG_OFFSET( 24 )( sp )
load_f f25, portFPU_REG_OFFSET( 25 )( sp )
load_f f26, portFPU_REG_OFFSET( 26 )( sp )
load_f f27, portFPU_REG_OFFSET( 27 )( sp )
load_f f28, portFPU_REG_OFFSET( 28 )( sp )
load_f f29, portFPU_REG_OFFSET( 29 )( sp )
load_f f30, portFPU_REG_OFFSET( 30 )( sp )
load_f f31, portFPU_REG_OFFSET( 31 )( sp )
load_x t0,  portFPU_REG_OFFSET( 32 )( sp )
csrw fcsr, t0
addi sp, sp, ( portFPU_CONTEXT_SIZE )
    .endm
/*-----------------------------------------------------------*/

.macro portcontextSAVE_CONTEXT_INTERNAL
    addi sp, sp, -portCONTEXT_SIZE
    store_x x1,  2  * portWORD_SIZE( sp )

#ifdef CONFIG_PS_EXTEND
    /* save trapin count, use 1 register at least */
    rdcycle x1
    csrw mscratch, x1
#endif

    store_x x5,  3  * portWORD_SIZE( sp )
    store_x x6,  4  * portWORD_SIZE( sp )
    store_x x7,  5  * portWORD_SIZE( sp )
    store_x x8,  6  * portWORD_SIZE( sp )
    store_x x9,  7  * portWORD_SIZE( sp )
    store_x x10, 8  * portWORD_SIZE( sp )
    store_x x11, 9  * portWORD_SIZE( sp )
    store_x x12, 10 * portWORD_SIZE( sp )
    store_x x13, 11 * portWORD_SIZE( sp )
    store_x x14, 12 * portWORD_SIZE( sp )
    store_x x15, 13 * portWORD_SIZE( sp )
#ifndef __riscv_32e
    store_x x16, 14 * portWORD_SIZE( sp )
    store_x x17, 15 * portWORD_SIZE( sp )
    store_x x18, 16 * portWORD_SIZE( sp )
    store_x x19, 17 * portWORD_SIZE( sp )
    store_x x20, 18 * portWORD_SIZE( sp )
    store_x x21, 19 * portWORD_SIZE( sp )
    store_x x22, 20 * portWORD_SIZE( sp )
    store_x x23, 21 * portWORD_SIZE( sp )
    store_x x24, 22 * portWORD_SIZE( sp )
    store_x x25, 23 * portWORD_SIZE( sp )
    store_x x26, 24 * portWORD_SIZE( sp )
    store_x x27, 25 * portWORD_SIZE( sp )
    store_x x28, 26 * portWORD_SIZE( sp )
    store_x x29, 27 * portWORD_SIZE( sp )
    store_x x30, 28 * portWORD_SIZE( sp )
    store_x x31, 29 * portWORD_SIZE( sp )
#endif /* ifndef __riscv_32e */

    load_x t0, xCriticalNesting                                   /* Load the value of xCriticalNesting into t0. */
    store_x t0, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp ) /* Store the critical nesting value to the stack. */

#if( configENABLE_FPU == 1 )
    csrr t0, mstatus
    srl t1, t0, MSTATUS_FS_OFFSET
    andi t1, t1, 3
    addi t2, x0, 3
    bne t1, t2, 1f /* If FPU status is not dirty, do not save FPU registers. */

    portcontexSAVE_FPU_CONTEXT
1:
#endif

    csrr t0, mstatus
    store_x t0, 1 * portWORD_SIZE( sp )

    portasmSAVE_ADDITIONAL_REGISTERS /* Defined in freertos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */

#if( configENABLE_FPU == 1 )
    /* Mark the FPU as clean, if it was dirty and we saved FPU registers. */
    srl t1, t0, MSTATUS_FS_OFFSET
    andi t1, t1, 3
    addi t2, x0, 3
    bne t1, t2, 3f

    li t1, ~MSTATUS_FS_MASK
    and t0, t0, t1
    li t1, MSTATUS_FS_CLEAN
    or t0, t0, t1
    csrw mstatus, t0
3:
#endif

#ifdef CONFIG_PS_EXTEND
    /* stat last trap cost */
    la t0, vCountLastTrapCost
    jalr t0
    /* save uLastTrapCount cause to lLastTrapNum */
    csrr  a0, mcause
    li t0, 0x80000FFF
    and a0, a0, t0
    la t0, lLastTrapNum
    store_x a0, (t0)
#endif

    load_x  t0, pTrapNetCounter
    lw   t1, 0( t0 )
    addi   t1, t1, 1
    store_x t1, 0( t0 )
#ifdef CONFIG_HIGH_ISR_STACK
    li t0, 2
    bgeu t1, t0, 1f
    load_x  t0, pxCurrentTCB
    j 2f
1:
    load_x  t0, pLowIrqContext
2:
    store_x  sp, 0( t0 )
#else
    load_x  t0, pxCurrentTCB    /* Load pxCurrentTCB. */
    store_x  sp, 0( t0 )        /* Write sp to first TCB member. */
#endif

    .endm
/*-----------------------------------------------------------*/

.macro portcontextSAVE_EXCEPTION_CONTEXT
    portcontextSAVE_CONTEXT_INTERNAL
    csrr a0, mcause
    csrr a1, mepc
    addi a1, a1, 4                      /* Synchronous so update exception return address to the instruction after the instruction that generated the exception. */
    store_x a1, 0( sp )                 /* Save updated exception return address. */
    load_x sp, xISRStackTop             /* Switch to ISR stack. */
    .endm
/*-----------------------------------------------------------*/

.macro portcontextSAVE_INTERRUPT_CONTEXT
    portcontextSAVE_CONTEXT_INTERNAL
    csrr a0, mcause
    csrr a1, mepc
    store_x a1, 0( sp )                 /* Asynchronous interrupt so save unmodified exception return address. */
    load_x sp, xISRStackTop             /* Switch to ISR stack. */
    .endm
/*-----------------------------------------------------------*/

.macro portcontextRESTORE_CONTEXT
    load_x  t0, pTrapNetCounter
    lw   t1, 0 ( t0 )
    addi   t1, t1, -1
    store_x t1, 0( t0 )
#ifdef CONFIG_HIGH_ISR_STACK
    bnez    t1, 1f
    load_x  t0, pxCurrentTCB
    j 2f
1:
    load_x  t0, pLowIrqContext
2:
    load_x  sp, 0( t0 )
#else
    load_x  t1, pxCurrentTCB
    load_x  sp, 0( t1 )
#endif

    /* Load mepc with the address of the instruction in the task to run next. */
    load_x t0, 0( sp )
    csrw mepc, t0

    /* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */
    portasmRESTORE_ADDITIONAL_REGISTERS

/* Restore mstatus register. It is important to use t3 (and not t0) here as t3
 * is not clobbered by portcontextRESTORE_VPU_CONTEXT and
 * portcontextRESTORE_FPU_CONTEXT. */
    load_x  t3, 1 * portWORD_SIZE( sp )
    csrw mstatus, t3

#if( configENABLE_FPU == 1 )
    srl t1, t3, MSTATUS_FS_OFFSET
    andi t1, t1, 3
    addi t2, x0, 3
    bne t1, t2, 6f /* If FPU status is not dirty, do not restore FPU registers. */

    portcontextRESTORE_FPU_CONTEXT
6:
#endif

    load_x t0, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp )    /* Obtain xCriticalNesting value for this task from task's stack. */
    load_x t1, pxCriticalNesting                                    /* Load the address of xCriticalNesting into t1. */
    store_x t0, 0 ( t1 )                                            /* Restore the critical nesting value for this task. */

    load_x x1,  2  * portWORD_SIZE( sp )
    load_x x5,  3  * portWORD_SIZE( sp )
    load_x x6,  4  * portWORD_SIZE( sp )
    load_x x7,  5  * portWORD_SIZE( sp )
    load_x x8,  6  * portWORD_SIZE( sp )
    load_x x9,  7  * portWORD_SIZE( sp )
    load_x x10, 8  * portWORD_SIZE( sp )
    load_x x11, 9  * portWORD_SIZE( sp )
    load_x x12, 10 * portWORD_SIZE( sp )
    load_x x13, 11 * portWORD_SIZE( sp )
    load_x x14, 12 * portWORD_SIZE( sp )
    load_x x15, 13 * portWORD_SIZE( sp )
#ifndef __riscv_32e
    load_x x16, 14 * portWORD_SIZE( sp )
    load_x x17, 15 * portWORD_SIZE( sp )
    load_x x18, 16 * portWORD_SIZE( sp )
    load_x x19, 17 * portWORD_SIZE( sp )
    load_x x20, 18 * portWORD_SIZE( sp )
    load_x x21, 19 * portWORD_SIZE( sp )
    load_x x22, 20 * portWORD_SIZE( sp )
    load_x x23, 21 * portWORD_SIZE( sp )
    load_x x24, 22 * portWORD_SIZE( sp )
    load_x x25, 23 * portWORD_SIZE( sp )
    load_x x26, 24 * portWORD_SIZE( sp )
    load_x x27, 25 * portWORD_SIZE( sp )
    load_x x28, 26 * portWORD_SIZE( sp )

#ifdef CONFIG_PS_EXTEND
    /* calc cost, use 2 registers at least */
    rdcycle x31
    csrr x30, mscratch
    sub x31, x31, x30
    la x30, ulLastTrapCount
    store_x x31, (x30)

    /* test if switch, update ulSwitchInExtra, use 3 resgister at least */
    la x30, ulSwitchOutExtra
    load_x x29, (x30)
    beq x29, x0, 1f
    sub x31, x31, x29
    la x30, ulSwitchInExtra
    store_x x31, (x30)

1:
#endif

    load_x x29, 27 * portWORD_SIZE( sp )
    load_x x30, 28 * portWORD_SIZE( sp )
    load_x x31, 29 * portWORD_SIZE( sp )
#endif
    addi sp, sp, portCONTEXT_SIZE

    mret
    .endm
/*-----------------------------------------------------------*/

#endif /* PORTCONTEXT_H */
