# OpenAI real-time voice conversation [中文](README_zh.md)

## Chip Support List

|      Chip        | Remark |
|:----------------:|:------:|
| BL616/BL618      |        |

## Software Adaptation

This demo is tested using the BL618 development board. Therefore, users need to modify some hardware-related configurations during development. The main modification points are in the `multimedia_port.c` file:

| Adaptation Item     | Description                                                  |
| ------------------- | ------------------------------------------------------------ |
| g_pa_delay_2        | PA start time, needs to be configured according to the PA chip manual |
| CONFIG_AUDIO_PA_PIN | PA control pin                                               |

In addition, in the `proj.conf` file, the `CONFIG_MSP_USE_STATIC_RAM` macro controls whether the xcodec layer uses static RAM when creating tasks. This function can be enabled when the user's memory is relatively tight to avoid the problem of failed dynamic task creation. By default, the built-in codec of the BL618 is used. Besides the PA-related configurations in `multimedia_port.c`, the following configurations are also required:

| Adaptation Item                         | Description                                                |
| --------------------------------------- | ---------------------------------------------------------- |
| INPUT_NEGATIVE_PIN/INPUT_POSITIVE_PIN   | codec input pin, if not using differential, set it to 255  |
| OUTPUT_NEGATIVE_PIN/OUTPUT_POSITIVE_PIN | codec output pin, if not using differential, set it to 255 |

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

#### Connecting to Wi-Fi

```
bouffalolab />wifi_sta_connect <ssid> <psk>
```

#### Setting the OpenAI Key

```
bouffalolab />psm_set "OPENAI_API_KEY" <"your key">
```

After setting a valid OpenAI key, the system will automatically connect. After a successful connection, logs such as `PeerConnectionState: completed` and `DataChannel created` will be printed.