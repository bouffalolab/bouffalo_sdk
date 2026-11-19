# cam_lcd_mipi

## 功能

- BL618DG AP 核采集 DVP 相机，通过 MIPI DSI 显示 RGB565 图像。
- 默认屏为 JD9365TX 7KF82，720×1280、2 data lanes；帧内存 RGB565，DSI 链路 RGB888。
- 自动识别传感器，要求传感器输出 YUYV，由 CAM 转换为 RGB565；容量上限为 1280×720 像素。
- 根据有效覆盖面积选择不旋转或顺时针旋转 90°，居中裁剪、居中显示，不缩放；未覆盖区域保持黑色。
- 图像与屏幕尺寸及方向一致时直接采集到显示槽；其余情况通过 DMA2D 拷贝或旋转。
- 三个显示槽、两个相机槽，FIFO 顺序显示；没有可写槽时暂停采集，有空闲槽后恢复。
- 启动时先显示黑帧，再开启背光和相机；串口每秒报告实际显示完成的 FPS。

## 接线

### BL618DG IFA

使用工程内 `bl618dg_ifa_board/board_gpio_overlay.c`：

| 信号 | 引脚/要求 |
| --- | --- |
| 相机 SCCB/I2C0 | GPIO20、GPIO21（SCL、SDA） |
| 相机 MCLK | GPIO14 |
| 相机 DVP | GPIO0～GPIO10，按芯片 CAM 引脚复用接线 |
| 屏幕 DSI | 专用 CLK±、D0±、D1±，lane order 3210 |
| 屏幕背光使能 | GPIO29，高电平开启 |
| JD9365TX RESET/RESET2 | 保留模组外部固定复位电路；不占用 GPIO2 |

相机电源、PWDN 和 RESET 须由板上电路保证有效。GPIO2 保留为 DVP 信号，不能同时连接或驱动为 LCD RESET。
DSI 差分线应匹配模组 FPC 定义并共地，不要接到普通 GPIO。
若工程板需要额外 DSI 控制脚，可修改 overlay 内 `cam_lcd_mipi_dsi_gpio_init()`；默认专用 DSI pads 无需普通 GPIO 复用。

### BL618DGDK

板级 DVP 配置使用 I2C0 GPIO16/13、MCLK GPIO15、PWDN GPIO29、相机 RESET GPIO19，以及 GPIO0～GPIO10。
**PWDN GPIO29 与默认屏背光 GPIO29 冲突，不能直接套用上述 IFA 接线。**
需先将背光改接空闲 GPIO，并将 `CONFIG_DISPLAY_DEFAULT_BACKLIGHT_PIN` 配置为该引脚，再选择 `BOARD=bl618dgdk`。
禁止用 GPIO2 复位屏幕，否则会破坏相机 DVP 总线。

## 配置

`defconfig` 默认启用：

| 配置 | 默认值 |
| --- | --- |
| `CONFIG_PSRAM` | `y` |
| `CONFIG_PSRAM_FOR_AP_SIZE` | `0x1000000`（16 MiB） |
| `CONFIG_BSP_IMAGE_SENSOR` / `CONFIG_BSP_DISPLAY` | `y` |
| `CONFIG_DISPLAY_DEFAULT` / `CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO` | `y` |
| `CONFIG_DISPLAY_PANEL_JD9365TX` | `y` |
| `CONFIG_DISPLAY_DEFAULT_VIDEO_JD9365TX_720X1280` | `y` |
| `CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_RGB565` | `y` |
| `CONFIG_DISPLAY_DEFAULT_DSI_LANES` | `2` |
| `CONFIG_DISPLAY_DEFAULT_RESET_USED` | `n`，JD9365TX 7KF82 为外部复位 |
| `CONFIG_DISPLAY_DEFAULT_BACKLIGHT_GPIO` / `CONFIG_DISPLAY_DEFAULT_BACKLIGHT_PIN` | `y` / `29` |

当前 Kconfig 已提供该 JD9365TX 模组选项，无需自定义模组。选择其他 DSI 屏时，须同时调整模组、lane 数/顺序和板级控制引脚，保持帧格式 RGB565。
只有实际带可控复位线的其他模组才启用 `CONFIG_DISPLAY_DEFAULT_RESET_USED`；复位脚必须避开相机总线。

默认五个帧槽占用约 8.79 MiB PSRAM。每槽按 64 字节对齐；CPU 仅在启动时清零并 clean/invalidate，之后由 CAM、DMA2D 和显示硬件访问。
显示扫描关闭自动缓存清理，应用运行时不得读写像素缓冲。

## 编译与烧录

在本工程目录执行，使用独立 `BUILD_DIR`，避免已有配置覆盖 `defconfig`：

```sh
make CHIP=bl618dg BOARD=bl618dg_ifa_board CPU_ID=ap BUILD_DIR="$PWD/build_ifa_rgb565"
make flash CHIP=bl618dg BOARD=bl618dg_ifa_board CPU_ID=ap BUILD_DIR="$PWD/build_ifa_rgb565" COMX=/dev/ttyACM0
```

## 运行与停止

串口依次输出传感器名称/分辨率、裁剪和旋转参数、`camera display fps`。
采集或显示速度不匹配时采用背压，不覆盖正在 DMA 读取或正在扫描的帧。

需要停止时，可通过调试器将 `cam_lcd_mipi_stop_requested` 置为 `true`：停止采集，依序显示已就绪的显示帧，关闭背光，等待最终帧释放后关闭显示。
发生提交失败、完成事件异常或 DMA2D/显示等待超时则冻结生产者并停留输出错误；不会将未确认释放的槽重新交给硬件写入。
