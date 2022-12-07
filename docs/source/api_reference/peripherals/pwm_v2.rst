PWM_v2
=============

.. note:: PWM V2 版本每个 PWM 的所有通道共用一个频率。

Macros
------------

pwm channel
^^^^^^^^^^^^^^

PWM V2 版本每个 PWM 共 4 个 pwm 通道。

pwm polarity
^^^^^^^^^^^^^^

PWM V2 版本输出有效极性。当正向通道阈值位于设置的低阈值和高阈值之间，为有效极性，如果设置有效极性为高，则输出高电平，反之输出低电平。反向通道相反，阈值位于设置的低阈值和高阈值之外，为有效极性，如果设置有效极性为高，则输出高电平，反之输出低电平。

.. code-block:: c
   :linenos:

   #define PWM_POLARITY_ACTIVE_LOW  0
   #define PWM_POLARITY_ACTIVE_HIGH 1

Structs
------------

struct bflb_pwm_v2_config_s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

pwm v2 初始化配置结构体。

.. code-block:: c
   :linenos:

    struct bflb_pwm_v2_config_s {
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

struct bflb_pwm_v2_channel_config_s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

pwm v2 初始化配置结构体。

.. code-block:: c
   :linenos:

    struct bflb_pwm_v2_channel_config_s {
        uint8_t positive_polarity;
        uint8_t negative_polarity;
        uint8_t positive_stop_state;
        uint8_t negative_stop_state;
        uint8_t positive_brake_state;
        uint8_t negative_brake_state;
        uint8_t dead_time;
    };

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - positive_polarity
      - 正向输出极性
    * - negative_polarity
      - 反向输出极性
    * - positive_stop_state
      - 正向输出空闲状态
    * - negative_stop_state
      - 反向输出空闲状态
    * - positive_brake_state
      - 正向输出刹车状态
    * - negative_brake_state
      - 反向输出刹车状态
    * - dead_time
      - 死区时间

Functions
------------

bflb_pwm_v2_init
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 初始化 pwm 。使用之前需要选择 gpio 为 pwm 功能。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_init(struct bflb_device_s *dev, const struct bflb_pwm_v2_config_s *config);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - config
      - 配置项

bflb_pwm_v2_deinit
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 复位 pwm 。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_deinit(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_pwm_v2_start
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 启动 pwm 输出。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_start(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_pwm_v2_stop
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 关闭 pwm 输出。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_stop(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_pwm_v2_set_period
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 修改 pwm 周期值，从而更改 pwm 输出的频率。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_set_period(struct bflb_device_s *dev, uint16_t period);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - period
      - 周期值

bflb_pwm_v2_channel_init
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: PWM 通道初始化。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_channel_init(struct bflb_device_s *dev, uint8_t ch, struct bflb_pwm_v2_channel_config_s *config);

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
      - 通道配置

bflb_pwm_v2_channel_set_threshold
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 设置 PWM 占空比。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_channel_set_threshold(struct bflb_device_s *dev, uint8_t ch, uint16_t low_threhold, uint16_t high_threhold);

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

bflb_pwm_v2_channel_positive_start
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: PWM 正向通道使能输出。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_channel_positive_start(struct bflb_device_s *dev, uint8_t ch);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道号

bflb_pwm_v2_channel_negative_start
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: PWM 反向通道使能输出。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_channel_negative_start(struct bflb_device_s *dev, uint8_t ch);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道号

bflb_pwm_v2_channel_positive_stop
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: PWM 正向通道停止输出。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_channel_positive_stop(struct bflb_device_s *dev, uint8_t ch);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道号

bflb_pwm_v2_channel_negative_stop
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: PWM 反向通道停止输出。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_channel_negative_stop(struct bflb_device_s *dev, uint8_t ch);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - ch
      - 通道号

bflb_pwm_v2_int_enable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: PWM 中断使能和关闭。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_int_enable(struct bflb_device_s *dev, uint32_t int_en, bool enable);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - int_en
      - 中断使能位
    * - enable
      - 是否开启中断

`int_en` 可以填入以下值，多个中断可以使用 `|` 连接：

.. code-block:: c
   :linenos:

   #define PWM_INTEN_CH0_L  (1 << 0)
   #define PWM_INTEN_CH0_H  (1 << 1)
   #define PWM_INTEN_CH1_L  (1 << 2)
   #define PWM_INTEN_CH1_H  (1 << 3)
   #define PWM_INTEN_CH2_L  (1 << 4)
   #define PWM_INTEN_CH2_H  (1 << 5)
   #define PWM_INTEN_CH3_L  (1 << 6)
   #define PWM_INTEN_CH3_H  (1 << 7)
   #define PWM_INTEN_PERIOD (1 << 8)
   #define PWM_INTEN_BRAKE  (1 << 9)
   #define PWM_INTEN_REPT   (1 << 10)

bflb_pwm_v2_get_intstatus
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取 PWM 中断标志。

.. code-block:: c
   :linenos:

   uint32_t bflb_pwm_v2_get_intstatus(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 返回中断标志

返回值如下：

.. code-block:: c
   :linenos:

   #define PWM_INTSTS_CH0_L  (1 << 0)
   #define PWM_INTSTS_CH0_H  (1 << 1)
   #define PWM_INTSTS_CH1_L  (1 << 2)
   #define PWM_INTSTS_CH1_H  (1 << 3)
   #define PWM_INTSTS_CH2_L  (1 << 4)
   #define PWM_INTSTS_CH2_H  (1 << 5)
   #define PWM_INTSTS_CH3_L  (1 << 6)
   #define PWM_INTSTS_CH3_H  (1 << 7)
   #define PWM_INTSTS_PERIOD (1 << 8)
   #define PWM_INTSTS_BRAKE  (1 << 9)
   #define PWM_INTSTS_REPT   (1 << 10)

bflb_pwm_v2_int_clear
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 清除 PWM 中断标志。

.. code-block:: c
   :linenos:

   void bflb_pwm_v2_int_clear(struct bflb_device_s *dev, uint32_t int_clear);

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

   #define PWM_INTCLR_CH0_L  (1 << 0)
   #define PWM_INTCLR_CH0_H  (1 << 1)
   #define PWM_INTCLR_CH1_L  (1 << 2)
   #define PWM_INTCLR_CH1_H  (1 << 3)
   #define PWM_INTCLR_CH2_L  (1 << 4)
   #define PWM_INTCLR_CH2_H  (1 << 5)
   #define PWM_INTCLR_CH3_L  (1 << 6)
   #define PWM_INTCLR_CH3_H  (1 << 7)
   #define PWM_INTCLR_PERIOD (1 << 8)
   #define PWM_INTCLR_BRAKE  (1 << 9)
   #define PWM_INTCLR_REPT   (1 << 10)

bflb_pwm_v2_feature_control
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明:  PWM 其他特性相关控制，一般不常用。

.. code-block:: c
   :linenos:

    int bflb_pwm_v2_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

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

   #define PWM_CMD_SET_TRIG_ADC_SRC       (0x01)
   #define PWM_CMD_SET_EXT_BRAKE_POLARITY (0x02)
   #define PWM_CMD_SET_EXT_BRAKE_ENABLE   (0x03)
   #define PWM_CMD_SET_SW_BRAKE_ENABLE    (0x04)
   #define PWM_CMD_SET_STOP_ON_REPT       (0x05)
   #define PWM_CMD_SET_REPT_COUNT         (0x06)