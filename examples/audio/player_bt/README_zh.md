# player a2dp [English](README.md)

**特别说明**: 截止到的更新文档为止，BL616的wifi/bt共存暂时没有移植，因此如果wifi播放音乐的过程中播放蓝牙音乐，均可能会引发未知异常，反之如此。敬请期待后续的更新。

## 芯片支持列表

|      芯片        | 备注    |
|:----------------:|:------:|
|BL616/BL618       |        |
|BL606p/BL808      |        |

## 编译

- BL616/BL618

```
make
```

## 烧录

```
make flash CHIP=bl616 COMX=/dev/ttyACM0
```

## 运行

### 启动蓝牙广播

```
bouffalolab />smta a2dp 1
```

### 手机连接并播放音乐
手机打开蓝牙，查找并连接类似BouffaloMSP[XX:XX:XX]这样命名的设备，即可蓝牙播放音乐

### 关闭蓝牙广播

```
bouffalolab />smta a2dp 0
```
