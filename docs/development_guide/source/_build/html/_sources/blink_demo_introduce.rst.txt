流水灯示例
====================

硬件连接
-----------------------------

.. figure:: img/blink_breath_sch.png
    :alt:

    **BL706_IOT板支持上述电路**

-  硬件连接如上图所示 ``GPIO29`` ``GPIO30`` ``GPIO31`` ``GPIO22`` 分别控制四路 LED 灯



软件实现
-----------------------------

-  软件代码见 ``examples/gpio/gpio_blink``

.. code-block:: C
    :linenos:

    gpio_set_mode(GPIO_PIN_22, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_29, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_30, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_31, GPIO_OUTPUT_PP_MODE);

-  使用上述代码将 ``GPIO22`` ``GPIO29`` ``GPIO30`` ``GPIO31`` 配置为输出模式。

.. code-block:: C
    :linenos:

    gpio_write(GPIO_PIN_22, 0);

-  使用上述代码修改输出的电平值。


最终效果
-----------------------------
见视频展示
