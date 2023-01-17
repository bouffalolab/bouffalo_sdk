/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * The FreeRTOS kernel's RISC-V port is split between the the code that is
 * common across all currently supported RISC-V chips (implementations of the
 * RISC-V ISA), and code which tailors the port to a specific RISC-V chip:
 *
 * + The code that is common to all RISC-V chips is implemented in
 *   FreeRTOS\Source\portable\GCC\RISC-V-RV32\portASM.S.  There is only one
 *   portASM.S file because the same file is used no matter which RISC-V chip is
 *   in use.
 *
 * + The code that tailors the kernel's RISC-V port to a specific RISC-V
 *   chip is implemented in freertos_risc_v_chip_specific_extensions.h.  There
 *   is one freertos_risc_v_chip_specific_extensions.h that can be used with any
 *   RISC-V chip that both includes a standard CLINT and does not add to the
 *   base set of RISC-V registers.  There are additional
 *   freertos_risc_v_chip_specific_extensions.h files for RISC-V implementations
 *   that do not include a standard CLINT or do add to the base set of RISC-V
 *   registers.
 *
 * CARE MUST BE TAKEN TO INCLDUE THE CORRECT
 * freertos_risc_v_chip_specific_extensions.h HEADER FILE FOR THE CHIP
 * IN USE.  To include the correct freertos_risc_v_chip_specific_extensions.h
 * header file ensure the path to the correct header file is in the assembler's
 * include path.
 *
 * This freertos_risc_v_chip_specific_extensions.h is for use on RISC-V chips
 * that include a standard CLINT and do not add to the base set of RISC-V
 * registers.
 *
 */
#if __riscv_xlen == 64
	#define portWORD_SIZE 8
	#define store_x sd
	#define load_x ld
	#define store_f fsd
	#define load_f fld
#elif __riscv_xlen == 32
	#define store_x sw
	#define load_x lw
	#define store_f fsw
	#define load_f flw
	#define portWORD_SIZE 4
#else
	#error Assembler did not define __riscv_xlen
#endif

#include "freertos_risc_v_chip_specific_extensions.h"

/* Check the freertos_risc_v_chip_specific_extensions.h and/or command line
definitions. */
#if defined( portasmHAS_CLINT ) && defined( portasmHAS_MTIME )
	#error The portasmHAS_CLINT constant has been deprecated.  Please replace it with portasmHAS_MTIME.  portasmHAS_CLINT and portasmHAS_MTIME cannot both be defined at once.  See https://www.freertos.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifdef portasmHAS_CLINT
	#warning The portasmHAS_CLINT constant has been deprecated.  Please replace it with portasmHAS_MTIME and portasmHAS_SIFIVE_CLINT.  For now portasmHAS_MTIME and portasmHAS_SIFIVE_CLINT are derived from portasmHAS_CLINT.  See https://www.freertos.org/Using-FreeRTOS-on-RISC-V.html
	#define portasmHAS_MTIME portasmHAS_CLINT
	#define portasmHAS_SIFIVE_CLINT portasmHAS_CLINT
#endif

#ifndef portasmHAS_MTIME
	#error freertos_risc_v_chip_specific_extensions.h must define portasmHAS_MTIME to either 1 (MTIME clock present) or 0 (MTIME clock not present).  See https://www.freertos.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef portasmHANDLE_INTERRUPT
	#error portasmHANDLE_INTERRUPT must be defined to the function to be called to handle external/peripheral interrupts.  portasmHANDLE_INTERRUPT can be defined on the assembler command line or in the appropriate freertos_risc_v_chip_specific_extensions.h header file.  https://www.freertos.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef portasmHAS_SIFIVE_CLINT
	#define portasmHAS_SIFIVE_CLINT 0
#endif

/* Only the standard core registers are stored by default.  Any additional
registers must be saved by the portasmSAVE_ADDITIONAL_REGISTERS and
portasmRESTORE_ADDITIONAL_REGISTERS macros - which can be defined in a chip
specific version of freertos_risc_v_chip_specific_extensions.h.  See the notes
at the top of this file. */
#define portCONTEXT_SIZE ( 30 * portWORD_SIZE )

.global xPortStartFirstTask
.global freertos_risc_v_trap_handler
.global Mtspend_Handler
.global pxPortInitialiseStack
.extern pxCurrentTCB
.extern ulPortTrapHandler
.extern vTaskSwitchContext
.extern xTaskIncrementTick
.extern Timer_IRQHandler
.extern pullMachineTimerCompareRegister
.extern pullNextTime
.extern uxTimerIncrementsForOneTick /* size_t type so 32-bit on 32-bit core and 64-bits on 64-bit core. */
.extern xISRStackTop
.extern portasmHANDLE_INTERRUPT

/*-----------------------------------------------------------*/
/* Enable interrupts when returning from the handler */
#define MSTATUS_PRV1 0x1880

/********************************************************************
 * Functions: vPortYield
 *
 ********************************************************************/
.global vPortYield
.type   vPortYield, %function
vPortYield:
#if __riscv_xlen == 64
    li      t0, 0xe4000000
#else
    li      t0, 0xe0000000
#endif
    li      t2, 0x1
    sw      t2, 0(t0)
    nop
    nop

    ret


.align 8
.func
freertos_risc_v_trap_handler:
Mtspend_Handler:
//	csrrw    sp, mscratch, sp
#ifndef __riscv_float_abi_soft
    addi     sp, sp, -(portWORD_SIZE * 32)
    store_f      f31, (portWORD_SIZE *  0)(sp)
    store_f      f30, (portWORD_SIZE *  1)(sp)
    store_f      f29, (portWORD_SIZE *  2)(sp)
    store_f      f28, (portWORD_SIZE *  3)(sp)
    store_f      f27, (portWORD_SIZE *  4)(sp)
    store_f      f26, (portWORD_SIZE *  5)(sp)
    store_f      f25, (portWORD_SIZE *  6)(sp)
    store_f      f24, (portWORD_SIZE *  7)(sp)
    store_f      f23, (portWORD_SIZE *  8)(sp)
    store_f      f22, (portWORD_SIZE *  9)(sp)
    store_f      f21, (portWORD_SIZE * 10)(sp)
    store_f      f20, (portWORD_SIZE * 11)(sp)
    store_f      f19, (portWORD_SIZE * 12)(sp)
    store_f      f18, (portWORD_SIZE * 13)(sp)
    store_f      f17, (portWORD_SIZE * 14)(sp)
    store_f      f16, (portWORD_SIZE * 15)(sp)
    store_f      f15, (portWORD_SIZE * 16)(sp)
    store_f      f14, (portWORD_SIZE * 17)(sp)
    store_f      f13, (portWORD_SIZE * 18)(sp)
    store_f      f12, (portWORD_SIZE * 19)(sp)
    store_f      f11, (portWORD_SIZE * 20)(sp)
    store_f      f10, (portWORD_SIZE * 21)(sp)
    store_f      f9,  (portWORD_SIZE * 22)(sp)
    store_f      f8,  (portWORD_SIZE * 23)(sp)
    store_f      f7,  (portWORD_SIZE * 24)(sp)
    store_f      f6,  (portWORD_SIZE * 25)(sp)
    store_f      f5,  (portWORD_SIZE * 26)(sp)
    store_f      f4,  (portWORD_SIZE * 27)(sp)
    store_f      f3,  (portWORD_SIZE * 28)(sp)
    store_f      f2,  (portWORD_SIZE * 29)(sp)
    store_f      f1,  (portWORD_SIZE * 30)(sp)
    store_f      f0,  (portWORD_SIZE * 31)(sp)
#endif

#ifndef __riscv_32e
    addi    sp, sp, -(portWORD_SIZE * 31)
#else
    addi    sp, sp, -(portWORD_SIZE * 15)
#endif

    store_x      x1,  (portWORD_SIZE *  0)(sp)
    store_x      x3,  (portWORD_SIZE *  1)(sp)
    store_x      x4,  (portWORD_SIZE *  2)(sp)
    store_x      x5,  (portWORD_SIZE *  3)(sp)
    store_x      x6,  (portWORD_SIZE *  4)(sp)
    store_x      x7,  (portWORD_SIZE *  5)(sp)
    store_x      x8,  (portWORD_SIZE *  6)(sp)
    store_x      x9,  (portWORD_SIZE *  7)(sp)
    store_x      x10, (portWORD_SIZE *  8)(sp)
    store_x      x11, (portWORD_SIZE *  9)(sp)
    store_x      x12, (portWORD_SIZE * 10)(sp)
    store_x      x13, (portWORD_SIZE * 11)(sp)
    store_x      x14, (portWORD_SIZE * 12)(sp)
    store_x      x15, (portWORD_SIZE * 13)(sp)
#ifndef __riscv_32e
    store_x      x16, (portWORD_SIZE * 14)(sp)
    store_x      x17, (portWORD_SIZE * 15)(sp)
    store_x      x18, (portWORD_SIZE * 16)(sp)
    store_x      x19, (portWORD_SIZE * 17)(sp)
    store_x      x20, (portWORD_SIZE * 18)(sp)
    store_x      x21, (portWORD_SIZE * 19)(sp)
    store_x      x22, (portWORD_SIZE * 20)(sp)
    store_x      x23, (portWORD_SIZE * 21)(sp)
    store_x      x24, (portWORD_SIZE * 22)(sp)
    store_x      x25, (portWORD_SIZE * 23)(sp)
    store_x      x26, (portWORD_SIZE * 24)(sp)
    store_x      x27, (portWORD_SIZE * 25)(sp)
    store_x      x28, (portWORD_SIZE * 26)(sp)
    store_x      x29, (portWORD_SIZE * 27)(sp)
    store_x      x30, (portWORD_SIZE * 28)(sp)
    store_x      x31, (portWORD_SIZE * 29)(sp)
#endif
    csrr         t0, mepc
#ifndef __riscv_32e
    store_x      t0, (portWORD_SIZE * 30)(sp)
#else
    store_x      t0, (portWORD_SIZE * 14)(sp)
#endif
    la           a1, pxCurrentTCB
    load_x       a1, (a1)
    store_x      sp, (a1)

    jal     vTaskSwitchContext

    la      a1, pxCurrentTCB
    load_x      a1, (a1)
    load_x      sp, (a1)

#if __riscv_xlen == 64
    li      t0, 0xe4000000
#else
    li      t0, 0xe0000000
#endif
    li      t2, 0x0
    sw      t2, 0(t0)

    /* Run in machine mode */
    li      t0, MSTATUS_PRV1
    csrs    mstatus, t0

#ifndef __riscv_32e
    load_x      t0, (portWORD_SIZE * 30)(sp)
#else
    load_x      t0, (portWORD_SIZE * 14)(sp)
#endif
    csrw        mepc, t0

    load_x      x1,  (portWORD_SIZE *  0)(sp)
    load_x      x3,  (portWORD_SIZE *  1)(sp)
    load_x      x4,  (portWORD_SIZE *  2)(sp)
    load_x      x5,  (portWORD_SIZE *  3)(sp)
    load_x      x6,  (portWORD_SIZE *  4)(sp)
    load_x      x7,  (portWORD_SIZE *  5)(sp)
    load_x      x8,  (portWORD_SIZE *  6)(sp)
    load_x      x9,  (portWORD_SIZE *  7)(sp)
    load_x      x10, (portWORD_SIZE *  8)(sp)
    load_x      x11, (portWORD_SIZE *  9)(sp)
    load_x      x12, (portWORD_SIZE * 10)(sp)
    load_x      x13, (portWORD_SIZE * 11)(sp)
    load_x      x14, (portWORD_SIZE * 12)(sp)
    load_x      x15, (portWORD_SIZE * 13)(sp)
#ifndef __riscv_32e
    load_x      x16, (portWORD_SIZE * 14)(sp)
    load_x      x17, (portWORD_SIZE * 15)(sp)
    load_x      x18, (portWORD_SIZE * 16)(sp)
    load_x      x19, (portWORD_SIZE * 17)(sp)
    load_x      x20, (portWORD_SIZE * 18)(sp)
    load_x      x21, (portWORD_SIZE * 19)(sp)
    load_x      x22, (portWORD_SIZE * 20)(sp)
    load_x      x23, (portWORD_SIZE * 21)(sp)
    load_x      x24, (portWORD_SIZE * 22)(sp)
    load_x      x25, (portWORD_SIZE * 23)(sp)
    load_x      x26, (portWORD_SIZE * 24)(sp)
    load_x      x27, (portWORD_SIZE * 25)(sp)
    load_x      x28, (portWORD_SIZE * 26)(sp)
    load_x      x29, (portWORD_SIZE * 27)(sp)
    load_x      x30, (portWORD_SIZE * 28)(sp)
    load_x      x31, (portWORD_SIZE * 29)(sp)
#endif

#ifndef __riscv_32e
    addi    sp, sp, (portWORD_SIZE * 31)
#else
    addi    sp, sp, (portWORD_SIZE * 15)
#endif

#ifndef __riscv_float_abi_soft
    load_f      f31,(portWORD_SIZE *  0)(sp)
    load_f      f30,(portWORD_SIZE *  1)(sp)
    load_f      f29,(portWORD_SIZE *  2)(sp)
    load_f      f28,(portWORD_SIZE *  3)(sp)
    load_f      f27,(portWORD_SIZE *  4)(sp)
    load_f      f26,(portWORD_SIZE *  5)(sp)
    load_f      f25,(portWORD_SIZE *  6)(sp)
    load_f      f24,(portWORD_SIZE *  7)(sp)
    load_f      f23,(portWORD_SIZE *  8)(sp)
    load_f      f22,(portWORD_SIZE *  9)(sp)
    load_f      f21,(portWORD_SIZE * 10)(sp)
    load_f      f20,(portWORD_SIZE * 11)(sp)
    load_f      f19,(portWORD_SIZE * 12)(sp)
    load_f      f18,(portWORD_SIZE * 13)(sp)
    load_f      f17,(portWORD_SIZE * 14)(sp)
    load_f      f16,(portWORD_SIZE * 15)(sp)
    load_f      f15,(portWORD_SIZE * 16)(sp)
    load_f      f14,(portWORD_SIZE * 17)(sp)
    load_f      f13,(portWORD_SIZE * 18)(sp)
    load_f      f12,(portWORD_SIZE * 19)(sp)
    load_f      f11,(portWORD_SIZE * 20)(sp)
    load_f      f10,(portWORD_SIZE * 21)(sp)
    load_f      f9, (portWORD_SIZE * 22)(sp)
    load_f      f8, (portWORD_SIZE * 23)(sp)
    load_f      f7, (portWORD_SIZE * 24)(sp)
    load_f      f6, (portWORD_SIZE * 25)(sp)
    load_f      f5, (portWORD_SIZE * 26)(sp)
    load_f      f4, (portWORD_SIZE * 27)(sp)
    load_f      f3, (portWORD_SIZE * 28)(sp)
    load_f      f2, (portWORD_SIZE * 29)(sp)
    load_f      f1, (portWORD_SIZE * 30)(sp)
    load_f      f0, (portWORD_SIZE * 31)(sp)

    addi        sp, sp, (portWORD_SIZE * 32)
#endif
//    csrrw    sp, mscratch, sp

    mret
    .endfunc

#if 0
.align 8
.func
freertos_risc_v_trap_handler:
	addi sp, sp, -portCONTEXT_SIZE
	store_x x1, 1 * portWORD_SIZE( sp )
	store_x x5, 2 * portWORD_SIZE( sp )
	store_x x6, 3 * portWORD_SIZE( sp )
	store_x x7, 4 * portWORD_SIZE( sp )
	store_x x8, 5 * portWORD_SIZE( sp )
	store_x x9, 6 * portWORD_SIZE( sp )
	store_x x10, 7 * portWORD_SIZE( sp )
	store_x x11, 8 * portWORD_SIZE( sp )
	store_x x12, 9 * portWORD_SIZE( sp )
	store_x x13, 10 * portWORD_SIZE( sp )
	store_x x14, 11 * portWORD_SIZE( sp )
	store_x x15, 12 * portWORD_SIZE( sp )
	store_x x16, 13 * portWORD_SIZE( sp )
	store_x x17, 14 * portWORD_SIZE( sp )
	store_x x18, 15 * portWORD_SIZE( sp )
	store_x x19, 16 * portWORD_SIZE( sp )
	store_x x20, 17 * portWORD_SIZE( sp )
	store_x x21, 18 * portWORD_SIZE( sp )
	store_x x22, 19 * portWORD_SIZE( sp )
	store_x x23, 20 * portWORD_SIZE( sp )
	store_x x24, 21 * portWORD_SIZE( sp )
	store_x x25, 22 * portWORD_SIZE( sp )
	store_x x26, 23 * portWORD_SIZE( sp )
	store_x x27, 24 * portWORD_SIZE( sp )
	store_x x28, 25 * portWORD_SIZE( sp )
	store_x x29, 26 * portWORD_SIZE( sp )
	store_x x30, 27 * portWORD_SIZE( sp )
	store_x x31, 28 * portWORD_SIZE( sp )

	csrr t0, mstatus					/* Required for MPIE bit. */
	store_x t0, 29 * portWORD_SIZE( sp )

	portasmSAVE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */

	load_x  t0, pxCurrentTCB			/* Load pxCurrentTCB. */
	store_x  sp, 0( t0 )				/* Write sp to first TCB member. */

	csrr a0, mcause
	csrr a1, mepc
	andi a2, a0, 0x3FF
handle_synchronous:
	addi a1, a1, 4						/* Synchronous so updated exception return address to the instruction after the instruction that generated the exeption. */
	store_x a1, 0( sp )					/* Save updated exception return address. */

test_if_environment_call:
	li t0, 11 							/* 11 == environment call. */
	bne a2, t0, trap_c			/* Not an M environment call, so some other exception. */
	load_x sp, xISRStackTop				/* Switch to ISR stack before function call. */
	jal vTaskSwitchContext
	j processed_source

is_exception:
	csrr t0, mcause						/* For viewing in the debugger only. */
	csrr t1, mepc						/* For viewing in the debugger only */
	csrr t2, mstatus
	j is_exception						/* No other exceptions handled yet. */

as_yet_unhandled:
	csrr t0, mcause						/* For viewing in the debugger only. */
	j as_yet_unhandled

processed_source:
	load_x  t1, pxCurrentTCB			/* Load pxCurrentTCB. */
	load_x  sp, 0( t1 )				 	/* Read sp from first TCB member. */

	/* Load mret with the address of the next instruction in the task to run next. */
	load_x t0, 0( sp )
	csrw mepc, t0

	portasmRESTORE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

	/* Load mstatus with the interrupt enable bits used by the task. */
	load_x  t0, 29 * portWORD_SIZE( sp )
	csrw mstatus, t0						/* Required for MPIE bit. */

	load_x  x1, 1 * portWORD_SIZE( sp )
	load_x  x5, 2 * portWORD_SIZE( sp )		/* t0 */
	load_x  x6, 3 * portWORD_SIZE( sp )		/* t1 */
	load_x  x7, 4 * portWORD_SIZE( sp )		/* t2 */
	load_x  x8, 5 * portWORD_SIZE( sp )		/* s0/fp */
	load_x  x9, 6 * portWORD_SIZE( sp )		/* s1 */
	load_x  x10, 7 * portWORD_SIZE( sp )	/* a0 */
	load_x  x11, 8 * portWORD_SIZE( sp )	/* a1 */
	load_x  x12, 9 * portWORD_SIZE( sp )	/* a2 */
	load_x  x13, 10 * portWORD_SIZE( sp )	/* a3 */
	load_x  x14, 11 * portWORD_SIZE( sp )	/* a4 */
	load_x  x15, 12 * portWORD_SIZE( sp )	/* a5 */
	load_x  x16, 13 * portWORD_SIZE( sp )	/* a6 */
	load_x  x17, 14 * portWORD_SIZE( sp )	/* a7 */
	load_x  x18, 15 * portWORD_SIZE( sp )	/* s2 */
	load_x  x19, 16 * portWORD_SIZE( sp )	/* s3 */
	load_x  x20, 17 * portWORD_SIZE( sp )	/* s4 */
	load_x  x21, 18 * portWORD_SIZE( sp )	/* s5 */
	load_x  x22, 19 * portWORD_SIZE( sp )	/* s6 */
	load_x  x23, 20 * portWORD_SIZE( sp )	/* s7 */
	load_x  x24, 21 * portWORD_SIZE( sp )	/* s8 */
	load_x  x25, 22 * portWORD_SIZE( sp )	/* s9 */
	load_x  x26, 23 * portWORD_SIZE( sp )	/* s10 */
	load_x  x27, 24 * portWORD_SIZE( sp )	/* s11 */
	load_x  x28, 25 * portWORD_SIZE( sp )	/* t3 */
	load_x  x29, 26 * portWORD_SIZE( sp )	/* t4 */
	load_x  x30, 27 * portWORD_SIZE( sp )	/* t5 */
	load_x  x31, 28 * portWORD_SIZE( sp )	/* t6 */
	addi sp, sp, portCONTEXT_SIZE

	mret
	.endfunc
/*-----------------------------------------------------------*/
#endif

.align 8
.func
xPortStartFirstTask:

#if( portasmHAS_SIFIVE_CLINT != 0 )
	/* If there is a clint then interrupts can branch directly to the FreeRTOS
	trap handler.  Otherwise the interrupt controller will need to be configured
	outside of this file. */
//	la t0, freertos_risc_v_trap_handler
//	csrw mtvec, t0
#endif /* portasmHAS_CLILNT */

	la      a0, pxCurrentTCB
    load_x      a0, (a0)
    load_x      sp, (a0)

    /* Run in machine mode */
    li      t0, MSTATUS_PRV1
    csrs    mstatus, t0

#ifndef __riscv_32e
    load_x      t0, (portWORD_SIZE * 30)(sp)
#else
    load_x      t0, (portWORD_SIZE * 14)(sp)
#endif
    csrw    mepc, t0

    load_x      x1,  (portWORD_SIZE *  0)(sp)
    load_x      x3,  (portWORD_SIZE *  1)(sp)
    load_x      x4,  (portWORD_SIZE *  2)(sp)
    load_x      x5,  (portWORD_SIZE *  3)(sp)
    load_x      x6,  (portWORD_SIZE *  4)(sp)
    load_x      x7,  (portWORD_SIZE *  5)(sp)
    load_x      x8,  (portWORD_SIZE *  6)(sp)
    load_x      x9,  (portWORD_SIZE *  7)(sp)
    load_x      x10, (portWORD_SIZE *  8)(sp)
    load_x      x11, (portWORD_SIZE *  9)(sp)
    load_x      x12, (portWORD_SIZE * 10)(sp)
    load_x      x13, (portWORD_SIZE * 11)(sp)
    load_x      x14, (portWORD_SIZE * 12)(sp)
    load_x      x15, (portWORD_SIZE * 13)(sp)
#ifndef __riscv_32e
    load_x      x16, (portWORD_SIZE * 14)(sp)
    load_x      x17, (portWORD_SIZE * 15)(sp)
    load_x      x18, (portWORD_SIZE * 16)(sp)
    load_x      x19, (portWORD_SIZE * 17)(sp)
    load_x      x20, (portWORD_SIZE * 18)(sp)
    load_x      x21, (portWORD_SIZE * 19)(sp)
    load_x      x22, (portWORD_SIZE * 20)(sp)
    load_x      x23, (portWORD_SIZE * 21)(sp)
    load_x      x24, (portWORD_SIZE * 22)(sp)
    load_x      x25, (portWORD_SIZE * 23)(sp)
    load_x      x26, (portWORD_SIZE * 24)(sp)
    load_x      x27, (portWORD_SIZE * 25)(sp)
    load_x      x28, (portWORD_SIZE * 26)(sp)
    load_x      x29, (portWORD_SIZE * 27)(sp)
    load_x      x30, (portWORD_SIZE * 28)(sp)
    load_x      x31, (portWORD_SIZE * 29)(sp)
#endif

#ifndef __riscv_32e
    addi        sp, sp, (portWORD_SIZE * 31)
#else
    addi        sp, sp, (portWORD_SIZE * 15)
#endif

#ifndef __riscv_float_abi_soft
    load_f      f31, (portWORD_SIZE *  0)(sp)
    load_f      f30, (portWORD_SIZE *  1)(sp)
    load_f      f29, (portWORD_SIZE *  2)(sp)
    load_f      f28, (portWORD_SIZE *  3)(sp)
    load_f      f27, (portWORD_SIZE *  4)(sp)
    load_f      f26, (portWORD_SIZE *  5)(sp)
    load_f      f25, (portWORD_SIZE *  6)(sp)
    load_f      f24, (portWORD_SIZE *  7)(sp)
    load_f      f23, (portWORD_SIZE *  8)(sp)
    load_f      f22, (portWORD_SIZE *  9)(sp)
    load_f      f21, (portWORD_SIZE * 10)(sp)
    load_f      f20, (portWORD_SIZE * 11)(sp)
    load_f      f19, (portWORD_SIZE * 12)(sp)
    load_f      f18, (portWORD_SIZE * 13)(sp)
    load_f      f17, (portWORD_SIZE * 14)(sp)
    load_f      f16, (portWORD_SIZE * 15)(sp)
    load_f      f15, (portWORD_SIZE * 16)(sp)
    load_f      f14, (portWORD_SIZE * 17)(sp)
    load_f      f13, (portWORD_SIZE * 18)(sp)
    load_f      f12, (portWORD_SIZE * 19)(sp)
    load_f      f11, (portWORD_SIZE * 20)(sp)
    load_f      f10, (portWORD_SIZE * 21)(sp)
    load_f      f9,  (portWORD_SIZE * 22)(sp)
    load_f      f8,  (portWORD_SIZE * 23)(sp)
    load_f      f7,  (portWORD_SIZE * 24)(sp)
    load_f      f6,  (portWORD_SIZE * 25)(sp)
    load_f      f5,  (portWORD_SIZE * 26)(sp)
    load_f      f4,  (portWORD_SIZE * 27)(sp)
    load_f      f3,  (portWORD_SIZE * 28)(sp)
    load_f      f2,  (portWORD_SIZE * 29)(sp)
    load_f      f1,  (portWORD_SIZE * 30)(sp)
    load_f      f0,  (portWORD_SIZE * 31)(sp)

    addi    sp, sp, (portWORD_SIZE * 32)
#endif

    mret
	.endfunc
/*-----------------------------------------------------------*/

/*
 * Unlike other ports pxPortInitialiseStack() is written in assembly code as it
 * needs access to the portasmADDITIONAL_CONTEXT_SIZE constant.  The prototype
 * for the function is as per the other ports:
 * StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters );
 *
 * As per the standard RISC-V ABI pxTopcOfStack is passed in in a0, pxCode in
 * a1, and pvParameters in a2.  The new top of stack is passed out in a0.
 *
 * RISC-V maps registers to ABI names as follows (X1 to X31 integer registers
 * for the 'I' profile, X1 to X15 for the 'E' profile, currently I assumed).
 *
 * Register		ABI Name	Description						Saver
 * x0			zero		Hard-wired zero					-
 * x1			ra			Return address					Caller
 * x2			sp			Stack pointer					Callee
 * x3			gp			Global pointer					-
 * x4			tp			Thread pointer					-
 * x5-7			t0-2		Temporaries						Caller
 * x8			s0/fp		Saved register/Frame pointer	Callee
 * x9			s1			Saved register					Callee
 * x10-11		a0-1		Function Arguments/return values Caller
 * x12-17		a2-7		Function arguments				Caller
 * x18-27		s2-11		Saved registers					Callee
 * x28-31		t3-6		Temporaries						Caller
 *
 * The RISC-V context is saved t FreeRTOS tasks in the following stack frame,
 * where the global and thread pointers are currently assumed to be constant so
 * are not saved:
 *
 * mstatus
 * x31
 * x30
 * x29
 * x28
 * x27
 * x26
 * x25
 * x24
 * x23
 * x22
 * x21
 * x20
 * x19
 * x18
 * x17
 * x16
 * x15
 * x14
 * x13
 * x12
 * x11
 * pvParameters
 * x9
 * x8
 * x7
 * x6
 * x5
 * portTASK_RETURN_ADDRESS
 * [chip specific registers go here]
 * pxCode
 */

/*-----------------------------------------------------------*/
