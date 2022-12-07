PWM_v1
=============

.. note:: PWM V1 版本每个 PWM 的所有通道频率可以单独设置。

Macros
------------

pwm channel
^^^^^^^^^^^^^^

PWM V1 版本共 5 个 pwm 通道。（BL702L 只有通道 0）。

Structs
------------

struct bflb_pwm_v1_channel_config_s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

pwm v1 初始化配置结构体。

.. code-block:: c
   :linenos:

    struct bflb_pwm_v1_channel_config_s {
        uint8_t clk_source;
        uint16_t clk_div;
        uint16_t period;
    };

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - clk_source
      - 时钟源选择：PBCLK or XCLK or 32K_CLK
    * - clk_div
      - 分频值
    * - period
      - 周期值

.. note:: PWM 最终产生的频率 = clk_source/clk_div/period

Functions
------------

bflb_pwm_v1_channel_init
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 初始化 pwm 通道。使用之前需要选择 gpio 为 pwm 功能。

.. code-block:: c
   :linenos:

   void bflb_pwm_v1_channel_init(struct bflb_device_s *dev, uint8_t ch, const struct bflb_pwm_v1_channel_config_s *config);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道号
    * - config
      - 配置项

bflb_pwm_v1_channel_deinit
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 复位 pwm 通道。

.. code-block:: c
   :linenos:

   void bflb_pwm_v1_channel_deinit(struct bflb_device_s *dev, uint8_t ch);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道号

bflb_pwm_v1_start
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 启动 pwm 通道输出。

.. code-block:: c
   :linenos:

   void bflb_pwm_v1_start(struct bflb_device_s *dev, uint8_t ch);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道号

bflb_pwm_v1_stop
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 关闭 pwm 通道输出。

.. code-block:: c
   :linenos:

   void bflb_pwm_v1_stop(struct bflb_device_s *dev, uint8_t ch);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道号

bflb_pwm_v1_set_period
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 修改 pwm 通道周期值，从而更改 pwm 通道输出的频率。

.. code-block:: c
   :linenos:

   void bflb_pwm_v1_set_period(struct bflb_device_s *dev, uint8_t ch, uint16_t period);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道号
    * - period
      - 周期值

bflb_pwm_v1_channel_set_threshold
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 设置 pwm 占空比。

.. code-block:: c
   :linenos:

   void bflb_pwm_v1_channel_set_threshold(struct bflb_device_s *dev, uint8_t ch, uint16_t low_threhold, uint16_t high_threhold);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道号
    * - low_threhold
      - 低阈值
    * - high_threhold
      - 高阈值,需要大于 low_threhold，并且小于等于 period

.. note:: PWM 占空比 = (high_threhold - low_threhold)/period