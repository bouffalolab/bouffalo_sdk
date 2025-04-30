# Codec [中文](README_zh.md)

**Special Note**: As of the latest update to this documentation, support for mic and speaker functionality is available. Loopback is pending further improvement.

## Chip Support List

|      Chip        | Remark |
|:----------------:|:------:|
| BL616/BL618      |        |

## Software Adaptation

This demo is tested using the BL618 development board, so users need to modify some hardware-related configurations during development. The main modification points are in ***multimedia_port*.c**:

| Adaptation Item     | Description                                                  |
| ------------------- | ------------------------------------------------------------ |
| g_pa_delay_2        | PA start time, needs to be configured according to the PA chip manual |
| CONFIG_AUDIO_PA_PIN | PA control pin                                               |

In addition, in ***proj.conf***, the ***CONFIG_MSP_USE_STATIC_RAM*** macro controls whether the xcodec layer uses static RAM to create tasks. This function can be enabled when the user's memory is tight to avoid the problem of dynamic task creation failure. At the same time, if the hardware uses an external ES8388 for recording/playback, you need to set ***CONFIG_CODEC_USE_ES8388*** to 1, and ignore ***CONFIG_CODEC_USE_I2S_TX*** and ***CONFIG_CODEC_USE_I2S_RX***. The function of using the 618 built-in codec + external codec combination will be added later.

If ***CONFIG_CODEC_USE_ES8388*** is set to 0, that is, the 618 built-in codec is used, **multimedia_port** needs to be configured as follows in addition to PA-related configurations:

| Adaptation Item                         | Description                                                |
| --------------------------------------- | ---------------------------------------------------------- |
| INPUT_NEGATIVE_PIN/INPUT_POSITIVE_PIN   | codec input pin, if not using differential, set it to 255  |
| OUTPUT_NEGATIVE_PIN/OUTPUT_POSITIVE_PIN | codec output pin, if not using differential, set it to 255 |

If ***CONFIG_CODEC_USE_ES8388*** is set to 1, that is, 618+ES8388 codec is used, **multimedia_port.c** needs to be configured as follows in addition to PA related configurations:

| Adaptation Item        | Description                        |
| ---------------------- | ---------------------------------- |
| i2s_gpio_init function | Configure i2s and i2c related pins |

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