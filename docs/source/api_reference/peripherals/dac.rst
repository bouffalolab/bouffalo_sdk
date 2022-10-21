DAC
=============

Macros
------------

dac sampling frequence
^^^^^^^^^^^^^^^^^^^^^^^^^

dac 采样频率可以选择 32K、16K、8K，512K 只有 dma 模式下才可以使用。

.. code-block:: c
   :linenos:

    #define DAC_SAMPLING_FREQ_32K  0
    #define DAC_SAMPLING_FREQ_16K  1
    #define DAC_SAMPLING_FREQ_8K   3
    #define DAC_SAMPLING_FREQ_512K 4

dac channel
^^^^^^^^^^^^^^^^^^^^^^^^^

博流系列芯片 dac 通道共两组：A 通道和 B 通道。

.. code-block:: c
   :linenos:

    #define DAC_CHANNEL_A (1 << 0)
    #define DAC_CHANNEL_B (1 << 1)

Structs
------------

无

Functions
------------

bflb_dac_init
^^^^^^^^^^^^^^^^^^^^

说明: 初始化 dac。dac 使用之前需要开启 dac ip 时钟、设置 dac 时钟源和分频值、选择使用的 gpio 为 analog 模式。

.. code-block:: c
   :linenos:

    void bflb_dac_init(struct bflb_device_s *dev, uint8_t frequence);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - frequence
      - 采样率

bflb_dac_deinit
^^^^^^^^^^^^^^^^^^^^

说明: 反初始化 dac。

.. code-block:: c
   :linenos:

    void bflb_dac_deinit(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_dac_link_txdma
^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: dac dma 开关使能。

.. code-block:: c
   :linenos:

    void bflb_dac_link_txdma(struct bflb_device_s *dev, bool enable);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - enable
      - 是否使能 dma

bflb_dac_channel_enable
^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: dac 通道使能。

.. code-block:: c
   :linenos:

    void bflb_dac_channel_enable(struct bflb_device_s *dev, uint8_t ch);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道，多个通道之间使用 | 连接

bflb_dac_channel_disable
^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: dac 通道关闭。

.. code-block:: c
   :linenos:

    void bflb_dac_channel_disable(struct bflb_device_s *dev, uint8_t ch);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道，多个通道之间使用 | 连接

bflb_dac_set_value
^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 更改 dac 通道值。

.. code-block:: c
   :linenos:

    void bflb_dac_set_value(struct bflb_device_s *dev, uint8_t ch, uint16_t value);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道，多个通道之间使用 | 连接
    * - value
      - 通道值