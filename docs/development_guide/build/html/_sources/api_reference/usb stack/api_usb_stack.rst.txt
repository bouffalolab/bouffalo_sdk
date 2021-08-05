USB Stack
=======================

USB Stack 是一个跨平台的、用于嵌入式 MCU 的 USB 协议栈。其中 DEVICE 协议栈对标准设备请求、CLASS 请求、VENDOR 请求规范了一套统一的函数框架，从而对复合设备或者使用自定义设备类时，能够在极短的时间内进行添加和移植。同时提供了一套标准的 dcd porting 接口，供给不同的 MCU 使用,因此，通用性也非常高。此外在代码优美方面，以及内存占用方面也是相当出色。USB DEVICE 协议栈当前具有以下功能：

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

USB DEVICE 协议栈 porting 接口
-------------------------------

USB DEVICE 协议栈 porting 接口在 ``usb_stack/common/usb_dc.h`` 文件中声明，用户根据自己的 MCU 实现以下接口

    - ``usbd_set_address``      调用    :ref:`usb_dc_set_address`
    - ``usbd_ep_open``          调用    :ref:`usb_dc_ep_open`
    - ``usbd_ep_close``         调用    :ref:`usb_dc_ep_close`
    - ``usbd_ep_set_stall``     调用    :ref:`usb_dc_ep_set_stall`
    - ``usbd_ep_clear_stall``   调用    :ref:`usb_dc_ep_clear_stall`
    - ``usbd_ep_is_stalled``    调用    :ref:`usb_dc_ep_is_stalled`
    - ``usbd_ep_write``         调用    :ref:`usb_dc_ep_write`
    - ``usbd_ep_read``          调用    :ref:`usb_dc_ep_read`

USB DEVICE 控制器接口
-------------------------------

**usb_dc_init**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
``usb_dc_init`` 用来注册 USB 设备和初始化 USB 硬件相关寄存器，注册 usb 中断回调函数。在注册之前需要打开对应 USB 设备的宏定义,例如定义宏 ``BSP_USING_USB`` 方可使用 USB 设备。

.. code-block:: C

    struct device *usb_dc_init(void)
    {
        usb_dc_register(USB_INDEX, "usb");
        usb = device_find("usb");
        device_set_callback(usb, usb_dc_event_callback);
        device_open(usb, 0);
        return usb;
    }

- **device** 返回 USB 设备句柄

.. note::中断处理函数则是调用 ``usbd_event_notify_handler``

USB DEVICE 应用层接口
------------------------

USB DEVICE 通用接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_desc_register**
""""""""""""""""""""""""""""""""""""

``usbd_desc_register`` 用来注册 USB 描述符，描述符种类包括：设备描述符、配置描述符、接口描述符、字符串描述符、设备限定描述符。

.. code-block:: C

    void usbd_desc_register(const uint8_t *desc);

- **desc**  描述符的句柄


**usbd_msosv1_desc_register**
""""""""""""""""""""""""""""""""""""

``usbd_msosv1_desc_register`` 用来注册一个 WINUSB 描述符，格式按照 ``struct usb_msosv1_descriptor``。

.. code-block:: C

    void usbd_msosv1_desc_register(struct usb_msosv1_descriptor *desc);

- **desc**  描述符句柄


**usbd_class_add_interface**
""""""""""""""""""""""""""""""""""""

``usbd_class_add_interface`` 用来给 USB 设备类增加接口，并将接口信息挂载在类的链表上。

.. code-block:: C

    void usbd_class_add_interface(usbd_class_t *class, usbd_interface_t *intf);

- **class**  USB 设备类的句柄
- **intf**   USB 设备接口的句柄

``usbd_class_t`` 定义如下

.. code-block:: C

    typedef struct usbd_class {
        usb_slist_t list;
        const char *name;
        usb_slist_t intf_list;
    } usbd_class_t;

- **list** 类的链表节点
- **name** 类的名称
- **intf_list** 接口的链表节点

``usbd_interface_t`` 定义如下

.. code-block:: C

    typedef struct usbd_interface {
        usb_slist_t list;
        /** Handler for USB Class specific Control (EP 0) communications */
        usbd_request_handler class_handler;
        /** Handler for USB Vendor specific commands */
        usbd_request_handler vendor_handler;
        /** Handler for USB custom specific commands */
        usbd_request_handler custom_handler;
        /** Handler for USB event notify commands */
        usbd_notify_handler notify_handler;
        uint8_t intf_num;
        usb_slist_t ep_list;
    } usbd_interface_t;

- **list** 接口的链表节点
- **class_handler** class setup 请求回调函数
- **vendor_handler** vendor setup 请求回调函数
- **custom_handler** custom setup 请求回调函数
- **notify_handler** 中断标志、协议栈相关状态回调函数
- **intf_num** 当前接口偏移
- **ep_list** 端点的链表节点

**usbd_interface_add_endpoint**
""""""""""""""""""""""""""""""""""""

``usbd_interface_add_endpoint`` 用来给 USB 接口增加端点，并将端点信息挂载在接口的链表上。

.. code-block:: C

    void usbd_interface_add_endpoint(usbd_interface_t *intf, usbd_endpoint_t *ep);


- **intf**  USB 设备接口的句柄
- **ep**    USB 设备端点的句柄

``usbd_class_t`` 定义如下

.. code-block:: C

    typedef struct usbd_endpoint {
        usb_slist_t list;
        uint8_t ep_addr;
        usbd_endpoint_callback ep_cb;
    } usbd_endpoint_t;

- **list** 端点的链表节点
- **ep_addr** 端点地址
- **ep_cb** 端点中断回调函数

**usb_device_is_configured**
""""""""""""""""""""""""""""""""""""

``usb_device_is_configured`` 用来检查 USB 设备是否被配置（枚举）。

.. code-block:: C

    bool usb_device_is_configured(void);

- **return** 配置状态， 0 表示未配置， 1 表示配置成功


USB Device CDC 类接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_cdc_add_acm_interface**
""""""""""""""""""""""""""""""""""""

``usbd_cdc_add_acm_interface`` 用来给 USB CDC ACM 类添加接口，并重写该接口相关的函数。重写的函数包括 ``cdc_acm_class_request_handler`` 和 ``cdc_notify_handler``，
其中 ``cdc_acm_class_request_handler`` 用于处理 USB CDC ACM Setup 中断请求， ``cdc_notify_handler`` 用于实现 USB CDC 其他中断回调函数。

.. code-block:: C

    void usbd_cdc_add_acm_interface(usbd_class_t *class, usbd_interface_t *intf);

- **class** 类的句柄
- **intf**  接口句柄

USB Device MSC 类接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_msc_class_init**
""""""""""""""""""""""""""""""""""""
``usbd_msc_class_init`` 用于初始化 USB MSC 类，注册 USB CDC ACM 设备并为其添加接口，且为接口添加 BLUK OUT 、BULK IN 端点及其回调函数。

.. code-block:: C

    void usbd_msc_class_init(uint8_t out_ep, uint8_t in_ep);

- **out_ep**     输出端点的地址
- **in_ep**      输入端点的地址


USB Device HID 类接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_hid_add_interface**
""""""""""""""""""""""""""""""""""""
``usbd_hid_add_interface``  用来给 USB HID 类添加接口，并重写该接口相关的函数。重写的函数包括 ``hid_class_request_handler`` 、 ``hid_custom_request_handler``
和 ``hid_notify_handler``，其中 ``hid_class_request_handler`` 用来处理 USB HID 类的 Setup 中断请求， ``hid_custom_request_handler`` 用来处理 USB HID 获取描述符请求，
``hid_notify_handler``  用来处理 USB HID 类的其他中断回调函数。

.. code-block:: C

    void usbd_hid_add_interface(usbd_class_t *class, usbd_interface_t *intf);

- **class** 类的句柄
- **intf**  接口句柄


USB Device AUDIO 类接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_audio_add_interface**
""""""""""""""""""""""""""""""""""""
``usbd_audio_add_interface``  用来给 USB Audio 类添加接口，并重写该接口相关的函数。重写的函数包括 ``audio_class_request_handler`` 和 ``audio_notify_handler``。

.. code-block:: C

    void usbd_audio_add_interface(usbd_class_t *class, usbd_interface_t *intf);

- **class** 类的句柄
- **intf**  接口句柄


USB Device VIDEO 类接口
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**usbd_video_add_interface**
""""""""""""""""""""""""""""""""""""
``usbd_video_add_interface``  用来给 USB Video 类添加接口，并重写该接口相关的函数。重写的函数包括 ``video_class_request_handler`` 和 ``video_notify_handler``。

.. code-block:: C

    void usbd_video_add_interface(usbd_class_t *class, usbd_interface_t *intf);

- **class** 类的句柄
- **intf**  接口句柄