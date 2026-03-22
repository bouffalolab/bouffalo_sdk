# sdio2_test

This is a multi-channel data testing based on SDIO2/SDIO3. 
Currently supported functions: Ethernet devices, tty devices, speedtest testing.
The driver implementation of the device is located in bsp/common/msg_router/device and is called in main.c.

A device running a Linux system that supports SDH Host is required as the host.
The source code of the kernel module driver is located in bsp/common/msg_router/linux_host/kernel, support hot swapping, and can be used for testing the stability of the SDIO driver.

## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL602/BL604 |        |
| BL616/BL618 |        |
| BL616CL     |        |
| BL616DG     |        |

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

- BL616DG

```
make CHIP=bl616dg BOARD=bl616dgdk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## Linux host 
```
cd bsp/common/msg_router/linux_host/kernel

make CONFIG_MR_NETDEV=y CONFIG_MR_NETDEV_ETH=y \
    CONFIG_MR_TTY=y CONFIG_MR_TTY_CMD=y CONFIG_MR_TTY_USER_1=y CONFIG_MR_TTY_USER_2=y CONFIG_MR_TTY_USER_3=y \
    CONFIG_MR_SPEED_TEST=y CONFIG_MR_NETLINK=y

sudo insmod mr_sdio.ko
```
