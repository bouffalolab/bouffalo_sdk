CMake 框架的使用
==================

本节主要介绍如何使用 CMake 框架。BouffaloSDK 中为用户封装了以下函数接口，基本满足常用场景的使用。

.. list-table::
    :widths: 30 30
    :header-rows: 1

    * - Function
      - Description
    * - sdk_generate_library
      - 生成库，库名如果函数没有形参则使用当前库所在目录名
    * - sdk_library_add_sources
      - 为库添加源文件
    * - sdk_library_add_sources_ifdef
      - 为库添加源文件（满足 if 条件）
    * - sdk_add_include_directories
      - 添加头文件路径
    * - sdk_add_include_directories_ifdef
      - 添加头文件路径（满足 if 条件）
    * - sdk_add_compile_definitions
      - 添加宏定义,不要带 -D
    * - sdk_add_compile_definitions_ifdef
      - 添加宏定义（满足 if 条件）
    * - sdk_add_compile_options
      - 添加编译选项
    * - sdk_add_compile_options_ifdef
      - 添加编译选项（满足 if 条件）
    * - sdk_add_link_options
      - 添加链接选项
    * - sdk_add_link_options_ifdef
      - 添加链接选项（满足 if 条件）
    * - sdk_add_link_libraries
      - 添加静态库
    * - sdk_add_link_libraries_ifdef
      - 添加静态库（满足 if 条件）
    * - sdk_add_subdirectory_ifdef
      - 编译子目录下的 cmakelist（满足 if 条件）
    * - sdk_add_static_library
      - 添加外部静态库
    * - sdk_set_linker_script
      - 设置链接脚本
    * - sdk_set_main_file
      - 设置 main 函数所在文件
    * - project
      - 工程编译
    * - target_source(app PRIVATE xxx)
      - 添加源文件到 app 库中，当用户需要自己添加一些源文件又不想创建 cmakelist 单独编译成库，可以使用该项

新建工程
----------------

用户可以复制一份工程，如 `helloworld`，并修改 `SDK_DEMO_PATH` 变量为你使用的路径即可创建一份全新的工程

为工程添加组件
--------------------

如果需要编译相关组件，如 FATFS、LVGL ，需要在 `proj.conf` 文件中添加组件的使能，举例如下：

.. code-block:: c
   :linenos:

    set(CONFIG_FATFS 1)
    set(CONFIG_LVGL 1)

使能条件编译项
--------------------

用户自定义的 cmake 条件编译项（ **使用了 cmake 的 if 语法**）、或者使用了 sdk 带 `ifdef` 结尾的函数, 使能方式同上