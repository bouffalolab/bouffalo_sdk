# Codec [English](README.md)

**特别说明**: 截止到的更新文档为止，已经支持mic、speaker功能，回环待后续完善

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

### 测试mic功能

```
bouffalolab />codec_mic
```

### 测试speaker功能

```
bouffalolab />codec_speaker
```

### 测试loop（回环）功能

```
bouffalolab />codec_loop
```
