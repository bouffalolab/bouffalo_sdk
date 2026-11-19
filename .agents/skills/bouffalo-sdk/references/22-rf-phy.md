# RF 参数、PHY 与产测边界

## 构建与初始化

`CONFIG_RF` 按芯片启用 RF 参数适配和 PHY；`CONFIG_PHY_LPFW` 等选项也会影响 PHY 构建。BL702/BL702L 的 `phyrf` CMake 列出源码；BL602、BL616、BL616CL、BL618DG 的相关构建路径链接不同预编译库，后几者还按 LPFW、产测、`CPU_MODEL` 等选择变体。以目标 `drivers/soc/<chip>/phyrf/CMakeLists.txt` 和最终链接命令为准；库目录名里的旧 GCC 版本不等于当前构建编译器版本，也不证明库内部已审查。

`drivers/rfparam/CMakeLists.txt` 对 BL602 选择单独 adapter；BL616CL、BL618DG 还引入各自 Flash OTP 实现，BL618DG 另有 eFuse 实现。Wi-Fi 示例在网络启动前调用 `rfparam_init(0, NULL, 0)` 并检查返回值。当前通用实现的三个形式参数未用于选择 TLV 地址，它设置 `rfparam_load` 回调、启用参数加载及全校准，再调用 `wl_init()`；不要仅凭参数名假定传入地址会改变来源。不同芯片的实现和库行为需分别确认。

## TLV、功率偏移与 capcode

通用 `rfparam_load` 默认从链接符号 `_ld_symbol_rftlv_address` 定位 RF TLV；`CONFIG_RFPARAM_TLV_FIXED_ADDRESS` 则按代码所在地址区域选择固定地址，具体规则见 `drivers/rfparam/Src/rfparam_adapter.c`。链接符号、镜像实际内容与板级工厂参数必须一起核对。`rfparam_load` 是 weak 函数，可被工程覆盖。

在通用 adapter 的 WLAN 功率偏移路径，`PWR_MODE` 按字符顺序尝试来源：`B/b` 读制造数据介质，`F/f` 读 RF TLV；大写成功时覆盖并返回，小写成功时累加。BL616CL/BL618DG 的制造介质可能是 Flash OTP 或 eFuse，不能把 `B` 一律解释为 eFuse。缺少 mode 会报错，但所有来源读取失败的某些路径仍返回成功；`rfparam_init` 返回成功不能证明每项校准数据都有效。`XTAL_MODE` 的 `F` 尝试 TLV、`M` 尝试制造介质，来源、默认值和 slot 回退受芯片实现约束。

`drivers/rfparam/Src/rfparam_rftlv.c` 的读取器校验 magic、TLV 头位置及调用方缓冲长度，但源码明确提示 value 可能越过 TLV 区域；不要把该解析器当成对任意不可信 TLV 的完整校验。国家码既可能来自工厂参数，也存在无线栈运行时配置；核对具体节点、芯片和当前配置，不能由默认值推断目标区域或认证状态。

## 产测和持久化

`examples/mfg_test/` 包含按芯片编译的产测功能；BL618DG 的 `RFCAL*` 命令受芯片编译条件限制。其写入流程中，部分 eFuse 写操作先暂存 shadow，确认/保存才执行编程；Flash OTP 保存可能擦除安全区后写入回读，`RFCALERS` 会擦除。暂存读取成功不等于永久写入成功，其他 eFuse 接口也不一定都遵循同一两阶段模型。寄存器写入和射频收发测试会改变设备状态。运行任何产测或写入前核对具体命令、设备、已有数据和目标芯片，避免从本说明直接推导校准值或写入流程。

检查点：目标芯片、核心与 PHY 库变体；链接到的 TLV 和工厂参数；实际介质、校验状态及读取日志；信道、功率和区域设置；必要时结合射频仪表和受控环境验证。单独编译不能证明射频性能、校准有效或法规符合性。
