L1C
=============

Macros
------------

无

Structs
------------

无

Functions
------------

bflb_l1c_icache_enable
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 开启 icache。

.. code-block:: c
   :linenos:

    void bflb_l1c_icache_enable(void);

bflb_l1c_icache_disable
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 关闭 icache。

.. code-block:: c
   :linenos:

    void bflb_l1c_icache_disable(void);

bflb_l1c_dcache_enable
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 开启 dcache。

.. code-block:: c
   :linenos:

    void bflb_l1c_dcache_enable(void);

bflb_l1c_dcache_disable
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 关闭 dcache。

.. code-block:: c
   :linenos:

    void bflb_l1c_dcache_disable(void);

bflb_l1c_dcache_clean_range
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: clean 一段数据到内存中。

.. code-block:: c
   :linenos:

    void bflb_l1c_dcache_clean_range(unsigned long addr, uint32_t len);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - addr
      - 首地址（必须 32 字节对齐）
    * - len
      - 长度

bflb_l1c_dcache_invalidate_range
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 将 cache 中的数据置为 dity。

.. code-block:: c
   :linenos:

    void bflb_l1c_dcache_invalidate_range(unsigned long addr, uint32_t len);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - addr
      - 首地址（必须 32 字节对齐）
    * - len
      - 长度

bflb_l1c_dcache_clean_invalidate_range
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: clean 一段数据到内存中，并使 cache 中的数据失效。

.. code-block:: c
   :linenos:

    void bflb_l1c_dcache_clean_invalidate_range(unsigned long addr, uint32_t len);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - addr
      - 首地址（必须 32 字节对齐）
    * - len
      - 长度
