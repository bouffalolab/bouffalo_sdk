ADC - dma
====================

本 demo 主要演示 adc dma 单端模式下读取电压值。默认扫描通道 0 ~ 通道10。 **需要注意，有些芯片不一定支持全部通道**。

硬件连接
-----------------------------

本 demo 使用到的 gpio 参考 ``board_adc_gpio_init`` 。

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/adc/adc_dma**

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
    cfg.continuous_conv_mode = true;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    bflb_adc_init(adc, &cfg);

- 获取 `adc` 句柄，并初始化 adc 配置，设置 adc 采样频率为 500K

.. code-block:: C
    :linenos:

    bflb_adc_channel_config(adc, chan, TEST_ADC_CHANNELS);

- 配置 adc 通道信息，使用的通道数通过 `TEST_ADC_CHANNELS` 可配，默认开启通道 0 ~ 10，根据 ``board_adc_gpio_init`` 需要选择性关闭其他通道。

.. code-block:: C
    :linenos:

    bflb_adc_link_rxdma(adc, true);

- 使能 adc rx dma 功能

.. code-block:: C
    :linenos:

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");

    struct bflb_dma_channel_config_s config;

    config.direction = DMA_PERIPH_TO_MEMORY;
    config.src_req = DMA_REQUEST_ADC;
    config.dst_req = DMA_REQUEST_NONE;
    config.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.src_burst_count = DMA_BURST_INCR1;
    config.dst_burst_count = DMA_BURST_INCR1;
    config.src_width = DMA_DATA_WIDTH_32BIT;
    config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma0_ch0, &config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

- 配置 `DMA CH0` 为 `ADC RX` 
- 注册 dma 通道中断

.. code-block:: C
    :linenos:

    struct bflb_dma_channel_lli_pool_s lli[1]; /* max trasnfer size 4064 * 1 */
    struct bflb_dma_channel_lli_transfer_s transfers[1];

    memset(raw_data, 0, sizeof(raw_data));

    transfers[0].src_addr = (uint32_t)DMA_ADDR_ADC_RDR;
    transfers[0].dst_addr = (uint32_t)raw_data;
    transfers[0].nbytes = sizeof(raw_data);

    bflb_dma_channel_lli_reload(dma0_ch0, lli, 1, transfers, 1);
    bflb_dma_channel_start(dma0_ch0);

    bflb_adc_start_conversion(adc);

    while (dma_tc_flag0 != 1) {
        bflb_mtimer_delay_ms(1);
    }

    bflb_adc_stop_conversion(adc);


- 分配一块 lli 内存池，个数为1，最多可以传输 4064 * 1 字节
- 配置一块内存进行传输
- 调用 ``bflb_dma_channel_lli_reload`` 初始化
- 调用 ``bflb_dma_channel_start`` 启动传输
- 调用 ``bflb_adc_start_conversion`` 启用 adc 的转换
- 等待传输完成并进入中断
- 调用 ``bflb_adc_stop_conversion`` 停止 adc 转换

.. code-block:: C
    :linenos:

    for (size_t j = 0; j < TEST_ADC_CHANNELS * TEST_COUNT; j++) {
        struct bflb_adc_result_s result;
        printf("raw data:%08x\r\n", raw_data[j]);
        bflb_adc_parse_result(adc, &raw_data[j], &result, 1);
        printf("pos chan %d,%d mv \r\n", result.pos_chan, result.millivolt);
    }

- 调用 ``bflb_adc_parse_result`` 对 adc 的转换结果进行解析，解析的值保存到 ``result`` 结构体中
- 打印通道号和电压值

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------
打印 raw data，通道号以及通道对应的电压值。