GPIO - interrupt
====================

本 demo 主要介绍 GPIO 0 的同步低电平中断类型。

硬件连接
-----------------------------

将 GPIO 0 和 GND 连接。

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/gpio/gpio_interrupt**

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中开启时钟

.. code-block:: C
    :linenos:

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_int_init(gpio, GPIO_PIN_0, GPIO_INT_TRIG_MODE_SYNC_LOW_LEVEL);

- 设置 GPIO 0 的中断类型。

.. code-block:: C
    :linenos:

    bflb_gpio_int_mask(gpio, GPIO_PIN_0, false);

    bflb_irq_attach(gpio->irq_num, gpio_isr, gpio);
    bflb_irq_enable(gpio->irq_num);

- ``bflb_gpio_int_mask(gpio, GPIO_PIN_0, false)`` 打开 GPIO 0 中断
- ``bflb_irq_attach(gpio->irq_num, gpio_isr, gpio)`` 注册 GPIO 中断函数
- ``bflb_irq_enable(gpio->irq_num)`` 使能 GPIO 中断

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------

将 GPIO 0 引脚电平拉低，进入中断并打印中断次数。
