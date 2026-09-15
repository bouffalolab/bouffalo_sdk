Menuconfig 配置系统使用指南
=============================

配置优先级（从高到低）
--------------------

1. Make 命令行中的配置参数
2. 工程 ``defconfig``
3. Kconfig 默认值

配置如何生效
------------

配置由 Kconfig 汇总并生成一份规范化的 ``build/generated/autoconfig.def``，
再从同一结果生成 ``autoconfig.cmake``、``autoconfig.h`` 和
``autoconfig.mk``。CMake、C/C++ 与 Make 分别只消费对应的生成文件。

布尔配置在 CMake 中使用 ``y/n``，而不是 ``1/0``。``n`` 会生成 ``#undef``，
避免 C 代码中的 ``#ifdef`` 误判。

快速开始
--------

无需预先运行 menuconfig，直接 make 即可：

.. code-block:: bash

    make

自定义配置
----------

.. code-block:: bash

    make menuconfig        # 命令行配置界面
    make guiconfig         # 图形化配置界面
    make diffconfig        # 对比 defconfig.old 和 defconfig

``make menuconfig`` 会先生成完整的 ``build/generated/autoconfig.def``，菜单界面
基于这份完整配置工作。启动菜单前会保存一份 ``autoconfig.def.before``；退出
并保存后，系统打印两份完整配置之间新增或值发生变化的配置项，将这些配置逐行
追加到工程 ``defconfig``，然后重新生成 ``autoconfig.*``。

差分过程不做额外过滤。前后没有变化的 ``CONFIG_CHIP`` 等配置不会被追加，工程
``defconfig`` 中已有的 Makefile 语句和注释不会被重写。

组件菜单
--------

menuconfig 的菜单结构与 ``components/CMakeLists.txt`` 的组件分级一致，包括
AI、Crypto、Debug、File System、Graphics、Memory Management、C Library、
Network、Operating System、Shell、USB、Utilities、Multimedia、IPC、FOTA 等。
每个组件的启用开关控制对应 CMake 子目录是否参与编译。

无线相关配置（Wi-Fi、Bluetooth/BLE、Thread、Zigbee、共存调度等）以预编译
库形式发布，由库侧定义，**不在 menuconfig 中开放**，只能通过工程的
``defconfig`` 指定。

注意事项
--------

- ``defconfig`` 只允许 Kconfig 配置语法，不允许 Make 的 ``ifeq``、``include``
  或变量展开；条件默认值应放在 Kconfig 中。
- 只有 Kconfig 树中定义的配置项才能写入 ``defconfig``。
