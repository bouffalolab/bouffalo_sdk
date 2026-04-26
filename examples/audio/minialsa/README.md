# minialsa demo

## Supported CHIPs

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616C/BL618M       |        |

## Software Adaptation

Please refer to the codec demo

## Compilation

- For BL616/BL618

```
make
```

## Flashing
make flash CHIP=bl616 COMX=/dev/ttyACM0

## Running

### 1. Play Local File System Audio

```
bouffalolab />minialsa_play
```

The self-test covers:

- playback path: `msp_pcm_hw_params_*`, `msp_pcm_sw_params_*`, `msp_pcm_writei`, `msp_pcm_pause`, `msp_pcm_recover`, `msp_pcm_drain`
- capture path: `msp_pcm_avail`, `msp_pcm_wait`, `msp_pcm_readi`, `msp_pcm_readn`
- helper path: `msp_pcm_set_params`, `msp_pcm_drop`, `msp_pcm_bytes_to_frames`, `msp_pcm_frames_to_bytes`

```
bouffalolab />minialsa_api_test
```
