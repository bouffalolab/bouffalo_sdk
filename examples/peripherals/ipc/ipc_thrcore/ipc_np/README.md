# ipc_ap_lp

## Overview

This example is used on tri-core platforms such as the BL616D to demonstrate the IPC (Inter-Core Communication) mechanism between NP and AP and LP.
Note: This project must be used in conjunction with the ipc_ap_lp project to program to the three cores of AP/LP/NP respectively for full triple core communication and interrupt response.

## How to use example

1. Separate compilation and burning of ipc_ap_lp items to AP/LP cores, and ipc_np projects to NP cores:
    AP：make flash CHIP=bl616d COMX=COMxx CPU_ID=ap
    LP：make flash CHIP=bl616d COMX=COMxx CPU_ID=ap
    NP: make flash CHIP=bl616d COMX=COMxx CPU_ID=np

2. Shell command trigger IPC:
AP: ap_trigger_lp <value> This value ranges from 65536-4294901760.
    ap_trigger_np <value> This value ranges from 65536-4294901760.
LP: lp_trigger_ap <value> This value ranges from 65536-4294901760.
    lp_trigger_np <value> This value ranges from 0-65535.
NP: np_trigger_ap <value> This vlaue ranges from 0-65535.
    np_trigger_lp <value> This vlaue ranges from 0-65535.

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