# GCOV 覆盖率报告生成指南

## 快速开始

### 1. 准备数据

```bash
# 在板子上运行程序并触发 GCOV dump
bouffalo /> fatfs_init
bouffalo /> gcov_dump

# 查看 SD 卡中的文件
bouffalo /> show
```

### 2. 复制 .gcda 文件到 PC

```bash
# 假设 SD 卡挂载在 /mnt/sd
cd /home/wyb/repo/new/bouffalo_sdk/examples/wifi/sta/wifi_gcov_dump
mkdir -p gcov_data
cp /mnt/sd/*.gcda gcov_data/
```

### 3. 生成覆盖率报告

```bash
# 运行脚本
./gcov_generate_report.sh
```

脚本会自动完成以下操作：
1. ✅ 从 build 目录复制所有 .gcno 文件
2. ✅ 重命名并复制 .gcda 文件（去掉 `#` 路径前缀）
3. ✅ 使用交叉编译的 gcov 工具生成 .gcov 文件
4. ⚠️ 生成 lcov .info 数据文件（需要安装 lcov）
5. ⚠️ 使用 genhtml 生成 HTML 报告（需要安装 lcov）

### 4. 查看覆盖率报告

#### 方法 1: 直接查看 .gcov 文件（不需要额外工具）

```bash
# 查看特定文件的覆盖率
cat gcov_report/bl616.c.gcov | less

# 解释输出：
# #####     - 该行未被执行
# 12345     - 该行被执行了 12345 次
# -:        - 该行不可执行（注释、空行、声明等）
```

#### 方法 2: 安装 lcov 生成 HTML 报告（推荐）

```bash
# 安装 lcov
sudo apt-get install lcov

# 重新运行脚本（会自动生成 HTML 报告）
./gcov_generate_report.sh

# 查看 HTML 报告
firefox gcov_report/html/index.html
```

## 详细步骤

### 步骤 1: 为库添加 GCOV 编译选项

要让特定库生成代码覆盖率数据，需要在库的 `CMakeLists.txt` 中添加 GCOV 编译选项。

#### GCOV 编译选项说明

| 选项 | 说明 |
|------|------|
| `--coverage` | 启用代码覆盖率（等同于 `-fprofile-arcs -ftest-coverage`） |
| `-fprofile-dir=/sd` | 指定 .gcda 文件输出到 SD 卡的 `/sd` 目录 |
| `-fprofile-info-section` | 将 GCOV 信息放到专门的 ELF section 中 |

#### 方法 1: 为独立库添加（标准组件库）

对于使用 `sdk_generate_library()` 的标准组件，在组件的 `CMakeLists.txt` 中添加：

```cmake
# 示例：为 wifi6 组件添加 GCOV 支持
# components/wireless/wifi6/CMakeLists.txt

# 方式 A: 添加私有编译选项（仅影响当前库）
sdk_add_private_compile_options(
    --coverage
    -fprofile-dir=/sd
    -fprofile-info-section
)
```

#### 方法 2: 为独立 CMake 项目添加（如 macsw）

对于使用原生 CMake 的独立库，参考 `macsw` 组件的实现：

```cmake
# 示例：components/wireless/macsw/CMakeLists.txt

# 定义库的通用编译选项
set(MYLIB_COMMON_OPTIONS
    --coverage                    # 启用 GCOV
    -fprofile-dir=/sd             # 输出到 SD 卡
    -fprofile-info-section        # 使用独立 section
    -Os                           # 其他优化选项...
    -fno-builtin
    # ... 其他选项
)

# 创建库
add_library(${LIBRARY_NAME} STATIC)

# 将编译选项应用到库
target_compile_options(${LIBRARY_NAME} PRIVATE ${MYLIB_COMMON_OPTIONS})
```

#### 方法 3: 为主程序添加

在示例程序的主 `CMakeLists.txt` 中添加：

```cmake
# 示例：examples/wifi/sta/wifi_gcov_dump/CMakeLists.txt

# 为当前组件（通常是 app 库）添加 GCOV 选项
sdk_add_private_compile_options(
    --coverage
    -fprofile-dir=/sd
    -fprofile-info-section
)
```

#### 完整示例

参考 `components/wireless/macsw/CMakeLists.txt:47-72` 中的实现：

```cmake
set(MACSW_COMMON_OPTIONS
    --coverage                # 第 48 行：启用代码覆盖率
    -fprofile-dir=/sd         # 第 49 行：指定输出目录
    -fprofile-info-section    # 第 50 行：使用独立 section
    -Os
    -fno-builtin
    -g
    # ... 其他选项
)

# 应用到主库（第 119 行）
target_compile_options(${LIBRARY_NAME} PRIVATE ${MACSW_COMMON_OPTIONS})

# 应用到配置库（第 272 行）
target_compile_options(${MACSW_CONFIG_LIB_NAME} PRIVATE ${MACSW_COMMON_OPTIONS})
```

#### 注意事项

1. **路径前缀**: `-fprofile-dir=/sd` 指定 SD 卡挂载点，与 `gcov_flush.c` 中的 `__gcov_root = "/sd"` 配合使用
2. **Section 存储**: `-fprofile-info-section` 将 GCOV 元数据放入 ELF section，减少 RAM 占用
3. **链接顺序**: 使用 GCOV 的库需要在链接时包含 `libgcov.a`（由 `--coverage` 自动处理）
4. **性能影响**: GCOV 会增加代码大小和运行时开销，仅用于测试/调试

### 步骤 2: 编译项目

```bash
cd /home/wyb/repo/new/bouffalo_sdk/examples/wifi/sta/wifi_gcov_dump
make CHIP=bl616 BOARD=bl616dk
```

### 步骤 3: 烧录并运行

```bash
make flash CHIP=bl616 COMX=/dev/ttyUSB0

# 在串口中执行
fatfs_init
gcov_dump
show  # 查看 .gcda 文件
```

### 步骤 4: 复制数据到 PC

```bash
# 创建数据目录
mkdir -p gcov_data

# 复制 .gcda 文件（假设 SD 卡挂载在 /dev/sdd1）
sudo mount /dev/sdd1 /mnt/sd
cp /mnt/sd/*.gcda gcov_data/
sudo umount /mnt/sd
```

### 步骤 5: 运行生成脚本

```bash
./gcov_generate_report.sh
```

脚本会自动完成以下操作：
1. ✅ 从 build 目录复制所有 .gcno 文件
2. ✅ 重命名并复制 .gcda 文件（将 `#` 替换为 `/`）
3. ✅ 使用交叉编译的 gcov 工具生成 .gcov 文件
4. ✅ 生成 lcov .info 数据文件
5. ✅ 使用 genhtml 生成 HTML 报告

## 输出文件结构

```
gcov_report/
├── gcno_files/          # .gcno 文件（编译时生成）
│   ├── bl616.c.gcno
│   ├── ftm_task.c.gcno
│   └── ...
├── gcda_files/          # .gcda 文件（运行时生成）
│   ├── /sd/home/wyb/repo/.../bl616.c.gcda
│   └── ...
├── *.gcov              # gcov 生成的覆盖率数据
├── coverage.info       # lcov 合并的数据
└── html/               # HTML 报告
    └── index.html      # 打开这个文件查看报告
```

## 安装依赖工具

### Ubuntu/Debian

```bash
# 安装 lcov 和 genhtml
sudo apt-get install lcov

# 安装浏览器（可选）
sudo apt-get install firefox
```

### CentOS/RHEL

```bash
# 安装 lcov 和 genhtml
sudo yum install lcov

# 或使用 EPEL
sudo yum install epel-release
sudo yum install lcov
```

## 手动生成报告（可选）

如果脚本失败，可以手动执行：

### 方法 1: 使用 gcov

```bash
cd gcov_report

# 为单个文件生成覆盖率
riscv64-zephyr-elf-gcov -o bl616.c.gcov gcno_files/bl616.c.gcno gcda_files/bl616.c.gcda

# 查看覆盖率
riscv64-zephyr-elf-gcov bl616.c.gcov | less
```

### 方法 2: 使用 lcov

```bash
cd gcov_report

# 生成 lcov 数据文件
lcov --capture --directory . --output-file coverage.info \
    --gcov-tool riscv64-zephyr-elf-gcov

# 生成 HTML 报告
genhtml coverage.info --output-directory html
```

## 查看报告

```bash
# 使用 Firefox
firefox gcov_report/html/index.html

# 使用 Chrome
google-chrome gcov_report/html/index.html

# 使用默认浏览器
xdg-open gcov_report/html/index.html
```

## 报告内容

HTML 报告包含：
- **总体覆盖率**: 行覆盖率、分支覆盖率、函数覆盖率
- **目录视图**: 按目录浏览代码覆盖率
- **文件视图**: 查看每个文件的详细覆盖率
- **源代码视图**: 点击文件名查看哪些行被执行、哪些未执行

## 常见问题

### Q1: gcov 报告 "stamp mismatch"

**原因**: .gcno 和 .gcda 文件不匹配

**解决**: 重新编译并运行完整的测试流程
```bash
make clean
make CHIP=bl616 BOARD=bl616dk
make flash
# 运行并重新生成 .gcda 文件
```

### Q2: 找不到 .gcno 文件

**原因**: .gcno 文件在 `build/build_macsw/` 目录

**解决**: 脚本会自动从正确的目录复制

### Q3: lcov 报告 "gcov tool not found"

**原因**: 交叉编译的 gcov 不在 PATH 中

**解决**: 在脚本中设置工具链路径
```bash
export PATH="/data/riscv64-zephyr-elf/bin:$PATH"
```

### Q4: HTML 报告为空

**原因**: .gcda 文件为 0 字节（没有实际数据）

**解决**:
- 检查是否正确调用了 `gcov_dump`
- 检查 __gcov_root 是否为 NULL
- 检查 FatFS 是否正确初始化

## 高级用法

### 只生成特定模块的覆盖率

```bash
cd gcov_report

# 只生成 macsw 模块的覆盖率
lcov --capture --directory gcno_files --output-file macsw.info \
    --gcov-tool riscv64-zephyr-elf-gcov \
    --pattern "*/macsw/*"

genhtml macsw.info --output-directory html_macsw
```

### 合并多次运行的覆盖率数据

```bash
# 第一次运行
lcov --capture --directory . --output-file run1.info

# 第二次运行
lcov --capture --directory . --output-file run2.info

# 合并
lcov --add run1.info --add run2.info --output-file total.info
```

### 过滤不需要的文件

```bash
# 排除测试文件的覆盖率
lcov --remove coverage.info "*/tests/*" --output-file coverage_filtered.info

# 只查看特定目录
lcov --extract coverage.info "*/src/*" --output-file coverage_src.info
```

## 参考资料

- [GCOV 文档](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html)
- [LCOV 文档](http://ltp.sourceforge.net/coverage/lcov.php)
- [genhtml 文档](http://ltp.sourceforge.net/coverage/lcov.php)
