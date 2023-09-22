# Codec [中文](README_zh.md)

**Special Note**: As of the latest update to this documentation, support for mic and speaker functionality is available. Loopback is pending further improvement.

## Chip Support List

|      Chip        | Remark |
|:----------------:|:------:|
| BL616/BL618      |        |
| BL606p/BL808     |        |

## Compilation

- BL616/BL618

```
make
```

## Flashing

```
make flash CHIP=bl616 COMX=/dev/ttyACM0
```

## Running

### Test Mic Function

```
bouffalolab />codec_mic
```

### Test Speaker Function

```
bouffalolab />codec_speaker
```

### Test Loop (Loopback) Function

```
bouffalolab />codec_loop
```