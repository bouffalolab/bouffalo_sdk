# Full-featured Player [中文](README_zh.md)

**Note**: As of the last update of this document, the coexistence of WiFi and Bluetooth on BL616 has not been ported yet. Therefore, playing Bluetooth music while playing WiFi music may lead to unknown exceptions, and vice versa. Please stay tuned for future updates.

## Supported CHIPs

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Software Adaptation

Please refer to the codec demo

## Compilation

- For BL616/BL618

```
make
```

## Flashing
make flash CHIP=bl616 COMX=/dev/ttyACM0

## Running

### 1. Play Local File System Audio

```
bouffalolab />smta play file:///romfs/music.mp3
```

### 2. Play Online Music

##### 2.1 Connect to WiFi

```
bouffalolab />wifi_sta_connect <ssid> <password>
```

##### 2.2 Play HTTP Music

The URL link should be accessible and supports HTTPS.

```
bouffalolab />smta play http://iot-du-home-test.bj.bcebos.com/test/bp_32k_mono_32kbps_03m53s.mp3
```

##### 2.3 Stop Player

```
bouffalolab />smta stop
```

### 3. Play multiple audio

##### 3.1 Play online music

```
bouffalolab />smta mixer play http://iot-du-home-test.bj.bcebos.com/test/bp_32k_mono_32kbps_03m53s.mp3 [vol]
```

##### 3.2 Play local music

```
bouffalolab />smta mixer play file:///romfs/music.mp3 [vol]
```

##### 3.3 Check playing info

```
bouffalolab />smta mixer info
```

##### 3.4 Stop audio

```
bouffalolab />smta mixer stop [id]
```

##### 3.5 Pause audio

```
bouffalolab />smta mixer pause [id]
```

##### 3.6 Resume audio

```
bouffalolab />smta mixer resume [id]
```

##### 3.7 Stop all audio

```
bouffalolab />smta mixer stop_all [id]
```

##### 3.8 Pause all audio

```
bouffalolab />smta mixer pause_all [id]
```

##### 3.9 Resume all audio

```
bouffalolab />smta mixer resume_all [id]
```

### 4. Play Bluetooth Music

##### 4.1 Enable Bluetooth Broadcast

```
bouffalolab />smta a2dp 1
```

##### 4.2 Connect and Play Music from a Mobile Device
Open Bluetooth on your mobile device, search for and connect to a device named something like BouffaloMSP[XX:XX:XX], and you can play music via Bluetooth.

##### 4.3 Turn Off Bluetooth Broadcast

```
bouffalolab />smta a2dp 0
```

### 5. Network Recording

tydb network recording is a tool for transferring audio data over a local network based on WebSocket. The development board sends locally recorded data over the network to a PC in the local network, where you can view the audio data. The tydb tool is located in the examples/audio/player/tools/tydb directory. Ensure that your board is connected to the network and in the same local network as your PC, with mutual pingability.
##### 5.1 Server Side

```
tydb.exe record web 0.0.0.0:8090 ./
```

##### 5.2 Device Side

To initiate audio recording transmission from the device side, `<dest_ip>` represents the IP address of the target PC:

* Single-channel audio recording (Supported on BL616/8 and BL606P platforms):

```bash
bouffalolab />record start ws://<dest_ip>:8090 16k_16bit_1ch.pcm 1
```

* Three-channel audio recording (Currently supported only on BL606P platform):

```bash
bouffalolab />record start ws://<dest_ip>:8090 16k_16bit_3ch.pcm 3
```

* Five-channel audio recording (Currently supported only on BL606P platform):

```bash
bouffalolab />record start ws://<dest_ip>:8090 16k_16bit_5ch.pcm 5
```

* To stop the recording:

```bash
bouffalolab />record stop
```
