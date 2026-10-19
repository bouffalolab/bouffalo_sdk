.. _bt-index:

BT
==================

CLI 命令
-------------

bredr_init
^^^^^^^^^^^^^^^^^^^^

- 命令功能：bt通用初始化，在所有bt cli操作前，需要先输入该命令
- 参数：无
- 示例：输入命令 ``bredr_init``

    .. figure:: img/bt_image1.png
       :alt:

bredr_connect
^^^^^^^^^^^^^^^^^^^^

- 命令功能: 连接指定地址的设备
- 第一个参数代表设备地址，高字节在前低字节在后
- 示例：输入命令 ``bredr_connect d435386d7424``

    .. figure:: img/bt_image2.png
       :alt:
	   
a2dp_connect
^^^^^^^^^^^^^^^^^^^^

- 命令功能: 建立A2DP 连接
- 参数：无
- 示例：输入命令 ``a2dp_connect``

    .. figure:: img/bt_image3.png
       :alt:

a2dp_source_resume
^^^^^^^^^^^^^^^^^^^^

- 命令功能: 播放一段音频数据
- 参数：无
- 示例：输入命令 ``a2dp_source_resume``

    .. figure:: img/bt_image4.png
       :alt:
	   
a2dp_source_suspend
^^^^^^^^^^^^^^^^^^^^

- 命令功能: 暂停音频播放
- 参数：无
- 示例：输入命令  ``a2dp_source_suspend``

    .. figure:: img/bt_image5.png
       :alt: