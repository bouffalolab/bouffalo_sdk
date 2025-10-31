#define DBG_TAG "TEST"

#include "bflb_log.h"

BFLB_LOG_DEFINE_TAG(TEST, DBG_TAG, true);
#undef BFLB_LOG_TAG
#define BFLB_LOG_TAG BFLB_LOG_GET_TAG(TEST)

extern bflb_log_t example_recorder;

void test_log(void)
{
    BFLB_LOG_F(&example_recorder, "hello test this is fatal error\r\n");
    BFLB_LOG_E(&example_recorder, "hello test this is error\r\n");
    BFLB_LOG_W(&example_recorder, "hello test this is warning\r\n");
    BFLB_LOG_I(&example_recorder, "hello test this is information\r\n");
    BFLB_LOG_D(&example_recorder, "hello test this is degug information\r\n");
    BFLB_LOG_T(&example_recorder, "hello test this is trace information\r\n");
}