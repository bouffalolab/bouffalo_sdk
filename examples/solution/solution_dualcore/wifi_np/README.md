# BL618DG NP WiFi / LVGL / CoreMark

本例运行于 BL618DG NP，使用 FreeRTOS，提供 UART1 Shell、CoreMark、LVGL v8 和可配置的 WiFi 网络功能。

## 功能与使用

- 上电初始化板级资源、Shell 和应用；在 Shell 输入 `coremark`，同时启动 LVGL 和 CoreMark 任务。每次启动后执行一次即可。
- 默认界面为 `lv_demo_benchmark()`。在 `app/src/app_init.c` 中选择 benchmark、keypad_encoder、music、stress、widgets 之一；各 demo 的编译开关保留在 `lvgl8_conf_user.h`。
- 保留 `sample`、`cpu_nop` 等应用命令及网络相关代码。WiFi 由 `CONFIG_WIFI_ENABLE` 控制；启用后保留 lwIP、WiFi6、iperf、DHCP server、ping 等配置，默认开关状态不变。
- 初始化任务优先级为 16，LVGL / CoreMark 为 27，WiFi 初始化任务为 10；RAM、WRAM 和 AP/NP PSRAM 分区沿用项目配置。

## 构建与配置

在本目录使用 SDK 工具链：

```sh
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np BUILD_DIR=build_np_display
make menuconfig CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np BUILD_DIR=build_np_display
```

首次使用应选择新的构建目录，避免已有保存配置覆盖 `defconfig`。AP/NP 固件须使用一致的内存分区。

默认显示为 NT35510 480×800、DBI Type-B 8-bit、RGB565，使用 SDK 的 default display 和 LVGL v8 default GRAM port。DMA 为 `dma0_ch3`，40 个 LLI，PSRAM 中两块各 192000 字节绘图缓冲区；改变色深不增加这两块缓冲区的字节数。

`CONFIG_LVGL8_DISP_DEFAULT_SW_ROTATE=y` 保留软件旋转能力，初始不旋转；应用可调用 `lv_disp_set_rotation()`。选择 `CONFIG_DISPLAY_DEFAULT_FORMAT_XRGB8888` 时，应用配置自动使用 32-bit 色深并启用透明绘制；GRAM 面板输出 RGB，不提供独立视频层的透明叠加。不支持 LVGL v8 packed RGB888，也不提供 NP 视频输出。

## 接线

`board_gpio_overlay.c` 整板覆盖开发板默认 GPIO 实现，下表为本项目接线，不应直接套用其他板子的默认引脚。

| 功能 | GPIO |
| --- | --- |
| UART1 Shell TX / RX | 15 / 16 |
| NT35510 WR / CS / D/C / RD | 17 / 18 / 19 / 20 |
| NT35510 D0～D7 | 21～28 |
| 面板 RESET（低有效） | 40 |

默认 CS 为硬件控制 GPIO18。接口 hook 只配置总线信号，硬件 CS 由独立 hook 配置；若选择软件 CS，需要显式设置 `CONFIG_DISPLAY_DEFAULT_CS_PIN`（相同接线用 18），不能使用其他开发板的默认值。面板逻辑电源、背光电源及地线必须按实际模块规格连接，本例不控制面板供电。

**背光默认 `CONFIG_DISPLAY_DEFAULT_BACKLIGHT_NONE=y`，不会驱动 GPIO16。GPIO16 是 Shell RX，不能同时作为背光输出。** 实际硬件必须独立提供背光电源/使能，或将背光控制改接到空闲的独立 GPIO，再在 menuconfig 中选择 GPIO 背光，设置 `CONFIG_DISPLAY_DEFAULT_BACKLIGHT_PIN` 及有效电平。若模块背光控制仍接在 GPIO16，必须先调整接线；`NONE` 不会消除物理共线冲突，也不会自动点亮背光。不得在不调整 Shell 接线和配置的情况下启用 GPIO16 背光。

项目还保留以下 GRAM 接线 hooks，切换屏幕时须同时选择匹配的 panel、接口、设备和 CS/DC 配置：

| 接口 | 信号 GPIO | 硬件 CS GPIO |
| --- | --- | --- |
| DBI C4 | SCL=11、SDA=13、D/C=14 | 12 |
| DBI C3 | SCL=11、SDA=13 | 12 |
| DBI QSPI | SCL=0、D0～D3=2～5 | 1 |
| SPI0 / SPI1（芯片提供时） | SCLK=13、MOSI=15；CS/DC 为单独的软件 GPIO | 无 |

SPI MOSI=15 与 Shell TX 冲突；原 SPI CS=14、DC=16 中 DC 也与 Shell RX 冲突。使用 SPI 必须先为 SPI 或控制台安排独立接线并修改项目 overlay / 配置。DBI C4/C3、QSPI 也需要核对 AP、调试口和其他外设的占用，不能让多个核同时驱动共享引脚或控制器。

## 可选输入

`CONFIG_LVGL8_PORT_INDEV=n` 默认关闭输入，保留 `CONFIG_BSP_TOUCH=y`。需要触摸时启用输入端口，并配置实际触摸芯片和独立引脚；可在项目内提供 `touch_conf_user.h`。SDK 默认触摸为 FT6X36，SCL=10、SDA=11，须核对板级占用。

应用的 `lvgl8_conf_user.h` 只向输入端口提供由 default panel 推导的 `LCD_W` / `LCD_H` 尺寸，不依赖 LCD 驱动。自定义 panel 需补充对应尺寸；触摸方向需与显示旋转及实际安装方向一致。