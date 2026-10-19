# wifi6


## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL616/BL618 |        |

## Compile

- BL616/BL618

```
 make CHIP=bl616 BOARD=bl616dk_solution
```

- BL616CL

```
 make CHIP=bl616cl BOARD=bl616cldk_solution
```

- BL618DG

```
 make CHIP=bl618dg BOARD=bl618dgdk_solution CPU_ID=ap
```

## Flash

```
make flash COMX=xxx ## xxx is your com name
```
