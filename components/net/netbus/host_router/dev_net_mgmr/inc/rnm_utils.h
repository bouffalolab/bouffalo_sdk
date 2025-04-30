#pragma once

#include <stdio.h>

#include <rnm_config.h>

#ifdef __cplusplus
extern "C" {
#endif

//// logger
#define RNM_LOG_ERR     0
#define RNM_LOG_WARNING 1
#define RNM_LOG_INFO    2
#define RNM_LOG_DEBUG   3

#if defined(RNM_LOG_ENABLE)
#define rnm_log(fmt, ...) printf("[rnm] "fmt, ##__VA_ARGS__)
#define rnm_dumpbuf(data, len) rnm_hex_dump(data, len)
#else
#define rnm_log(...) do{}while(0)
#define rnm_dumpbuf(...) do{}while(0)
#endif
#if defined(RNM_LOG_ENABLE) && RNM_LOG_LEVEL >= RNM_LOG_ERR
#define rnm_err(fmt, ...) printf("[rnm E] "fmt, ##__VA_ARGS__)
#define rnm_errbuf(data, len) rnm_hex_dump(data, len)
#else
#define rnm_err(...) do{}while(0)
#define rnm_errbuf(...) do{}while(0)
#endif
#if defined(RNM_LOG_ENABLE) && RNM_LOG_LEVEL >= RNM_LOG_WARNING
#define rnm_warning(fmt, ...) printf("[rnm W] "fmt, ##__VA_ARGS__)
#define rnm_warningbuf(data, len) rnm_hex_dump(data, len)
#else
#define rnm_warning(...) do{}while(0)
#define rnm_warningbuf(...) do{}while(0)
#endif
#if defined(RNM_LOG_ENABLE) && RNM_LOG_LEVEL >= RNM_LOG_INFO
#define rnm_info(fmt, ...) printf("[rnm I] "fmt, ##__VA_ARGS__)
#define rnm_infobuf(data, len) rnm_hex_dump(data, len)
#else
#define rnm_info(...) do{}while(0)
#define rnm_infobuf(...) do{}while(0)
#endif
#if defined(RNM_LOG_ENABLE) && RNM_LOG_LEVEL >= RNM_LOG_DEBUG
#define rnm_debug(fmt, ...) printf("[rnm D] "fmt, ##__VA_ARGS__)
#define rnm_debugbuf(data, len) rnm_hex_dump(data, len)
#else
#define rnm_debug(...) do{}while(0)
#define rnm_debugbuf(...) do{}while(0)
#endif


#define MAC_ADDR_FMT "%02X:%02X:%02X:%02X:%02X:%02X"
#define ARR_ELE_LIST_6(arr) (arr)[0], (arr)[1], (arr)[2], (arr)[3], (arr)[4], (arr)[5]

void rnm_hex_dump(const void* data, size_t size);

#ifdef __cplusplus
}
#endif
