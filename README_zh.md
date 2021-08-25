[![License](https://img.shields.io/badge/License-Apache--2.0-brightgreen)](LICENSE)
[![Release](https://img.shields.io/github/v/tag/bouffalolab/bl_mcu_sdk?color=s&label=release)]()

[English Version](README.md)

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
│   ├── list
│   ├── memheap
│   ├── misc
│   ├── partition
│   ├── ring_buffer
│   └── soft_crc
├── components
│   ├── ble
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
│   ├── dac
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

- bsp/bsp_common : 存放一些板级相关的常用外设驱动代码
- bsp/board : 存放板级描述文件 `clock_config.h`(描述时钟配置文件) `pinmux_config.h`(描述io功能文件) `peripheral_config.h` (描述外设默认配置文件) 这几个文件共同形成 board 文件夹描述特定应用的所有硬件信息。
- common : 存放一些常见的与硬件无关的算法函数库
- components : 存放第三方库公共组件库
- drivers : 存放博流智能提供的公共芯片底层驱动库
- examples : 存放官方提供的示例代码
- out : 中间文件，用来存放编译链接后的产生的bin文件
- tools : 存放编译下载相关的工具包
- docs : 存放教程文档以及其他帮助信息

更多文件目录参考信息，请查阅 [bl mcu sdk 文档教程](http://bouffalolab.gitee.io/bl_mcu_sdk)
## 层次结构

[![层次结构](https://z3.ax1x.com/2021/06/18/RpUVoj.png)](https://imgtu.com/i/RpUVoj)

# 资源

## 硬件资源

- BL706 IOT 开发板
[![BL706_IOT](https://z3.ax1x.com/2021/08/06/fnPcLT.png)](https://imgtu.com/i/fnPcLT)

- BL706 AVB 开发板
[![BL706_AVB](https://z3.ax1x.com/2021/06/18/Rpa7DO.jpg)](https://imgtu.com/i/Rpa7DO)


目前暂时没有提供官方的购买渠道，如果想获得上述开发板，可到任意开源社区留言申请。
## 芯片手册

[芯片参考手册](https://dev.bouffalolab.com/media/upload/doc/BL702_BL704_706_RM_zh_CN_1.1.pdf)|[芯片数据手册](https://dev.bouffalolab.com/media/upload/doc/BL702_BL704_BL706_DS_zh_CN_Combo_2.0.pdf)

更多资料请访问 BouffaloLab 开发者网站: [https://dev.bouffalolab.com/](https://dev.bouffalolab.com/)

## 文档教程

包含 bl mcu sdk 开发的快速入门教程， api 手册以及驱动框架等详细介绍

- [bl mcu sdk 文档教程](http://bouffalolab.gitee.io/bl_mcu_sdk)

## 视频教程

- [BL706 MCU 开发系列视频教程](https://www.bilibili.com/video/BV1xK4y1P7ur)

## 开发工具

### 命令行开发

命令行开发需要的工具，具体使用方法参考 [linux 开发指南](http://bouffalolab.gitee.io/bl_mcu_sdk/get_started/Linux_quick_start_ubuntu.html)

- [cmake 3.19](https://cmake.org/files/v3.19/)， cmake 编译工具，建议使用 cmake v3.15 以上版本
- [riscv64-unknown-elf-gcc](https://gitee.com/bouffalolab/toolchain_gcc_sifive_linux), risc-v linux 端工具链，下载方式：

```

git clone https://gitee.com/bouffalolab/toolchain_gcc_sifive_linux.git

```

### Eclipse开发

Eclipse 开发需要的工具，具体使用方法参考 [Eclipse 开发指南](http://bouffalolab.gitee.io/bl_mcu_sdk/get_started/Windows_quick_start_eclipse.html)

- [Eclipse](https://dev.bouffalolab.com/media/upload/download/BouffaloLab_eclipse_x86_64_win.zip) Windows 下 eclipse 开发免安装包
- [riscv64-unknown-elf-gcc](https://gitee.com/bouffalolab/toolchain_gcc_sifive_windows), risc-v windows 端工具链，下载方式：

```

git clone https://gitee.com/bouffalolab/toolchain_gcc_sifive_windows.git

```

- [J-Link v10](https://www.segger.com/downloads/jlink)， J-Link 调试器，用于在线调试芯片，建议使用 J-Link V10 以上版本硬件，软件驱动建议使用 V6.98 版本

### CDK开发

CDK 开发需要的工具，具体使用方法参考 [CDK 开发指南](http://bouffalolab.gitee.io/bl_mcu_sdk/get_started/Windows_quick_start_cdk.html)

- [CDK](https://occ.t-head.cn/development/activities/cdk)，平头哥剑池 CDK 集成开发环境，建议使用 CDK v2.8.4 以上版本

### 烧写工具

除了可以使用 CK-link、J-link 以及命令行烧写以外，还支持图形化的烧写工具。
博流提供的图形化烧写工具：

- [Bouffalo Lab Dev Cube For Windows](https://dev.bouffalolab.com/media/upload/download/BouffaloLabDevCube-1.5.3-win32.zip)
- [Bouffalo Lab Dev Cube For Ubuntu](https://dev.bouffalolab.com/media/upload/download/BouffaloLabDevCube-1.5.3-linux-x86.tar.gz)

## 如何将 SDK 作为 submodule 使用

首先使用添加子模块的命令添加到你自己的工程下，然后将 gitmodules 文件提交到远程仓库。

```

git submodule add https://gitee.com/bouffalolab/bl_mcu_sdk.git bl_mcu_sdk
cd bl_mcu_sdk
git pull --rebase
cd ..
git add .gitmodules
git add bl_mcu_sdk
git commit -m "xxx"
git push

```

最终目录呈现的结果如下：

```
.
├── hardware
├── xxxx
├── xxxx
├── xxxx
├── bl_mcu_sdk
├── user_code
│   └── gpio
│       ├── gpio_blink
│       ├── gpio_dht11
│       └── gpio_int

```

### 命令行编译方式

```
    cd bl_mcu_sdk
    make APP=xxx APP_DIR=../user_code
```

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