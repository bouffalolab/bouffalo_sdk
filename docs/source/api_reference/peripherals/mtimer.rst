MTIMER
=============

Macros
------------

无

Structs
------------

无

Functions
------------

bflb_mtimer_config
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: mtimer 定时配置。

.. code-block:: c
   :linenos:

   void bflb_mtimer_config(uint64_t ticks, void (*interruptfun)(void));

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - ticks
      - 定时 tick 数
    * - interruptfun
      - 中断回调

bflb_mtimer_get_freq
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取 mtimer 频率。默认 mtimer 经过分频后设置成了 1M，所以频率就是 1M。

.. code-block:: c
   :linenos:

   uint32_t bflb_mtimer_get_freq(void);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - return
      - 频率

bflb_mtimer_delay_ms
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: mtimer 毫秒延时。

.. code-block:: c
   :linenos:

   void bflb_mtimer_delay_ms(uint32_t time);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - time
      - 延时时间

bflb_mtimer_delay_us
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: mtimer 微妙延时。

.. code-block:: c
   :linenos:

   void bflb_mtimer_delay_us(uint32_t time);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - time
      - 延时时间

bflb_mtimer_get_time_us
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取 mtimer 当前时间，us 为单位。

.. code-block:: c
   :linenos:

   uint64_t bflb_mtimer_get_time_us();

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - return
      - 当前时间

bflb_mtimer_get_time_ms
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取 mtimer 当前时间，ms 为单位。

.. code-block:: c
   :linenos:

   uint64_t bflb_mtimer_get_time_ms();

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - return
      - 当前时间
