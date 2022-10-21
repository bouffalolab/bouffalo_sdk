DAC - 正弦波
====================

本 demo 主要介绍基于 DAC 生成正弦波。

硬件连接
-----------------------------

不同芯片 DAC Channel A 对应的 GPIO 口如下表所示：

.. table:: GPIO 口
    :widths: 30, 30, 40
    :width: 80%
    :align: center

    +----------------+-----------+---------------------------+
    |   名称         | 芯片型号  |           GPIO            |
    +================+===========+===========================+
    | DAC Channel A  | BL702     | GPIO 11                   |
    +                +-----------+---------------------------+
    |                | BL616     | GPIO 3                    |
    +----------------+-----------+---------------------------+

软件实现
-----------------------------

更详细的代码请参考 ``examples/peripherals/dac/dac_polling``

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 dac 外设

.. code-block:: C
    :linenos:

    board_dac_gpio_init();

- 配置相关引脚为 `DAC` 功能

.. code-block:: C
    :linenos:

    dac = bflb_device_get_by_name("dac");

    bflb_dac_init(dac, DAC_SAMPLING_FREQ_32K);

- 获取 `dac` 句柄，并初始化 dac，时钟配置为 32K

.. code-block:: C
    :linenos:

    bflb_dac_channel_enable(dac, DAC_CHANNEL_A);

- 配置 dac 通道信息，当前使用的 A 通道

.. code-block:: C
    :linenos:

    for (uint16_t i = 0; i < sizeof(SIN_LIST) / sizeof(uint16_t); i++) {
        bflb_dac_set_value(dac, DAC_CHANNEL_A, SIN_LIST[i]);
        bflb_mtimer_delay_us(100);
    }

- 调用 ``bflb_dac_set_value(dac, DAC_CHANNEL_A, SIN_LIST[i])`` ，将需要转换的数据通过通道 A 输出

编译和烧录
-----------------------------

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/examples/peripherals/dac/dac_polling
    $ make CHIP=blxxx BOARD=blxxx_dk

.. note:: blxxx为所使用的芯片型号，以bl616为例，编译命令为：make CHIP=bl616 BOARD=bl616_dk

-  **烧录**

   详见 :ref:`bl_dev_cube`

实验现象
-----------------------------

DAC Channel A 对应的 GPIO 输出正弦波。
