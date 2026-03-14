# X.509 Certificate Bundle for Bouffalo SDK

本组件提供 X.509 根 CA 证书束功能，用于 TLS/SSL 连接时验证服务器证书。

## 一、功能简介

bl_crt_bundle 是 Bouffalo SDK 的证书束组件，提供以下核心功能：

- **空间优化**：只保留证书的 subject name 和 public key，大幅减少 Flash 占用（相比完整 PEM 格式节省约 70% 空间）
- **快速查找**：使用二分查找算法，快速定位验证证书
- **自动集成**：通过 menuconfig 配置，构建时自动打包到固件
- **灵活配置**：支持自定义证书集合，可选择性包含特定 CA 证书

**核心特性**：
- 无需在代码中硬编码 CA 证书
- 支持 200+ 根 CA 证书（Mozilla 根证书列表）
- 与 mbedTLS 无缝集成

**快速使用**：
```c
#include "bl_crt_bundle.h"

// 附加证书束到 SSL 配置
bl_crt_bundle_attach(&ssl_conf);

// TLS 握手会自动使用证书束验证服务器
mbedtls_ssl_handshake(&ssl);

// 清理（可选）
bl_crt_bundle_detach(&ssl_conf);
```

## 二、Cert Bundle 与 CA Root 的关系

### 2.1 证书验证原理

TLS 连接时，设备需要验证服务器证书的有效性。验证过程依赖信任链：

```
TLS 握手证书验证流程：

设备连接服务器
    ↓
服务器返回证书链
    ↓
┌───────────────────────────────────────┐
│  bl_crt_bundle 证书束                 │
│  [Root CA 1] [Root CA 2] ... [Root CA N] │
│         ↓                              │
│  验证服务器证书是否由某个 Root CA 签发   │
└───────────────────────────────────────┘
    ↓
  验证成功 → 建立安全连接
  验证失败 → 连接终止
```

### 2.2 三者的关系

```
┌─────────────────────────────────────────────────────────────┐
│                    bl_crt_bundle (证书束)                    │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  存储在 Flash 中的二进制证书集合                      │  │
│  │  - 公共 CA Root: Mozilla 根证书列表（100+ 个）        │  │
│  │  - 私有 CA Root: 企业自签根证书（可选）               │  │
│  │  - 空间优化：只保存 subject + public key              │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                           │
                           │ 1. 固件编译时打包
                           │
        ┌──────────────────┴──────────────────┐
        │                                     │
        ↓                                     ↓
┌──────────────────┐              ┌──────────────────┐
│  公共 CA Root    │              │  私有 CA Root    │
│  (Mozilla CA)    │              │  (企业自建)      │
├──────────────────┤              ├──────────────────┤
│ • DigiCert       │              │ • IoT 设备 CA    │
│ • Let's Encrypt  │              │ • 内网服务 CA    │
│ • GlobalSign     │              │ • 企业内部 CA    │
│ • ...            │              │ • ...            │
│                  │              │                  │
│ 信任源：Mozilla  │              │ 信任源：企业自己  │
│ 适用场景：公网   │              │ 适用场景：内网   │
│  HTTPS 服务      │              │ IoT、私有服务    │
└──────────────────┘              └──────────────────┘
```

### 2.3 使用场景对比

| 场景 | 证书来源 | 证书束配置 | 示例 |
|------|---------|-----------|------|
| **访问公网 HTTPS** | 公共 CA Root | 默认 Mozilla 根 CA 列表 | 连接 `https://api.github.com` |
| **访问企业内网** | 私有 CA Root | 添加企业 Root CA 到 bundle | 连接 `https://internal.company.com` |
| **IoT 设备认证** | 私有 CA Root | 只包含设备签发 CA | 设备与云平台双向认证 |
| **混合场景** | 公共 + 私有 | 合并两种 CA 证书 | 同时支持公网和内网服务 |

### 2.4 实际应用示例

**场景 1：默认配置（只有公共 CA）**
```bash
# cacrt_all.pem = Mozilla 根 CA 列表
python3 gen_crt_bundle.py -i cacrt_all.pem

# 验证公网 HTTPS 服务器
# ✅ https://www.baidu.com
# ✅ https://api.github.com
# ❌ https://internal.company.com (内网私有 CA)
```

**场景 2：添加私有 CA**
```bash
# 合并证书
cat mozilla_root.pem company_root.pem > cacrt_all.pem
python3 gen_crt_bundle.py -i cacrt_all.pem

# 验证效果
# ✅ https://www.baidu.com
# ✅ https://api.github.com
# ✅ https://internal.company.com
```

**场景 3：只用私有 CA（IoT 设备）**
```bash
# 只包含设备签发 CA
python3 gen_crt_bundle.py -i iot_device_ca.pem

# 验证效果
# ❌ https://www.baidu.com
# ✅ https://iot-device.company.com
```

## 三、从 Mozilla 下载 CA Root

### 3.1 Mozilla CA Certificate Program

**Mozilla CA Certificate Program** 是 Mozilla 维护的根证书信任列表，被以下项目广泛采用：

- Mozilla Firefox
- Google Chrome (Linux)
- Android
- curl
- **Bouffalo SDK (本组件)**

该列表包含 100+ 根 CA 证书，覆盖全球主流证书颁发机构。

### 3.2 下载方法

#### 方法 1：直接下载（推荐）

```bash
# 进入证书束目录
cd components/crypto/mbedtls/bl_crt_bundle/

# 下载最新的 Mozilla 根 CA 证书束
wget https://curl.se/ca/cacert.pem -O cacert_mozilla.pem

# 或使用 curl
curl -o cacert_mozilla.pem https://curl.se/ca/cacert.pem
```

#### 方法 2：从 Mozilla 源码获取

```bash
# 克隆 Mozilla NSS 源码
git clone https://github.com/nss-dev/nss.git
cd nss

# 复制根证书束
cp lib/ckfw/builtins/certdata.txt ../certdata.txt

# 使用 Python 脚本转换为 PEM 格式
# (需要转换脚本，参考方法 3)
```

#### 方法 3：使用 certdata.txt 转换

```bash
# 下载 Mozilla certdata.txt
wget https://hg.mozilla.org/mozilla-central/raw-file/default/security/nss/lib/ckfw/builtins/certdata.txt

# 使用 mk-ca-bundle.pl 转换
wget https://raw.githubusercontent.com/curl/curl/master/scripts/mk-ca-bundle.pl
perl mk-ca-bundle.pl -u cacert_mozilla.pem
```

### 3.3 证书格式说明

**PEM 格式（推荐）**：
```
-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs
...
( Base64 编码的证书内容)
...
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIDiTCCAnegAwIBAgIQBP3j/zCv6lJA7E5zR9SuYjANBgkqhkiG9w0BAQUFADBX
...
-----END CERTIFICATE-----
```

**DER 格式（二进制）**：
- 需要转换为 PEM 格式才能使用：
```bash
openssl x509 -inform DER -in cert.der -outform PEM -out cert.pem
```

### 3.4 验证下载的证书

```bash
# 查看证书数量
grep -c "BEGIN CERTIFICATE" cacert_mozilla.pem

# 查看证书列表
openssl crl2pkcs7 -nocrl -certfile cacert_mozilla.pem | \
  openssl pkcs7 -print_certs -text -noout | \
  grep "Subject:" | head -20

# 验证 PEM 格式
openssl x509 -in cacert_mozilla.pem -noout -text
```

### 3.5 在 Bouffalo SDK 中使用

**步骤 1：替换默认证书文件**
```bash
cd components/crypto/mbedtls/bl_crt_bundle/

# 备份原文件
cp cacrt_all.pem cacrt_all.pem.bak

# 使用下载的 Mozilla 证书
cp cacert_mozilla.pem cacrt_all.pem
```

**步骤 2：生成证书束**
```bash
python3 gen_crt_bundle.py -i cacrt_all.pem
```

输出示例：
```
Parsing 154 certificates from cacert_all.pem
Writing bundle: x509_crt_bundle
Total size: 147712 bytes
```

**步骤 3：编译并烧录**
```bash
cd examples/wifi/https
make menuconfig CHIP=bl616 BOARD=bl616dk
# 确保 "X.509 Certificate Bundle" 已启用

make CHIP=bl616 BOARD=bl616dk
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

### 3.6 自定义证书过滤

如果不需要所有 Mozilla 根 CA，可以过滤证书：

**创建过滤器文件** (`mozilla_filter.csv`)：
```csv
序号,证书名称
1,DST Root CA X3
2,Entrust Root Certification Authority
3,GlobalSign Root CA
4,DigiCert Assured ID Root CA
5,Let's Encrypt Authority X3
```

**生成过滤后的证书束**：
```bash
python3 gen_crt_bundle.py \
  -i cacert_all.pem \
  -f mozilla_filter.csv \
  -o x509_crt_bundle
```

**验证结果**：
```bash
# 查看包含的证书
python3 gen_crt_bundle.py -i cacrt_all.pem -f mozilla_filter.csv --dump

# 输出：
# Certificate 1: DST Root CA X3
# Certificate 2: Entrust Root Certification Authority
# ...
```

### 3.7 更新证书束

**定期更新（推荐每 6 个月）**：
```bash
# 创建更新脚本
cat > update_certs.sh << 'EOF'
#!/bin/bash
cd components/crypto/mbedtls/bl_crt_bundle/

# 备份当前证书
[ -f cacrt_all.pem ] && cp cacrt_all.pem cacrt_all.pem.old

# 下载最新证书
wget -q https://curl.se/ca/cacert.pem -O cacert_all.pem.new

# 验证新文件
if [ -s cacert_all.pem.new ]; then
    mv cacert_all.pem.new cacrt_all.pem
    echo "Certificates updated successfully"

    # 重新生成 bundle
    python3 gen_crt_bundle.py -i cacrt_all.pem
    echo "Certificate bundle regenerated"
else
    echo "Failed to download certificates"
    rm -f cacert_all.pem.new
    exit 1
fi
EOF

chmod +x update_certs.sh
./update_certs.sh
```

### 3.8 证书版本管理

**记录证书版本**：
```bash
# 在证书目录添加版本信息
cat > cacrt_all.pem.version << EOF
Source: Mozilla Root CA Program
URL: https://curl.se/ca/cacert.pem
Download Date: $(date -u +"%Y-%m-%d %H:%M:%S UTC")
Certificate Count: $(grep -c "BEGIN CERTIFICATE" cacert_all.pem)
SHA256: $(sha256sum cacert_all.pem | awk '{print $1}')
EOF
```

**对比证书变化**：
```bash
# 检查证书变化
diff <(grep "BEGIN CERTIFICATE" cacrt_all.pem.old | wc -l) \
     <(grep "BEGIN CERTIFICATE" cacrt_all.pem | wc -l)
```
