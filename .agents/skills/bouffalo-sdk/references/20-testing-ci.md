# 测试框架与 CI

## 按验证层次选择

区分静态核查、主机测试、交叉构建、烧录、设备运行和实际业务验证；每一步只能证明对应范围。先确认芯片、板子、工具链和配置，并将构建输出放入独立目录，避免覆盖已有 `build/`。SDK 的主构建入口及工具链要求见 [06-build-system.md](06-build-system.md)。

本地实机测试（非 Jenkins）还需记录串口、烧录方式和复位/启动模式控制。SDK 开发测试串口工装约定 RTS→CHIP_EN、DTR→BOOT（各芯片通用），烧录后可能停在下载模式，需按[固件打包与烧录](35-firmware-pack-flash.md)的“串口控制脚与正常启动”恢复，再核对应用枚举与日志。USB 设备的枚举和功能测试见 [USB 与 CherryUSB](30-usb-cherryusb.md)。

## 板端 Unity 测试

`tests/` 包含 Unity / Unity Fixture 及驱动测试；`tests/example` 构建后由板端 shell 的 `test` 命令调用 `UnityMain`，不是开机自动运行。示例有故意失败的测试用于演示，不能把它当作应全部通过的产品回归。构建时检查 Shell 是否在目标工程中启用，并以当前 README 和配置为准。`proj.conf` 中的旧设置不能替代当前 Kconfig 生成配置，其他芯片需分别确认。运行硬件测试前确认测试所需接线、板卡、串口和烧录条件；记录完整的测试计数、失败项和原始输出。单独构建成功不代表 Unity 测试已运行。

## 不依赖开发板的测试

在 SDK 根目录，具备主机编译器及所需依赖时可运行：

```bash
# 确认 PATH 中 GCC 是目标 SDK 要求的版本后：
python3 tests/toolchain/test_e907.py
# 使用主机 C 编译器、ASan/UBSan 和临时目录
python3 components/fs/littlefs/tests/run_host_tests.py
```

前者编译并检查 E907 指令编码、启动与 libc 集成；缺 Picolibc 时相应测试会跳过，应查看测试输出，不要只看退出码。后者使用模拟 Flash 与故障注入测试 LittleFS/KV；它不是实机掉电验证。其他按模块选择的入口包括 `bsp/common/audio_player/tests/run_host_tests.py`、`components/multimedia/speexdsp/tests/run_host_tests.py`；依赖和覆盖范围以脚本为准。

## 自动化和 CI

`tools/autotest/` 会生成配置、循环编译、烧录设备、运行并写日志及结果表；它不是只读或纯主机测试。执行前核对配置、真实设备身份、烧录授权及产物位置；不要沿用旧文档中的固定串口。

`tools/CI` 是独立仓库。`config/build_all.tab` 定义应用、路径、配置及芯片，`pipelines/tab2json_build.perl` 按 `BUILD_CHIPS` 等条件展开生成命令，`pipelines/jenkins-build.py` 调用各工程并收集结果。`start_up/bouffalo_sdk_pipeline.sh` 的示例会设置严格编译、生成配置和产物清单；先阅读脚本的写入和构建副作用，不要在现有工作区直接当成无副作用的检查。实际行数会随版本变化，表中一行也可能展开多个目标。

设备侧 Jenkins 用 `config/test_all.groovy` 和 `pipelines/tester/test_template.groovy` 匹配 agent、固件及测试脚本，可能执行烧录。SDK `.github/workflows/sync-*.yml` 主要用于子仓同步，不等于完整 SDK 构建或硬件测试。仓库配置只能说明设计和候选工作流；远端任务是否启用和最近结果须另行核实。
