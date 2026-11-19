# cxx_runtime

C++ 运行时测试 demo。

## 测试内容

| 测试 | 说明 |
|------|------|
| SYSINIT 构造函数 | `cxx_runtime` SYSINIT 记录在 `main()` 之前（调度器 barrier 之前）只执行一次全局构造函数数组 |
| 全局 `new`/`delete` | 全局对象在构造函数中通过 `operator new` 分配系统堆内存，并校验内容 |
| `std::condition_variable` | `wait`/`notify_one`、`notify_all` 唤醒多个等待任务、`wait_for` 超时，以及 `std::mutex`/`std::unique_lock` 配合使用 |

`main()` 成功时输出 `CXX_RUNTIME_PASS`，失败时输出 `CXX_RUNTIME_FAIL` 并返回 1。

## 配置说明

C++ 条件变量需要线程和 pthread 实现，因此默认 `defconfig` 打开：

```makefile
CONFIG_FREERTOS =y
CONFIG_POSIX    =y
```

FreeRTOS POSIX 兼容层为 libstdc++ 的 gthread 后端提供 pthread 实现；`std::condition_variable`
的构造函数只写入工具链的条件变量句柄，其余内部状态由第一次 `wait`/`notify` 时在
`FreeRTOS_POSIX_pthread_cond.c` 中延迟初始化。

libstdc++ 的异常全局变量会调用 `pthread_key_create()`。POSIX pthread 的线程局部存储使用
FreeRTOS 的 task-local storage pointers（和 `__thread`/errno 的 picolibc TLS 是两套独立机制）：

```c
/* TLS index 0 保留给 lwIP FreeRTOS port，pthread key 使用 1..7 */
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 8
#define configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS 1
```

pthread key 析构函数由 FreeRTOS 的任务删除回调执行；`__thread` 变量和 `errno` 仍由
picolibc TLS 提供（任务栈顶 TLS block，`_set_tls()` 切换 `tp`）。

如果关闭 `CONFIG_FREERTOS` 或 `CONFIG_POSIX`，demo 仍可编译为裸机镜像，但只运行构造函数和
`new`/`delete` 测试，条件变量测试会被跳过（输出中会说明需要打开的配置）。

## 构建与运行

```bash
cd examples/cxx_runtime
make CHIP=bl616 BOARD=bl616dk

# 烧录
make CHIP=bl616 BOARD=bl616dk flash COMX=/dev/ttyUSB0
```

串口输出示例：

```text
CXX_RUNTIME_CV_PASS: wait/notify_one
CXX_RUNTIME_CV_PASS: wait_for timeout
CXX_RUNTIME_CV_PASS: notify_all
CXX_RUNTIME_PASS: SYSINIT constructors once, global new/delete, std::mutex/std::condition_variable
```

裸机构建（只验证 C++ 运行时启动路径，跳过条件变量测试）：

```bash
make CHIP=bl616 BOARD=bl616dk BUILD_DIR=build-baremetal \
     DEFCONFIG=defconfig.baremetal
```

`defconfig.baremetal` 不包含 `CONFIG_FREERTOS`/`CONFIG_POSIX`。
