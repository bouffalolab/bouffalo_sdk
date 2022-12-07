IRQ
=============

Macros
------------

无

Structs
------------

无

Functions
------------

bflb_irq_initialize
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 中断控制器初始化，清除所有中断和中断 pending。

.. code-block:: c
   :linenos:

    void bflb_irq_initialize(void);

bflb_irq_save
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 关闭全局中断并保存之前的状态。

.. code-block:: c
   :linenos:

   uintptr_t bflb_irq_save(void);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - return
      - 返回关闭之前的状态

bflb_irq_restore
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 恢复关闭全局中断之前的状态。

.. code-block:: c
   :linenos:

   void bflb_irq_restore(uintptr_t flags);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - flags
      - 关闭之前的状态

bflb_irq_attach
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 注册中断入口函数。

.. code-block:: c
   :linenos:

    int bflb_irq_attach(int irq, irq_callback isr, void *arg);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - flags
      - 关闭之前的状态

bflb_irq_detach
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 取消中断函数注册。

.. code-block:: c
   :linenos:

    int bflb_irq_detach(int irq);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - flags
      - 关闭之前的状态

bflb_irq_enable
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 开启中断。

.. code-block:: c
   :linenos:

    void bflb_irq_enable(int irq);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - irq
      - 中断号

bflb_irq_disable
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 关闭中断。

.. code-block:: c
   :linenos:

    void bflb_irq_disable(int irq);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - irq
      - 中断号

bflb_irq_set_pending
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 设置中断 pending 位。

.. code-block:: c
   :linenos:

    void bflb_irq_set_pending(int irq);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - irq
      - 中断号

bflb_irq_clear_pending
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 清除中断 pending 位。

.. code-block:: c
   :linenos:

    void bflb_irq_clear_pending(int irq);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - irq
      - 中断号

bflb_irq_set_nlbits
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 设置中断分组。

.. code-block:: c
   :linenos:

    void bflb_irq_set_nlbits(uint8_t nlbits);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - irq
      - 中断号

bflb_irq_set_priority
^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 设置中断优先级。

.. code-block:: c
   :linenos:

    void bflb_irq_set_priority(int irq, uint8_t preemptprio, uint8_t subprio);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - irq
      - 中断号
    * - preemptprio
      - 抢占优先级
    * - subprio
      - 子优先级