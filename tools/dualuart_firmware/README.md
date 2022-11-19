# Bouffalo DualUart

This is the instructions for the dualuart firmware used with BL702 or BL616.

## Hardware

## Sipeed RV-Debugger-Plus

- Use BL702C-A0

![RV-Debugger-Plus](rv-debugger.png)

## Flash download

- Open BL Devcube
- Select IOT UI and select single download
- Load firmware
- Press and hold the boot pin then plug the usb in the computer to go to the boot mode.
- Download

![Flash download](flash_download.png)

## UART PIN

**DTR** and **RTS** are only used for downloading with devcube.

|   Function    |  Label   |  GPIO  |
|:-------------:|:--------:|:------:|
|UART0 TX       | TCK   | GPIO15  |
|UART0 RX       | TDI   | GPIO0  |
|UART0 DTR      | TMS   | GPIO2  |
|UART0 RTS      | TDO   | GPIO1  |
|UART1 TX       | TX   | GPIO26  |
|UART1 RX       | RX   | GPIO27  |
|UART1 DTR      | DTR   | GPIO28  |
|UART1 RTS      | RTS   | GPIO24  |
