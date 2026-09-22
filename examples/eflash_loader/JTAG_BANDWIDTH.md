# JTAG eFlash Loader bandwidth analysis

This note describes the data path used by the BL618DG JTAG eFlash Loader and
separates the JTAG wire-rate limit from the measured end-to-end Flash rate.
The figures below assume the BL618DG B0 scan chain used by this eFlash loader:

- `riscv0` is NP and `riscv1` is AP.
- The AP DTM reports a 7-bit DMI address (`abits = 7`).
- The Nuclei FTDI configuration uses `nscan1_mode` unless four-wire mode is
  explicitly selected.
- The scan chain contains two TAPs; the non-selected TAP contributes one
  BYPASS bit to every DR scan.

## What is transferred for one word

The host does not send a complete SRAM address for every word. OpenOCD first
loads the initial destination address into the program-buffer register `s0`.
The program buffer then executes the equivalent of:

```text
sw   s1, 0(s0)
addi s0, s0, 4
ebreak
```

For each following 32-bit word, the host writes only the RISC-V Debug Module
`data0` register. The `abstractauto` setting executes the program buffer after
that write, and `s0` increments on the target. The DMI request still contains
the Debug Module register address, the 32-bit data value, and the DMI opcode.

OpenOCD appends a DMI NOP at the end of a batch to collect the result of the
last request. It normally batches 128 DMI scans into one adapter queue, so a
word does not cause a separate USB round trip. It does, however, require its
own DR update because the Debug Module operation is committed at
`Update-DR`.

## Logical JTAG scan cost

For a 32-bit DMI access:

```text
DMI payload  = 32 data + 7 address + 2 opcode = 41 bits
other TAP    = 1 BYPASS bit
TAP movement = approximately 5 clocks
------------------------------------------------
one word     = approximately 47 logical JTAG clocks
```

The five movement clocks are the transitions from Run-Test/Idle to
Shift-DR, and from the last shifted bit through Exit1-DR and Update-DR back to
Run-Test/Idle. A target-specific DMI/abstract-command idle delay must be added
to 47 when the Debug Module needs extra time. The final NOP adds about one
more scan per 128 words.

The ideal four-wire JTAG limit at a 15 MHz TCK, ignoring target execution,
adapter gaps, and idle delays, is therefore:

```text
useful efficiency = 32 / 47                 = 68.1%
useful rate       = 15e6 * 32 / 47 / 8      = 1.22 MiB/s
```

This is a protocol upper bound, not an expected Flash write rate.

## NScan1/OScan1 cost

`interface/nuclei_ftdi.cfg` enables Nuclei NScan1. This is the two-wire
cJTAG/OScan1 encoding, where TDI, TMS, and TDO share TMSC. OpenOCD's FTDI
driver expands one logical JTAG clock into three physical TCKC clocks.

Consequently, a configured 15 MHz adapter clock provides approximately 5 MHz
of logical JTAG clocking:

```text
logical rate      = 15 MHz / 3             = 5 MHz
ideal NScan1 rate = 5e6 * 32 / 47 / 8      = 416 KiB/s
```

Including the batch NOP changes this only slightly, to roughly 412 KiB/s.
The absolute physical pin-rate ceiling before DMI overhead is about
`15e6 / 3 / 8 = 610 KiB/s`.

The NScan1 number is still optimistic. The current FTDI driver emits several
short MPSSE commands for each OScan1 bit, and the adapter/USB command stream
is not a perfectly continuous clock. DMI busy/idle delays, batch boundaries,
halt/resume operations, and FlashLoader command acknowledgements reduce the
measured rate further.

## Measured BL618DG results

The following measurements used a 1 MiB incompressible/random payload at
Flash address `0x300000` and the optimized 20 KiB mailbox packets.

| Path | Flash-load phase | Tool total |
| --- | ---: | ---: |
| OpenOCD, 1 MHz NScan1, before optimization | 111.445 s | 126.698 s |
| OpenOCD, 15 MHz NScan1, optimized | 13.923 s | 20.321 s |
| OpenOCD, 15 MHz four-wire JTAG | 9.502 s | 14.405 s |
| UART, 2 Mbps | 8.336 s | 11.595 s |

An isolated OpenOCD RAM-image load of 183072 bytes at 15 MHz NScan1 took
1.656 s, or approximately 108 KiB/s. This is about 26% of the ideal NScan1
DMI rate. The remaining Flash-load time is target-side Flash programming and
the per-packet command/response synchronization.

The optimized OpenOCD path is therefore about 6.31 times faster than the
original 1 MHz result, but remains about 1.73 times slower than the UART
baseline for this random payload. Raising the adapter from 15 MHz to 20 MHz
did not improve the result; 30 MHz was unreliable on the tested adapter.

## Four-wire results

With the same 15 MHz adapter setting and a working four-wire connection,
NScan1's 3x encoding disappears. The ideal DMI RAM-transfer limit rises from
about 416 KiB/s to about 1.22 MiB/s. The measured isolated RAM transfer was:

| Adapter clock | RAM transfer |
| --- | ---: |
| 8 MHz | 384 KiB/s |
| 15 MHz | 587 KiB/s |
| 20 MHz | 594 KiB/s |
| 30 MHz | 809 KiB/s before a subsequent DMI signal error |

The 15 MHz result loaded 183840 bytes in 0.306 s. Compared with the historical
15 MHz NScan1 measurement of about 108 KiB/s, four-wire JTAG improved the
isolated RAM rate by approximately 5.4 times. The 20 MHz gain was only about
1.3%, while 30 MHz subsequently returned an invalid Debug Module version and
must not be treated as stable. The validated setting remains 15 MHz.

For the complete 1 MiB random-data test, four-wire JTAG reduced the Flash-load
phase from 13.923 s to 9.502 s (31.8%) and the tool total from 20.321 s to
14.405 s (29.1%). Device-side XIP SHA256 verification passed. The smaller
end-to-end improvement is expected: approximately 3.88 s of range erase and
most of the roughly 8 s target-side Flash programming time do not depend on
the JTAG wire mode. Four-wire JTAG primarily removes host-to-SRAM transfer
time.

The tested Flash region was restored after the benchmark. The restored
1 MiB device-side SHA256 was:

```text
f5fb04aa5b882706b9309e885f19477261336ef76a150c3b4d3489dfac3953ec
```

Select two-wire cjtag mode for a direct OpenOCD invocation by setting the
variable before the interface file is sourced:

```text
openocd -c "set NUCLEI_NSCAN1 1" \
  -f interface/nuclei_ftdi.cfg -f target/bl618dg.cfg
```

`JTAG_SEL` selects the physical pinout: high for four-wire JTAG and low for
cjtag. Nuclei OpenOCD drives it from the selected scan mode, so a scan mode that
does not match the wired pinout makes scan-chain interrogation return all ones
on the tested adapter.
