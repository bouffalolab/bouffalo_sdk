SPI
=============

Macros
------------

spi role
^^^^^^^^^^^^

spi 支持主从角色。

.. code-block:: c
   :linenos:

    #define SPI_ROLE_MASTER 0
    #define SPI_ROLE_SLAVE  1

spi mode
^^^^^^^^^^^^

spi 支持 4 种工作模式。

.. code-block:: c
   :linenos:

    #define SPI_DEV_MODE0 0 /* CPOL=0 CHPHA=0 */
    #define SPI_DEV_MODE1 1 /* CPOL=0 CHPHA=1 */
    #define SPI_DEV_MODE2 2 /* CPOL=1 CHPHA=0 */
    #define SPI_DEV_MODE3 3 /* CPOL=1 CHPHA=1 */

spi data_width
^^^^^^^^^^^^^^^

spi 支持 4 种位宽。

.. code-block:: c
   :linenos:

    #define SPI_DATA_WIDTH_8BIT  0
    #define SPI_DATA_WIDTH_16BIT 1
    #define SPI_DATA_WIDTH_24BIT 2
    #define SPI_DATA_WIDTH_32BIT 3

spi bit_order
^^^^^^^^^^^^^^^

spi bit 先行方式。

.. code-block:: c
   :linenos:

    #define SPI_BIT_LSB 1
    #define SPI_BIT_MSB 0

spi byte_order
^^^^^^^^^^^^^^^

spi 字节先行方式。

.. code-block:: c
   :linenos:

    #define SPI_BYTE_LSB 0
    #define SPI_BYTE_MSB 1

Structs
------------

struct bflb_spi_config_s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

spi 初始化配置结构体。

.. code-block:: c
   :linenos:

    struct bflb_spi_config_s {
        uint32_t freq;
        uint8_t role;
        uint8_t mode;
        uint8_t data_width;
        uint8_t bit_order;
        uint8_t byte_order;
        uint8_t tx_fifo_threshold;
        uint8_t rx_fifo_threshold;
    };

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - freq
      - 频率(BL702 不得超过32M,其他芯片不得超过 40M)
    * - role
      - 主从选择
    * - mode
      - 工作模式
    * - data_width
      - 数据宽度
    * - bit_order
      - bit 先行方式
    * - byte_order
      - 字节先行方式
    * - tx_fifo_threshold
      - 发送 fifo 中断触发阈值(大于阈值触发中断)
    * - rx_fifo_threshold
      - 接收 fifo 中断触发阈值(大于阈值触发中断)

.. note:: SPI 阈值最大为 16

Functions
------------

bflb_spi_init
^^^^^^^^^^^^^^^^^^^^

说明: 初始化 spi。使用之前需要开启 spi ip 时钟、设置 spi 时钟源和分频值、选择 gpio 为 spi 功能。

.. code-block:: c
   :linenos:

    void bflb_spi_init(struct bflb_device_s *dev, const struct bflb_spi_config_s *config);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - config
      - 配置项

bflb_spi_deinit
^^^^^^^^^^^^^^^^^^^^

说明: 反初始化 spi。

.. code-block:: c
   :linenos:

    void bflb_spi_deinit(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_spi_link_txdma
^^^^^^^^^^^^^^^^^^^^^^^

说明: spi tx dma 使能开关。

.. code-block:: c
   :linenos:

    void bflb_spi_link_txdma(struct bflb_device_s *dev, bool enable);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - enable
      - 是否使能 dma

bflb_spi_link_rxdma
^^^^^^^^^^^^^^^^^^^^^^^

说明: spi rx dma 使能开关。

.. code-block:: c
   :linenos:

    void bflb_spi_link_rxdma(struct bflb_device_s *dev, bool enable);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - enable
      - 是否使能 dma

bflb_spi_poll_send
^^^^^^^^^^^^^^^^^^^^

说明: 通过 spi 阻塞式发送一个数据并接收一个数据。

.. code-block:: c
   :linenos:

    uint32_t bflb_spi_poll_send(struct bflb_device_s *dev, uint32_t data);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - data
      - 发送的数据(可以为字节、字、双字)
    * - return
      - 接收的数据(可以为字节、字、双字)

bflb_spi_poll_exchange
^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 通过 spi 阻塞式发送一段数据并接收一段数据。

.. code-block:: c
   :linenos:

    int bflb_spi_poll_exchange(struct bflb_device_s *dev, const void *txbuffer, void *rxbuffer, size_t nbyte);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - txbuffer
      - 发送缓冲区
    * - rxbuffer
      - 接收缓冲区
    * - nbyte
      - 数据长度

bflb_spi_txint_mask
^^^^^^^^^^^^^^^^^^^^^^^

说明:  spi tx fifo 阈值中断屏蔽开关，开启后超过设定阈值则触发中断。

.. code-block:: c
   :linenos:

    void bflb_spi_txint_mask(struct bflb_device_s *dev, bool mask);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - mask
      - 是否屏蔽中断

bflb_spi_rxint_mask
^^^^^^^^^^^^^^^^^^^^^^^

说明:  spi rx fifo 阈值中断和超时屏蔽开关，开启后超过设定阈值则或者超时则触发中断。

.. code-block:: c
   :linenos:

    void bflb_spi_rxint_mask(struct bflb_device_s *dev, bool mask);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - mask
      - 是否屏蔽中断

bflb_spi_errint_mask
^^^^^^^^^^^^^^^^^^^^^^^

说明:  spi 错误中断屏蔽开关。

.. code-block:: c
   :linenos:

    void bflb_spi_errint_mask(struct bflb_device_s *dev, bool mask);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - mask
      - 是否屏蔽中断

bflb_spi_get_intstatus
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明:  获取 spi 中断标志。

.. code-block:: c
   :linenos:

    uint32_t bflb_spi_get_intstatus(struct bflb_device_s *dev);

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

   #define SPI_INTSTS_TC                (1 << 0)
   #define SPI_INTSTS_TX_FIFO           (1 << 1)
   #define SPI_INTSTS_RX_FIFO           (1 << 2)
   #define SPI_INTSTS_SLAVE_TIMEOUT     (1 << 3)
   #define SPI_INTSTS_SLAVE_TX_UNDERRUN (1 << 4)
   #define SPI_INTSTS_FIFO_ERR          (1 << 5)

bflb_spi_int_clear
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明:  清除 spi 中断标志。

.. code-block:: c
   :linenos:

    void bflb_spi_int_clear(struct bflb_device_s *dev, uint32_t int_clear);

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

   #define SPI_INTCLR_TC                (1 << 16)
   #define SPI_INTCLR_SLAVE_TIMEOUT     (1 << 19)
   #define SPI_INTCLR_SLAVE_TX_UNDERRUN (1 << 20)

bflb_spi_feature_control
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明:  spi 其他特性相关控制，一般不常用。

.. code-block:: c
   :linenos:

    int bflb_spi_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

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

    #define SPI_CMD_SET_DATA_WIDTH  (0x01)
    #define SPI_CMD_GET_DATA_WIDTH  (0x02)
    #define SPI_CMD_CLEAR_TX_FIFO   (0x03)
    #define SPI_CMD_CLEAR_RX_FIFO   (0x04)
    #define SPI_CMD_SET_CS_INTERVAL (0x05)
