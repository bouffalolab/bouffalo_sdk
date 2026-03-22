# audio_dac_adc

## Overview

This example demonstrates a fully builtin audio path:

- Capture from the builtin BL AUADC path
- Playback through the builtin BL AUDAC/AUPWM path

The recorder shell commands are reused from `examples/peripherals/i2s/i2s_codec`.

## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL616/BL618 |        |
| BL618DG     |        |
| BL616CL     |        |

## Compile

- BL616/BL618

```bash
make CHIP=bl616 BOARD=bl616dk
```

- BL618DG

```bash
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

- BL616CL

```bash
make CHIP=bl616cl BOARD=bl616cldk
```

## Flash

```bash
make flash CHIP=chip_name COMX=xxx
```

## Shell

After boot, you can use the existing recorder and player shell commands, for example:

```text
audio_info
rec_start
rec_stop
rec_play
```
