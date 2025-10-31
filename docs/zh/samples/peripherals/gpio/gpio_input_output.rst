GPIO - input/output
====================

本 demo 主要介绍 GPIO 0 输出和 GPIO 1 输入功能。

硬件连接
-----------------------------

使用杜邦线将 GPIO 0 和 GPIO 1 引脚连接。

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/gpio/gpio_input_output**

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中开启时钟

.. code-block:: C
    :linenos:

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);

- 配置 GPIO 0 为 GPIO_OUTPUT 功能，GPIO 1 为 GPIO_INPUT 功能。

.. code-block:: C
    :linenos:

    while (1) {
        bflb_gpio_set(gpio, GPIO_PIN_0);
        printf("GPIO_PIN_1=%x\r\n", bflb_gpio_read(gpio, GPIO_PIN_1));
        bflb_mtimer_delay_ms(2000);

        bflb_gpio_reset(gpio, GPIO_PIN_0);
        printf("GPIO_PIN_1=%x\r\n", bflb_gpio_read(gpio, GPIO_PIN_1));
        bflb_mtimer_delay_ms(2000);
    }

- ``bflb_gpio_set(gpio, GPIO_PIN_0)`` 将 GPIO 0 引脚置位
- ``bflb_gpio_read(gpio, GPIO_PIN_1)`` 读取 GPIO 1 引脚电平
- ``bflb_gpio_reset(gpio, GPIO_PIN_0)`` 将 GPIO 0 引脚置 0

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------

打印 GPIO 1 引脚电平。
