# NetHub VirtualChannel

NetHub 虚拟通道，基于 Netlink 机制实现用户空间与内核空间之间的高效数据传输。

## 功能特性

- **多种数据类型**：支持 USER、AT、SYSTEM 三种数据通道
- **消息包传输**：以消息包为单位，底层接收完整的数据包（非流式）
- **最大数据长度**：单次传输最大 1500 字节
- **回调机制**：支持注册数据接收回调函数
- **用户态 API**：提供简洁的 C 语言接口

## 数据类型

| 类型 | 值 | 用途 |
|------|-----|------|
| USER | 0x01 | 用户私有数据 |
| AT | 0x02 | WiFi 相关数据/AT 命令 |
| SYSTEM | 0x03 | 系统相关数据 |

## 快速开始

### 编译

```bash
cd virtualchan/
make
```

编译产物：`nethub_vchan_app`

### 运行测试程序

```bash
# 需要加载内核模块 mr_sdio.ko
sudo ./nethub_vchan_app
```

### 命令说明

测试程序支持以下命令：

| 命令 | 说明 | 示例 |
|------|------|------|
| `send <type> <data>` | 发送数据 | `send user hello` |
| `test mtu` | MTU 测试 | `test mtu` |
| `status` | 查看状态 | `status` |
| `help` | 显示帮助 | `help` |
| `quit/exit` | 退出程序 | `quit` |

**数据类型参数**：
- `user` - USER 类型数据
- `at` - AT 类型数据
- `system` - SYSTEM 类型数据

### 使用示例

```bash
# 发送 USER 类型数据
nethub_vchan> send user test_message_123

# 查看状态
nethub_vchan> status
Virtual Channel Status: Active
USER callback: registered
AT callback: not registered
SYSTEM callback: not registered

# MTU 测试
nethub_vchan> test mtu
Testing maximum transfer unit...
MTU test completed: max size=1500 bytes
```

## API 接口

### 初始化与清理

```c
/* 初始化虚拟通道 */
int nethub_vchan_init(void);

/* 清理虚拟通道 */
int nethub_vchan_deinit(void);
```

### 数据发送

```c
/* 发送数据（通用接口） */
int nethub_vchan_send(uint8_t type, const void *data, size_t len);

/* 发送 USER 类型数据（便捷接口） */
int nethub_vchan_user_send(const void *data, size_t len);

/* 发送 AT 类型数据（便捷接口） */
int nethub_vchan_at_send(const void *data, size_t len);
```

### 回调注册

```c
/* 接收回调函数类型 */
typedef void (*nethub_vchan_recv_callback_t)(const void *data, size_t len);

/* 注册数据接收回调（通用接口） */
int nethub_vchan_register_callback(uint8_t data_type,
                                    nethub_vchan_recv_callback_t callback);

/* 注册 USER 类型回调（便捷接口） */
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);

/* 注册 AT 类型回调（便捷接口） */
int nethub_vchan_at_register_callback(nethub_vchan_recv_callback_t callback);

/* 注销回调 */
int nethub_vchan_unregister_callback(uint8_t data_type);
```

### 代码示例

```c
#include "nethub_vchan.h"

/* 数据接收回调函数 */
void user_data_callback(const void *data, size_t len)
{
    printf("Received USER data: %.*s\n", (int)len, (char *)data);
}

int main(void)
{
    /* 初始化虚拟通道 */
    if (nethub_vchan_init() != 0) {
        fprintf(stderr, "Failed to initialize virtual channel\n");
        return -1;
    }

    /* 注册 USER 数据接收回调 */
    nethub_vchan_user_register_callback(user_data_callback);

    /* 发送 USER 数据 */
    const char *msg = "Hello from userspace!";
    nethub_vchan_user_send(msg, strlen(msg));

    /* 等待数据... */
    sleep(1);

    /* 清理 */
    nethub_vchan_deinit();
    return 0;
}
```

## 编译选项

### 启用 Readline 支持

如果系统安装了 readline 库，可以启用命令自动补全和历史记录功能：

```bash
make HAVE_READLINE=1
```

安装 readline：
```bash
# Ubuntu/Debian
sudo apt-get install libreadline-dev

# CentOS/RHEL
sudo yum install readline-devel
```

### 清理编译产物

```bash
make clean
```

## 目录结构

```
virtualchan/
├── nethub_vchan.h          # 虚拟通道头文件（API 接口）
├── nethub_vchan.c          # 虚拟通道核心实现
├── nethub_vchan_app.c      # 测试应用程序
├── Makefile                # 编译配置
└── README_CN.md            # 本文档
```

## 错误码

| 错误码 | 宏定义 | 说明 |
|--------|--------|------|
| 0 | NETHUB_VCHAN_OK | 成功 |
| -1 | NETHUB_VCHAN_ERROR | 通用错误 |
| -2 | NETHUB_VCHAN_ERROR_INIT | 初始化失败 |
| -3 | NETHUB_VCHAN_ERROR_NOT_INIT | 未初始化 |
| -4 | NETHUB_VCHAN_ERROR_PARAM | 参数错误 |
| -5 | NETHUB_VCHAN_ERROR_BUSY | 忙碌状态 |
| -6 | NETHUB_VCHAN_ERROR_NOMEM | 内存不足 |
| -7 | NETHUB_VCHAN_ERROR_IO | IO 错误 |

## 依赖

- **内核模块**：需要加载 `mr_sdio.ko` 内核驱动
- **系统库**：
  - 标准库：libc、pthread
  - 可选：libreadline（用于命令行增强）

## 限制与注意事项

1. **数据长度限制**：单次传输最大 1500 字节
2. **需要 root 权限**：Netlink 通信需要特权用户
3. **依赖内核模块**：必须先加载 `mr_sdio.ko`
4. **消息包完整性**：保证按包接收，不会出现半包或粘包

## 应用场景

- **私有数据通道**：用户自定义数据传输
- **AT 命令通道**：WiFi 配置和控制
- **系统消息通道**：内核态与用户态信息交换
