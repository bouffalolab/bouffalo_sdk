# 构建系统

## 构建系统概览

Bouffalo SDK 使用混合构建系统：
- **Makefile**: 作为入口和包装器
- **CMake**: 作为实际构建引擎
- **Kconfig**: 作为配置系统

## 构建流程

```
Makefile (用户入口)
   ↓
project.build (Makefile包装器)
   ↓
CMakeLists.txt (CMake配置)
   ↓
board.build (板级配置)
   ↓
Kconfig (配置系统)
   ↓
defconfig (.config → .config.cmake)
   ↓
CMake (生成构建文件)
   ↓
Ninja/Make (实际构建)
```

## 目录结构

```
bouffalo_sdk/
├── Makefile                 # 示例Makefile入口
├── project.build            # 主Makefile包装器
├── board.build              # 板级Makefile
├── CMakeLists.txt           # 主CMakeLists
├── Kconfig                  # 主Kconfig
├── cmake/                   # CMake工具链配置
│   ├── toolchain.cmake     # 工具链配置
│   ├── compiler_flags.cmake # 编译器标志
│   ├── auto_config.cmake   # 自动配置
│   └── bflb_flash.cmake    # Flash配置
├── components/              # 组件CMakeLists
│   └── Kconfig
├── examples/                # 示例
│   └── helloworld/
│       ├── Makefile
│       ├── CMakeLists.txt
│       ├── Kconfig
│       └── defconfig
└── tools/                   # 构建工具
    └── kconfig/            # Kconfig工具
        ├── menuconfig.py
        ├── guiconfig.py
        └── kconfig2cmake.py
```

## Makefile 入口

### 基本结构

示例的Makefile通常很简单：

```makefile
include $(BL_SDK_BASE)/project.build
```

### 常用Make命令

#### 基本构建

```bash
# 进入示例目录
cd examples/helloworld

# 构建BL616
make CHIP=bl616 BOARD=bl616dk

# 构建BL602
make CHIP=bl602 BOARD=bl602dk
```

#### 多核芯片构建

```bash
# AP核心
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap

# NP核心
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np
```

#### 使用Ninja

```bash
# 更快的构建
make ninja CHIP=bl616 BOARD=bl616dk
```

#### 清理

```bash
make clean
```

### Makefile 参数

| 参数 | 说明 | 示例 |
|-----|------|------|
| CHIP | 芯片类型 | bl616, bl602, bl618dg |
| BOARD | 开发板 | bl616dk, bl602dk, bl618dgdk |
| CPU_ID | 核心ID（多核芯片） | ap, np |
| COMX | 串口 | COM5, /dev/ttyUSB0 |
| BAUDRATE | 波特率 | 2000000 |

## project.build 详解

### 主要内容

`project.build` 是Makefile包装器，负责：
1. 包含 `board.build`
2. 配置构建工具
3. 定义构建目标
4. 处理配置变量

### 关键变量

```makefile
# 工具链路径
CROSS_COMPILE ?= riscv64-unknown-elf-
CMAKE = $(BL_SDK_BASE)/tools/cmake/bin/cmake

# 构建目录
BUILD_DIR?=build

# 芯片配置
CHIP ?=
BOARD ?=
CPU_ID ?=
CPU_MODEL ?=a0

# 配置变量
CONFIG_ROMAPI ?=y
CONFIG_DEBUG ?=y
CONFIG_TLSF ?=y
CONFIG_COREDUMP ?=y

# 串口配置
COMX ?=COM5
BAUDRATE ?=2000000
```

### 构建目标

```makefile
# 默认目标（构建）
$(BUILD_DIR): Makefile cmake_cache library_build
	$(MAKE) -C $(BUILD_DIR) pre_build
	$(MAKE) -C $(BUILD_DIR) -j8
	$(MAKE) -C $(BUILD_DIR) post_build
	$(MAKE) -C $(BUILD_DIR) combine

# Ninja构建
ninja: Makefile
	$(CMAKE) -S . -B $(BUILD_DIR) -G Ninja $(cmake_definition)
	cd $(BUILD_DIR) && ninja && ninja combine

# 清理
clean:
	$(RM) $(BUILD_DIR)

# 烧录
flash:
	$(command_flash)
```

## CMakeLists.txt

### 主CMakeLists.txt

```cmake
# 设置最低CMake版本
cmake_minimum_required(VERSION 3.15)

# 项目名称
project(bouffalo_sdk C ASM)

# 加载工具链
include(${CMAKE_CURRENT_LIST_DIR}/cmake/toolchain.cmake)

# 加载编译器标志
include(${CMAKE_CURRENT_LIST_DIR}/cmake/compiler_flags.cmake)

# 加载自动配置
include(${CMAKE_CURRENT_LIST_DIR}/cmake/auto_config.cmake)

# 加载Flash配置
include(${CMAKE_CURRENT_LIST_DIR}/cmake/bflb_flash.cmake)

# 加载项目构建
include(${BL_SDK_BASE}/project.build)
```

### 示例CMakeLists.txt

```cmake
# 示例CMakeLists.txt
cmake_minimum_required(VERSION 3.15)

# 定义可执行文件
add_executable(${PROJECT_NAME}
    main.c
)

# 添加组件
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    bflb_platform
    bflb_common
)
```

## Kconfig 配置系统

### Kconfig 概述

Kconfig是一个配置系统，用于管理SDK的编译选项。

### 主要特性

- **图形化配置**: menuconfig, guiconfig
- **依赖管理**: 自动处理组件依赖
- **配置文件**: defconfig
- **格式转换**: Kconfig → CMake

### menuconfig 使用

```bash
# 启动图形化配置
make menuconfig

# 或使用Python脚本
python3 $(BL_SDK_BASE)/tools/kconfig/menuconfig.py
```

### Kconfig 语法

```kconfig
# 布尔选项
config CONFIG_DEBUG
    bool "Enable debug"
    default y

# 三态选项
config CONFIG_FREERTOS
    bool "Enable FreeRTOS"
    default y

# 字符串选项
config CONFIG_BOARD_NAME
    string "Board name"
    default "bl616dk"

# 数字选项
config CONFIG_HEAP_SIZE
    int "Heap size"
    default 65536

# 选择选项
choice CONFIG_RTOS
    prompt "RTOS selection"
    default CONFIG_FREERTOS

config CONFIG_FREERTOS
    bool "FreeRTOS"

config CONFIG_NUTTX
    bool "NuttX"
endchoice

# 依赖
config CONFIG_BLE
    bool "Enable BLE"
    depends on CONFIG_WIFI

# 选择
config CONFIG_FATFS
    bool "Enable FatFS"
    select CONFIG_LWIP
```

### defconfig 示例

```
# 示例defconfig
CONFIG_DEBUG=y
CONFIG_FREERTOS=y
CONFIG_HEAP_SIZE=65536
CONFIG_BOARD_NAME="bl616dk"
CONFIG_BLE=y
CONFIG_FATFS=y
```

### Kconfig 到 CMake 转换

```bash
# 生成.config.cmake
make menuconfig
python3 $(BL_SDK_BASE)/tools/kconfig/kconfig2cmake.py --config .config --output .config.cmake

# 或使用Make目标
make .config.cmake
```

## 工具链配置

### toolchain.cmake

```cmake
# 交叉编译工具链
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR RISC-V)

# 工具链前缀
set(CROSS_COMPILE ${CROSS_COMPILE})

# 编译器
set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc)
```

### 编译器标志

```cmake
# C标志
add_compile_options(
    -Wall
    -Wextra
    -O2
    -g
    -fdata-sections
    -ffunction-sections
)

# 链接器标志
add_link_options(
    -Wl,--gc-sections
    -Wl,--start-group
    -Wl,--end-group
)
```

## 构建过程详解

### 1. 配置阶段

```bash
# 1. 生成配置
make menuconfig

# 2. 转换Kconfig到CMake
make .config.cmake

# 3. 生成CMake配置
make cmake_cache
```

### 2. 编译阶段

```bash
# 标准构建
make

# 或使用Ninja
make ninja
```

### 3. 后处理阶段

```bash
# 合并固件（自动在构建完成后执行）
make combine
```

## 配置系统工作流

### 配置文件生成流程

```
menuconfig (.config)
   ↓
kconfig2cmake
   ↓
.config.cmake
   ↓
CMake (CMakeCache.txt)
```

### 依赖管理

Kconfig自动处理依赖关系：

```kconfig
# WiFi依赖BLE
config CONFIG_BLE
    bool "Enable BLE"
    depends on CONFIG_WIFI

# 选择BLE时自动启用WiFi
```

### 配置变量传递

```makefile
# Kconfig → .config → .config.cmake → CMake
CONFIG_DEBUG=y  →  set(CONFIG_DEBUG TRUE)  →  add_definitions(-DDEBUG=y)
```

## 组件构建

### 组件CMakeLists.txt

```cmake
# 组件CMakeLists.txt
cmake_minimum_required(VERSION 3.15)

# 定义库
add_library(my_component STATIC
    src/component.c
)

# 头文件
target_include_directories(my_component PUBLIC
    include
)

# 依赖
target_link_libraries(my_component PUBLIC
    bflb_common
)
```

### 组件Kconfig

```kconfig
# 组件Kconfig
config CONFIG_MY_COMPONENT
    bool "Enable my component"
    default y
    depends on CONFIG_FREERTOS
    help
      This is my custom component
```

## 常见构建问题

### 1. 找不到工具链

**问题：**
```
error: cannot find riscv64-unknown-elf-gcc
```

**解决：**
```bash
# 设置工具链路径
export PATH=/path/to/toolchain/bin:$PATH

# 或在Makefile中指定
CROSS_COMPILE=/path/to/toolchain/bin/riscv64-unknown-elf-
```

### 2. Kconfig 错误

**问题：**
```
error: recursive dependency detected
```

**解决：**
检查Kconfig中的依赖关系，确保没有循环依赖。

### 3. CMake 配置错误

**问题：**
```
CMake Error: could not find cache file
```

**解决：**
```bash
# 清理构建目录
make clean

# 重新配置
make menuconfig
make cmake_cache
```

### 4. 链接错误

**问题：**
```
undefined reference to `xxx'
```

**解决：**
1. 检查是否链接了正确的库
2. 检查Kconfig配置是否启用了相关组件
3. 检查组件的CMakeLists.txt

## 最佳实践

### 1. 使用menuconfig

```bash
# 总是使用menuconfig配置
make menuconfig

# 保存配置
make .config.cmake
```

### 2. 清理构建

```bash
# 配置改变后总是清理
make clean

# 或删除构建目录
rm -rf build
```

### 3. 使用Ninja

```bash
# 使用Ninja可以加速构建
make ninja CHIP=bl616 BOARD=bl616dk
```

### 4. 检查配置

```bash
# 查看当前配置
cat .config

# 查看CMake配置
cat build/CMakeCache.txt
```

### 5. 增量构建

```bash
# 只重新构建修改的部分
make -j8
```

## 总结

Bouffalo SDK使用混合构建系统，Makefile作为入口，CMake作为构建引擎，Kconfig作为配置系统。理解三者之间的关系和交互是有效使用SDK的关键。使用menuconfig进行配置，使用make或ninja进行构建。
