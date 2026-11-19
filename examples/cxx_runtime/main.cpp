#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * C++ runtime demo.
 *
 * The cxx_runtime SYSINIT record runs the global constructor array once from
 * the startup path, after the board console/heap records and before the
 * scheduler barrier or main().  This demo validates that startup contract and
 * the libstdc++ features the SDK supports:
 *
 * - global constructors and global new/delete (always built);
 * - std::mutex / std::condition_variable (built with CONFIG_FREERTOS=y and
 *   CONFIG_POSIX=y, where the FreeRTOS POSIX layer supplies the pthread
 *   implementation used by libstdc++'s gthread backend).
 *
 * After a successful run the image prints CXX_RUNTIME_PASS; every failure
 * prints a CXX_RUNTIME_FAIL line and returns 1.
 */

/* BL616/BL616CL do not link a hosted C++ allocator. */
void *operator new(size_t size) noexcept
{
    return malloc(size);
}

void operator delete(void *ptr) noexcept
{
    free(ptr);
}

static unsigned int constructor_calls;

class HeapBackedGlobal {
public:
    HeapBackedGlobal() : value(new unsigned int(0xC0DEC0DE)), constructed(value != nullptr)
    {
        ++constructor_calls;
    }

    ~HeapBackedGlobal()
    {
        delete value;
    }

    unsigned int *value;
    bool constructed;
};

static HeapBackedGlobal global_probe;

#if defined(CONFIG_FREERTOS) && defined(CONFIG_POSIX)

#include <chrono>
#include <condition_variable>
#include <mutex>

extern "C" {
#include <FreeRTOS.h>
#include <pthread.h>
#include "task.h"
}

namespace {

constexpr int kWorkerStackSize = 8192;
constexpr int kBroadcastWaiters = 3;

/* libstdc++'s condition variable primitives are quite stack hungry with
 * printf() inside a worker, so give every worker room instead of relying on
 * the default PTHREAD_STACK_MIN. */
bool create_worker(pthread_t *thread, void *(*entry)(void *), void *arg)
{
    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);

    if (ret == 0) {
        ret = pthread_attr_setstacksize(&attr, kWorkerStackSize);
    }
    if (ret == 0) {
        ret = pthread_create(thread, &attr, entry, arg);
    }
    (void)pthread_attr_destroy(&attr);
    return ret == 0;
}

bool join_worker(pthread_t thread)
{
    return pthread_join(thread, nullptr) == 0;
}

void delay_ms(unsigned int ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/*
 * Test 1: a global std::condition_variable wakes a waiting thread.
 *
 * The object is in static storage, so libstdc++'s constructor only stores the
 * toolchain condition handle and lets the first wait/notify lazily initialize
 * the rest of the FreeRTOS condition variable.
 */
struct NotifyOneState {
    std::mutex mutex;
    std::condition_variable cond;
    bool ready;
    int woken;
};

NotifyOneState s_notify_one;

void *notify_one_waiter(void *)
{
    std::unique_lock<std::mutex> lock(s_notify_one.mutex);
    s_notify_one.cond.wait(lock, [] { return s_notify_one.ready; });
    ++s_notify_one.woken;
    return nullptr;
}

bool test_wait_notify_one()
{
    s_notify_one.ready = false;
    s_notify_one.woken = 0;

    pthread_t thread;
    if (!create_worker(&thread, notify_one_waiter, nullptr)) {
        return false;
    }

    /* Let the waiter block on the condition variable first. */
    delay_ms(20);
    {
        std::lock_guard<std::mutex> lock(s_notify_one.mutex);
        s_notify_one.ready = true;
    }
    s_notify_one.cond.notify_one();

    return join_worker(thread) && s_notify_one.woken == 1;
}

/*
 * Test 2: a stack std::condition_variable times out when nobody notifies it.
 *
 * This covers the constructor/destructor of a thread-local condition variable
 * as well as the timed wait path (pthread_cond_timedwait) and the elapsed time
 * measured with std::chrono::steady_clock.
 */
bool test_wait_for_timeout()
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
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count());

    /* The predicate stayed false, so wait_for() must report a timeout and it
     * must have waited for about the requested duration. */
    return !signaled && elapsed >= 40;
}

/*
 * Test 3: notify_all() wakes every thread blocked on the condition variable.
 */
struct BroadcastState {
    std::mutex mutex;
    std::condition_variable cond;
    bool go;
    int woken;
};

BroadcastState s_broadcast;

void *broadcast_waiter(void *)
{
    std::unique_lock<std::mutex> lock(s_broadcast.mutex);
    s_broadcast.cond.wait(lock, [] { return s_broadcast.go; });
    ++s_broadcast.woken;
    return nullptr;
}

bool test_notify_all()
{
    s_broadcast.go = false;
    s_broadcast.woken = 0;

    pthread_t threads[kBroadcastWaiters];
    int created = 0;
    bool ok = true;

    while (created < kBroadcastWaiters) {
        if (!create_worker(&threads[created], broadcast_waiter, nullptr)) {
            ok = false;
            break;
        }
        ++created;
    }

    if (ok) {
        /* Give the workers time to block on the condition variable. */
        delay_ms(30);
        {
            std::lock_guard<std::mutex> lock(s_broadcast.mutex);
            s_broadcast.go = true;
        }
        s_broadcast.cond.notify_all();
    }

    for (int i = 0; i < created; ++i) {
        ok = join_worker(threads[i]) && ok;
    }

    return ok && s_broadcast.woken == kBroadcastWaiters;
}

struct TestCase {
    const char *name;
    bool (*run)();
};

const TestCase kConditionVariableTests[] = {
    { "wait/notify_one", test_wait_notify_one },
    { "wait_for timeout", test_wait_for_timeout },
    { "notify_all", test_notify_all },
};

bool run_condition_variable_tests()
{
    bool all_passed = true;

    for (const TestCase &test : kConditionVariableTests) {
        const bool passed = test.run();
        printf("CXX_RUNTIME_CV_%s: %s\r\n", passed ? "PASS" : "FAIL", test.name);
        all_passed = all_passed && passed;
    }

    return all_passed;
}

} // namespace

#endif /* CONFIG_FREERTOS && CONFIG_POSIX */

int main(void)
{
    /*
     * The cxx_runtime SYSINIT record already ran the global constructor array
     * once from the startup path, after the board console/heap records and
     * before the scheduler barrier or main().  Application code must not
     * initialize the C++ runtime manually anymore.
     */
    if (constructor_calls != 1 || !global_probe.constructed || !global_probe.value ||
        *global_probe.value != 0xC0DEC0DE) {
        printf("CXX_RUNTIME_FAIL: SYSINIT constructor init or global new failed\r\n");
        return 1;
    }

#if defined(CONFIG_FREERTOS) && defined(CONFIG_POSIX)
    if (!run_condition_variable_tests()) {
        printf("CXX_RUNTIME_FAIL: C++ condition variable tests failed\r\n");
        return 1;
    }
#endif

    delete global_probe.value;
    global_probe.value = nullptr;

#if defined(CONFIG_FREERTOS) && defined(CONFIG_POSIX)
    printf("CXX_RUNTIME_PASS: SYSINIT constructors once, global new/delete, "
           "std::mutex/std::condition_variable\r\n");
#else
    printf("CXX_RUNTIME_PASS: SYSINIT ran constructors once before main, global new/delete "
           "(C++ condition variable tests require CONFIG_FREERTOS=y and CONFIG_POSIX=y)\r\n");
#endif
    return 0;
}
