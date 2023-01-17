#ifndef _EXAMPLE_LOG_H
#define _EXAMPLE_LOG_H

#include <stdint.h>
#include <string.h>
#include "bflog.h"

extern bflog_t example_recorder;

#define LOG_F(...)  BFLOG_F((void *)&example_recorder, __VA_ARGS__)
#define LOG_E(...)  BFLOG_E((void *)&example_recorder, __VA_ARGS__)
#define LOG_W(...)  BFLOG_W((void *)&example_recorder, __VA_ARGS__)
#define LOG_I(...)  BFLOG_I((void *)&example_recorder, __VA_ARGS__)
#define LOG_D(...)  BFLOG_D((void *)&example_recorder, __VA_ARGS__)
#define LOG_T(...)  BFLOG_T((void *)&example_recorder, __VA_ARGS__)
#define LOG_FLUSH() bflog_flush(&example_recorder)

extern void error_handler(void);

#ifdef CONFIG_ASSERT_DISABLE

#define _ASSERT_PARAM(x) ((void)(0))
#define _ASSERT_FUNC(x)  ((void)(x))
#define _CALL_ERROR()    error_handler()

#else

#define _CALL_ERROR()                       \
    do {                                    \
        printf("(Call Error Handler)\r\n"); \
        LOG_F("Call Error Handler\r\n");    \
        LOG_FLUSH();                        \
        error_handler();                    \
    } while (0)

#define _ASSERT_PARAM(x)                        \
    if ((uint32_t)(x) == 0) {                   \
        printf("(Assertion Faild)\r\n");        \
        printf("(%s)\r\n", (const char *)(#x)); \
        LOG_F("Assertion Faild\r\n");           \
        LOG_F("%s\r\n", (const char *)(#x));    \
        LOG_FLUSH();                            \
        error_handler();                        \
    }

#define _ASSERT_FUNC(x)                             \
    do {                                            \
        if ((uint32_t)(x) == 0) {                   \
            printf("(Assertion Faild)\r\n");        \
            printf("(%s)\r\n", (const char *)(#x)); \
            LOG_F("Assertion Faild\r\n");           \
            LOG_F("%s\r\n", (const char *)(#x));    \
            LOG_FLUSH();                            \
            error_handler();                        \
        }                                           \
    } while (0)
#endif

#endif
