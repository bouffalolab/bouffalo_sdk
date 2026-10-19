# Notes

## 免密 sudo

运行 `test_manual` 的 Jenkins 构建机和硬件测试节点需要在 `/etc/sudoers` 中为执行用户配置免密 sudo。构建脚本会以非交互方式调用 `sudo -E unshare`、mount overlay 和清理临时目录，部分测试还会调用 `sudo -n`；缺少 `NOPASSWD` 配置会导致任务等待密码或直接失败。

使用 `visudo` 或 `/etc/sudoers.d/` 配置并先验证：

```sudoers
<user> ALL=(ALL) NOPASSWD: ALL
```

```bash
sudo -n true
```

不要由 agent 自动修改 `/etc/sudoers`。该文件属于机器级安全配置，必须由机器管理员审核和安装；生产环境应按实际命令收窄授权范围。

## 在 submodule 中 bisect

Bouffalo SDK 包含多个 submodule。对某个组件做 `git bisect` 时，必须先指定并进入实际发生变更的 submodule 仓库，例如：

```bash
git -C components/wireless/macsw bisect start <bad> <good>
```

不要默认在 SDK 顶层仓库执行 bisect；顶层通常只记录 submodule 指针，不能逐个定位 submodule 内部提交。开始前确认以下信息：

- 要 bisect 的具体 submodule 路径。
- 该 submodule 内可解析的 good/bad commit。
- 一棵完整且依赖一致的 SDK 工作树，作为 `test_manual --sdk-path` 的构建输入。

每次由 `git bisect` 切换 submodule 提交后，从 SDK 根目录运行测试，并显式保留待测工作树：

```bash
./tools/CI/test_manual wifi_http_616 --sdk-path "$PWD"
```

`test_manual` 不会 checkout、初始化或更新 submodule，也不会执行 `git bisect good/bad`。先根据退出码和日志区分测试失败（退出码 `1`）与环境或工具错误（退出码 `2`）；只有可重复的测试结论才能标记 good/bad，基础设施错误应修复后重试。
