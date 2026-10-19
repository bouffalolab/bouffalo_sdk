I2C - eeprom
====================

本 demo 主要介绍 I2C 读写 eeprom。

硬件连接
-----------------------------

本 demo 使用到的 gpio 参考 ``board_i2c0_gpio_init`` ，将 eeprom 模块与开发板连接，具体引脚连接方式如下表（以BL616为例）：

.. table:: 硬件连接
    :widths: 50, 50
    :width: 80%
    :align: center

    +-------------------+------------------+
    | 开发板 I2C 引脚   | eeprom 模块      |
    +===================+==================+
    | SCL(GPIO14)       | SCL              |
    +-------------------+------------------+
    | SDA(GPIO15)       | SDA              |
    +-------------------+------------------+
    | GND               | GND              |
    +-------------------+------------------+
    | VCC               | VCC              |
    +-------------------+------------------+

软件实现
-----------------------------

更详细的代码请参考 **examples/peripherals/i2c/i2c_eeprom**

.. code-block:: C
    :linenos:

    board_init();

- ``board_init`` 中会开启 I2C IP 时钟，并选择 I2C 时钟源和分频。

.. code-block:: C
    :linenos:

    board_i2c0_gpio_init();

- 配置相关引脚为 `I2C` 功能

.. code-block:: C
    :linenos:

    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 400000);

- 获取 `i2c0` 句柄，并初始化 i2c0 频率为 400K

.. code-block:: C
    :linenos:

    struct bflb_i2c_msg_s msgs[2];
    uint8_t subaddr[2] = { 0x00, EEPROM_SELECT_PAGE0};
    uint8_t write_data[256];

    /* Write and read buffer init */
    for (size_t i = 0; i < 256; i++) {
        write_data[i] = i;
        read_data[i] = 0;
    }

    /* Write page 0 */
    msgs[0].addr = 0x50;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = subaddr;
    msgs[0].length = 2;

    msgs[1].addr = 0x50;
    msgs[1].flags = 0;
    msgs[1].buffer = write_data;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;

    bflb_i2c_transfer(i2c0, msgs, 2);

- 初始化发送数据(write_data)，接收buffer和配置从设备信息(msgs)
- ``bflb_i2c_transfer(i2c0, msgs, 2)`` 开启 i2c 传输

.. code-block:: C
    :linenos:

    uint8_t read_data[256];

    /* Read page 0 */
    msgs[1].addr = 0x50;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = read_data;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;
    bflb_i2c_transfer(i2c0, msgs, 2);

- 读取从设备寄存器地址中的数据，存放至 read_data 中
- ``bflb_i2c_transfer(i2c0, msgs, 2)`` 开启 i2c 传输

.. code-block:: C
    :linenos:

    /* Check read data */
    for (uint8_t i = 0; i < EEPROM_TRANSFER_LENGTH; i++) {
        if (write_data[i] != read_data[i]) {
            printf("check fail, %d write: %02x, read: %02x\r\n", i, write_data[i], read_data[i]);
        }
    }

- 检查发送和读取的数据是否一致

编译和烧录
-----------------------------

参考 :ref:`get_started`

实验现象
-----------------------------

按下 RST 按键，数据传输完成后，打印“write over”，“read over”和“check over”。