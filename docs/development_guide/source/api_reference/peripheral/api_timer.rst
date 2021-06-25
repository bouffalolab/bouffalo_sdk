TIMER 设备
=========================

简介
------------------------

TIMER 通用定时器，博流系列 MCU 中 TIMER 设备具有以下特性：

- 多种时钟来源选择
- 8-bit 时钟分频器，分频系数为 1-256
- 两组 32-bit 定时器
- 每个定时器包含三组报警值设定，可独立设定每组报警值溢出时报警
- 支持 FreeRun 模式和 PreLoad 模式
- 16-bit 看门狗定时器
- 支持写入密码保护，防止误设定造成系统异常
- 支持中断或复位两种看门狗溢出方式

TIMER 设备结构体定义
------------------------

.. code-block:: C

    typedef struct timer_device {
        struct device parent;
        uint8_t id;
        uint8_t ch;
        uint8_t clk_div;
        timer_cnt_mode cnt_mode;
        timer_pl_trig_t pl_trig_src;
    } timer_device_t;

- parent        继承父类属性
- id            定时器 id ，使能定时器 0 则 id 为 0 ，使能定时器 1 则 id 为 1 ，以此类推
- ch            timer 的通道号
- clk_div       时钟分频系数
- cnt_mode      计数模式
- pl_trig_src   比较器的触发源


``ch`` 提供以下类型

.. code-block:: C

    enum timer_index_type {
        TIMER_CH0_INDEX,
        TIMER_CH1_INDEX,
        TIMER_MAX_INDEX
    };

``cnt_mode`` 提供以下类型

.. code-block:: C

    typedef enum {
        TIMER_CNT_PRELOAD,
        TIMER_CNT_FREERUN,
    } timer_cnt_mode;

``pl_trig_src`` 提供以下类型

.. code-block:: C

    typedef enum {
        TIMER_PL_TRIG_NONE,
        TIMER_PL_TRIG_COMP0,
        TIMER_PL_TRIG_COMP1,
        TIMER_PL_TRIG_COMP2,
    } timer_pl_trig_t;

TIMER 设备参数配置表
------------------------

每一个 TIMER 设备都有一个参数配置宏,宏定义位于 ``bsp/board/xxx`` 目录下 ``peripheral_config.h`` 文件,变量定义位于 ``hal_timer.c`` 中，因此无需用户自己定义变量。当用户打开对应设备的宏，该设备的配置才生效。例如打开宏 ``BSP_USING_TIMER_CH0`` ，``TIMER_CH0_CONFIG`` 即生效，同时 ``TIMER_CH0_INDEX`` 设备就可以进行注册和使用了。

.. code-block:: C

    /*参数配置宏*/
    #if defined(BSP_USING_TIMER_CH0)
    #ifndef TIMER_CH0_CONFIG
    #define TIMER_CH0_CONFIG                    \
        {                                       \
            .id = 0,                            \
            .ch = 0,                            \
            .cnt_mode = TIMER_CNT_PRELOAD,      \
            .pl_trig_src = TIMER_PL_TRIG_COMP0, \
        }
    #endif
    #endif

    #if defined(BSP_USING_TIMER_CH1)
    #ifndef TIMER_CH1_CONFIG
    #define TIMER_CH1_CONFIG                    \
        {                                       \
            .id = 0,                            \
            .ch = 1,                            \
            .cnt_mode = TIMER_CNT_PRELOAD,      \
            .pl_trig_src = TIMER_PL_TRIG_COMP2, \
        }
    #endif
    #endif

    /*变量定义*/
    enum timer_index_type {
    #ifdef BSP_USING_TIMER_CH0
        TIMER_CH0_INDEX,
    #endif
    #ifdef BSP_USING_TIMER_CH1
        TIMER_CH1_INDEX,
    #endif
        TIMER_MAX_INDEX
    };

.. note:: 上述配置可以通过 ``TIMER_DEV(dev)->xxx`` 进行修改，只能在调用 ``device_open`` 之前使用。

TIMER 设备接口
------------------------

TIMER 设备接口全部遵循标准设备驱动管理层提供的接口。

**timer_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``timer_register`` 用来注册一个 TIMER 设备，注册该设备需要使用的标准接口，在注册之前需要打开对应 TIMER 设备的宏定义。例如定义宏 ``BSP_USING_TIMER_CH0`` 方可使用 ``TIMER_CH0_INDEX`` 设备,注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 ``TIMER_CH0_INDEX`` 设备。

.. code-block:: C

    int timer_register(enum timer_index_type index, const char *name, uint16_t flag);

- index 要注册的设备索引
- name 为注册的设备命名
- flag 默认可读可写属性

``index`` 用来选择 TIMER 设备配置，一个 index 对应一个 TIMER 设备配置，比如 ``TIMER_CH0_INDEX`` 对应 ``TIMER_CH0_CONFIG`` 配置，``index`` 有如下可选类型

.. code-block:: C

    enum timer_index_type {
    #ifdef BSP_USING_TIMER_CH0
        TIMER_CH0_INDEX,
    #endif
    #ifdef BSP_USING_TIMER_CH1
        TIMER_CH1_INDEX,
    #endif
        TIMER_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于 TIMER 设备的打开，``oflag`` 表示以何种方式打开。实际调用 ``timer_open``。

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);

- dev 设备句柄
- oflag 设备的打开方式
- return 错误码，0 表示打开成功，其他表示错误

``oflag`` 可以写入以下参数：

.. code-block:: C

    #define DEVICE_OFLAG_INT             0x01
    #define DEVICE_OFLAG_POLL            0x02
    #define DEVICE_CTRL_TIMER_CH_START   0x80
    #define DEVICE_CTRL_TIMER_CH_STOP    0x81
    #define DEVICE_CTRL_GET_MATCH_STATUS 0x82

**device_close**
^^^^^^^^^^^^^^^^

``device_close`` 用于设备的关闭。实际调用 ``timer_close``。

.. code-block:: C

    int device_close(struct device *dev);

- dev 设备句柄
- return 错误码，0 表示关闭成功，其他表示错误

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于根据命令对 TIMER 设备进行控制和参数的修改。实际调用 ``timer_control``。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- dev 设备句柄
- cmd 设备控制命令
- args 控制参数
- return 不同的控制命令返回的意义不同。

TIMER 设备除了标准的控制命令，还具有私有的控制命令。

.. code-block:: C

    #define DEVICE_CTRL_TIMER_CH_START   0x80
    #define DEVICE_CTRL_TIMER_CH_STOP    0x81
    #define DEVICE_CTRL_GET_MATCH_STATUS 0x82

``args`` 根据不同的 ``cmd`` 传入不同，具体如下：

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - NULL
      - 开启 TIMER 中断
    * - DEVICE_CTRL_CLR_INT
      - NULL
      - 关闭 TIMER 中断
    * - DEVICE_CTRL_RESUME
      - NULL
      - 开启 TIMER
    * - DEVICE_CTRL_SUSPEND
      - NULL
      - 关闭 TIMER
    * - DEVICE_CTRL_TIMER_CH_START
      - timer_user_cfg_t
      - 开启 TIMER 及其中断
    * - DEVICE_CTRL_TIMER_CH_STOP
      - timer_user_cfg_t
      - 关闭 TIMER 及其中断
    * - DEVICE_CTRL_GET_CONFIG
      - NULL
      - 获取 TIMER 当前的计数值
    * - DEVICE_CTRL_GET_MATCH_STATUS
      - uint32_t
      - 获取 TIMER 比较器的状态


**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于注册一个 TIMER 中断回调函数。

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- dev 设备句柄
- callback 要注册的中断回调函数

    - dev 设备句柄
    - args 接收发送缓冲区，数据类型为 uint8_t*
    - size 传输长度
    - event 中断事件类型

TIMER 设备 ``event`` 类型如下

.. code-block:: C

    enum timer_event_type {
        TIMER_EVENT_COMP0,
        TIMER_EVENT_COMP1,
        TIMER_EVENT_COMP2,
        TIMER_EVENT_UNKNOWN
    };