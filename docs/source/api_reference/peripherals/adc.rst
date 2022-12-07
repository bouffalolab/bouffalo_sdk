ADC
=============

Macros
------------

adc channel
^^^^^^^^^^^^^^^^^^^^

参考 `bflb_adc.h` 文件，不详细列出。

adc clock div
^^^^^^^^^^^^^^^^^^^^

对 adc 时钟再一次进行分频。分频后的时钟必须小于等于 500K。ADC CLK = CLK_SOURCE/CLK_DIV/adc_clk_div。

.. code-block:: c
   :linenos:

    #define ADC_CLK_DIV_4  1
    #define ADC_CLK_DIV_8  2
    #define ADC_CLK_DIV_12 3
    #define ADC_CLK_DIV_16 4
    #define ADC_CLK_DIV_20 5
    #define ADC_CLK_DIV_24 6
    #define ADC_CLK_DIV_32 7

adc resolution
^^^^^^^^^^^^^^^^^^^^

adc 位数，可以选择 12B、14B、16B。其中 14B 和 16B 自带过采样处理。

.. code-block:: c
   :linenos:

    #define ADC_RESOLUTION_12B 0
    #define ADC_RESOLUTION_14B 2
    #define ADC_RESOLUTION_16B 4

adc vref
^^^^^^^^^^^^^^^^^^^^

adc 内置参考电压选择，可以选择 2.0 V 或者 3.2V。

.. code-block:: c
   :linenos:

    #define ADC_VREF_3P2V 0
    #define ADC_VREF_2P0V 1

Structs
------------

struct bflb_adc_config_s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

adc 初始化配置结构体。

.. code-block:: c
   :linenos:

    struct bflb_adc_config_s {
        uint8_t clk_div;
        uint8_t scan_conv_mode;
        uint8_t continuous_conv_mode;
        uint8_t differential_mode;
        uint8_t resolution;
        uint8_t vref;
    };

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - clk_div
      - 分频值
    * - scan_conv_mode
      - 是否开启扫描模式
    * - continuous_conv_mode
      - 是否开启连续转换模式
    * - differential_mode
      - 是否开启差分模式
    * - resolution
      - 采样位数
    * - vref
      - 参考电压选择

struct bflb_adc_channel_s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

配置 adc 通道时使用。

.. code-block:: c
   :linenos:

    struct bflb_adc_channel_s {
        uint8_t pos_chan;
        uint8_t neg_chan;
    };

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - pos_chan
      - 正向通道
    * - neg_chan
      - 反向通道（单端模式下无用）

struct bflb_adc_result_s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

adc 标准转换结果

.. code-block:: c
   :linenos:

    struct bflb_adc_result_s {
      int8_t pos_chan;
      int8_t neg_chan;
      int32_t value;
      int32_t millivolt;
    };

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - pos_chan
      - 正向通道
    * - neg_chan
      - 反向通道（单端模式下无用）
    * - value
      - adc 转换结果
    * - millivolt
      - 转换结果转 mv

Functions
------------

bflb_adc_init
^^^^^^^^^^^^^^^^^^^^

说明: 初始化 adc。adc 使用之前需要开启 adc ip 时钟、设置 adc 时钟源和分频值、选择使用的 gpio 为 analog 模式。

.. code-block:: c
   :linenos:

    void bflb_adc_init(struct bflb_device_s *dev, const struct bflb_adc_config_s *config);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - config
      - 配置项

bflb_adc_deinit
^^^^^^^^^^^^^^^^^^^^

说明: 反初始化 adc。

.. code-block:: c
   :linenos:

    void bflb_adc_deinit(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_adc_link_rxdma
^^^^^^^^^^^^^^^^^^^^

说明: adc dma 功能开关。

.. code-block:: c
   :linenos:

    void bflb_adc_link_rxdma(struct bflb_device_s *dev, bool enable);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - enable
      - 是否开启 dma 功能

bflb_adc_channel_config
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 配置 adc 通道。

.. code-block:: c
   :linenos:

    int bflb_adc_channel_config(struct bflb_device_s *dev, struct bflb_adc_channel_s *chan, uint8_t channels);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - chan
      - 通道（一对）
    * - channels
      - 通道对数（单次扫描模式下只能为 1）

bflb_adc_start_conversion
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 启动 adc 转换。连续转换模式下只需要调用一次。

.. code-block:: c
   :linenos:

    void bflb_adc_start_conversion(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_adc_stop_conversion
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 停止 adc 转换。

.. code-block:: c
   :linenos:

    void bflb_adc_stop_conversion(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_adc_get_count
^^^^^^^^^^^^^^^^^^^^

说明: 获取 adc 转换个数。

.. code-block:: c
   :linenos:

    uint8_t bflb_adc_get_count(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 转换个数（最大为32）

bflb_adc_read_raw
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 读取一次 adc 转换值。

.. code-block:: c
   :linenos:

    uint32_t bflb_adc_read_raw(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 转换值（注意不是最终采样值）

bflb_adc_rxint_mask
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: adc 转换完成中断开关。

.. code-block:: c
   :linenos:

    void bflb_adc_rxint_mask(struct bflb_device_s *dev, bool mask);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - mask
      - 是否屏蔽中断

bflb_adc_errint_mask
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: adc 错误中断开关。

.. code-block:: c
   :linenos:

    void bflb_adc_errint_mask(struct bflb_device_s *dev, bool mask);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - mask
      - 是否屏蔽中断

bflb_adc_get_intstatus
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: adc 中断标志。

.. code-block:: c
   :linenos:

   uint32_t bflb_adc_get_intstatus(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 中断标志

返回值如下:

.. code-block:: c
   :linenos:

    ADC_INTSTS_NEG_SATURATION
    ADC_INTSTS_NEG_SATURATION
    ADC_INTSTS_FIFO_UNDERRUN
    ADC_INTSTS_FIFO_OVERRUN
    ADC_INTSTS_ADC_READY

bflb_adc_int_clear
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 清除 adc 中断标志。

.. code-block:: c
   :linenos:

    void bflb_adc_int_clear(struct bflb_device_s *dev, uint32_t int_clear);

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

    ADC_INTCLR_NEG_SATURATION
    ADC_INTCLR_POS_SATURATION
    ADC_INTCLR_FIFO_UNDERRUN
    ADC_INTCLR_FIFO_OVERRUN
    ADC_INTCLR_ADC_READY

bflb_adc_parse_result
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 对 adc 转换结果进行解析。

.. code-block:: c
   :linenos:

    void bflb_adc_parse_result(struct bflb_device_s *dev, uint32_t *buffer, struct bflb_adc_result_s *result, uint16_t count);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - buffer
      - 转换值
    * - result
      - 输出结果
    * - count
      - 转换个数

bflb_adc_tsen_init
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 初始化 adc tsen 模块。

.. code-block:: c
   :linenos:

   void bflb_adc_tsen_init(struct bflb_device_s *dev, uint8_t tsen_mod);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - tsen_mod
      - 模式选择

bflb_adc_tsen_get_temp
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 初始化 adc tsen 模块。

.. code-block:: c
   :linenos:

   float bflb_adc_tsen_get_temp(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 温度

bflb_adc_vbat_enable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 开启 vbat 。

.. code-block:: c
   :linenos:

   void bflb_adc_vbat_enable(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_adc_vbat_disable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 关闭 vbat。

.. code-block:: c
   :linenos:

   void bflb_adc_vbat_disable(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
