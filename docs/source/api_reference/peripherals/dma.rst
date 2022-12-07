DMA
=============

Macros
------------

dma direction
^^^^^^^^^^^^^^^

dma 传输方向支持 4 种。

.. code-block:: c
   :linenos:

    #define DMA_MEMORY_TO_MEMORY 0
    #define DMA_MEMORY_TO_PERIPH 1
    #define DMA_PERIPH_TO_MEMORY 2
    #define DMA_PERIPH_TO_PERIPH 3

dma addr increment
^^^^^^^^^^^^^^^^^^^^^

dma 地址自增使能。

.. code-block:: c
   :linenos:

    #define DMA_ADDR_INCREMENT_DISABLE 0
    #define DMA_ADDR_INCREMENT_ENABLE  1

dma data width
^^^^^^^^^^^^^^^^^^^^^

dma 传输位宽支持 8BIT 、16BIT、32BIT。

.. code-block:: c
   :linenos:

    #define DMA_TRANSFER_WIDTH_8BIT  0
    #define DMA_TRANSFER_WIDTH_16BIT 1
    #define DMA_TRANSFER_WIDTH_32BIT 2

dma burst size
^^^^^^^^^^^^^^^^^^^^^

dma 突发传输个数，最多 16 字节突发。

.. code-block:: c
   :linenos:

    #define DMA_BURST_INCR1  0
    #define DMA_BURST_INCR4  1
    #define DMA_BURST_INCR8  2
    #define DMA_BURST_INCR16 3

.. warning:: 位宽 * 突发个数 需要小于等于 16 字节

dma addr
^^^^^^^^^^^^^^^^^^^^^

参考 `bflb_dma.h` 文件，不详细列出。

dma request
^^^^^^^^^^^^^^^^^^^^^

参考 `bflb_dma.h` 文件，不详细列出。

Structs
------------

struct bflb_dma_channel_lli_pool_s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

由于 dma 支持 lli 模式，所以需要在传输之前分配一块内存池给 lli 使用, 调用 `bflb_dma_lli_reload` 后会自动消耗内存池和配置相关 lli 信息，无需用户手动配置。

.. code-block:: c
   :linenos:

    struct bflb_dma_channel_lli_pool_s {
        uint32_t src_addr;
        uint32_t dst_addr;
        uint32_t nextlli;
        union bflb_dma_lli_control_s lli_control;
    };

struct bflb_dma_channel_lli_transfer_s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

用户使用 lli 传输时，需要填充的传输内容：源地址、目标地址和长度，并且如果地址不连续，可以增加多个 transfer。

.. code-block:: c
   :linenos:

    struct bflb_dma_channel_lli_transfer_s {
        uint32_t src_addr;
        uint32_t dst_addr;
        uint32_t nbytes;
    };

struct bflb_dma_channel_config_s
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

初始化 dma 通道时需要填充的信息。

.. code-block:: c
   :linenos:

    struct bflb_dma_channel_config_s {
        uint8_t direction;
        uint32_t src_req;
        uint32_t dst_req;
        uint8_t src_addr_inc;
        uint8_t dst_addr_inc;
        uint8_t src_burst_count;
        uint8_t dst_burst_count;
        uint8_t src_width;
        uint8_t dst_width;
    };

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - direction
      - 传输方向
    * - src_req
      - 源 dma 请求
    * - dst_req
      - 目标 dma 请求
    * - src_addr_inc
      - 源地址是否自增
    * - dst_addr_inc
      - 目标地址是否自增
    * - src_burst_count
      - 源地址突发个数
    * - dst_burst_count
      - 目标地址突发个数
    * - src_width
      - 源地址位宽
    * - dst_width
      - 目标地址位宽

Functions
------------

bflb_dma_channel_init
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 初始化 dma 通道。

.. code-block:: c
   :linenos:

    void bflb_dma_channel_init(struct bflb_device_s *dev, const struct bflb_dma_channel_config_s *config);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - config
      - 配置参数

bflb_dma_channel_deinit
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 反初始化 dma 通道。

.. code-block:: c
   :linenos:

    void bflb_dma_channel_deinit(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_dma_channel_start
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 启动 dma 通道传输。

.. code-block:: c
   :linenos:

    void bflb_dma_channel_start(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_dma_channel_stop
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 关闭 dma 通道传输。

.. code-block:: c
   :linenos:

    void bflb_dma_channel_stop(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_dma_channel_isbusy
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 检查 dma 通道是否处于 busy 状态。

.. code-block:: c
   :linenos:

   bool bflb_dma_channel_isbusy(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - 是否 busy

bflb_dma_channel_irq_attach
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 注册 dma 通道中断完成回调函数并开启通道完成中断。

.. code-block:: c
    :linenos:

    void bflb_dma_channel_irq_attach(struct bflb_device_s *dev, void (*callback)(void *arg), void *arg);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - callback
      - 回调函数
    * - arg
      - 用户参数

bflb_dma_channel_irq_detach
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 关闭 dma 通道完成中断。

.. code-block:: c
    :linenos:

    void bflb_dma_channel_irq_detach(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄

bflb_dma_channel_lli_reload
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 配置 dma 通道的 lli 信息。

.. code-block:: c
   :linenos:

    int bflb_dma_channel_lli_reload(struct bflb_device_s *dev,
                                    struct bflb_dma_channel_lli_pool_s *lli_pool, uint32_t max_lli_count,
                                    struct bflb_dma_channel_lli_transfer_s *transfer, uint32_t count);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - lli_pool
      - lli 内存池
    * - max_lli_count
      - lli 内存池大小
    * - transfer
      - 传输句柄
    * - count
      - 传输次数
    * - return
      - 返回使用的 lli count个数，小于0表示错误

bflb_dma_channel_lli_link_head
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 将 lli 最后一个链表与头部连接，表示开启循环模式。

.. code-block:: c
   :linenos:

   void bflb_dma_channel_lli_link_head(struct bflb_device_s *dev,
   struct bflb_dma_channel_lli_pool_s *lli_pool, uint32_t used_lli_count);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - lli_pool
      - lli 内存池
    * - used_lli_count
      - 已经使用的 lli 个数

bflb_dma_feature_control
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明:  dma 其他特性相关控制，一般不常用。

.. code-block:: c
    :linenos:

    void bflb_dma_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - cmd
      - 控制字
    * - arg
      - 控制参数

`cmd` 可以填入以下参数:

.. code-block:: c
    :linenos:

    #define DMA_CMD_SET_SRCADDR_INCREMENT (0x01)
    #define DMA_CMD_SET_DSTADDR_INCREMENT (0x02)
    #define DMA_CMD_SET_ADD_MODE          (0x03)
    #define DMA_CMD_SET_REDUCE_MODE       (0x04)
