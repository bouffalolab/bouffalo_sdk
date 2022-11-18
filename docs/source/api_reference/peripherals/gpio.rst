GPIO
=============

Macros
------------

gpio pin
^^^^^^^^^^

参考 `bflb_gpio.h` 文件，不详细列出。

.. warning:: 相同系列芯片引脚数会有区别，注意区分

gpio mode
^^^^^^^^^^

gpio 模式可以配置成输入、输出、模拟、复用。

.. code-block:: c
   :linenos:

    #define GPIO_INPUT      (0 << GPIO_MODE_SHIFT) /* Input Enable */
    #define GPIO_OUTPUT     (1 << GPIO_MODE_SHIFT) /* Output Enable */
    #define GPIO_ANALOG     (2 << GPIO_MODE_SHIFT) /* Analog Enable */
    #define GPIO_ALTERNATE  (3 << GPIO_MODE_SHIFT) /* Alternate Enable */

gpio function
^^^^^^^^^^^^^^^

参考 `bflb_gpio.h` 文件，不详细列出。

gpio pupd
^^^^^^^^^^^^^^^

gpio 上下拉可以选择上拉、下拉、浮空。

.. code-block:: c
   :linenos:

    #define GPIO_FLOAT      (0 << GPIO_PUPD_SHIFT) /* No pull-up, pull-down */
    #define GPIO_PULLUP     (1 << GPIO_PUPD_SHIFT) /* Pull-up */
    #define GPIO_PULLDOWN   (2 << GPIO_PUPD_SHIFT) /* Pull-down */

gpio smt
^^^^^^^^^^^^^^^

gpio 滤波开关。

.. code-block:: c
   :linenos:

    #define GPIO_SMT_DIS   (0 << GPIO_SMT_SHIFT)
    #define GPIO_SMT_EN    (1 << GPIO_SMT_SHIFT)

gpio drive
^^^^^^^^^^^^^^^

gpio 输出能力选择。

.. code-block:: c
   :linenos:

    #define GPIO_DRV_0     (0 << GPIO_DRV_SHIFT)
    #define GPIO_DRV_1     (1 << GPIO_DRV_SHIFT)
    #define GPIO_DRV_2     (2 << GPIO_DRV_SHIFT)
    #define GPIO_DRV_3     (3 << GPIO_DRV_SHIFT)

gpio int trig mode
^^^^^^^^^^^^^^^^^^^^^

gpio 外部中断触发模式。

.. code-block:: c
   :linenos:

    #define GPIO_INT_TRIG_MODE_SYNC_FALLING_EDGE 0
    #define GPIO_INT_TRIG_MODE_SYNC_RISING_EDGE  1
    #define GPIO_INT_TRIG_MODE_SYNC_LOW_LEVEL    2
    #define GPIO_INT_TRIG_MODE_SYNC_HIGH_LEVEL   3
    #if defined(BL702)
    #define GPIO_INT_TRIG_MODE_ASYNC_FALLING_EDGE 4
    #define GPIO_INT_TRIG_MODE_ASYNC_RISING_EDGE  5
    #define GPIO_INT_TRIG_MODE_ASYNC_LOW_LEVEL    6
    #define GPIO_INT_TRIG_MODE_ASYNC_HIGH_LEVEL   7
    #else
    #define GPIO_INT_TRIG_MODE_SYNC_FALLING_RISING_EDGE 4
    #define GPIO_INT_TRIG_MODE_ASYNC_FALLING_EDGE       8
    #define GPIO_INT_TRIG_MODE_ASYNC_RISING_EDGE        9
    #define GPIO_INT_TRIG_MODE_ASYNC_LOW_LEVEL          10
    #define GPIO_INT_TRIG_MODE_ASYNC_HIGH_LEVEL         11
    #endif

.. warning:: 602、702 不支持双边沿

gpio uart function
^^^^^^^^^^^^^^^^^^^^^

博流系列芯片，每个 gpio 可以选择到 UART 的任意一个功能。

.. code-block:: c
   :linenos:

    #define GPIO_UART_FUNC_UART0_RTS 0
    #define GPIO_UART_FUNC_UART0_CTS 1
    #define GPIO_UART_FUNC_UART0_TX  2
    #define GPIO_UART_FUNC_UART0_RX  3
    #define GPIO_UART_FUNC_UART1_RTS 4
    #define GPIO_UART_FUNC_UART1_CTS 5
    #define GPIO_UART_FUNC_UART1_TX  6
    #define GPIO_UART_FUNC_UART1_RX  7
    #if defined(BL808) || defined(BL606P)
    #define GPIO_UART_FUNC_UART2_RTS 8
    #define GPIO_UART_FUNC_UART2_CTS 9
    #define GPIO_UART_FUNC_UART2_TX  10
    #define GPIO_UART_FUNC_UART2_RX  11
    #endif

Structs
------------

无

Functions
------------

bflb_gpio_init
^^^^^^^^^^^^^^^^^^^^

说明: 初始化 gpio。如果配置为 UART 功能，请使用 `bflb_gpio_uart_init` 。

.. code-block:: c
   :linenos:

    void bflb_gpio_init(struct bflb_device_s *dev, uint8_t pin, uint32_t cfgset);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - pin
      - gpio pin
    * - cfgset
      - gpio 配置项：gpio mode、gpio function、gpio pupd、gpio smt、gpio drive，多个配置需要使用 | 连接

bflb_gpio_deinit
^^^^^^^^^^^^^^^^^^^^

说明: 反初始化 gpio，默认为输入浮空状态

.. code-block:: c
   :linenos:

    void bflb_gpio_deinit(struct bflb_device_s *dev, uint8_t pin);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - pin
      - gpio pin

bflb_gpio_set
^^^^^^^^^^^^^^^^^^^^

说明: gpio 输出高电平

.. code-block:: c
   :linenos:

    void bflb_gpio_set(struct bflb_device_s *dev, uint8_t pin);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - pin
      - gpio pin

bflb_gpio_reset
^^^^^^^^^^^^^^^^^^^^

说明: gpio 输出低电平

.. code-block:: c
   :linenos:

    void bflb_gpio_reset(struct bflb_device_s *dev, uint8_t pin);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - pin
      - gpio pin

bflb_gpio_read
^^^^^^^^^^^^^^^^^^^^

说明: 读取 gpio 电平

.. code-block:: c
   :linenos:

    bool bflb_gpio_read(struct bflb_device_s *dev, uint8_t pin);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - pin
      - gpio pin
    * - return
      - true 为 高电平，false 为低电平

bflb_gpio_int_init
^^^^^^^^^^^^^^^^^^^^

说明: gpio 外部中断初始化

.. code-block:: c
   :linenos:

    void bflb_gpio_int_init(struct bflb_device_s *dev, uint8_t pin, uint8_t trig_mode);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - pin
      - gpio pin
    * - trig_mode
      - 中断触发模式

bflb_gpio_int_mask
^^^^^^^^^^^^^^^^^^^^

说明: gpio 外部中断屏蔽开关

.. code-block:: c
   :linenos:

    void bflb_gpio_int_mask(struct bflb_device_s *dev, uint8_t pin, bool mask);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - pin
      - gpio pin
    * - mask
      - 是否屏蔽中断

bflb_gpio_get_intstatus
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取 gpio 外部中断是否触发的标志

.. code-block:: c
   :linenos:

    bool bflb_gpio_get_intstatus(struct bflb_device_s *dev, uint8_t pin);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - pin
      - gpio pin
    * - return
      - 为 true 表示触发

bflb_gpio_int_clear
^^^^^^^^^^^^^^^^^^^^^^^^

说明: 清除 gpio 中断标志

.. code-block:: c
   :linenos:

    void bflb_gpio_int_clear(struct bflb_device_s *dev, uint8_t pin);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - pin
      - gpio pin

bflb_gpio_uart_init
^^^^^^^^^^^^^^^^^^^^^^^

说明: gpio 配置成 uart 的某一个功能。

.. code-block:: c
   :linenos:

    void bflb_gpio_uart_init(struct bflb_device_s *dev, uint8_t pin, uint8_t uart_func);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - pin
      - gpio pin
    * - uart_func
      - uart 具体某一个功能
