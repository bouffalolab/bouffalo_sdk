OpenOCD 的使用
================
SDK 中的路径
--------------------------------
如果使用 CH347 对 BL616 进行调试，请使用以下命令启动 openocd 作为 GDB server。关于调试器与开发板如何连接的信息，请参阅硬件手册。

.. code-block:: bash

    sudo tools/openocd/openocd_ch347 -f tools/openocd/ch347.cfg -f tools/openocd/target/tgt_e907.cfg

打开 On-Chip Debugger
-----------------------------

.. code-block:: console

    jtag
    Ready for Remote Connections
    Info : CH347 Open Succ.
    Info : clock speed 1000 kHz
    Info : JTAG tap: riscv.cpu tap/device found: 0x10000b6f (mfg: 0x5b7 (T-HEAD Semiconductor Co Ltd), part: 0x0000, ver: 0x1)
    Info : datacount=1 progbufsize=2
    Info : Examined RISC-V core; found 1 harts
    Info :  hart 0: XLEN=32, misa=0x40909125
    Info : starting gdb server for riscv.cpu.0 on 3333
    Info : Listening on port 3333 for gdb connections
    Info : Listening on port 6666 for tcl connections
    Info : Listening on port 4444 for telnet connections

启动 GDB client 并连接到 openocd server
-----------------------------------------------------------

.. code-block:: bash

    riscv64-unknown-elf-gdb -se examples/helloworld/build/build_out/helloworld_bl616.elf

GDB 输出示例
-------------------

.. code-block:: console

    GNU gdb (Xuantie-900 elf newlib gcc Toolchain V2.6.1 B-20220906) 10.0.50.20200724-git
    Copyright (C) 2020 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.
    Type "show copying" and "show warranty" for details.
    This GDB was configured as "--host=x86_64-pc-linux-gnu --target=riscv64-unknown-elf".
    Type "show configuration" for configuration details.
    For bug reporting instructions, please see:
    <https://www.gnu.org/software/gdb/bugs/>.
    Find the GDB manual and other documentation resources online at:
        <http://www.gnu.org/software/gdb/documentation/>.

    For help, type "help".
    Type "apropos word" to search for commands related to "word"...
    Reading symbols from examples/helloworld/build/build_out/helloworld_bl616.elf...
    (gdb) target remote :3333
    Remote debugging using :3333
    0xa0001488 in __udivmoddi4 (rp=0x0, d=<optimized out>, n=<optimized out>) at /mnt/ssd/jenkins_iotsw/slave/workspace/Toolchain/build-gnu-riscv/./source/riscv/riscv-gcc/libgcc/libgcc2.c:1082
    1082    /mnt/ssd/jenkins_iotsw/slave/workspace/Toolchain/build-gnu-riscv/./source/riscv/riscv-gcc/libgcc/libgcc2.c: No such file or directory.
    (gdb) bt
    #0  0xa0001488 in __udivmoddi4 (rp=0x0, d=<optimized out>, n=<optimized out>) at /mnt/ssd/jenkins_iotsw/slave/workspace/Toolchain/build-gnu-riscv/./source/riscv/riscv-gcc/libgcc/libgcc2.c:1082
    #1  __udivdi3 (n=<optimized out>, d=<optimized out>) at /mnt/ssd/jenkins_iotsw/slave/workspace/Toolchain/build-gnu-riscv/./source/riscv/riscv-gcc/libgcc/libgcc2.c:1320
    #2  0x62fc095e in bflb_mtimer_get_time_ms () at ./drivers/lhal/src/bflb_mtimer.c:105
    #3  0x62fc0978 in bflb_mtimer_delay_ms (time=time@entry=1000) at ./drivers/lhal/src/bflb_mtimer.c:121
    #4  0xa00043d2 in main () at ./examples/helloworld/main.c:24

OpenOCD 的 USB 权限
--------------------------
注意 openocd 的 USB 权限。

支持 CH347 的 openocd 仓库
-----------------------------

参考：https://github.com/profound1987/ch347.git