LUA
=============

简介
------------
- 目前移植完成的LUA版本是5.4.4
- LUA5.4中数字全部为number类型，但对应到c隐式分成浮点型和整形，需要在luaconf中配置浮点型和整形对应的类型
- LUA交互式编程(REPL)还未移植完成

不兼容项
------------

.. list-table:: 
    :header-rows: 1

    * - 不兼容项名称
      - 描述
    * - string.format %p
      - 输出始终为0x????????形式
    * - string.format %q
      - 不支持浮点变量
    * - string.format %f
      - 输出最大小数位数11位，更高位数会丢失精度
    * - string.format %a
      - 不支持p-计数法
    * - os.getenv
      - 返回为NULL
    * - os.tmpname
      - 返回为NULL
    * - 文件操作的读取
      - 不支持”n”读取单独读数字
