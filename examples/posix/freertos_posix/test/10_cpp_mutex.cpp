/**
 * @file 10_cpp_mutex.cpp
 * @brief C++ std::mutex compile/link and FreeRTOS task contention test.
 */

#include <mutex>

extern "C" {
#include <FreeRTOS.h>
#include "task.h"
#include "test_common.h"
}

#ifdef POSIX_CPP_MUTEX_STANDALONE
extern "C" {
int g_tests_run;
int g_tests_passed;
int g_tests_failed;
}
#endif

namespace {

constexpr int kWorkerCount = 4;
constexpr int kIterations = 100;

std::mutex s_mutex;
int s_shared_counter;
TaskHandle_t s_test_task;

void cpp_mutex_worker(void *)
{
    for (int i = 0; i < kIterations; ++i) {
        {
            std::lock_guard<std::mutex> lock(s_mutex);
            ++s_shared_counter;
        }
        vTaskDelay(1);
    }

    xTaskNotifyGive(s_test_task);
    vTaskDelete(NULL);
}

} // namespace

extern "C" void test_cpp_mutex_run(void)
{
    s_shared_counter = 0;
    s_test_task = xTaskGetCurrentTaskHandle();

    int created = 0;
    for (int i = 0; i < kWorkerCount; ++i) {
        BaseType_t ret = xTaskCreate(cpp_mutex_worker, "cpp_mtx", 1024, NULL,
                                     tskIDLE_PRIORITY + 1, NULL);
        TEST_ASSERT_EQUAL(pdPASS, ret);
        if (ret == pdPASS) {
            ++created;
        }
    }

    int completed = 0;
    while (completed < created) {
        const uint32_t notified = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000));
        TEST_ASSERT(notified > 0);
        if (notified == 0) {
            return;
        }
        completed += notified;
    }
    TEST_ASSERT_EQUAL(created, completed);

    TEST_ASSERT_EQUAL(kWorkerCount * kIterations, s_shared_counter);
}

#ifdef POSIX_CPP_MUTEX_STANDALONE
namespace {

void cpp_mutex_test_task(void *)
{
    test_cpp_mutex_run();
    vTaskDelete(NULL);
}

} // namespace

extern "C" int main(void)
{
    board_init();
    xTaskCreate(cpp_mutex_test_task, "cpp_mtx_test", 2048, NULL,
                tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}
#endif
