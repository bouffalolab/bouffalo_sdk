cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)

# Increase pthread stack size for printf usage in tests
sdk_add_compile_definitions(PTHREAD_STACK_MIN=8192)

# Increase shell exec thread stack size
sdk_add_compile_definitions(SHELL_EXEC_THREAD_STACK_SIZE=4096)

# Select compilation mode based on environment variable
if(DEFINED ENV{POSIX_TEST_MODE})
    set(TEST_MODE $ENV{POSIX_TEST_MODE})
else()
    set(TEST_MODE "all")
endif()

if(TEST_MODE STREQUAL "all")
    # Compile all tests, use main.c as entry point
    sdk_set_main_file(main.c)

    # Add all test source files to app library
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test/01_pthread.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/02_pthread_attr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/03_pthread_mutex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/04_pthread_cond.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/05_pthread_barrier.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/06_semaphore.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/07_mqueue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/08_clock_timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/09_sched_unistd.c
    )

    project(freertos_posix_all)
else()
    # Compile single test
    if(DEFINED ENV{POSIX_TEST_TARGET})
        set(TEST_TARGET $ENV{POSIX_TEST_TARGET})
    else()
        set(TEST_TARGET "01_pthread")
    endif()

    sdk_set_main_file(test/${TEST_TARGET}.c)
    project(freertos_posix_${TEST_TARGET})
endif()
