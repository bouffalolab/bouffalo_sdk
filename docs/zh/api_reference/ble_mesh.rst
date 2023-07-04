.. _blemesh-index:

BLE_MESH
==================

CLI 命令
-------------	 

blemesh_init
^^^^^^^^^^^^^^^^^^^^

- 命令功能：blemesh通用初始化，在所有blemesh cli操作前，需要先输入该命令
- 参数：无
- 示例：输入命令 ``blemesh_init``

blemesh_pb
^^^^^^^^^^^^^^^^^^^^

- 命令功能：设置 Provisioning 承载
- 第一个参数表示 Provisioning 承载方式
  - 1：承载方式 PB-ADV
  - 2：承载方式 PB-GATT

- 第二个参数表示使能
- 示例：输入命令 ``blemesh_pb 2 1``
 
Functions
----------------

ble stack 采用 zephyr ble mesh，因此 API 请参考 `zephyr bluetooth mesh api <https://docs.zephyrproject.org/apidoc/latest/group__bt__mesh.html>`_ 。