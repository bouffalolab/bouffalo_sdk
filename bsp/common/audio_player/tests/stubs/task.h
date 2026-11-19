/* Host fixture supplies the task primitives before production source inclusion. */
#ifndef TEST_TASK_H
#define TEST_TASK_H
#include "FreeRTOS.h"
#ifndef taskENTER_CRITICAL
#define taskENTER_CRITICAL() ((void)0)
#define taskEXIT_CRITICAL() ((void)0)
#endif
#endif
