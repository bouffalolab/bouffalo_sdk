---
name: test-manual
description: 使用 tools/CI/test_manual 查询、编译并运行 Bouffalo SDK 手工 Jenkins 测试；适用于单个 case、suite，以及配合 git bisect 验证指定 SDK 或 submodule 提交。
---

# test_manual 测试工具

使用仓库根目录下的 `./tools/CI/test_manual` 编译测试依赖的固件、触发 Jenkins `manual_test`，等待结果并下载日志。

## 运行前

先阅读 [notes.md](references/notes.md)。其中包含构建机免密 sudo 要求，以及 bisect 时必须指定 submodule 仓库的约束。

确认当前目录是 SDK 根目录，并用 `show` 查询准确的 case 或 suite 名称：

```bash
./tools/CI/test_manual show
```

## 运行测试

运行单个 BL616 WiFi HTTP case：

```bash
./tools/CI/test_manual wifi_http_616
```

工具也接受多个 case、suite 或 `suite/test` 选择器。只需要验证编译时使用 `--build-only`；需要保留产物到固定目录时使用 `--artifact-dir`。

```bash
./tools/CI/test_manual wifi_http_616 --build-only
./tools/CI/test_manual base_616 --artifact-dir /tmp/test_manual-base-616
```

默认待测 SDK 是工具所在目录的 `../..`。测试另一棵完整 SDK 工作树时，显式指定：

```bash
./tools/CI/test_manual wifi_http_616 --sdk-path /path/to/bouffalo_sdk
```

不要让工具负责 checkout、初始化 submodule 或执行 bisect；它只构建 `--sdk-path` 当前指向的源码状态。

## 结果判断

- 退出码 `0`：所有测试通过。
- 退出码 `1`：至少一个测试失败。
- 退出码 `2`：参数、编译、Jenkins 或工具执行错误。
- 未指定 `--artifact-dir` 时，产物和 Jenkins 日志保存在输出所示的 `/tmp/test_manual-*` 目录。

执行真实 Jenkins 硬件测试会触发外部任务；除非用户已明确要求运行，否则先说明将使用的 case、SDK 路径和 agent，再取得授权。
