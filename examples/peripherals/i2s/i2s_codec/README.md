# i2s_codec


## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL616/BL618 |        |
| BL618DG     |        |
| BL616CL     |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## Audio controls

The shared BSP audio player exports these device controls:

| Command | Function |
| --- | --- |
| `speaker_vol <0-100> [slot_mask]` | Set speaker playback volume |
| `speaker_mute <0\|1> [slot_mask]` | Unmute/mute speaker playback, retaining its volume |
| `mic_vol <0-100> [slot_mask]` | Set codec input digital volume |
| `mic_gain_db <dB> [slot_mask]` | Set analog microphone gain in physical dB |
| `mic_mute <0\|1> [slot_mask]` | Unmute/mute microphone samples, retaining input gain and capture clocks |
| `audio_info` | Show playback/capture configuration and explicit speaker/microphone mute states |

Speaker volume zero does not change the speaker mute flag. Use `speaker_mute`
for temporary silence; unmuting keeps the selected volume, including zero.
`mic_gain_db` uses the nearest supported analog dB step, not a percentage.
Zero dB is not microphone mute. The optional `slot_mask` is a nonzero decimal
mask: bit N selects physical PCM slot N in that direction. Omitting the mask
selects all declared channels of that role; explicitly passing zero is invalid. Volume and
gain controls reject reference and unknown slots; unsupported independent
controls return an error rather than changing all ADCs. Microphone mute silences
only the selected declared microphone slots while preserving reference slots.

The composite codec maps PCM slots 2/3 (`0x04`/`0x08`) to its ADC helper's local
ADC0/1 bits (`0x01`/`0x02`). These masks belong to different API layers; callers
of the player/PCM controls always use capture slot masks. See the
[BSP audio control contract](../../../../bsp/common/audio_player/README.md) for
multi-microphone examples and the raw capture mute metadata contract.

The pure `i2s_dma` example exercises I2S transport without a codec-control layer;
it does not provide these device gain/mute commands.
