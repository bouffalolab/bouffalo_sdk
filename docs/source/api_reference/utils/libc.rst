LIBC
============

简介
------------
- LIBC 中实现了一个vsnprintf，用以避开使用完整的c库，减少代码体积。

.. note:: LIBC 中浮点打印只能最大支持到7位小数精度

配置LIBC相关功能
------------
如果需要配置LIBC的相关功能需要在对应的工程目录下 `proj.conf` 文件中添加对应的代码，举例如下：

.. code-block:: cmake
    :linenos:

    # 使能浮点打印
    set(CONFIG_VSNPRINTF_FLOAT 1)

格式化控制字符
------------

.. list-table:: 
    :header-rows: 1

    * - proj.conf 配置项
      - 功能描述
    * - CONFIG_VLIBC
      - 使能 **VLIBC** 库，默认使用 **LIBC** ，不配置 **CONFIG_VLIBC** 或配置为 **0** 使能 **LIBC**
    * - CONFIG_VSNPRINTF_FLOAT
      - 使能格式化输出 (%f, %F) 浮点数格式化支持
    * - CONFIG_VSNPRINTF_FLOAT_EX
      - 使能格式化输出 (%e, %g, %E, %G) 浮点数扩展格式化支持
    * - CONFIG_VSNPRINTF_LONG_LONG
      - 使能格式化输出 (%lld,%lli,%llo,%llx,%llX,%llu) 长整型格式化支持

.. list-table:: 
    :header-rows: 1

    * - 格式化控制字符 (specifier)
      - 支持情况
    * - %s
      - ✔️
    * - %c
      - ✔️
    * - %d
      - ✔️
    * - %i
      - ✔️
    * - %u
      - ✔️
    * - %x (%X)
      - ✔️
    * - %o
      - ✔️
    * - %b
      - ✔️
    * - %f (%F)
      - ✔️
    * - %e (%E)
      - ✔️
    * - %g (%G)
      - ✔️
    * - %a (%A)
      - ❌
    * - %p
      - ✔️
    * - %n
      - ❌

.. list-table:: 
    :header-rows: 1

    * - 格式化控制变量长度字符 (length)
      - 支持情况
    * - l
      - ✔️
    * - ll
      - ✔️
    * - h
      - ✔️
    * - hh
      - ✔️
    * - t
      - ✔️
    * - j
      - ✔️
    * - z
      - ✔️

.. list-table:: 
    :header-rows: 1

    * - 格式化控制标志字符 (flags)
      - 支持情况
    * - 0
      - ✔️
    * - \-
      - ✔️
    * - \+
      - ✔️
    * - ' ' (空格)
      - ✔️
    * - #
      - ✔️

格式化输出格式
------------
.. code-block:: c
    :linenos:

    /* 格式化输出控制字符格式 */
    /* %[flags][width][.precision][length][specifier] */
    printf("%+12.9f\r\n", var)
    printf("%-12lld\r\n", var)
    printf("%#x\r\n", var)
    printf("%08x\r\n", var)
    printf("% 8x\r\n", var)