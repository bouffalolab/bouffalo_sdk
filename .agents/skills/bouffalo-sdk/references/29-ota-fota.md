# OTA/FOTA：镜像、状态和分区

## 格式与分区

`CONFIG_FOTA` 启用 `components/fota/`；HTTP(S)、TCP、服务端等前端按配置编入。标准 OTA 头 `ota_header_t` 为 512 字节，包含标识、类型、body 长度、软硬件版本和 SHA-256；`compat/bflb_ota` 使用另一种 raw firmware 加尾随 SHA-256 格式，不能互换。打包产物与版本受工程、工具和 `CONFIG_OTA_HEADER_USE_SDK_VERSION` 等配置影响，应检查实际文件头和目标固件的解析规则。

当前标准头解析对精确类型 `RAW` 返回非零，而调用方把非零视为头错误；部分 README 却示例使用 `.bin.ota`。本次 BL616 示例构建生成的 `.bin.ota` 类型字段是带空格的 `RAW `，不是精确的 `RAW`；其头部长度和 SHA-256 与 body 相符。文件后缀不能证明头类型或设备可接收，须检查生成文件和目标版本，并通过实际升级验证。`CONFIG_OTA_VERSION_CHECK` 默认关闭；是否防止降级应核对应用及启动链，不能从版本字段存在推断已启用。

`ota_start()` 选择分区表中的非活动 FW 槽；`CONFIG_FAST_OTA` 路径会先擦除该槽。开始之前核对实际分区表、固件大小与所有重叠区域：例如当前 BL618DG 4 MiB 示例表中 FW 非活动槽与 mfg 条目共用地址范围，具体部署是否使用该区域需查实际生成分区表。`ota_finish(handle, check_hash, reboot)` 在长度一致时可回读并检查 SHA-256，然后更新分区表中的活动槽。非认证来源可同时替换镜像和摘要，摘要匹配只说明内容一致，不证明来源；TLS 对端验证、镜像签名、安全启动和抗回滚需分别检查。

`ota_rollback()` 修改活动槽，当前实现本身不验证目标槽完整性；boot2 的回滚、XZ 解压和试运行确认还依赖实际 boot2、分区及固件。不要将两个槽简单等同于自动可靠回滚，也不要声称解压开始后旧镜像一定完整保留。断电、校验失败及恢复需要在受控目标上单独测试。

## HTTPS FOTA 客户端

接口在 `components/fota/https/client/https_fota.h`：`https_fota_init(url, &config)` 返回 handle，`https_fota_start(handle)` 创建后台任务，返回 0 只表示任务创建成功。响应 body 分段交给 OTA，收齐并匹配 Content-Length 后触发 `HTTPS_FOTA_TRANSFER_FINISH`。手动模式完成后通过 `https_fota_finish(handle, reboot)` 检查并提交；`https_fota(url, &config)` 是异步便利接口，任务会自动完成或中止。失败与进度通过状态回调及日志观察，不能把启动函数返回 0 当作升级成功。

配置结构是浅拷贝：CA、客户端证书、私钥和 user_arg 所指对象应在异步任务使用期保持有效。当前状态回调同步运行在下载任务中；源码上直接在回调内 finish/abort 可能释放任务随后仍使用的 handle，外部并发 abort 也需核对任务协调，尚未做运行复现。应用应设计好明确所有权和完成同步，并针对当前实现验证，不要假设回调里释放句柄安全。启用 reboot 时底层可能在成功回调前复位，应以重启后的版本与启动状态验证最终结果。

HTTPS 需要有效 CA 或正确启用并附加证书 bundle；无可用 CA/bundle 时 HTTP 适配可能关闭证书校验，URL 也支持普通 HTTP。TLS 信任链和 hostname、固件签名分别见 [11-security.md](11-security.md) 与 [25-app-protocols-cloud.md](25-app-protocols-cloud.md)。`examples/wifi/sta/wifi_ota_by_http` 展示客户端、服务器和不同重启参数，但测试服务和固件升级会改变网络/设备状态，不应当作只读检查。

## 验证顺序

先离线核对目标芯片、分区表、镜像头/长度/hash、打包版本和证书；再在有恢复方案的目标上分别验证下载、失败清理、hash 错误、断电、分区切换、启动与回滚。记录升级前后实际镜像和运行日志；构建与离线文件检查不代表设备升级成功。
