BFLOG
=============

简介
------------

- BFLOG 是一个移植简单，功能多样的多线程日志记录库
- 具有同步异步两种工作模式，异步模式如果缓冲区溢出会将最早的一条记录完整删除
- 总体三部分、记录器、定向输出、格式化器
- 一个记录器可多重定向输出,可同时输出到缓冲区、IO外设、文件、文件大小划分、文件时间划分
- 定向到文件输出，支持设定保留数量，支持按文件大小划分，支持按时间划分
- 每个定向输出可单独设定格式化器、输出等级、TAG标签过滤、输出方式、颜色
- 格式化器支持简单格式、用户自定义格式、YAML格式(实现中)、CSV格式(规划中)
- 六级日志等级控制, FATAL、ERROR、WARNING、INFO、DEBUG、TRACE
- 支持等级、TAG标签、函数、行数、文件名、TICK数、TIME、线程信息输出
- 支持等级、TAG标签过滤功能
- 可对不需要的等级、功能、标签进行裁剪，缩小代码体积

.. figure:: img/bflog_log.png
    :align: center

    log 样式

配置BFLOG相关功能
-------------------

如果需要配置BFLOG的相关功能需要在对应的工程目录下 `proj.conf` 文件中添加对应的代码，举例如下：

以下是一个不支持文件输出的 `proj.conf` 配置

.. code-block:: cmake
    :linenos:

    # 使能 BFLOG
    set(CONFIG_BFLOG 1)
    # 使能参数检查, 可不开启
    set(CONFIG_BFLOG_DEBUG 1)

此外在 `proj.conf` 配置中添加以下配置可以使用用户的 **bflog_user.h** 配置文件，配置文件默认为 **bflog_default.h**

.. code-block:: cmake
    :linenos:

    # 使能 BFLOG_USER 配置文件
    set(CONFIG_BFLOG_USER 1)

Macros
------------

BFLOG_CSI
^^^^^^^^^^^^^^^^^^^^

Control Sequence Introducer
用于控制终端

.. code-block:: c
   :linenos:

    #define BFLOG_CSI_START "\033["
    #define BFLOG_CSI_CUU   "A"
    #define BFLOG_CSI_CUD   "B"
    #define BFLOG_CSI_CUF   "C"
    #define BFLOG_CSI_CUB   "D"
    #define BFLOG_CSI_CNL   "E"
    #define BFLOG_CSI_CPL   "F"
    #define BFLOG_CSI_CHA   "G"
    #define BFLOG_CSI_CUP   "H"
    #define BFLOG_CSI_ED    "J"
    #define BFLOG_CSI_EL    "K"
    #define BFLOG_CSI_SU    "S"
    #define BFLOG_CSI_SD    "T"
    #define BFLOG_CSI_SGR   "m"

BFLOG_SGR
^^^^^^^^^^^^^^^^^^^^

Select Graphic Rendition
用于文字图形

.. code-block:: c
   :linenos:

    #define BFLOG_SGR_RESET      "0"
    #define BFLOG_SGR_BOLD       "1"
    #define BFLOG_SGR_FAINT      "2"
    #define BFLOG_SGR_ITALICS    "3"
    #define BFLOG_SGR_UNDERLINE  "4"
    #define BFLOG_SGR_BLINKS     "5"
    #define BFLOG_SGR_BLINKR     "6"
    #define BFLOG_SGR_REVERSE    "7"
    #define BFLOG_SGR_HIDE       "8"
    #define BFLOG_SGR_STRIKE     "9"
    #define BFLOG_SGR_NORMAL     "22"
    #define BFLOG_SGR_FG_BLACK   "30"
    #define BFLOG_SGR_FG_RED     "31"
    #define BFLOG_SGR_FG_GREEN   "32"
    #define BFLOG_SGR_FG_YELLOW  "33"
    #define BFLOG_SGR_FG_BLUE    "34"
    #define BFLOG_SGR_FG_MAGENTA "35"
    #define BFLOG_SGR_FG_CYAN    "36"
    #define BFLOG_SGR_FG_WHITE   "37"
    #define BFLOG_SGR_BG_BLACK   "40"
    #define BFLOG_SGR_BG_RED     "41"
    #define BFLOG_SGR_BG_GREEN   "42"
    #define BFLOG_SGR_BG_YELLOW  "43"
    #define BFLOG_SGR_BG_BLUE    "44"
    #define BFLOG_SGR_BG_MAGENTA "45"
    #define BFLOG_SGR_BG_CYAN    "46"
    #define BFLOG_SGR_BG_WHITE   "47"

BFLOG_COLOR
^^^^^^^^^^^^^^^^^^^^

一系列颜色用于配置使用

.. code-block:: c
   :linenos:

    #define BFLOG_COLOR_START BFLOG_CSI_START
    #define BFLOG_COLOR_END   BFLOG_CSI_SGR
    #define BFLOG_CLOLR_SEP   ";"
    #define BFLOG_COLOR_DEFAULT
    #define BFLOG_COLOR_RESET BFLOG_SGR_RESET BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_FG_NONE
    #define BFLOG_COLOR_FG_BLACK   BFLOG_SGR_FG_BLACK BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_FG_RED     BFLOG_SGR_FG_RED BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_FG_GREEN   BFLOG_SGR_FG_GREEN BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_FG_YELLOW  BFLOG_SGR_FG_YELLOW BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_FG_BLUE    BFLOG_SGR_FG_BLUE BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_FG_MAGENTA BFLOG_SGR_FG_MAGENTA BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_FG_CYAN    BFLOG_SGR_FG_CYAN BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_FG_WHITE   BFLOG_SGR_FG_WHITE BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_BG_NONE
    #define BFLOG_COLOR_BG_BLACK   BFLOG_SGR_BG_BLACK BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_BG_RED     BFLOG_SGR_BG_RED BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_BG_GREEN   BFLOG_SGR_BG_GREEN BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_BG_YELLOW  BFLOG_SGR_BG_YELLOW BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_BG_BLUE    BFLOG_SGR_BG_BLUE BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_BG_MAGENTA BFLOG_SGR_BG_MAGENTA BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_BG_CYAN    BFLOG_SGR_BG_CYAN BFLOG_CLOLR_SEP
    #define BFLOG_COLOR_BG_WHITE   BFLOG_SGR_BG_WHITE BFLOG_CLOLR_SEP

BFLOG_COLOR_CONTROL
^^^^^^^^^^^^^^^^^^^^

默认配置的各等级LOG颜色

.. code-block:: c
   :linenos:

    #ifndef BFLOG_COLOR_FATAL
    #define BFLOG_COLOR_FATAL BFLOG_COLOR_FG_MAGENTA BFLOG_COLOR_BG_NONE BFLOG_SGR_BLINKS
    #endif

    #ifndef BFLOG_COLOR_ERROR
    #define BFLOG_COLOR_ERROR BFLOG_COLOR_FG_RED BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
    #endif

    #ifndef BFLOG_COLOR_WARN
    #define BFLOG_COLOR_WARN BFLOG_COLOR_FG_YELLOW BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
    #endif

    #ifndef BFLOG_COLOR_INFO
    #define BFLOG_COLOR_INFO BFLOG_COLOR_FG_NONE BFLOG_COLOR_BG_NONE BFLOG_SGR_RESET
    #endif

    #ifndef BFLOG_COLOR_DEBUG
    #define BFLOG_COLOR_DEBUG BFLOG_COLOR_FG_WHITE BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
    #endif

    #ifndef BFLOG_COLOR_TRACE
    #define BFLOG_COLOR_TRACE BFLOG_COLOR_FG_WHITE BFLOG_COLOR_BG_NONE BFLOG_SGR_FAINT
    #endif

BFLOG_LEVEL_STRING
^^^^^^^^^^^^^^^^^^^^

默认配置的各等级提示信息

.. code-block:: c
   :linenos:

    #ifndef BFLOG_LEVEL_FATAL_STRING
    #define BFLOG_LEVEL_FATAL_STRING "FATL"
    #endif

    #ifndef BFLOG_LEVEL_ERROR_STRING
    #define BFLOG_LEVEL_ERROR_STRING "ERRO"
    #endif

    #ifndef BFLOG_LEVEL_WARN_STRING
    #define BFLOG_LEVEL_WARN_STRING "WARN"
    #endif

    #ifndef BFLOG_LEVEL_INFO_STRING
    #define BFLOG_LEVEL_INFO_STRING "INFO"
    #endif

    #ifndef BFLOG_LEVEL_DEBUG_STRING
    #define BFLOG_LEVEL_DEBUG_STRING "DBUG"
    #endif

    #ifndef BFLOG_LEVEL_TRACE_STRING
    #define BFLOG_LEVEL_TRACE_STRING "TRAC"
    #endif

BFLOG_LEVEL
^^^^^^^^^^^^^^^^^^^^

用于配置 recorder 和 direct 的 LOG等级

.. code-block:: c
   :linenos:

    #define BFLOG_LEVEL_FATAL 0x00 /*!< level fatal, create a panic */
    #define BFLOG_LEVEL_ERROR 0x01 /*!< level error                 */
    #define BFLOG_LEVEL_WARN  0x02 /*!< level warning               */
    #define BFLOG_LEVEL_INFO  0x03 /*!< level information           */
    #define BFLOG_LEVEL_DEBUG 0x04 /*!< level debug                 */
    #define BFLOG_LEVEL_TRACE 0x05 /*!< level trace information     */


BFLOG_FLAG
^^^^^^^^^^^^^^^^^^^^

用于配置 recorder 和 direct 的功能

.. code-block:: c
   :linenos:

    #define BFLOG_FLAG_LEVEL  ((uint8_t)0x01) /*!< supported print level     */
    #define BFLOG_FLAG_TAG    ((uint8_t)0x02) /*!< supported record tag      */
    #define BFLOG_FLAG_FUNC   ((uint8_t)0x04) /*!< supported record function */
    #define BFLOG_FLAG_LINE   ((uint8_t)0x08) /*!< supported record line     */
    #define BFLOG_FLAG_FILE   ((uint8_t)0x10) /*!< supported record file     */
    #define BFLOG_FLAG_CLK    ((uint8_t)0x20) /*!< supported record clock    */
    #define BFLOG_FLAG_TIME   ((uint8_t)0x40) /*!< supported record time     */
    #define BFLOG_FLAG_THREAD ((uint8_t)0x80) /*!< supported record thread   */


BFLOG_MODE
^^^^^^^^^^^^^^^^^^^^

用于配置 recorder 的模式

.. code-block:: c
   :linenos:

    #define BFLOG_MODE_SYNC  ((uint8_t)0x00)
    #define BFLOG_MODE_ASYNC ((uint8_t)0x01)

BFLOG_COMMAND
^^^^^^^^^^^^^^^^^^^^

用于 bflog_control 第二个参数

.. code-block:: c
   :linenos:

    #define BFLOG_CMD_FLAG           ((uint32_t)0x01)
    #define BFLOG_CMD_LEVEL          ((uint32_t)0x02)
    #define BFLOG_CMD_QUEUE_POOL     ((uint32_t)0x03)
    #define BFLOG_CMD_QUEUE_SIZE     ((uint32_t)0x04)
    #define BFLOG_CMD_QUEUE_RST      ((uint32_t)0x05)
    #define BFLOG_CMD_ENTER_CRITICAL ((uint32_t)0x06)
    #define BFLOG_CMD_EXIT_CRITICAL  ((uint32_t)0x07)
    #define BFLOG_CMD_FLUSH_NOTICE   ((uint32_t)0x08)
    #define BFLOG_CMD_MODE           ((uint32_t)0x09)

BFLOG_DIRECT_COMMAND
^^^^^^^^^^^^^^^^^^^^

用于 bflog_direct_control 第二个参数

.. code-block:: c
   :linenos:

    #define BFLOG_DIRECT_CMD_ILLEGAL ((uint32_t)0x00)
    #define BFLOG_DIRECT_CMD_LEVEL   ((uint32_t)0x02)
    #define BFLOG_DIRECT_CMD_LOCK    ((uint32_t)0x06)
    #define BFLOG_DIRECT_CMD_UNLOCK  ((uint32_t)0x07)
    #define BFLOG_DIRECT_CMD_COLOR   ((uint32_t)0x0A)

BFLOG_DIRECT_COMMAND
^^^^^^^^^^^^^^^^^^^^

用于 bflog_direct_create 第二个参数

.. code-block:: c
   :linenos:

    #define BFLOG_DIRECT_TYPE_ILLEGAL   ((uint8_t)0x00)
    #define BFLOG_DIRECT_TYPE_BUFFER    ((uint8_t)0x01)
    #define BFLOG_DIRECT_TYPE_STREAM    ((uint8_t)0x02)
    #define BFLOG_DIRECT_TYPE_FILE      ((uint8_t)0x03)
    #define BFLOG_DIRECT_TYPE_FILE_TIME ((uint8_t)0x04)
    #define BFLOG_DIRECT_TYPE_FILE_SIZE ((uint8_t)0x05)

BFLOG_DIRECT_COLOR
^^^^^^^^^^^^^^^^^^^^

用于 bflog_direct_create 第三个参数

.. code-block:: c
   :linenos:

    #define BFLOG_DIRECT_COLOR_DISABLE ((uint8_t)0)
    #define BFLOG_DIRECT_COLOR_ENABLE  ((uint8_t)1)

BFLOG_LAYOUT_TYPE
^^^^^^^^^^^^^^^^^^^^

用于 bflog_layout_create 第二个参数

.. code-block:: c
   :linenos:

    #define BFLOG_LAYOUT_TYPE_SIMPLE ((uint8_t)0)
    #define BFLOG_LAYOUT_TYPE_FORMAT ((uint8_t)1)
    #define BFLOG_LAYOUT_TYPE_YAML   ((uint8_t)2)

Port Functions
------------

bflog_clock
^^^^^^^^^^^^^^^^^^^^

获取当前cpu时钟数

.. code-block:: c
   :linenos:

    uint64_t bflog_clock(void);

bflog_time
^^^^^^^^^^^^^^^^^^^^

获取当前UTC时间戳

.. code-block:: c
   :linenos:

    uint32_t bflog_time(void);

bflog_thread
^^^^^^^^^^^^^^^^^^^^

获取当前线程名称

.. code-block:: c
   :linenos:

    char *bflog_thread(void);

Global Functions
------------

bflog_global_filter
^^^^^^^^^^^^^^^^^^^^

用于对标签过滤器进行全局的开关,会影响所有的recorder和direct

.. code-block:: c
   :linenos:

    int bflog_global_filter(void *tag_string, uint8_t enable);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - tag_string
      - 标签字符串的指针
    * - enable
      - 是否使能


Recorder Functions
------------

recorder负责收集日志, 具有illegal、ready、running、suspend四种状态
running状态可以收集日志, ready、suspend状态可以对其进行配置
除level配置操作外, 其他配置操作必须在ready、suspend下

bflog_create
^^^^^^^^^^^^^^^^^^^^

创建一个recorder, 需要定义一个bflog_t结构体并将其指针传入,定义一块内存数组用于换冲
成功返回0,失败返回-1

.. code-block:: c
   :linenos:

    int bflog_create(bflog_t *log, void *pool, uint16_t size, uint8_t mode);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - log
      - recorder 指针
    * - pool
      - 用于缓冲的数组
    * - size
      - 用户缓冲的数组大小
    * - mode
      - BFLOG_MODE

bflog_delete
^^^^^^^^^^^^^^^^^^^^

删除一个recorder
处于ready、suspend
线程安全的
成功返回0,失败返回-1

.. code-block:: c
   :linenos:

    int bflog_delete(bflog_t *log);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - log
      - recorder 指针

bflog_append
^^^^^^^^^^^^^^^^^^^^

将一个direct添加到此recorder
处于ready、suspend
线程安全的
成功返回0,失败返回-1

.. code-block:: c
   :linenos:

    int bflog_append(bflog_t *log, bflog_direct_t *direct);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - log
      - recorder 指针
    * - direct
      - direct 指针

bflog_remove
^^^^^^^^^^^^^^^^^^^^

将一个direct从recorder移除
处于ready、suspend
线程安全的
成功返回0,失败返回-1

.. code-block:: c
   :linenos:

    int bflog_remove(bflog_t *log, bflog_direct_t *direct);

.. list-table::
    :widths: 10 10
    :header-rows: 1

    * - parameter
      - description
    * - log
      - recorder 指针
    * - direct
      - direct 指针

