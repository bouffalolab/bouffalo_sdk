# RF OTA 测试固件

该示例基于 `wifi_tcp`，用于给射频厂商编译 OTA 信号辐射测试固件。保留 Wi-Fi CLI、iperf、TCP client/server 功能，并增加 STA 配置持久化与自动重连。

## 支持芯片

与 `wifi_tcp` 一致：BL602、BL616、BL618、BL618DG。

## 行为

- 串口控制台波特率固定为 2000000。
- 不内置 SSID 或密码；首次上电且 PSM 分区中没有有效 AP 配置时，不发起连接，等待串口命令。
- 使用 `wifi_sta_connect` 成功连接后，将当前 SSID 和密码保存到 PSM 分区的 LittleFS。
- 后续重启会使用保存的参数自动连接，并由 Wi-Fi manager 自动处理测试过程中的异常断开重连。
- 手动成功连接新的 AP 后，会覆盖原有保存配置。
- 执行 `wifi_sta_forget` 会删除已保存的 AP 配置、关闭自动重连并断开当前 STA，之后可手动连接其他 AP。

## 编译

BL616C 使用 BL616 工程目标：

```bash
make CHIP=bl616 BOARD=bl616dk
```

其他芯片沿用 `wifi_tcp` 的参数：

```bash
make CHIP=bl602 BOARD=bl602dk
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CONFIG_ROMAPI=n
```

BL618 请按实际开发板指定 `BOARD`。

## 烧录

```bash
make flash CHIP=bl616 COMX=/dev/ttyACM0
```

## 测试

首次连接测试 AP：

```text
wifi_sta_connect vela 12345678
```

连接成功后重启开发板，固件会自动连接 `vela`。可使用以下命令检查网络状态和执行辐射测试流量：

```text
wifi_sta_info
wifi_iperf -c <server-ip>
```

切换路由器：

```text
wifi_sta_forget
wifi_sta_connect <ssid> <password>
```

TCP 测试命令与 `wifi_tcp` 相同：

```text
wifi_tcp_test <server-ip> <port>
wifi_tcp_echo_test <port>
```
