/*
 * Copyright (C) 2016-2020 Bouffalo Lab
 */

    .section .init
    .align  2
    .globl  __start
    .type   __start, %function
__start:
.option push
.option norelax
    la      gp, __global_pointer$
.option pop
    csrci   mstatus, 8

#if defined(__riscv_flen)
    /* FP: initial state */
    csrr    t0, mstatus
    li      t1, ~0x6000
    and     t0, t0, t1
    li      t1, 0x2000
    or      t0, t0, t1
    csrw    mstatus, t0
    /* csrwi   fcsr, 0 */
#endif

#ifndef CPU_NP
    /* jump to np */
    lui a0, 0xF0000 
    lw t0, 0(a0)
    lui t1, 0xE9078
    xor t2, t0, t1
    beq t2, x0, cpu_np_start
#endif
    /* mtvec: for all exceptions and non-vector mode IRQs */
    la      a0, default_trap_handler
    ori     a0, a0, 3
    csrw    mtvec, a0
#ifdef CONFIG_IRQ_USE_VECTOR
    /* mtvt: for all vector mode IRQs */
    la      a0, __Vectors
    csrw    mtvt, a0
#endif

    .weak __StackTop
    la      sp, __StackTop
    csrw    mscratch, sp

    /* Load data section removed */

    /* Clear bss section removed */

    call     SystemInit

    /* start load code to itcm like. */
    call     start_load

    call     System_Post_Init

    call     main

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

    .size   __start, . - __start

