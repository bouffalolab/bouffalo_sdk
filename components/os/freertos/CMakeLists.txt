sdk_generate_library()

# MTIME base addresses for different chips
if("${CHIP}" STREQUAL "bl602" OR "${CHIP}" STREQUAL "bl702" OR "${CHIP}" STREQUAL "bl702l")
    set(MTIME_BASE "0x0200BFF8UL")
    set(MTIMECMP_BASE "0x02004000UL")
elseif("${CHIP}" STREQUAL "bl618dg" AND NOT "${CPU_MODEL}" STREQUAL "a0")
    set(MTIME_BASE "0x18030000UL")
    set(MTIMECMP_BASE "0x18030008UL")
else()
    set(MTIME_BASE "0xE000BFF8UL")
    set(MTIMECMP_BASE "0xE0004000UL")
endif()

sdk_add_compile_definitions(
    -DconfigMTIME_BASE_ADDRESS=${MTIME_BASE}
    -DconfigMTIMECMP_BASE_ADDRESS=${MTIMECMP_BASE}
)

sdk_library_add_sources(croutine.c)
sdk_library_add_sources(event_groups.c)
sdk_library_add_sources(list.c)
sdk_library_add_sources(queue.c)
sdk_library_add_sources(tasks.c)
sdk_library_add_sources(timers.c)
sdk_library_add_sources(stream_buffer.c)
sdk_library_add_sources(freertos_port.c)
sdk_library_add_sources(portable/MemMang/heap_3.c)
sdk_library_add_sources_ifdef(CONFIG_PS_EXTEND freertos_helper/ps_extend.c portable/GCC/RISC-V/portStatTrap.c)

sdk_add_include_directories(include)
sdk_add_include_directories_ifdef(CONFIG_PS_EXTEND freertos_helper/include)

sdk_library_add_sources(portable/GCC/RISC-V/common/port.c)
sdk_library_add_sources(portable/GCC/RISC-V/common/portASM.S)
sdk_add_include_directories(portable/GCC/RISC-V/common)
sdk_add_include_directories(portable/GCC/RISC-V/common/chip_specific_extensions/RV32I_CLINT_no_extensions)
sdk_add_compile_definitions(-DportasmHANDLE_INTERRUPT=interrupt_entry -Ddefault_interrupt_handler=freertos_risc_v_trap_handler)

if(CONFIG_POSIX)
# posix for freertos
sdk_library_add_sources(posix/source/FreeRTOS_POSIX_clock.c)
sdk_library_add_sources(posix/source/FreeRTOS_POSIX_mqueue.c)
sdk_library_add_sources(posix/source/FreeRTOS_POSIX_pthread_barrier.c)
sdk_library_add_sources(posix/source/FreeRTOS_POSIX_pthread_cond.c)
sdk_library_add_sources(posix/source/FreeRTOS_POSIX_pthread_mutex.c)
sdk_library_add_sources(posix/source/FreeRTOS_POSIX_pthread.c)
sdk_library_add_sources(posix/source/FreeRTOS_POSIX_sched.c)
sdk_library_add_sources(posix/source/FreeRTOS_POSIX_semaphore.c)
sdk_library_add_sources(posix/source/FreeRTOS_POSIX_timer.c)
sdk_library_add_sources(posix/source/FreeRTOS_POSIX_unistd.c)
sdk_library_add_sources(posix/source/FreeRTOS_POSIX_utils.c)
sdk_add_include_directories(posix/include)
endif()
