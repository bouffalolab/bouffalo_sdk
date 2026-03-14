# NetHub Host 侧架构 Review

本文记录 `bsp/common/msg_router/linux_host/userspace/nethub` 当前 host 侧实现的 review 结论。

## 1. 总结

当前 host 侧主架构已经基本合理，核心方向是对的：

- userspace 已统一构建
- `tty` / `vchan` 已收敛为 runtime backend 选择
- daemon、CLI、WiFi 控制库、virtual channel 已形成比较清晰的分层
- 公共命令面已固定为 9 条

当前问题已经不再是“大方向错误”，而是少量边界还不够干净。

## 2. 主要结论

### 2.1 合理部分

- `build.sh`
  - 现在只负责构建与装载流程，不再制造 userspace 多套产物
- `bflbwifid`
  - 已回到“控制面统一入口”的定位
- `bflbwifictrl`
  - 已收敛为纯 CLI + 本地 IPC 客户端
- `libbflbwifi`
  - 已承担真实的 WiFi 语义控制职责
- `virtualchan/nethub_vchan`
  - 已成为 `vchan` backend 与 USER 数据通道的共用基础能力

### 2.2 仍存在的架构问题

1. `include/bflbwifi.h` 还保留了 `bflbwifi_init(const char *tty_dev, int baudrate)` 这种 TTY 语义入口。
   - 这不是功能 bug，但会持续向外部使用者传递“库本质上是串口库”的错误心智。

2. daemon 与库边界虽然已经明显改善，但还没有完全变成纯 public API。
   - `bflbwifid_request.c`、`bflbwifid_service.c` 已不再直接包含 `src/bflbwifi_internal.h`。
   - 当前只剩 `bflbwifid_ota.*` 单点承接 OTA 看门狗所需的少量非 public helper，边界已经明显收口。

3. `channel.h` 的内部抽象仍偏“全功能统一接口”。
   - 现在虽然已经有 capability 字段，但 `pause_rx`、`get_fd`、`link_event_cb` 这类能力仍然被统一挂在同一套 vtable 上。
   - 后续若继续扩展 backend，维护成本会上升。

4. 历史文档与旧示例残留会误导使用者。
   - 本轮已把架构文档拆分，并删除了过时的 `examples/test_bflbwifi.c` 与 Makefile 中对应目标。

## 3. 无用或低价值文件判断

### 3.1 已处理

- `bflbwifictrl/examples/test_bflbwifi.c`
  - 与当前统一 daemon / CLI 使用方式不一致
  - 示例自身也带有过时假设，继续保留没有价值

### 3.2 当前保留但应明确定位

- `HOST_ARCH_REVIEW_CN.md`
  - 它是 review 记录，不应继续兼做用户手册
- `virtualchan/nethub_vchan_app.c`
  - 不是主路径组件，是 USER 数据通道的调试 / 验证工具

## 4. 推荐的后续优化顺序

1. 逐步弱化 `bflbwifi_init()` 的 TTY 语义，把 `bflbwifi_init_ex()` 作为主入口。
2. 继续把 `channel` 层收敛成“基础能力 + 可选能力”模型。
3. 评估 `bflbwifid_ota.*` 里的 OTA 看门狗能力是否继续下沉到库，或稳定成更清晰的 daemon-facing 接口。
4. 等外部命名策略确定后，再统一处理 `bflb` 前缀问题。

## 5. 当前判断

如果目标是继续稳定迭代，这套 host 侧架构已经可以承载后续功能完善。

如果目标是做长期可复用的跨平台交付，则下一步最值得处理的是：

- 库边界再收紧
- 公共 API 去 TTY 偏置
- backend 能力模型继续收敛
