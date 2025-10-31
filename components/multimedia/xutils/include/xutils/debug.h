/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __XUTILS_DEBUG_H__
#define __XUTILS_DEBUG_H__

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MSP_DEBUG_COLOR (1)

typedef void (*except_process_t)(int err, const char *file, int line, const char *func_name, void *caller);

/**
 *brief       set user except callback
 *param[in]   except     callback function
*/
void msp_set_except_callback(except_process_t except);

/**
 *brief       set default except callback
*/
void msp_set_except_default();

extern void msp_print_version(const char *format, ...);
extern void msp_debug(const char *tag, const char *filename, const long line, const char *funcname, const char *format, ...);

#ifndef __FILENAME__
#define __FILENAME__ \
            (strchr(__FILE__, '\\') \
            ? ((strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)) \
            : ((strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)))
#endif

#define MSP_LOG_TAG __FILENAME__, __LINE__

#ifndef CHECK_PARAM
#define CHECK_PARAM(x, ret) \
	do { \
		if (!(x)) { \
			return ret; \
		}\
	} while (0)
#endif

#ifndef CHECK_RET_WITH_GOTO
#define CHECK_RET_WITH_GOTO(x, label) \
	do { \
		if (!(x)) { \
			printf("%s, %d fail.\r\n", __FUNCTION__, __LINE__); \
			goto label; \
		}\
	} while (0)
#endif

#ifndef CHECK_RET_WITH_RET
#define CHECK_RET_WITH_RET(x, ret) \
	do { \
		if (!(x)) { \
			printf("%s, %d fail.\r\n", __FUNCTION__, __LINE__); \
			return ret; \
		}\
	} while (0)
#endif

#ifndef CHECK_RET_TAG_WITH_GOTO
#define CHECK_RET_TAG_WITH_GOTO(x, label) \
	do { \
		if (!(x)) { \
			printf("%s, %d fail.\r\n", __FUNCTION__, __LINE__); \
			goto label; \
		}\
	} while (0)
#endif

#ifndef CHECK_RET_TAG_WITH_RET
#define CHECK_RET_TAG_WITH_RET(x, ret) \
	do { \
		if (!(x)) { \
			printf("%s, %d fail.\r\n", __FUNCTION__, __LINE__); \
			return ret; \
		}\
	} while (0)
#endif

#define SHORT_FILE __FUNCTION__

#define debug_print_assert(A, B, C, D, E, F)                                                       \
    do {                                                                                           \
        printf("!!!assert: %s: %d, %s\r\n", D, E, F);                                              \
        while (1);                                                                                 \
    } while (0)

#define msp_assert(X)                                                                              \
    do {                                                                                           \
        if (unlikely(!(X))) {                                                                      \
            debug_print_assert(0, #X, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__);            \
        }                                                                                          \
    } while (0)
#define except_process(err) msp_except_process(err, SHORT_FILE, __LINE__,                          \
                                __PRETTY_FUNCTION__,  __builtin_return_address(0))

#if (!defined(unlikely))
#define unlikely(EXPRESSSION) !!(EXPRESSSION)
#endif

#if (!defined(msp_check))
#define msp_check(X, err)                                                                          \
    do {                                                                                           \
        if (unlikely(!(X))) {                                                                      \
            except_process(err);                                                                   \
        }                                                                                          \
    } while (0)
#endif

#if (!defined(msp_check_return_val))
#define msp_check_return_val(X, ret)                                                               \
    do {                                                                                           \
        if (unlikely(!(X))) {                                                                      \
            except_process(ret);                                                                   \
            return ret;                                                                            \
        }                                                                                          \
    } while (0)
#endif

#if (!defined(msp_check_param))
#define msp_check_param(X) msp_check(X, EINVAL)
#endif

#if (!defined(msp_check_return_einval))
#define msp_check_return_einval(X) msp_check_return_val(X, EINVAL)
#endif

#if (!defined(msp_check_return_enomem))
#define msp_check_return_enomem(X) msp_check_return_val(X, ENOMEM)
#endif

#define MSP_LOGD(...) msp_debug("mspd", MSP_LOG_TAG, __func__, __VA_ARGS__)
#define MSP_LOGI(...) msp_debug("mspi", MSP_LOG_TAG, __func__, __VA_ARGS__)
#define MSP_LOGW(...) msp_debug("mspw", MSP_LOG_TAG, __func__, __VA_ARGS__)
#define MSP_LOGE(...) msp_debug("mspe", MSP_LOG_TAG, __func__, __VA_ARGS__)
#define MSP_LOGA(...) msp_debug("mspa", MSP_LOG_TAG, __func__, __VA_ARGS__)

#ifndef LOGE
#define LOGE(mod, ...) MSP_LOGE(__VA_ARGS__)
#endif

#ifndef LOGW
#define LOGW(mod, ...) MSP_LOGW(__VA_ARGS__)
#endif

#ifndef LOGI
#define LOGI(mod, ...) MSP_LOGI(__VA_ARGS__)
#endif

#ifndef LOGD
#define LOGD(mod, ...) MSP_LOGD(__VA_ARGS__)
#endif

#ifndef MSP_UNUSED_ARG
#define MSP_UNUSED_ARG(x) (void)__x86_64
#endif /* MSP_UNUSED_ARG */

void msp_except_process(int err, const char *file, int line, const char *func_name, void *caller);

#endif
