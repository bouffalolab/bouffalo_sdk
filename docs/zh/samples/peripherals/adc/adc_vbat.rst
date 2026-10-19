ADC - vbat
====================

本 demo 主要演示 adc 测量芯片 VDD33 的电压值。

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/adc/adc_vbat**

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 ADC IP 时钟，并选择 ADC 时钟源和分频(ADC 时钟必须小于等于 500K)。

.. code-block:: C
    :linenos:

    adc = bflb_device_get_by_name("adc");

    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    struct bflb_adc_channel_s chan;

    chan.pos_chan = ADC_CHANNEL_VABT_HALF;
    chan.neg_chan = ADC_CHANNEL_GND;

    bflb_adc_init(adc, &cfg);

- 获取 `adc` 句柄，并初始化 adc 配置，设置 adc 采样频率为 500K。

.. code-block:: C
    :linenos:

    bflb_adc_channel_config(adc, chan, 1);

- 配置 adc 通道信息。

.. code-block:: C
    :linenos:

    bflb_adc_vbat_enable(adc);

- 开启 vbat 功能。

.. code-block:: C
    :linenos:

    struct bflb_adc_result_s result;
    for (uint16_t i = 0; i < 10; i++) {
        bflb_adc_start_conversion(adc);
        while (bflb_adc_get_count(adc) == 0) {
            bflb_mtimer_delay_ms(1);
        }
        uint32_t raw_data = bflb_adc_read_raw(adc);

        bflb_adc_parse_result(adc, &raw_data, &result, 1);
        printf("vBat = %d mV\r\n", (uint32_t)(result.millivolt * 2));
        bflb_adc_stop_conversion(adc);

        bflb_mtimer_delay_ms(500);
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
打印芯片 VDD33 的电压值。