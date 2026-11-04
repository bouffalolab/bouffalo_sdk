cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)

# Increase pthread stack size for printf usage in tests
sdk_add_compile_definitions(PTHREAD_STACK_MIN=8192)

# Increase shell exec thread stack size
sdk_add_compile_definitions(SHELL_EXEC_THREAD_STACK_SIZE=4096)

# Use the same shell entry point for all and single-TARGET builds
sdk_set_main_file(main.c)

set(POSIX_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test/01_pthread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/02_pthread_attr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/03_pthread_mutex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/04_pthread_cond.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/05_pthread_barrier.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/06_semaphore.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/07_mqueue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/08_clock_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/09_sched_unistd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/10_cpp_mutex.cpp
)

# TARGET=<case> selects one test translation unit.  Keep the default build
# unchanged, while allowing quick compile-only checks such as TARGET=10_cpp_mutex.
if("$ENV{POSIX_TEST_MODE}" STREQUAL "single")
    set(POSIX_TEST_TARGET "$ENV{POSIX_TEST_TARGET}")
    set(POSIX_TEST_SOURCE_C "${CMAKE_CURRENT_SOURCE_DIR}/test/${POSIX_TEST_TARGET}.c")
    set(POSIX_TEST_SOURCE_CXX "${CMAKE_CURRENT_SOURCE_DIR}/test/${POSIX_TEST_TARGET}.cpp")

    if(EXISTS "${POSIX_TEST_SOURCE_CXX}")
        set(POSIX_TEST_SOURCES "${POSIX_TEST_SOURCE_CXX}")
    elseif(EXISTS "${POSIX_TEST_SOURCE_C}")
        set(POSIX_TEST_SOURCES "${POSIX_TEST_SOURCE_C}")
    else()
        message(FATAL_ERROR "Unknown POSIX test TARGET=${POSIX_TEST_TARGET}")
    endif()
endif()

target_sources(app PRIVATE ${POSIX_TEST_SOURCES})

project(freertos_posix_all)
