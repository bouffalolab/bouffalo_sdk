# 无线协议栈

## 无线协议概览

Bouffalo SDK 支持多种无线协议，包括 WiFi（4/6）、BLE、Zigbee、Thread。不同芯片系列支持的协议有所不同。

## 协议支持矩阵

| 协议 | BL602/BL604 | BL702/BL704/BL706 | BL616/BL618 | BL618DG | BL616CL |
|-----|-------------|-------------------|------------|--------|--------|
| WiFi 4 | √ | - | - | - | - |
| WiFi 6 | - | - | √ | √ | - |
| BLE | √ | × | √ | √ | √ |
| Zigbee | - | × | × | × | × |
| Thread | - | × | × | × | × |

## WiFi 协议栈

### WiFi 架构

```
┌────────────────────────────────────┐
│   应用层 (examples/wifi/)          │
│   - Station模式                   │
│   - AP模式                         │
│   - AP+STA混合模式                 │
└────────────────────────────────────┘
              ↓
┌────────────────────────────────────┐
│    WiFi管理层 (wifi_mgmr/)          │
│    - 连接管理                       │
│    - 扫描                           │
│    - 网络配置                       │
└────────────────────────────────────┘
              ↓
┌────────────────────────────────────┐
│    网络抽象层 (net_al/)             │
│    - LwIP适配                       │
│    - Socket接口                    │
└────────────────────────────────────┘
              ↓
┌────────────────────────────────────┐
│    MAC层 (macsw/ - 软件MAC)          │
│    - IEEE 802.11 MAC实现           │
└────────────────────────────────────┘
              ↓
┌────────────────────────────────────┐
│  Supplicant (wpa_supplicant)       │
│    - 认证 (WPA/WPA2/WPA3)          │
│    - 密钥管理                       │
└────────────────────────────────────┘
              ↓
┌────────────────────────────────────┐
│        LwIP协议栈                   │
│    - TCP/IP协议栈                  │
│    - Socket API                    │
└────────────────────────────────────┘
```

### WiFi 4 (BL602)

**特性：**
- 标准：IEEE 802.11n
- 频段：2.4GHz
- 最大速率：72Mbps
- 加密：WPA/WPA2/WPA3
- 工作模式：Station、AP、AP+STA

**目录结构：**
```
components/wireless/wifi4/
├── wpa_supplicant/    # wpa_supplicant
├── wifi_mgmr/          # WiFi管理
├── net_al/             # 网络抽象层
└── macsw/              # 软件MAC
```

### WiFi 6 (BL616)

**特性：**
- 标准：IEEE 802.11ax
- 频段：2.4GHz
- 最大速率：1200Mbps
- 加密：WPA2/WPA3
- 工作模式：Station、AP、AP+STA

**目录结构：**
```
components/wireless/wifi6/
├── wpa_supplicant/    # wpa_supplicant v2.10
├── wifi_mgmr/          # WiFi管理
├── net_al/             # 网络抽象层
└── macsw/              # 软件MAC
```

### WiFi 使用示例

#### Station模式连接

```c
#include "wifi_mgmr.h"

void wifi_connect(char *ssid, char *password)
{
    struct wifi_mgmr_sta_config config = {
        .ssid = ssid,
        .password = password,
        .channel = 0,  // 自动扫描
    };

    wifi_mgmr_sta_connect(&config);
}

// 状态回调
void wifi_event_handler(wifi_event_t event, void *arg)
{
    switch (event) {
        case WIFI_EVENT_STA_CONNECTED:
            printf("Connected to AP\n");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            printf("Disconnected from AP\n");
            break;
    }
}

wifi_mgmr_register_event_handler(wifi_event_handler, NULL);
wifi_connect("MyWiFi", "password");
```

#### AP模式

```c
#include "wifi_mgmr.h"

void wifi_start_ap(char *ssid, char *password)
{
    struct wifi_mgmr_ap_config config = {
        .ssid = ssid,
        .password = password,
        .channel = 6,
        .max_sta = 4,
    };

    wifi_mgmr_ap_start(&config);
}

wifi_start_ap("MyAP", "12345678");
```

#### Socket通信（基于LwIP）

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

    char data[] = "Hello, World!";
    send(sock, data, strlen(data), 0);

    char buf[100];
    int len = recv(sock, buf, sizeof(buf), 0);

    close(sock);
}
```

### WiFi Coexistence (WiFi + BLE)

对于同时支持WiFi和BLE的芯片（如BL616），SDK提供coex模块：

```
WiFi_MGMR ──┐
             ├──→ COEX ──→ MACSW
NET_AL   ───┤
BT_API   ───┘
```

**配置：**
```bash
# Kconfig
CONFIG_WIFI=y
CONFIG_BLE=y
CONFIG_COEX=y
```

## BLE 协议栈

### BLE 架构

```
┌────────────────────────────────────┐
│   应用层 (examples/btble/)          │
│   - Peripheral (从机)                │
│   - Central (主机)                  │
│   - Observer (观察者)               │
└────────────────────────────────────┘
              ↓
┌────────────────────────────────────┐
│    BLE API (btble_api/)             │
│    - GAP API                       │
│    - GATT API                      │
└────────────────────────────────────┘
              ↓
┌────────────────────────────────────┐
│  Zephyr Bluetooth Stack v2.1.0     │
│    - Controller                    │
│    - Host                         │
└────────────────────────────────────┘
              ↓
┌────────────────────────────────────┐
│      BLE Controller (芯片内)        │
└────────────────────────────────────┘
```

### BLE 特性

**支持的角色：**
- **Peripheral**: 从机，可连接
- **Central**: 主机，主动连接
- **Broadcaster**: 广播，不可连接
- **Observer**: 观察者，扫描广播

**支持的配置：**
- **GAP**: 通用访问配置
- **GATT**: 通用属性配置
- **Service**: 服务定义
- **Characteristic**: 特征值

### BLE 使用示例

#### Peripheral示例

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

// 连接回调
void connected_callback(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printf("Connection failed: %d\n", err);
    } else {
        printf("Connected\n");
    }
}

// 断开回调
void disconnected_callback(struct bt_conn *conn, uint8_t reason)
{
    printf("Disconnected: reason %d\n", reason);

    // 重启广播
    bt_le_adv_start();
}

// 初始化
void ble_init(void)
{
    bt_conn_cb_register(&conn_callbacks);
    bt_gatt_service_register(&svc);
    bt_le_adv_start();
}
```

#### Central示例

```c
#include "btble_api.h"

// 扫描回调
void scan_callback(const bt_addr_le_t *addr, int8_t rssi,
                   const uint8_t *adv_data, size_t len)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    printf("Found device: %s, RSSI: %d\n", addr_str, rssi);
}

// 连接回调
void connected_callback(struct bt_conn *conn, uint8_t err)
{
    if (!err) {
        printf("Connected\n");

        // 发现服务
        bt_gatt_discover(conn, NULL);
    }
}

// 初始化
void ble_init(void)
{
    bt_conn_cb_register(&conn_callbacks);

    // 开始扫描
    bt_le_scan_start(BT_LE_SCAN_ACTIVE, scan_callback);
}

// 连接设备
void connect_device(const bt_addr_le_t *addr)
{
    struct bt_conn_le_create_param param = {
        .interval = 0x10,
        .window = 0x10,
        .interval_c = 0,
        .timeout = 400,
    };

    bt_conn_le_create(addr, &param, BT_LE_CONN_PARAM_DEFAULT,
                     connected_callback);
}
```

#### BLE CLI示例

SDK提供BLE命令行工具用于测试：

```
ble init
ble scan on
ble scan off
ble connect <addr>
ble disconnect
ble gatt discover
ble gatt read <handle>
ble gatt write <handle> <data>
```

### BLE 安全性

**配对模式：**
- Legacy Pairing (LE Secure Connections之前)
- LE Secure Connections (SC)

**认证方式：**
- Just Works (无密钥)
- Passkey Entry (6位数字)
- OOB (带外)

**配置：**
```c
struct bt_le_oob *oob_local, *oob_remote;

// Passkey配对
bt_conn_auth_passkey_entry(conn, passkey);

// OOB配对
bt_le_oob_get_local(&oob_local);
bt_conn_auth_oob(conn, oob_local, oob_remote);
```

## Zigbee 协议栈

**状态：** 部分芯片支持（BL602），但SDK中未充分验证。

**特性：**
- IEEE 802.15.4
- Zigbee协议栈
- 设备角色：Coordinator、Router、End Device

**目录：** `components/wireless/zigbee/`

## Thread 协议栈

**状态：** 部分芯片支持（BL602），但SDK中未充分验证。

**特性：**
- Thread 1.2
- 基于6LoWPAN
- 边界路由器支持

**目录：** `components/wireless/thread/`

## 无线协议选择建议

### 应用场景对比

| 场景 | 推荐 | 理由 |
|-----|------|------|
| 需要互联网访问 | WiFi | 直接连接路由器 |
| 低功耗传感器 | BLE | 低功耗、手机直接连接 |
| Mesh网络 | Zigbee/Thread | 自组网、大规模 |
| 高吞吐量 | WiFi 6 | 高速数据传输 |
| 音频流 | WiFi 6 | 低延迟、高吞吐 |
| 固件升级 | WiFi | 大文件传输 |

### 功耗对比

| 协议 | 空闲功耗 | 工作功耗 | 电池寿命 |
|-----|---------|---------|---------|
| BLE | ~5μA | ~15mA | 数年 |
| WiFi | ~20μA | ~200mA | 数周 |
| Zigbee | ~5μA | ~20mA | 数年 |
| Thread | ~5μA | ~20mA | 数年 |

### 吞吐量对比

| 协议 | 最大速率 | 典型速率 | 延迟 |
|-----|---------|---------|------|
| WiFi 4 | 72Mbps | 20Mbps | 10-100ms |
| WiFi 6 | 1200Mbps | 100Mbps | 1-10ms |
| BLE | 1Mbps | 100kbps | 10-100ms |
| Zigbee | 250kbps | 40kbps | 100-500ms |
| Thread | 250kbps | 40kbps | 100-500ms |

### 连接数量

| 协议 | 最大连接数 | 典型连接数 |
|-----|-----------|-----------|
| WiFi (AP) | 32 | 4-8 |
| BLE (Central) | 20 | 5-10 |
| BLE (Peripheral) | - | 1 |
| Zigbee | 240 | 20-50 |
| Thread | 32 | 10-20 |

## 调试和测试

### WiFi 调试

```bash
# 启用WiFi调试
menuconfig → Debug Options → Enable WiFi Debug

# 查看日志
# 打印WiFi事件、连接状态、网络信息
```

### BLE 调试

```bash
# 启用BLE调试
menuconfig → Debug Options → Enable BLE Debug

# 使用BLE CLI
# ble init
# ble scan on
# ble connect <addr>
```

### 抓包工具

**WiFi:**
- Wireshark + 捕获模式
- 内部抓包接口

**BLE:**
- nRF Sniffer
- 内部HCI日志

## 常见问题

### WiFi 连接失败

**检查项：**
1. SSID和密码是否正确
2. 频段和信道是否支持
3. 加密方式是否支持
4. 信号强度是否足够

### BLE 连接失败

**检查项：**
1. 广播参数是否正确
2. 设备是否在广播中
3. 配对模式是否正确
4. 设备是否已达最大连接数

### Coex 问题

**检查项：**
1. WiFi和BLE是否同时启用
2. Coex配置是否正确
3. 优先级设置是否合理

## 总结

Bouffalo SDK提供完整的无线协议栈支持，包括WiFi和BLE。选择协议时，需根据应用场景、功耗要求、吞吐量需求等因素综合考虑。使用时，参考examples目录中的示例代码。
