I2C - eeprom_dma
====================

本 demo 主要介绍 I2C 使用 DMA 的方式读写 eeprom。

硬件连接
-----------------------------

本 demo 使用到的 gpio 参考 ``board_i2c0_gpio_init`` ，将 eeprom 模块与开发板连接，具体引脚连接方式如下表（以BL616为例）：

.. table:: 硬件连接
    :widths: 50, 50
    :width: 80%
    :align: center

    +-------------------+------------------+
    | 开发板 I2C 引脚   | eeprom 模块      |
    +===================+==================+
    | SCL(GPIO14)       | SCL              |
    +-------------------+------------------+
    | SDA(GPIO15)       | SDA              |
    +-------------------+------------------+
    | GND               | GND              |
    +-------------------+------------------+
    | VCC               | VCC              |
    +-------------------+------------------+

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/i2c/i2c_eeprom_dma**

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 I2C IP 时钟，并选择 I2C 时钟源和分频。

.. code-block:: C
    :linenos:

    board_i2c0_gpio_init();

- 配置相关引脚为 `I2C` 功能

.. code-block:: C
    :linenos:

    /* Send and receive buffer init */
    for (size_t i = 0; i < 32; i++) {
        ((uint8_t *)send_buffer)[i] = i;
        ((uint8_t *)receive_buffer)[i] = 0;
    }

    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 400000);
    bflb_i2c_link_txdma(i2c0, true);
    bflb_i2c_link_rxdma(i2c0, true);

- 初始化发送和接收 buffer
- 获取 `i2c0` 句柄，并初始化 i2c0 频率为 400K
- ``bflb_i2c_link_txdma(i2c0, true)`` 开启 I2C TX DMA 功能
- ``bflb_i2c_link_rxdma(i2c0, true)`` 开启 I2C RX DMA 功能

.. code-block:: C
    :linenos:

    /* Write page 0 */
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");

    struct bflb_dma_channel_config_s tx_config;

    tx_config.direction = DMA_MEMORY_TO_PERIPH;
    tx_config.src_req = DMA_REQUEST_NONE;
    tx_config.dst_req = DMA_REQUEST_I2C0_TX;
    tx_config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    tx_config.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    tx_config.src_burst_count = DMA_BURST_INCR1;
    tx_config.dst_burst_count = DMA_BURST_INCR1;
    tx_config.src_width = DMA_DATA_WIDTH_32BIT;
    tx_config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma0_ch0, &tx_config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

- 对于 TX， DMA 的配置如下：传输方向(direction)为内存到外设(MEMORY_TO_PERIPH)，源请求(src_req)为内存，目标请求(dst_req)为 DMA_REQUEST_I2C0_TX
- 调用 ``bflb_dma_channel_init(dma0_ch0, &tx_config)`` 初始化 DMA
- 调用 ``bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL)`` 注册 dma 通道 0 中断

.. code-block:: C
    :linenos:

    struct bflb_dma_channel_lli_pool_s tx_llipool[20]; /* max trasnfer size 4064 * 20 */
    struct bflb_dma_channel_lli_transfer_s tx_transfers[1];
    tx_transfers[0].src_addr = (uint32_t)send_buffer;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_I2C0_TDR;
    tx_transfers[0].nbytes = 32;
    bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 20, tx_transfers, 1);

    msgs[0].addr = 0x50;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = subaddr;
    msgs[0].length = 2;

    msgs[1].addr = 0x50;
    msgs[1].flags = I2C_M_DMA;
    msgs[1].buffer = NULL;
    msgs[1].length = 32;
    bflb_i2c_transfer(i2c0, msgs, 2);

    bflb_dma_channel_start(dma0_ch0);

- 分配二十块 lli 内存池，最多可以传输 4064 * 20 字节
- 配置一块内存(tx_transfers)进行传输，源地址(src_addr)为存储发送数据的内存地址(send_buffer)，目标地址(dst_addr)为 I2C TX FIFO地址(DMA_ADDR_I2C0_TDR)
- 调用 ``bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 20, tx_transfers, 1)`` 初始化
- 调用 ``bflb_i2c_transfer(i2c0, msgs, 2)`` 开启 I2C 传输
- 调用 ``bflb_dma_channel_start(dma0_ch0)`` 启动 DMA 传输

.. code-block:: C
    :linenos:

    /* Read page 0 */
    dma0_ch1 = bflb_device_get_by_name("dma0_ch1");

    struct bflb_dma_channel_config_s rx_config;

    rx_config.direction = DMA_PERIPH_TO_MEMORY;
    rx_config.src_req = DMA_REQUEST_I2C0_RX;
    rx_config.dst_req = DMA_REQUEST_NONE;
    rx_config.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    rx_config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    rx_config.src_burst_count = DMA_BURST_INCR1;
    rx_config.dst_burst_count = DMA_BURST_INCR1;
    rx_config.src_width = DMA_DATA_WIDTH_32BIT;
    rx_config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma0_ch1, &rx_config);

    bflb_dma_channel_irq_attach(dma0_ch1, dma0_ch1_isr, NULL);

- 对于 RX， DMA 的配置如下：传输方向(direction)为外设到内存(PERIPH_TO_MEMORY)，源请求(src_req)为 DMA_REQUEST_I2C0_RX ，目标请求(dst_req)为内存
- 调用 ``bflb_dma_channel_init(dma0_ch1, &rx_config)`` 初始化 DMA
- 调用 ``bflb_dma_channel_irq_attach(dma0_ch1, dma0_ch1_isr, NULL)`` 注册 dma 通道 1 中断

.. code-block:: C
    :linenos:

    struct bflb_dma_channel_lli_pool_s rx_llipool[20];
    struct bflb_dma_channel_lli_transfer_s rx_transfers[1];
    rx_transfers[0].src_addr = (uint32_t)DMA_ADDR_I2C0_RDR;
    rx_transfers[0].dst_addr = (uint32_t)receive_buffer;
    rx_transfers[0].nbytes = 32;

    bflb_dma_channel_lli_reload(dma0_ch1, rx_llipool, 20, rx_transfers, 1);

    msgs[1].addr = 0x50;
    msgs[1].flags = I2C_M_DMA | I2C_M_READ;
    msgs[1].buffer = NULL;
    msgs[1].length = 32;
    bflb_i2c_transfer(i2c0, msgs, 2);

    bflb_dma_channel_start(dma0_ch1);

- 分配二十块 lli 内存池，最多可以传输 4064 * 20 字节
- 配置一块内存(rx_transfers)进行传输，源地址(src_addr)为 I2C RX FIFO地址(DMA_ADDR_I2C0_RDR)，目标地址(dst_addr)为存储接收数据的内存地址(receive_buffer)
- 调用 ``bflb_dma_channel_lli_reload(dma0_ch1, rx_llipool, 20, rx_transfers, 1)`` 初始化
- 调用 ``bflb_i2c_transfer(i2c0, msgs, 2)`` 开启 I2C 传输
- 调用 ``bflb_dma_channel_start(dma0_ch1)`` 启动 DMA 传输

.. code-block:: C
    :linenos:

    while (dma_tc_flag1 == 0) {
    }
    while ((bflb_i2c_get_intstatus(i2c0) & I2C_INTSTS_END) == 0) {
    }
    bflb_i2c_deinit(i2c0);

- 数据传输完成后，复位 I2C 模块

.. code-block:: C
    :linenos:

    /* Check read data */
    for (uint8_t i = 0; i < 32; i++) {
        if (((uint8_t *)send_buffer)[i] != ((uint8_t *)receive_buffer)[i]) {
            printf("check fail, %d write: %02x, read: %02x\r\n", i, ((uint8_t *)send_buffer)[i], ((uint8_t *)receive_buffer)[i]);
        }
    }

- 检查发送和读取的数据是否一致

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------

按下 RST 按键，数据传输完成后，打印“write over”，“read over”和“check over”。