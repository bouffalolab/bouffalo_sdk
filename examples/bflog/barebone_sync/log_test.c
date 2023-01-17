#define DBG_TAG "TEST"

#include "bflog.h"

BFLOG_DEFINE_TAG(TEST, DBG_TAG, true);
#undef BFLOG_TAG
#define BFLOG_TAG BFLOG_GET_TAG(TEST)

extern bflog_t example_recorder;

void test_log(void)
{
    BFLOG_F(&example_recorder, "hello test this is fatal error\r\n");
    BFLOG_E(&example_recorder, "hello test this is error\r\n");
    BFLOG_W(&example_recorder, "hello test this is warning\r\n");
    BFLOG_I(&example_recorder, "hello test this is information\r\n");
    BFLOG_D(&example_recorder, "hello test this is degug information\r\n");
    BFLOG_T(&example_recorder, "hello test this is trace information\r\n");
}