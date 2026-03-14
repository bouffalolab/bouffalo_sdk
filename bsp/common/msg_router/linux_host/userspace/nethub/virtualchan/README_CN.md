# NetHub Virtual Channel 用户指南

## 1. 概述

NetHub Virtual Channel 是承载在当前 host link 上的逻辑消息通道。

它不是独立物理接口，而是复用当前的 `SDIO / USB / SPI` 链路，把不同类型的数据拆分到不同逻辑通道中。

当前最常用的是：

- `USER`
  - 给客户或应用层传输私有数据
- `AT`
  - 给 host 控制面承载 AT 数据
- `SYSTEM`
  - 内部协调使用

对客户来说，最主要关注的是 `USER` 通道。

## 2. 数据类型

| 类型 | 值 | 用途 |
| --- | --- | --- |
| USER | `0x01` | 用户私有数据 |
| AT | `0x02` | 控制面 AT 数据 |
| SYSTEM | `0x03` | 系统内部消息 |

## 3. bringup 前提

使用 host 侧 USER Virtual Channel 前，建议确认：

1. device 侧已编入：
   - `CONFIG_NETHUB=y`
   - `CONFIG_MR_VIRTUALCHAN=y`
2. host 侧已加载 `mr_sdio.ko`
3. device 与 host 的底层链路已经正常工作

说明：

- USER 通道和 AT 通道是并列关系
- 如果 host 控制面本身走 `vchan`，它使用的是 `AT` 类型
- 客户自己的私有消息建议使用 `USER` 类型

## 4. 编译

### 4.1 跟随 host 整体编译

推荐直接使用 host 总入口：

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub
./build.sh build
```

生成产物包括：

- `output/libnethub_vchan.a`
- `output/nethub_vchan_app`

### 4.2 单独编译 virtualchan

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub/virtualchan
make
```

## 5. host 侧公共 API

头文件：

- `bsp/common/msg_router/linux_host/userspace/nethub/virtualchan/nethub_vchan.h`

### 5.1 初始化与清理

```c
int nethub_vchan_init(void);
int nethub_vchan_deinit(void);
```

### 5.2 USER 通道

```c
typedef void (*nethub_vchan_recv_callback_t)(const void *data, size_t len);

int nethub_vchan_user_send(const void *data, size_t len);
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);
```

### 5.3 可选状态查询

如果客户需要判断当前 virtual channel 是否已经 ready，可选使用：

```c
typedef struct {
    nethub_vchan_link_state_t link_state;
    nethub_vchan_host_state_t host_state;
} nethub_vchan_state_snapshot_t;

int nethub_vchan_get_state_snapshot(nethub_vchan_state_snapshot_t *snapshot);
```

常用判断：

- `link_state == NETHUB_VCHAN_LINK_UP`
  - 表示链路已可收发
- `host_state == NETHUB_VCHAN_HOST_STATE_DEVICE_RUN`
  - 表示 host 侧状态机已经和 device 完成握手

### 5.4 可选链路事件回调

如果应用希望在链路 up/down 时被动收到通知，可选注册：

```c
int nethub_vchan_register_link_event_callback(
    nethub_vchan_link_event_callback_t callback,
    void *user_data);
```

通常客户集成只需要：

- `nethub_vchan_init()`
- `nethub_vchan_user_register_callback()`
- `nethub_vchan_user_send()`
- `nethub_vchan_deinit()`

只有在应用需要更严格的 ready 判断或链路事件感知时，再额外使用状态快照和链路事件回调。

## 6. 典型使用方式

典型流程：

1. `nethub_vchan_init()`
2. `nethub_vchan_user_register_callback()`
3. `nethub_vchan_user_send()`
4. 结束时调用 `nethub_vchan_deinit()`

示例：

```c
#include <stdio.h>
#include <string.h>

#include "nethub_vchan.h"

static void user_rx_cb(const void *data, size_t len)
{
    printf("recv USER data: %.*s\n", (int)len, (const char *)data);
}

int main(void)
{
    const char *msg = "hello from host";

    if (nethub_vchan_init() != 0) {
        return -1;
    }

    nethub_vchan_user_register_callback(user_rx_cb);
    nethub_vchan_user_send(msg, strlen(msg));

    nethub_vchan_deinit();
    return 0;
}
```

## 7. 测试程序

`nethub_vchan_app` 是一个可选的调试工具，不是客户集成 USER 通道时必须依赖的组件。

如果需要快速验证链路，可执行：

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub/virtualchan
sudo ./nethub_vchan_app
```

## 8. 限制与注意事项

1. 单次消息最大长度为 `1500` 字节。
2. USER 通道是消息包语义，不是流式字节流。
3. 使用前必须保证内核模块已经加载。
4. 如果 device 侧未启用 `CONFIG_MR_VIRTUALCHAN`，host 侧 USER 通道无法工作。
5. 如果同时使用 host 控制面和 USER 通道，建议把客户私有数据固定放在 `USER` 类型，不要复用 `AT` 类型。
