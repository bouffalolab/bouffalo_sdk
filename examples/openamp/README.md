# BL618DG OpenAMP 双核通信压测

本示例直接使用 `rpmsg_service_register_endpoint()` 和
`rpmsg_service_send()`，用于 AP/NP 单向吞吐和双向长期稳定性测试。

## 编译与烧录

```bash
cd examples/openamp
make
make flash FLASH_COMX=/dev/ttyUSB0
```

编译会同时生成 AP、NP 镜像和合并镜像
`build/openamp_bl618dg.bin`。Windows 串口示例为 `FLASH_COMX=COM5`。

## 串口

两个串口均使用 2000000 baud、8N1：

| 核 | 串口 | BL618DGDK 引脚 |
|---|---|---|
| AP | UART0 | TX GPIO11，RX GPIO12（QFN68 为 GPIO42） |
| NP | UART1 | TX GPIO20，RX GPIO21 |

上电后两边分别打印 `IPC_THROUGHPUT_READY`。AP 打印
`IPC_THROUGHPUT_SYNCED` 后自动开始测试。

## 自动测试

AP 和 NP 使用 1400 bytes 的数据包，按以下固定时序自动完成全部测试：

- AP 发送 10 秒，NP 接收；
- 间隔 2 秒后，NP 发送 10 秒，AP 接收；
- 再间隔 2 秒后，AP 和 NP 同时收发 48 小时。

AP 和 NP 的 `main.c` 通过 `ipc_throughput_crc_enabled` 控制软件 CRC32
校验，两个核必须使用相同配置。默认开启，校验包头和数据，并分别打印每秒和整个接收阶段的错误数。

## 手动测试命令

Shell 命令仍支持手动指定数据大小和运行秒数：

```text
ipc_throughput_test [data_size] [duration_sec]
ipc_throughput_stop
```

默认 data size 为 1400 bytes，默认运行 10 秒；`duration_sec` 为 0 时一直运行，
直到在对应核的串口执行：

```text
ipc_throughput_stop
```

## 配置说明

示例沿用参考吞吐测试的 RPMsg 配置：`RPMSG_BUFFER_SIZE=1600`、
`VRING_SIZE=4`，不修改公共 IPC 组件 API。AP 必须在 `defconfig` 中设置
`CONFIG_RPMSG_SERVICE_MODE_MASTER=y`。
