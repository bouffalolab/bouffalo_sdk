UART - poll
====================

本 demo 主要演示 UART 轮询模式收发功能。

硬件连接
-----------------------------

- 芯片 UART TX 引脚连接 USB2TTL 模块 RX
- 芯片 UART RX 引脚连接 USB2TTL 模块 TX

本 demo 使用到的 gpio 如下表：

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

具体软件代码见 **examples/peripherals/uart/uart_poll**

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 UART IP 时钟，并选择 UART 时钟源和分频。

.. code-block:: C
    :linenos:

    board_uartx_gpio_init();

-  配置相关引脚为 ``UARTx TX`` 、 ``UARTx RX`` 功能，默认 demo 使用 UART1 外设。

.. code-block:: C
    :linenos:

    uartx = bflb_device_get_by_name(DEFAULT_TEST_UART);

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    bflb_uart_init(uartx, &cfg);

- 获取 `DEFAULT_TEST_UART` 句柄，并初始化 UART

.. code-block:: C
    :linenos:

    int ch;
    while (1) {
        ch = bflb_uart_getchar(uartx);
        if (ch != -1) {
            bflb_uart_putchar(uartx, ch);
        }
    }

- 调用 ``bflb_uart_getchar`` 从 uart rx fifo 中读取数据，如果返回 -1，表示没有数据
- 调用 ``bflb_uart_putchar`` 将数据 `ch` 填充到 uart tx fifo 中

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------

将 UART1 TX, RX, GND 引脚分别与 USB2TTL 模块 RX, TX, GND 相连，按下 reset 按键。
使用串口给 UART1 发送 ``0123456789`` ，USB2TTL 模块能接收到同样的数据。


