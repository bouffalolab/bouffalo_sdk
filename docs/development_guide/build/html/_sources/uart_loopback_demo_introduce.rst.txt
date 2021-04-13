串口自收发示例
====================

硬件连接
-----------------------------

.. figure:: img/uart_loop_sch.png
    :alt:

    **BL706_IOT板支持上述电路**

-  硬件连接如上图所示 板子将芯片对应的 RX 与 TX 脚引到了排针 ``HD1``。
-  将对应的 TX 接上串口转换芯片的 RX，RX 接上串口转换芯片的 TX，将串口转换芯片连接电脑。



软件实现
-----------------------------

-  软件代码见 ``examples/uart/uart_echo``

-  ``pwm`` 的输出引脚由板级描述文件 ``bsp/board/bl706_iot/pinmux_config.h`` 来配置

.. code-block:: C
    :linenos:

    #define CONFIG_GPIO14_FUNC GPIO_FUN_UART0_TX
    #define CONFIG_GPIO15_FUNC GPIO_FUN_UART0_RX
 
-  可以看到，我们将 ``GPIO_PIN_14`` 与 ``GPIO_PIN_15`` 的功能选择了 ``UART`` 功能。

-  不同的电路板会存在不同的引脚配置，我们会使用向导来配置不同应用场景下的引脚配置，用户也可以手动修改。


.. code-block:: C
    :linenos:

    bflb_platform_init();

-  在 ``bflb_platform_init`` 函数中，我们已经注册并且打开了一个调试用的串口设备，给用户实现一个 ``printf`` 的基本功能用作打印输出报文。

.. code-block:: C
    :linenos:

    struct device *uart = device_find("debug_log");

-  我们通过 ``find`` 函数，找到在 ``bflb_platform_init`` 函数中注册并且打开的串口设备句柄，储存于 ``uart`` 变量中。

.. code-block:: C
    :linenos:

    if (uart)
    {
        device_set_callback(uart, uart_irq_callback);
        device_control(uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT|UART_RTO_IT));
    }

-  通过 ``set_callback`` 函数，注册用户指定的中断服务函数。通过 ``device_control`` 函数打开中断使能开关

.. code-block:: C
    :linenos:

    void uart_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
    {
        if (state == UART_EVENT_RX_FIFO)
        {
            device_write(dev,0,(uint8_t *)args,size);
        }
        else if (state == UART_EVENT_RTO)
        {
            device_write(dev,0,(uint8_t *)args,size);
        }
    }

    
-  此函数是示例的中断服务函数，作用是将接受到的数据原封不动的发送出去。
    
    - ``state`` 会返回 ``UART`` 设备的中断类型
    - ``args`` 包含了返回数据指针
    - ``size`` 包含返回数据的长度
    - ``dev`` 为中断的 ``uart`` 设备句柄

-  当中断产生，将会调用 ``device_write`` 函数将接受到的数据发送回去。



最终效果
-----------------------------
见视频讲解


