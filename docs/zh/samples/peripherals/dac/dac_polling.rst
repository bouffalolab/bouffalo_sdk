DAC - poll
====================

本 demo 主要介绍基于 DAC 轮询模式生成正弦波。

硬件连接
-----------------------------

本 demo 使用到的 gpio 参考 ``board_adc_gpio_init`` 。

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/dac/dac_polling**

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 DAC IP 时钟，并选择 DAC 时钟源和分频。

.. code-block:: C
    :linenos:

    board_dac_gpio_init();

- 配置相关引脚为 `DAC` 功能

.. code-block:: C
    :linenos:

    dac = bflb_device_get_by_name("dac");

    bflb_dac_init(dac, DAC_SAMPLING_FREQ_32K);

- 获取 `dac` 句柄，并初始化 dac 频率为 32K

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

参考 :ref:`get_started`

实验现象
-----------------------------

DAC Channel A 对应的 GPIO 输出正弦波。
