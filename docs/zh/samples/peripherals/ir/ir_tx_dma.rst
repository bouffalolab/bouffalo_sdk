IR - tx_dma
====================

本 demo 主要介绍 IR 使用 DMA 的方式发送数据。

硬件连接
-----------------------------

本 demo 使用到的 gpio 参考 ``board_ir_gpio_init`` ，将红外发射二极管与 IR 引脚连接，具体连接方式如下表（以BL808为例）：

.. table:: 硬件连接
    :widths: 50, 50
    :width: 80%
    :align: center

    +-------------------+----------------------+
    | 开发板 IR 引脚    | 外接模块             |
    +===================+======================+
    | VCC               | 红外发射二极管正极   |
    +-------------------+----------------------+
    | TX(GPIO11)        | 红外发射二极管负极   |
    +-------------------+----------------------+

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/ir/ir_tx_dma**

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 IR 时钟，并选择 IR 时钟源和分频。

.. code-block:: C
    :linenos:

    board_ir_gpio_init();

- 配置相关引脚为 `IR` 功能

.. code-block:: C
    :linenos:

    struct bflb_ir_tx_config_s tx_cfg = {
        .tx_mode = IR_TX_CUSTOMIZE,
        .data_bits = 0,
        .tail_inverse = 0,
        .tail_enable = 0,
        .head_inverse = 0,
        .head_enable = 0,
        .logic1_inverse = 1,
        .logic0_inverse = 1,
        .data_enable = 1,
        .swm_enable = 0,
        .output_modulation = 1,
        .output_inverse = 0,
        .freerun_enable = 1,
        .continue_enable = 1,
        .fifo_width = IR_TX_FIFO_WIDTH_24BIT,
        .fifo_threshold = 1,
        .logic0_pulse_width_1 = 0,
        .logic0_pulse_width_0 = 0,
        .logic1_pulse_width_1 = 2,
        .logic1_pulse_width_0 = 0,
        .head_pulse_width_1 = 0,
        .head_pulse_width_0 = 0,
        .tail_pulse_width_1 = 0,
        .tail_pulse_width_0 = 0,
        .modu_width_1 = 17,
        .modu_width_0 = 34,
        .pulse_width_unit = 1124,
    };

    irtx = bflb_device_get_by_name("irtx");

    /* TX init */
    bflb_ir_tx_init(irtx, &tx_cfg);
    bflb_ir_link_txdma(irtx, true);
    bflb_ir_tx_enable(irtx, true);

- 获取 `irtx` 句柄
- 设置 tx_mode 为 IR_TX_CUSTOMIZE 模式，调用 ``bflb_ir_tx_init(irtx, &tx_cfg)`` 初始化 ir tx
- 调用 ``bflb_ir_link_txdma(irtx, true)`` 使能 ir tx dma 功能
- 调用 ``bflb_ir_tx_enable(irtx, true)`` 开启 ir tx 

.. code-block:: C
    :linenos:

    struct bflb_dma_channel_config_s dma_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_IR_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };

    for (i = 0; i < 128; i++) {
        tx_buffer[i] = i * 0x01010101;
    }

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    bflb_dma_channel_init(dma0_ch0, &dma_config);
    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

- 对于 TX， DMA 的配置如下：传输方向(direction)为内存到外设(MEMORY_TO_PERIPH)，源请求(src_req)为内存，目标请求(dst_req)为 DMA_REQUEST_IR_TX
- 初始化 tx_buffer
- 调用 ``bflb_dma_channel_init(dma0_ch0, &dma_config)`` 初始化 DMA
- 调用 ``bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL)`` 注册 dma 通道 0 中断

.. code-block:: C
    :linenos:

    struct bflb_dma_channel_lli_pool_s tx_llipool[1];
    struct bflb_dma_channel_lli_transfer_s tx_transfers[1];

    tx_transfers[0].src_addr = (uint32_t)tx_buffer;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_IR_TDR;
    tx_transfers[0].nbytes = 128 * 4;
    bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 1, tx_transfers, 1);
    bflb_dma_channel_start(dma0_ch0);

- 分配一块 lli 内存池，最多可以传输 4064 * 1 字节
- 配置一块内存(tx_transfers)进行传输，源地址(src_addr)为存储发送数据的内存地址(tx_buffer)，目标地址(dst_addr)为 IR TX FIFO地址(DMA_ADDR_IR_TDR)
- 调用 ``bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 1, tx_transfers, 1)`` 初始化
- 调用 ``bflb_dma_channel_start(dma0_ch0)`` 启动 DMA 传输

.. code-block:: C
    :linenos:

    while (dma_tc_flag0 != 1) {
        bflb_mtimer_delay_ms(1);
    }
    printf("Check wave\r\n");

- DMA 传输完成后，查看波形

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------

按下 RST 按键，数据传输完成后，查看波形是否正确。