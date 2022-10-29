I2C
=============

Macros
------------

无

Structs
------------

struct bflb_i2c_msg_s
^^^^^^^^^^^^^^^^^^^^^^^^^^

i2c 传输时需要填充的消息。

.. code-block:: c
   :linenos:

    struct bflb_i2c_msg_s {
        uint16_t addr;
        uint16_t flags;
        uint8_t *buffer;
        uint16_t length;
    };

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - addr
      - 设备地址（7 位或者 10 位）
    * - flags
      - 传输时附带标志
    * - buffer
      - 数据区
    * - length
      - 数据长度

`flag` 可以为以下参数:

.. code-block:: c
   :linenos:

   #define I2C_M_READ    0x0001
   #define I2C_M_TEN     0x0002
   #define I2C_M_DMA     0x0004
   #define I2C_M_NOSTOP  0x0040
   #define I2C_M_NOSTART 0x0080

.. note:: I2C_M_NOSTOP 表示 i2c 设备需要操作寄存器地址

Functions
------------

bflb_i2c_init
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 初始化 i2c 并配置频率。

.. code-block:: c
   :linenos:

    void bflb_i2c_init(struct bflb_device_s *dev, uint32_t frequency);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - frequency
      - 配置频率（范围 305 HZ ~ 400KHZ）

bflb_i2c_deinit
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 反初始化 i2c。

.. code-block:: c
   :linenos:

    void bflb_i2c_deinit(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_i2c_link_txdma
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: i2c 发送 dma 使能开关。

.. code-block:: c
   :linenos:

    void bflb_i2c_link_txdma(struct bflb_device_s *dev, bool enable);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - enable
      - 是否使能 dma

bflb_i2c_link_rxdma
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: i2c 接收 dma 使能开关。

.. code-block:: c
   :linenos:

    void bflb_i2c_link_rxdma(struct bflb_device_s *dev, bool enable);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - enable
      - 是否使能 dma

bflb_i2c_transfer
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: i2c 消息传输。

.. code-block:: c
   :linenos:

    int bflb_i2c_transfer(struct bflb_device_s *dev, struct bflb_i2c_msg_s *msgs, int count);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - msgs
      - 消息指针
    * - count
      - 消息个数
