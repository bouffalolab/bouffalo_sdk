#include "example_log.h"

#ifndef __STATIC_INLINE_H__
#define __STATIC_INLINE_H__

/*!< 只能在不被include的c文件中定义tag标签 */
/*!< 头文件中可以使用extern将标签导出 */
BFLOG_EXTERN_TAG(INLINE);

/*!< 设置此文件使用的tag标签 */
#undef BFLOG_TAG
#define BFLOG_TAG BFLOG_GET_TAG(INLINE)

static inline void test_log_static_inline(void)
{
    LOG_I("static inline\r\n");
}

#endif