UART - dma
====================

本 demo 主要演示 UART dma 模式收发功能。

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

具体软件代码见 **examples/peripherals/uart/uart_podma**

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

    bflb_uart_link_txdma(uartx, true);
    bflb_uart_link_rxdma(uartx, true);

- 使能 uart tx、rx dma 功能

.. code-block:: C
    :linenos:

    struct bflb_dma_channel_config_s config;

    config.direction = DMA_MEMORY_TO_PERIPH;
    config.src_req = DMA_REQUEST_NONE;
    config.dst_req = DEFAULT_TEST_UART_DMA_TX_REQUEST;
    config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config.src_burst_count = DMA_BURST_INCR1;
    config.dst_burst_count = DMA_BURST_INCR1;
    config.src_width = DMA_DATA_WIDTH_8BIT;
    config.dst_width = DMA_DATA_WIDTH_8BIT;
    bflb_dma_channel_init(dma0_ch0, &config);

    struct bflb_dma_channel_config_s rxconfig;

    rxconfig.direction = DMA_PERIPH_TO_MEMORY;
    rxconfig.src_req = DEFAULT_TEST_UART_DMA_RX_REQUEST;
    rxconfig.dst_req = DMA_REQUEST_NONE;
    rxconfig.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    rxconfig.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    rxconfig.src_burst_count = DMA_BURST_INCR1;
    rxconfig.dst_burst_count = DMA_BURST_INCR1;
    rxconfig.src_width = DMA_DATA_WIDTH_8BIT;
    rxconfig.dst_width = DMA_DATA_WIDTH_8BIT;
    bflb_dma_channel_init(dma0_ch1, &rxconfig);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);
    bflb_dma_channel_irq_attach(dma0_ch1, dma0_ch1_isr, NULL);

- 配置 `DMA CH0` 为 `UARTx TX` ， `DMA CH1` 为 `UARTx RX` .
- 注册 dma 通道中断

.. code-block:: C
    :linenos:

    struct bflb_dma_channel_lli_pool_s tx_llipool[20]; /* max trasnfer size 4064 * 20 */
    struct bflb_dma_channel_lli_transfer_s tx_transfers[3];

    tx_transfers[0].src_addr = (uint32_t)src_buffer;
    tx_transfers[0].dst_addr = (uint32_t)DEFAULT_TEST_UART_DMA_TDR;
    tx_transfers[0].nbytes = 4100;

    tx_transfers[1].src_addr = (uint32_t)src2_buffer;
    tx_transfers[1].dst_addr = (uint32_t)DEFAULT_TEST_UART_DMA_TDR;
    tx_transfers[1].nbytes = 4100;

    tx_transfers[2].src_addr = (uint32_t)src3_buffer;
    tx_transfers[2].dst_addr = (uint32_t)DEFAULT_TEST_UART_DMA_TDR;
    tx_transfers[2].nbytes = 4100;

    struct bflb_dma_channel_lli_pool_s rx_llipool[20];
    struct bflb_dma_channel_lli_transfer_s rx_transfers[1];
    rx_transfers[0].src_addr = (uint32_t)DEFAULT_TEST_UART_DMA_RDR;
    rx_transfers[0].dst_addr = (uint32_t)receive_buffer;
    rx_transfers[0].nbytes = 50;

    bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 20, tx_transfers, 3);
    bflb_dma_channel_lli_reload(dma0_ch1, rx_llipool, 20, rx_transfers, 1);
    bflb_dma_channel_start(dma0_ch0);
    bflb_dma_channel_start(dma0_ch1);

- 分配一块 lli 内存池，个数为20，最多可以传输 4094 * 20 字节
- 配置三块不连续的内存进行传输
- 调用 ``bflb_dma_channel_lli_reload`` 初始化
- 调用 ``bflb_dma_channel_start`` 启动传输
- 等待传输完成并进入中断

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------


