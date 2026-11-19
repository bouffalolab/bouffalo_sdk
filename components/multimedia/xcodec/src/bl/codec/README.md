# Legacy Xcodec — Deprecated / 旧接口已弃用

This directory implements the deprecated `CONFIG_XCODEC` API declared in
[`xcodec.h`](../../../include/xcodec.h). It remains for existing applications.
New applications should use `CONFIG_XCODEC_IO` and
[`xcodec_dev.h`](../../../include/xcodec_dev.h).

本目录是旧 `CONFIG_XCODEC` 接口的实现，**已弃用**，保留供现有工程兼容。
新工程使用 `CONFIG_XCODEC_IO` 和 `xcodec_dev.h`；完整说明见
[English README](../../../README.md) / [中文 README](../../../README_ZN.md)。

## Migration / 迁移

| Legacy API / 旧接口                                       | New API / 新接口                                                              |
| --------------------------------------------------------- | ----------------------------------------------------------------------------- |
| `CONFIG_XCODEC`, `xcodec.h`                               | `CONFIG_XCODEC_IO`, `xcodec_dev.h`                                            |
| `xcodec_init()`, application DMA and hardware hooks       | Board creates Codec and data interfaces; application calls `xcodec_dev_new()` |
| `xcodec_input_config()`, `xcodec_output_config()`         | `xcodec_dev_sample_info_t` supplied to `xcodec_dev_open()`                    |
| `xcodec_input_read()`, `xcodec_output_write()`            | `xcodec_dev_read()`, `xcodec_dev_write()`                                     |
| `xcodec_input_start/stop()`, `xcodec_output_start/stop()` | `xcodec_dev_open()`, `xcodec_dev_close()`                                     |
| Per-channel close and `xcodec_uninit()`                   | Close/delete devices, then release Board-owned interfaces                     |

The APIs are not binary or source compatible. In particular, the new read/write
functions return **0 for a complete transfer or a negative error**, rather than
a byte count. Sizes are bytes and need not match DMA periods or complete PCM
frames. The driver retains partial PCM frames across calls.

新旧 API 的类型、返回值和生命周期不同，不能只替换函数名。旧的异步读写、回调、
HFP 和 EQ/DRC 接口也不属于新设备层的直接替换范围。

GPIO, clocks, DMA and Codec selection move into the Board. Applications use
device handles for PCM IO and controls. Stop IO, check `xcodec_dev_close()`
success, delete devices, and then deinitialize the Board. Shared I2S data
interfaces must be released only once.

GPIO、时钟、DMA 和 Codec 选择交给 Board，应用通过设备句柄处理 PCM 和控制。
停止读写后先关闭设备并检查结果，再删除设备、释放 Board。
不要让新旧实现同时占用同一组外设或 DMA 通道。

See the [xcodec_io example](../../../../../../examples/audio/xcodec_io/README.md)
for the complete recording/playback and cleanup flow.
