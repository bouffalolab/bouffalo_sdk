# Smart Audio

[English](README.md) | 简体中文

## 概述

在智能语音控制系统中，语音播放是一个多种操作混杂在一起的复杂场景。具体表现如下：

* 声音来源：可能是本地、云端，也可以是蓝牙音乐、蓝牙打电话等其他播放源
* 声音类型：包括本地提示音, 云端下发TTS(Text-To-Speech), 云端下发音乐链接, 蓝牙播歌, 蓝牙打电话类型; 每种声音类型的处理都略有不同。
* 行为多样：用户会发起多种播放控制请求，例如音量控制，暂停/继续播放，静音控制
* 仲裁管理：当多个播放请求同时产生时，决定播放优先顺序

因此需要一个播放器控制组件，在原始播放器的基础上进行更高层的抽象，一方面根据应用场景实现更复杂的逻辑控制，另一方面也对播放器接口进一步抽象，增加开发者的开发效率。

### 播控协议
播控组件包括多声音类型：

* 系统音：包括系统提示音，以及云端的TTS（Text-To-Speech）声音
* 音乐：包括本地/云端播放的歌曲
* 蓝牙播歌：包括蓝牙播放的歌曲
* 蓝牙打电话：包括接听、拨打电话

smart audio 封装了控制层(smart_audio.c中以smtaudio_开头的接口), 加入至smart audio播控的播放类型必须实现以下属性, 由控制层调用播放类型的具体属性。

* const char *name;                                             |音源名称
* int         prio;                                             |音源优先级(数值越小，优先级越高，数值最小为0)
* int         id;                                               |音源序号
* msp_dlist_t     node;                                             |音源链表节点
* int         status;                                           |音源状态(实时更新的状态)
* int (*init)(void);                                            |音源初始化
* int (*deinit)(void);                                          |音源去初始化
* int (*start)(const char *url, uint64_t seek_time, int resume);|开始播放音源
* int (*pause)(void);                                           |暂停播放音源
* int (*stop)(void);                                            |停止播放音源
* int (*resume)(void);                                          |恢复播放音源
* int (*vol_get)(void);                                         |获取目前音量
* int (*vol_set)(int vol);                                      |设置音量
* int (*vol_up)(int vol);                                       |升高音量
* int (*vol_down)(int vol);                                     |降低音量
* void (*callback)(int type, smtaudio_player_evtid_t evt_id);   |音源发生start、pause、stop等事件，需调用此callback告知播控

每个播放类型均有优先级(prio)属性，模块中蓝牙打电话具有最高优先级(prio为0)，其次是系统音(prio为1), 最后是蓝牙音乐和网络音乐(prio为2) 具体体现在：

* 要播放某种音源时，若高优先级音源正在播放，待播放的音源会暂时添加进 delay play list 中，待高优先级播放完毕后从 delay play list 中取出，然后进行播放。
* 要播放某种音源时，若低优先级音源在播放，会打断正在播放的音源并将被打断的音源加入到 resume play list中，然后播放待播放的音源，播放完毕后从resume play lisy中取出音源，恢复其播放。
* 正在播放音乐(蓝牙或在线音乐)：如果此时要播放系统声音，会”打断“音乐（暂停音乐播放），优先播放系统声音；系统声音播放完成后，在根据情况决定是否恢复音乐播放;
* 对于同优先级的音源，新的音源会打断当前播放音源，并且被打断的音源没法恢复播放。

### 最低播放音量

音源注册接口保留 `min_vol` 参数，用于最低播放音量设计。
当前内置音源实现未应用该参数，提示音注册函数中的最低音量设置也处于注释状态。
因此不能依赖传入 `min_vol` 保证零音量时仍有提示音；如应用需要该行为，
应明确配置并验证音量策略。实际接口及行为以本目录源码为准。

## SDK 集成

组件目录为 `components/multimedia/smart_audio`。在应用的 `defconfig` 中启用：

```makefile
CONFIG_MULTIMEDIA=y
CONFIG_SMART_AUDIO=y
```

应用继续使用 `#include <smart_audio.h>` 和 `smtaudio_*` 接口。
原有 `CONFIG_SMART_AUDIO_BL616` 配置需改为 `CONFIG_SMART_AUDIO`。
若已有 menuconfig 配置，也需要同步更新或重新生成。

播控层不限定芯片；应用需配置适合目标平台的音频后端、板级引脚及编解码器。
本地/网络播放通过媒体框架实现，蓝牙播放通过蓝牙适配接口实现。

## 初始化与音源注册

`smtaudio_init()` 只初始化播控，应用还需注册实际使用的音源。
先完成板级音频初始化及 `snd_card_register()`，再按顺序调用：

```c
#include <smart_audio.h>

int setup_playback(audio_evt_t callback)
{
    int ret = smtaudio_init(callback);
    if (ret < 0) {
        return ret;
    }
    ret = smtaudio_register_local_play(10, NULL, 0, 1.0f, 16000);
    if (ret < 0) {
        return ret;
    }
    return smtaudio_register_online_music(0, NULL, 0, 1.0f, 16000);
}
```

以上为播控初始化片段，要求板级声卡和媒体后端已配置，且只调用一次。
注册参数依次为最低音量、音效配置、配置字节数、初始倍速和重采样率；
目标采样率须与媒体配置及音频后端匹配。最低音量通过音源注册接口传入，
不属于 `smtaudio_init()` 的参数。

| 音源 | 注册接口 | 类型 | 默认优先级 |
| --- | --- | --- | --- |
| 提示音 | `smtaudio_register_local_play()` | `MEDIA_SYSTEM` | 1 |
| 本地/网络音乐 | `smtaudio_register_online_music()` | `MEDIA_MUSIC` | 2 |
| 蓝牙音乐 | `smtaudio_register_bt_a2dp()` | `SMTAUDIO_BT_A2DP` | 2 |
| 蓝牙通话 | `smtaudio_register_bt_hfp()` | `SMTAUDIO_BT_HFP` | 0 |

本地/网络播放还需配置 `CONFIG_USERVICE`、`CONFIG_AV`、音频设备及解码后端，
并在 `multimedia_user.h` 中注册对应解码器、解封装器和流读取器。
蓝牙音源需配套 `CONFIG_BLUETOOTH_PORT`、协议栈和相应 profile。
完整配置见[多媒体总览](../README_CN.md)，实际初始化见
[player_mp3](../../../examples/audio/player_mp3/app/player/app_player.c)。

从 SDK 根目录构建：

```bash
make -C examples/audio/player_mp3 CHIP=bl616 BOARD=bl616dk
make -C examples/audio/player CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

启动失败时检查音源是否注册、声卡是否可打开、媒体后端是否启用，以及文件系统或网络是否就绪。

## 接口列表

| 函数 | 说明 |
| :--- | :--- |
| smtaudio_init | 播控初始化 |
| smtaudio_vol_up | 增加音量 |
| smtaudio_vol_down | 减少音量              |
| smtaudio_vol_set | 设置音量到固定值 |
| smtaudio_vol_get | 获取当前音量          |
| smtaudio_pause | 暂停音乐播放          |
| smtaudio_resume | 恢复音乐播放          |
| smtaudio_mute | 进入/退出静音禁麦状态 |
| smtaudio_start      | 开始播放一段音频      |
| smtaudio_stop | 停止当前音频播放      |
| smtaudio_set_speed | 设置播放速率 |
| smtaudio_get_state | 查看当前播控状态 |
| smtaudio_lpm | 进入/退出低功耗状态 |

## 接口详细说明

### media_evt

`typedef void (*audio_evt_t)(int type, smtaudio_player_evtid_t evt_id);`

- 功能描述:
   - 播放器事件处理回调函数。

- 参数:
   - `type`: 音频类型
   - `evt_id`: 播放器事件

- 返回值:
   - 无。

### smtaudio_init

`int8_t smtaudio_init(audio_evt_t audio_evt_cb);`

- 功能描述:
   - 初始化播控组件。

- 参数:
   - `audio_evt_cb`: 向用户提供的播放事件回调函数。

- 返回值:
   - 0: 初始化成功。
   - -1: 初始化失败。

### smtaudio_vol_up
`int8_t smtaudio_vol_up(int16_t vol);`

- 功能描述:
   - 增加音量。

- 参数:
   - `vol`: 增加音量大小。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### smtaudio_vol_down
`int8_t smtaudio_vol_down(int16_t vol);`

- 功能描述:
   - 减少音量。

- 参数:
   - `vol`: 减少音量大小。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### smtaudio_vol_set
`int8_t smtaudio_vol_set(int16_t vol);`

- 功能描述:
   - 设置音量到固定值。

- 参数:
   - `vol`: 设置音量大小。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### smtaudio_vol_get
`int8_t smtaudio_vol_get(void);`

- 功能描述:
   - 获取当前音量。

- 参数:
   - 无。

- 返回值:
   - 获取到的音量值，范围在0~100。

### smtaudio_pause
`int8_t smtaudio_pause(void);`

- 功能描述:
   - 暂停当前播放。

- 参数:
  - 无。

- 返回值:
   - 0: 暂停成功。
   - -1: 暂停失败。

### smtaudio_resume
`int8_t smtaudio_resume(void);`

- 功能描述:
   - 恢复被暂停的音乐，如果当前没有音乐被暂停，则直接返回。

- 参数:
  - 无。

- 返回值:
   - 0: 恢复成功。
   - -1: 恢复失败。

### smtaudio_mute
`int8_t smtaudio_mute(void);`

- 功能描述:
   - 进入/退出静音禁麦状态。第一次调用进入静音禁麦状态，第二次调用退出静音禁麦状态。

- 参数:
  - 无。

- 返回值:
   - 0: 操作成功。
   - -1: 操作失败。

### smtaudio_start
`int8_t smtaudio_start(int type, char *url, uint64_t seek_time, uint8_t resume);`

- 功能描述:
   - 开始播放一段音频。

- 参数:
   - `type`: 音频类型。
   - `url`：音频内容地址。
   - `seek_time`：播放起始位置，0表示从头播放。
   - `resume`：播放结束后是否恢复播放被打断音乐。

- 返回值:
   - 0: 开始播放成功。
   - -1: 开始播放失败。

### smtaudio_stop
`int8_t smtaudio_stop(int type);`

- 功能描述:
   - 停止当前音频播放。

- 参数:
   - `type`: 音频类型

- 返回值:
   - 0: 停止播放成功。
   - -1: 停止播放失败。
### smtaudio_set_speed
`int smtaudio_set_speed(float speed)`

- 功能描述:
   - 设置播放速率。

- 参数:
   - `speed`: 播放速率，范围0.5 ~ 2.0。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### smtaudio_get_state
`smtaudio_state_t smtaudio_get_state(void);`

- 功能描述:
   - 获取播控状态。

- 参数:
   - 无。

- 返回值:
   - 当前播控状态，具体说明如下：
```c
// SYSTEM状态变化不会改变SMTAUDIO_STATE，只有MUSIC状态会改变
typedef enum {
    SMTAUDIO_STATE_PLAYING,
    SMTAUDIO_STATE_PAUSE,
    SMTAUDIO_STATE_STOP,
    SMTAUDIO_STATE_MUTE,
    SMTAUDIO_STATE_NOINIT,
    SMTAUDIO_STATE_NUM
} smtaudio_state_t;
```

### smtaudio_lpm
`int8_t smtaudio_lpm(uint8_t state);`

- 功能描述:
   - 进入/退出低功耗状态。

- 参数:
   - `state`: 低功耗状态，1 - 进入低功耗，0 - 退出低功耗

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

## 示例

```c
// 回调函数，会注册到播控组件中，用于接收播放事件
static void media_evt(int type, smtaudio_player_evtid_t evt_id)
{
  switch (evt_id) {
        case SMTAUDIO_PLAYER_EVENT_START:
            /* 开始播放事件 */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_ERROR:
            /* 播放错误事件 */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_STOP:
            /* 播放结束事件 */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_PAUSE:
            /* 播放暂停事件 */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_RESUME:
            /* 恢复播放事件 */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_UNDER_RUN:
            /* 播放缓冲数据不足事件 */
            ...
            break;
        case SMTAUDIO_PLAYER_EVENT_OVER_RUN:
            /* 播放缓冲数据溢出事件 */
            ...
            break;
        default:
            break;
    }
}

void main()
{
    ...;
    /* 初始化播控组件 */
    smtaudio_init(media_evt);
    ...
    /* 使用系统声音播放网络流 */
    smtaudio_start(MEDIA_SYSTEM,
                  "https://www.kozco.com/tech/LRMonoPhase4.mp3", 0, 1);
    ...
    /* 获取当前播控组件状态，并控制音量 */
    if (SMTAUDIO_STATE_MUTE != smtaudio_get_state()) {
        smtaudio_vol_up(10);
    }
}
```

## 诊断错误码
无。

## 运行资源

无。

## 依赖资源
  - `xport`：MSP 操作系统和平台适配
  - `xutils`：链表及日志工具
  - `uservice`、`av`：媒体服务和播放框架
  - `CONFIG_BT_A2DP` 启用时：蓝牙适配后端

## 组件参考
无。
