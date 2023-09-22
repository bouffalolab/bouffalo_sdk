# Player A2DP [中文](README_zh.md)

**Note**: Up to the latest update of the document, the coexistence of wifi/bt on BL616 has not been ported yet. Therefore, playing Bluetooth audio while streaming music via wifi, or vice versa, may cause unknown issues. Please stay tuned for future updates.

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

## Run

### Enable Bluetooth Advertising on the Device

```
bouffalolab />smta a2dp 1
```

### Connect and Play Music from a Mobile Device
Turn on Bluetooth on your mobile phone.
Search for and connect to a device named similar to "BouffaloMSP[XX:XX:XX]" to enable Bluetooth audio streaming.

### Disable Bluetooth Advertising on the Device

```
bouffalolab />smta a2dp 0
```
