USB Stack
=======================

USB Stack 是一个跨平台的、用于嵌入式 MCU 的 USB 协议栈，该协议栈对标准设备请求、CLASS 请求、VENDOR 请求规范了一套统一的函数框架，从而对复合设备或者使用自定义设备类时，能够在极短的时间内进行添加和移植。此外，该协议栈代码优美，内存占用小（不使用静态数组），通用性非常高，提供了一套标准的 porting 接口，供给不同的 MCU 使用。其中，usb device 协议栈当前具有以下功能：

- 支持 USB2.0 全速和高速设备
- 支持端点中断注册功能，porting 给用户自己处理中断里的数据
- 支持复合设备
- 支持 Communication Class (CDC)
- 支持 Human Interface Device (HID)
- 支持 Custom human Interface Device (HID)
- 支持 Mass Storage Class (MSC)
- 支持 USB VIDEO CLASS (UVC)
- 支持 USB AUDIO CLASS (UAC)
- 支持 vendor 类 class
- 支持 WINUSB1.0、WINUSB2.0

.. note:: USB DEVICE 协议栈的代码实现过程参考 `<https://www.bilibili.com/video/BV1Ef4y1t73d>`_

USB Device Porting 接口
------------------------

    USB Device Porting 接口在 ``usb_stack/common/usb_dc.h`` 文件中声明，用户根据自己的 MCU 进行实现

**usbd_set_address**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_ep_open**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_ep_close**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_ep_set_stall**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_ep_clear_stall**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_ep_is_stalled**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_ep_write**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_ep_read**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_event_notify_handler**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

USB Device 应用层接口
----------------------

USB Device 控制器接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usb_dc_init**
""""""""""""""""""

USB Device 通用接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_desc_register**
""""""""""""""""""""""""""""""""""""

**usbd_msosv1_desc_register**
""""""""""""""""""""""""""""""""""""

**usbd_class_add_interface**
""""""""""""""""""""""""""""""""""""

**usbd_interface_add_endpoint**
""""""""""""""""""""""""""""""""""""

**usb_device_is_configured**
""""""""""""""""""""""""""""""""""""

USB Device CDC 类接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

USB Device MSC 类接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

USB Device HID 类接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

USB Device AUDIO 类接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

USB Device VIDEO 类接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
