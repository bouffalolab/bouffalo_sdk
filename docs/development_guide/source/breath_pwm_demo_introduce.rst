呼吸灯示例
====================

硬件连接
-----------------------------

.. figure:: img/blink_breath_sch.png
    :alt:

    **BL706_IOT板支持上述电路**

-  硬件连接如上图所示 ``GPIO29`` ``GPIO30`` ``GPIO31`` ``GPIO22`` 分别控制四路 LED 灯
-  呼吸灯会单独使用 ``GPIO22`` 来演示效果



软件实现
-----------------------------

-  软件代码见 ``examples/pwm/pwm_breath_led``


-  ``PWM`` 的输出引脚由板级描述文件 ``bsp/board/bl706_iot/pinmux_config.h`` 来配置

.. code-block:: C
    :linenos:

    #define CONFIG_GPIO22_FUNC GPIO_FUN_UNUSED


-  可以看到，我们将 ``GPIO22`` 的功能选择为 ``PWM`` 输出，不同的电路板会存在不同的引脚配置。

-  我们会使用向导来配置不同应用场景下的引脚配置，用户也可以手动修改。

.. code-block:: C
    :linenos:

    pwm_register(PWM0_INDEX, "led_breath", DEVICE_OFLAG_RDWR); 

    struct device *led_breath = device_find("led_breath");

    if (led_breath)
    {
        device_open(led_breath, 0);
    }


-  上述代码展现了 ``bl_mcu_sdk`` 框架最一般的设备初始化流程，首先调用 ``pwm_register`` 函数注册 ``pwm`` 设备
-  然后通过 ``find`` 函数找到设备对应的句柄，保存于 ``led_breath`` 对象中
-  使用 ``device_open`` 来打开 ``led_breath`` 设备。

.. code-block:: C
    :linenos:

    device_control(led_breath, DEVICE_CTRL_CONFIG, &pwm_cfg);  

使用 ``device_contorl`` 函数，配合 ``DEVICE_CTRL_CONFIG`` 指令，输入需要配置的频率和占空比，即可配置 ``PWM`` 的输出波形。



最终效果
-----------------------------
见视频展示

