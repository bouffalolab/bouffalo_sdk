# display / lvgl_v9_with_osd

[English](README.md)

播放 JPEG 视频并叠加 LVGL v9 动画 OSD。显示框架（`bsp/common/display`）
将 **NV12 Base** 视频层与 **ARGB8888 OSD0** UI 层合成输出。
JPEG 输入可选择 SD 卡（FS）或 USB DATA ACM，两种输入均可配合 WiFi HTTP OTA。

`demos/ui/` 使用一套自适应 UI 和七个内联 C 图标，包含播放控件、音量滑条和滚动字幕。
UI 根据 LVGL 显示分辨率布局，按短边选择字体，并在分辨率变化时重新布局。
竖屏帧缓冲中的控件逐个旋转 270 度，供横向观看；方屏和横屏使用原生方向。
背景保持透明。不配置触摸或音频播放；控件是 UI 动画，不是音频传输控制。

## 平台与运行模式

仅支持 **BL618DG AP**，其他芯片及 NP 核会被拒绝。
随附配置使用 `BOARD=bl618dgdk`、16 MiB AP PSRAM；默认面板为
EK79007 1024x600 MIPI DSI 2 lane，复位引脚为 GPIO2。

| 模式 | `CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO` | `CONFIG_LVGL_V9_WITH_OSD_OTA` | JPEG 来源与网络服务 |
|---|---|---|---|
| FS（默认） | `n` | `n` | 仅 SD 卡，无 USB 视频和 WiFi OTA |
| USB | `y` | `n` | USB DATA ACM、NetHub ECM/CMD ACM、WiFi 和 iperf |
| FS + OTA | `n` | `y` | SD 卡及 WiFi HTTP OTA |
| USB + OTA | `y` | `y` | USB DATA ACM、NetHub 服务及 WiFi HTTP OTA |

两个选项默认均为 **`n`**。启用 OTA 不会启用 USB；启用 USB 会替换 SD 读取任务，
不是在运行时切换视频来源。

`config_file/` 集中存放 FreeRTOS、FatFS、LVGL、lwIP、USB 和视频配置头文件，
以及仅供参考的 LCD 配置头文件。

## 编译与烧录

配置好 SDK 工具链后，在 `examples/display/lvgl_v9_with_osd` 目录执行下列命令。
每种模式和面板配置使用独立构建目录，通过命令行明确覆盖配置，无需 `menuconfig`。

```sh
# 仅 FS
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/fs CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=n CONFIG_LVGL_V9_WITH_OSD_OTA=n

# USB 视频
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/usb CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=y CONFIG_LVGL_V9_WITH_OSD_OTA=n

# FS + WiFi HTTP OTA
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/ota CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=n CONFIG_LVGL_V9_WITH_OSD_OTA=y

# USB 视频 + WiFi HTTP OTA
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/usb_ota CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=y CONFIG_LVGL_V9_WITH_OSD_OTA=y
```

产物位于 `<BUILD_DIR>/build_out/display_lvgl_v9_with_osd_bl618dg_ap.{elf,bin,map}`。
OTA 构建还生成 RAW `.bin.ota` 包。

连接烧录串口，按开发板说明进入下载模式。先完成编译，再烧录对应的已有产物：

```sh
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/fs COMX=xxx CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=n CONFIG_LVGL_V9_WITH_OSD_OTA=n
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/usb COMX=xxx CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=y CONFIG_LVGL_V9_WITH_OSD_OTA=n
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/ota COMX=xxx CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=n CONFIG_LVGL_V9_WITH_OSD_OTA=y
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/usb_ota COMX=xxx CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=y CONFIG_LVGL_V9_WITH_OSD_OTA=y
```

只运行所选模式对应的烧录命令。将 `xxx` 替换为实际串口，烧录波特率默认为 2000000，
可用 `BAUDRATE` 覆盖。编译和烧录须使用相同的 `BUILD_DIR`、模式及面板配置参数。
烧录后退出下载模式并复位。应用 UART 控制台波特率为 **2000000**。

## 面板选择与接线

在编译命令行使用 `CONFIG_DISPLAY_DEFAULT_VIDEO_*=y` 选择视频屏模块。
面板选择由 `bsp/common/display/default/Kconfig` 定义。

下表列出可用的面板模块，每次只将一个适用的 Kconfig 模块设为 `y`。

| 参考选项 | 生效的 Kconfig 选项（`=y`） | 分辨率 / 接口 |
|---|---|---|
| `LCD_DSI_ILI9881C_KD050020` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9881C_KD050020` | 720x1280，DSI 4 lane |
| `LCD_DSI_ILI9881C_KD050023W4` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9881C_KD050023W4` | 720x1280，DSI 2 lane |
| `LCD_DSI_ILI9806E_KD050FWFIA019` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9806E_480X854` | 480x854，DSI 2 lane |
| `LCD_DSI_ST7102_YH494` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ST7102_YH494` | 480x960，DSI 2 lane |
| `LCD_DSI_ST7102_KD027HVF` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ST7102_KD027HVF` | 320x320，DSI 1 lane |
| `LCD_DSI_AXS15231B_HS035` | `CONFIG_DISPLAY_DEFAULT_VIDEO_AXS15231B_172X640` | 172x640，DSI 1 lane |
| `LCD_DSI_AXS15231E_JX371` | `CONFIG_DISPLAY_DEFAULT_VIDEO_AXS15231E_258X960` | 258x960，DSI 1 lane |
| `LCD_DSI_EK79007_WKS70WSV114` | `CONFIG_DISPLAY_DEFAULT_VIDEO_EK79007_2LANE`（默认）或 `CONFIG_DISPLAY_DEFAULT_VIDEO_EK79007_4LANE` | 1024x600，DSI 2 或 4 lane |
| `LCD_DSI_ST7701S_HD40007C30` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701S_DSI_480X480` | 480x480，DSI 2 lane |
| `LCD_DSI_FL7707N_HD395003C30` | `CONFIG_DISPLAY_DEFAULT_VIDEO_FL7707N_720X720` | 720x720，DSI 2 lane |
| `LCD_DSI_JD9365TX_7KF82` | `CONFIG_DISPLAY_DEFAULT_VIDEO_JD9365TX_720X1280` | 720x1280，DSI 2 lane |
| `LCD_DPI_GC9503V` | `CONFIG_DISPLAY_DEFAULT_VIDEO_GC9503V_480X480` | 480x480，RGB + 命令口 |
| `LCD_DPI_ST7701P` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701P_480X480` | 480x480，RGB + 命令口 |
| `LCD_DPI_ST7701S` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701S_RGB_480X480` | 480x480，RGB + 命令口 |
| `LCD_DPI_JD9165BA` | `CONFIG_DISPLAY_DEFAULT_VIDEO_JD9165BA_1024X600` | 1024x600，RGB，命令口可选 |
| `LCD_DPI_STANDARD` | `CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_800X480` 或 `CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_1024X600` | 标准 RGB，无命令口 |

- 面板尺寸通过 `DISPLAY_DEFAULT_VIDEO_WIDTH/HEIGHT` 来自所选默认模块，LVGL 使用该显示分辨率。
	JPEG 尺寸及 `FRAME_DIR` 在 `config_file/video_config.h` 中单独设置，**不会随面板选择自动变化**。
	DSI 和 RGB 的视频默认值均为 **1024x600**、**`/sd/1024x600`**。
- 保持 `CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_ARGB8888=y` 和 `CONFIG_LVGL9_PORT_DISP=y`。
	默认视频屏 LVGL port 管理 OSD0；RGB565 和 XRGB8888 不能提供所需的逐像素 alpha。
	Base 独立使用 NV12。
- DSI lane 数默认跟随模块。必要时通过 `CONFIG_DISPLAY_DEFAULT_DSI_LANES`
	和 lane 顺序选项匹配实际接线。
- 背光默认为 **`CONFIG_DISPLAY_DEFAULT_BACKLIGHT_NONE=y`**，面板和背光需外部供电。
	BL618DG USB 使用 **GPIO40/41**，使用 USB 时**不能将 GPIO40 配置为背光**。
	不要将参考头文件中的 GPIO40 背光设置复制到生效配置。
- RGB/DPI 要求 **`CONFIG_DPI_DISPLAY_BOARD_MODE=y`**，使控制信号使用 GPIO0..3，
	避开 SDH GPIO45..48。本示例 Kconfig 会为 RGB 视频自动选择该模式。
	在 FS（含 FS + OTA）模式下，CMake 还会强制 SDH **1-line** 协商，读取任务不占用
	GPIO43/44，将其留给 RGB；这两个引脚与 SDH DAT3/DAT2 复用。
	板级模式与 1-line 协商均不可缺少，仅修改 GPIO 复用不够。
- **GPIO2 是 RGB 控制信号**。使用 RGB 时必须以 **`CONFIG_DISPLAY_DEFAULT_RESET_USED=n`**
	覆盖 DSI 默认复位配置；只有实际复位接到了其他空闲 GPIO 时，才设置
	`CONFIG_DISPLAY_DEFAULT_RESET_USED=y` 并通过 `CONFIG_DISPLAY_DEFAULT_RESET_PIN`
	指定真实引脚。CMake 会拒绝将 GPIO0..3 中任一 RGB 控制引脚用作复位。
- RGB 命令口默认 **CS=GPIO34、SCK=GPIO35、数据=GPIO36**（3 线）。
	按硬件设置 `CONFIG_DISPLAY_DEFAULT_RGB_CMD_CS_PIN`、`_SCK_PIN`、`_MOSI_PIN`。
	4 线命令口选择 `CONFIG_DISPLAY_DEFAULT_RGB_CMD_C4=y`，并配置 `_DC_PIN`（默认 GPIO37）。
	JD9165BA 若未接初始化命令口，可选择 `CONFIG_DISPLAY_DEFAULT_RGB_CMD_NONE=y`。
	这些可选命令引脚也需检查与其他板级接线的冲突。

例如，标准 RGB 1024x600、FS 输入、不使用 GPIO 复位：

```sh
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/fs_rgb CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=n CONFIG_LVGL_V9_WITH_OSD_OTA=n CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_1024X600=y CONFIG_DISPLAY_DEFAULT_RESET_USED=n
```

## SD 视频资源（FS 和 FS + OTA）

将 FAT32 SD 卡插入开发板 SDH 卡槽。默认配置可使用已有的 **9468 帧 1024x600**
资源序列。读取任务自动确定连续帧数，9468 不是固定上限。
如需导出其他视频，主机需要 Python 3 和 ffmpeg：

```sh
python3 tools/export_video.py input.mp4 osd-assets --frames 300 --quality 8
```

将生成的 `1024x600` 目录复制到卡根目录：

```text
/1024x600/000/p0001.jpg ... p0100.jpg
/1024x600/001/p0101.jpg ... p0200.jpg
/1024x600/002/p0201.jpg ...
```

卡上首帧为 `/1024x600/000/p0001.jpg`，固件通过 `/sd/1024x600/000/p0001.jpg`
访问。**卡上不要额外创建 `sd` 目录**。帧从 1 连续编号，默认每目录 100 帧，
最多 10000 帧，播放到检测到的末尾后循环。`FRAMES_PER_CHUNK` 必须与卡上布局一致。

导出工具默认 100 fps、quality 8、最多 10000 帧，只生成 1024x600 JPEG。
为避免旧尾帧残留，工具拒绝已存在的 `1024x600` 输出目录；失败后应丢弃不完整导出再重试。
如需其他面板/视频尺寸，准备对应资源，并同时调整 `config_file/video_config.h` 中的
`VIDEO_WIDTH`、`VIDEO_HEIGHT` 和 `FRAME_DIR`；此导出工具没有宽高参数。

每帧必须完整，为 baseline 8-bit YCbCr 4:2:0，尺寸精确匹配 `VIDEO_WIDTH/HEIGHT`，
且不超过 128 KiB。必须使用标准 Huffman 表，每个 FFC4 DHT 段只放一张表；
普通 ffmpeg 默认输出或优化表不满足要求。两个主机工具均会规范化 DHT 布局。

缺卡、文件系统错误或首帧缺失会报错并使读取任务空闲；后续帧缺失时回到序列开头，
大小无效或读取失败会记录错误并跳过。错误/重试路径延时 10 ms，避免忙循环。
解码器丢弃格式、尺寸或表不符合要求的输入。
JPEG 兼容性细节见 [tools/README.md](tools/README.md)。

## USB 视频与主机连接

USB 模式由示例管理复合设备描述符、接口、端点和枚举；NetHub 负责 ECM/CMD ACM
传输及网络路由，USB 事件由示例通知 NetHub。视频 DATA ACM 回调直接交给
`usb_reader`，不经过 NetHub stream。

| 功能 | 接口 | 端点 |
|---|---|---|
| ECM 网络 | 0/1 | IN `0x81`、OUT `0x02`、INT `0x85` |
| CMD ACM / vchan | 2/3 | IN `0x83`、OUT `0x04`、INT `0x86` |
| Video DATA ACM | 4/5 | IN `0x87`、OUT `0x08`、INT `0x07` |

主机需要 PATH 中的 ffmpeg，以及 pyserial（`pip install pyserial`）。
Linux 连接 USB 数据接口后查看：

```sh
ifconfig -a
ls /dev/ttyACM*
python3 tools/stream_video.py --list-devs
```

应出现一个 ECM 网口（例如 `enxb4e8423cb200`）和两个 CDC ACM tty 节点
（通常为 `/dev/ttyACM0`、`/dev/ttyACM1`）。视频 DATA ACM 通常为接口 **`04`**，
不传 `--data-dev` 时脚本自动识别。

对默认 1024x600 固件，应显式传入宽高：

```sh
python3 tools/stream_video.py --video video_ls.mp4 --width 1024 --height 600 --loop
```

基本用法 `python3 tools/stream_video.py --video video_ls.mp4 --loop` 仍可使用，
但脚本默认值保持为 **480x960**、24 fps、quality 4，仅适用于视频配置为 480x960
的固件，不匹配默认 1024x600 固件。始终使 `--width/--height` 匹配 `VIDEO_WIDTH/HEIGHT`。
其他选项包括 `--fps`、`--quality`（2 最佳，至 31）、`--data-dev`、`--data-interface`、
`--cmd-dev`、`--cmd-interface`、`--baud`、`--no-pace` 和 `--ping`，
详见 [tools/README.md](tools/README.md)。

串流期间 USB 断开或设备复位时，脚本保持 ffmpeg 运行，等待 DATA 接口 `04` 重新出现，
从下一个完整 JPEG 恢复发送；重新枚举后 `/dev/ttyACM*` 编号可以变化。
固件在断开时将接收状态复位到下一个 12 字节帧头。

### WiFi 与 iperf（USB 模式）

设备运行 WiFi station，通过 ECM 向主机桥接流量。在 `uart0` shell（2000000 baud）连接：

```text
wifi_sta_connect -D <SSID> <PASSWORD>
```

`-D` 关闭设备侧 DHCP，适用于桥接。若要设备本地 OTA，应改用
`wifi_sta_connect <SSID> <PASSWORD>`，**不加 `-D`**，并等待 `CODE_WIFI_ON_GOT_IP`。

在主机查找 ECM 网口，将 iperf 流量绑定到其 IP：

```sh
ifconfig -a
IF=enxb4e8423cb200
iperf3 -c <server_ip> -B <enx_iface_ip>
```

将示例网口名、地址替换为实际 ECM 网口、该网口配置的 IP 和 iperf 服务端 IP。

## WiFi HTTP OTA（FS + OTA 和 USB + OTA）

首先使用相应模式的编译/烧录命令，经 UART 安装**已启用 OTA 的应用及
`partition_ota_8M.toml` 分区表**。Flash 至少需要 **8 MiB**。
分区表提供两个互不重叠的 2 MiB 固件槽，固件有效载荷必须能放入非活动槽。

通过 HTTP 提供所选 OTA 构建生成的 RAW `.bin.ota` 包：

```sh
# FS + OTA
python3 -m http.server 8000 --directory build/ota/build_out
# USB + OTA 则改为提供 build/usb_ota/build_out。
```

在 UART shell（2000000 baud）连接 WiFi，并使用计算机的局域网 IP：

```text
wifi_sta_connect <SSID> <PASSWORD>
# Wait for CODE_WIFI_ON_GOT_IP.
https_ota_start http://<COMPUTER_LAN_IP>:8000/display_lvgl_v9_with_osd_bl618dg_ap.bin.ota
ota_status
```

命令名仍为 `https_ota_start`，但本应用**仅接受 HTTP**。
升级会在安全边界暂停 JPEG 读取与解码任务，将包写入非活动槽，校验 SHA256，
成功后自动重启；OSD 显示阶段和进度。若只暂存、不自动重启，在
`https_ota_start <URL>` 后加 `0`，成功后使用 `ota_reboot`。
失败时终止本次升级并恢复视频管线。

WiFi 启动及 USB ECM/CMD ACM 枚举不等待显示初始化成功；只有视频读取与解码任务
受显示就绪屏障约束。显示初始化失败时仍保留网络及 OTA 命令访问，但不保证升级成功：
OTA 要求所有已启用的视频暂停参与者确认，缺少任何活跃参与者都会终止本次升级。
视频任务启动前不登记视频参与者；解码器等待已接受的显示提交时也能确认暂停，
但不会因此归还或复用仍被扫描或排队的帧缓冲。

## 数据流与缓冲所有权

```text
FS:  SD JPEG files -> filesystem_reader -> JPEG queue -> dpi_manager -> MJDEC
USB: host video -> ffmpeg -> tools/stream_video.py -> DATA ACM callback
		 -> usb_reader -> JPEG queue -> dpi_manager -> MJDEC

MJDEC -> NV12 buffers -> app_display -> DPU Base
LVGL demos/ui -> default-video LVGL port -> ARGB8888 OSD0
Base + OSD0 -> selected DSI/RGB panel
```

- 所选读取任务使用 128 KiB JPEG 缓冲（FS 两块，USB 三块）；`dpi_manager` 负责
	MJDEC、NV12 帧缓冲（DSI 三缓冲，RGB 双缓冲）、帧率限制和解码恢复。
- 默认全屏 1024x600 直接解码到 NV12。较小视频区域或非 MCU 对齐宽度使用 DMA2D
	居中裁剪/重排。视频不能超出面板，NV12 要求视频和面板宽高均为偶数。
- `app_display` 在默认视频屏 port 初始化后配置 Base 层。面板尺寸来自所选默认模块；
	LVGL port 管理两块全屏 ARGB8888 OSD 缓冲及 UI 更新。
- DMA 缓冲槽为 64 字节对齐。NV12 plane 初始化时 clean/invalidate，随后仅由
	MJDEC/DMA2D 写入。Base 禁止 cache clean，避免旧 CPU cache 覆盖 DMA 输出；
	初始化后 CPU 不得写这些 plane。JPEG 输入在 MJDEC 读取前执行 cache clean。
- FIRST/RETIRED/LAST 显示完成事件决定缓冲所有权，不能因 VSYNC 或超时提前复用。
	缺少完成中断时记录错误并继续等待。

## 配置限制

- 面板表说明可选模块，不代表任意开发板均可直接使用。面板时序、lane 接线、供电、
	复位、背光和命令口都必须匹配实际硬件。
- 切换面板不会缩放 JPEG 资源。默认 1024x600 视频无法放入较小面板，需同步调整
	`config_file/video_config.h` 与 SD 资源或 USB 串流尺寸；较大配置还受可用 PSRAM 限制。
- `VIDEO_TARGET_FPS` 默认 **100 fps**，是上限而非保证值。SD 播放不保留源视频时间戳。
	导出工具的 `--fps` 应匹配预期上限；USB 工具默认 24 fps。
- OTA 仅支持 RAW `.bin.ota`、HTTP 200 及有效的固定 `Content-Length`。
	本应用不支持 HTTPS 信任配置、重定向、chunked 响应和 XZ 包。应使用可信局域网；
	SHA256 完整性校验不能替代服务端身份认证。
- `demos/` 包含完整的随附示例源码和资源，但 CMake 只递归编译 `demos/ui/*.c`。
	其他 demo 不会因目录存在而自动启用；本示例未配置触摸和音频子系统。