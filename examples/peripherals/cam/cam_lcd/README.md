# cam_lcd

## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL616/BL618 |        |

## Compile

- BL616DK

```
make CHIP=bl616 BOARD=bl616dk
```

This uses the default GPIO configuration in `bsp/board/bl616dk/board_gpio.c`.

- BL616 IFA board

```
make CHIP=bl616 BOARD=bl616_ifa_board
```

This uses the GPIO overlay in `bl616_ifa_board/board_gpio_overlay.c`.

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np
```

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```
## Flash

```
make flash CHIP=bl616 COMX=xxx # xxx is your com name
```
