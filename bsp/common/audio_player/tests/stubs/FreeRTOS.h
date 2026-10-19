#ifndef TEST_FREERTOS_H
#define TEST_FREERTOS_H
#include <stdint.h>
#include <stddef.h>
typedef uint32_t StackType_t;
typedef uint32_t TickType_t;
typedef struct { int unused; } StaticTask_t;
typedef void *TaskHandle_t;
typedef void *QueueHandle_t;
#define pdTRUE 1
#define pdMS_TO_TICKS(x) (x)
#endif
