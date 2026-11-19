# touch_cont

Continuously scan self-capacitance keys, print channel data every two seconds, and report touch detection interrupts.

## Support CHIP

|  CHIP  | Remark |
| :-----: | :-----: |
| BL618DG | AP core |

## Hardware

See the [touch GPIO map and calibration notes](../README.md) before connecting the electrodes.

## Compile

```bash
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

Use the SDK's GCC 14.3/Picolibc toolchain. Set `CROSS_COMPILE` to its prefix if it is not the default compiler.

## Flash

```bash
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap COMX=/dev/ttyUSB0
```

## Usage

Release all electrodes during startup. The example waits two seconds, calibrates the idle LTA baseline with a single scan, then starts continuous scanning. Calibration aborts if a scan does not complete within 200 ms.

`[DET]` reports the raw, filtered, LTA, threshold, hysteresis and baseline difference values captured by the interrupt handler. Output is deferred to the main loop; repeated interrupts on one channel may be combined before printing. Filtering is disabled, so the diagnostic difference uses raw data. The default detection threshold is 200 and the hysteresis is 3.

`[DATA]` prints live samples from all enabled key channels every two seconds. `sw` compares LTA minus raw data against the channel's configured threshold; it is a diagnostic estimate, not a hardware key-state report.
