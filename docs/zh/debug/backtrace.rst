Backtrace
=============

简介
------------

Backtrace 是一个用于栈回溯的调试组件，支持 FreeRTOS 多任务环境，可获取函数调用链帮助定位程序异常。

主要特性:

- 支持当前任务和其他任务的栈回溯
- 提供 Shell 命令行接口
- 可配置回溯深度

配置
------------

在 ``defconfig`` 中配置:

.. code-block:: cmake
    :linenos:

    # 使能 Backtrace 组件
    set(CONFIG_BACKTRACE 1)

    # 配置回溯深度 (可选，默认 64)
    set(CONFIG_BACKTRACE_DEPTH 32)

    # 使能 Shell 命令
    set(CONFIG_SHELL 1)

API
------------

backtrace_now
^^^^^^^^^^^^^^^^^^^^

打印当前任务的栈回溯。

.. code-block:: c
   :linenos:

    void backtrace_now(void);

**示例:**

.. code-block:: c
   :linenos:

    void my_function(void)
    {
        backtrace_now();
    }

**输出:**

.. code-block:: text

    my_task     : *0xa002dca8 0xa002dd98 0xa003ea88

backtrace_tasks_all
^^^^^^^^^^^^^^^^^^^^

打印所有任务的栈回溯。

.. code-block:: c
   :linenos:

    void backtrace_tasks_all(void);

**输出示例:**

.. code-block:: text

    ========== Backtrace ==========
    bt_task     : *0xa002c5a6
    shell_task  : 0xa003bc1c 0xa003bed0 0xa002c446 0xa006dc6e 0xa006e212 0xa006e446
    IDLE        : 0xa003c172
    WPA         : 0xa003a826 0xa00542e6 0xa00898da 0xa00736aa 0xa0086746
    tcpip_thread: 0xa003a622 0xa0053e12 0xa00484dc
    wifi fw     : 0xa003ca42 0xa000ddca
    Tmr Svc     : 0xa003d142 0xa003d0a0
    wifi driver : 0xa003ca42 0xa002c628
    ===============================

- ``*`` 表示当前正在运行的任务
- ``-`` 表示无法获取回溯信息

backtrace_task
^^^^^^^^^^^^^^^^^^^^

获取指定任务的栈回溯。

.. code-block:: c
   :linenos:

    int backtrace_task(TaskHandle_t handle, uintptr_t *bt, int max);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - 参数
      - 描述
    * - handle
      - FreeRTOS 任务句柄
    * - bt
      - 用于存储回溯地址的数组
    * - max
      - 数组最大容量

**返回值:** 实际获取的回溯深度

**示例:**

.. code-block:: c
   :linenos:

    TaskHandle_t task_handle;
    uintptr_t bt[16];

    int depth = backtrace_task(task_handle, bt, 16);
    for (int i = 0; i < depth; i++) {
        printf("[%d] 0x%lx\r\n", i, bt[i]);
    }

backtrace_task_by_name
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

通过任务名称获取栈回溯。

.. code-block:: c
   :linenos:

    int backtrace_task_by_name(const char *name, uintptr_t *bt, int max);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - 参数
      - 描述
    * - name
      - 任务名称字符串
    * - bt
      - 存储回溯地址的数组
    * - max
      - 数组最大容量

**返回值:** 实际获取的回溯深度，任务不存在返回 0

**示例:**

.. code-block:: c
   :linenos:

    uintptr_t bt[16];
    int depth = backtrace_task_by_name("shell_task", bt, 16);
    if (depth > 0) {
        printf("Found %d frames\r\n", depth);
    }

Shell 命令
------------

当启用 ``CONFIG_SHELL`` 时，可使用 ``bt_task`` 命令:

.. code-block:: text

    bt_task                      # 显示所有任务的栈回溯
    bt_task "task_name"            # 显示指定任务
    bt_task "task_name" 32         # 显示指定任务 (深度 32)
    bt_task "my task" 16           # 支持带空格的任务名

地址解析
------------

获取到的地址可使用工具链中的 ``addr2line`` 工具解析为源代码位置。

.. code-block:: bash

    # 单个地址 (请替换 <toolchain-prefix> 为实际工具链前缀)
    <toolchain-prefix>addr2line -e build/build_out/firmware.elf -f -p 0xa002dca8

    # 批量解析
    <toolchain-prefix>addr2line -e build/build_out/firmware.elf -f -p \
        0xa002dca8 0xa002dd98 0xa003ea88
