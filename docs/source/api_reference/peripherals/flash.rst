FLASH
=============

Macros
------------

flash iomode
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c
   :linenos:

   #define FLASH_IOMODE_NIO 0
   #define FLASH_IOMODE_DO  1
   #define FLASH_IOMODE_QO  2
   #define FLASH_IOMODE_DIO 3
   #define FLASH_IOMODE_QIO 4

Structs
------------

无

Functions
------------

bflb_flash_init
^^^^^^^^^^^^^^^^^^^^

说明: flash 初始化，自动识别支持的 flash 并重新配置 flash 的参数。如果返回错误，必须停止运行代码。

.. code-block:: c
   :linenos:

   int bflb_flash_init(void);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - return
      - 返回 0 表示成功，其他表示错误，必须停止运行代码

bflb_flash_get_jedec_id
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取 flash jedec id。

.. code-block:: c
   :linenos:

   uint32_t bflb_flash_get_jedec_id(void);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - return
      - 返回 flash jedec id

bflb_flash_get_cfg
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取 flash 配置。

.. code-block:: c
   :linenos:

   void bflb_flash_get_cfg(uint8_t **cfg_addr, uint32_t *len);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - cfg_addr
      - 保存 flash 配置的地址
    * - len
      - flash 配置的长度

bflb_flash_set_iomode
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 设置 flash IO 工作模式。

.. code-block:: c
   :linenos:

   void bflb_flash_set_iomode(uint8_t iomode);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - iomode
      - flash IO 工作模式

bflb_flash_get_image_offset
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取代码 xip 虚拟地址实际所在的 flash 物理地址。

.. code-block:: c
   :linenos:

   uint32_t bflb_flash_get_image_offset(void);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - return
      - 返回 flash xip 物理地址

bflb_flash_erase
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: flash 扇区擦除。 **len** 为擦除的长度，需要为 4096 倍数，假设 **addr** 为0 ， **len** 为 4096，则擦除范围为 0 ~ 4095。

.. code-block:: c
   :linenos:

   int bflb_flash_erase(uint32_t addr, uint32_t len);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - addr
      - 擦除的物理地址
    * - len
      - 擦除长度，需要是 4096 的倍数
    * - return
      - 返回 0 表示成功，其他表示错误

bflb_flash_write
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取代码 xip 虚拟地址实际所在的 flash 物理地址。

.. code-block:: c
   :linenos:

   int bflb_flash_write(uint32_t addr, uint8_t *data, uint32_t len);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - addr
      - 写入的物理地址
    * - data
      - 写入的数据缓冲区
    * - len
      - 写入长度
    * - return
      - 返回 0 表示成功，其他表示错误

bflb_flash_read
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取代码 xip 虚拟地址实际所在的 flash 物理地址。

.. code-block:: c
   :linenos:

   int bflb_flash_read(uint32_t addr, uint8_t *data, uint32_t len);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - addr
      - 读取的物理地址
    * - data
      - 读取的数据缓冲区
    * - len
      - 读取长度
    * - return
      - 返回 0 表示成功，其他表示错误

bflb_flash_aes_init
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 配置某一段 flash 区域进行硬件 aes 解密，能够通过 xip 直接读取解密后的内容。

.. code-block:: c
   :linenos:

   void bflb_flash_aes_init(struct bflb_flash_aes_config_s *config);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - config
      - flash aes 配置

bflb_flash_aes_enable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 使能 flash aes 解密。

.. code-block:: c
   :linenos:

   void bflb_flash_aes_enable(void);

bflb_flash_aes_disable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 关闭 flash aes 解密。

.. code-block:: c
   :linenos:

   void bflb_flash_aes_disable(void);

