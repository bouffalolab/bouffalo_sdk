RTC
=============

Macros
------------

BFLB_RTC_SEC2TIME
^^^^^^^^^^^^^^^^^^^^^^^

将秒转换成 RTC 时间

BFLB_RTC_TIME2SEC
^^^^^^^^^^^^^^^^^^^^^^^

将 RTC 时间转换成秒

Structs
------------

无

Functions
------------

bflb_rtc_set_time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 设置 RTC 时间。

.. code-block:: c
   :linenos:

    void bflb_rtc_set_time(struct bflb_device_s *dev, uint64_t time);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - time
      - RTC 时间(单位: 1/32768 s)

bflb_rtc_get_time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

说明: 获取 RTC 时间。

.. code-block:: c
   :linenos:

    uint64_t bflb_rtc_get_time(struct bflb_device_s *dev);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - dev
      - 设备句柄
    * - return
      - RTC 时间(单位: 1/32768 s)

