# Standard RISC-V toolchain

The Bouffalo SDK builds every supported chip with the standard bare-metal
`riscv64-unknown-elf` GCC 14.3 and its Picolibc. This is the only supported
toolchain. The T-Head (Xuantie) GCC and the Zephyr SDK are not supported; the
Zephyr SDK uses a single-threaded POSIX model and its Picolibc does not
provide the C++ `std::thread` support required by this SDK. The `xtheade`,
`psfoperand`, `xtheadse` and `-mtune=e907` extensions are not used by the SDK
build.

## Default toolchain

Use the Bouffalo `riscv64-unknown-elf` GCC 14.3 toolchain (the one built with
Picolibc as its default C library) and add its `bin` directory to `PATH`.
`board.build` defaults `CROSS_COMPILE` to `riscv64-unknown-elf-` for every
chip:

```sh
export PATH="/path/to/riscv64-unknown-elf/bin:$PATH"
make -C examples/helloworld CHIP=bl602 BOARD=bl602dk BUILD_DIR=build-bl602
make -C examples/helloworld CHIP=bl702 BOARD=bl702dk BUILD_DIR=build-bl702
make -C examples/helloworld CHIP=bl616 BOARD=bl616dk BUILD_DIR=build-bl616
```

The SDK is built around this specific toolchain; no other toolchain is
supported. `cmake/toolchain.cmake` hardcodes the ISA/ABI pairs below; there is
no configure-time multilib probing.

## ISA and ABI selection

The multilib directory names use the Zephyr-style spelling: the directory may
drop the `c` extension (`rv32imafd` for `rv32imafdc`) or the `a` extension
(`rv32imfc` for `rv32imafc`). The SDK sets `BFLB_RISCV_ARCH_FLAGS` and
`BFLB_RISCV_ARCH_FLAGS_NOFPU`; all SoC and standalone wireless builds consume
those variables.

| Chip / core | ISA | ABI | Selected multilib |
| --- | --- | --- | --- |
| BL602/BL604 | `rv32imafc_zicsr_zifencei` | `ilp32f` | `rv32imafc_zicsr_zifencei/ilp32f` |
| BL702/BL704/BL706 | `rv32imafc_zicsr_zifencei` | `ilp32f` | `rv32imafc_zicsr_zifencei/ilp32f` |
| BL702L | `rv32imafc_zicsr_zifencei` | `ilp32f` | `rv32imafc_zicsr_zifencei/ilp32f` |
| BL616/BL616CL | `rv32imafc_zicsr_zifencei` | `ilp32f` | `rv32imafc_zicsr_zifencei/ilp32f` |
| BL618DG b0 AP/NP | `rv32imafdc_zicsr_zifencei` | `ilp32d` | `rv32imafd_zicsr_zifencei/ilp32d` |
| BL618DG b0 LP | `rv32emc_zicsr_zifencei` | `ilp32e` | `rv32emc_zicsr_zifencei/ilp32e` |
| BL618DG a0 AP/NP | `rv32imafc_zicsr_zifencei` | `ilp32f` | `rv32imafc_zicsr_zifencei/ilp32f` |
| BL618DG a0 LP | `rv32emc_zicsr_zifencei` | `ilp32e` | `rv32emc_zicsr_zifencei/ilp32e` |

The `_zicsr_zifencei` suffix is part of the flags. The same `-march` /
`-mabi` pair is used at both compile and link time.

## Picolibc and TLS

Picolibc is mandatory and is the default C library of the supported
Bouffalo `riscv64-unknown-elf` toolchain. The SDK enables
`BFLB_USE_PICOLIBC` unconditionally, so the Picolibc compatibility headers and
TLS errno integration are always active. There is no Newlib fallback and the
SDK does not add `--specs=picolibc.specs`; the toolchain is expected to
provide Picolibc as its default libc.

Picolibc is supported only with TLS errno. The compiler must provide a
Picolibc built with `__THREAD_LOCAL_STORAGE`/`__THREAD_LOCAL_STORAGE_API`.
FreeRTOS enables `configUSE_PICOLIBC_TLS`, reserves a TLS block from every
task stack, calls `_init_tls()` when a task is created, and updates `tp` with
`_set_tls()` on task switches.

The startup code initializes a separate boot TLS block after `start_load()`
has made RAM-backed code available and before `System_Post_Init()`/`main()`,
so libc calls made before the scheduler starts also have a valid `tp`. The
SDK's `__errno()` returns `&errno`, so lwIP, Bluetooth, OpenThread and other
components that use `#define errno (*__errno())` share the same per-task errno
as the Picolibc library.

Every SDK linker script used with Picolibc provides the TLS template and boot
block: `__tdata_start`, `__tdata_source`, `__tdata_size`, `__tbss_start`,
`__tbss_offset`, `__tbss_size`, `__tls_size`, `__tls_align` and
`__boot_tls_start`. This includes the BL602/BL702/BL702L flash and RAM scripts
and the BL618DG RAM scripts. Custom scripts must provide the same symbols or
use a non-Picolibc toolchain.

## Vendor libraries

Precompiled SDK libraries and ROM code can still contain E907/E902
instructions and retain the ABI they were built with. For BL618DG b0 the
precompiled `libbl618dg_phyrf_b0.a` and `libbl618dg_phyrf_lpfw_b0.a` are
double-float (`ilp32d`) objects, so the b0 application and LP firmware must
use `ilp32d` as well. The BL618DG b0 LP core itself uses `ilp32e`; the
`_lpfw_b0` archive is only selected for the AP-side low-power firmware path.

## Verification

Build a basic image for every chip/core and representative wireless/RTOS
images:

```sh
make -C examples/helloworld CHIP=bl602 BOARD=bl602dk
make -C examples/helloworld CHIP=bl702 BOARD=bl702dk
make -C examples/helloworld CHIP=bl702l BOARD=bl702ldk
make -C examples/helloworld CHIP=bl616 BOARD=bl616dk
make -C examples/helloworld CHIP=bl616cl BOARD=bl616cldk
make -C examples/helloworld CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0
make -C examples/freertos CHIP=bl602 BOARD=bl602dk
make -C examples/freertos CHIP=bl702 BOARD=bl702dk
make -C examples/wifi/wfa_dualcore/wfa_np CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np
make -C examples/pmu/bl618dg/mini_sys CHIP=bl618dg BOARD=bl618dgdk CPU_ID=lp
make -C examples/pmu/lp_fw/bl618dg CHIP=bl618dg BOARD=bl618dg_lpfw_board CPU_ID=ap CPU_MODEL=b0 MAKE_LPFW=y
```

The toolchain tests at `tests/toolchain/test_e907.py` verify instruction
encodings and the TLS layout. They check for a non-empty `PT_TLS` segment,
`__tdata_*`/`__tbss_*`/`__tls_*` symbols, the boot TLS block, and a
`tp`-relative `__errno()` accessor.

```sh
python3 tests/toolchain/test_e907.py
```

Build and ELF checks do not replace on-device testing of interrupts, cache
coherence, power recovery, or radio operation.
