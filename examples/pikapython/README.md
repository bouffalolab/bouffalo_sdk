# PikaPython on BL618

PikaPython is an ultra-lightweight Python engine that can run with only 4KB of RAM, zero dependencies, and easy to bind with C. It is similar to MicroPython and JerryScript, but much smaller and simpler.

BL618 is a RISC-V based chip that supports PikaPython as an example program.

This README will guide you through the steps of installing, using, and exploring the features and advantages of PikaPython on BL618.

## How to build

on windows:

``` bash
make_bl618.cmd
```

## How to flash

``` bash
make flash COMX=<your com port>
```

## How to Connect

The REPL is avalible on the USB virtual serial port. You can use any serial terminal program to connect to it. The default baud rate is 115200.

After connecting, you can enter the REPL mode by pressing the Enter key.

``` python
~~~/ POWERED BY \\~~~
~  pikascript.com ~
~~~~~~~~~~~~~~~~~~~~~
hello PikaPython!
>>> print('hello BL618!')
hello BL618!
>>> 
```