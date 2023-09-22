# Player A2DP [中文](README_zh.md)

**Note**: This example provides the most streamlined support for local MP3 audio playback. If you need to add other configurations for streaming media, you can refer to multimedia_user.h. You can complete the trimming of streaming media functions by modifying multimedia_user.h.

## Supported CHIPs

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |
|BL606p/BL808      |        |

## Compilation

- For BL616/BL618

```
make
```

## Flashing
make flash CHIP=bl616 COMX=/dev/ttyACM0

## Running

### 1. Play Local File System Audio

You need to flash the local file system, which is the default ROMFS.

```
bouffalolab />smta play file:///romfs/music.mp3
```

### 2. Play Array Audio

The 'addr' should be in decimal and correspond to the RAM address or XIP address of the audio array.

```
bouffalolab />smta play mem://addr=2687922304&size=10152&avformat=mp3&acodec=mp3
```

