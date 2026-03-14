# NetHub Virtual Channel 演示代码使用说明

## 概述

本目录包含 NetHub Virtual Channel 的演示代码，用于展示 `nethub_vchan_user_send` 和 `nethub_vchan_user_recv_register` 的使用方法。

## 设计原则

1. **单一命令入口**：所有功能通过 `vchanuser` 命令访问
2. **最小静态内存占用**：仅使用 1 字节静态内存（`g_recv_callback_registered` 标志）
3. **按需注册回调**：默认不注册任何回调，由用户通过命令触发
4. **临时内存分配**：测试时使用 `pvPortMalloc` 临时分配，测试结束立即释放

## 命令格式

```
vchanuser <subcmd> [args...]
```

## 子命令说明

### 1. 发送数据

```bash
# 发送 ASCII 字符串
vchanuser send hello

# 发送十六进制数据
vchanuser send hex 0102030405
```

### 2. 接收回调控制

```bash
# 注册接收回调
vchanuser recv on

# 注销接收回调
vchanuser recv off
```

### 3. 流量测试

```bash
# 发送 100 个 512 字节的数据包
vchanuser test flow 100 512
```

**说明**：
- 自动注册接收回调（如未注册）
- 每个数据包前 4 字节为序号（大端序）
- 每发送 100 个包显示进度
- 等待 1 秒让接收完成
- 显示收发统计结果

### 4. MTU 测试

```bash
# 测试 1024 字节 MTU
vchanuser test mtu 1024
```

**说明**：
- 自动注册接收回调（如未注册）
- 发送指定大小的测试数据
- 数据填充递增模式（0x00, 0x01, 0x02...）

### 5. 统计信息

```bash
vchanuser stats
```

显示：
- 接收回调注册状态
- 发送/接收的包数和字节数

### 6. 帮助信息

```bash
vchanuser help
```

## 使用示例

### 基本收发测试

```bash
# 1. 开启接收
vchanuser recv on

# 2. 发送测试数据
vchanuser send test123

# 3. 查看接收结果（设备端会自动打印接收到的数据）

# 4. 关闭接收
vchanuser recv off
```

### 流量测试

```bash
# 发送 1000 个 1024 字节的数据包
vchanuser test flow 1000 1024

# 查看统计
vchanuser stats
```

### MTU 探测

```bash
# 测试不同大小，找出最大 MTU
vchanuser test mtu 512
vchanuser test mtu 1024
vchanuser test mtu 1500
```

## API 使用示例

代码中演示了以下 API 的使用：

```c
// 发送数据
int ret = nethub_vchan_user_send(data_buf, data_len);

// 注册接收回调
int ret = nethub_vchan_user_recv_register(callback_fn, NULL);

// 注销接收回调（传入 NULL）
int ret = nethub_vchan_user_recv_register(NULL, NULL);
```

## 文件结构

```
virtualchan/
├── app_vchan.c       # 主实现文件
├── app_vchan.h       # 头文件
└── README.md         # 本文档
```

## 注意事项

1. 最大数据长度为 1500 字节（`NETHUB_VCHAN_MAX_DATA_LEN`）
2. 十六进制字符串必须是偶数长度（如 "0102"，不能是 "010"）
3. 流量测试时会自动注册接收回调
4. 统计数据仅在本次运行期间有效，复位后清零
