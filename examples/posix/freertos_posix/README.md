# FreeRTOS POSIX Compatibility Layer

本示例展示 Bouffalo SDK 中 FreeRTOS POSIX 兼容层的使用方法。

## 概述

Bouffalo SDK 提供了 FreeRTOS 的 POSIX 兼容层，允许开发者使用标准的 POSIX API（如 pthread、semaphore、mqueue 等）进行多线程编程。该兼容层基于 Amazon FreeRTOS POSIX V1.1.0 实现。

## 启用方法

在项目的 `defconfig` 文件中添加：

```makefile
CONFIG_POSIX=y
```

或者通过 `make menuconfig` 图形化配置启用该选项。

## 头文件

```c
#include <pthread.h>      /* 线程、互斥量、条件变量 */
#include <semaphore.h>    /* 信号量 */
#include <mqueue.h>       /* 消息队列 */
#include <sched.h>        /* 调度器 */
#include <unistd.h>       /* sleep, usleep */
#include <time.h>         /* 时钟、定时器 */
```

---

## API 参考

### 一、线程管理 (pthread)

#### 线程创建与控制

| 函数 | 说明 |
|------|------|
| `int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)` | 创建线程 |
| `void pthread_exit(void *value_ptr)` | 线程退出 |
| `int pthread_join(pthread_t thread, void **retval)` | 等待线程结束 |
| `int pthread_detach(pthread_t thread)` | 分离线程 |
| `int pthread_cancel(pthread_t thread)` | 取消线程（返回 ENOSYS，未实现） |
| `pthread_t pthread_self(void)` | 获取当前线程 ID |
| `int pthread_equal(pthread_t t1, pthread_t t2)` | 比较线程 ID |

#### 线程属性

| 函数 | 说明 |
|------|------|
| `int pthread_attr_init(pthread_attr_t *attr)` | 初始化线程属性 |
| `int pthread_attr_destroy(pthread_attr_t *attr)` | 销毁线程属性 |
| `int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)` | 获取分离状态 |
| `int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)` | 设置分离状态 |
| `int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)` | 获取栈大小 |
| `int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)` | 设置栈大小 |
| `int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param)` | 获取调度参数 |
| `int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)` | 设置调度参数 |
| `int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)` | 设置调度策略 |

#### 线程调度

| 函数 | 说明 |
|------|------|
| `int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param)` | 获取线程调度参数 |
| `int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param)` | 设置线程调度参数 |

---

### 二、互斥量 (pthread_mutex)

| 函数 | 说明 |
|------|------|
| `int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)` | 初始化互斥量 |
| `int pthread_mutex_destroy(pthread_mutex_t *mutex)` | 销毁互斥量 |
| `int pthread_mutex_lock(pthread_mutex_t *mutex)` | 加锁（阻塞） |
| `int pthread_mutex_trylock(pthread_mutex_t *mutex)` | 尝试加锁（非阻塞） |
| `int pthread_mutex_unlock(pthread_mutex_t *mutex)` | 解锁 |
| `int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime)` | 超时加锁 |

#### 互斥量属性

| 函数 | 说明 |
|------|------|
| `int pthread_mutexattr_init(pthread_mutexattr_t *attr)` | 初始化互斥量属性 |
| `int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)` | 销毁互斥量属性 |
| `int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type)` | 获取互斥量类型 |
| `int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)` | 设置互斥量类型 |

**互斥量类型：**
- `PTHREAD_MUTEX_NORMAL` - 普通互斥量
- `PTHREAD_MUTEX_RECURSIVE` - 递归互斥量
- `PTHREAD_MUTEX_ERRORCHECK` - 错误检查互斥量

---

### 三、条件变量 (pthread_cond)

| 函数 | 说明 |
|------|------|
| `int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)` | 初始化条件变量 |
| `int pthread_cond_destroy(pthread_cond_t *cond)` | 销毁条件变量 |
| `int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)` | 等待条件变量 |
| `int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)` | 超时等待条件变量 |
| `int pthread_cond_signal(pthread_cond_t *cond)` | 发送信号（唤醒一个等待线程） |
| `int pthread_cond_broadcast(pthread_cond_t *cond)` | 广播信号（唤醒所有等待线程） |

---

### 四、屏障 (pthread_barrier)

| 函数 | 说明 |
|------|------|
| `int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count)` | 初始化屏障 |
| `int pthread_barrier_destroy(pthread_barrier_t *barrier)` | 销毁屏障 |
| `int pthread_barrier_wait(pthread_barrier_t *barrier)` | 等待屏障 |

---

### 五、信号量 (semaphore)

| 函数 | 说明 |
|------|------|
| `int sem_init(sem_t *sem, int pshared, unsigned value)` | 初始化信号量 |
| `int sem_destroy(sem_t *sem)` | 销毁信号量 |
| `int sem_wait(sem_t *sem)` | 等待信号量（P操作，阻塞） |
| `int sem_trywait(sem_t *sem)` | 尝试等待信号量（非阻塞） |
| `int sem_timedwait(sem_t *sem, const struct timespec *abstime)` | 超时等待信号量 |
| `int sem_post(sem_t *sem)` | 释放信号量（V操作） |
| `int sem_getvalue(sem_t *sem, int *sval)` | 获取信号量当前值 |

---

### 六、消息队列 (mqueue)

| 函数 | 说明 |
|------|------|
| `mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr)` | 打开/创建消息队列 |
| `int mq_close(mqd_t mqdes)` | 关闭消息队列 |
| `int mq_unlink(const char *name)` | 删除消息队列 |
| `int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio)` | 发送消息 |
| `ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio)` | 接收消息 |
| `int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abstime)` | 超时发送消息 |
| `ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio, const struct timespec *abstime)` | 超时接收消息 |
| `int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat)` | 获取消息队列属性 |

**打开标志：**
- `O_RDONLY` - 只读
- `O_WRONLY` - 只写
- `O_RDWR` - 读写
- `O_CREAT` - 创建
- `O_EXCL` - 独占创建
- `O_NONBLOCK` - 非阻塞

---

### 七、时钟与定时器 (clock/timer)

#### 时钟函数

| 函数 | 说明 |
|------|------|
| `clock_t clock(void)` | 获取进程 CPU 时间 |
| `int clock_gettime(clockid_t clock_id, struct timespec *tp)` | 获取指定时钟的时间 |
| `int clock_settime(clockid_t clock_id, const struct timespec *tp)` | 设置指定时钟的时间 |
| `int clock_getres(clockid_t clock_id, struct timespec *res)` | 获取时钟分辨率 |
| `int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp, struct timespec *rmtp)` | 纳秒级睡眠 |
| `int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)` | 纳秒级睡眠 |
| `int clock_getcpuclockid(pid_t pid, clockid_t *clock_id)` | 获取进程 CPU 时钟 ID |

**时钟 ID：**
- `CLOCK_REALTIME` - 系统实时时钟
- `CLOCK_MONOTONIC` - 单调递增时钟

#### 定时器函数

| 函数 | 说明 |
|------|------|
| `int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid)` | 创建定时器 |
| `int timer_delete(timer_t timerid)` | 删除定时器 |
| `int timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue)` | 设置定时器 |
| `int timer_gettime(timer_t timerid, struct itimerspec *value)` | 获取定时器剩余时间 |
| `int timer_getoverrun(timer_t timerid)` | 获取定时器溢出次数 |

---

### 八、调度器 (sched)

| 函数 | 说明 |
|------|------|
| `int sched_yield(void)` | 让出 CPU |
| `int sched_get_priority_max(int policy)` | 获取调度策略的最大优先级 |
| `int sched_get_priority_min(int policy)` | 获取调度策略的最小优先级 |

**调度策略：**
- `SCHED_OTHER` - 默认调度策略
- `SCHED_FIFO` - 先进先出实时调度
- `SCHED_RR` - 轮转实时调度

---

### 九、UNIX 标准 (unistd)

| 函数 | 说明 |
|------|------|
| `unsigned sleep(unsigned seconds)` | 秒级睡眠 |
| `int usleep(useconds_t usec)` | 微秒级睡眠 |

---

### 十、内部工具函数 (utils)

| 函数 | 说明 |
|------|------|
| `int UTILS_TimespecToTicks(const struct timespec *pxTimespec, TickType_t *pxResult)` | timespec 转换为 FreeRTOS ticks |
| `void UTILS_NanosecondsToTimespec(int64_t llSource, struct timespec *pxDestination)` | 纳秒转换为 timespec |
| `int UTILS_TimespecAdd(const struct timespec *x, const struct timespec *y, struct timespec *pxResult)` | timespec 加法 |
| `int UTILS_TimespecSubtract(const struct timespec *x, const struct timespec *y, struct timespec *pxResult)` | timespec 减法 |
| `int UTILS_TimespecAddNanoseconds(const struct timespec *x, int64_t llNanoseconds, struct timespec *pxResult)` | timespec 加纳秒 |
| `int UTILS_TimespecCompare(const struct timespec *x, const struct timespec *y)` | timespec 比较 |
| `int UTILS_AbsoluteTimespecToDeltaTicks(const struct timespec *pxAbsoluteTime, const struct timespec *pxCurrentTime, TickType_t *pxResult)` | 绝对时间转换为增量 ticks |
| `bool UTILS_ValidateTimespec(const struct timespec *pxTimespec)` | 验证 timespec 是否有效 |
| `size_t UTILS_strnlen(const char *pcString, size_t xMaxLength)` | 安全字符串长度 |

---

## 限制说明

| 功能 | 限制 |
|------|------|
| `pthread_cancel` | 未实现，返回 `ENOSYS` |
| 定时器信号通知 | 不支持 `SIGEV_SIGNAL`，仅支持 `SIGEV_THREAD` |
| 消息队列优先级 | 不支持，`msg_prio` 参数被忽略 |
| 时钟 ID | 大部分函数忽略 `clock_id` 参数 |
| 进程共享 | `pshared` 参数被忽略，所有同步对象仅支持线程共享 |
| 信号 | 不支持 POSIX 信号 |

---

## 示例代码

### 线程创建

```c
#include <pthread.h>
#include <stdio.h>

void *thread_func(void *arg) {
    int id = *(int *)arg;
    printf("Thread %d running\n", id);
    return NULL;
}

int main(void) {
    pthread_t thread;
    int id = 1;

    pthread_create(&thread, NULL, thread_func, &id);
    pthread_join(thread, NULL);

    return 0;
}
```

### 互斥量

```c
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void *thread_func(void *arg) {
    pthread_mutex_lock(&mutex);
    counter++;
    printf("Counter: %d\n", counter);
    pthread_mutex_unlock(&mutex);
    return NULL;
}
```

### 条件变量

```c
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 0;

void *producer(void *arg) {
    pthread_mutex_lock(&mutex);
    ready = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *consumer(void *arg) {
    pthread_mutex_lock(&mutex);
    while (!ready) {
        pthread_cond_wait(&cond, &mutex);
    }
    /* 处理数据 */
    pthread_mutex_unlock(&mutex);
    return NULL;
}
```

### 信号量

```c
#include <semaphore.h>

sem_t sem;

void init(void) {
    sem_init(&sem, 0, 0);  /* 初始值为 0 */
}

void wait_for_signal(void) {
    sem_wait(&sem);  /* 阻塞等待 */
}

void send_signal(void) {
    sem_post(&sem);  /* 发送信号 */
}
```

### 消息队列

```c
#include <mqueue.h>
#include <string.h>
#include <fcntl.h>

#define QUEUE_NAME "/test_queue"
#define MAX_MSG_SIZE 256
#define MAX_MSGS 10

void sender(void) {
    mqd_t mq;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSGS;
    attr.mq_msgsize = MAX_MSG_SIZE;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    mq_send(mq, "Hello", 6, 0);
    mq_close(mq);
}

void receiver(void) {
    mqd_t mq;
    char buffer[MAX_MSG_SIZE];

    mq = mq_open(QUEUE_NAME, O_RDONLY);
    mq_receive(mq, buffer, MAX_MSG_SIZE, NULL);
    printf("Received: %s\n", buffer);
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
}
```

---

## 构建与运行

```bash
# 构建
cd examples/freertos/freertos_posix
make CHIP=bl616 BOARD=bl616dk

# 烧录
make flash CHIP=bl616 COMX=/dev/ttyUSB0

# 使用 ninja 构建（更快）
make ninja CHIP=bl616 BOARD=bl616dk
```

---

## 参考资料

- [FreeRTOS POSIX Compatibility Layer](https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_POSIX/)
- [POSIX Threads Programming](https://computing.llnl.gov/tutorials/pthreads/)
- Bouffalo SDK 文档: `components/os/CLAUDE.md`
