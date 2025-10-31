#ifndef _EXAMPLE_LOG_H
#define _EXAMPLE_LOG_H

#include <stdint.h>
#include <string.h>
#include "bflb_log.h"

extern bflb_log_t example_recorder;

#define LOG_F(...)  BFLB_LOG_F((void *)&example_recorder, __VA_ARGS__)
#define LOG_E(...)  BFLB_LOG_E((void *)&example_recorder, __VA_ARGS__)
#define LOG_W(...)  BFLB_LOG_W((void *)&example_recorder, __VA_ARGS__)
#define LOG_I(...)  BFLB_LOG_I((void *)&example_recorder, __VA_ARGS__)
#define LOG_D(...)  BFLB_LOG_D((void *)&example_recorder, __VA_ARGS__)
#define LOG_T(...)  BFLB_LOG_T((void *)&example_recorder, __VA_ARGS__)
#define LOG_FLUSH() bflb_log_flush(&example_recorder)

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
