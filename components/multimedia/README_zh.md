
# 多媒体系统平台[English](README.md)

## 概述

**Multimedia** 是 Bouffalo Lab 提供的 音频相关 软件开发包，支持博流智能所有音频相关芯片。该软件包已经在家电、智能音箱、智能闹钟、高端故事机、玩具、门铃等应用领域大批出货。

## SDK 架构

<!-- ![SDK Architecture](multimedia.png) -->

## 音频编/解码格式支持情况

|   Format     |  BL606P/BL808  | BL616/BL618 |
|:------------:|:--------------:|:-----------:|
|  pcm         |      √         |      √      |
|  wav         |      √         |      √      |
|  mp3         |      √         |      √      |
|  mp4/m4a     |      √         |      √      |
|  aac         |      √         |      √      |
|  amr         |      √         |      √      |
|  flac        |      √         |      √      |
|  HLS         |      √         |      √      |
|  m3u8        |      √         |      √      |
|  MPEG-Dash   |      √         |      ○      |
|  ogg         |      √         |      √      |
|  opus        |      √         |      √      |
|  vorbis      |      √         |      √      |
|  speex       |      √         |      √      |
|  sbc         |      √         |      √      |

备注：**√** 表示已支持；**×** 表示未支持；**○** 表示已支持但未测试；**-** 表示没有该外设。

## 资源占用情况

...

## 代码目录

| 名称                  | 描述                          |
|:----------------------|:------------------------------|
| aacdec                | aac 解码器                     |
| amrnb                 | amrnb 解码器                   |
| amrwb                 | amrwb 解码器                   |
| drv_snd_bl606p        | drv_snd_bl606p 解码器          |
| drv_snd_bl616         | drv_snd_bl616 解码器           |
| flac                  | flac 解码器                    |
| ogg                   | ogg 解码器                     |
| opus                  | opus 解码器                    |
| pvmp3dec              | pvmp3dec 解码器                |
| sonic                 | sonic 解码器                   |
| speex                 | speex 解码器                   |
| speexdsp              | speexdsp 解码器                |
| vorbis                | vorbis 解码器                  |
| algrtc                | algrtc aec、vad等音频算法       |
| uservice              | uservice 音频系统必要的服务组件   |
| bl606p_player         | bl606p_player 对接bt           |
| smart_audio_bl616     | smart_audio_bl616             |
| minialsa              | minialsa 标准的音频驱动接口      |
| xav                   | 流媒体服务框架                   |
| xcodec                | 底层codeec接口                  |
| xport                 | 操作系统平台适配层                |
| xutils                | 通用的工具集                     |
| bl616_blimpls         | bl616蓝牙的适配层                |
| audio_flowctrl_bridge | 音频流控管理器，是bt和player的桥梁 |

