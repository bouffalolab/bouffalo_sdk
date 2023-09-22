# player 全功能 [English](README.md)

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

### 1. 播放本地文件系统音频

```
bouffalolab />smta play file:///romfs/music.mp3
```

### 2. 播放在线音乐

##### 2.1 连接wifi

```
bouffalolab />wifi_sta_connect <ssid> <password>
```

##### 2.2 播放http音乐

url链接需要可访问, 支持https

```
bouffalolab />smta play http://iot-du-home-test.bj.bcebos.com/test/bp_32k_mono_32kbps_03m53s.mp3
```

##### 2.3 停止播放

```
bouffalolab />smta stop
```
### 3. 播放蓝牙音乐

##### 3.1 开启蓝牙广播

```
bouffalolab />smta a2dp 1
```

##### 3.2 手机连接并播放音乐
手机打开蓝牙，查找并连接类似BouffaloMSP[XX:XX:XX]这样命名的设备，即可蓝牙播放音乐

##### 3.3 关闭蓝牙广播

```
bouffalolab />smta a2dp 0
```

### 4. 网络录音

tydb网络录音是基于websocket的局域网环境下传输工具, 开发板将本地录音的数据通过网络发送到局域网的PC端,可在PC端打开查看音频数据。工具tydb在examples/audio/player/tools/tydb 目录下，需要保证我们的板子已经连接到网络，并且需要保证与PC在统一局域网，可以相互Ping通

##### 4.1 服务端

```
tydb.exe record web 0.0.0.0:8090 ./
```

##### 4.2 设备端

设备端开启录音传输, `<dest_ip>` 表示目标PC的ip地址:

* 一路录音(支持BL616/8和BL606P平台)

```
bouffalolab />record start ws://<dest_ip>:8090 16k_16bit_1ch.pcm 1
```

* 三路录音(暂时只支持BL606P平台)

```
bouffalolab />record start ws://<dest_ip>:8090 16k_16bit_3ch.pcm 3
```

* 五路录音(暂时只支持BL606P平台)

```
bouffalolab />record start ws://<dest_ip>:8090 16k_16bit_5ch.pcm 5
```

* 停止录音

```
record stop
```
