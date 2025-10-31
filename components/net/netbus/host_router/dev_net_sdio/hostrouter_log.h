#ifndef __HOSTROUTER_LOG_H__
#define __HOSTROUTER_LOG_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef __SHORT_FILENAME__
#define __SHORT_FILENAME__ \
            (strchr(__FILE__, '\\') \
            ? ((strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)) \
            : ((strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)))
#endif

extern void hr_debug(const char *tag, const char *filename, const char *funcname, const long line, const char *format, ...);
#define HR_LOGD(...) //hr_debug("d", __SHORT_FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define HR_LOGI(...) hr_debug("i", __SHORT_FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define HR_LOGW(...) hr_debug("w", __SHORT_FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define HR_LOGE(...) hr_debug("e", __SHORT_FILENAME__, __func__, __LINE__, __VA_ARGS__)
#define HR_LOGA(...) hr_debug("a", __SHORT_FILENAME__, __func__, __LINE__, __VA_ARGS__)

int hr_binary_regshow(uint32_t *reg);
int hr_binary_valshow(uint32_t value);

#endif
