# cam_lcd_mipi

## Support CHIP

| CHIP | Remark |
|:----:|:------:|
| BL618DG | MIPI DSI display with DVP camera |

## Compile

- BL618DGDK

```sh
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

This uses the default GPIO configuration in `bsp/board/bl618dgdk/board_gpio.c`.

- BL618DG IFA board

```sh
make CHIP=bl618dg BOARD=bl618dg_ifa_board CPU_ID=ap
```

This uses the GPIO overlay in `bl618dg_ifa_board/board_gpio_overlay.c`.

## Flash

```sh
make flash CHIP=bl618dg COMX=xxx
```
