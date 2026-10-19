# 示例代码

## 示例概览

Bouffalo SDK 提供丰富的示例代码，涵盖从基础功能到复杂应用的各个方面。所有示例位于 `examples/` 目录下。

## 示例目录结构

```
examples/
├── helloworld/           # 基础示例
├── peripherals/          # 外设示例
│   ├── uart/             # UART示例
│   ├── spi/              # SPI示例
│   ├── i2c/              # I2C示例
│   ├── i2s/              # I2S示例
│   ├── adc/              # ADC示例
│   ├── dac/              # DAC示例
│   ├── pwm/              # PWM示例
│   ├── timer/            # 定时器示例
│   ├── dma/              # DMA示例
│   └── gpio/             # GPIO示例
├── wifi/                 # WiFi示例
│   ├── sta/              # Station模式
│   ├── ap/               # AP模式
│   ├── ap_sta_bridge/    # AP+STA桥接
│   └── wifi_http/        # HTTP请求
├── btble/                # BLE示例
│   ├── peripheral/       # BLE外设
│   ├── central/          # BLE主机
│   └── btble_cli/        # BLE命令行
├── freertos/             # FreeRTOS示例
├── nuttx/                # NuttX示例
├── fatfs/                # FatFS示例
├── littlefs/             # LittleFS示例
├── lvgl/                 # LVGL示例
├── mbedtls/              # mbedTLS示例
├── usb/                  # USB示例
├── shell/                # Shell示例
├── pmu/                  # 电源管理示例
├── crash/                # 崩溃分析示例
├── partition/            # 分区示例
├── audio/                # 音频示例
├── dsp/                  # DSP示例
├── coex/                 # WiFi/BLE共存示例
└── [更多...]
```

## 基础示例

### helloworld

**位置：** `examples/helloworld/`

**功能：**
- 基础初始化
- printf输出
- 定时器延时

**代码：**
```c
#include "bflb_mtimer.h"

int main(void)
{
    printf("Hello, Bouffalo SDK!\n");

    while(1) {
        bflb_mtimer_delay_ms(1000);
        printf("Running...\n");
    }
}
```

**编译：**
```bash
cd examples/helloworld
make CHIP=bl616 BOARD=bl616dk
```

## 外设示例

### UART

**位置：** `examples/peripherals/uart/`

**示例：**
- `uart_poll/` - 轮询模式
- `uart_fifo_interrupt/` - FIFO中断模式
- `uart_dma/` - DMA模式

**poll示例：**
```c
#include "bflb_uart.h"

int main(void)
{
    // 配置UART
    struct bflb_uart_config cfg = {
        .baudrate = 115200,
        .data_bits = 8,
        .stop_bits = 1
    };
    bflb_device_t *uart = bflb_uart_init(0, &cfg);

    // 发送
    bflb_uart_write(uart, "Hello UART", 11);

    // 接收
    uint8_t buf[100];
    int len = bflb_uart_read(uart, buf, sizeof(buf));

    while(1);
}
```

### SPI

**位置：** `examples/peripherals/spi/`

**示例：**
- `spi_flash/` - SPI Flash读写
- `spi_sensor/` - SPI传感器通信

**Flash示例：**
```c
#include "bflb_spi.h"

int main(void)
{
    struct bflb_spi_config cfg = {
        .clk_freq = 10000000,
        .mode = SPI_MODE_0
    };
    bflb_device_t *spi = bflb_spi_init(0, &cfg);

    // 发送命令
    uint8_t cmd[] = {0x05};  // READ STATUS
    uint8_t status[1];
    bflb_spi_transfer(spi, cmd, status, 1);

    printf("Status: 0x%02X\n", status[0]);

    while(1);
}
```

### I2C

**位置：** `examples/peripherals/i2c/`

**示例：**
- `i2c_eeprom/` - EEPROM读写
- `i2c_sensor/` - I2C传感器

**EEPROM示例：**
```c
#include "bflb_i2c.h"

int main(void)
{
    struct bflb_i2c_config cfg = {
        .clk_freq = 400000,
        .addr_bits = 7
    };
    bflb_device_t *i2c = bflb_i2c_init(0, &cfg);

    // 写入
    uint8_t data[] = {0x10, 0x20};
    bflb_i2c_write(i2c, 0x50, data, 2);

    // 读取
    uint8_t buf[10];
    bflb_i2c_read(i2c, 0x50, buf, sizeof(buf));

    printf("Read: 0x%02X\n", buf[0]);

    while(1);
}
```

### ADC

**位置：** `examples/peripherals/adc/`

**示例：**
- `adc_poll/` - 轮询模式
- `adc_interrupt/` - 中断模式
- `adc_dma/` - DMA模式

**poll示例：**
```c
#include "bflb_adc.h"

int main(void)
{
    struct bflb_adc_config cfg = {
        .clk_freq = 2000000,
        .vref = ADC_VREF_3P3V
    };
    bflb_device_t *adc = bflb_adc_init(0, &cfg);

    while(1) {
        uint16_t value = bflb_adc_read(adc, ADC_CHANNEL_0);
        float voltage = bflb_adc_parse_result(adc, value);
        printf("ADC: %.2fV\n", voltage);

        bflb_mtimer_delay_ms(1000);
    }
}
```

### PWM

**位置：** `examples/peripherals/pwm/`

**示例：**
- `pwm_v1/` - PWM v1 (BL602/BL702)
- `pwm_v2/` - PWM v2 (BL616/BL618DG)

**PWM v2示例：**
```c
#include "bflb_pwm_v2.h"

int main(void)
{
    struct bflb_pwm_v2_config cfg = {
        .clk_freq = 1000000,
        .period = 1000  // 1kHz
    };
    bflb_device_t *pwm = bflb_pwm_v2_init(0, &cfg);

    bflb_pwm_v2_set_duty(pwm, 0, 500);  // 50%
    bflb_pwm_v2_start(pwm);

    while(1) {
        // 动态调整占空比
        for(int i = 0; i <= 1000; i += 10) {
            bflb_pwm_v2_set_duty(pwm, 0, i);
            bflb_mtimer_delay_ms(10);
        }
    }
}
```

## WiFi 示例

### Station模式

**位置：** `examples/wifi/sta/`

**功能：**
- 连接AP
- DHCP获取IP
- Ping测试

**代码：**
```c
#include "wifi_mgmr.h"

void wifi_connect(char *ssid, char *password)
{
    struct wifi_mgmr_sta_config config = {
        .ssid = ssid,
        .password = password,
        .channel = 0
    };
    wifi_mgmr_sta_connect(&config);
}

void wifi_event_handler(wifi_event_t event, void *arg)
{
    switch(event) {
        case WIFI_EVENT_STA_CONNECTED:
            printf("Connected\n");
            break;
        case WIFI_EVENT_STA_GOT_IP:
            printf("Got IP\n");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            printf("Disconnected\n");
            break;
    }
}

int main(void)
{
    wifi_mgmr_register_event_handler(wifi_event_handler, NULL);
    wifi_connect("MyWiFi", "password");

    while(1);
}
```

### AP模式

**位置：** `examples/wifi/ap/`

**功能：**
- 启动AP
- DHCP服务
- 管理连接

**代码：**
```c
#include "wifi_mgmr.h"

void wifi_start_ap(char *ssid, char *password)
{
    struct wifi_mgmr_ap_config config = {
        .ssid = ssid,
        .password = password,
        .channel = 6,
        .max_sta = 4
    };
    wifi_mgmr_ap_start(&config);
}

int main(void)
{
    wifi_start_ap("MyAP", "12345678");

    while(1);
}
```

### HTTP请求

**位置：** `examples/wifi/wifi_http/`

**功能：**
- 连接WiFi
- HTTP GET请求
- 解析响应

**代码：**
```c
#include "lwip/sockets.h"

void http_get(const char *host, const char *path)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr(host);

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    char request[256];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);
    send(sock, request, strlen(request), 0);

    char response[1024];
    int len = recv(sock, response, sizeof(response), 0);
    response[len] = '\0';

    printf("Response:\n%s\n", response);

    close(sock);
}

int main(void)
{
    // 连接WiFi...
    http_get("192.168.1.1", "/");

    while(1);
}
```

## BLE 示例

### Peripheral (外设)

**位置：** `examples/btble/peripheral/`

**功能：**
- BLE广播
- GATT服务
- 特征值读写

**代码：**
```c
#include "btble_api.h"

// 定义服务
static struct bt_gatt_service svc = {
    .uuid = BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(...)),
    .chars = (struct bt_gatt_chrc[]){
        {
            .uuid = BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(...)),
            .value = &value,
            .read_cb = char_read_cb,
            .write_cb = char_write_cb,
        },
        { }
    },
};

void connected_callback(struct bt_conn *conn, uint8_t err)
{
    if (!err) {
        printf("Connected\n");
    }
}

void disconnected_callback(struct bt_conn *conn, uint8_t reason)
{
    printf("Disconnected: %d\n", reason);
    // 重启广播
    set_adv_enable(true);
}

int main(void)
{
    bt_conn_cb_register(&conn_callbacks);
    bt_gatt_service_register(&svc);
    set_adv_enable(true);

    while(1);
}
```

### Central (主机)

**位置：** `examples/btble/central/`

**功能：**
- BLE扫描
- 连接设备
- 读写GATT

**代码：**
```c
#include "btble_api.h"

void scan_callback(const bt_addr_le_t *addr, int8_t rssi,
                   const uint8_t *adv_data, size_t len)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    printf("Found: %s, RSSI: %d\n", addr_str, rssi);
}

void connected_callback(struct bt_conn *conn, uint8_t err)
{
    if (!err) {
        printf("Connected\n");
        bt_gatt_discover(conn, NULL);
    }
}

int main(void)
{
    bt_conn_cb_register(&conn_callbacks);
    bt_le_scan_start(BT_LE_SCAN_ACTIVE, scan_callback);

    while(1);
}
```

## 文件系统示例

### FatFS

**位置：** `examples/fatfs/`

**功能：**
- FatFS初始化
- 文件读写
- 目录遍历

**代码：**
```c
#include "ff.h"

FATFS fs;
FIL file;

int main(void)
{
    // 挂载文件系统
    f_mount(&fs, "0:", 1);

    // 写入文件
    f_open(&file, "test.txt", FA_WRITE | FA_CREATE_ALWAYS);
    f_write(&file, "Hello FatFS", 12, &bw);
    f_close(&file);

    // 读取文件
    f_open(&file, "test.txt", FA_READ);
    f_read(&file, buf, sizeof(buf), &br);
    f_close(&file);

    // 遍历目录
    DIR dir;
    FILINFO fno;
    f_opendir(&dir, "/");
    while(f_readdir(&dir, &fno) == FR_OK && fno.fname[0]) {
        printf("%s\n", fno.fname);
    }
    f_closedir(&dir);

    while(1);
}
```

### LittleFS

**位置：** `examples/littlefs/`

**功能：**
- LittleFS初始化
- 文件读写
- 掉电保护

**代码：**
```c
#include "lfs.h"

int main(void)
{
    // 初始化LittleFS
    lfs_mount(&lfs, &cfg);

    // 写入文件
    lfs_file_open(&lfs, &file, "hello.txt",
                  LFS_O_WRONLY | LFS_O_CREAT);
    lfs_file_write(&lfs, &file, "Hello", 5);
    lfs_file_close(&lfs, &file);

    // 读取文件
    lfs_file_open(&lfs, &file, "hello.txt", LFS_O_RDONLY);
    lfs_file_read(&lfs, &file, buf, 5);
    lfs_file_close(&lfs, &file);

    while(1);
}
```

## FreeRTOS 示例

**位置：** `examples/freertos/`

**功能：**
- 任务创建
- 信号量
- 队列
- 定时器

**代码：**
```c
#include "FreeRTOS.h"
#include "task.h"

void task1(void *pvParameters)
{
    while(1) {
        printf("Task 1\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task2(void *pvParameters)
{
    while(1) {
        printf("Task 2\n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

int main(void)
{
    xTaskCreate(task1, "Task1", 1024, NULL, 1, NULL);
    xTaskCreate(task2, "Task2", 1024, NULL, 2, NULL);

    vTaskStartScheduler();

    while(1);
}
```

## LVGL 示例

**位置：** `examples/lvgl/`

**功能：**
- LVGL初始化
- 控件使用
- 触摸支持

**代码：**
```c
#include "lvgl.h"

int main(void)
{
    lv_init();
    lv_port_disp_init();

    // 创建标签
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello LVGL");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    // 创建按钮
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 100);

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Button");
    lv_obj_center(btn_label);

    while(1) {
        lv_task_handler();
        vTaskDelay(5);
    }
}
```

## 电源管理示例

**位置：** `examples/pmu/`

**功能：**
- 浅睡眠
- 深度睡眠
- 唤醒源

**代码：**
```c
#include "bflb_pm.h"

void pmu_test(void)
{
    printf("Entering shallow sleep...\n");
    bflb_pm_shallow_sleep(1000);
    printf("Woke up from shallow sleep\n");

    printf("Entering deep sleep...\n");
    bflb_pm_deep_sleep();
    printf("Woke up from deep sleep\n");
}

int main(void)
{
    while(1) {
        pmu_test();
        bflb_mtimer_delay_ms(5000);
    }
}
```

## 使用示例的步骤

### 1. 选择示例

```bash
cd examples/helloworld
```

### 2. 配置项目

```bash
make menuconfig
```

### 3. 编译

```bash
make CHIP=bl616 BOARD=bl616dk
```

### 4. 烧录

```bash
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

### 5. 运行

```bash
# 查看串口输出
minicom -D /dev/ttyUSB0 -b 2000000
```

## 基于示例创建项目

### 步骤

1. **复制示例：**
```bash
cp -r examples/helloworld my_project
cd my_project
```

2. **修改代码：**
```c
// 编辑main.c
// 添加你的代码
```

3. **配置项目：**
```bash
make menuconfig
```

4. **编译：**
```bash
make CHIP=bl616 BOARD=bl616dk
```

## 示例最佳实践

### 1. 从简单开始

先从helloworld开始，理解基本流程，再尝试复杂示例。

### 2. 逐个功能测试

不要一次启用所有功能，逐个测试每个外设。

### 3. 参考官方示例

参考examples中的官方示例，而不是自己从头开始。

### 4. 修改示例

基于示例修改，而不是完全重写。

### 5. 查看配置

注意每个示例的Kconfig和defconfig配置。

## 总结

Bouffalo SDK提供丰富的示例代码，涵盖基础功能到复杂应用。从helloworld开始，逐步学习和理解SDK的使用。基于示例创建项目，可以快速开始开发。
