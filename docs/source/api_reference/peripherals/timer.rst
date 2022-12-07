TIMER
=============

Macros
------------

timer clock source
^^^^^^^^^^^^^^^^^^^^^^

定时器输入时钟源可以选择以下类型，注意： `TIMER_CLKSRC_GPIO` BL602/BL702 系列没有此功能。

.. code-block:: c
   :linenos:

   #define TIMER_CLKSRC_BCLK 0
   #define TIMER_CLKSRC_32K  1
   #define TIMER_CLKSRC_1K   2
   #define TIMER_CLKSRC_XTAL 3
   #define TIMER_CLKSRC_GPIO 4
   #define TIMER_CLKSRC_NO   5

timer counter mode
^^^^^^^^^^^^^^^^^^^^^^

定时器计数模式分为两种: freerun(向上计数模式)、preload(重装载模式)。

.. code-block:: c
   :linenos:

    #define TIMER_COUNTER_MODE_PROLOAD 0
    #define TIMER_COUNTER_MODE_UP      1

timer compare id
^^^^^^^^^^^^^^^^^^^^^^

定时器一共三个 compare id, 用于设置不同的定时时间，可以当三个定时器使用。

.. code-block:: c
   :linenos:

    #define TIMER_COMP_ID_0 0
    #define TIMER_COMP_ID_1 1
    #define TIMER_COMP_ID_2 2

Structs
------------

struct bflb_timer_config_s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

timer 初始化配置结构体。

.. code-block:: c
   :linenos:

    struct bflb_timer_config_s {
        uint8_t counter_mode;
        uint8_t clock_source;
        uint8_t clock_div;
        uint8_t trigger_comp_id;
        uint32_t comp0_val;
        uint32_t comp1_val;
        uint32_t comp2_val;
        uint32_t preload_val;
    };

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - counter_mode
      - 计数模式
    * - clock_source
      - 时钟源选择
    * - clock_div
      - 分频值
    * - trigger_comp_id
      - 选择触发的最高 comp id(选择越高,则能够定时的个数越多)
    * - comp0_val
      - comp0 比较值
    * - comp1_val
      - comp1 比较值(需要大于 comp0_val)
    * - comp2_val
      - comp2 比较值(需要大于 comp1_val)
    * - preload_val
      - 重装载值

Functions
------------

bflb_timer_init
^^^^^^^^^^^^^^^^^^^^

说明: 初始化 timer。使用之前需要开启 timer ip 时钟。

.. code-block:: c
   :linenos:

    void bflb_timer_init(struct bflb_device_s *dev, const struct bflb_timer_config_s *config);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - config
      - 配置项

bflb_timer_deinit
^^^^^^^^^^^^^^^^^^^^

说明: 反初始化 timer。

.. code-block:: c
   :linenos:

    void bflb_timer_deinit(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_timer_start
^^^^^^^^^^^^^^^^^^^^

说明: 启动 timer 。

.. code-block:: c
   :linenos:

    void bflb_timer_start(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_timer_stop
^^^^^^^^^^^^^^^^^^^^

说明: 停止 timer。

.. code-block:: c
   :linenos:

    void bflb_timer_stop(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_timer_set_compvalue
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 设置 timer comp id 比较值。

.. code-block:: c
   :linenos:

    void bflb_timer_set_compvalue(struct bflb_device_s *dev, uint8_t cmp_no, uint32_t val);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - cmp_no
      - comp id
    * - val
      - 比较值

bflb_timer_get_compvalue
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取 comp id 比较值。

.. code-block:: c
   :linenos:

    uint32_t bflb_timer_get_compvalue(struct bflb_device_s *dev, uint8_t cmp_no);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - cmp_no
      - comp id
    * - return
      - 比较值

bflb_timer_get_countervalue
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取 timer 计数值。

.. code-block:: c
   :linenos:

    uint32_t bflb_timer_get_countervalue(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 计数值

bflb_timer_compint_mask
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: timer comp 中断屏蔽开关。

.. code-block:: c
   :linenos:

    void bflb_timer_compint_mask(struct bflb_device_s *dev, uint8_t cmp_no, bool mask);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - cmp_no
      - comp id
    * - mask
      - 是否屏蔽中断

bflb_timer_get_compint_status
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取 timer comp id 中断匹配标志。

.. code-block:: c
   :linenos:

    bool bflb_timer_get_compint_status(struct bflb_device_s *dev, uint8_t cmp_no);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - cmp_no
      - comp id
    * - return
      - 为 true 表示匹配


bflb_timer_compint_clear
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 清除 timer comp id 中断标志。

.. code-block:: c
   :linenos:

    void bflb_timer_compint_clear(struct bflb_device_s *dev, uint8_t cmp_no);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - cmp_no
      - comp id

