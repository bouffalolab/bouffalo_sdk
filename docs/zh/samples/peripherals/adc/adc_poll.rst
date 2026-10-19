ADC - poll
====================

本 demo 主要演示 adc poll 单端模式下读取电压值。默认扫描通道 0 ~ 通道10。 **需要注意，有些芯片不一定支持全部通道**。

硬件连接
-----------------------------

本 demo 使用到的 gpio 参考 ``board_adc_gpio_init`` 。

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/adc/adc_poll**

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 ADC IP 时钟，并选择 ADC 时钟源和分频(ADC 时钟必须小于等于 500K)。

.. code-block:: C
    :linenos:

    board_adc_gpio_init();

- 配置相关引脚为 `ADC` 功能

.. code-block:: C
    :linenos:

    adc = bflb_device_get_by_name("adc");

    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = true;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    bflb_adc_init(adc, &cfg);

- 获取 `adc` 句柄，并初始化 adc 配置，设置 adc 采样频率为 500K

.. code-block:: C
    :linenos:

    bflb_adc_channel_config(adc, chan, TEST_ADC_CHANNELS);

- 配置 adc 通道信息，使用的对数根据 `TEST_ADC_CHANNELS` 可配，默认开启通道 0 ~ 10，根据 ``board_adc_gpio_init`` 需要选择性关闭其他通道。

.. code-block:: C
    :linenos:

    for (uint32_t i = 0; i < TEST_COUNT; i++) {
        bflb_adc_start_conversion(adc);

        while (bflb_adc_get_count(adc) < TEST_ADC_CHANNELS) {
            bflb_mtimer_delay_ms(1);
        }

        for (size_t j = 0; j < TEST_ADC_CHANNELS; j++) {
            struct bflb_adc_result_s result;
            uint32_t raw_data = bflb_adc_read_raw(adc);
            printf("raw data:%08x\r\n", raw_data);
            bflb_adc_parse_result(adc, &raw_data, &result, 1);
            printf("pos chan %d,%d mv \r\n", result.pos_chan, result.millivolt);
        }

        bflb_adc_stop_conversion(adc);
        bflb_mtimer_delay_ms(100);
    }

- 调用 ``bflb_adc_start_conversion(adc)`` 启用 adc 的转换
- 调用 ``bflb_adc_get_count(adc)`` 读取转换完成的个数
- 调用 ``bflb_adc_read_raw(adc)`` 读取一次 adc 的转换值
- 调用 ``bflb_adc_parse_result(adc, &raw_data, &result, 1)`` 对 adc 的转换结果进行解析，解析的值保存到 ``result`` 结构体中
- 调用 ``bflb_adc_stop_conversion(adc)`` 停止 adc 转换

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------
打印 raw data，通道号以及通道对应的电压值。