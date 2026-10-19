# WAMR integration

The `upstream/` sources are from
[WebAssembly Micro Runtime (WAMR)](https://github.com/wasm-micro-runtime/wasm-micro-runtime),
tag [`WAMR-2.4.4`](https://github.com/wasm-micro-runtime/wasm-micro-runtime/tree/WAMR-2.4.4)
(commit `8c18e3f68b16c4bcaf05996b2636f6ed2b4cf629`), licensed under
[Apache-2.0 WITH LLVM-exception](upstream/LICENSE).
The build includes only the core interpreter,
AOT loader, built-in libc, allocator, and shared utilities. The `platform/`
directory adapts WAMR to the Bouffalo SDK's FreeRTOS, heap and cache APIs.

`CONFIG_WAMR_INTERP` and `CONFIG_WAMR_AOT` can be enabled separately. AOT
currently targets the BL616 ILP32F ABI only and needs executable SRAM. It does
not support loading AOT code into non-executable PSRAM. No JIT or WASI APIs
are enabled. The interpreter can run architecture-independent `.wasm` modules.
