# BL618DG Touch V2 examples

| Example                     | Mode                              | Output                                          |
| --------------------------- | --------------------------------- | ----------------------------------------------- |
| [touch_int](touch_int)       | Self-capacitance, continuous scan | Detection interrupts                            |
| [touch_cont](touch_cont)     | Self-capacitance, continuous scan | Channel data every 2 s and detection interrupts |
| [touch_mutual](touch_mutual) | Mutual-capacitance, 4 TX x 4 RX   | Detection interrupts                            |

These examples target the BL618DG AP core and share `touch_common.c` and `touch_common.h`. They use the SDK device table entry `BFLB_NAME_TOUCH_V2` and the standard board initialization, including flash initialization.

## GPIO map

| Signal | GPIO | Self-capacitance   | Mutual-capacitance |
| ------ | ---- | ------------------ | ------------------ |
| C_EXT  | 13   | External capacitor | External capacitor |
| TOUCH0 | 6    | Key                | TX0                |
| TOUCH1 | 7    | Key                | TX1                |
| TOUCH2 | 14   | Key                | TX2                |
| TOUCH3 | 15   | Key                | TX3                |
| TOUCH4 | 16   | Active shield      | RX0                |
| TOUCH5 | 17   | Key                | RX1                |
| TOUCH6 | 18   | Key                | RX2                |
| TOUCH7 | 19   | Key                | RX3                |

Configure the external capacitor and electrodes for the actual board. TOUCH5/TOUCH6 require QFN56/QFN68 and TOUCH7 requires QFN68, so check which pins are bonded out on the target package.

## Calibration and detection

Release all electrodes for the two-second startup delay. A single calibration scan establishes the idle LTA baseline; normal scanning then runs continuously with channel calibration disabled. The scan timeout is 200 ms.

The interrupt handler captures the detected channels' data and acknowledges the interrupt. The main loop prints the captured data, so UART output does not delay interrupt handling. Repeated interrupts on one channel before the main loop consumes it are combined, retaining that channel's latest sample; these logs do not count individual key presses.

Filtering is disabled. The diagnostic `diff` and `sw` fields compare the LTA against raw data using the channel's configured threshold. `[DET]` identifies an interrupt snapshot; `[DATA]` contains live samples and is not a hardware key-state report.

The defaults in `touch_common.h` are a detection threshold of 200 and a hysteresis of 3. Adjust them for the electrodes and board. Self-capacitance uses CH0~CH3 and CH5~CH7, with CH4 as an active shield. Mutual-capacitance uses CH0~CH3 as TX and CH4~CH7 as RX, with 16 logical detection channels.
