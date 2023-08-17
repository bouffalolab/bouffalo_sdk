DAC - dma
====================

本 demo 主要介绍基于 DAC DMA 模式生成正弦波。

硬件连接
-----------------------------

本 demo 使用到的 gpio 参考 ``board_dac_gpio_init`` 。

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/dac/dac_dma**

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

    /* 512K / 1 = 512K */
    bflb_dac_init(dac, DAC_CLK_DIV_1);
    bflb_dac_channel_enable(dac, DAC_CHANNEL_A);
    bflb_dac_channel_enable(dac, DAC_CHANNEL_B);
    bflb_dac_link_txdma(dac, true);

- 获取 `dac` 句柄，并初始化 dac 频率为 512K
- ``bflb_dac_channel_enable`` 配置 dac 通道信息，当前使用的 A 通道和 B 通道
- ``bflb_dac_link_txdma`` 开启 dac txdma 功能

.. code-block:: C
    :linenos:

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");

    struct bflb_dma_channel_config_s config;

    config.direction = DMA_MEMORY_TO_PERIPH;
    config.src_req = DMA_REQUEST_NONE;
    config.dst_req = DMA_REQUEST_DAC;
    config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config.src_burst_count = DMA_BURST_INCR1;
    config.dst_burst_count = DMA_BURST_INCR1;
    config.src_width = DMA_DATA_WIDTH_16BIT;
    config.dst_width = DMA_DATA_WIDTH_16BIT;
    bflb_dma_channel_init(dma0_ch0, &config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

- 配置 `DMA CH0` 为 `DAC` 
- 注册 dma 通道中断

.. code-block:: C
    :linenos:

    struct bflb_dma_channel_lli_pool_s lli[1]; /* max trasnfer size 4064 * 1 */
    struct bflb_dma_channel_lli_transfer_s transfers[1];

    transfers[0].src_addr = (uint32_t)SIN_LIST;
    transfers[0].dst_addr = (uint32_t)DMA_ADDR_DAC_TDR;
    transfers[0].nbytes = sizeof(SIN_LIST);
    bflb_l1c_dcache_clean_range((void*)SIN_LIST,sizeof(SIN_LIST));

    bflb_dma_channel_lli_reload(dma0_ch0, lli, 1, transfers, 1);
    bflb_dma_channel_start(dma0_ch0);

    while (dma_tc_flag0 != 1) {
        bflb_mtimer_delay_ms(1);
    }

- 分配一块 lli 内存池，个数为1，最多可以传输 4064 * 1 字节
- 配置一块内存进行传输
- 调用 ``bflb_dma_channel_lli_reload`` 初始化
- 调用 ``bflb_dma_channel_start`` 启动传输
- 等待传输完成并进入中断

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------

DAC Channel A 和 B 对应的 GPIO 输出正弦波。
