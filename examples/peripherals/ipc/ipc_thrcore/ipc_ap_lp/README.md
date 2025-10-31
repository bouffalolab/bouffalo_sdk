# ipc_ap_lp

## Overview

This example demonstrates the inter-core (IPC) mechanism between the three cores (AP/LP/NP) of the BL616D, including IPC channel initialization, interrupt enablement, cross-core interrupt triggering and response, and shell command interaction.

## How to use example

1. Compile the AP and LP (NP engineering if NP is required) firmware separately and burn them to the corresponding core:
    AP：make flash CHIP=bl616d COMX=COMxx CPU_ID=ap
    LP：make flash CHIP=bl616d COMX=COMxx CPU_ID=ap
When you want to compile the AP, you need to comment out 15-16 lines of the code first, when you want to compile the LP, you need to compile the LP first, 15-16 lines do not need to comment, then compile the AP, and then download.If you want to do a triple core test, then you need to compile ipc_np download examples.


2. Shell command trigger IPC:
AP: ap_trigger_lp <value> This value ranges from 65536-4294901760.
    ap_trigger_np <value> This value ranges from 65536-4294901760.
LP: lp_trigger_ap <value> This value ranges from 0-65535.
    lp_trigger_np <value> This value ranges from 0-65535.

3. 

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616D            |        |

## Compile

- BL616D

```
make CHIP=bl616d BOARD=bl616ddk CPU_ID=xx #xx can be ap np lp
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## Example Output

Enter the following command in your serial port: ap_trigger_lp 65536

The log will look like this:
» ap_trigger_lp 65536
« ap_trigger_lp 65536
Trigger AP->LP interrupt

and look at your LP serial output, the log will look like this:
bouffalolab />
[I][MAIN] AP->LP interrupt trigger!
[I][MAIN] Status=0x00000001