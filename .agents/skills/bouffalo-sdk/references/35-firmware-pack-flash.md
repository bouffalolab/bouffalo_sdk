# 固件打包与烧录

路径相对 SDK 根目录。先确认目标芯片、板、核心、工具链、构建目录和项目的设备操作规则。固件生成、串口写入及启动验证是不同步骤。

## 产物与配置

`project.build` 的标准流程是 pre_build、编译、post_build、combine；`build_out/` 包含 ELF、MAP、应用 BIN 等。完整工具目录存在时由 `bflb_fw_post_proc` 更新固件头及相关打包；`CONFIG_SKIP_COMBINE` 会跳过后处理。`--appkeys=shared` 不能单独证明镜像已签名或加密；密钥、证书和启用配置都需核对，不能将敏感值写入日志或文档。

CMake 配置阶段的 `tools/bflb_tools/flash_prog_cfg.py` 根据分区 TOML 写 `<BUILD_DIR>/flash_prog_cfg.ini`。标准应用配置 boot2、partition 和 FW；直接启动 FW 使用地址 0。INI 的 `filedir` 是相对 INI 目录生成，boot2 可能包含通配符；生成器不验证镜像文件存在。生成 INI 会覆盖指定输出文件，应使用隔离输出并核对实际分区地址、文件及通配符匹配。

BL618DG 的构建区分 AP、NP、LP；AP 与 NP 分别构建不等于已经合并。通用双核 `tools/make/dualcore.mk` 先 NP 后 AP，将 NP 作为 AP 配置输入；其他工程可能有自定义 Makefile。`tools/byai/multi_bins.py --show <bin>` 可离线显示 SELF 和其他段描述符，不修改输入；输出中 NP/LP 地址为零不能宣称已附加相应固件。该工具的部分解析错误只打印错误而仍退出 0，应同时检查输出与描述符。追加操作会写镜像，需使用明确授权的输出路径。

## 烧录边界

常规 `make flash` 直接调用 FlashCube，默认波特率 2000000、默认 COM5，读取现有构建目录的 `flash_prog_cfg.ini`，不自动重新构建。必须显式检查 CHIP、BOARD、CPU_ID、BUILD_DIR 和目标端口，核实所选文件与擦除范围。典型 Linux 二进制是 `tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu`，选项以 `tools/bflb_tools/project_flash.build` 为准。

`make whole` 会调用 FlashCube 的构建模式并写共享工具目录和构建目录；不能视作纯只读。`make ram`、`make flash` 涉及设备，`make efuse` 涉及 eFuse；部分多核例程的 flash 采用整片镜像。执行前按项目规则确认设备和操作权限。`sdk_set_flash_erase` 的 0/1/2 表示不擦除、擦除烧录区域、整片擦除；实际使用前核对生成 INI。

离线可先检查分区 TOML、INI、镜像路径和 `multi_bins.py --show` 的输出；镜像成功生成、烧录成功与目标启动成功必须分别记录。不要把这些检查当作签名验证或硬件运行结果。

## 串口控制脚与正常启动（SDK 开发测试约定）

SDK 开发测试环节使用的 USB 串口工装统一约定：

- RTS → CHIP_EN
- DTR → BOOT

该约定适用于 SDK 各芯片的开发/测试板，不是 BL616 特有。FlashCube 通过串口烧录时会自行驱动这些脚进入下载模式；烧录结束后芯片可能仍停留在 boot2/ISP 模式（USB 枚举成下载设备的 CDC 口，而不是应用固件；例如 BL616 测试台观察为 `349b:6160` "Bouffalo CDC DEMO"）。可用 `lsusb` 对比应用固件的 VID/PID 确认所处模式。需要显式释放复位并退出 boot 模式，例如：

```bash
picocom -b 2000000 --raise-dtr --lower-rts /dev/ttyUSB0
```

即 DTR=1、RTS=0。等价的 pyserial 设置：

```python
import serial
s = serial.Serial('/dev/ttyUSB0', 2000000)
s.dtr = True
s.rts = False
```

该动作只负责把芯片放回正常固件启动；应用运行后串口不必持续占用（USB、网络等功能不依赖持有该进程）。恢复正常启动后应再次检查枚举状态（下载模式 CDC 设备消失、应用设备出现），再执行功能测试。串口同时用于日志时可保持打开，但不要假设每次关闭都会复位芯片，实际行为取决于 HUPCL 和板级电路。

该约定用于 SDK 开发/测试板；客户板或第三方板不一定采用同一工装，可能使用另一路串口、反相电平或没有自动复位电路。对非 SDK 开发测试板，操作前仍需查原理图确认 RTS/DTR 与 CHIP_EN/BOOT 的对应关系，避免把芯片置于复位或下载模式。

本地复现烧录时先构建再烧录；`make flash` 不自动重新编译（见上一节）：

```bash
make CHIP=bl616 BOARD=bl616dk
make flash CHIP=bl616 BOARD=bl616dk COMX=/dev/ttyUSB0 BAUDRATE=2000000
```
