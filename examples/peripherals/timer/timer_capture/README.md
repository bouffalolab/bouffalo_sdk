# timer_capture

## Overview

The timer capture feature allows the system to measure the duration of external pulse signals. By capturing the timestamps of rising and falling edges of these pulses, the application can determine the pulse width and frequency.

## How to use example

### Hardware Connection

The following GPIO pins are used in this example. You can modify these pin assignments based on your hardware requirements:

|    Macro               |  GPIO Pin   | Function                           |
|------------------------|-------------|------------------------------------|
| PWM_OUTPUT_PIN         | GPIO_PIN_0  | PWM Output Pin                     |
| TIMER_CAPTURE_PIN      | GPIO_PIN_1  | Timer Capture Pin                  |
| GPIO_PULSE_CREATE_PIN  | GPIO_PIN_3  | Generate the Test Pulse (If you already have the pulse source, this is not needed.) |
| GPIO_FOR_INT_PIN       | GPIO_PIN_20 | To Generate Interrupt              |
| TEST_FOR_INT_DELAY_PIN | GPIO_PIN_27 | GPIO Toggle when Interrupt Trigger (For test interrupt delay time)  |

Ensure that `GPIO_PIN_3` is connected to both `GPIO_PIN_1` (the timer capture pin) and `GPIO_PIN_20` (the pulse interrupt pin).

Make sure to connect your hardware accordingly to ensure proper functionality.

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |
|BL616D/BL618D     |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL616D/BL618D

```
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## Example Output

**Set Pulse Width**: Use the serial command `set_pulse_width <high-level time (us)>` to set the duration of the pulse on `GPIO_PIN_3`. For example, sending `set_pulse_width 800` will set the pulse width to 800 microseconds.
The log will look like this:
   ```
   bflb />set_pulse_width 800
   Make a pulse
   Pulse delay 800us
   bflb />set_pulse_width 1200
   Make a pulse
   Pulse delay 1200us
   ```

The case will calculate the pulse width on the falling edge. The default duty cycle is 25%. When the pulse width is ≥ 500 and < 1000, the PWM duty cycle output on `GPIO_PIN_0` will be adjusted to 50%. The modification method is as follows. An oscilloscope can be used to measure the accuracy of the waveform.

|    Pulse Width (us)    | PWM duty cycle   |
|------------------------|------------------|
|       < 500            |       25%        |
|   ≥ 500 && < 1000      |       50%        |
|  ≥ 1000 && < 1500      |       75%        |
|       ≥ 1500           |       100%       |

**Dump Event Information**: To view all recorded event information, send the command `dump_event_info` via the serial interface. This will print out the details of all captured events, including their types and timestamps, in the following format:
   ```
   bflb />dump_event_info
   Dump event info
   Total capture events: 4
   Latest 4 event info:
   Event 00:  Rising, Timestamp: 0x1722573d, Counter: 0x17225c9e
   Event 01: Falling, Timestamp: 0x1722d4c6, Counter: 0x1722d53f
   Event 02:  Rising, Timestamp: 0x90957f82, Counter: 0x90957fb0
   Event 03: Falling, Timestamp: 0x90963b17, Counter: 0x90963b91
   ```

According to the above log, the pulse width can be calculated using the formula: (Falling Edge Timestamp - Rising Edge Timestamp) * 1000 * 1000 / 40M. For example, for the latest set of data, using the values (0x90963b17 - 0x90957f82) results in a pulse width of approximately 1200us.