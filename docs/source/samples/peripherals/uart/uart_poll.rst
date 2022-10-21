UART - 轮询收发数据
====================

本 demo 主要介绍 UART1 外设的轮询发送和轮询接收，实现自发自收功能。

硬件连接
-----------------------------

将 UART1 TX 和 RX 引脚分别与 USB2TTL 模块 RX 和 TX 相连。本 demo 中不同芯片 UART1 引脚对应的 GPIO 口如下表所示：

.. table:: GPIO 口
    :widths: 30, 30, 40
    :width: 80%
    :align: center

    +----------+-----------+---------------------------+
    |   名称   | 芯片型号  |           GPIO            |
    +==========+===========+===========================+
    | UART1_TX | BL702     | GPIO 18                   |
    +          +-----------+---------------------------+
    |          | BL616     | GPIO 23                   |
    +----------+-----------+---------------------------+
    | UART1_RX | BL702     | GPIO 19                   |
    +          +-----------+---------------------------+
    |          | BL616     | GPIO 24                   |
    +----------+-----------+---------------------------+

软件实现
-----------------------------

具体软件代码见 ``examples/peripherals/uart/uart_poll``

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 uart1 外设，并且初始化 uart1 的时钟

.. code-block:: C
    :linenos:

    board_uart1_gpio_init();

-  配置相关引脚为 ``UART1 TX`` 、 ``UART1 RX`` 功能

.. code-block:: C
    :linenos:

    uart1 = bflb_device_get_by_name("uart1");

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    bflb_uart_init(uart1, &cfg);

- 获取 `uart1` 句柄，并初始化 UART1

.. code-block:: C
    :linenos:

    int ch;
    while (1) {
        ch = bflb_uart_getchar(uart1);
        if (ch != -1) {
            bflb_uart_putchar(uart1, ch);
        }
    }

- 调用 ``bflb_uart_getchar(uart1)`` ，从 UART1 RX FIFO 中读取 USB2TTL 模块发送给 UART1 的数据，保存在 ch 中，如果没有数据则返回 -1
- 调用 ``bflb_uart_putchar(uart1, ch)`` 将数据 ch 写入 UART1 TX FIFO 中，并发送给 USB2TTL 模块

编译和烧录
-----------------------------

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/examples/peripherals/uart/uart_poll
    $ make CHIP=blxxx BOARD=blxxx_dk

.. note:: blxxx为所使用的芯片型号，以bl616为例，编译命令为：make CHIP=bl616 BOARD=bl616_dk

-  **烧录**

   详见 :ref:`bl_dev_cube`

实验现象
-----------------------------

将 UART1 TX, RX, GND 引脚分别与 USB2TTL 模块 RX, TX, GND 相连，按下 reset 按键。
使用串口给 UART1 发送 ``0123456789`` ，USB2TTL 模块能接收到同样的数据。


