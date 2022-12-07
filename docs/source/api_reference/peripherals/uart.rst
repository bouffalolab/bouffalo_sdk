UART
=============

Macros
------------

uart direction
^^^^^^^^^^^^^^^^^^

UART 方向可以设置为 TX、RX、TXRX

uart databits
^^^^^^^^^^^^^^^^^^

.. code-block:: c
   :linenos:

    #define UART_DATA_BITS_5 0
    #define UART_DATA_BITS_6 1
    #define UART_DATA_BITS_7 2
    #define UART_DATA_BITS_8 3
    #define UART_DATA_BITS_9 4

uart stopbits
^^^^^^^^^^^^^^^^^^

.. code-block:: c
   :linenos:

    #define UART_STOP_BITS_0_5 0
    #define UART_STOP_BITS_1   1
    #define UART_STOP_BITS_1_5 2
    #define UART_STOP_BITS_2   3

uart parity
^^^^^^^^^^^^^^^^^^

.. code-block:: c
   :linenos:

    #define UART_PARITY_NONE  0
    #define UART_PARITY_ODD   1
    #define UART_PARITY_EVEN  2
    #define UART_PARITY_MARK  3
    #define UART_PARITY_SPACE 4

uart bit order
^^^^^^^^^^^^^^^^^^

.. code-block:: c
   :linenos:

    #define UART_LSB_FIRST 0
    #define UART_MSB_FIRST 1

Structs
------------

struct bflb_uart_config_s
^^^^^^^^^^^^^^^^^^^^^^^^^^

uart 初始化配置结构体。

.. code-block:: c
   :linenos:

    struct bflb_uart_config_s {
        uint32_t baudrate;
        uint8_t direction;
        uint8_t data_bits;
        uint8_t stop_bits;
        uint8_t parity;
        uint8_t bit_order;
        uint8_t flow_ctrl;
        uint8_t tx_fifo_threshold;
        uint8_t rx_fifo_threshold;
    };

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - baudrate
      - 波特率
    * - direction
      - 方向
    * - data_bits
      - 数据位
    * - stop_bits
      - 停止位
    * - parity
      - 校验位
    * - bit_order
      - bit 先行方式
    * - flow_ctrl
      - 流控
    * - tx_fifo_threshold
      - 发送 fifo 中断触发阈值(大于阈值触发中断)
    * - rx_fifo_threshold
      - 接收 fifo 中断触发阈值(大于阈值触发中断)

.. note::  BL702 阈值为 128, 其他芯片为 32

Functions
------------

bflb_uart_init
^^^^^^^^^^^^^^^^^^^^

说明: 初始化 uart。使用之前需要开启 uart ip 时钟、设置 uart 时钟源和分频值、选择 gpio 为 uart 中的一个功能。

.. code-block:: c
   :linenos:

    void bflb_uart_init(struct bflb_device_s *dev, const struct bflb_uart_config_s *config);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - config
      - 配置项

bflb_uart_deinit
^^^^^^^^^^^^^^^^^^^^

说明: 反初始化 uart。

.. code-block:: c
   :linenos:

    void bflb_uart_deinit(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_uart_link_txdma
^^^^^^^^^^^^^^^^^^^^^^^

说明: uart tx dma 使能开关。

.. code-block:: c
   :linenos:

    void bflb_uart_link_txdma(struct bflb_device_s *dev, bool enable);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - enable
      - 是否使能 dma

bflb_uart_link_rxdma
^^^^^^^^^^^^^^^^^^^^^^^

说明: uart rx dma 使能开关。

.. code-block:: c
   :linenos:

    void bflb_uart_link_rxdma(struct bflb_device_s *dev, bool enable);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - enable
      - 是否使能 dma

bflb_uart_putchar
^^^^^^^^^^^^^^^^^^^^

说明: 通过 uart 阻塞式发送一个字符。

.. code-block:: c
   :linenos:

    void bflb_uart_putchar(struct bflb_device_s *dev, int ch);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 字符

bflb_uart_getchar
^^^^^^^^^^^^^^^^^^^^

说明: 通过 uart 异步接收一个字符。

.. code-block:: c
   :linenos:

    int bflb_uart_getchar(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 返回 -1 表示没有数据，返回其他表示接收的字符

bflb_uart_txready
^^^^^^^^^^^^^^^^^^^^

说明: 查询 uart tx fifo 是否准备就绪，准备好才可以填充字符。

.. code-block:: c
   :linenos:

    bool bflb_uart_txready(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 为 true 表示就绪

bflb_uart_txempty
^^^^^^^^^^^^^^^^^^^^

说明: 查询 uart tx fifo 是否为空。

.. code-block:: c
   :linenos:

    bool bflb_uart_txempty(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 为 true 表示 fifo 已空，无法填充数据

bflb_uart_rxavailable
^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 查询 uart rx 是否有数据。

.. code-block:: c
   :linenos:

    bool bflb_uart_rxavailable(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 为 true 表示有数据，可以进行读取

bflb_uart_txint_mask
^^^^^^^^^^^^^^^^^^^^^^^

说明:  uart tx fifo 阈值中断屏蔽开关，开启后超过设定阈值则触发中断。

.. code-block:: c
   :linenos:

    void bflb_uart_txint_mask(struct bflb_device_s *dev, bool mask);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - mask
      - 是否屏蔽中断

bflb_uart_rxint_mask
^^^^^^^^^^^^^^^^^^^^^^^

说明:  uart rx fifo 阈值中断和超时屏蔽开关，开启后超过设定阈值则或者超时则触发中断。

.. code-block:: c
   :linenos:

    void bflb_uart_rxint_mask(struct bflb_device_s *dev, bool mask);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - mask
      - 是否屏蔽中断

bflb_uart_errint_mask
^^^^^^^^^^^^^^^^^^^^^^^

说明:  uart 错误中断屏蔽开关。

.. code-block:: c
   :linenos:

    void bflb_uart_errint_mask(struct bflb_device_s *dev, bool mask);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - mask
      - 是否屏蔽中断

bflb_uart_get_intstatus
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明:  获取 uart 中断标志。

.. code-block:: c
   :linenos:

    uint32_t bflb_uart_get_intstatus(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 中断标志

中断标志有以下选项:

.. code-block:: c
   :linenos:

   #define UART_INTSTS_TX_END  (1 << 0)
   #define UART_INTSTS_RX_END  (1 << 1)
   #define UART_INTSTS_TX_FIFO (1 << 2)
   #define UART_INTSTS_RX_FIFO (1 << 3)
   #define UART_INTSTS_RTO     (1 << 4)
   #define UART_INTSTS_PCE     (1 << 5)
   #define UART_INTSTS_TX_FER  (1 << 6)
   #define UART_INTSTS_RX_FER  (1 << 7)
   #if !defined(BL602)
   #define UART_INTSTS_RX_LSE (1 << 8)
   #endif
   #if !defined(BL602) && !defined(BL702)
   #define UART_INTSTS_RX_BCR (1 << 9)
   #define UART_INTSTS_RX_ADS (1 << 10)
   #define UART_INTSTS_RX_AD5 (1 << 11)
   #endif

bflb_uart_int_clear
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明:  清除 uart 中断标志。

.. code-block:: c
   :linenos:

    void bflb_uart_int_clear(struct bflb_device_s *dev, uint32_t int_clear);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - int_clear
      - 清除值

`int_clear` 可以填入以下参数:

.. code-block:: c
   :linenos:

   #define UART_INTCLR_TX_END (1 << 0)
   #define UART_INTCLR_RX_END (1 << 1)
   #define UART_INTCLR_RTO    (1 << 4)
   #define UART_INTCLR_PCE    (1 << 5)
   #if !defined(BL602)
   #define UART_INTCLR_RX_LSE (1 << 8)
   #endif
   #if !defined(BL602) && !defined(BL702)
   #define UART_INTCLR_RX_BCR (1 << 9)
   #define UART_INTCLR_RX_ADS (1 << 10)
   #define UART_INTCLR_RX_AD5 (1 << 11)
   #endif

bflb_uart_feature_control
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明:  uart 其他特性相关控制，一般不常用。

.. code-block:: c
   :linenos:

    int bflb_uart_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - cmd
      - 控制字
    * - arg
      - 控制参数
    * - return
      - 负值表示不支持此命令

`cmd` 可以填入以下参数:

.. code-block:: c
   :linenos:

   #define UART_CMD_SET_BAUD_RATE           (0x01)
   #define UART_CMD_SET_DATA_BITS           (0x02)
   #define UART_CMD_SET_STOP_BITS           (0x03)
   #define UART_CMD_SET_PARITY_BITS         (0x04)
   #define UART_CMD_CLR_TX_FIFO             (0x05)
   #define UART_CMD_CLR_RX_FIFO             (0x06)
   #define UART_CMD_SET_RTO_VALUE           (0x07)
   #define UART_CMD_SET_RTS_VALUE           (0x08)
   #define UART_CMD_GET_TX_FIFO_CNT         (0x09)
   #define UART_CMD_GET_RX_FIFO_CNT         (0x0a)
   #define UART_CMD_SET_AUTO_BAUD           (0x0b)
   #define UART_CMD_GET_AUTO_BAUD           (0x0c)
   #define UART_CMD_SET_BREAK_VALUE         (0x0d)
   #define UART_CMD_SET_TX_LIN_VALUE        (0x0e)
   #define UART_CMD_SET_RX_LIN_VALUE        (0x0f)
   #define UART_CMD_SET_TX_RX_EN            (0x10)
   #define UART_CMD_SET_TX_RS485_EN         (0x11)
   #define UART_CMD_SET_TX_RS485_POLARITY   (0x12)
   #define UART_CMD_SET_ABR_ALLOWABLE_ERROR (0x13)
   #define UART_CMD_SET_SW_RTS_CONTROL      (0x14)
   #define UART_CMD_IR_CONFIG               (0x15)
   #define UART_CMD_SET_TX_FREERUN          (0x16)
   #define UART_CMD_SET_TX_END_INTERRUPT    (0x17)
   #define UART_CMD_SET_RX_END_INTERRUPT    (0x18)
   #define UART_CMD_SET_TX_TRANSFER_LEN     (0x19)
   #define UART_CMD_SET_RX_TRANSFER_LEN     (0x20)
   #define UART_CMD_SET_TX_EN               (0x21)
   #define UART_CMD_SET_BCR_END_INTERRUPT   (0x22)
   #define UART_CMD_GET_BCR_COUNT           (0x23)