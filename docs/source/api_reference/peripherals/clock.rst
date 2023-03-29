CLOCK
=============

Macros
------------

无

Structs
------------

无

Functions
------------

bflb_clk_get_system_clock
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取系统时钟频率。

.. code-block:: c
   :linenos:

    uint32_t bflb_clk_get_system_clock(uint8_t type);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - type
      - 外设类型

**type** 可以填入以下参数：

.. code-block:: c
   :linenos:

   #define BFLB_SYSTEM_ROOT_CLOCK 0
   #define BFLB_SYSTEM_CPU_CLK    1
   #define BFLB_SYSTEM_PBCLK      2
   #define BFLB_SYSTEM_XCLK       3
   #define BFLB_SYSTEM_32K_CLK    4

bflb_clk_get_peripheral_clock
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取外设时钟频率。

.. code-block:: c
   :linenos:

    uint32_t bflb_clk_get_peripheral_clock(uint8_t type, uint8_t idx);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - type
      - 外设类型
    * - idx
      - 外设 id

**type** 可以填入以下参数：

.. code-block:: c
   :linenos:

   #define BFLB_DEVICE_TYPE_ADC      0
   #define BFLB_DEVICE_TYPE_DAC      1
   #define BFLB_DEVICE_TYPE_AUDIOADC 2
   #define BFLB_DEVICE_TYPE_AUDIODAC 3
   #define BFLB_DEVICE_TYPE_GPIO     4
   #define BFLB_DEVICE_TYPE_UART     5
   #define BFLB_DEVICE_TYPE_SPI      6
   #define BFLB_DEVICE_TYPE_I2C      7
   #define BFLB_DEVICE_TYPE_DMA      8
   #define BFLB_DEVICE_TYPE_I2S      9
   #define BFLB_DEVICE_TYPE_IR       10
   #define BFLB_DEVICE_TYPE_TIMER    11
   #define BFLB_DEVICE_TYPE_PWM      12
   #define BFLB_DEVICE_TYPE_CAMERA   14
   #define BFLB_DEVICE_TYPE_FLASH    15
   #define BFLB_DEVICE_TYPE_QSPI     16
   #define BFLB_DEVICE_TYPE_SDH      17
   #define BFLB_DEVICE_TYPE_SDU      18
   #define BFLB_DEVICE_TYPE_ETH      19
   #define BFLB_DEVICE_TYPE_RTC      20
   #define BFLB_DEVICE_TYPE_CRC      21
   #define BFLB_DEVICE_TYPE_RNG      22
   #define BFLB_DEVICE_TYPE_MIPI     23
   #define BFLB_DEVICE_TYPE_DPI      24
   #define BFLB_DEVICE_TYPE_DSI      25
   #define BFLB_DEVICE_TYPE_CSI      26
   #define BFLB_DEVICE_TYPE_USB      27
   #define BFLB_DEVICE_TYPE_AES      28
   #define BFLB_DEVICE_TYPE_SHA      29
   #define BFLB_DEVICE_TYPE_MD5      30
   #define BFLB_DEVICE_TYPE_TRNG     31
   #define BFLB_DEVICE_TYPE_PKA      32
   #define BFLB_DEVICE_TYPE_CKS      33
   #define BFLB_DEVICE_TYPE_MJPEG    34

