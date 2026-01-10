# 组件系统

## 组件系统概览

Bouffalo SDK 提供丰富的软件组件，包括操作系统、网络协议栈、文件系统、加密库、图形库、多媒体组件等。所有组件都可通过 Kconfig 灵活配置。

## 组件目录结构

```
components/
├── ai/                    # AI组件
├── crypto/                # 加密组件
│   └── mbedtls/          # mbedTLS
├── fota/                  # 固件OTA升级
├── fs/                    # 文件系统
│   ├── fatfs/            # FatFS
│   ├── littlefs/         # LittleFS
│   └── romfs/            # ROMFS
├── graphics/              # 图形库
│   ├── lvgl_v8/          # LVGL v8
│   └── lvgl_v9/          # LVGL v9
├── ipc/                   # 核间通信
│   ├── open-amp/         # OpenAMP
│   ├── libmetal/         # libmetal
│   └── rpmsg_service/    # RPMsg服务
├── net/                   # 网络组件
│   ├── lwip/             # LwIP协议栈
│   ├── netbus/           # 网络总线
│   └── nethub/           # 网络中心
├── os/                    # 操作系统
│   ├── freertos/         # FreeRTOS
│   └── power_mgmt/       # 电源管理
├── shell/                 # 命令行Shell
├── usb/                   # USB协议栈
│   └── cherryusb/        # CherryUSB
├── utils/                 # 工具库
│   ├── cjson/            # cJSON
│   ├── partition/        # 分区管理
│   └── [其他...]
└── wireless/              # 无线协议栈
    ├── wifi4/            # WiFi 4
    ├── wifi6/            # WiFi 6
    ├── bluetooth/        # BLE
    ├── zigbee/           # Zigbee
    └── thread/           # Thread
```

## 操作系统组件

### FreeRTOS

**特性：**
- 实时操作系统内核
- 任务调度、信号量、队列
- 内存管理
- 定时器

**使用：**
```c
#include "FreeRTOS.h"
#include "task.h"

void task1(void *pvParameters)
{
    while(1) {
        // 任务代码
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    xTaskCreate(task1, "Task1", 1024, NULL, 1, NULL);
    vTaskStartScheduler();
}
```

**配置：**
```kconfig
# Kconfig
config CONFIG_FREERTOS
    bool "Enable FreeRTOS"
    default y
```

### NuttX

**特性：**
- POSIX兼容
- 虚拟内存支持
- 文件系统
- 网络协议栈

**使用：**
```c
#include <nuttx/config.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("NuttX Hello World\n");
    return 0;
}
```

**配置：**
```kconfig
config CONFIG_NUTTX
    bool "Enable NuttX"
    default n
```

## 网络组件

### LwIP

**特性：**
- 轻量级TCP/IP协议栈
- 支持IPv4/IPv6
- Socket API
- DHCP客户端
- DNS客户端

**使用：**
```c
#include "lwip/sockets.h"

void tcp_client(void)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr("192.168.1.1");

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    char data[] = "GET / HTTP/1.1\r\n\r\n";
    send(sock, data, strlen(data), 0);

    char buf[1024];
    int len = recv(sock, buf, sizeof(buf), 0);

    close(sock);
}
```

**配置：**
```kconfig
config CONFIG_LWIP
    bool "Enable LwIP"
    default y
    select CONFIG_WIFI
```

### MQTT-C

**特性：**
- MQTT客户端库
- 支持MQTT 3.1.1
- QoS 0/1/2

**使用：**
```c
#include "mqtt.h"

void mqtt_connect(void)
{
    struct mqtt_client client;
    mqtt_init(&client, NULL);

    mqtt_connect(&client, "broker.hivemq.com", 1883);

    mqtt_publish(&client, "topic", "message", 0, 0);
}
```

## 文件系统组件

### FatFS

**特性：**
- FAT12/FAT16/FAT32
- 支持长文件名
- 可移植性强

**使用：**
```c
#include "ff.h"

FATFS fs;
FIL file;

// 挂载文件系统
f_mount(&fs, "0:", 1);

// 写入文件
f_open(&file, "test.txt", FA_WRITE | FA_CREATE_ALWAYS);
f_write(&file, "Hello", 5, &bw);
f_close(&file);

// 读取文件
f_open(&file, "test.txt", FA_READ);
f_read(&file, buf, sizeof(buf), &br);
f_close(&file);
```

**配置：**
```kconfig
config CONFIG_FATFS
    bool "Enable FatFS"
    default n
```

### LittleFS

**特性：**
- 轻量级文件系统
- 掉电保护
- 适合嵌入式Flash

**使用：**
```c
#include "lfs.h"

struct lfs_config cfg = {
    // Flash配置
    .read  = flash_read,
    .prog  = flash_prog,
    .erase = flash_erase,
    .sync  = flash_sync,
};

lfs_t lfs;
lfs_mount(&lfs, &cfg);

// 写入文件
lfs_file_t file;
lfs_file_open(&lfs, &file, "hello.txt", LFS_O_WRONLY | LFS_O_CREAT);
lfs_file_write(&lfs, &file, "Hello", 5);
lfs_file_close(&lfs, &file);
```

**配置：**
```kconfig
config CONFIG_LITTLEFS
    bool "Enable LittleFS"
    default n
```

### ROMFS

**特性：**
- 只读文件系统
- 编译时打包
- 零运行时开销

**使用：**
```bash
# 创建ROMFS
tools/genromfs/genromfs -f romfs.img -d romfs_dir

# 挂载
romfs_mount("romfs.img", "/rom");
```

## 加密组件

### mbedTLS

**特性：**
- AES、SHA、RSA
- TLS/SSL
- X.509证书

**AES加密：**
```c
#include "mbedtls/aes.h"

void aes_encrypt(void)
{
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, key, 128);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, input, output);
    mbedtls_aes_free(&aes);
}
```

**TLS连接：**
```c
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"

void tls_connect(void)
{
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ssl_setup(&ssl, &conf);

    mbedtls_ssl_set_hostname(&ssl, "example.com");

    // 连接...
    mbedtls_ssl_handshake(&ssl);

    // 发送数据
    mbedtls_ssl_write(&ssl, buf, len);

    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
}
```

**配置：**
```kconfig
config CONFIG_MBEDTLS
    bool "Enable mbedTLS"
    default y
```

## 图形库组件

### LVGL v8

**特性：**
- 轻量级图形库
- 丰富的小部件
- 支持触摸屏
- 动画和主题

**使用：**
```c
#include "lvgl.h"

void lvgl_init_example(void)
{
    lv_init();

    // 注册显示驱动
    lv_port_disp_init();

    // 创建标签
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello LVGL");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    while(1) {
        lv_task_handler();
        vTaskDelay(5);
    }
}
```

**配置：**
```kconfig
config CONFIG_LVGL_V8
    bool "Enable LVGL v8"
    default n
```

### LVGL v9

**特性：**
- 最新版本LVGL
- 更好的性能
- 新的特性

**使用：**
```c
#include "lvgl.h"

// API与v8类似，但有更新
void lvgl_v9_init(void)
{
    lv_init();
    lv_display_create(...);
    // ...
}
```

**配置：**
```kconfig
config CONFIG_LVGL_V9
    bool "Enable LVGL v9"
    default n
```

## USB组件

### CherryUSB

**特性：**
- 设备/主机模式
- 支持多种USB类
- 轻量级

**设备模式：**
```c
#include "usbh_core.h"
#include "usbd_core.h"

void usb_device_init(void)
{
    // 初始化USB设备
    usbd_init(&device);

    // 添加配置
    usbd_add_config(&device, &config);

    // 启动
    usbd_start(&device);
}
```

**主机模式：**
```c
#include "usbh_core.h"

void usb_host_init(void)
{
    // 初始化USB主机
    usbh_init(&host);

    // 启动主机
    usbh_start(&host);
}
```

**配置：**
```kconfig
config CONFIG_USB
    bool "Enable USB"
    default n
```

## 工具库组件

### cJSON

**特性：**
- 轻量级JSON解析库
- 简单易用

**使用：**
```c
#include "cJSON.h"

void json_example(void)
{
    // 创建JSON对象
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "test");
    cJSON_AddNumberToObject(root, "value", 123);

    // 序列化
    char *json_str = cJSON_Print(root);

    // 解析
    cJSON *parsed = cJSON_Parse(json_str);

    // 读取
    cJSON *name = cJSON_GetObjectItem(parsed, "name");
    printf("Name: %s\n", name->valuestring);

    // 释放
    cJSON_Delete(parsed);
    cJSON_Delete(root);
    free(json_str);
}
```

### partition

**特性：**
- 分区管理
- 支持多种分区类型
- OTA支持

**使用：**
```c
#include "partition.h"

void partition_example(void)
{
    // 获取分区信息
    partition_item_t *part = partition_get("app");

    // 读取分区
    partition_read(part, offset, buf, len);

    // 写入分区
    partition_write(part, offset, buf, len);
}
```

## 核间通信组件（IPC）

### OpenAMP + RPMsg

**特性：**
- 核间通信
- 支持多核芯片（BL808）
- 基于共享内存

**使用：**
```c
#include "openamp.h"
#include "rpmsg.h"

void rpmsg_init(void)
{
    // 初始化RPMsg
    rpmsg_dev_init();

    // 创建端点
    struct rpmsg_endpoint *ept = rpmsg_create_ept(...);

    // 发送消息
    rpmsg_send(ept, data, len);

    // 接收消息
    rpmsg_recv(ept, buf, &len);
}
```

**配置：**
```kconfig
config CONFIG_OPENAMP
    bool "Enable OpenAMP"
    depends on BL808 || BL606P
```

## Shell组件

**特性：**
- 命令行界面
- 支持自定义命令
- 调试工具

**使用：**
```c
#include "shell.h"

void cmd_test(int argc, char **argv)
{
    printf("Test command\n");
    for(int i = 0; i < argc; i++) {
        printf("Arg %d: %s\n", i, argv[i]);
    }
}

// 注册命令
SHELL_CMD_EXPORT(cmd_test, "Test command");
```

**内置命令：**
- `help`: 显示帮助
- `version`: 显示版本
- `reboot`: 重启
- `meminfo`: 内存信息

## 组件依赖管理

### Kconfig依赖

```kconfig
# WiFi依赖LwIP
config CONFIG_WIFI
    bool "Enable WiFi"
    select CONFIG_LWIP

# FATFS依赖存储
config CONFIG_FATFS
    bool "Enable FatFS"
    depends on CONFIG_FLASH || CONFIG_SDIO

# OTA依赖FatFS和HTTPS
config CONFIG_HTTPS_FOTA
    bool "Enable HTTPS FOTA"
    select CONFIG_FATFS
    select CONFIG_MBEDTLS
    select CONFIG_LWIP
```

### 组件启用流程

```
menuconfig → .config → .config.cmake → CMake → 编译
```

## 自定义组件

### 创建组件

1. **创建组件目录：**
```bash
mkdir components/my_component
mkdir components/my_component/include
mkdir components/my_component/src
```

2. **创建CMakeLists.txt：**
```cmake
cmake_minimum_required(VERSION 3.15)

add_library(my_component STATIC
    src/my_component.c
)

target_include_directories(my_component PUBLIC
    include
)
```

3. **创建Kconfig：**
```kconfig
config CONFIG_MY_COMPONENT
    bool "Enable my component"
    default y
    help
      This is my custom component
```

4. **在主Kconfig中添加：**
```kconfig
source "components/my_component/Kconfig"
```

5. **在主CMakeLists.txt中添加：**
```cmake
add_subdirectory(components/my_component)
```

## 组件最佳实践

### 1. 按需启用

```kconfig
# 只启用需要的组件
config CONFIG_WIFI
    bool "Enable WiFi"
    default n  # 默认禁用
```

### 2. 检查依赖

```kconfig
# 检查依赖
config CONFIG_MY_COMPONENT
    bool "Enable my component"
    depends on CONFIG_FREERTOS && CONFIG_WIFI
```

### 3. 使用默认值

```kconfig
# 为不同芯片设置不同默认值
config CONFIG_WIFI
    bool "Enable WiFi"
    default y if BL616 || BL602
    default n if BL702
```

### 4. 添加帮助信息

```kconfig
config CONFIG_MY_COMPONENT
    bool "Enable my component"
    default y
    help
      This component provides XXX functionality.
      Enable it if you need XXX.
```

## 总结

Bouffalo SDK提供丰富的组件库，涵盖操作系统、网络、文件系统、加密、图形等。所有组件通过Kconfig统一管理，支持灵活配置。创建自定义组件时，遵循SDK的目录结构和Kconfig规范。
