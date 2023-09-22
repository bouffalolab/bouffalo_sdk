# Player Wifi Case[English](README.md)

**特别说明**: 播放音频流，至于流媒体的配置可以参考multimedia_user.h，通过修改multimedia_user.h可以完成对流媒体功能的裁剪

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

### 在线播放流媒体

```
bouffalolab />wifi_sta_connect <ssid> <password>
bouffalolab />bouffalolab />smta play http://iot-du-home-test.bj.bcebos.com/test/bp_32k_mono_32kbps_03m53s.mp3
```
