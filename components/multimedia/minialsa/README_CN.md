# minialsa

[English](README.md) | 简体中文

minialsa 提供嵌入式 ALSA 风格的 PCM、声卡和 mixer 接口，可直接播放或采集原始 PCM。
压缩文件、网络流播放以及多音源仲裁可使用上层的
[smart_audio](../smart_audio/README_CN.md) 和媒体框架。

## 目录与后端

| 路径 | 作用 |
| --- | --- |
| [include/alsa/pcm.h](include/alsa/pcm.h) | PCM 类型、参数、播放和采集接口 |
| [include/alsa/mixer.h](include/alsa/mixer.h) | mixer 控制项及音量接口 |
| [include/alsa/snd.h](include/alsa/snd.h) | 声卡及后端驱动接口 |
| `src/pcm.c` | PCM 句柄、状态、参数及读写分发 |
| `src/mixer.c`、`src/snd.c`、`src/core.c` | mixer、声卡注册及公共支持 |
| [../drv_snd](../drv_snd/) | 基于 xcodec 的现有 minialsa 后端 |

输出链路为 `应用 → minialsa → drv_snd → xcodec → 板级音频`。
minialsa 本身不配置板级引脚、DMA、时钟和外部 codec。
接口命名遵循 ALSA 习惯，但这是桌面 ALSA 的功能子集，实际支持及行为以头文件和后端实现为准。

## SDK 配置

以 [examples/audio/minialsa](../../../examples/audio/minialsa/) 为基础，启用：

```makefile
CONFIG_FREERTOS=y
CONFIG_MULTIMEDIA=y
CONFIG_MSP_USE_CODEC_ONLY=y
CONFIG_MINIALSA=y
CONFIG_DRV_SND=y
CONFIG_XCODEC=y
CONFIG_XPORT=y
CONFIG_XUTILS=y
```

`CONFIG_MSP_USE_CODEC_ONLY` 精简路径适合直接 PCM 应用；集成完整媒体播放器时应关闭。
目标音频硬件仍需单独配置，参考[多媒体总览](../README_CN.md)及
[板级音频适配](../../../examples/audio/common/multimedia_port_common.c)。

初始化板级资源后，通过 [devices/drv_snd.h](../drv_snd/include/devices/drv_snd.h)
中的 `snd_card_register()` 注册声卡，使用适合实际硬件的 `snd_config_t` 增益配置。
调用 `snd_card_unregister()` 前应关闭全部 PCM 和 mixer 句柄。
现有后端提供播放设备 `pcmP0` 和采集设备 `pcmC0`。

## PCM 使用流程

1. 使用 `msp_pcm_open()` 打开设备，指定 `MSP_PCM_STREAM_PLAYBACK` 或 `MSP_PCM_STREAM_CAPTURE`。
2. 配置访问模式、格式、声道和采样率。基本配置可使用 `msp_pcm_set_params()`；需要明确 period/buffer 大小时，使用 `msp_pcm_hw_params_*` 和 `msp_pcm_hw_params()`。
3. 按需通过 `msp_pcm_sw_params_*` 和 `msp_pcm_sw_params()` 配置软件阈值。
4. 使用 `msp_pcm_writei()` 写入交错 PCM，或通过 `msp_pcm_readi()` / `msp_pcm_readn()` 采集。处理部分传输，并检查负错误码。
5. 播放结束时用 `msp_pcm_drain()` 等待缓冲数据输出，或用 `msp_pcm_drop()` 丢弃待输出数据，最后 `msp_pcm_close()`。

**PCM 帧**包含各声道各一个采样。双声道 16-bit PCM 每帧为 4 字节。
读写长度、成功返回值、`period_size` 和 `buffer_size` 都以帧为单位。
使用 `msp_pcm_bytes_to_frames()` / `msp_pcm_frames_to_bytes()` 换算；
打包格式与带填充格式的实际存储位宽可能不同。

`msp_pcm_set_params()` 的 `latency` 单位为**微秒**，例如 `200000` 表示 200 ms。
当前实现忽略 `soft_resample` 参数。支持的格式、采样率、声道及 DMA 对齐约束由后端决定。

### 播放示例

以下函数要求板级音频和声卡已初始化，输入为 16 kHz、16-bit 有符号单声道 PCM。
该单声道格式下，`frames` 等于采样个数。

```c
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <alsa/pcm.h>

int play_pcm16_mono(const int16_t *samples, size_t frames)
{
    msp_pcm_t *pcm = NULL;
    int ret = msp_pcm_open(&pcm, "pcmP0", MSP_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        return ret;
    }

    ret = msp_pcm_set_params(pcm, SND_PCM_FORMAT_S16_LE,
                            MSP_PCM_ACCESS_RW_INTERLEAVED,
                            1, 16000, 0, 200000);
    if (ret < 0) {
        goto done;
    }

    while (frames > 0) {
        msp_pcm_sframes_t written = msp_pcm_writei(pcm, samples, frames);
        if (written <= 0) {
            ret = written < 0 ? (int)written : -EIO;
            goto done;
        }
        samples += written;
        frames -= written;
    }
    ret = msp_pcm_drain(pcm);

done:
    {
        int close_ret = msp_pcm_close(pcm);
        return ret < 0 ? ret : close_ret;
    }
}
```

采集时，以 `MSP_PCM_STREAM_CAPTURE` 打开 `pcmC0`，配置后端支持的输入参数，
再通过 `msp_pcm_readi()` 循环读入可写缓冲区。
使用 `msp_pcm_readn()` 时，按选定访问模式为每个声道提供缓冲区。
采集还需配置有效的 ADC/麦克风或 I2S 输入。

## Mixer 与错误处理

通过 `msp_mixer_open()` 创建 mixer，用 `msp_mixer_attach()` 连接后端声卡，
再用 `msp_mixer_load()` 加载控制项。枚举控制项后，可使用 `msp_mixer_selem_*`
查询范围并设置播放/采集音量。控制项名称、范围及 dB 映射由后端决定。
使用完毕调用 `msp_mixer_close()`。

出现 `-EPIPE`（播放欠载或采集溢出）时，可调用 `msp_pcm_recover(pcm, error, 0)`，
检查成功后再重试。当前恢复流程会重新打开设备并应用硬件参数，可能打断已排队音频；
其他错误直接返回给调用者。按后端行为使用 `msp_pcm_avail()` / `msp_pcm_wait()` 控制供数节奏。

## 构建与硬件验证

在 SDK 根目录执行：

```bash
make -C examples/audio/minialsa CHIP=bl616 BOARD=bl616dk
```

烧录并运行示例后，在串口 shell 中执行：

```text
minialsa_play wa
minialsa_play sin
minialsa_api_test all wa
minialsa_api_test playback sin
minialsa_api_test capture
minialsa_output_clock_test 48000
```

`wa` 和 `sin` 为内置 16 kHz、16-bit、单声道样本。采集需连接有效输入。
时钟测试持续输出静音，方便示波器测量，采样率要求为 8000 Hz 的整数倍。
参数及检查项见 [app_minialsa.c](../../../examples/audio/minialsa/app/player/app_minialsa.c)。
编译可验证集成，实际播放、采集及音频时钟精度仍需在板上验证。

打开设备失败时，检查声卡注册和设备名；无声时检查 PA、静音、音量、codec 初始化及
I2S 引脚/时钟；播放断续时检查供数、调度、period/buffer 大小及帧单位。
