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
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np
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

Device controls use explicit direction names: `speaker_vol <0-100> [slot_mask]`,
`speaker_mute <0|1> [slot_mask]`, `mic_vol <0-100> [slot_mask]`, `mic_gain_db <dB> [slot_mask]`, and
`mic_mute <0|1> [slot_mask]`. `mic_gain_db` selects physical analog gain in dB; its minimum is not mute.
The optional decimal channel mask must be nonzero; omitting it selects the
whole configured role. Digital microphone modes do not expose analog gain. Playback volume, playback mute,
and microphone mute are independent. See the shared
[I2S audio controls](../../i2s/i2s_codec/README.md#audio-controls) for details.
