# USB Console (usb_log)

Replaces the UART console with a USB CDC virtual COM port. All `printf` / log output goes to USB, and shell commands can be typed over the same port.

## Enable

Set the following in the project `defconfig`:

```
CONFIG_BSP_CONSOLE_USB_CDC=y
```

It also requires (checked at configure time):

```
CONFIG_FREERTOS=y
CONFIG_SHELL=y
CONFIG_CHERRYUSB=y
CONFIG_CHERRYUSB_DEVICE=y
CONFIG_CHERRYUSB_DEVICE_CDC_ACM=y
CONFIG_CHERRYUSB_OSAL=freertos
```

Example:

```
make CHIP=bl616 BOARD=bl616dk CONFIG_BSP_CONSOLE_USB_CDC=y
```

Flash:

```
make flash CHIP=bl616 COMX=<serial-port>
```

## When it takes effect

- When `CONFIG_BSP_CONSOLE_USB_CDC=y` is enabled.
- `usb_console_init()` is called automatically from `board_init()`.
- Once the host enumerates the CDC port, log output is routed to USB instead of UART.

## Features

- All console output (`printf`, `puts`, log) is sent over USB CDC.
- USB RX is forwarded to the shell (`shell_exe_cmd`), so the CDC port works as a shell terminal.
- UART console is disabled when USB console is enabled.

## API

```c
void usb_console_init(void);            // start the USB console (called by board_init)
ssize_t usb_console_write(const void *data, size_t size); // low-level write
```

## Files

| File             | Description                         |
|------------------|-------------------------------------|
| `usb_console.c`  | USB CDC console implementation      |
| `usb_console.h`  | Public API                          |
| `usb_config.h`   | CherryUSB config for the console    |
