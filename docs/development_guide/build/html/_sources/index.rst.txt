.. bl_mcu_sdk_development_guide documentation master file, created by
   sphinx-quickstart on Fri Mar 12 16:32:56 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

========================================================
BL MCU SDK 开发指南
========================================================

博流 `BL702 <https://www.bouffalolab.com/bl70X>`_ 系列产品是基于 “SiFive E24 Core” `RISC-V <https://en.wikipedia.org/wiki/RISC-V>`_ 处理器的通用微控制器，具有 BLE 5.0、zigbee 和以太网等网络功能，同时还具备其它丰富的外设。
可被广泛应用于 `IoT <https://en.wikipedia.org/wiki/Internet_of_things>`_ 和其他低功耗领域中。
支持通过 `JTAG <https://en.wikipedia.org/wiki/JTAG>`_ 对芯片进行烧写和调试，也支持通过 `UART <https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter>`_ 进行程序烧录。

BL MCU SDK 将为您提供博流 BL70X 系列 MCU 开发的全方位支持。

.. toctree::
   :maxdepth: 1
   :caption: 快速开发指南
   :numbered:

   get_started
   Windows_quick_start_cdk
   Windows_quick_start_eclipse
   Linux_quick_start_ubuntu
   connecting_hardware


   
.. toctree::
   :maxdepth: 1
   :caption: API 手册
   :numbered:

   api_guide

   
.. toctree::
   :maxdepth: 1
   :caption: 示例代码
   :numbered:

   blink_demo_introduce
   breath_pwm_demo_introduce
   uart_loopback_demo_introduce

