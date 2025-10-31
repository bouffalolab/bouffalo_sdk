#ifndef __NX_LOG_H__
#define __NX_LOG_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef __SHORT_FILENAME__
#define __SHORT_FILENAME__ \
            (strchr(__FILE__, '\\') \
            ? ((strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)) \
            : ((strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)))
#endif

extern void nxs_debug(const char *tag, const char *filename, const char *funcname, const long line, const char *format, ...);
#if 0
#define NX_LOGT(...) nxs_debug("i", __SHORT_FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define NX_LOGD      //printf//(...) nxs_debug("d", __SHORT_FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define NX_LOGI printf//(...) nxs_debug("i", __SHORT_FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define NX_LOGW(...) nxs_debug("w", __SHORT_FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define NX_LOGE(...) nxs_debug("e", __SHORT_FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define NX_LOGA(...) nxs_debug("a", __SHORT_FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define NX_LOGP      printf
#else
#define NX_LOGT
#define NX_LOGD
#define NX_LOGI
#if NXSPI_NET
#define NX_LOGW //printf
#define NX_LOGE //printf
#define NX_LOGA printf
#else
#define NX_LOGW //printf
#define NX_LOGE //printf
#define NX_LOGA //printf
#endif
#define NX_LOGP printf
#endif

#endif
