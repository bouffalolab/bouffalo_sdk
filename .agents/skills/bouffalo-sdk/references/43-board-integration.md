# 板级复用示例：BL618DG Orion

`bsp/board/bl618dg_orion/` 是一个板级复用示例：其 CMake 复用 `bsp/board/bl618dgdk/` 的部分配置并替换 GPIO 实现；当前板级入口要求 BL618DG 的 AP 核。查看该板的 `CMakeLists.txt`、`Kconfig.defconfig`、`board_config.h` 和 `board_audio.c`，以确认引脚、codec、DMA、I2C/I2S 和资源使用；这些定义属于该板，不能推广到其他硬件。

音频使用示例见 `examples/audio/xcodec_io/`。板级目录选择见 `bsp/board/CMakeLists.txt`；通用架构见 [02-architecture.md](02-architecture.md)，音频组件见 [32-audio-video-media.md](32-audio-video-media.md)。
