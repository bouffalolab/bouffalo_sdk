#ifndef _TEST_COMMON_H
#define _TEST_COMMON_H

#include <stdio.h>
#include <string.h>
#include "board.h"

/* External test counters (defined in main.c) */
extern int g_tests_run;
extern int g_tests_passed;
extern int g_tests_failed;
extern int g_tests_skipped;

/* Simplified assertion macros */
#define TEST_ASSERT(cond) do { \
    g_tests_run++; \
    if (cond) { \
        g_tests_passed++; \
        printf("[PASS] %s:%d %s\r\n", __func__, __LINE__, #cond); \
    } else { \
        g_tests_failed++; \
        printf("[FAIL] %s:%d %s\r\n", __func__, __LINE__, #cond); \
    } \
} while(0)

#define TEST_ASSERT_EQUAL(expected, actual) do { \
    g_tests_run++; \
    if ((expected) == (actual)) { \
        g_tests_passed++; \
        printf("[PASS] %s:%d expected=%ld actual=%ld\r\n", \
               __func__, __LINE__, (long)(expected), (long)(actual)); \
    } else { \
        g_tests_failed++; \
        printf("[FAIL] %s:%d expected=%ld actual=%ld\r\n", \
               __func__, __LINE__, (long)(expected), (long)(actual)); \
    } \
} while(0)

#define TEST_ASSERT_EQUAL_INT(expected, actual) TEST_ASSERT_EQUAL(expected, actual)

#define TEST_ASSERT_NOT_EQUAL(expected, actual) do { \
    g_tests_run++; \
    if ((expected) != (actual)) { \
        g_tests_passed++; \
        printf("[PASS] %s:%d %ld != %ld\r\n", \
               __func__, __LINE__, (long)(expected), (long)(actual)); \
    } else { \
        g_tests_failed++; \
        printf("[FAIL] %s:%d %ld == %ld (should not equal)\r\n", \
               __func__, __LINE__, (long)(expected), (long)(actual)); \
    } \
} while(0)

#define TEST_ASSERT_EQUAL_STRING(expected, actual) do { \
    g_tests_run++; \
    if (strcmp((expected), (actual)) == 0) { \
        g_tests_passed++; \
        printf("[PASS] %s:%d string match\r\n", __func__, __LINE__); \
    } else { \
        g_tests_failed++; \
        printf("[FAIL] %s:%d expected=\"%s\" actual=\"%s\"\r\n", \
               __func__, __LINE__, (expected), (actual)); \
    } \
} while(0)

#define TEST_ASSERT_WITHIN_MESSAGE(low, high, actual, message) do { \
    g_tests_run++; \
    if ((actual) >= (low) && (actual) <= (high)) { \
        g_tests_passed++; \
        printf("[PASS] %s:%d %ld in range [%ld, %ld]\r\n", \
               __func__, __LINE__, (long)(actual), (long)(low), (long)(high)); \
    } else { \
        g_tests_failed++; \
        printf("[FAIL] %s:%d %ld not in range [%ld, %ld]: %s\r\n", \
               __func__, __LINE__, (long)(actual), (long)(low), (long)(high), message); \
    } \
} while(0)

#define TEST_SKIP(message) do { \
    g_tests_run++; \
    g_tests_skipped++; \
    printf("[SKIP] %s: %s\r\n", __func__, message); \
} while(0)

/* Test summary print */
static inline void print_test_summary(void)
{
    printf("\r\n========== TEST SUMMARY ==========\r\n");
    printf("Total:   %d\r\n", g_tests_run);
    printf("Passed:  %d\r\n", g_tests_passed);
    printf("Failed:  %d\r\n", g_tests_failed);
    printf("Skipped: %d\r\n", g_tests_skipped);
    printf("==================================\r\n");
}

#endif /* _TEST_COMMON_H */
