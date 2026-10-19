# adc_key_basic


## Support CHIP

| CHIP              | Driver |
|:-----------------:|:------:|
| BL602/BL604       |        |
| BL702/BL704/BL706 |        |
| BL702L/BL704L     |        |
| BL616             | ADC    |
| BL616CL           | ADC_V2 |
| BL618DG           | ADC_V3 |

## Compile

- BL602/BL604

```
make CHIP=bl602 BOARD=bl602dk
```

- BL702/BL704/BL706

```
make CHIP=bl702 BOARD=bl702dk
```

- BL702L/BL704L

```
make CHIP=bl702l BOARD=bl702ldk
```

- BL616

```
make CHIP=bl616 BOARD=bl616dk
```

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

## Notes

- `adckey` keeps one common source file and selects the ADC driver internally by chip: `BL616CL -> ADC_V2`, `BL618DG -> ADC_V3`, others default to `ADC`.
- One `adckey` instance owns one ADC peripheral and can manage multiple items. Each item can be `ADCKEY_ITEM_TYPE_KEY` or `ADCKEY_ITEM_TYPE_ADC`.
- When multiple items are configured on external ADC channels, `adckey` uses ADC scan mode internally. If an `ADC_V2` or `ADC_V3` item uses an internal channel such as VBAT, the component falls back to single-channel polling because regular scan mode does not support internal channels there.
- Key items use `target_mv[]` for threshold judgment. By default idle voltage is treated as `3200mV`, so `target_mv[]` must be configured from large to small. If `CONFIG_ADCKEY_DEFAULT_LOW` is enabled, idle voltage is treated as `0mV`, and `target_mv[]` must be configured from small to large.
- `CONFIG_ADCKEY_SUPPORT_PRESS` enables the `PRESS` event. Without it, key items only report `LONG_PRESS` and `RELEASE`.
- Normal ADC items report sampled millivolt values through their own callback and share the same ADC owner with key items.
- If one item uses an internal ADC channel such as VBAT, set `gpio_pin = ADCKEY_GPIO_UNUSED`. VBAT channels are enabled automatically by the component.
- `items[]` and each key item's `target_mv[]` are referenced directly to keep memory usage small, so they must remain valid while the `adckey` handle is alive.
- When `CONFIG_FREERTOS=y`, the component creates an internal scan task. If `CONFIG_FREERTOS=n`, `main.c` falls back to `adckey_poll()`.

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
