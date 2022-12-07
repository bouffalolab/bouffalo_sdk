.. _debug:

使用调试器调试代码
====================

本节主要介绍如何使用调试器进行代码的调试。

芯片默认 JTAG PIN 列表
-------------------------------------------

.. list-table::
    :widths: 10 10 10 10 10
    :header-rows: 1

    * - CHIP/Pin
      - BL602/BL604
      - BL702/BL704/BL706
      - BL616/BL618
      - BL808
    * - TMS
      - GPIO12
      - GPIO0
      - GPIO0
      - GPIO6
    * - TCK
      - GPIO14
      - GPIO2
      - GPIO1
      - GPIO12
    * - TDO
      - GPIO11
      - GPIO9
      - GPIO2
      - GPIO7
    * - TDI
      - GPIO17
      - GPIO1
      - GPIO3
      - GPIO13

使用 CKLink + Eclipse 调试代码
-------------------------------------------

- 从 T-HEAD 官网下载最新版本 `CKLink 驱动 <https://occ.t-head.cn/community/download?id=4103855596351066112>`_

安装和配置 CKLink
^^^^^^^^^^^^^^^^^^^^

- 将下载的 CKLink 安装包解压，并双击 `Setup` ,一路 next 即可，安装完成以后，桌面会有一个 `T-HeadDebugServer` 图标

.. figure:: img/cklink1.png
    :alt:

.. figure:: img/cklink2.png
    :alt:

- 将板子的 JTAG 引脚和 CKLink 调试器连接以后，点击 **三角** 按钮，如果变成 **圆圈**，则表示 JTAG 连接成功。如果失败，则需要检查 JTAG 线序是否正确。

.. figure:: img/cklink3.png
    :alt:

使用 Eclipse 调试
^^^^^^^^^^^^^^^^^^^^

参考 :ref:`eclipse_gcc`
