# usbhost

If you use bl808, please modify configs (like CONFIG_USBHOST_MAX_INTF_ALTSETTINGS) in usb_config.h because bl808 does not have too much memory.

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |
|BL808             |   D0 has no irq     |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL808

```
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```