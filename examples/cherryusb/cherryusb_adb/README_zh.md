# CherryUSB ADB 设备 Demo

本示例使用 CherryUSB 的 `adb` 类，把 Bouffalo SDK 开发板变成一个 USB ADB
设备。烧录后开发板会以标准 ADB 接口枚举（`class 0xff / subclass 0x42 /
protocol 0x01`），主机可以直接用 `adb` 命令行工具访问。

已支持：

- `adb devices` 枚举（Linux 无需装驱动，Windows 需要 WinUSB 驱动）
- 交互式 shell：`adb shell`
- 单条命令：`adb shell help`、`adb shell echo hello` 等
- 内置命令：`help`、`echo`、`free`、`uptime`、`version`、`exit`

本 demo 未实现：

- ADB RSA 认证（`A_AUTH`）。设备不会向主机要求认证，因此任何主机都能连接，
  不要用于量产。
- 文件同步（`adb push` / `adb pull` / `adb ls`）。`sync:` 服务会被主动关闭，
  这样 `adb push/pull` 会立即报错而不是一直挂起。

## 硬件

需要带 USB 设备口的开发板，例如：

| 芯片 | 说明 |
| --- | --- |
| BL616 / BL618DG / BL616CL | 高速 USB |
| BL702 | 仅全速 USB（不定义 `CONFIG_USB_HS`） |

默认目标为 BL616DK 开发板。

## 编译和烧录

```bash
cd examples/cherryusb/cherryusb_adb

# 编译
make CHIP=bl616 BOARD=bl616dk

# 烧录（COMX 按实际串口修改）
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

固件输出在 `build/build_out/cherryusb_adb_bl616.bin`。

## 主机配置

### Linux

安装 adb（Debian / Ubuntu 可执行 `sudo apt install adb`），并添加 udev 规则，
让普通用户可以直接访问设备。本 demo 使用 VID:PID `18d1:4ee7`：

```bash
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="18d1", MODE="0666", GROUP="plugdev"' \
    | sudo tee /etc/udev/rules.d/51-cherryusb-adb.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

插上开发板后：

```bash
adb kill-server
adb devices
# List of devices attached
# 2024111701      device

adb shell
# cherryadb:/ $ help
# ...
```

### Windows

安装 Android platform-tools，并为 VID:PID `18D1:4EE7` 安装驱动。Android SDK
自带的 Google USB driver 可用；也可以使用
[Zadig](https://zadig.akeo.ie/) 将设备绑定到 WinUSB。

## 示例会话

```text
$ adb devices
List of devices attached
2024111701      device

$ adb shell version
CherryUSB v1.6.1 (0x010601)

$ adb shell free
heap: 12345 bytes free, largest block: 9000 bytes

$ adb shell
cherryadb:/ $ echo hello
hello
cherryadb:/ $ uptime
uptime: 12.345 s
cherryadb:/ $ exit
bye
```

## 实现说明

- `main.c` 初始化开发板并启动 FreeRTOS 调度器。
- `adb_demo.c` 注册 USB 描述符、调用 `usbd_adb_init_intf()`，并实现 ADB
  类需要的回调：
  - `usbd_adb_notify_open()`：`A_OPEN` 已被接受。service 为空表示交互式
    shell，否则 `shell:` 后面的文本作为单条命令执行。
  - `usbd_adb_notify_shell_read()`：收到 shell 输入，写入 FreeRTOS stream
    buffer。
  - `usbd_adb_notify_write_done()`：上一次 IN 传输完成。
- `adb_shell.c` 实现按行处理的 shell 和内置命令。
- 端点回调运行在 USB 中断上下文，因此只操作 FreeRTOS 原语；实际传输和命令
  执行都在 ADB 工作任务中完成。

`components/usb/Kconfig` 已新增 `CONFIG_CHERRYUSB_DEVICE_ADB`。在其他应用中
启用时，需要同时打开 `CONFIG_CHERRYUSB` 和 `CONFIG_CHERRYUSB_DEVICE`。

## 文件说明

| 文件 | 作用 |
| --- | --- |
| `main.c` | 板级初始化、启动调度器 |
| `adb_demo.c` | USB 描述符、ADB 回调、收/发工作任务 |
| `adb_shell.c` | 通过 ADB 提供的最小 shell |
| `config_file/usb_config.h` | CherryUSB 配置 |
| `config_file/FreeRTOSConfig.h` | FreeRTOS 配置 |
| `defconfig` | SDK 配置（`CONFIG_CHERRYUSB_DEVICE_ADB=y`） |
