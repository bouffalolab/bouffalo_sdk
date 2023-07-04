.. _ble-index:

BLE
==================

概述
------
- BLE支持的特性：
    + 蓝牙HOST特性
        - GAP支持的角色：Peripheral与Central，Observer与Broadcaster
        - GATT支持的角色：Server与Client
        - 支持配对包括蓝牙4.2中的安全连接特性
        - 支持永久存储蓝牙特定的设置和数据
    + 蓝牙mesh特性
        - 支持Relay, Friend Node, Low-Power Node (LPN) and GATT Proxy角色
        - 支持两种Provisioning bearers(PB-ADV & PB-GATT)
- BLE协议栈的架构：
                        .. figure:: img/ble1.png

    + 总共有3个主要层，共同构成了一个完整的蓝牙低能耗协议栈 
        - Host：这一层位于应用程序之下,由多个(非实时)网络和传输协议组成，使应用程序能够以标准和互操作的方式与对等设备通信。
        - Controller：控制器实现了链路层(LE LL)，这是一种低层次的实时协议，它与无线电硬件一起提供了空中通信的标准互操作。LL处理包的接收和传输，保证数据的传递，并处理所有LL控制程序。
        - Radio Hardware：实现所需的模拟和数字基带功能块，允许链路层固件在频谱的2.4GHz波段发送和接收。

- 主控Host：
                        .. figure:: img/ble2.png

    * 蓝牙Host层实现了所有高级协议和配置文件，最重要的是它为应用程序提供了高级API 
        - HCI:Host与controller接口
        - L2CAP:逻辑链路控制和适应协议
        - GATT:通用属性配置层（Generic Attribute Profile）
        - GAP:通用访问配置层（Generic Access Profile）
        - SMP:安全管理器配置层（Security Manager Specification）

- 应用Application
    * 应用层是用户开发实际蓝牙应用的地方，包含必要的协议栈参数设置，以及各种功能函数的调用。我们分别从蓝牙从机和蓝牙主机两种设备来分析。
        * 蓝牙从机 
            - 相关硬件和基础服务初始化
            - 设置广播参数：广播数据，广播间隔，扫描回应等参数或者数据
            - 设置Profile：添加从机服务、特征值，还有设置回调函数用于接收主机数据等
            - 设置配对参数（可选）
            - 启动广播，开始运行
            - 等待相关事件，及事件处理，例如收到主机发来的数据，被链接等等
        * 蓝牙主机 
            - 相关硬件和基础服务初始化
            - 设置扫描参数
            - 设置连接参数
            - 设置配对参数（可选）
            - 启动协议栈，开始运行
            - 等待相关事件，及事件处理，例如扫描事件，从机的Notify事件等等。

CLI 命令
-------------	 

ble_init
^^^^^^^^^^^^^^^^^^^^

- 命令功能：ble通用初始化，在所有ble cli操作前，需要先输入该命令
- 参数：无
- 示例：输入命令 ``ble_init``

    .. figure:: img/image2.png
       :alt:

ble_auth
^^^^^^^^^^^^^^^^^^^^

- 命令功能：注册SMP接口函数
- 参数：无
- 示例：输入命令 ``ble_auth``
 
    .. figure:: img/image3.png
       :alt:	   

ble_unpair
^^^^^^^^^^^^^^^^^^^^

- 命令功能：清除配对key
- 第一个参数表示设备地址类型
  - 0：设备表示public地址类型
  - 1：表示设备地址为random类型
  - 2：表示设备地址为可解析的地址或者Public地址
  - 3：表示设备地址为可解析的地址或者random地址
- 第二个参数代表设备地址，高字节在前低字节在后，如果为0，代表清除所有设备的key
- 示例：输入命令 ``ble_unpair 0 0``
 
    .. figure:: img/image21.png
       :alt:
	   
ble_start_adv
^^^^^^^^^^^^^^^^^^^^

- 命令功能表示：开启广播
- 第一个参数表示广播类型
  - 0：adv_ind 可连接可被扫描;
  - 1：adv_scan_ind 不可被连接可被扫描;
  - 2：adv_nonconn_ind 不可被连接不可被扫描;
  - 3：adv_direct_ind 可被指定的设备连接不可被扫描
	
- 第二个参数表示广播模式
  - 0：General discoverable;
  - 1：non discoverable;
  - 2：limit discoverable;
  
- 第三个参数表示广播间隙最小值,其计算方式为 0.625ms * N,范围应在20 ms to 10.24 s之间
- 第四个参数表示广播间隙最大值,其计算方式为 0.625ms * N,范围应在20 ms to 10.24 s之间
- 示例：输入命令 ``ble_start_adv 0 0 0x80 0x80``
 
    .. figure:: img/image4.png
       :alt:

ble_stop_adv
^^^^^^^^^^^^^^^^^^^^

 - 命令功能：停止ADV广播
 - 参数：无
 - 示例：输入命令 ``ble_stop_adv``
 
    .. figure:: img/image17.png
       :alt:
	   
ble_start_scan
^^^^^^^^^^^^^^^^^^^^

- 命令功能：表示扫描广播设备
- 第一个参数表示扫描类型 
  - 0：表示scan passive type只监听广播数据
  - 1：表示scan active,不仅监听当满足条件还会发scan_req包
  
- 第二个参数表示过滤设备广播包
  - 0：表示不启用重复过滤
  - 1：表示启用重复过滤
  - 2：仅仅接收白名单列表发起的广播和scan response包，除了指定连接地址不是自己的adv_direct_ind广播包
  - 4：使用扩展过滤策略，过滤设备
  
- 第三个参数表示扫描间隙,其计算方式为 0.625ms * N,范围在2.5 ms to 10.24 s之间,其应该大于等于扫描窗口
- 第四个参数表示扫描窗口,其计算方式为 0.625ms * N,范围在2.5 ms to 10.24 s之间,其应该小于等于扫描间隙
- 示例：输入命令 ``ble_start_scan 0 0 0x80 0x40``
 
    .. figure:: img/image11.png
       :alt:
 
ble_stop_scan
^^^^^^^^^^^^^^^^^^^^

- 命令功能：停止扫描
- 参数：无
- 示例：系统进入SCAN后，输入命令 ``ble_stop_scan``
 
    .. figure:: img/image14.png
       :alt:

ble_conn_update
^^^^^^^^^^^^^^^^^^^^

- 命令功能：表示更新连接参数	
- 第一个参数表示连接间隙的最小值,其计算方式为 N * 1.25 ms,其范围在7.5 ms to 4 s
- 第二个参数表示连接间隙的最大值,其计算方式为 N * 1.25 ms,其范围在7.5 ms to 4 s
- 第三个参数表示从设备时延多少个连接事件范围是0~499,比如：该值设置为1，表明延时一个事件的时间进行数据交互，作用是降低交互频率更省电
- 第四个参数表示连接超时时间，计算方式 N * 10 ms,范围是100 ms to 32 s
- 示例：连接成功后，输入命令 ``ble_conn_update 0x28 0x28 0x0 0xf4``
 
    .. figure:: img/image7.png
       :alt:
 
ble_security 
^^^^^^^^^^^^^^^^^^^^

- 命令功能：设置SMP的加密等级	
- 第一个参数表示加密等级，总共有5个等级
  - 0：仅用于BR/EDR，比如SDP服务;
  - 1：表示不需要加密不需要认证的过程; 
  - 2：表示需要加密不需要认证的过程 
  - 3：表示需要加密和认证，比如双方需要输入PIN码 
  - 4：表示需要加密和认证，通过128bit的key
  - 示例：连接成功后，输入命令 ``ble_security 2``
 
    .. figure:: img/image8.png
       :alt:

ble_get_device_name
^^^^^^^^^^^^^^^^^^^^^^^^^^

- 命令功能：获取本地设备名称
- 参数：无
- 示例：输入命令 ``ble_get_device_name``
 
    .. figure:: img/image12.png
       :alt:

ble_set_device_name
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- 命令功能：设置本地设备名称
- 参数：需要设置的设备名字
- 参数：无
- 示例：输入命令 ``ble_set_device_name bl602``
 
    .. figure:: img/image13.png
       :alt:

ble_read_local_address
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- 命令功能：读取本地设备地址
- 参数：无
- 示例：输入命令 ``ble_read_local_address``
 
    .. figure:: img/image15.png
       :alt:
	   
ble_set_adv_channel
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

 - 命令功能：设置ADV通道
 - 参数：需要设定的ADV通道数，其值范围为1-7，参数大小为1byte，bit0代表通道37，bit1代表通道38，bit2代表通道39
 - 示例：输入命令 ``ble_set_adv_channel 4``
 
    .. figure:: img/image16.png
       :alt:

ble_connect
^^^^^^^^^^^^^^^^^^^^

- 命令功能：连接指定地址的设备
- 第一个参数表示设备地址类型
  - 0：设备表示public地址类型
  - 1：表示设备地址为random类型
  - 2：表示设备地址为可解析的地址或者Public地址
  - 3：表示设备地址为可解析的地址或者random地址
- 第二个参数代表设备地址，高字节在前低字节在后
- 示例：输入命令 ``ble_connect 0 18B905DE96E0``
 
    .. figure:: img/image18.png
       :alt:

ble_disconnect
^^^^^^^^^^^^^^^^^^^^

- 命令功能：断开指定地址的设备的连接
- 第一个参数表示设备地址类型
  - 0：设备表示public地址类型
  - 1：表示设备地址为random类型
  - 2：表示设备地址为可解析的地址或者Public地址
  - 3：表示设备地址为可解析的地址或者random地址
- 第二个参数代表设备地址，高字节在前低字节在后
- 示例：连接成功后，输入命令 ``ble_disconnect 0 18B905DE96E0``
 
    .. figure:: img/image19.png
       :alt:

ble_select_conn
^^^^^^^^^^^^^^^^^^^^

- 命令功能：多个连接中，将某一个连接对象设置为当前连接对象
- 第一个参数表示设备地址类型
  - 0：设备表示public地址类型
  - 1：表示设备地址为random类型
  - 2：表示设备地址为可解析的地址或者Public地址
  - 3：表示设备地址为可解析的地址或者random地址
- 第二个参数代表设备地址，高字节在前低字节在后	
- 示例：多个设备连接成功后，输入命令 ``ble_select_conn 1 5F10546C8D83``，将选定的连接对象设置为当前连接对象，后续的ble_read等操作将会作用在该连接上
 
    .. figure:: img/image20.png
       :alt:

ble_auth_cancel
^^^^^^^^^^^^^^^^^^^^

- 命令功能：取消加密认证过程
- 参数：无
- 示例：当在SMP过程中，输入命令 ``ble_auth_cancel``
 
    .. figure:: img/image22.png
       :alt:

ble_auth_passkey_confirm
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- 命令功能：接收到passkey后回复远端，并且对端设备在配对过程中也有显示该passkey； 例如：配对过程本地打印 Confirm passkey for 48:95:E6:73:1C:1A (random): 745491；可发送该函数进行回复
   
- 参数：无
- 示例：当在SMP过程中，对应security level为3，需要输入命令 ``ble_auth_passkey_confirm``
 
    .. figure:: img/image9.png
       :alt:

ble_auth_pairing_confirm
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：接收到远端配对请求，用此函数回复远端配对请求，例如：配对过程本地打印 Confirm pairing for 00:1B:DC:F2:20:E9 (public)；可发送该函数进行回复
- 参数：无
- 示例：当在SMP过程中，对应的security level为2，输入命令 ``ble_auth_pairing_confirm``，
 
    .. figure:: img/image23.png
       :alt:

ble_auth_passkey
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：请求输入passkey
- 参数：passkey值，其范围为0-999999
- 示例：当用ble_security 3命令进行配对，且SMP配对方法为PASSKEY_INPUT（代码中实现方法：用ble_auth注册smp接口函数时，在数据结构bt_conn_auth_cb中将函数passkey_entry填充，passkey_display与passkey_confirm不填充，其它接口函数默认即可），串口将打印出Enter passkey for XX:XX:XX:XX:XX:XX (public)，此时输入命令 ``ble_auth_passkey 111111`` 完成配对
    
    .. figure:: img/image24.png
       :alt:
	   
ble_exchange_mtu
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：交换mtu大小
- 参数： 无
- 示例：连接成功后，输入命令 ``ble_exchange_mtu``
 
    .. figure:: img/image25.png
       :alt:

ble_discover
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：查询指定的服务或特性
- 第一个参数表示需要查询的类型
  - 0：primary
  - 1：secondary
  - 2：include
  - 3：Characteristic
  - 4：Descriptor
- 第二个参数表示2BYTES的uuid
- 第三个参数表示起始句柄，占2BYTES
- 第四个参数表示结束句柄，占2BYTES
- 示例：连接成功后，输入命令 ``ble_discover 0 0x1800 0x1 0xffff``
 
    .. figure:: img/image26.png
       :alt:

ble_read
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：读取指定句柄的数据
- 第一个参数表示句柄
- 第二个参数表示偏移量
- 示例：连接成功后，输入命令 ``ble_read 0x5 0``
 
    .. figure:: img/image27.png
       :alt:

ble_write
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：指定句柄写入相应的数据
- 第一个参数表示句柄，占2bytes
- 第二个参数表示偏移量，占2bytes
- 第三个参数表示数据长度，占2bytes,最大不超过512
- 第四个参数表示需要写入的数据
 
- 示例：连接成功后，写入2个字节的数据，命令为 ``ble_write 0xf 0 2 0102``,其中01为一个byte，02为一个byte
 
    .. figure:: img/image28.png
       :alt:

ble_write_without_rsp
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：指定句柄写入相应的数据并且不需要回复
- 第一参数表示是否启动sign write命令
  - 0：不使能sign write命令
  - 1：使能sign write命令
- 第二个参数表示句柄，占2bytes，
- 第三个参数表示数据的长度，占2bytes，最大不超过512
- 第四个参数表示写入的数据
 
 - 示例：连接成功后，写入2个字节的数据，命令为 ``ble_write_without_rsp 0 0xf 2 0102``，其中01为一个byte，02为一个byte
 
    .. figure:: img/image29.png
       :alt:

ble_subscribe
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：订阅CCC
- 第一个参数表示CCC句柄
- 第二个参数表示订阅值的句柄
- 第三个参数表示订阅类型
  - 1：表示notification
  - 2：表示indication

 - 示例：连接成功后，输入命令 ``ble_subscribe 0xf 0xd 0x1``，表示使能CCC的notification
 
    .. figure:: img/image30.png
       :alt:
 
ble_unsubscribe
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：取消订阅CCC
- 参数：无
- 示例：输入命令 ``ble_unsubscribe``
 
    .. figure:: img/image31.png
       :alt:	   

ble_set_data_len
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：设置pdu数据长度
- 第一个参数表示有效荷载传输的最大值,范围为0x001B - 0x00FB
- 第二个参数表示有效荷载传输的最大时间,范围值为0x0148 - 0x4290
 
- 示例：当连接成功后，发送命令 ``ble_set_data_len 0xfb 0x0848``
 
    .. figure:: img/image32.png
       :alt:
	   
ble_conn_info
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：获取所有的连接信息
- 参数：无
- 示例：当连接成功后，发送命令 ``ble_conn_info`` ，获取已连接的设备
 
    .. figure:: img/image33.png
       :alt:

ble_disable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：注销BLE
- 参数：无
- 示例：当无scan/adv/connect事件，发送命令 ``ble_disable``
 
    .. figure:: img/image34.png
       :alt:

ble_set_tx_pwr
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
- 命令功能：设置发送功率
- 第一个参数表示设置功率值
- 示例：发送命令 ``ble_set_tx_pwr 0xa``
 
    .. figure:: img/image35.png
       :alt:

Functions
----------------

ble stack 采用 zephyr ble stack，因此 API 请参考 `zephyr bluetooth api <https://docs.zephyrproject.org/apidoc/latest/group__bluetooth.html>`_ 。