# 图形、显示与触摸

## LVGL 与显示框架

当前源码包含 LVGL 8.4.0 和 9.5.0，分别通过 `CONFIG_LVGL8`、`CONFIG_LVGL9` 选择；版本 API 和 port 不应混用。`examples/display/lvgl_v8` 使用配置的自定义 tick，v9 显式 `lv_tick_set_cb`；两者都初始化显示 port、按需初始化输入，再周期执行 `lv_task_handler`。同一 LVGL 实例的任务与锁策略以工程配置为准。

`CONFIG_BSP_DISPLAY` 选择新的 `bsp/common/display/` 分层框架：Display、Panel、IO、Bus，以及视频屏的 Scanout。GRAM 屏通过 `display_draw` / `display_draw_async` 发送区域，视频屏用 `display_flip` 持续扫描帧缓冲。屏幕、像素格式、总线、DMA、板级引脚和背光需要与真实模块匹配；`display_get_info()` 提供对齐和单次绘制限制。框架初始化后背光默认关闭，通常应在首帧生效后开启。

GRAM 异步绘制的缓冲在 `DISPLAY_EVT_DRAW_DONE` 前不可修改；DMA 回调可能在 ISR 中，无 DMA 时也可能在调用线程执行，回调内不要阻塞或续交 IO/Bus 传输。视频帧须等待 `FIRST`、替换后的 `RETIRED` 或关闭后的 `LAST` 等事件确认所有权；事件不携带被归还帧地址，应用需自行追踪，超时不等于缓冲已释放。具体缓存、对齐和并发约束见 `bsp/common/display/README_CN.md`。

LVGL GRAM port 用局部缓冲和异步绘制；视频 port 使用双帧，v8 full refresh、v9 DIRECT。BL618DG DPU/DSI 需要 B0 AP；BL616 SIM v1 并行 RGB 缺 ROM 函数表库时可链接但初始化返回 `-ENOTSUP`；BL616CL 串行 RGB 尚无后端。不同工程可能仍使用旧 `bsp/common/lcd/`，不可混用初始化路径。

## 触摸

`CONFIG_BSP_TOUCH` 编译公共触摸层，LVGL 输入 port 还需分别启用 `CONFIG_LVGL8_PORT_INDEV` 或 `CONFIG_LVGL9_PORT_INDEV`；显示示例默认关闭输入。当前公共实现支持 FT6X36 和 GT911，固定 I2C0、200 kHz，引脚可由 `touch_conf_user.h` 配置。默认仅读取一个触点并直接把原始坐标交给 LVGL，不自动交换轴、镜像或随屏幕旋转；板子的芯片、I2C、复位/中断和方向须实测。

当前两种 I2C 封装忽略 `bflb_i2c_transfer()` 的返回值，LVGL port 也不检查 touch 初始化/读取返回值，因此上层成功不能证明总线通信成功。调试需独立检查设备和传输，不能把未检测到触点直接归因于图形层。

构建可参考 `examples/display/lvgl_v8` / `lvgl_v9` 的 SPI、DBI 和 BL618DG AP DSI 配置，分别使用独立构建目录；实际颜色、显示刷新、触摸校准、DMA/cache 和帧率需要匹配显示硬件验证，编译通过不代表屏幕点亮。
