#pragma once

#include <stdio.h>

#include <sdiowifi_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SDM_ALIGN_LO(ptr, align) ((uintptr_t)(ptr) - ((uintptr_t)(ptr) & ((align) - 1)))
#define SDM_ALIGN_HI(ptr, align) (((uintptr_t)(ptr) + (align) - 1) & ~((align) - 1))

//// logger
#define SDM_LOG_ERR     0
#define SDM_LOG_WARNING 1
#define SDM_LOG_INFO    2
#define SDM_LOG_DEBUG   3

#if defined(SDM_LOG_ENABLE)
#define sdm_log(fmt, ...) printf("[sdm] "fmt, ##__VA_ARGS__)
#else
#define sdm_log(...) do{}while(0)
#endif
#if defined(SDM_LOG_ENABLE) && SDM_LOG_LEVEL >= SDM_LOG_ERR
#define sdm_err(fmt, ...) printf("[sdm E] "fmt, ##__VA_ARGS__)
#else
#define sdm_err(...) do{}while(0)
#endif
#if defined(SDM_LOG_ENABLE) && SDM_LOG_LEVEL >= SDM_LOG_WARNING
#define sdm_warning(fmt, ...) printf("[sdm W] "fmt, ##__VA_ARGS__)
#else
#define sdm_warning(...) do{}while(0)
#endif
#if defined(SDM_LOG_ENABLE) && SDM_LOG_LEVEL >= SDM_LOG_INFO
#define sdm_info(fmt, ...) printf("[sdm I] "fmt, ##__VA_ARGS__)
#else
#define sdm_info(...) do{}while(0)
#endif
#if defined(SDM_LOG_ENABLE) && SDM_LOG_LEVEL >= SDM_LOG_DEBUG
#define sdm_debug(fmt, ...) printf("[sdm D] "fmt, ##__VA_ARGS__)
#else
#define sdm_debug(...) do{}while(0)
#endif

#ifdef __cplusplus
}
#endif
