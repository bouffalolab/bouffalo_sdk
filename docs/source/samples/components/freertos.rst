FreeRTOS
====================

本 demo 主要演示 FreeRTOS 基本功能：任务创建和任务切换、信号量、内存管理。更详细的代码请参考 **examples/freertos**。

内存管理
---------------

FreeRTOS 内存管理默认使用 heap3。

中断管理
---------------

FreeRTOS 为 RISC-V 提供了统一的中断和异常入口，名为 ``freertos_risc_v_trap_handler``，该函数主要作用如下：

- 压栈
- 根据 mcause 查找 `mtimer` 中断，并执行 ``xTaskIncrementTick``
- 根据 mcause 查找 `ecall` 异常，并执行 ``vTaskSwitchContext``
- 根据 mcause，如果是非 `ecall` 异常，则执行 ``exception_entry``；如果是非 `mtimer` 中断，则执行 ``portasmHANDLE_INTERRUPT``, ``portasmHANDLE_INTERRUPT`` 实际调用 ``interrupt_entry``。
- 出栈

那么问题来了，是如何统一中断和异常的呢？

通常 RISC-V SOC 中断支持 vector 模式和 direct 模式，博流系列芯片都使用了 vector 模式，在 `startup/start.S` 文件中可以看到

.. code-block:: C
    :linenos:

    /* mtvec: for all exceptions and non-vector mode IRQs */
    la      a0, default_trap_handler
    ori     a0, a0, 3
    csrw    mtvec, a0

    /* mtvt: for all vector mode IRQs */
    la      a0, __Vectors
    csrw    mtvt, a0

在 `startup/interrupt` 中可以找到 ``__Vectors``:

.. code-block:: C
    :linenos:

    const pFunc __Vectors[] __attribute__((section(".init"), aligned(64))) = {
        default_interrupt_handler, /*         */
        default_interrupt_handler, /*         */
        default_interrupt_handler, /*         */
        ....
    };

此时还没有跟 ``freertos_risc_v_trap_handler`` 扯上关系，在 `freertos/CMakelist.txt` 中配置了 ``freertos_risc_v_trap_handler`` 和 ``default_interrupt_handler`` 之间的关系。那么当中断触发时，
调用 ``default_interrupt_handler`` 其实就是调用 ``freertos_risc_v_trap_handler`` 了。

.. code-block:: C
    :linenos:

    sdk_add_compile_definitions(-DportasmHANDLE_INTERRUPT=interrupt_entry -Ddefault_interrupt_handler=freertos_risc_v_trap_handler)

在 `portASM.S` 文件中 ``xPortStartFirstTask`` 函数又重新配置了 ``mtvec`` 为 ``freertos_risc_v_trap_handler``,这个时候，中断和异常就统一使用 ``freertos_risc_v_trap_handler`` 函数了。

编译和烧录
-----------------------------

-  **命令行编译**

参考 :ref:`linux_cmd` 或者 :ref:`windows_cmd`

-  **烧录**

参考 :ref:`bl_dev_cube`

实验现象
-----------------------------