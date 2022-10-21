BFLOG
=============

简介
------------
- BFLOG 是一个移植简单，功能多样的多线程日志记录库
- 具有同步异步两种工作模式，异步模式如果缓冲区溢出会将最早的一条记录完整删除
- 总体三部分、记录器、定向输出、格式化器
- 一个记录器可多重定向输出，可同时输出到缓冲区、IO外设、文件、文件大小划分、文件时间划分
- 定向到文件输出，支持设定保留数量，支持按文件大小划分，支持按时间划分
- 每个定向输出可单独设定格式化器，输出等级，输出方式，颜色
- 格式化器支持简单格式、用户自定义格式、YAML格式(实现中)、CSV格式(规划中)
- 六级日志等级控制，FATAL、ERROR、WARNING、INFO、DEBUG、TRACE
- 支持等级、TAG标签、函数、行数、文件名、TICK数、TIME、线程信息输出
- 可对不需要的等级、功能、标签进行裁剪，缩小代码体积

.. figure:: img/bflog_log.png
    :align: center

    log 样式

配置BFLOG相关功能
------------
如果需要配置BFLOG的相关功能需要在对应的工程目录下 `proj.conf` 文件中添加对应的代码，举例如下：

.. note:: BFLOG 默认依赖 VLIBC 需要先启用 VLIBC。
    如果需要支持文件输出，还需要启用Fatfs和 VLIBC的Fatfs接口支持


以下是一个不支持BFLOG文件输出的 `proj.conf` 配置

.. code-block:: cmake
    :linenos:

    # 使能VLIBC
    set(CONFIG_VLIBC 1)

    # 使能 BFLOG
    set(CONFIG_BFLOG 1)


以下是一个支持BFLOG文件输出的 `proj.conf` 配置

.. code-block:: cmake
    :linenos:

    # 使能VLIBC
    set(CONFIG_VLIBC 1)
    # 使能VLIBC FATFS 文件接口
    set(CONFIG_VLIBC_FATFS 1)

    # 使能 BFLOG
    set(CONFIG_BFLOG 1)

    # 使能 Fatfs
    set(CONFIG_FATFS 1)

    # 使能 BSP_COMMON 文件夹中驱动支持
    set(CONFIG_BSP_COMMON 1)

    # 使能 SD卡
    set(CONFIG_BSP_SDH_SDCARD 1)

    # 使能 Fatfs 的 SD 卡接口
    set(CONFIG_BSP_FATFS_SDH_SDCARD 1)

此外在 `proj.conf` 配置中添加以下配置可以使用用户的 **bflog_user.h** 配置文件，配置文件模板为 **bflog_default.h**

.. code-block:: cmake
    :linenos:

    # 使能 BFLOG_USER 配置文件
    set(CONFIG_BFLOG_USER 1)
