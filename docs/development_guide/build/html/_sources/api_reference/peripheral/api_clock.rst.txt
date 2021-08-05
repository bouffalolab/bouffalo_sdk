时钟树
=========================

简介
------------------------

博流系列芯片拥有丰富的时钟源选择，为方便用户配置，提供了时钟树配置表，不需要用户手动调用时钟设置接口，用户只需要关心最终的系统时钟和外设时钟频率即可。时钟配置表位于 ``bsp/board/xxx_board`` 目录下 ``clock_config.h`` 文件。


时钟频率获取接口
------------------------

**system_clock_get**
^^^^^^^^^^^^^^^^^^^^^^^^

``system_clock_get`` 用来获取系统时钟频率。

.. code-block:: C

    uint32_t system_clock_get(enum system_clock_type type);

- **type** 获取的系统时钟频率类型

``type`` 提供以下几种类型

.. code-block:: C

    enum system_clock_type
    {
        SYSTEM_CLOCK_ROOT_CLOCK = 0,
        SYSTEM_CLOCK_FCLK,
        SYSTEM_CLOCK_BCLK,
        SYSTEM_CLOCK_XCLK,
        SYSTEM_CLOCK_32K_CLK,
        SYSTEM_CLOCK_AUPLL,
    };


**peripheral_clock_get**
^^^^^^^^^^^^^^^^^^^^^^^^

``peripheral_clock_get`` 用来获取外设时钟频率。

.. code-block:: C

    uint32_t peripheral_clock_get(enum peripheral_clock_type type);

- **type** 获取的外设时钟频率类型

``type`` 提供以下几种类型

.. code-block:: C

    enum peripheral_clock_type
    {
        PERIPHERAL_CLOCK_UART = 0,
        PERIPHERAL_CLOCK_SPI,
        PERIPHERAL_CLOCK_I2C,
        PERIPHERAL_CLOCK_ADC,
        PERIPHERAL_CLOCK_DAC,
        PERIPHERAL_CLOCK_I2S,
        PERIPHERAL_CLOCK_PWM,
        PERIPHERAL_CLOCK_CAM,
    };