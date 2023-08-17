IR - rc5
====================

本 demo 主要介绍 IR 以 rc5 协议收发数据。

硬件连接
-----------------------------

本 demo 使用到的 gpio 参考 ``board_ir_gpio_init`` ，将红外发射二极管和接收头与 IR 引脚连接，具体连接方式如下表（以BL808为例）：

.. table:: 硬件连接
    :widths: 50, 50
    :width: 80%
    :align: center

    +-------------------+----------------------+
    | 开发板 IR 引脚    | 外接模块             |
    +===================+======================+
    | VCC               | 红外接收头 VCC       |
    +-------------------+----------------------+
    | GND               | 红外接收头 GND       |
    +-------------------+----------------------+
    | RX(GPIO17)        | 红外接收头 OUT       |
    +-------------------+----------------------+
    | VCC               | 红外发射二极管正极   |
    +-------------------+----------------------+
    | TX(GPIO11)        | 红外发射二极管负极   |
    +-------------------+----------------------+

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/ir/ir_rc5**

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 IR 时钟，并选择 IR 时钟源和分频。

.. code-block:: C
    :linenos:

    board_ir_gpio_init();

- 配置相关引脚为 `IR` 功能

.. code-block:: C
    :linenos:

    uint32_t tx_buffer[1] = { 0x123D };
    struct bflb_ir_tx_config_s tx_cfg;

    irtx = bflb_device_get_by_name("irtx");

    /* TX init */
    tx_cfg.tx_mode = IR_TX_RC5;
    bflb_ir_tx_init(irtx, &tx_cfg);

- 获取 `irtx` 句柄
- 设置 tx_mode 为 RC5 模式，调用 ``bflb_ir_tx_init(irtx, &tx_cfg)`` 初始化 ir tx

.. code-block:: C
    :linenos:

    uint64_t rx_data;
    uint8_t rx_len;
    struct bflb_ir_rx_config_s rx_cfg;

    irrx = bflb_device_get_by_name("irrx");

    /* RX init */
    rx_cfg.rx_mode = IR_RX_RC5;
    rx_cfg.input_inverse = true;
    rx_cfg.deglitch_enable = false;
    bflb_ir_rx_init(irrx, &rx_cfg);

    /* Enable rx, wait for sending */
    bflb_ir_rx_enable(irrx, true);

- 获取 `irrx` 句柄
- 设置 rx_mode 为 RC5 模式，调用 ``bflb_ir_rx_init(irrx, &rx_cfg)`` 初始化 ir rx
- 调用 ``bflb_ir_rx_enable(irrx, true)`` 使能 ir rx，等待数据发送

.. code-block:: C
    :linenos:

    bflb_ir_send(irtx, tx_buffer, 1);
    rx_len = bflb_ir_receive(irrx, &rx_data);

- 调用 ``bflb_ir_send(irtx, tx_buffer, 1)`` 发送 tx_buffer 中的数据
- 调用 ``bflb_ir_receive(irrx, &rx_data)`` 将接收到的数据存放在 rx_data 中

.. code-block:: C
    :linenos:

    /* Check data received */
    if (rx_data != tx_buffer[0]) {
        printf("Data error! receive bit: %d, value: 0x%016lx\r\n", rx_len, rx_data);
    } else {
        printf("Received correctly. receive bit: %d, value: 0x%016lx\r\n", rx_len, rx_data);
    }

- 检查发送和接收的数据是否一致

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------

按下开发板中 RST 按键，串口打印接收到的数据。
