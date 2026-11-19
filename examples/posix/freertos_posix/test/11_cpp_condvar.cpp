/**
 * @file 11_cpp_condvar.cpp
 * @brief C++ std::condition_variable compile/link and FreeRTOS task test.
 *
 * The test relies on the FreeRTOS POSIX pthread implementation underneath
 * libstdc++'s gthread backend.  A std::condition_variable object is only
 * initialized lazily by libstdc++ (its constructor just stores the toolchain
 * condition handle), so this also covers the lazy condition-variable
 * initialization in FreeRTOS_POSIX_pthread_cond.c.
 */

#include <chrono>
#include <condition_variable>
#include <mutex>

extern "C" {
#include <FreeRTOS.h>
#include "task.h"
#include "test_common.h"
}

#ifdef POSIX_CPP_CONDVAR_STANDALONE
extern "C" {
int g_tests_run;
int g_tests_passed;
int g_tests_failed;
}
#endif

namespace {

constexpr int kBroadcastWaiters = 3;
constexpr int kWorkerStackSize = 2048;

std::mutex s_mutex;
std::condition_variable s_cond;
bool s_flag;
int s_woken;
TaskHandle_t s_test_task;

void condvar_waiter(void *)
{
    {
        std::unique_lock<std::mutex> lock(s_mutex);
        s_cond.wait(lock, [] { return s_flag; });
        ++s_woken;
    }

    /* Release the mutex before deleting this task: vTaskDelete() never returns,
     * so a lock held here would stay owned by a deleted task and deadlock the
     * next test group. */
    xTaskNotifyGive(s_test_task);
    vTaskDelete(NULL);
}

bool start_waiter()
{
    return xTaskCreate(condvar_waiter, "cv_wait", kWorkerStackSize, NULL,
                       tskIDLE_PRIORITY + 1, NULL) == pdPASS;
}

void reset_state()
{
    std::lock_guard<std::mutex> lock(s_mutex);
    s_flag = false;
    s_woken = 0;
}

} // namespace

/**
 * T_CPP_COND_001: notify_one() before any waiter initializes the condition
 * variable through the signal path (no crash, no hang).
 */
static void test_cpp_cond_notify_without_waiter(void)
{
    s_cond.notify_one();
    s_cond.notify_all();
    TEST_ASSERT(true);
}

/**
 * T_CPP_COND_002: one waiting task is woken by notify_one().
 */
static void test_cpp_cond_wait_notify_one(void)
{
    s_test_task = xTaskGetCurrentTaskHandle();
    reset_state();

    if (!start_waiter()) {
        TEST_ASSERT(false);
        return;
    }

    vTaskDelay(pdMS_TO_TICKS(20));

    {
        std::lock_guard<std::mutex> lock(s_mutex);
        s_flag = true;
    }
    s_cond.notify_one();

    const uint32_t notified = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000));
    TEST_ASSERT_EQUAL(1, notified);
    TEST_ASSERT_EQUAL(1, s_woken);
}

/**
 * T_CPP_COND_003: notify_all() wakes every waiting task.
 */
static void test_cpp_cond_notify_all(void)
{
    s_test_task = xTaskGetCurrentTaskHandle();
    reset_state();

    int created = 0;
    while (created < kBroadcastWaiters) {
        if (!start_waiter()) {
            break;
        }
        ++created;
    }
    TEST_ASSERT_EQUAL(kBroadcastWaiters, created);

    if (created == kBroadcastWaiters) {
        vTaskDelay(pdMS_TO_TICKS(30));

        {
            std::lock_guard<std::mutex> lock(s_mutex);
            s_flag = true;
        }
        s_cond.notify_all();

        int woken = 0;
        while (woken < created) {
            const uint32_t notified = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000));

            if (notified == 0) {
                break;
            }
            woken += notified;
        }
        TEST_ASSERT_EQUAL(created, woken);
    }

    TEST_ASSERT_EQUAL(kBroadcastWaiters, s_woken);
}

/**
 * T_CPP_COND_004: a stack condition variable times out with the predicate
 * still false, and the wait lasts for about the requested duration.
 */
static void test_cpp_cond_wait_for_timeout(void)
{
    std::mutex mutex;
    std::condition_variable cond;
    bool ready = false;
    bool signaled = true;

    const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    {
        std::unique_lock<std::mutex> lock(mutex);
        signaled = cond.wait_for(lock, std::chrono::milliseconds(50), [&ready] { return ready; });
    }

    const long long elapsed = static_cast<long long>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start)
            .count());

    TEST_ASSERT(!signaled);
    TEST_ASSERT(elapsed >= 40);
}

extern "C" void test_cpp_condvar_run(void)
{
    test_cpp_cond_notify_without_waiter();
    test_cpp_cond_wait_notify_one();
    test_cpp_cond_notify_all();
    test_cpp_cond_wait_for_timeout();
}

#ifdef POSIX_CPP_CONDVAR_STANDALONE
namespace {

void cpp_condvar_test_task(void *)
{
    test_cpp_condvar_run();
    vTaskDelete(NULL);
}

} // namespace

extern "C" int main(void)
{
    board_init();
    xTaskCreate(cpp_condvar_test_task, "cpp_cv_test", 2048, NULL,
                tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}
#endif
