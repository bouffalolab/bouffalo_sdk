ADC - tsen
====================

本 demo 主要演示通过 adc 测量二极管的电压差，计算得到环境温度。

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/adc/adc_tsen**

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
    cfg.vref = ADC_VREF_2P0V;

    struct bflb_adc_channel_s chan;

    chan.pos_chan = ADC_CHANNEL_TSEN_P;
    chan.neg_chan = ADC_CHANNEL_GND;

    bflb_adc_init(adc, &cfg);

- 获取 `adc` 句柄，并初始化 adc 配置(参考电压必须设置为2.0V)，设置 adc 采样频率为 500K。

.. code-block:: C
    :linenos:

    bflb_adc_channel_config(adc, chan, 1);

- 配置 adc 通道信息。

.. code-block:: C
    :linenos:

    bflb_adc_tsen_init(adc, ADC_TSEN_MOD_INTERNAL_DIODE);

- 开启 tsen 功能，使用内部二极管测量电压值。

.. code-block:: C
    :linenos:

        for (i = 0; i < 50; i++) {
            average_filter += bflb_adc_tsen_get_temp(adc);
            bflb_mtimer_delay_ms(10);
        }

        printf("temp = %d\r\n", (uint32_t)(average_filter / 50.0));
        average_filter = 0;

- 调用 ``bflb_adc_tsen_get_temp(adc)`` 获取 adc tsen 计算得到的环境温度值。

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------
打印计算得到的环境温度。