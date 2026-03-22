# sdio3_test

This is a speed test based on SDIO3.
The device will send back all the data it has received.

A device running a Linux system that supports SDH Host is required as the host.
The source code of the kernel module driver is located in bsp/common/msg_router/linux_host/kernel, support hot swapping, and can be used for testing the stability of the SDIO driver.

## Support CHIP

| CHIP    | Remark |
|:-------:|:------:|
| BL618DG |        |

## Compile

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
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
