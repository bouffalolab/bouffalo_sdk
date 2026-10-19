# libfvad

[English](README.md) | 简体中文

libfvad 将 WebRTC 的语音活动检测（VAD）引擎提取为独立库，用于判断 PCM 帧中是否存在语音。
它提供检测结果，不负责音频采集、解码或播放。公开接口见 [include/fvad.h](include/fvad.h)。

## Bouffalo SDK 集成

在应用配置中启用：

```makefile
CONFIG_MULTIMEDIA=y
CONFIG_LIBFVAD=y
```

SDK 通过 CMake 构建随附源码；下方 Autotools 命令仅用于独立主机版本。

1. `fvad_new()` 创建实例，检查返回值。
2. `fvad_set_mode()` 设置 0–3 的检测模式，数值越大判定越严格，也可能漏检更多语音。
3. `fvad_set_sample_rate()` 设置采样率，支持 8000、16000、32000、48000 Hz。
4. 对有符号 16-bit 单声道 PCM 调用 `fvad_process()`，长度以采样个数计。
5. 结束后调用 `fvad_free()`；复用时可通过 `fvad_reset()` 重置实例。

每帧必须为 10、20 或 30 ms。例如 16 kHz、20 ms 对应 320 个采样。
`fvad_process()` 返回 1 表示语音、0 表示非语音、-1 表示帧长无效；配置函数也需检查返回值。
音频采集可使用 [minialsa](../minialsa/README_CN.md)，并将输入转换为所需的单声道格式。

## 独立构建与开发

```bash
autoreconf -i
./configure
make
make check
```

从 Git 源码构建时，`autoreconf -i` 用于生成 configure，需要 autoconf、libtool 和 pkg-config。
发布包若已包含 configure，可省略该步骤。安装可运行 `sudo make install`。
启用 `./configure --enable-examples` 可构建示例，需要 libsndfile。
推荐编译告警参数为 `-std=c11 -Wall -Wextra -Wpedantic`。

## 上游维护

源码主要来自 WebRTC 的 `common_audio/vad` 及其依赖的信号处理代码。
上游项目通过 `upstream-import`、`upstream-renamed` 分支及 `tools/import.sh`
管理导入及文件改名，再合并到主分支。完整维护说明见[英文版](README.md)。
版权、许可及第三方声明以本目录随附文件为准。
