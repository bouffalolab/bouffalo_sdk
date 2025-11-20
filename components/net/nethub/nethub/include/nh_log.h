/**
 * @file nh_log.h
 * @brief nethub log system
 */

#ifndef NH_LOG_H
#define NH_LOG_H

#include <stdio.h>

#ifdef CONFIG_NETHUB_ON_LINUX
/* Default log level */
#ifndef CONFIG_NETHUB_LOG_LEVEL
#define CONFIG_NETHUB_LOG_LEVEL  3  /* Default INFO level */
#endif

/* Simplified log levels */
#define NH_LOGLEVEL_OFF     0   /* Log off */
#define NH_LOGLEVEL_ERR     1   /* Error level */
#define NH_LOGLEVEL_WARN    2   /* Warning level */
#define NH_LOGLEVEL_INFO    3   /* Info level */
#define NH_LOGLEVEL_DEBUG   4   /* Debug level */

/* Basic log macros - unified using 5 levels */
#if CONFIG_NETHUB_LOG_LEVEL >= NH_LOGLEVEL_ERR
#define nh_err(fmt, ...)    printf("[NH_ERR] " fmt, ##__VA_ARGS__)
#else
#define nh_err(fmt, ...)    do { } while(0)
#endif

#if CONFIG_NETHUB_LOG_LEVEL >= NH_LOGLEVEL_WARN
#define nh_warn(fmt, ...)   printf("[NH_WARN] " fmt, ##__VA_ARGS__)
#else
#define nh_warn(fmt, ...)   do { } while(0)
#endif

#if CONFIG_NETHUB_LOG_LEVEL >= NH_LOGLEVEL_INFO
#define nh_info(fmt, ...)   printf("[NH_INFO] " fmt, ##__VA_ARGS__)
#else
#define nh_info(fmt, ...)   do { } while(0)
#endif

#if CONFIG_NETHUB_LOG_LEVEL >= NH_LOGLEVEL_DEBUG
#define nh_debug(fmt, ...)  printf("[NH_DEBUG] " fmt, ##__VA_ARGS__)
#else
#define nh_debug(fmt, ...)  do { } while(0)
#endif
#else
#include <log.h>
#ifdef CONFIG_NETHUB_DEBUG
#define nh_err     LOG_E
#define nh_warn    LOG_W
#define nh_info    LOG_I
#define nh_debug   LOG_D
#else
#define nh_err(...)
#define nh_warn(...)
#define nh_info(...)
#define nh_debug(...)
#endif
#endif
#endif /* NH_LOG_H */
