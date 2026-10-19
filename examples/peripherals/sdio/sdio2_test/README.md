# sdio2_test

This is a speed test based on SDIO2.
The device will send back all the data it has received.

A device running a Linux system that supports SDH Host is required as the host.
The source code of the kernel module driver is located in bsp/common/msg_router/linux_host/kernel, support hot swapping, and can be used for testing the stability of the SDIO driver.

## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL602/BL604 |        |
| BL616/BL618 |        |
| BL616CL     |        |

## Compile

- BL602/BL604

```
make CHIP=bl602 BOARD=bl602dk
```

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## Linux host 
```
cd bsp/common/msg_router/linux_host/kernel

make CONFIG_MR_SPEED_TEST=y

sudo insmod mr_sdio.ko
```
