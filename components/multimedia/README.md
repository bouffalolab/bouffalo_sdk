
# Multimedia System Platform[中文](README_zh.md)

## Overview

**Multimedia** is a software development package provided by Bouffalo Lab for audio-related functionalities, supporting all audio-related chips by Bouffalo Lab. This software package has been widely used in various applications such as home appliances, smart speakers, smart alarm clocks, high-end story machines, toys, doorbells, and more.is a software development package provided by Bouffalo Lab for audio-related functionalities, supporting all audio-related chips by Bouffalo Lab. This software package has been widely used in various applications such as home appliances, smart speakers, smart alarm clocks, high-end story machines, toys, doorbells, and more.

## SDK Architecture

<!-- ![SDK Architecture](multimedia.png) -->

## Audio Encoding/Decoding Format Support

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

Note: √ indicates supported, × indicates not supported, ○ indicates supported but not tested, and - indicates no such peripheral.

## Resource Usage

...

## 代码目录

| Name               | Description                       |
|:----------------------|:-------------------------------|
| aacdec                | AAC decoder                                |
| amrnb                 | AMR-NB decoder                             |
| amrwb                 | AMR-WB decoder                             |
| drv_snd_bl606p        | drv_snd_bl606p decoder                     |
| drv_snd_bl616         | drv_snd_bl616 decoder                      |
| flac                  | FLAC decoder                               |
| ogg                   | Ogg decoder                                |
| opus                  | Opus decoder                               |
| pvmp3dec              | PVMP3 decoder                              |
| sonic                 | Sonic decoder                              |
| speex                 | Speex decoder                              |
| speexdsp              | SpeexDSP decoder                           |
| vorbis                | Vorbis decoder                             |
| algrtc                | Audio algorithms (AEC, VAD, etc.)          |
| uservice              | Essential audio system service             |
| bl606p_player         | BL606P player for BT integration           |
| smart_audio_bl616     | Smart Audio BL616                          |
| minialsa              | Standard audio driver interface (MiniALSA) |
| xav                   | Streaming media service framework          |
| xcodec                | Low-level codec interface                  |
| xport                 | Operating system platform adaptationlayer  |
| xutils                | General utility functions                  |
| bl616_blimpls         | bl616 BT/BLE adapter                       |
| audio_flowctrl_bridge | Audio Stream Control Manager acts as a bridge between BT (Bluetooth) and the player. |
