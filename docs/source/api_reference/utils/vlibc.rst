VLIBC
============

简介
------------
- VLIBC 是适用于MCU使用的C标准库 STDIO的重新实现，与标准的STDIO库具有相同的API以及相同的使用方法。
- VLIBC 支持文件操作，诸如fopen、fwrite、fprintf等API，与Fatfs文件系统对接，为其他依赖标准C库文件操作的组件提供支持。
- VLIBC 同时支持对外设进行操作，例如将UART当做文件进行处理，使用fprintf进行输出。
- VLIBC 体积小、移植极简、同时可以裁剪掉对Fatfs的依赖，仅仅当做一个体积小的printf库使用。


配置VLIBC相关功能
------------
如果需要配置VLIBC的相关功能需要在对应的工程目录下 `proj.conf` 文件中添加对应的代码，举例如下：

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
      - 使能VLIBC库，默认使用LIBC
    * - CONFIG_VLIBC_FATFS，需要启用FATFS相关功能
      - 使能VLIBC FATFS接口
    * - CONFIG_VSNPRINTF_FLOAT
      - 使能格式化输出 (%f, %F) 浮点数格式化支持
    * - CONFIG_VSNPRINTF_FLOAT_EX
      - 使能格式化输出 (%e, %g, %E, %G) 浮点数扩展格式化支持
    * - CONFIG_VSNPRINTF_LONG_LONG
      - 使能格式化输出 (%lld,%lli,%llo,%llx,%llX,%llu) 长整型格式化支持
    * - CONFIG_VSNPRINTF_WRITEBACK
      - 使能格式化字符 (%n) 字符统计支持

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
      - ✔️

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

IO 接口移植
------------

.. code-block:: c
    :linenos:

    #define IOCONSOLE_IO   ((uint32_t)0x00000001)
    #define IOCONSOLE_NAME "console"

    struct bflb_device_s *uart0 = NULL;

    uint32_t __vlibc_io_init(const char *name, uint8_t mode)
    {
        (void)mode;
        if (strcmp(name, IOCONSOLE_NAME) == 0) {
            return IOCONSOLE_IO;
        } else {
        }

        return ENOENT;
    }

    uint32_t __vlibc_io_deinit(struct __vlibc_io *io)
    {
        if (io->dev == IOCONSOLE_IO) {
            return IOCONSOLE_IO;
        } else {
        }

        return EOF;
    }

    size_t __vlibc_io_mem2dev(struct __vlibc_io *io, const void *ptr, size_t size)
    {
        if (io->dev == IOCONSOLE_IO) {
            for (size_t i = 0; i < size; i++) {
                bflb_uart_putchar(uart0, ((char *)ptr)[i]);
            }
            return size;
        } else {
        }

        return 0;
    }

    void main(){

      board_init();

      uart0 = bflb_device_get_by_name("uart0");

      vlibc_stdout = vlibc_fopen("<console", "w");

      printf("hello world\r\n");

    }

上述是一个简单的添加一个支持的IO设备，通过串口输出
使用时，调用 **vlibc_fopen** 函数，文件地址以 '<' 为开头，以 '<' 开头代表是IO设备,
后续的字符串会被认为是IO设备的名称，此名称会传入 **__vlibc_io_init** 中交由用户移植的IO接口进行判断

API参考
------------
- API 基本与标准C库一致
- 等待完善