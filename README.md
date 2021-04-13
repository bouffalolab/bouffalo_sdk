# 简介

**bl mcu sdk** 是博流智能科技团队专为 BL602/BL604,BL702/BL704/BL706 以及未来其他系列芯片提供的 MCU 软件开发包。

## 代码框架

```

bl_mcu_sdk
├── bsp
│   ├── board
│   │   ├── bl706_avb
│   │   ├── bl706_iot
│   │   └── bl602_xxx
│   └── bsp_common
├── common
│   ├── device
│   ├── libc
│   ├── list
│   ├── memheap
│   ├── misc
│   ├── partition
│   ├── ring_buffer
│   └── soft_crc
├── components
│   ├── fatfs
│   ├── freertos
│   ├── shell
│   └── usb_stack
├── docs
│   ├── bl70x_docs
│   ├── development_guide
├── drivers
│   ├── bl602_driver
│   └── bl702_driver
├── examples
│   ├── adc
│   ├── audio
│   ├── camera
│   ├── coremark
│   ├── dac
│   ├── dhrystone
│   ├── dma
│   ├── gpio
│   ├── hellowd
│   ├── i2c
│   ├── i2s
│   ├── memheap
│   ├── spi
│   ├── uart
│   └── usb
├── out
└── tools
    ├── bflb_flash_tool
    ├── cdk_flashloader
    ├── cmake
    └── openocd
    
```

- bsp 存放板级相关的配置文件和驱动。
- common 存放一些常见的函数库
- components 存放第三方库
- drivers 存放芯片外设驱动库
- examples 存放外设 demo
- out 存放目标文件输出路径
- tools 存放一些工具包

# 资源

## 硬件资源

- BL706 IOT 开发板
- BL706 AVB 开发板

## 芯片手册

[芯片参考手册](https://dev.bouffalolab.com/media/upload/doc/BL702_BL704_706_RM_zh_CN_1.1.pdf)|[芯片数据手册](https://dev.bouffalolab.com/media/upload/doc/BL702_BL704_BL706_DS_zh_CN_Combo_1.7.pdf)

更多资料请访问 BouffaloLab 开发者网站: [https://dev.bouffalolab.com/](https://dev.bouffalolab.com/)

## 文档教程

[bl mcu sdk 文档教程](http://bouffalolab.gitee.io/bl_mcu_sdk)

## 视频教程

- [五分钟带你了解 RISC-V 的指令集架构](https://www.bilibili.com/video/BV1aA411L77o)
- [BL706 开发板基于 Eclipse 的环境搭建](https://www.bilibili.com/video/BV1HK4y1o7P1)
- [BL706 开发板基于 CDK 的环境搭建](https://www.bilibili.com/video/BV14U4y1h7d2)

## 工具

- [Eclipse](https://dev.bouffalolab.com/media/upload/download/BouffaloLab_eclipse_x86_64_win.zip) Windows 下 eclipse 开发免安装包
- [CDK](https://occ.t-head.cn/development/activities/cdk) ，平头哥剑池 CDK 集成开发环境，建议使用 CDK v2.8.4 以上版本
- [cmake 3.19](https://cmake.org/files/v3.19/) ， cmake 编译工具，建议使用 cmake v3.19 以上版本
- [J-Link v10](https://www.segger.com/downloads/jlink)， J-Link 调试器，用于在线调试芯片，建议使用 J-Link V10 以上版本硬件，软件驱动建议使用 V6.98 版本

博流提供的图形化烧写工具：
- [Bouffalo Lab Dev Cube For Windows](https://dev.bouffalolab.com/media/upload/download/BouffaloLabDevCube-1.5.0-win32.zip)
- [Bouffalo Lab Dev Cube For Ubuntu](https://dev.bouffalolab.com/media/upload/download/bouffalo_build_tool_for_ubuntu_v1.0.0.rar)

## 论坛

博流开发者交流论坛: [https://bbs.bouffalolab.com/](https://bbs.bouffalolab.com/)

# 许可协议

**bl mcu sdk** 完全开源，遵循 Apache License 2.0 开源许可协议，可以免费在商业产品中使用，并且不需要公开私有代码。

```
/*
 * Copyright (c) 2021 Bouffalolab team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 ```