ISO11898 - selftest
====================

本 demo 主要介绍 ISO11898 数据自发自收。

硬件连接
-----------------------------

本 demo 使用到的 gpio 参考 ``board_iso11898_gpio_init`` ，将 TX 和 RX 连接。

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/iso11898/iso11898_selftest**

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 ISO11898 时钟，并选择时钟源和分频。

.. code-block:: C
    :linenos:

    board_iso11898_gpio_init();

- 配置相关引脚为 `ISO11898` 功能

.. code-block:: C
    :linenos:

    iso11898 = bflb_device_get_by_name("iso11898");

    bflb_iso11898_init(iso11898, &cfg);
    bflb_iso11898_set_filter(iso11898, &filter);
    ret = bflb_iso11898_send(iso11898, &msg_tx, 1000);

- 获取 `iso11898` 句柄
- 调用 ``bflb_iso11898_init(iso11898, &cfg)`` 初始化 iso11898
- 调用 ``bflb_iso11898_set_filter(iso11898, &filter)`` 开启接收标识符滤波功能
- 调用 ``bflb_iso11898_send(iso11898, &msg_tx, 1000)`` 发送帧信息，ID 号以及数据

.. code-block:: C
    :linenos:

    ret = bflb_iso11898_recv(iso11898, &msg_rx, 10000);

- 调用 ``bflb_iso11898_recv(iso11898, &msg_rx, 10000)`` 将接收到的数据存放在 msg_rx 中

.. code-block:: C
    :linenos:

    bflb_ir_send(irtx, tx_buffer, 1);
    rx_len = bflb_ir_receive(irrx, &rx_data);

- 调用 ``bflb_ir_send(irtx, tx_buffer, 1)`` 发送 tx_buffer 中的数据
- 调用 ``bflb_ir_receive(irrx, &rx_data)`` 将接收到的数据存放在 rx_data 中

编译和烧录
-----------------------------

参考 :ref:`linux_cmd` 或者 :ref:`windows_cmd`

实验现象
-----------------------------

按下开发板中 RST 按键，串口打印接收到的数据。
