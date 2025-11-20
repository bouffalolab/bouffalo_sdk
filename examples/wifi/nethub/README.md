# Nethub

## Support CHIP

|      CHIP               | Remark |
|:-----------------------:|:------:|
|bl616/bl618/bl616d       |        |

## Compile

- bl616/bl618/bl616d

```bash
make CHIP=<chipname> BOARD=<boardname>
```

eg:
```bash
make CHIP=bl616 BOARD=bl616dk
```

- bl616d

```bash
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap CONFIG_ROMAPI=n
```

## Flash

- bl616/bl618/bl616d

```bash
make flash CHIP=<chipname> COMX=xxx ## xxx is your com name, chipname is bl616...
```

