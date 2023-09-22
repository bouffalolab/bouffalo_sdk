# player url [English](README.md)

**特别说明**: 该例程提供了最精简的仅支持mp3本地音频播放,需要新增流媒体其他配置可以参考multimedia_user.h，通过修改multimedia_user.h可以完成对流媒体功能的裁剪

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

需要烧录本地的文件系统,默认为ROMFS

```
bouffalolab />smta play file:///romfs/music.mp3
```

### 2. 播放数组音频

addr为 **10** 进制,需要对应音频数组RAM地址或者XIP地址

```
bouffalolab />smta play mem://addr=2687922304&size=10152&avformat=mp3&acodec=mp3
```
