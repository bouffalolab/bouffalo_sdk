API 分层模型
=========================

简介
----

为了不影响用户应用层代码因为芯片驱动的不同而频繁修改，**bl_mcu_sdk** 的外设 API 采用了 HAL 层的思想，屏蔽了底层硬件的实现。而 HAL 层又分为两大类：

- 设备驱动管理层：提供一套标准的接口，具体实现由外设驱动适配层实现。
- 外设驱动适配层：实现设备驱动管理层的标准接口，并且拓展自己的独特的接口。

整体的代码分层框架如图所示：

.. figure:: img/api1.png
    :alt:

    code structure

- 应用层是用户自己编写的代码，接口则是调用 hal 层的接口。
- HAL 层提供给应用层接口，底层调用 std 层，不同的芯片调用不同的 std 层。
- STD 层则是对硬件寄存器进行简单封装，由外设驱动适配层进行调用。

设备驱动管理层实现
---------------------

设备驱动管理层的实现采用了面向对象的思想，首先我们将外设看成是一个设备或者是文件，秉承 **一切皆文件** 的理念，而文件又都具有标准的调用接口：``open``、``close``、``ctrl``、``write``、``read``、``callback``，不同文件类别不同（比如串口设备、ADC设备、SPI设备），并且打开的方式也不同（比如轮询、中断、DMA），由此，我们可以构建出一个对象的基类（父类）。

**基类**

.. code-block:: C

    struct device
    {
        char name[NAME_MAX];            /*name of device */
        dlist_t list;                   /*list node of device */
        enum device_status_type status; /*status of device */
        enum device_class_type type;    /*type of device */
        uint16_t oflag;                 /*oflag of device */

        int (*open)(struct device *dev, uint16_t oflag);
        int (*close)(struct device *dev);
        int (*control)(struct device *dev, int cmd, void *args);
        int (*write)(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);
        int (*read)(struct device *dev, uint32_t pos, void *buffer, uint32_t size);
        void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event);
        void *handle;
    };

**基类成员：name**

给设备取名，后面会使用 ``device_find`` 找到这个设备。

**基类成员：type**

``type`` 记录当前设备的类别，可以选择的 ``type`` 类型如下。

.. code-block:: C

    enum device_class_type
    {
        DEVICE_CLASS_NONE = 0,
        DEVICE_CLASS_GPIO,
        DEVICE_CLASS_UART,
        DEVICE_CLASS_SPI,
        DEVICE_CLASS_I2C,
        DEVICE_CLASS_ADC,
        DEVICE_CLASS_DMA,
        DEVICE_CLASS_TIMER,
        DEVICE_CLASS_PWM,
        DEVICE_CLASS_SDIO,
        DEVICE_CLASS_USB,
        DEVICE_CLASS_I2S,
        DEVICE_CLASS_CAMERA,
        DEVICE_CLASS_SEC_HASH,
    } ;

**基类成员：status**

``status`` 用来记录当前设备的状态，当前提供 4 种状态。

.. code-block:: C

    enum device_status_type
    {
        DEVICE_UNREGISTER = 0,
        DEVICE_REGISTERED,
        DEVICE_OPENED,
        DEVICE_CLOSED
    } ;

**基类成员：oflag**

``oflag`` 记录 注册时填入的 flag 信息以及使用 ``device_open`` 时填入的 ``oflag`` 信息。

**基类成员：list**

设备的增加和删除使用双向链表进行存储，节省内存。

**基类成员：标准的函数指针**

为不同的外设提供了标准的函数接口，当外设实现此类接口并赋值给该成员，便能达到重写的功能。

设备驱动管理层标准接口
-----------------------

**device_register**
^^^^^^^^^^^^^^^^^^^^

``device_register`` 用于设备的注册，将设备信息注册到链表当中。

.. code-block:: C

    int device_register(struct device *dev, const char *name, uint16_t flag);   

- dev 设备句柄。
- name 设备名称。
- flag 设备的读写属性
- return 返回错误码，0 表示注册成功，其他表示错误。

``flag`` 可以写入以下参数，表示：**只读**、**只写**、**可读可写**。

.. code-block:: C

    #define DEVICE_OFLAG_RDONLY 0x1000 /* 以只读方式打开设备 */
    #define DEVICE_OFLAG_WRONLY 0x2000 /* 以只写方式打开设备 */
    #define DEVICE_OFLAG_RDWR   0x3000 /* 以读写方式打开设备 */


**device_unregister**
^^^^^^^^^^^^^^^^^^^^^^^

``device_unregister`` 用于设备的删除，将设备信息从链表中删除。

.. code-block:: C

    int device_unregister(const char *name);   

- dev 设备句柄
- name 要删除的设备名称
- return 错误码，0 表示删除，其他表示错误

**device_find**
^^^^^^^^^^^^^^^^

``device_find`` 用于根据 ``name`` 从链表中寻找设备，并返回设备句柄的首地址。

.. code-block:: C

    struct device *device_find(const char *name);

- dev 设备句柄
- name 要查找的设备名称
- return 错误码，不为 0 表示找到的设备句柄，NULL 表示未找到该设备。


**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于设备的打开，``oflag`` 表示以何种方式打开，目前提供 6 种打开方式。底层调用 ``dev`` 句柄中的 ``open`` 成员。

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);   

- dev 设备句柄
- oflag 设备的打开方式
- return 错误码，0 表示打开成功，其他表示错误

``oflag`` 可以写入以下参数：

.. code-block:: C

    #define DEVICE_OFLAG_STREAM_TX  0x001 /* 设备以轮训发送模式打开 */
    #define DEVICE_OFLAG_STREAM_RX  0x002 /* 设备以轮训接收模式打开 */
    #define DEVICE_OFLAG_INT_TX     0x004 /* 设备以中断发送模式打开 */
    #define DEVICE_OFLAG_INT_RX     0x008 /* 设备以中断接收模式打开 */
    #define DEVICE_OFLAG_DMA_TX     0x010 /* 设备以 DMA 发送模式打开 */
    #define DEVICE_OFLAG_DMA_RX     0x020 /* 设备以 DMA 接收模式打开 */

**device_close**
^^^^^^^^^^^^^^^^

``device_close`` 用于设备的关闭。底层调用 ``dev`` 句柄中的 ``close`` 成员。

.. code-block:: C

    int device_close(struct device *dev);   

- dev 设备句柄
- return 错误码，0 表示关闭成功，其他表示错误
    
**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于根据命令对设备进行控制和参数的修改。底层调用 ``dev`` 句柄中的 ``control`` 成员。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);   

- dev 设备句柄
- cmd 设备控制命令
- args 控制参数
- return 不同的控制命令返回的意义不同。

``cmd`` 提供了以下标准命令，除此之外，不同外设还具有自己的命令

.. code-block:: C

    #define DEVICE_CTRL_SET_INT             0x01    /* set interrupt */
    #define DEVICE_CTRL_CLR_INT             0x02    /* clear interrupt */
    #define DEVICE_CTRL_GET_INT             0x03    /* get interrupt status*/
    #define DEVICE_CTRL_RESUME              0x04    /* resume device */
    #define DEVICE_CTRL_SUSPEND             0x05    /* suspend device */
    #define DEVICE_CTRL_CONFIG              0x06    /* config device */
    #define DEVICE_CTRL_GET_CONFIG          0x07    /* get device configuration */
    #define DEVICE_CTRL_ATTACH_TX_DMA       0x08
    #define DEVICE_CTRL_ATTACH_RX_DMA       0x09
    #define DEVICE_CTRL_TX_DMA_SUSPEND      0x0a
    #define DEVICE_CTRL_RX_DMA_SUSPEND      0x0b
    #define DEVICE_CTRL_TX_DMA_RESUME       0x0c
    #define DEVICE_CTRL_RX_DMA_RESUME       0x0d
    #define DEVICE_CTRL_RESVD1              0x0E
    #define DEVICE_CTRL_RESVD2              0x0F

**device_write**
^^^^^^^^^^^^^^^^

``device_write`` 用于数据的发送，发送方式根据打开方式可以是轮询、中断、dma。底层调用 ``dev`` 句柄中的 ``write`` 成员。

.. code-block:: C

    int device_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);  

- dev 设备句柄
- pos 不同的设备 pos 的意义不同
- buffer 要写入的 buffer 缓冲区
- size 要写入的长度
- return 错误码，0 表示写入成功，其他表示错误

**device_read**
^^^^^^^^^^^^^^^^

``device_read`` 用于数据的接收，接收方式根据打开方式可以是轮询、中断、dma。底层调用 ``dev`` 句柄中的 ``read`` 成员。

.. code-block:: C

    int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);  

- dev 设备句柄
- pos 不同的设备 pos 的意义不同
- buffer 要读入的 buffer 缓冲区
- size 要读入的长度
- return 错误码，0 表示读入成功，其他表示错误

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于中断回调函数的注册。底层调用 ``dev`` 句柄中的 ``callback`` 成员。

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- dev 设备句柄
- callback 要注册的中断回调函数


    * dev 设备句柄
    * args 不同外设意义不同
    * size 传输长度
    * event 中断事件类型

外设驱动适配层实现
------------------------

**子类继承父类**

不同的外设首成员为 ``struct device`` ，这就相当于父类的继承，从而可以使用子类来访问父类成员，当使用子类修改父类成员时，便拥有了子类自己的功能。实现原理是不同结构体的首地址是该结构体中首个成员的地址。

.. code-block:: C

    typedef struct xxx_device
    {
        struct device parent;

    } xxx_device_t;

**重写标准接口**

每个外设都有一个 ``xxx_register`` 函数，用来重写标准接口。

.. code-block:: C

    dev->open = xxx_open;
    dev->close = xxx_close;
    dev->control = xxx_control;
    dev->write = xxx_write;
    dev->read = xxx_read;


时钟树
=========================

简介
------------------------

博流系列芯片拥有丰富的时钟源选择，为方便用户配置，提供了时钟树配置表，不需要用户手动调用时钟设置接口，用户只需要关心最终的系统时钟和外设时钟频率即可。时钟配置表位于 ``bsp/board/xxx_board`` 目录下 ``xxx_clock_config.h`` 文件。


时钟频率获取接口
------------------------

**system_clock_get**
^^^^^^^^^^^^^^^^^^^^^^^^

``system_clock_get`` 用来获取系统时钟频率。

.. code-block:: C

    uint32_t system_clock_get(enum system_clock_type type);

- type 获取的系统时钟频率类型

``type`` 提供以下几种类型

.. code-block:: C

    enum system_clock_type
    {
        SYSTEM_CLOCK_ROOT_CLOCK = 0,
        SYSTEM_CLOCK_FCLK,
        SYSTEM_CLOCK_BCLK,
        SYSTEM_CLOCK_XCLK,
    };


**peripheral_clock_get**
^^^^^^^^^^^^^^^^^^^^^^^^

``peripheral_clock_get`` 用来获取外设时钟频率。

.. code-block:: C

    uint32_t peripheral_clock_get(enum peripheral_clock_type type);

- type 获取的外设时钟频率类型

``type`` 提供以下几种类型
    
.. code-block:: C
    
    enum peripheral_clock_type
    {
        PERIPHERAL_CLOCK_UART = 0,
        PERIPHERAL_CLOCK_SPI,
        PERIPHERAL_CLOCK_I2C,
        PERIPHERAL_CLOCK_ADC,
        PERIPHERAL_CLOCK_DAC,
        PERIPHERAL_CLOCK_I2S,
    };
    
        
GPIO 设备
=========================


简介
------------------------

GPIO 全称 General Purpose Input Output（通用输入 / 输出），博流系列芯片的 GPIO 外设主要有以下功能。

- 普通输入输出带上下拉
- 复用功能带上下拉
- 模拟功能
- 外部中断（上升沿、下降沿、高电平、低电平）
- 硬件消抖
- 驱动能力控制

bl mcu sdk 的引脚配置方式分为两种。

- GPIO 复用功能通过专门的 **pinmux table** ，用户只需要修改 table 中的相关引脚的功能，程序会自动配置这些引脚。**pinmux table** 位于 ``bsp/board/xxx_board`` 目录下 ``pinmux_config.h`` 文件。
- 通过标准的 GPIO 设备接口配置引脚，缺点是只能配置普通的输入输出和中断功能，复用功能建议还是使用 table 进行配置。

GPIO 设备接口
------------------------

**gpio_set_mode**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_set_mode`` 用来配置 gpio 的模式。

.. code-block:: C
    
    void gpio_set_mode(uint32_t pin, uint32_t mode);

- pin 要配置的引脚
- mode 要配置的引脚功能

mode 提供以下几种类型

.. code-block:: C
    
    #define GPIO_OUTPUT_MODE                        0
    #define GPIO_OUTPUT_PP_MODE                     1
    #define GPIO_OUTPUT_PD_MODE                     2
    #define GPIO_INPUT_MODE                         3
    #define GPIO_INPUT_PP_MODE                      4
    #define GPIO_INPUT_PD_MODE                      5
    #define GPIO_ASYNC_RISING_TRIGER_INT_MODE       6
    #define GPIO_ASYNC_FALLING_TRIGER_INT_MODE      7
    #define GPIO_ASYNC_HIGH_LEVEL_INT_MODE          8
    #define GPIO_ASYNC_LOW_LEVEL_INT_MODE           9
    #define GPIO_SYNC_RISING_TRIGER_INT_MODE        10
    #define GPIO_SYNC_FALLING_TRIGER_INT_MODE       11
    #define GPIO_SYNC_HIGH_LEVEL_INT_MODE           12
    #define GPIO_SYNC_LOW_LEVEL_INT_MODE            13

**gpio_write**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_write`` 设置引脚电平

.. code-block:: C
    
    void gpio_write(uint32_t pin, uint32_t value);


- pin 要设置的引脚
- value 要设置的电平
  

**gpio_toggle**
^^^^^^^^^^^^^^^^^^^^^^^^

``gpio_toggle`` 翻转引脚电平

.. code-block:: C
    
    void gpio_toggle(uint32_t pin);


- pin 要翻转的引脚


**gpio_read**
^^^^^^^^^^^^^^^^^^^^^^^^
 
``gpio_read`` 读取引脚电平

.. code-block:: C
    
    int  gpio_read(uint32_t pin);


- pin 要读取电平的引脚
- return 0 为低电平，1 为高电平

**gpio_attach_irq**
^^^^^^^^^^^^^^^^^^^^^^^^
 
``gpio_attach_irq`` 为中断引脚附加中断回调函数

.. code-block:: C
    
    void gpio_attach_irq(uint32_t pin, void (*cbFun)(void));

- pin 要附加中断回调的引脚
- cbFun 中断回调函数

**gpio_irq_enable**
^^^^^^^^^^^^^^^^^^^^^^^^
 
``gpio_irq_enable`` 读取引脚电平

.. code-block:: C
    
    void gpio_irq_enable(uint32_t pin,uint8_t enabled);

- pin 要开启或者关闭中断的引脚
- enabled 0 为关闭中断，1 为打开中断


UART 设备
=========================

简介
------------------------

UART 全称（Universal Asynchronous Receiver/Transmitter）通用异步收发传输器，博流系列芯片的 UART 外设自带可配深度 **硬件fifo** ，并且拥有 **RTO（接收超时）中断** ，在使用中断的时候，无需频繁进入中断，fifo 和 rto 的搭配能够使得 UART 的利用性更加高效，UART 设备具有以下功能。

- 轮询收发模式
- 中断收发模式
- dma 收发模式
- 自动波特率检测
- 硬件 fifo
- 特有的 rto 中断

UART 设备结构体定义
------------------------

.. code-block:: C
    
    typedef struct uart_device
    {
        struct device parent;
        uint8_t id;
        uint32_t baudrate;
        uart_databits_t databits;
        uart_stopbits_t stopbits;
        uart_parity_t parity;
        uint8_t fifo_threshold;
        void* tx_dma;
        void* rx_dma;
    } uart_device_t;

- parent 继承父类属性
- id 串口 id，0 表示串口 0，,1 表示串口 1，以此类推
- baudrate 波特率
- databits 数据位
- stopbits 停止位
- parity 校验位
- fifo_threshold fifo 阈值,不同 mcu 最大值不同
- tx_dma 附加的发送 dma 句柄
- rx_dma 附加的接收 dma 句柄

databits 提供以下类型

.. code-block:: C

    typedef enum  
    {
        UART_DATA_LEN_5 = 0,  /*!< Data length is 5 bits */
        UART_DATA_LEN_6 = 1,  /*!< Data length is 6 bits */
        UART_DATA_LEN_7 = 2,  /*!< Data length is 7 bits */
        UART_DATA_LEN_8 = 3   /*!< Data length is 8 bits */
    } uart_databits_t;

stopbits 提供以下类型

.. code-block:: C
    
    typedef enum  
    {
        UART_STOP_ONE = 0,  /*!< One stop bit */
        UART_STOP_ONE_D_FIVE = 0,  /*!< 1.5 stop bit */
        UART_STOP_TWO = 1   /*!< Two stop bits */
    } uart_stopbits_t;

``parity`` 提供以下类型

.. code-block:: C
    
    typedef enum  
    {
        UART_PAR_NONE = 0,  /*!< No parity */
        UART_PAR_ODD  = 1,  /*!< Parity bit is odd */
        UART_PAR_EVEN = 2,  /*!< Parity bit is even */
    } uart_parity_t;

UART 设备参数配置表
------------------------

每一个 UART 设备都有一个参数配置，配置文件位于 ``bsp/board/xxx_board`` 目录下 ``peripheral_config.h`` 文件。当用户打开对应设备的宏，该设备的配置才生效。例如打开宏 ``BSP_USING_UART0`` ，UART0 设备就可以进行注册和使用了。

.. code-block:: C

    uart_device_t uartx_device[UART_MAX_INDEX] =
    {
    #ifdef BSP_USING_UART0
            UART0_CONFIG,
    #endif
    #ifdef BSP_USING_UART1
            UART1_CONFIG,
    #endif
    };

    #if defined(BSP_USING_UART0)
    #ifndef UART0_CONFIG
    #define UART0_CONFIG \
    {   \
     .id = 0, \
     .baudrate = 2000000,\
     .databits = UART_DATA_LEN_8, \
     .stopbits = UART_STOP_ONE, \
     .parity = UART_PAR_NONE, \
     .fifo_threshold = 1, \
    }
    #endif
    #endif

UART 设备接口
------------------------

UART 设备接口全部遵循标准设备驱动管理层提供的接口。

**uart_register**
^^^^^^^^^^^^^^^^^^^^^^^^
 
``uart_register`` 用来注册一个 UART 设备，在注册之前需要打开对应 UART 设备的宏定义,例如 ``BSP_USING_UART0`` 方可使用 UART0 设备。注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 UART 设备。

.. code-block:: C
    
    int uart_register(enum uart_index_type index, const char *name, uint16_t flag);

- index 要注册的设备索引
- name 为注册的设备命名
- flag 默认可读可写属性

``index`` 用来选择 UART 设备配置，一个 index 对应一个 UART 设备配置，比如 ``UART0_INDEX`` 对应 UART0 配置，``index`` 有如下可选类型

.. code-block:: C
    
    enum uart_index_type
    {
    #ifdef BSP_USING_UART0
        UART0_INDEX,
    #endif
    #ifdef BSP_USING_UART1
        UART1_INDEX,
    #endif
    #ifdef BSP_USING_UART2
        UART2_INDEX,
    #endif
    #ifdef BSP_USING_UART3
        UART3_INDEX,
    #endif
    #ifdef BSP_USING_UART4
        UART4_INDEX,
    #endif
        UART_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于设备的打开，``oflag`` 表示以何种方式打开。

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);   

- dev 设备句柄
- oflag 设备的打开方式
- return 错误码，0 表示打开成功，其他表示错误

``oflag`` 可以写入以下参数：

.. code-block:: C

    #define DEVICE_OFLAG_STREAM_TX  0x001 /* 设备以轮训发送模式打开 */
    #define DEVICE_OFLAG_STREAM_RX  0x002 /* 设备以轮训接收模式打开 */
    #define DEVICE_OFLAG_INT_TX     0x004 /* 设备以中断发送模式打开 */
    #define DEVICE_OFLAG_INT_RX     0x008 /* 设备以中断接收模式打开 */
    #define DEVICE_OFLAG_DMA_TX     0x010 /* 设备以 DMA 发送模式打开 */
    #define DEVICE_OFLAG_DMA_RX     0x020 /* 设备以 DMA 接收模式打开 */

**device_close**
^^^^^^^^^^^^^^^^

``device_close`` 用于设备的关闭。

.. code-block:: C

    int device_close(struct device *dev);   

- dev 设备句柄
- return 错误码，0 表示关闭成功，其他表示错误
    
**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于根据命令对设备进行控制和参数的修改。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);   

- dev 设备句柄
- cmd 设备控制命令
- args 控制参数
- return 不同的控制命令返回的意义不同。

串口设备除了标准的控制命令，还具有自己特殊的控制命令。

.. code-block:: C

    #define DEVICE_CTRL_UART_GET_TX_FIFO        0x10
    #define DEVICE_CTRL_UART_GET_RX_FIFO        0x11

``args`` 根据不同的 ``cmd`` 传入不同，具体如下：

+---------------------------------------+--------------------------------------+
|         cmd                           |           args                       |
+=======================================+======================================+
|     DEVICE_CTRL_SET_INT               |         uart_it_type                 |
+---------------------------------------+--------------------------------------+
|     DEVICE_CTRL_CLR_INT               |         uart_it_type                 |
+---------------------------------------+--------------------------------------+
|     DEVICE_CTRL_CONFIG                |         uart_param_cfg_t             |
+---------------------------------------+--------------------------------------+
|     DEVICE_CTRL_ATTACH_TX_DMA         |         dma句柄                      |
+---------------------------------------+--------------------------------------+
|     DEVICE_CTRL_ATTACH_RX_DMA         |         dma句柄                      |
+---------------------------------------+--------------------------------------+
|     DEVICE_CTRL_UART_GET_TX_FIFO      |         缓存变量                     |
+---------------------------------------+--------------------------------------+
|     DEVICE_CTRL_UART_GET_RX_FIFO      |         缓存变量                     |
+---------------------------------------+--------------------------------------+

**device_write**
^^^^^^^^^^^^^^^^

``device_write`` 用于串口数据的发送，发送方式根据打开方式可以是轮询、中断、dma。

.. code-block:: C

    int device_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);  

- dev 设备句柄
- pos 无作用
- buffer 要写入的 buffer 缓冲区
- size 要写入的长度
- return 错误码，0 表示写入成功，其他表示错误

**device_read**
^^^^^^^^^^^^^^^^

``device_read`` 用于串口数据的接收，接收方式根据打开方式可以是轮询、中断、dma。

.. code-block:: C

    int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);  

- dev 设备句柄
- pos 无作用
- buffer 要读入的 buffer 缓冲区
- size 要读入的长度
- return 错误码，0 表示读入成功，其他表示错误

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于注册一个串口中断回调函数。

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- dev 设备句柄
- callback 要注册的中断回调函数

    - dev 设备句柄
    - args 接收发送缓冲区，数据类型为 uint8_t*
    - size 传输长度
    - event 中断事件类型    

串口设备 ``event`` 有以下类型

.. code-block:: C

    enum uart_event_type
    {
        UART_EVENT_TX_END,
        UART_EVENT_TX_FIFO,
        UART_EVENT_RX_END,
        UART_EVENT_RX_FIFO,
        UART_EVENT_RTO,
        UART_EVENT_UNKNOWN
    };

PWM设备
=========================

简介
------------------------

PWM设备接口
------------------------