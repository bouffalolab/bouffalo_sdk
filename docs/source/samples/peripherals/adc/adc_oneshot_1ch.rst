ADC - 电压检测
====================

本 demo 主要介绍基于 ADC 的电压检测功能，使用 ADC 检测 CH8 输入的电压值。

硬件连接
-----------------------------

将 ADC CH8 对应的 GPIO 口与 3.3 V 引脚相连。不同芯片 ADC CH8 对应的 GPIO 口如下表所示：

.. table:: GPIO 口
    :widths: 30, 30, 40
    :width: 80%
    :align: center

    +----------+-----------+---------------------------+
    |   名称   | 芯片型号  |           GPIO            |
    +==========+===========+===========================+
    | ADC CH8  | BL702     | GPIO 18                   |
    +          +-----------+---------------------------+
    |          | BL616     | GPIO 1                    |
    +----------+-----------+---------------------------+

软件实现
-----------------------------

更详细的代码请参考 ``examples/peripherals/adc/adc_oneshot_1ch``

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 adc 外设，并且初始化 adc 的时钟

.. code-block:: C
    :linenos:

    board_adc_gpio_init();

- 配置相关引脚为 `ADC` 功能

.. code-block:: C
    :linenos:

    adc = bflb_device_get_by_name("adc");

    struct bflb_adc_config_s cfg;

    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    bflb_adc_init(adc, &cfg);

- 获取 `adc` 句柄，并初始化 adc

.. code-block:: C
    :linenos:

    struct bflb_adc_channel_s chan;

    chan.pos_chan = ADC_CHANNEL_8;
    chan.neg_chan = ADC_CHANNEL_GND;

    bflb_adc_channel_config(adc, &chan, 1);

- 配置 adc 通道信息，当前使用了一对通道

.. code-block:: C
    :linenos:

    for (uint32_t i = 0; i < 10; i++) {
        bflb_adc_start_conversion(adc);

        while (bflb_adc_get_count(adc) == 0) {
        }
        struct bflb_adc_result_s result;
        uint32_t raw_data = bflb_adc_read_raw(adc);
        printf("raw data:%08x\r\n", raw_data);
        bflb_adc_parse_result(adc, &raw_data, &result, 1);
        printf("pos chan %d,%d mv \r\n", result.pos_chan, result.millivolt);
        bflb_adc_stop_conversion(adc);
        bflb_mtimer_delay_ms(100);
    }

- 调用 ``bflb_adc_start_conversion(adc)`` 启用 adc 的转换
- 调用 ``bflb_adc_read_raw(adc)`` 读取一次 adc 的转换值
- 调用 ``bflb_adc_parse_result(adc, &raw_data, &result, 1)`` 对 adc 的转换结果进行解析，解析的值保存到 ``result`` 结构体中
- 调用 ``bflb_adc_stop_conversion(adc)`` 停止 adc 转换

编译和烧录
-----------------------------

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/examples/peripherals/adc/adc_oneshot_1ch
    $ make CHIP=blxxx BOARD=blxxx_dk

.. note:: blxxx为所使用的芯片型号，以bl616为例，编译命令为：make CHIP=bl616 BOARD=bl616_dk

-  **烧录**

   详见 :ref:`bl_dev_cube`

实验现象
-----------------------------

将 ADC_CH8 对应的 GPIO口与 3.3V 相连，打印的信息如下：

.. figure:: img/adc_log.png
   :align: center

   log 信息

