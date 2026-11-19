# USB 与 CherryUSB

## 控制器和配置

`CONFIG_CHERRYUSB` 启用组件，Device、Host 和各 class 由 `components/usb/Kconfig` 分别控制。当前 LHAL 按芯片选择 USB v1/v2：BL702 使用 v1；BL616、BL616CL、BL618DG 使用 v2；其他目标须核对当前源码的芯片分支。`examples/cherryusb/cherryusb_cli` 文档列出 BL702 的 Device 示例及 BL616/BL616CL/BL618DG 的 Device/Host 示例；BL618DG 示例按 AP 核构建。配置或示例存在不等于目标板电气和外设组合均已验证。 本次 BL702 示例链接时缺少 `usbd_execute_test_mode` 与 `usbd_set_remote_wakeup`，不能把 README 支持声明当作当前 checkout 已构建通过。

板级 USB 引脚复用、PHY、电源和 Host VBUS 须核对具体板；控制器把 VBUS 状态标记有效不证明外部 5V 供电路径存在。示例 Device/Host 测试同一时刻互斥，切换前先停止当前角色；具体 class 受目标配置限制。

## Device 模式

当前 core API 是 `usbd_desc_register`、`usbd_add_interface`、`usbd_add_endpoint`、`usbd_initialize(busid, reg_base, event_handler)` 和 `usbd_deinitialize`；描述符、接口和端点回调的完整组合参见 `examples/cherryusb/cherryusb_cli/device_demo/cdc_acm_template.c`。通过 `USBD_EVENT_CONFIGURED`、reset、disconnect 等事件判断状态，启动 API 成功不能证明主机已枚举。

`usbd_ep_start_read()` / `usbd_ep_start_write()` 异步提交，数据和相关对象必须保持有效直到端点完成回调；返回负值时处理提交失败。某些协议需要整最大包长时发送 ZLP，按 class 协议和示例处理。USB v2 使用 VDMA 的路径与 v1 FIFO 路径不同，不能假定所有芯片完全相同。缓冲区按目标 `usb_config.h` 的对齐、非缓存区和 cache 维护要求配置；示例使用 `USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX`。

## Host 模式

当前入口是 `usbh_initialize(busid, reg_base, event_handler)` / `usbh_deinitialize()`；控制传输 `usbh_control_transfer()` 是阻塞接口，默认超时 500 ms。`usbh_submit_urb()` 的 URB timeout 为 0 表示异步、非零为等待模式；异步请求的 URB 和缓冲必须在完成或取消结束前有效。class 功能还依赖相应配置、控制器能力及外设兼容性，不能仅因 class 代码存在而推断所有 ISO 或多设备组合可用。

`cherryusb_cli` 的 Host serial、ECM、HID、MSC 示例各只支持一个活动实例；Host MSC 测试会写连接的存储设备。CDC ACM loopback 示例要求 PC 置 DTR。真实验证应分开记录固件构建、供电与连接、枚举、类驱动、数据收发、断开恢复和角色切换；不要在不明确介质内容时运行写测试。

## ADB 设备类

`components/usb/Kconfig` 的 `CHERRYUSB_DEVICE_ADB` 编译 `components/usb/cherryusb/class/adb/`；`examples/cherryusb/cherryusb_adb` 是设备端 demo，使用 VID:PID `18d1:4ee7`、接口 class `0xff` / subclass `0x42` / protocol `0x01`，支持 `adb devices`、交互式 `adb shell` 和 `adb shell <command>`。当前实现或修改相关 class/示例时注意：

- A_OPEN 的 payload 由 adb 主机按 NUL 结尾发送，`data_length` 包含结尾 `\0`；解析 `shell:` / `sync:` 服务名前必须剥掉尾部 NUL。
- 交互式终端的 Enter 是 CR（0x0d），而单条命令的文本位于 `shell:` 服务名中；shell 行处理应同时接受 CR、LF 和 CRLF。
- ADB class 响应（CNXN/OKAY/CLSE）与应用数据共用 IN 端点，必须串行。demo 使用响应队列 + 应用数据优先 + OSAL 临界区；`usbd_abd_write()` 在端点忙时返回负值，调用方需重试。OUT 端点收到零长度包或尺寸异常时应安全丢弃并重新挂读。
- 当前 class 不处理 `A_AUTH`。Linux adb 会直接把设备识别为 `device`（不是 `unauthorized`），即实际没有认证，只能用于调试，不能用于量产安全场景。
- `sync:` 文件服务未实现，`adb push/pull/ls` 会快速失败；`MAX_PAYLOAD` 为 4 KiB，单条 shell 命令受示例行缓冲限制。
- Windows 需要为 `18D1:4EE7` 安装 Google USB driver，或用 Zadig 绑定 WinUSB。

Linux 主机侧测试流程（未安装 Android udev 规则时需要权限规则）：

```bash
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="18d1", MODE="0666", GROUP="plugdev"' \
    | sudo tee /etc/udev/rules.d/51-cherryusb-adb.rules
sudo udevadm control --reload-rules && sudo udevadm trigger

adb devices          # 应显示 device，product/model 为 demo 上报值
adb shell help       # 单条命令
adb shell            # 交互式，输入 exit 退出
```

串口烧录后的复位/正常启动方式遵循 SDK 开发测试串口工装约定（各芯片通用：RTS→CHIP_EN、DTR→BOOT），见[固件打包与烧录](35-firmware-pack-flash.md)的“串口控制脚与正常启动”。验证时分别记录固件构建、烧录、复位/启动、USB 枚举描述符、`adb devices`、shell 交互、断开重连；构建成功或设备枚举成功都不等于 ADB 功能已验证。
