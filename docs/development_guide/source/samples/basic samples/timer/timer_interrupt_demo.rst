TIMER - 定时器中断
====================

本 demo 基于 TIMER 外设周期性触发中断模式编写。

软件实现
-----------------------------

-  软件代码见 ``examples/timer/timer_int``

.. code-block:: C
    :linenos:

    #define TIMER_CLK_SRC   (0)
    #define TIMER_CLK_DIV   (0)

-  配置 ``TIMER`` 设备时钟源，见 ``drivers\bl702_driver\hal_drv\default_config\timer_config.h``和``drivers\bl702_driver\hal_drv\src\hal_timer.c``

.. code-block:: C
    :linenos:

    #define CONFIG_GPIO14_FUNC GPIO_FUN_UART0_TX
    #define CONFIG_GPIO15_FUNC GPIO_FUN_UART0_RX

-  配置 ``UART`` 设备复用引脚，见 ``bsp/board/bl706_iot/pinmux_config.h``

.. code-block:: C
    :linenos:

    #define BSP_USING_TIMER_CH0

    #if defined(BSP_USING_TIMER_CH0)
    #ifndef TIMER_CH0_CONFIG
    #define TIMER_CH0_CONFIG                    \
        {                                       \
            .id = 0,                            \
            .ch = 0,                            \
            .cnt_mode = TIMER_CNT_PRELOAD,      \
            .pl_trig_src = TIMER_PL_TRIG_COMP0, \
        }
    #endif
    #endif

-  使能 ``BSP_USING_TIMER_CH0`` 并配置 ``TIMER`` 设备配置，见 ``bsp/board/bl706_iot/peripheral_config.h``

.. code-block:: C
    :linenos:

    if (timer_ch0) {
        device_open(timer_ch0, DEVICE_OFLAG_INT);
        device_set_callback(timer_ch0, timer_ch0_irq_callback);
        device_control(timer_ch0, DEVICE_CTRL_SET_INT, NULL);
        device_control(timer_ch0, DEVICE_CTRL_TIMER_CH_START, (void *)(&timer_user_cfg));
    }

-  通过 ``timer_ch0_irq_callback`` 函数，注册用户指定的 ``TIMER0`` 中断服务函数。通过 ``device_control`` 函数使能中断和配置定时周期。

.. code-block:: C
    :linenos:

    void timer_ch0_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
    {
        MSG("timer ch0 interrupt! \n");

        if (state == TIMER_EVENT_COMP0) {
            cnt++;
            MSG("timer event comp0! cnt=%d\n", cnt);
        } else if (state == TIMER_EVENT_COMP1) {
            MSG("timer event comp1! \n");
        } else if (state == TIMER_EVENT_COMP2) {
            MSG("timer event comp2! \n");
        }
    }


-  此函数是示例的中断服务函数，作用是判断具体是哪个 COMP 触发的中断和打印 COMP0 触发中断的次数。

    - ``state`` 会输入 ``TIMER`` 设备的 EVENT 类型
    - ``args`` 包含了返回数据指针
    - ``size`` 包含返回数据的长度
    - ``dev`` 为中断的 ``TIMER`` 设备句柄


编译和烧录
-----------------------------

-  **CDK 编译**

   打开项目中提供的工程文件：timer_int.cdkproj

   参照 :ref:`windows_cdk_quick_start` 的步骤编译下载即可

-  **命令行编译**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=timer_int

-  **烧录**

   详见 :ref:`bl_dev_cube`


实验现象
-----------------------------

   每 1 秒触发 1 次 ``timer ch0`` ``COMP0`` 中断，每次触发 ``TIMER`` 中断都会打印一次 ``timer ch0 interrupt!`` ，并且 ``cnt`` 值每次都会加 1 。
   串口打印：

    ``timer ch0 interrupt! timer event comp0! cnt=1``
    ``timer ch0 interrupt! timer event comp0! cnt=2``
    ``timer ch0 interrupt! timer event comp0! cnt=3``
