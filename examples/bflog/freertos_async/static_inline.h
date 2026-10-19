#include "example_log.h"

#ifndef __STATIC_INLINE_H__
#define __STATIC_INLINE_H__

/*!< Tags can only be defined in C files that are not included  */
/*!< Tags can be exported using extern in header files  */
BFLB_LOG_EXTERN_TAG(INLINE);

/*!< Set the tag used by this file */
#undef BFLB_LOG_TAG
#define BFLB_LOG_TAG BFLB_LOG_GET_TAG(INLINE)

static inline void test_log_static_inline(void)
{
    LOG_I("static inline\r\n");
}

#endif