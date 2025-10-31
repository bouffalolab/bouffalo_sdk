# OpenAI实时语音对话 [English](README.md)

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

另外在***proj.conf*** 中，***CONFIG_MSP_USE_STATIC_RAM*** 宏时控制xcodec层创建task是否使用静态ram，这个功能在用户内存比较紧张的时候可以开启，避免出现动态创建task失败的问题。默认使用618内置codec，**multimedia_port.c**除了PA相关配置，还需要进行如下配置：

| 适配项                                  | 说明                                              |
| --------------------------------------- | ------------------------------------------------- |
| INPUT_NEGATIVE_PIN/INPUT_POSITIVE_PIN   | codec input引脚，如果不使用差分，请将其设置为255  |
| OUTPUT_NEGATIVE_PIN/OUTPUT_POSITIVE_PIN | codec output引脚，如果不使用差分，请将其设置为255 |

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

### 连接wifi功能

```
bouffalolab />wifi_sta_connect <ssid> <psk>
```

### 设置OpenAI的key

```
bouffalolab />psm_set "OPENAI_API_KEY" <"your key">
```

设置完有效的OpenAI key后，系统会自动连接，连接成功后会打印`PeerConnectionState: completed`  `DataChannel created`等log。
