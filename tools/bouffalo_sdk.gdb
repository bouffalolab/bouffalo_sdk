mem 0x00000000 0xffffffff ro
set pagination off
set confirm off

define task_dump
  if $argc == 0
    set $tcb_ptr = pxCurrentTCB
  else
    set $tcb_ptr = $arg0
  end

  p *(TCB_t *)$tcb_ptr

  set $stack_top = ((TCB_t *)$tcb_ptr).pxTopOfStack

  # Detect FPU status by reading mstatus
  # Note: mstatus is at offset 1 in both clean and dirty cases
  # because f0/fcsr are at offset 0 in FPU context
  set $mstatus = *(uint32_t *)($stack_top + 1)
  set $mepc = *(uint32_t *)$stack_top

  # Calculate FPU offset based on FS bits [14:13]
  # FS = 0x3 (dirty) means FPU context (33 words) is saved below general registers
  if $mstatus & 0x6000 == 0x6000
    set $stack_offset = 31 + 33
    printf "FPU is DIRTY (FS=0x3) - using offset +%d words\n", $stack_offset
  else
    set $stack_offset = 31
    printf "FPU is CLEAN (FS=0x%x) - using offset +%d words\n", ($mstatus >> 13) & 0x3, $stack_offset
  end
  printf "mstatus: 0x%x\n", $mstatus
  printf "stack_top: 0x%x\n\n", $stack_top

  # Restore sp register (points to top of general register area)
  set $sp = $stack_top + $stack_offset

  # x1  - ra   (return address)
  set $ra = *(uint32_t *)($stack_top + 2)
  # x5  - t0   (temporary)
  set $t0 = *(uint32_t *)($stack_top + 3)
  # x6  - t1   (temporary)
  set $t1 = *(uint32_t *)($stack_top + 4)
  # x7  - t2   (temporary)
  set $t2 = *(uint32_t *)($stack_top + 5)
  # x8  - s0/fp (saved register / frame pointer)
  set $s0 = *(uint32_t *)($stack_top + 6)
  # x9  - s1   (saved register)
  set $s1 = *(uint32_t *)($stack_top + 7)
  # x10 - a0   (function argument / return value)
  set $a0 = *(uint32_t *)($stack_top + 8)
  # x11 - a1   (function argument / return value)
  set $a1 = *(uint32_t *)($stack_top + 9)
  # x12 - a2   (function argument)
  set $a2 = *(uint32_t *)($stack_top + 10)
  # x13 - a3   (function argument)
  set $a3 = *(uint32_t *)($stack_top + 11)
  # x14 - a4   (function argument)
  set $a4 = *(uint32_t *)($stack_top + 12)
  # x15 - a5   (function argument)
  set $a5 = *(uint32_t *)($stack_top + 13)
  # x16 - a6   (function argument)
  set $a6 = *(uint32_t *)($stack_top + 14)
  # x17 - a7   (function argument)
  set $a7 = *(uint32_t *)($stack_top + 15)
  # x18 - s2   (saved register)
  set $s2 = *(uint32_t *)($stack_top + 16)
  # x19 - s3   (saved register)
  set $s3 = *(uint32_t *)($stack_top + 17)
  # x20 - s4   (saved register)
  set $s4 = *(uint32_t *)($stack_top + 18)
  # x21 - s5   (saved register)
  set $s5 = *(uint32_t *)($stack_top + 19)
  # x22 - s6   (saved register)
  set $s6 = *(uint32_t *)($stack_top + 20)
  # x23 - s7   (saved register)
  set $s7 = *(uint32_t *)($stack_top + 21)
  # x24 - s8   (saved register)
  set $s8 = *(uint32_t *)($stack_top + 22)
  # x25 - s9   (saved register)
  set $s9 = *(uint32_t *)($stack_top + 23)
  # x26 - s10  (saved register)
  set $s10 = *(uint32_t *)($stack_top + 24)
  # x27 - s11  (saved register)
  set $s11 = *(uint32_t *)($stack_top + 25)
  # x28 - t3   (temporary)
  set $t3 = *(uint32_t *)($stack_top + 26)
  # x29 - t4   (temporary)
  set $t4 = *(uint32_t *)($stack_top + 27)
  # x30 - t5   (temporary)
  set $t5 = *(uint32_t *)($stack_top + 28)
  # x31 - t6   (temporary)
  set $t6 = *(uint32_t *)($stack_top + 29)

  # PC is at offset $stack_offset + 0 (mepc)
  set $pc = $mepc - 4
  bt
end

define task_list
  set $item = $arg0.xListEnd.pxPrevious
  while ($item != &$arg0.xListEnd)
    set $tcb = (TCB_t *)$item.pvOwner
    set $stack_top = (uint32_t *)$tcb->pxTopOfStack

    # Read mstatus - it's always at offset 1 (word offset)
    # because f0 is at offset 0 in FPU context
    set $mstatus = *((uint32_t *)$stack_top + 1)

    # Check FPU status
    if $mstatus & 0x6000 == 0x6000
      set $fpu_str = "[FPU_DIRTY]"
    else
      set $fpu_str = "[FPU_CLEAN]"
    end

    printf "tcb: %p\tmstatus: 0x%X\t%s\tname: %s\n", $tcb, $mstatus, $fpu_str, $tcb->pcTaskName
    set $item = $item.pxPrevious
  end
end

define task_list_ready
  set $i = 0
  while ($i < sizeof(pxReadyTasksLists)/sizeof(struct xLIST))
      task_list pxReadyTasksLists[$i]
      set $i = $i + 1
  end
end

define task_list_wait
  task_list xDelayedTaskList1
  task_list xDelayedTaskList2
  task_list xPendingReadyList
  task_list xSuspendedTaskList
end
