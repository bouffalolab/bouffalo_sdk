# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

这是 Bouffalo Lab 的 Bouffalo SDK，一个支持所有 Bouffalo 芯片系列的 IoT 和 MCU 软件开发套件。该项目结合了 bl_mcu_sdk 和 bl_iot_sdk。

## 常用构建命令

### 基本构建
```bash
# 进入示例目录并构建
cd examples/helloworld
make CHIP=bl616 BOARD=bl616dk

# 使用 ninja 构建
make ninja CHIP=bl616 BOARD=bl616dk

# 多核芯片构建（如 BL808 或 BL606P）
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0
make CHIP=bl808 BOARD=bl808dk CPU_ID=d0
```

### 烧录命令
```bash
# 基本烧录
make flash CHIP=bl616 COMX=/dev/ttyUSB0

# 多核芯片烧录
make flash CHIP=bl808 CPU_ID=m0 COMX=/dev/ttyUSB0
```

### 支持的芯片类型
- BL602/BL604
- BL702/BL704/BL706
- BL616/BL618
- BL808
- BL606P

## 项目架构

### 核心目录结构
- **drivers/lhal/** - 低级别硬件抽象层（LHAL），为所有芯片提供通用外设驱动
- **drivers/soc/** - 芯片特定的非通用外设驱动
- **components/** - 可重用组件（文件系统、网络协议栈、多媒体等）
- **bsp/** - 板级支持包（时钟、引脚复用、内存堆和控制台初始化）
- **examples/** - 官方示例代码
- **tools/** - 开发工具

### LHAL 架构
LHAL（Low Level HAL）是 Bouffalo Lab 设计的通用外设低级别硬件抽象层，支持所有 Bouffalo 芯片使用相同的 API。

### 支持的无线协议
- WiFi 4/6
- Bluetooth/BLE
- Zigbee
- Thread

## 开发环境设置

### 工具链要求
- riscv64-unknown-elf-gcc 工具链
- make 或 ninja 构建工具
- CKLink 调试器（用于调试）

### 配置系统
- 使用 Kconfig 进行配置管理
- 支持 menuconfig 进行图形化配置

## 测试和验证

### 单元测试
- 位于 `tests/` 目录
- 使用 Unity 测试框架
- 驱动程序测试在 `tests/drivers/` 下

### 示例验证
- 每个外设都有对应的示例代码
- 从基础的 helloworld 到复杂的外设示例

## 调试支持

### 调试工具
- 仅支持 CKLink 调试器
- 提供 GDB 初始化脚本在 `tools/gdbinit/` 目录
- Eclipse GDB 启动配置在 `tools/eclipse_gdb_launch/` 目录

### 崩溃转储
- 支持崩溃转储功能
- 工具位于 `tools/crash_tools/` 目录

## 特殊注意事项

### 多核支持
- BL808 和 BL606P 为多核芯片
- 需要指定 CPU_ID（m0、d0 等）
- 每个核心有独立的启动代码和链接脚本

### 内存管理
- 支持 PSRAM（伪静态 RAM）
- 提供多种内存管理方案（TLSF 等）
- 支持不同的内存布局配置

### 文件系统支持
- FATFS
- LittleFS
- ROMFS

## 工具链文件位置
- 项目根目录包含 `compile_commands.json`（用于 IDE 集成）
- CMake 配置文件在 `cmake/` 目录
- 工具链配置在 `cmake/toolchain.cmake`