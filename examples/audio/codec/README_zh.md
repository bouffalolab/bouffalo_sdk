# Codec [English](README.md)

**特别说明**: 截止到的更新文档为止，已经支持mic、speaker功能，回环待后续完善

## 芯片支持列表

|      芯片        | 备注    |
|:----------------:|:------:|
|BL616/BL618       |        |

## 软件适配

该demo是使用BL618开发板进行测试的，因此用户在开发时需要修改和硬件相关的一些配置，主要修改点在***multimedia_port*.c**中：

| 适配项              | 说明                                   |
| ------------------- | -------------------------------------- |
| g_pa_delay_2        | PA的开启时间，需要根据PA的芯片手册配置 |
| CONFIG_AUDIO_PA_PIN | PA控制引脚                             |

另外在***proj.conf*** 中，***CONFIG_MSP_USE_STATIC_RAM*** 宏时控制xcodec层创建task是否使用静态ram，这个功能在用户内存比较紧张的时候可以开启，避免出现动态创建task失败的问题。同时，如果硬件上使用外置的ES8388来录音/播放时，需要将***CONFIG_CODEC_USE_ES8388***设置为1，***CONFIG_CODEC_USE_I2S_TX***和***CONFIG_CODEC_USE_I2S_RX***请忽略，后续会增加使用618内置codec+外置codec组合的功能。

如果***CONFIG_CODEC_USE_ES8388***设置为0，即使用618内置codec，**multimedia_port.c**除了PA相关配置，还需要进行如下配置：

| 适配项                                  | 说明                                              |
| --------------------------------------- | ------------------------------------------------- |
| INPUT_NEGATIVE_PIN/INPUT_POSITIVE_PIN   | codec input引脚，如果不使用差分，请将其设置为255  |
| OUTPUT_NEGATIVE_PIN/OUTPUT_POSITIVE_PIN | codec output引脚，如果不使用差分，请将其设置为255 |

如果***CONFIG_CODEC_USE_ES8388***设置为1，即使用618+ES8388 codec，**multimedia_port.c**除了PA相关配置，还需要进行如下配置：

| 适配项            | 说明                 |
| ----------------- | -------------------- |
| i2s_gpio_init函数 | 配置i2s和i2c相关引脚 |

## *编译*

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
